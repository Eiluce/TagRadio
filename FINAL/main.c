#include "l2cap_client.h"
#include "hci_controler.h"
#include "hci_socket.h"
#include "matrice.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/poll.h>
#include <pthread.h>

#define NB_LIGNES 7
#define NB_COLONNES 7

#define CLIENT_GET_RSSI 1
#define CLIENT_CLOSE_CONNECTION 2

// METTRE BT_DEVICES PARTOUT ?
static const char sensorAdd[18] = "1C:BA:8C:20:E9:1E";
static const char btControllerAdd[18] = "00:1A:7D:DA:71:0D"; //Pearl
static const char server1[18] = "00:02:72:CD:29:60"; // Belkin 1
static const char server2[18] = "00:02:72:CD:29:66"; // Belkin 2
static const char server3[18] = "00:02:72:CD:29:67"; // Belkin 3

static bdaddr_t controllerAdd;
static bdaddr_t server1Add;
static bdaddr_t server2Add;
static bdaddr_t server3Add;

static pthread_mutex_t mutexMatrice;

struct routine_data_t {
	int16_t timeout;
	hci_socket_t *hci_socket;
	bt_device_t sensor;
	l2cap_client_t *client;
	uint8_t num_captor;
	uint8_t num_row;
	uint8_t num_col;
	struct Matrice *matrice;
};

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

static void *get_rssi_thread_routine(void *data) {
	struct routine_data_t *routine_data = (struct routine_data_t *)data;

	uint8_t i = routine_data->num_row;
	uint8_t j = routine_data->num_col;
	uint8_t num_captor = routine_data->num_captor;
	int16_t timeout = routine_data->timeout;
	hci_socket_t *hci_socket = routine_data->hci_socket; 
	bt_device_t sensor = routine_data->sensor;
	struct Matrice *matrice = routine_data->matrice;
	l2cap_client_t *client = routine_data->client;

	if (client) {
		l2cap_client_send(client, 4000, CLIENT_GET_RSSI);
		if (client->buffer) { 
			pthread_mutex_lock(&mutexMatrice);
			insertVal(matrice, i,j, num_captor, client->buffer);
			pthread_mutex_unlock(&mutexMatrice);
		}
	} else {
		char * rssi_values;
		rssi_values = hci_LE_get_RSSI(hci_socket, NULL, NULL, 4, 0x00, 0x20, 0x10, 0x00, 0x01);
		fprintf(stderr, "%s\n", rssi_values);
		if (rssi_values) {
			pthread_mutex_lock(&mutexMatrice);
			insertVal(matrice, i, j, 3, rssi_values);
			pthread_mutex_unlock(&mutexMatrice);
		}
	}

	pthread_exit((void *)0);

}

int main(int arc, char**argv) {

	struct Matrice *matrice = CreateMatrice(NB_LIGNES,NB_COLONNES);
	pthread_mutex_init(&mutexMatrice, NULL);

	str2ba(btControllerAdd, &controllerAdd); 
	str2ba(server1, &server1Add); 
	str2ba(server2, &server2Add); 
	str2ba(server3, &server3Add); 

	bt_device_t sensor;
	str2ba(sensorAdd, &(sensor.mac));
	sensor.add_type = PUBLIC_DEVICE_ADDRESS;
	strcpy(sensor.custom_name, "SENSOR TAG");
	
	hci_socket_t hci_socket = open_hci_socket(&controllerAdd);
	hci_LE_clear_white_list(&hci_socket);
	hci_LE_add_white_list(&hci_socket, sensor);
	
	// Création des trois clients :
	l2cap_client_t clients[3] = {0};
	l2cap_client_create(&clients[0], &server1Add, 0x1001, 500, NULL, &(send_req_func));
	l2cap_client_create(&clients[1], &server2Add, 0x1001, 500, NULL, &(send_req_func)); 
	l2cap_client_create(&clients[2], &server3Add, 0x1001, 500, NULL, &(send_req_func));


	if (l2cap_client_connect(&clients[0]) != 0) {	
		perror("client_connect : unable to connect client 1");
	}
	if (l2cap_client_connect(&clients[1]) != 0) {	
		perror("client_connect : unable to connect client 2");
	}
	if (l2cap_client_connect(&clients[2]) != 0) {	
		perror("client_connect : unable to connect client 3");
	}

	pthread_t clients_threads[4];
	struct routine_data_t routine_data[4];
	for (uint8_t k = 0; k < 4; k++) {
		routine_data[k].timeout = 4500;
		routine_data[k].num_captor = k;
		routine_data[k].hci_socket = &hci_socket;
		routine_data[k].sensor = sensor;
		if (k < 3) {
			routine_data[k].client = &(clients[k]);
		} else {
			routine_data[k].client = NULL;
		}
		routine_data[k].matrice = matrice;
	}
	for (uint8_t i = 0; i < NB_LIGNES; i++) {
		for (uint8_t j = 0; j < NB_COLONNES; j++) {
			for (uint8_t k = 0; k < 4; k++) {
				routine_data[k].num_row = i;
				routine_data[k].num_col = j;
				pthread_create(&(clients_threads[k]), NULL, 
					       &(get_rssi_thread_routine),
					       (void *)&routine_data[k]);
			}
			for (uint8_t k = 0; k < 4; k ++) {
				pthread_join(clients_threads[k], NULL);
			}
		}
	}

	// Fermeture des clients :
	// Envoie des demandes de fin de connexion :
	l2cap_client_send(&clients[0], 3000, CLIENT_CLOSE_CONNECTION);
	l2cap_client_send(&clients[1], 3000, CLIENT_CLOSE_CONNECTION);
	l2cap_client_send(&clients[2], 3000, CLIENT_CLOSE_CONNECTION);

	// Destruction des clients :
	l2cap_client_close(&clients[0]);
	l2cap_client_close(&clients[1]);
	l2cap_client_close(&clients[2]);

	afficherMatrice(matrice);
	pthread_mutex_destroy(&mutexMatrice);

} 
