#ifndef __L2CAP_SOCKET__
#define __L2CAP_SOCKET__

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <stdint.h>
#include "list.h"

/* --------------
   - STRUCTURES -
   --------------
*/

/* L2CAP socket strucutre : */
typedef struct l2cap_socket_t {
	int8_t sock; // Socket id (-1 indicates that an error occured).
	struct sockaddr_l2 sockaddr;
} l2cap_socket_t;


extern l2cap_socket_t open_l2cap_socket(bdaddr_t *adaptater, uint16_t port, char to_bind);
extern void close_l2cap_socket(l2cap_socket_t *l2cap_socket);

extern list_t *get_l2cap_socket_list(void);

extern void close_all_l2cap_sockets(void);

extern void display_l2cap_socket_list(void);

#endif
