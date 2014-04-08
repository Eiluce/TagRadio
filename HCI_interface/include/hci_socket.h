#ifndef __HCI_SOCKET__
#define __HCI_SOCKET__

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <stdint.h>
#include "list.h"

/* --------------
   - STRUCTURES -
   --------------
*/

/* HCI socket strucutre : */
typedef struct hci_socket_t {
	int8_t sock; // Socket id (-1 indicates that an error occured).
	int8_t dev_id; // Bluetooth controler id.
} hci_socket_t;

//------------------------------------------------------------------------------------

/* --------------
   - PROTOTYPES -
   --------------
*/

// TODO : FAIRE UNE LISTE DE SOCKETS PAR DEV_ID !!


extern hci_socket_t open_hci_socket(bdaddr_t *controler);
extern void close_hci_socket(hci_socket_t *hci_socket);

extern list_t *get_hci_socket_list(void);
extern void close_all_sockets(void);

extern int8_t get_hci_socket_filter(hci_socket_t hci_socket, struct hci_filter *old_flt);
extern int8_t set_hci_socket_filter(hci_socket_t hci_socket, struct hci_filter *flt);

extern void display_hci_socket_list(void);

#endif
