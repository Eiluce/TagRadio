#include "l2cap_server.h"
#include "trace.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/errno.h>
#include <bluetooth/l2cap.h>
#include <sys/unistd.h>
#include <pthread.h>

// ENVOYER MESSAGE AU CLIENT POUR DONNER RAISON FERMETURE CONNEXION (TIMEOUT ETC...)


struct routine_data_t {
	int16_t timeout;
	uint8_t num_client;
	uint16_t max_req;
	l2cap_server_t server; 
};
	
//------------------------------------------------------------------------------------

static void treat_buffer_default_func(l2cap_server_t server, uint8_t num_client) {
	fprintf(stderr, "Buffer trace : %s\n", server.clients[num_client].buffer);
	return;
}

static void send_response_default_func(l2cap_server_t server, uint8_t num_client, uint8_t res_type) {
	char response[] = "Response echo.";
	write(server.clients[num_client].conn_id, response, sizeof(response));
	return;
}

//------------------------------------------------------------------------------------

int8_t l2cap_server_create(l2cap_server_t *server, bdaddr_t *adapter, uint16_t port, 
			   uint8_t max_clients, uint16_t buffer_length, 
			   void (*treat_buffer_func)(l2cap_server_t server, uint8_t num_client),
			   void (*send_response_func)(l2cap_server_t server, uint8_t num_client, uint8_t res_type)) {
	int8_t opened_sockets = 0;
	

	if (!server) {
		print_trace(TRACE_ERROR, "l2cap_server_create : invalid server reference.\n");
		return -1;
	}

	if (!adapter) {
		print_trace(TRACE_ERROR, "l2cap_server_create : invalid BT adapter reference.\n");
		return -1;
	}

	if (!buffer_length) {
		print_trace(TRACE_WARNING, "l2cap_server_create : null buffer, creation aborted.\n");
		return -1;
	}

	server->launched = 0;
	server->buffer_length = buffer_length;
	server->max_clients = max_clients;
	server->clients = calloc(max_clients, sizeof(client_t));
	for (uint8_t i = 0; i < max_clients; i++) {
		server->clients[i].buffer = calloc(buffer_length, sizeof(char));
		memset(server->clients[i].buffer, 0, sizeof(server->clients[i].buffer));
		/* On ne peut pas "bind" deux sockets sur le même couple (@, port) !*/
		server->clients[i].server_sock = open_l2cap_socket(adapter, port+(i*2), 1);
		if (server->clients[i].server_sock.sock >= 0) {
			opened_sockets++;
		}
	}
	
	if (!treat_buffer_func) {
		server->treat_buffer = &(treat_buffer_default_func);
	} else {
		server->treat_buffer = treat_buffer_func;
	}
	if (!send_response_func) {
		server->send_response = &(send_response_default_func);
	} else {
		server->send_response = send_response_func;
	}

	return opened_sockets;
}

//------------------------------------------------------------------------------------

static void *socket_thread_routine(void *data) {
	struct routine_data_t *routine_data = (struct routine_data_t *)data;
	socklen_t sockaddr_len = sizeof(struct sockaddr_l2);

	uint8_t i = routine_data->num_client;
	int8_t sock = routine_data->server.clients[i].server_sock.sock;
	int16_t timeout = routine_data->timeout;
	char thread_launched = 0;
	char debug[100] = {0};
	char end_info[100] = {0};

	if (sock >= 0) {
		listen(sock, 1);
		sprintf(debug, "Connexion en attente sur la socket %i...\n", sock);
		print_trace(TRACE_INFO, debug);

		/* Maintenant que la socket est en attente de connexion,
		   on va vérifier l'arrivée de connexions éventuelles et les accepter.
		*/
		struct pollfd p;
		int8_t n = 0;
		p.fd = sock;
		p.events = POLLIN;
		while ((n = poll(&p, 1, timeout)) < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				continue;
			perror("server_routine_accept : error while polling socket");
			goto end;
		}
		
		if (!n) {
			errno = ETIMEDOUT;
			perror("server_routine_accept : error while polling socket");
			goto end;
		}
		routine_data->server.clients[i].conn_id = accept(sock,
								 (struct sockaddr *)&(routine_data->server.clients[i].rem_addr),
								 &sockaddr_len);
		memset(debug, 0, strlen(debug));
		sprintf(debug, "Connexion établie sur la socket %i, id de connexion : %i...\n", sock, routine_data->server.clients[i].conn_id);
		print_trace(TRACE_INFO, debug);
	} else {
		print_trace(TRACE_ERROR, "server_launch_socket_routine : invalid socket.\n");
		return NULL;
	}
	thread_launched = 1;
	uint16_t num_req = 0;

	while (thread_launched && (num_req < routine_data->max_req) ) {
		memset(routine_data->server.clients[i].buffer, 0, sizeof(routine_data->server.clients[i].buffer));
		int8_t bytes_read = 0;
		int8_t n = 0;
		struct pollfd p;
		p.fd = routine_data->server.clients[i].conn_id;
		p.events = POLLIN;
		while ((n = poll(&p, 1, timeout)) < 0) {
			if (errno == EAGAIN || errno == EINTR) {
				continue;
			}
			perror("server_routine : error while polling socket");
			goto end;
		}
		
		if (!n) {
			errno = ETIMEDOUT;
			perror("server_routine : error while polling socket");
			goto end;
		}
		while ((bytes_read = read(routine_data->server.clients[i].conn_id,
					 routine_data->server.clients[i].buffer,
					 routine_data->server.buffer_length)) < 0) {
			if (errno == EAGAIN || errno == EINTR)
				continue;
			perror("server_routine : error while reading socket.\n");
			goto end;
		}
		if (bytes_read == 0) { // 0 Bytes read means that the connection has been lost.
			print_trace(TRACE_WARNING, "l2cap_server : connection reset by peer.\n");
			goto end;
		}
		num_req++;
		if (strcmp(routine_data->server.clients[i].buffer, L2CAP_SERVER_UNIVERSAL_STOP) == 0) {
			char closeACK[10] = "STOP_ACK";
			write(routine_data->server.clients[i].conn_id, closeACK, strlen(closeACK));
			thread_launched = 0;
		}
		routine_data->server.treat_buffer(routine_data->server, i);		
	}

 end:
	sprintf(end_info, "l2cap_server : connection %i ended.\n", routine_data->server.clients[i].conn_id);
	print_trace(TRACE_INFO, end_info);
	close(routine_data->server.clients[i].conn_id);

	return NULL;
}

//------------------------------------------------------------------------------------

int8_t l2cap_server_launch(l2cap_server_t *server, int16_t timeout, uint16_t max_req) {
	if (server == NULL) {
		print_trace(TRACE_ERROR, "l2cap_server_launch : invalid server reference.\n");
		return -1;
	}

	if (server->max_clients == 0) {
		print_trace(TRACE_ERROR, "l2cap_server_launch : no socket available to run the server.\n");
		return -1;
	}

	pthread_t server_threads[server->max_clients];
	struct routine_data_t routine_data[server->max_clients];
	for (uint8_t i = 0; i < server->max_clients; i++) {
		routine_data[i].server = *server;
		routine_data[i].timeout = timeout;
		routine_data[i].max_req = max_req;
		routine_data[i].num_client = i;
	}
	for (uint8_t i = 0; i < server->max_clients; i++) {
		pthread_create(&(server_threads[i]), NULL, &(socket_thread_routine), (void *)&routine_data[i]);
	}

	for (uint8_t i = 0; i < server->max_clients; i++) {
		pthread_join(server_threads[i], NULL);
	}
	
	return 0;
}

//------------------------------------------------------------------------------------

void l2cap_server_close(l2cap_server_t * server) {
	for (uint8_t i = 0; i < server->max_clients; i++) {
		free(server->clients[i].buffer);
		if (server->clients[i].server_sock.sock >= 0) {
			close(server->clients[i].server_sock.sock);
		}
	}
	free(server->clients);
}
