#ifndef __BT_DEVICE__
#define __BT_DEVICE__

#include <stdint.h>
#include <bluetooth/bluetooth.h>

#define BT_NAME_LENGTH 50 // Max length of a stored name.

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
	char real_name[BT_NAME_LENGTH]; // Real "constructor" name of the device.
	char custom_name[BT_NAME_LENGTH]; // User-friendly name of the device.
} bt_device_t;

/* Bluetooth devices table structure :*/
typedef struct {
	bt_device_t *device; // Table of registred devices.
	uint16_t length;
} bt_device_table_t;

//------------------------------------------------------------------------------------

/* --------------
   - PROTOTYPES -
   --------------
*/

extern char bt_compare_addresses(const bdaddr_t *a1, const bdaddr_t *a2);

extern char bt_already_registered_device(bdaddr_t add);

extern bt_device_t *bt_register_device(bt_device_t bt_device);

extern bt_device_t bt_get_device(bdaddr_t add);

extern void bt_destroy_device_table(void);

extern bt_device_t bt_device_create(bdaddr_t mac, uint8_t add_type, 
				    const char *real_name, const char *custom_name);

/* Function "bt_device_display" :
   Displays the relative informations of a device on the standard output.
*/
extern void bt_device_display(bt_device_t device);

/* Function "bt_device_table_display" :
   Displays the relative informatios of all of the devices contained in
   the given table on the standard output.
*/
extern void bt_device_table_display(bt_device_table_t device_table);

#endif
