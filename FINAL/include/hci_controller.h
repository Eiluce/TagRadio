#ifndef __HCI_CONTROLLER__
#define __HCI_CONTROLLER__

#include "bt_device.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include "hci_socket.h"
#include "list.h"
#include <stdarg.h>

#define DEFAULT_TIMEOUT 3000

typedef enum {
	HCI_STATE_CLOSED = 0,
	HCI_STATE_OPEN = 1,
	HCI_STATE_SCANNING = 2,
	HCI_STATE_ADVERTISING = 3,
	HCI_STATE_READING = 4,
	HCI_STATE_WRITING = 5} hci_state_t;

typedef struct hci_controller_t {
	bt_device_t device;
	list_t *sockets_list;
	hci_state_t state;
	char interrupted;
} hci_controller_t;
	

extern hci_controller_t hci_open_controller(bdaddr_t *mac, char *name);

extern int8_t hci_resolve_interruption(hci_socket_t *hci_socket, hci_controller_t *hci_controller);

extern void hci_close_controller(hci_controller_t *hci_controller);

extern hci_socket_t hci_open_socket_controller(hci_controller_t *hci_controller);

extern void hci_close_socket_controller(hci_controller_t *hci_controller, hci_socket_t *hci_socket);

extern void hci_compute_filter(struct hci_filter *flt, ...);

extern bt_device_table_t hci_scan_devices(hci_socket_t *hci_socket, hci_controller_t *hci_controller,
					  uint8_t duration, uint16_t max_rsp, long flags);

extern void hci_compute_device_name(hci_socket_t *hci_socket, hci_controller_t *hci_controller, bt_device_t *bt_device);

extern char *hci_get_RSSI(hci_socket_t *hci_socket, hci_controller_t *hci_controller, int8_t *file_descriptor,
			 bdaddr_t *mac, uint8_t duration, uint16_t max_rsp);	

extern char *hci_LE_get_RSSI(hci_socket_t *hci_socket, hci_controller_t *hci_controller, int8_t *file_descriptor,
			    bdaddr_t *mac, uint16_t max_rsp, uint8_t scan_type, uint16_t scan_interval,
			    uint16_t scan_window, uint8_t own_add_type, uint8_t scan_filter_policy);

extern int8_t hci_LE_clear_white_list(hci_socket_t *hci_socket, hci_controller_t *hci_controller);

extern int8_t hci_LE_add_white_list(hci_socket_t *hci_socket, hci_controller_t *hci_controller, const bt_device_t bt_device);

extern int8_t hci_LE_rm_white_list(hci_socket_t *hci_socket, hci_controller_t *hci_controller, const bt_device_t bt_device);

extern int8_t hci_LE_get_white_list_size(hci_socket_t *hci_socket, hci_controller_t *hci_controller, uint8_t *size);

extern int8_t hci_LE_read_local_supported_features(hci_socket_t *hci_socket, hci_controller_t *hci_controller, uint8_t *features);

extern int8_t hci_LE_read_supported_states(hci_socket_t *hci_socket, hci_controller_t *hci_controller, uint64_t *states);

#endif 
