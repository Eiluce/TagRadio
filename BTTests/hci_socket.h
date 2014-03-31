#ifndef __HCI_SOCKET__
#define __HCI_SOCKET__

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <stdint.h>
#include "list.h"

typedef struct hci_socket_t {
	int8_t sock;
	int8_t dev_id;
} hci_socket_t;


extern hci_socket_t open_hci_socket(bdaddr_t *controler);
extern void close_hci_socket(hci_socket_t *hci_socket);

extern list_t *get_hci_socket_list(void);
extern void close_all_sockets();

extern struct hci_filter get_hci_socket_filter(hci_socket_t hci_socket);
extern int8_t set_hci_socket_filter(hci_socket_t hci_socket, struct hci_filter flt);

#endif
