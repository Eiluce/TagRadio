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

// TODO : cf ioctl FIONBIO : donner possibilité d'avoir dd non bloquant ?
// => On pourrait par exemple lancer un scan en asynchrone, et, tout en maintenant
// un groupe de n données dans un buffer, récupérer n données quand bon nous semble sans jamais
// avoir à relancer de scan ?

/* HCI socket strucutre : */
typedef struct hci_socket_t {
	int8_t sock; // Socket id or "device descriptor" (-1 indicates that an error occured).
	int8_t dev_id; // Bluetooth controller id.
	/* NOTE : we can retrieve the bt@ of the controller with the hci_devba(int dev_id, bdaddr_t *bdaddr)
	   function.*/
} hci_socket_t;

//------------------------------------------------------------------------------------

/* --------------
   - PROTOTYPES -
   --------------
*/

extern hci_socket_t open_hci_socket(bdaddr_t *controller);
extern void close_hci_socket(hci_socket_t *hci_socket);

extern void close_all_hci_sockets(list_t **hci_socket_list);

extern int8_t get_hci_socket_filter(hci_socket_t hci_socket, struct hci_filter *old_flt);
extern int8_t set_hci_socket_filter(hci_socket_t hci_socket, struct hci_filter *flt);

extern void display_hci_socket_list(list_t *hci_socket_list);

#endif
