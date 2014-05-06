#include "l2cap_client.h"
#include "hci_controler.h"
#include "hci_socket.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/poll.h>

#define CLIENT_GET_RSSI 1
#define CLIENT_CLOSE_CONNECTION 2

static char sensorAdd[18] = "1C:BA:8C:20:E9:1E";
static char btControllerAdd[18] = "00:1A:7D:DA:71:0D"; //Pearl
static char server1[18] = "00:02:72:CD:29:67"; // Belkin
static char server2[18] = "00:02:72:CD:29:67";
static char server3[18] = "00:02:72:CD:29:67";

static bdaddr_t controllerAdd;
static bdaddr_t server1Add;
static bdaddr_t server2Add;
static bdaddr_t server3Add;

static void send_req_func(l2cap_client_t client, uint8_t req_type) {
	int8_t sock = client.l2cap_socket.sock;
	char req[25];
	if (sock < 0) {
		return;
	}
	switch (req_type) {
	case CLIENT_GET_RSSI :
		strcpy(req, "GET_RSSI");
		write(sock, req, sizeof(req));
		break;
	case CLIENT_CLOSE_CONNECTION :
		strcpy(req , "CLOSE");
		write(sock, req, sizeof(req));
		break;
	default:
		fprintf(stderr, "client_send_request error : unknown request type.\n");
		break;
	}
}


int main(int arc, char**argv) {

	bt_device_t sensor;
	str2ba(btControllerAdd, &controllerAdd); 
	str2ba(sensorAdd, &(sensor.mac));
	str2ba(server1, &server1Add); 
	str2ba(server2, &server2Add); 
	str2ba(server3, &server3Add); 
	sensor.add_type = PUBLIC_DEVICE_ADDRESS;
	strcpy(sensor.custom_name, "SENSOR TAG");

	hci_socket_t hci_socket;
	hci_socket = open_hci_socket(&controllerAdd);
	hci_LE_clear_white_list(&hci_socket);
	hci_LE_add_white_list(&hci_socket, sensor);

	// Création des trois clients :
	l2cap_client_t client1, client2, client3;
	l2cap_client_create(&client1, &server1Add, 0x1001, 500, NULL, &(send_req_func));
	l2cap_client_create(&client2, &server2Add, 0x1001+2, 500, NULL, &(send_req_func)); //+2 et +4 car même serveur...
	l2cap_client_create(&client3, &server3Add, 0x1001+4, 500, NULL, &(send_req_func));


	if (l2cap_client_connect(&client1) != 0) {	
		perror("client_connect : unable to connect client1");
	}
	if (l2cap_client_connect(&client2) != 0) {	
		perror("client_connect : unable to connect client2");
	}
	if (l2cap_client_connect(&client3) != 0) {	
		perror("client_connect : unable to connect client3");
	}

	// Envoie des demandes :
	l2cap_client_send(&client1, 3000, CLIENT_GET_RSSI);
	l2cap_client_send(&client2, 3000, CLIENT_GET_RSSI);
	l2cap_client_send(&client2, 3000, CLIENT_GET_RSSI);
	l2cap_client_send(&client3, 3000, CLIENT_GET_RSSI);
	l2cap_client_send(&client1, 3000, CLIENT_GET_RSSI);

	// Fermeture des clients :
	// Envoie des demandes de fin de connexion :
	l2cap_client_send(&client1, 3000, CLIENT_CLOSE_CONNECTION);
	l2cap_client_send(&client2, 3000, CLIENT_CLOSE_CONNECTION);
	l2cap_client_send(&client3, 3000, CLIENT_CLOSE_CONNECTION);

	// Destruction des clients :
	l2cap_client_close(&client1);
	l2cap_client_close(&client2);
	l2cap_client_close(&client3);

	// Récupération de ses données :
	char * rssi_values;
	rssi_values = hci_LE_get_RSSI(&hci_socket, NULL, NULL, 4, 0x00, 0x20, 0x10, 0x00, 0x01);
	fprintf(stderr, "%s\n", rssi_values);

	close_hci_socket(&hci_socket);
} 
