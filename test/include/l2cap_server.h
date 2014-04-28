#ifndef __L2CAP_SERVER__
#define __L2CAP_SERVER__

#include "l2cap_socket.h"
#include <bluetooth/bluetooth.h>
#include <stdint.h>

/* A NOTER : les ports L2CAP dynamiques vont de 4097 (0x1001) à 32765, mais en prenant que les numéros impairs !
 */

typedef struct l2cap_server_t {
	char launched;
	char **buffer_table;
	uint16_t buffer_length;
	l2cap_socket_t *l2cap_socket_table;
	uint8_t num_sockets;
	void (*treat_buffer)(char *buffer);
	void (*send_response)(int8_t sock, char *res);
} l2cap_server_t;

extern int8_t l2cap_server_create(l2cap_server_t *server, bdaddr_t *adaptater, uint16_t port, 
				  uint8_t num_sockets, uint16_t buffer_length, 
				  void (*treat_buffer_func)(char *buffer),
				  void (*send_response_func)(int8_t sock, char * response));

extern int8_t l2cap_server_launch(l2cap_server_t *server, uint16_t timeout, uint16_t max_req);

extern void l2cap_server_close(l2cap_server_t * server);


#endif 
