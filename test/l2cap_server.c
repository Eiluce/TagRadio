#include "l2cap_server.h"
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
	uint16_t timeout;
	uint8_t num_client;
	uint16_t max_req;
	l2cap_server_t server; // A METTRE EN MUTEX !!!!!!!!!!!!! POINTEUR ??????????? (MODIF ADD REMOTE CLIENTS)
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
		fprintf(stderr, "l2cap_server_create error : invalid server reference.\n");
		return -1;
	}

	if (!buffer_length) {
		fprintf(stderr, "l2cap_server_create warning : null buffer, creation aborted.\n");
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
	uint16_t timeout = routine_data->timeout;
	char thread_launched = 0;

	if (sock >= 0) {
		listen(sock, 1);
		fprintf(stderr, "Connexion en attente sur la socket %i...\n", sock);
		// TIMEOUT SUR ACCEPT ?????????????????????
		routine_data->server.clients[i].conn_id = accept(sock,
								 (struct sockaddr *)&(routine_data->server.clients[i].rem_addr),
								 &sockaddr_len);
		fprintf(stderr, "Connexion établie sur la socket %i, id de connexion : %i...\n", sock, routine_data->server.clients[i].conn_id);
	} else {
		fprintf(stderr, "server_launch_socket_routine error : invalid socket.\n");
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
					 sizeof(routine_data->server.clients[i].buffer))) < 0) {
			if (errno == EAGAIN || errno == EINTR)
				continue;
			perror("check_cmd_complete : error while reading socket.\n");
			goto end;
		}
		if (bytes_read == 0) {
			fprintf(stderr, "l2cap_server warning : nothing to read on the socket.\n");
			continue;
		}
		num_req++;
		if (strcmp(routine_data->server.clients[i].buffer, "STOP") == 0) {
			thread_launched = 0;
		}
		routine_data->server.treat_buffer(routine_data->server, i);		
	}

 end:
	close(routine_data->server.clients[i].conn_id);

	return NULL;
}

//------------------------------------------------------------------------------------

int8_t l2cap_server_launch(l2cap_server_t *server, uint16_t timeout, uint16_t max_req) {
	if (server == NULL) {
		fprintf(stderr, "l2cap_server_launch error : invalid server reference.\n");
		return -1;
	}

	if (server->max_clients == 0) {
		fprintf(stderr, "l2cap_server_launch error : no socket available to run the server.\n");
		return -1;
	}

	pthread_t server_threads[server->max_clients];
	struct routine_data_t routine_data;
	routine_data.server = *server;
	routine_data.timeout = timeout;
	routine_data.max_req = max_req;
	for (uint8_t i = 0; i < server->max_clients; i++) {
		routine_data.num_client = i;
		pthread_create(&(server_threads[i]), NULL, &(socket_thread_routine), (void *)&routine_data);
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
