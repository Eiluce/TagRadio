#ifndef __HCI_DEVICE__
#define __HCI_DEVICE__

#include <stdint.h>
#include <bluetooth/bluetooth.h>

#define NAME_LENGTH 50 // Max length of a stored name.

// Addresses types :
#define PUBLIC_DEVICE_ADDRESS 0x00
#define RANDOM_DEVICE_ADDRESS 0x01
#define UNKNOWN_ADDRESS_TYPE 0x12

/* --------------
   - STRUCTURES -
   --------------
*/

/* Bluetooth Device structure : */
typedef struct {
	bdaddr_t mac; // Mac (Public or not) address of the device. 
	uint8_t add_type; /* Address type. The following value are allowed :
			   * - 0x00 : Public Device Address (PDA).
			   * - 0x01 : Random Device Address (RDA).
			   * - 0x12 : Unknown address type (personnal code).
			   */
	char real_name[NAME_LENGTH]; // Real "constructor" name of the device.
	char custom_name[NAME_LENGTH]; // User-friendly name of the device.
} hci_device_t;

/* Bluetooth devices table structure :*/
typedef struct {
	hci_device_t *device; // Table of registred devices.
	uint16_t length;
} hci_device_table_t;

//------------------------------------------------------------------------------------

/* --------------
   - PROTOTYPES -
   --------------
*/

/* Function "hci_device_display" :
   Displays the relative informations of a device on the standard output.
*/
extern void hci_device_display(hci_device_t device);

/* Function "hci_device_table_display" :
   Displays the relative informatios of all of the devices contained in
   the given table on the standard output.
*/
extern void hci_device_table_display(hci_device_table_t device_table);

#endif
