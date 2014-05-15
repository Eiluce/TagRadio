#include "l2cap_server.h"
#include "hci_controller.h"
#include "hci_socket.h"
#include "simulation_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_SEND_RSSI 1

static char sensorAdd[18] = "1C:BA:8C:20:E9:1E";
static char btControllerAdd[18] = "00:02:72:CD:29:66"; //Belkin 2
static bdaddr_t controllerAdd;

static bt_device_t sensor;
static hci_controller_t hci_controller; // Peut être stocké en local car on a qu'UN SEUL client.

static void treat_buffer_default_func(l2cap_server_t server, uint8_t num_client) {
	fprintf(stderr, "Buffer trace : %s\n", server.clients[num_client].buffer);
	if (strcmp(server.clients[num_client].buffer, "GET_RSSI") == 0) {
		server.send_response(server, num_client, SERVER_SEND_RSSI);
	}
}

static void send_response_default_func(l2cap_server_t server, uint8_t num_client, uint8_t res_type) {
	uint8_t i = num_client;

	char *rssi_values = NULL;
	uint8_t *fd = malloc(sizeof(uint8_t));
	*fd=STDERR_FILENO;

	switch(res_type) {
	case SERVER_SEND_RSSI:		
		rssi_values = hci_LE_get_RSSI(NULL, &hci_controller, fd, NULL, NUM_MEASURES, 0x00, SCAN_INTERVAL, SCAN_WINDOW, 0x00, 0x01);
		if (rssi_values) {
			if (write(server.clients[i].conn_id, rssi_values, strlen(rssi_values)) != strlen(rssi_values)) {
				fprintf(stderr, "server_send_response warning : an error occured when sending response to client.\n");
			}
			free(rssi_values);
		}
		break;
	default:
		fprintf(stderr, "Unknown command.\n");
		break;
	}
	free(fd);
}

int main(int arc, char**argv) {

	// Mise en place des paramètres globaux :
	bdaddr_t sensorMac;
	str2ba(btControllerAdd, &controllerAdd); 
	str2ba(sensorAdd, &sensorMac);

	hci_controller = hci_open_controller(&controllerAdd, "SERVER_2");
	sensor = bt_device_create(sensorMac, PUBLIC_DEVICE_ADDRESS, NULL, "SENSOR_TAG");

	bt_device_display(hci_controller.device);
	display_hci_socket_list(hci_controller.sockets_list);

	hci_LE_clear_white_list(NULL, &hci_controller);
	hci_LE_add_white_list(NULL, &hci_controller, sensor); 

	// Création et lancement du serveur :
	l2cap_server_t server;
	
	l2cap_server_create(&server, &controllerAdd, 0x1001, 1, 500, &treat_buffer_default_func, &send_response_default_func);
	while (strcmp(server.clients[0].buffer, "CLOSE") != 0) {
		l2cap_server_launch(&server, -1, 2000); // Timeout de -1 implique qu'on bloquera tant que rien ne se passe.
	}
	l2cap_server_close(&server);

	hci_close_controller(&hci_controller);
	display_hci_socket_list(hci_controller.sockets_list);
	bt_destroy_device_table();
} 
