#ifndef __L2CAP_SERVER__
#define __L2CAP_SERVER__

#include "l2cap_socket.h"
#include <bluetooth/bluetooth.h>
#include <stdint.h>

#define L2CAP_SERVER_UNIVERSAL_STOP "STOP"

/* A NOTER : les ports L2CAP dynamiques vont de 4097 (0x1001) à 32765, mais en ne prenant que les numéros impairs !
 */


// Structure représentant un client traité par le serveur.
typedef struct client_t { // ASSOCIER UN ID PERSO A UN CLIENT ? => MAINTENIR TABLE IDs ??
	l2cap_socket_t server_sock; // Socket côté serveur.
	int8_t conn_id; // Id de la connexion établie via la précédente socket.
	struct sockaddr_l2 rem_addr; // Adresse du device client.
	char *buffer; // Buffer associé à cette connexion.
} client_t;


typedef struct l2cap_server_t {
	char launched; // Vrai si au moins un serveur tourne.
	uint16_t buffer_length; // Taille des buffers.
	uint8_t max_clients; // Nombre max de clients possibles.
	client_t *clients;
	void (*treat_buffer)(struct l2cap_server_t server, uint8_t num_client);
	void (*send_response)(struct l2cap_server_t server, uint8_t num_client, uint8_t res_type);
} l2cap_server_t;

extern int8_t l2cap_server_create(l2cap_server_t *server, bdaddr_t *adapter, uint16_t port, 
				  uint8_t max_clients, uint16_t buffer_length, 
				  void (*treat_buffer_func)(l2cap_server_t server, uint8_t num_client),
				  void (*send_response_func)(l2cap_server_t server, uint8_t num_client, uint8_t res_type)); 
// Renvoie le nombre de sockets effectivement ouvertes (max : max_clients).

extern int8_t l2cap_server_launch(l2cap_server_t *server, int16_t timeout, uint16_t max_req);

extern void l2cap_server_close(l2cap_server_t * server);

// Fonction pour fermer un client particulier ? => Stocker les threads en global ?

#endif 
