#ifndef __L2CAP_CLIENT__
#define __L2CAP_CLIENT__

#include "l2cap_socket.h"
#include <bluetooth/bluetooth.h>
#include <stdint.h>

/* LANCER CLIENTS SUR THREADS DIFFERENTS*/
typedef struct l2cap_client_t {
	char *buffer;
	uint16_t buffer_length;
	l2cap_socket_t l2cap_socket;
	void (*treat_buffer)(char *buffer);
	void (*send_request)(int8_t sock, char *req);
} l2cap_client_t;

extern int8_t l2cap_client_create(l2cap_client_t *client, bdaddr_t *adaptater, uint16_t port, 
				  uint16_t buffer_length, void (*treat_buffer_func)(char *buffer), 
				  void (*send_request_func)(int8_t sock, char *req));

extern int8_t l2cap_client_connect(l2cap_client_t *client, uint16_t timeout);

extern void l2cap_client_close(l2cap_client_t *client);


#endif 
