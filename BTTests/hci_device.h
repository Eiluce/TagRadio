#ifndef __HCI_DEVICE__
#define __HCI_DEVICE__

#include <stdint.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#define NAME_LENGTH 50

typedef struct {
	bdaddr_t mac;
	uint8_t add_type;
	char real_name[NAME_LENGTH];
	char custom_name[NAME_LENGTH];
} hci_device_t;

typedef struct {
	hci_device_t *device;
	uint16_t length;
} hci_device_table_t;

extern void hci_device_display(hci_device_t device);

#endif
