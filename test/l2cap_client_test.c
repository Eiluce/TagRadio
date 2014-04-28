#include "l2cap_client.h"
#include <errno.h>
#include <stdio.h>
#include <sys/unistd.h>



static void send_req_func(int8_t sock, char *req) {
	if (sock <= 0) {
		return;
	}
	write(sock, req, sizeof(req));
}

int main(int arc, char**argv) {
	l2cap_client_t client;
	int8_t status;
	char rem_add_s[18] = "00:02:72:CD:29:67";
	bdaddr_t rem_add;
	str2ba(rem_add_s, &rem_add); 
	l2cap_client_create(&client, &rem_add, 0x1001, 50, NULL, &(send_req_func));
	fprintf(stderr, "%i\n", client.l2cap_socket.sock);
	status = l2cap_client_connect(&client, 10);
	if (status == 0) {		
		client.send_request(client.l2cap_socket.sock, "GET_RSSI");
	} else {
		perror("connect");
		fprintf(stderr, "WTF ???????????\n");
	}
	int8_t bytes_read = read(client.l2cap_socket.sock, client.buffer, sizeof(client.buffer));
	while (bytes_read > 0) {
		client.treat_buffer(client.buffer);
		bytes_read = read(client.l2cap_socket.sock, client.buffer, sizeof(client.buffer));
	}
	l2cap_client_close(&client);
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
