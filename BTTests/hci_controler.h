#ifndef __HCI_CONTROLER__
#define __HCI_CONTROLER__

#include "hci_device.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <stdarg.h>
#include "hci_socket.h"


extern void hci_compute_filter(struct hci_filter *flt, ...);

extern hci_device_table_t hci_scan_devices(hci_socket_t *hci_socket,
				       uint8_t duration, uint8_t max_rsp, long flags);

extern void hci_get_RSSI(hci_socket_t *hci_socket,
			 bdaddr_t *mac, uint8_t duration, uint8_t max_rsp);	

extern void hci_LE_get_RSSI(hci_socket_t *hci_socket,
			    bdaddr_t *mac, uint8_t max_rsp, uint8_t scan_type, uint8_t scan_interval,
			    uint8_t scan_window, uint8_t own_add_type, uint8_t scan_filter_policy);

#endif 
