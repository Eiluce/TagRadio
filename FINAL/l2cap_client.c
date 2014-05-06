#include "l2cap_client.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/errno.h>
#include <bluetooth/l2cap.h>
#include <sys/unistd.h>
#include <pthread.h>


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
	
	client->connected = 0;
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
	int8_t status = -1;
	status = connect(client->l2cap_socket.sock, 
			 (struct sockaddr *)&(client->l2cap_socket.sockaddr),
			 sizeof(client->l2cap_socket.sockaddr));
	if (status == 0) {
		client->connected = 1;
	} else {
		client->connected = 0;
	}

	return status;
}

//------------------------------------------------------------------------------------

int8_t l2cap_client_send(l2cap_client_t *client, int16_t timeout, uint8_t req_type) {
	if (!client) {
		fprintf(stderr, "l2cap_client_send error ; invalid client.\n");
		return -1;
	}
	if (client->connected) {
		memset(client->buffer, 0, strlen(client->buffer));
		client->send_request(*client, req_type);
		int8_t n = 0;
		int8_t bytes_read;
		struct pollfd p;
		p.fd = client->l2cap_socket.sock;
		p.events = POLLIN;
		
		while ((n = poll(&p, 1, timeout)) < 0) {
			if (errno == EAGAIN || errno == EINTR) {
				continue;
			}
			perror("l2cap_client_send : error while polling socket");
			return -1;
		}
		
		if (!n) {
			errno = ETIMEDOUT;
			perror("l2cap_client_send : error while polling socket");
			return -1;
		}
		while(bytes_read = read(client->l2cap_socket.sock,
					client->buffer,
					client->buffer_length) < 0) {
			if (errno == EAGAIN || errno == EINTR)
				continue;
			perror("l2cap_client_send : error while reading socket.\n");
			return -1;
		}
		client->treat_buffer(*client);
	} else {
		fprintf(stderr, "l2cap_client_send : error : invalid connexion.\n");
		return -1;
	}

	return 0;
}

//------------------------------------------------------------------------------------

void l2cap_client_close(l2cap_client_t *client) {
	free(client->buffer);
	close(client->l2cap_socket.sock);
}
