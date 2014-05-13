#include "bt_device.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cfuhash.h"

static cfuhash_table_t *bt_devices_table = NULL;

//------------------------------------------------------------------------------------

char bt_compare_addresses(const bdaddr_t *a1, const bdaddr_t *a2) {
	return (memcmp((const void *)a1, (const void *)a2, sizeof(bdaddr_t)) == 0);
}

//------------------------------------------------------------------------------------

char bt_already_registered_device(bdaddr_t add) {
	if (!bt_devices_table) {
		bt_devices_table = cfuhash_new_with_initial_size(200);
	}
	char string_add[18]; 
	memset(string_add, 0, 18);
	ba2str((const bdaddr_t *)&(add), string_add);

	return (char)(cfuhash_exists(bt_devices_table, string_add));
}

//------------------------------------------------------------------------------------

bt_device_t *bt_register_device(bt_device_t bt_device) {
	if (!bt_devices_table) {
		bt_devices_table = cfuhash_new_with_initial_size(200);
	}
	char string_add[18]; 
	memset(string_add, 0, 18);
	ba2str((const bdaddr_t *)&(bt_device.mac), string_add);

	bt_device_t *tmp = malloc(sizeof(bt_device_t));
	memcpy(tmp, &bt_device, sizeof(bt_device_t));

	return (bt_device_t *)cfuhash_put(bt_devices_table, string_add, (void *)tmp);
} 

//------------------------------------------------------------------------------------

bt_device_t bt_get_device(bdaddr_t add) {
	bt_device_t *tmp;

	if (!bt_devices_table) {
		bt_devices_table = cfuhash_new_with_initial_size(200);
	}

	char string_add[18]; 
	memset(string_add, 0, 18);
	ba2str((const bdaddr_t *)&(add), string_add);

	tmp = (bt_device_t *)cfuhash_get(bt_devices_table, string_add);

	return *tmp;
}

//------------------------------------------------------------------------------------

void bt_destroy_device_table(void) {
	if (bt_devices_table) {
		cfuhash_destroy_with_free_fn(bt_devices_table, free);
	}
	bt_devices_table = NULL;
}

//------------------------------------------------------------------------------------

bt_device_t bt_device_create(bdaddr_t mac, uint8_t add_type, 
			     const char *real_name, const char *custom_name) {
	bt_device_t res = {0};
	res.mac = mac;
	res.add_type = add_type;
	if (real_name) {
		strncpy(res.real_name, real_name, BT_NAME_LENGTH);
	} else {
		strncpy(res.real_name, "UNKNOWN", BT_NAME_LENGTH);
	}
	if (custom_name) {
		strncpy(res.custom_name, custom_name, BT_NAME_LENGTH);
	} else {
		strncpy(res.custom_name, "UNKNOWN", BT_NAME_LENGTH);
	}

	bt_register_device(res);

	return res;
}		

//------------------------------------------------------------------------------------

void bt_device_display(bt_device_t device) {
	
	char tmp[18]; 
	memset(tmp, 0, 18);
	ba2str((const bdaddr_t *)&(device.mac), tmp);

	char address_type_mess[6] = {0};
	switch(device.add_type) {
	case PUBLIC_DEVICE_ADDRESS: 
		strcpy(address_type_mess, "[PDA]");
		break;
	case RANDOM_DEVICE_ADDRESS: 
		strcpy(address_type_mess, "[RDA]");
		break;
	default:
		strcpy(address_type_mess, "[???]");
		break;
	}

	fprintf(stdout, "%s [%s] : %s a.k.a %s\n", 
		address_type_mess, 
		tmp,
		device.real_name,
		device.custom_name);
}

//------------------------------------------------------------------------------------

void bt_device_table_display(bt_device_table_t device_table) {
	for (uint32_t i = 0; i < device_table.length; i++) {
		bt_device_display(device_table.device[i]);
	}
}
