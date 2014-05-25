#include "l2cap_client.h"
#include "hci_controller.h"
#include "hci_socket.h"
#include "matrice.h"
#include "simulation_data.h"
#include "trace.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/poll.h>
#include <pthread.h>
#include <time.h>

#define CLIENT_GET_RSSI 1
#define CLIENT_CLOSE_CONNECTION 2

static const char sensorAdd[18] = "1C:BA:8C:20:E9:1E";
static const char btControllerAdd[18] = "00:1A:7D:DA:71:0D"; //Pearl
static const char server1Add[18] = "00:02:72:CD:29:60"; // Belkin 1
static const char server2Add[18] = "00:02:72:CD:29:66"; // Belkin 2
static const char server3Add[18] = "00:02:72:CD:29:67"; // Belkin 3

static bt_device_t server1;
static bt_device_t server2;
static bt_device_t server3;
static bt_device_t sensor;

static pthread_mutex_t mutexMatrice;

char * mesures[NUM_CAPTORS] = {0};

struct routine_data_t {
	int16_t timeout;
	hci_controller_t *hci_controller;
	bt_device_t sensor;
	l2cap_client_t *client;
	uint8_t num_captor;
	uint8_t num_row;
	uint8_t num_col;
	struct Matrix *matrice;
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
	char *status = malloc(sizeof(char));
	memset(status, 0, sizeof(status));

	uint8_t i = routine_data->num_row;
	uint8_t j = routine_data->num_col;
	uint8_t num_captor = routine_data->num_captor;
	int16_t timeout = routine_data->timeout;
	hci_controller_t *hci_controller = routine_data->hci_controller; 
	bt_device_t sensor = routine_data->sensor;
	struct Matrix *matrice = routine_data->matrice;
	l2cap_client_t *client = routine_data->client;

	if (client) {
		if (!client->connected) {
			if (l2cap_client_connect(client) != 0) {	
				perror("client_connect : unable to connect client.");
				*status = 1;
				pthread_exit((void *)status);
			}
		}
		l2cap_client_send(client, 8000, CLIENT_GET_RSSI);
		if (client->buffer && (strcmp(client->buffer, "") != 0)) { 
			pthread_mutex_lock(&mutexMatrice);
			insert_val(matrice, i,j, num_captor, client->buffer);
			pthread_mutex_unlock(&mutexMatrice);
		} else {
			*status = 1;
		}
	} else {
		char * rssi_values;
		rssi_values = hci_LE_get_RSSI(NULL, hci_controller, NULL, NULL, NUM_MEASURES, 0x00, SCAN_INTERVAL, SCAN_WINDOW, 0x00, 0x01);
		fprintf(stderr, "%s\n", rssi_values);
		if (rssi_values && (strcmp(rssi_values, "") != 0)) {
			pthread_mutex_lock(&mutexMatrice);
			insert_val(matrice, i, j, NUM_CAPTORS-1, rssi_values);
			pthread_mutex_unlock(&mutexMatrice);
		} else {
			*status = 1;
		}
		free(rssi_values);

	}

	pthread_exit((void *)status);
}

int main(int arc, char**argv) {


	if (arc != 2) {
		print_trace(TRACE_ERROR, "Veuillez indiquer un fichier où stocker la matrice de calibration.\n");
		return EXIT_FAILURE;
	} 
	
	struct Matrix *matrice = create_matrix(NB_LIGNES,NB_COLONNES, NUM_CAPTORS);
	pthread_mutex_init(&mutexMatrice, NULL);

	for (uint8_t i = 0; i < NUM_CAPTORS; i++) {
		mesures[i] = calloc(200, sizeof(char));
	}

	bdaddr_t controllerAdd, server1Mac, server2Mac, server3Mac, sensorMac;
	str2ba(btControllerAdd, &controllerAdd); 
	str2ba(server1Add, &server1Mac); 
	str2ba(server2Add, &server2Mac); 
	str2ba(server3Add, &server3Mac); 
	str2ba(sensorAdd, &sensorMac);

	hci_controller_t hci_controller = hci_open_controller(&controllerAdd, "MAIN_SERVER");
	sensor = bt_device_create(sensorMac, PUBLIC_DEVICE_ADDRESS, NULL, "SENSOR_TAG");
	server1 = bt_device_create(server1Mac, PUBLIC_DEVICE_ADDRESS, NULL, "SERVER_1");
	server2 = bt_device_create(server2Mac, PUBLIC_DEVICE_ADDRESS, NULL, "SERVER_2");
	server3 = bt_device_create(server3Mac, PUBLIC_DEVICE_ADDRESS, NULL, "SERVER_3");

	hci_LE_clear_white_list(NULL, &hci_controller);
	hci_LE_add_white_list(NULL, &hci_controller, sensor);
	
	// Création des trois clients :
	l2cap_client_t clients[NUM_CAPTORS-1] = {0};
	l2cap_client_create(&clients[0], &server1Mac, 0x1001, 500, NULL, &(send_req_func));
	l2cap_client_create(&clients[1], &server2Mac, 0x1001, 500, NULL, &(send_req_func)); 
	l2cap_client_create(&clients[2], &server3Mac, 0x1001, 500, NULL, &(send_req_func));


	if (l2cap_client_connect(&clients[0]) != 0) {	
		perror("client_connect : unable to connect client 1");
		return EXIT_FAILURE;
	}
	if (l2cap_client_connect(&clients[1]) != 0) {	
		perror("client_connect : unable to connect client 2");
		return EXIT_FAILURE;
	}
	if (l2cap_client_connect(&clients[2]) != 0) {	
		perror("client_connect : unable to connect client 3");
		return EXIT_FAILURE;
	}

	fprintf(stderr, "\n-------------------\n");
	fprintf(stderr, "----Calibration----\n");
	fprintf(stderr, "-------------------\n");

	pthread_t clients_threads[NUM_CAPTORS];
	struct routine_data_t routine_data[NUM_CAPTORS];
	for (uint8_t k = 0; k < NUM_CAPTORS; k++) {
		routine_data[k].timeout = 4500;
		routine_data[k].num_captor = k;
		routine_data[k].hci_controller = &hci_controller;
		routine_data[k].sensor = sensor;
		if (k < NUM_CAPTORS-1) {
			routine_data[k].client = &(clients[k]);
		} else {
			routine_data[k].client = NULL;
		}
		routine_data[k].matrice = matrice;
	}
	char command[20] = {0};
	char *status;
	char retry = 0;
	for (uint8_t i = 0; i < NB_LIGNES; i++) {
		for (uint8_t j = 0; j < NB_COLONNES; j++) {
			scan:
			retry = 0;
			fprintf(stdout, "Position courante : %i, %i\n", i, j); 
			scanf("%s", command);
			for (uint8_t k = 0; k < NUM_CAPTORS; k++) {
				routine_data[k].num_row = i;
				routine_data[k].num_col = j;
				pthread_create(&(clients_threads[k]), NULL, 
					       &(get_rssi_thread_routine),
					       (void *)&routine_data[k]);
			}
			for (uint8_t k = 0; k < 4; k ++) {
				pthread_join(clients_threads[k], (void **)&status);
				retry = retry || *status;
			}
			if (retry) {
				print_trace(TRACE_WARNING, "Attention : impossible d'acquérir les mesures pour cette case, veuillez réessayer.\n");
				goto scan;
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

	display_matrix(matrice);
	save_matrix(argv[1], *matrice);
	pthread_mutex_destroy(&mutexMatrice);

	hci_close_controller(&hci_controller);
	display_hci_socket_list(hci_controller.sockets_list);
	bt_destroy_device_table();

} 
