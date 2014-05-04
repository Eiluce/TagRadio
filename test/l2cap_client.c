#include "l2cap_client.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/l2cap.h>
#include <sys/unistd.h>

static void treat_buffer_default_func(l2cap_client_t client) {
	fprintf(stderr, "Buffer trace : %s\n", client.buffer);
	return;
}

static void send_request_default_func(l2cap_client_t client, uint8_t req_type) {
	if (client.l2cap_socket.sock < 0) {
		fprintf(stderr, "client_send_request error : invalid socket descriptor.\n");
		return;
	}
	char request[] = "Request echo.";
	write(client.l2cap_socket.sock, request, sizeof(request));
	return;
}

//------------------------------------------------------------------------------------

int8_t l2cap_client_create(l2cap_client_t *client, bdaddr_t *server_add, uint16_t port, 
			   uint16_t buffer_length, 
			   void (*treat_buffer_func)(l2cap_client_t client),
			   void (*send_request_func)(l2cap_client_t client, uint8_t req_type)) {
	
	if (!client) {
		fprintf(stderr, "l2cap_client_create error : invalid client reference.\n");
		return -1;
	}

	if (!buffer_length) {
		fprintf(stderr, "l2cap_client_create warning : null buffer, creation aborted.\n");
		return -1;
	}
	
	client->l2cap_socket = open_l2cap_socket(server_add, port, 0);
	if (client->l2cap_socket.sock < 0) {
		fprintf(stderr, "l2cap_client_create error : cannot open socket.\n");
		return -1;
	}
	client->buffer = calloc(buffer_length, sizeof(char));
	memset(client->buffer, 0, sizeof(client->buffer));
	client->buffer_length = buffer_length;
	
	if (!treat_buffer_func) {
		client->treat_buffer = &(treat_buffer_default_func);
	} else {
		client->treat_buffer = treat_buffer_func;
	}
	if (!send_request_func) {
		client->send_request = &(send_request_default_func);
	} else {
		client->send_request = send_request_func;
	}

	return 0;
}

//------------------------------------------------------------------------------------

int8_t l2cap_client_connect(l2cap_client_t *client) {
	return connect(client->l2cap_socket.sock, 
		       (struct sockaddr *)&(client->l2cap_socket.sockaddr),
		       sizeof(client->l2cap_socket.sockaddr));
}

//------------------------------------------------------------------------------------

void l2cap_client_close(l2cap_client_t *client) {
	free(client->buffer);
	close(client->l2cap_socket.sock);
}
