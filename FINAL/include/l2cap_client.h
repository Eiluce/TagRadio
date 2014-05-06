#ifndef __L2CAP_CLIENT__
#define __L2CAP_CLIENT__

#include "l2cap_socket.h"
#include <bluetooth/bluetooth.h>
#include <stdint.h>

typedef struct l2cap_client_t {
	char *buffer;
	uint16_t buffer_length;
	l2cap_socket_t l2cap_socket;
	char connected;
	void (*treat_buffer)(struct l2cap_client_t client);
	void (*send_request)(struct l2cap_client_t client, uint8_t req_type);
} l2cap_client_t;

extern int8_t l2cap_client_create(l2cap_client_t *client, bdaddr_t *server_add, uint16_t port, 
			   uint16_t buffer_length, 
			   void (*treat_buffer_func)(l2cap_client_t client),
			   void (*send_request_func)(l2cap_client_t client, uint8_t req_type));

extern int8_t l2cap_client_connect(l2cap_client_t *client);

extern int8_t l2cap_client_send(l2cap_client_t *client, int16_t timeout, uint8_t req_type);

extern void l2cap_client_close(l2cap_client_t *client);


#endif 
