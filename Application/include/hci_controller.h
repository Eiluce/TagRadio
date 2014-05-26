#ifndef __HCI_CONTROLLER__
#define __HCI_CONTROLLER__

/* Package "hci_controller".
 * Description : this package brings an "upper-layer" to the BlueZ's HCI
 (Host Controller Interface) functions in order to have more reliability and adaptivity. 
 All of the following functions are using an "hci_controller" structure in order to
 abstract entirely the BlueZ methods.
 * Author : FabLab ENSIMAG team : BERTAULD Thomas, BIANCHERI Cedric, BRELOT Lucie, FONT Ludovic.
 * Last modification : 23/05/2014.
 */

#include "bt_device.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include "hci_socket.h"
#include "list.h"
#include <stdarg.h>

// Default timeout used to communicate with the adapter using HCI.
// It it is set to 0, then no timeout will be used and we will we wait until the end of the command.
#define DEFAULT_TIMEOUT 3000

/* Our hci_controller is a state machine.
   That is to say that we associate a virtual state to each real used state
   of the adapter. For instance, when our adapter is scanning devices, our state
   machine will be in the "HCI_STATE_SCANNING" state.
   If you want to add more states, please follow the naming convention "HCI_STATE_..."
   for more convenience.
*/
typedef enum {
	HCI_STATE_CLOSED = 0,
	HCI_STATE_OPEN = 1, // Default state (the adapter doesn't do anything).
	HCI_STATE_SCANNING = 2,
	HCI_STATE_ADVERTISING = 3,
	HCI_STATE_READING = 4,
	HCI_STATE_WRITING = 5} hci_state_t;

/* Hci_controller structure : */
typedef struct hci_controller_t {
	bt_device_t device; // BT_device corresponding to the physical adapter.
	list_t *sockets_list; // List of opened sockets on this adapter.
	hci_state_t state; // Current state of the controller.
	char interrupted; /* Error indicator : if an error occured during a request,
			     the adapter could be stuck in a bad state. By putting
			     this indicator to true (1), we can see that an error
			     occured and then we can try to solve it (i.e try to
			     put the controller in the default state) by using
			     the "hci_resolve_interruption" function to be able to use
			     it properly again.
			  */
} hci_controller_t;
	
/* Function "hci_open_controller" :
   Creates a new hci_controller using  the adapter given by the "mac" reference.
   If this reference is NULL, then we take the first available adapter in
   the system.
   The field "name" is used to describe in an user-friendly way the adapter.
   If NULL then the name of the adapter will be "UNKNOWN".
*/
extern hci_controller_t hci_open_controller(bdaddr_t *mac, char *name);

/* Function "hci_resolve_interruption" :
   Tryes to resolve a past interruption of a controller in order to put it
   in the default state to be able to use it properly again.
   The field "hci_socket" must refers either to NULL, so we open a new socket
   to the given controller, or a valid opened socket on the given controller.
   The filed "hci_controller" has to be a valid reference on an opened 
   hci_controller.
*/
extern int8_t hci_resolve_interruption(hci_socket_t *hci_socket, hci_controller_t *hci_controller);

/* Function "hci_close_controller" :
   Closes and destroys an hci_controller structure. More precisely,
   it closes all of the opened sockets on this controller and frees
   all of the used memory.
   The "hci_controller" reference has to be a valid one.
*/
extern void hci_close_controller(hci_controller_t *hci_controller);

/* Function "hci_open_socket_controller" :
   Creates and opens a new socket on the given hci_controller.
   The "hci_controller" reference has to be a valid one.
   WARNING : only this function should be used to open a socket on an hci_controller
   for the socket to properly be added to the sockets list of the controller.
   Indeed, if you use the classic "open_hci_socket" function, the socket will not be
   added to the controller and it could result to an UNSTABLE system state when using
   a socket non-created with this function.
*/
extern hci_socket_t hci_open_socket_controller(hci_controller_t *hci_controller);

/* Function "hci_close_socket_controller" :
   Closes one of the opened sockets of an hci_controller.
   The "hci_controller" reference has to be a reference on a valid and
   opened hci_controller. 
   The "hci_socket" filed has to refer to an opened socket on the given controller.
   WARNING : only this function should be used to close a socket on an hci_controller
   for the socket to properly be removed from the sockets list of the controller.
   In fact, if you use the classic "close_hci_socket" function, the socket will still
   be referenced in the list and could be used (while closed) in another and lead
   to errors.
*/
extern void hci_close_socket_controller(hci_controller_t *hci_controller, hci_socket_t *hci_socket);


/* Function "hci_compute_filter" :
   Computes an hci_filter using a variable list of uint32_t (corresonding to binary filters
   to apply) ending by 0 (in order for the va_list to know where it ends).
*/
extern void hci_compute_filter(struct hci_filter *flt, ...);

/* Function "hci_scan_devices" :
   Performs a basic Bluetooth scan to recognize the nerby devices.
   The field "hci_socket" can be either a valid opened socket on a valid Bluetooth adapter
   or NULL in which case we open a new socket on the "hci_controller" given as parameter.
   The field "hci_controller" has to refer to a valid opened hci_controller.
   The field max_rsp sets the maximum number of scan's responses we want to have.
   FIELD DURATION TODOTODO TODO TODO TODO TODO
 */
extern bt_device_table_t hci_scan_devices(hci_socket_t *hci_socket, hci_controller_t *hci_controller,
					  uint8_t duration, uint16_t max_rsp, long flags);

extern void hci_compute_device_name(hci_socket_t *hci_socket, hci_controller_t *hci_controller, bt_device_t *bt_device);

extern char *hci_get_RSSI(hci_socket_t *hci_socket, hci_controller_t *hci_controller, int8_t *file_descriptor,
			 bdaddr_t *mac, uint8_t duration, uint16_t max_rsp);	

extern char *hci_LE_get_RSSI(hci_socket_t *hci_socket, hci_controller_t *hci_controller, int8_t *file_descriptor,
			    bdaddr_t *mac, uint16_t max_rsp, uint8_t scan_type, uint16_t scan_interval,
			    uint16_t scan_window, uint8_t own_add_type, uint8_t scan_filter_policy);


/* Function "hci_LE_clear_white_list" :
   Clears the white list of a Bluetooth adapter. The white list is the list of
   the devices from which we may take the answers while performing LE inquiries.
   The field "hci_socket" can be either a valid opened socket on a valid Bluetooth adapter
   or NULL in which case we open a new socket on the "hci_controller" given as parameter.
   The field "hci_controller" has to refer to a valid opened hci_controller.
   On success, the function should return a non-negative value.
*/
extern int8_t hci_LE_clear_white_list(hci_socket_t *hci_socket, hci_controller_t *hci_controller);

/* Function "hci_LE_add_white_list" :
   Adds the device "bt_device" to the white list of a Bluetooth adapter.
   The field "hci_socket" can be either a valid opened socket on a valid Bluetooth adapter
   or NULL in which case we open a new socket on the "hci_controller" given as parameter.
   The field "hci_controller" has to refer to a valid opened hci_controller.
   On success, the function should return a non-negative value.
*/
extern int8_t hci_LE_add_white_list(hci_socket_t *hci_socket, hci_controller_t *hci_controller, const bt_device_t bt_device);

/* Function "hci_LE_rm_white_list" :
   Removes the device "bt_device" from the white list of a Bluetooth adapter.
   The field "hci_socket" can be either a valid opened socket on a valid Bluetooth adapter
   or NULL in which case we open a new socket on the "hci_controller" given as parameter.
   The field "hci_controller" has to refer to a valid opened hci_controller.
   On success, the function should return a non-negative value.
*/
extern int8_t hci_LE_rm_white_list(hci_socket_t *hci_socket, hci_controller_t *hci_controller, const bt_device_t bt_device);

/* Function "hci_LE_get_white_list_size" :
   Reads the size of the white list of a Bluetooth adapter.
   The field "hci_socket" can be either a valid opened socket on a valid Buletooth adapter
   or NULL in which case we open a new socket on the "hci_controller" given as parameter.
   The field "hci_controller" has to refer to a valid opened hci_controller.
   On success, the function should return a non-negative value and the variable pointed
   by "size" should have been set to the size of the white list.
*/
extern int8_t hci_LE_get_white_list_size(hci_socket_t *hci_socket, hci_controller_t *hci_controller, uint8_t *size);

/* Function "hci_LE_read_local_supported_features" :
   Reserved for Future Use.
   This function gives the supported LE features of a Bluetooth adapter but since the official
   Bluetooth core specification doesn't give any information on this expect that most of the
   values returned are RFU, this function has no warranty to work well.
*/
extern int8_t hci_LE_read_local_supported_features(hci_socket_t *hci_socket, hci_controller_t *hci_controller, uint8_t *features);

/* Function "hci_LE_read_supported_states" :
   Reads the supported (real) states of a Bluetooth adapter.
   The field "hci_socket" can be either a valid opened socket on a valid Bluetooth adapter
   or NULL in which case we open a new socket on the "hci_controller" given as parameter.
   The field "hci_controller" has to refer to a valid opened hci_controller.
   On success, the function should return a non-negative value and the variable pointed by "states"
   should have been set to an unsigned int representing a binary filter of the available states.
   For an user-friendly view, the states contained in this binary filter can be displayed 
   on the standard output by using the function "hci_display_LE_states" from the package
   "HCI_UTILS".
*/
extern int8_t hci_LE_read_supported_states(hci_socket_t *hci_socket, hci_controller_t *hci_controller, uint64_t *states);

#endif 
