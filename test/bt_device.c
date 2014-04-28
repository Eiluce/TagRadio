#include "bt_device.h"
#include <stdio.h>

void bt_device_display(bt_device_t device) {
	
	char tmp[18]; 
	memset(tmp, 0, 18);
	ba2str((const bdaddr_t *)&(device.mac), tmp);

	char address_type_mess[6] = {0};
	switch(device.add_type) {
	case 0x00: // Public Device Address
		strcpy(address_type_mess, "[PDA]");
		break;
	case 0x01: // Random Device Address
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
