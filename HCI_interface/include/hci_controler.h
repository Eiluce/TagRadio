#ifndef __HCI_CONTROLER__
#define __HCI_CONTROLER__

#include "hci_device.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <stdarg.h>
#include "hci_socket.h"

/* Function "hci_compute_filter" :
   
*/
extern void hci_compute_filter(struct hci_filter *flt, ...);

extern hci_device_table_t hci_scan_devices(hci_socket_t *hci_socket,
				       uint8_t duration, uint16_t max_rsp, long flags);

extern void hci_get_RSSI(hci_socket_t *hci_socket,
			 bdaddr_t *mac, uint8_t duration, uint16_t max_rsp);	

extern void hci_LE_get_RSSI(hci_socket_t *hci_socket,
			    bdaddr_t *mac, uint16_t max_rsp, uint8_t scan_type, uint16_t scan_interval,
			    uint16_t scan_window, uint8_t own_add_type, uint8_t scan_filter_policy);

extern int8_t hci_LE_clear_white_list(hci_socket_t *hci_socket);

extern int8_t hci_LE_add_white_list(hci_socket_t *hci_socket, const hci_device_t hci_device);

extern int8_t hci_LE_rm_white_list(hci_socket_t *hci_socket, const hci_device_t hci_device);

extern int8_t hci_LE_get_white_list_size(hci_socket_t *hci_socket, uint8_t *size);

extern int8_t hci_LE_read_local_supported_features(hci_socket_t *hci_socket, uint8_t *features);

extern int8_t hci_LE_read_supported_states(hci_socket_t *hci_socket, uint64_t *states);

extern char hci_already_registered_device(bdaddr_t add);

extern hci_device_t *hci_register_device(hci_device_t hci_device);

extern hci_device_t hci_get_device(bdaddr_t add);

extern void hci_destroy_device_table(void);

#endif 
