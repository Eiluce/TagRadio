#include "hci_controller.h"
#include "hci_socket.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static const char sensorAdd[18] = "1C:BA:8C:20:E9:1E";

static const char btControllerAdd[18] = "00:1A:7D:DA:71:0D"; //Pearl

int main(int argc, char **argv) {
	bdaddr_t controllerAdd;
	bdaddr_t sensorMac;
	str2ba(btControllerAdd, &controllerAdd);
	hci_controller_t hci_controller = hci_open_controller(&controllerAdd, "SERVER_TEST");

	bt_device_t sensor;
	str2ba(sensorAdd, &(sensorMac));
	sensor = bt_device_create(sensorMac,PUBLIC_DEVICE_ADDRESS, NULL, "SENSOR_TAG");

	hci_socket_t test = hci_open_socket_controller(&hci_controller);
	hci_socket_t test2 = hci_open_socket_controller(&hci_controller);
	hci_socket_t test3 = hci_open_socket_controller(&hci_controller);
	hci_socket_t test4 = hci_open_socket_controller(&hci_controller);
	hci_socket_t test5 = hci_open_socket_controller(&hci_controller);

	display_hci_socket_list(hci_controller.sockets_list);
	fprintf(stderr, "%i, %i\n", test.dev_id, test.sock);
	hci_close_socket_controller(&hci_controller, &test);
	fprintf(stderr, "Etat courant :\n");
	display_hci_socket_list(hci_controller.sockets_list);

	hci_LE_clear_white_list(&test2, &hci_controller);
	hci_LE_add_white_list(NULL, &hci_controller, sensor);
	hci_LE_rm_white_list(&test4, &hci_controller, sensor);
	hci_LE_add_white_list(&test3, &hci_controller, sensor);
	display_hci_socket_list(hci_controller.sockets_list);

	char * rssi_values;
	rssi_values = hci_LE_get_RSSI(&test5, &hci_controller, NULL, NULL, 20, 0x00, 0x20, 0x10, 0x00, 0x01);
	fprintf(stderr, "%s\n", rssi_values);
	free(rssi_values);
	
	hci_socket_t *head = (hci_socket_t *)(hci_controller.sockets_list->val);
	fprintf(stderr, "%i, %i \n\n", head->dev_id, head->sock);
	hci_close_controller(&hci_controller);
	display_hci_socket_list(hci_controller.sockets_list);
	bt_destroy_device_table();
}

