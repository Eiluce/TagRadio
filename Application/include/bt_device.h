#ifndef __BT_DEVICE__
#define __BT_DEVICE__

/* Package "bt_device".
 * Description : this package gives the most useful "basic" functions to deal with
 a Bluetooth device.
 * Author : FabLab ENSIMAG team : BERTAULD Thomas, BIANCHERI Cedric, BRELOT Lucie, FONT Ludovic.
 * Last modification : 23/05/2014.
 */

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

/* Function "bt_compare_addresses" :
   Allows us to compare two bt mac addresses.
   Returns 1 if the the two addresses are the same. Else returns 0.
*/
extern char bt_compare_addresses(const bdaddr_t *a1, const bdaddr_t *a2);


/* Function "bt_already_registered_device" :
   Allows us to know if we already have encoutered (and stored the information)
   of a bt device.
   Please note that we use a hash table to store the bt_device's information.
   Returns 1 if the device was already registered. Else returns 0.
*/
extern char bt_already_registered_device(bdaddr_t add);

/* Function "bt_register_device" : 
   Stores a device in our data structure (currently an hash table).
   We store the devices by using a couple (@, bt_device).
   WARNING : due to our implementation, there is no possible double-entries
   (corresponding to two same @) and so, if we try to add a device having the
   same mac @ that another previously stored device, it will erase that device.
   Returns the previous stored device corresponding to the @ associated
   to the parameter "bt_device". If there is no such device, the function
   returns NULL.
*/
extern bt_device_t *bt_register_device(bt_device_t bt_device);

/* Function "bt_get_device" :
   Returns the stored device corresponding to the given address.
   If there is no suitable stored device, then the function returns NULL.
*/
extern bt_device_t *bt_get_device(bdaddr_t add);

/* Function "bt_destroy_device_table" :
   Function used to destroy and free our hash table containing the
   (@, bt_device) couples.
   Since it is possible to recreate the table by using any function accessing
   to this table, this function could be used to "reset" the table.
*/
extern void bt_destroy_device_table(void);

/* Funcion "bt_device_create" :
   Creates a new "bt_device" structure filled with the given parameters.
   If one (or both) of the "name" parameters is NULL, the function will automatically
   fill the corresponding field with the name "UNKNOWN".
*/
extern bt_device_t bt_device_create(bdaddr_t mac, uint8_t add_type, 
				    const char *real_name, const char *custom_name);

/* Function "bt_device_display" :
   Displays the relative information of a device on the standard output.
*/
extern void bt_device_display(bt_device_t device);

/* Function "bt_device_table_display" :
   Displays the relative informatios of all of the devices contained in
   the given table on the standard output.
*/
extern void bt_device_table_display(bt_device_table_t device_table);

#endif
