#include "l2cap_client.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/poll.h>

#define CLIENT_GET_RSSI 1
#define CLIENT_CLOSE_CONNECTION 2

static void send_req_func(l2cap_client_t client, uint8_t req_type) {
	int8_t sock = client.l2cap_socket.sock;
	char req[25];
	if (sock < 0) {
		return;
	}
	switch (req_type) {
	case CLIENT_GET_RSSI :
		strcpy(req, "GET_RSSI");
		write(sock, req, sizeof(req));
		break;
	case CLIENT_CLOSE_CONNECTION :
		strcpy(req , "CLOSE");
		write(sock, req, sizeof(req));
		break;
	default:
		fprintf(stderr, "client_send_request error : unknown request type.\n");
		break;
	}
}

int main(int arc, char**argv) {
	l2cap_client_t client;
	int8_t status;
	char rem_add_s[18] = "00:02:72:CD:29:67";
	bdaddr_t rem_add;
	str2ba(rem_add_s, &rem_add); 
	int nb_mesures = 0;
	l2cap_client_create(&client, &rem_add, 0x1001, 500, NULL, &(send_req_func));
	fprintf(stderr, "%i\n", client.l2cap_socket.sock);
	status = l2cap_client_connect(&client);	
	if (status == 0) {
		while (nb_mesures < 10) {
			client.send_request(client, CLIENT_GET_RSSI);
			int8_t n = 0;
			int8_t bytes_read;
			struct pollfd p;
			p.fd = client.l2cap_socket.sock;
			p.events = POLLIN;

			while ((n = poll(&p, 1, 4000)) < 0) {
				if (errno == EAGAIN || errno == EINTR) {
					continue;
				}
				perror("client : error while polling socket");
				goto end;
			}
			
			if (!n) {
				errno = ETIMEDOUT;
				perror("client : error while polling socket");
				goto end;
			}
			while(bytes_read = read(client.l2cap_socket.sock,
						client.buffer,
						client.buffer_length) < 0) {
				if (errno == EAGAIN || errno == EINTR)
					continue;
				perror("client : error while reading socket.\n");
				goto end;
			}
			client.treat_buffer(client);		
			
			nb_mesures++;
		}
	end:
		client.send_request(client, CLIENT_CLOSE_CONNECTION);
		l2cap_client_close(&client);
	} else {
		perror("client_connect");
	}		
} 










/*
static int8_t nb_appels = 0;

static void send_req_func(int8_t sock, char *req) {
	if (sock <= 0) {
		return;
	}
	nb_appels++;
	char tmp[100];
	sprintf(tmp, "req n°%u : %s", nb_appels, req);
	write(sock, tmp, sizeof(tmp));
}

int main(int arc, char**argv) {
	l2cap_client_t client;
	int8_t status;
	char rem_add_s[18] = "00:02:72:CD:29:67";
	bdaddr_t rem_add;
	str2ba(rem_add_s, &rem_add); 
	for (uint8_t i = 0; i < 10; i++) {
		l2cap_client_create(&client, &rem_add, 0x1001+(i*2), 50, NULL, &(send_req_func));
		fprintf(stderr, "%i\n", client.l2cap_socket.sock);
		status = l2cap_client_connect(&client, 10);
		if (status == 0) {		
			client.send_request(client.l2cap_socket.sock, "hello!");
		} else {
			perror("connect");
			fprintf(stderr, "WTF ???????????\n");
		}

	}	
	l2cap_client_close(&client);
} 
*/
