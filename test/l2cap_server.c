#include "l2cap_server.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/l2cap.h>
#include <sys/unistd.h>
#include <pthread.h>

struct client_t {
	struct sockaddr_l2 rem_addr;
	int8_t conn_id;
};

struct routine_data_t {
	int8_t sock;
	uint8_t num_sock;
	struct client_t client;
	l2cap_server_t server;
};
	

static void treat_buffer_default_func(char *buffer) {
	fprintf(stderr, "Buffer trace : %s\n", buffer);
	return;
}

static void send_response_default_func(int8_t sock, char *response) {
	write(sock, response, sizeof(response));
	return;
}

int8_t l2cap_server_create(l2cap_server_t *server, bdaddr_t *adapter, uint16_t port, 
			   uint8_t num_sockets, uint16_t buffer_length, 
			   void (*treat_buffer_func)(char *buffer),
			   void (*send_response_func)(int8_t sock, char *response)) {
	int8_t opened_sockets = 0;
	

	if (!server) {
		fprintf(stderr, "l2cap_server_create error : invalid server reference.\n");
		return -1;
	}

	if (!buffer_length) {
		fprintf(stderr, "l2cap_server_create warning : null buffer, creation aborted.\n");
		return -1;
	}

	server->buffer_table = calloc(num_sockets, sizeof(char *));
	server->l2cap_socket_table = calloc(num_sockets, sizeof(l2cap_socket_t));

	for (uint8_t i = 0; i < num_sockets; i++) {
		server->buffer_table[i] = calloc(buffer_length, sizeof(char));
		memset(server->buffer_table[i], 0, sizeof(server->buffer_table[i]));
		/* On ne peut pas "bind" deux sockets sur le même couple (@, port) !*/
		server->l2cap_socket_table[i] = open_l2cap_socket(adapter, port+(i*2), 1);
		if (server->l2cap_socket_table[i].sock >= 0) {
			opened_sockets++;
		}
	}
	server->buffer_length = buffer_length;
	server->num_sockets = num_sockets;
	
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
	server->launched = 0;

	return opened_sockets;
}

static void *socket_thread_routine(void *data) {
	struct routine_data_t *routine_data = (struct routine_data_t *)data;
	socklen_t sockaddr_len = sizeof(struct sockaddr_l2);
	if (routine_data->sock >= 0) {
		listen(routine_data->sock, 1);
		fprintf(stderr, "JE TECOUTE !\n");
		routine_data->client.conn_id = accept(routine_data->sock,
					   (struct sockaddr *)&(routine_data->client.rem_addr), 
					   &sockaddr_len);
		fprintf(stderr, "JE TENTEND !!\n");
	} else {
		fprintf(stderr, "server_launch_socket_routine error : invalid socket.\n");
		return NULL;
	}

	uint8_t i = routine_data->num_sock;
	memset(routine_data->server.buffer_table[i], 0, sizeof(routine_data->server.buffer_table[i]));
	int8_t bytes_read = 0;
	bytes_read = read(routine_data->client.conn_id,
			  routine_data->server.buffer_table[i],
			  sizeof(routine_data->server.buffer_table[i]));
	if( bytes_read > 0 ) {
		routine_data->server.treat_buffer(routine_data->server.buffer_table[i]);
		routine_data->server.send_response(routine_data->client.conn_id, NULL); // A MODIFIER
	}

	close(routine_data->client.conn_id);

	return NULL;
}

int8_t l2cap_server_launch(l2cap_server_t *server, uint16_t timeout, uint16_t max_req) {
	if (server == NULL) {
		fprintf(stderr, "l2cap_server_launch error : invalid server reference.\n");
		return -1;
	}

	if (server->num_sockets == 0) {
		fprintf(stderr, "l2cap_server_launch error : no socket available to run the server.\n");
		return -1;
	}

	struct client_t client[server->num_sockets];
	pthread_t server_threads[server->num_sockets];
	struct routine_data_t routine_data;
	routine_data.server = *server;

	for (uint8_t i = 0; i < server->num_sockets; i++) {
		routine_data.num_sock = i;
		routine_data.sock = server->l2cap_socket_table[i].sock;
		routine_data.client = client[i];
		pthread_create(&(server_threads[i]), NULL, &(socket_thread_routine), (void *)&routine_data);
	}

	for (uint8_t i = 0; i < server->num_sockets; i++) {
		pthread_join(server_threads[i], NULL);
	}
	
	return 0;
}

void l2cap_server_close(l2cap_server_t * server) {
	for (uint8_t i = 0; i < server->num_sockets; i++) {
		free(server->buffer_table[i]);
		if (server->l2cap_socket_table[i].sock >= 0) {
			close(server->l2cap_socket_table[i].sock);
		}
	}
	free(server->l2cap_socket_table);
	free(server->buffer_table);
}
