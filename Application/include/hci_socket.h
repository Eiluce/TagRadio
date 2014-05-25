#ifndef __HCI_SOCKET__
#define __HCI_SOCKET__

/* Package "hci_socket".
 * Description : this package gives the most useful functions to deal with
 hci_sockets. Those sockets are "intern" sockets and are only used to communicate with
 a LOCAL bt adapter (typically a bt dongle).
 * Author : FabLab ENSIMAG team : BERTAULD Thomas, BIANCHERI Cedric, BRELOT Lucie, FONT Ludovic.
 * Last modification : 23/05/2014.
 */

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
	/* NOTE : we can retrieve the bt@ of the controller with the "hci_devba(int dev_id, bdaddr_t *bdaddr)"
	   function.*/
} hci_socket_t;

//------------------------------------------------------------------------------------

/* --------------
   - PROTOTYPES -
   --------------
*/

/* Function "open_hci_socket" :
   Opens an hci_socket on the given controller. If this address
   is NULL, we take the first available controller on the system.
   On success, the returned socket must have its both fields ("sock"
   and "dev_id") non-negative.
*/
extern hci_socket_t open_hci_socket(bdaddr_t *controller);

/* Function "close_hci_socket" :
   Closes a previsouly opened hci_socket. If the given socket's reference
   is invalid or if the socket is already closed, this function should
   print a warning message on the standard error output.
*/
extern void close_hci_socket(hci_socket_t *hci_socket);

/* Function "close_all_hci_sockets" :
   Closes all the hci_sockets stored in a list.
   Because this function calls "close_hci_socket", if one of the references
   stored in the list is invalid or if one of the contained sockets was already
   closed, a warning message should be displayed on the standard error output.
*/
extern void close_all_hci_sockets(list_t **hci_socket_list);

/* Function "get_hci_socket_filter" :
   Retrieves the current socket filter applied to the given hci_socket.
   On success, the filter referenced by the parameter "old_flt" is filled with the
   current filter applied to the given socket and the function returns 0.
   If an error occured, the filter has not been retrieved and -1 is returned.
*/
extern int8_t get_hci_socket_filter(hci_socket_t hci_socket, struct hci_filter *old_flt);

/* Function "set_hci_socket_filer" :
   Sets the filter of the given hci_socket by using the given filter's reference.
   The reference has to be a valid one. 
   One success, the filter has been set on the socket and the function returns 0.
   If an error occured, the filter has bot been set and -1 is returned.
*/
extern int8_t set_hci_socket_filter(hci_socket_t hci_socket, struct hci_filter *flt);

/* Function "display_hci_socket_list" :
   Allows us to display all the hci_sockets stored in an hci_socket
   list on the standard output.
   The list's reference has to be a valid one.
*/
extern void display_hci_socket_list(list_t *hci_socket_list);

#endif
