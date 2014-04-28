#include "l2cap_server.h"
#include "hci_controler.h"
#include "hci_socket.h"
#include <stdio.h>

static void send_response_default_func(int8_t sock, char *response) {
	char add_s[18] = "00:02:72:CD:29:67";
	bdaddr_t add;
	str2ba(add_s, &add); 
	hci_socket_t hci_socket1 = open_hci_socket(&add);

	bt_device_t test;
	str2ba("1C:BA:8C:20:E9:1E", &(test.mac));
	test.add_type = PUBLIC_DEVICE_ADDRESS;
	strcpy(test.custom_name, "MON SUPER SENSOR !");
	hci_LE_clear_white_list(NULL);
	hci_LE_add_white_list(&hci_socket1, test); // 0x00 : PDA 

	hci_LE_get_RSSI(&hci_socket1, &sock, NULL, 200, 0x00, 0x20, 0x10, 0x00, 0x00);
	return;
}

int main(int arc, char**argv) {
	l2cap_server_t server;
	char add_s[18] = "00:02:72:CD:29:67";
	bdaddr_t add;
	str2ba(add_s, &add); 
	l2cap_server_create(&server, &add, 0x1001, 1, 50, NULL, &send_response_default_func);
	fprintf(stderr, "%i\n", server.l2cap_socket_table[0].sock);
	l2cap_server_launch(&server, 100, 100);
	l2cap_server_close(&server);
} 





/*
int main(int arc, char**argv) {
	l2cap_server_t server;
	char add_s[18] = "00:02:72:CD:29:67";
	bdaddr_t add;
	str2ba(add_s, &add); 
	l2cap_server_create(&server, &add, 0x1001, 10, 50, NULL, NULL);
	for (uint16_t i = 0; i < server.num_sockets; i++) {
		fprintf(stderr, "%i\n", server.l2cap_socket_table[i].sock);
	}
	l2cap_server_launch(&server, 10, 100);
	l2cap_server_close(&server);
} 
*/
