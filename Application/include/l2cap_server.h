#ifndef __L2CAP_SERVER__
#define __L2CAP_SERVER__

/* Package "l2cap_server".
 * Description : this package defines the main useful functions to create and use
 a l2cap-based server.
 * Author : FabLab ENSIMAG team : BERTAULD Thomas, BIANCHERI Cedric, BRELOT Lucie, FONT Ludovic.
 * Last modification : 23/05/2014.
 */

#include "l2cap_socket.h"
#include <bluetooth/bluetooth.h>
#include <stdint.h>

// Universal (i.e protocol independant) message to send to the server for it to stop the communication.
#define L2CAP_SERVER_UNIVERSAL_STOP "STOP"

/* A NOTER : les ports L2CAP dynamiques vont de 4097 (0x1001) à 32765, mais en ne prenant que les numéros impairs !
 */

/* --------------
   - STRUCTURES -
   --------------
*/

/* Structure representing a client treated by the server. */
typedef struct client_t { // ASSOCIER UN ID PERSO A UN CLIENT ? => MAINTENIR TABLE IDs ??
	l2cap_socket_t server_sock; // Socket côté serveur.
	int8_t conn_id; // Id de la connexion établie via la précédente socket.
	struct sockaddr_l2 rem_addr; // Adresse du device client.
	char *buffer; // Buffer associé à cette connexion.
} client_t;


/* L2CAP server structure : */
typedef struct l2cap_server_t {
	char launched; // Vrai si au moins un serveur tourne.
	uint16_t buffer_length; // Taille des buffers.
	uint8_t max_clients; // Nombre max de clients possibles.
	client_t *clients; // Tableau des différents clients traités par le server.
	void (*treat_buffer)(struct l2cap_server_t server, uint8_t num_client); // Function called on the buffer when a message is received.
	void (*send_response)(struct l2cap_server_t server, uint8_t num_client, uint8_t res_type); // Function called when we need to send a response to the client.
} l2cap_server_t;

//------------------------------------------------------------------------------------

/* --------------
   - PROTOTYPES -
   --------------
*/

/* Function "l2cap_server_create" :
   Creates and initializes a new server by using the "server_add" reference and the port "port".
   When it is created, it is possible to redefine the default functions used
   to communicate with the client by passing their references as "treat_buffer_func"
   and "send_response_func". If one (or the two) of these references is NULL, we keep
   the default functions :
   * For treat_buffer it justs prints the trace of the received message on the standard output.
   * For send_response it prints "Response echo" on the standard output.
   You may want to define your own communication protocol by associating response numbers to
   responses and use a "switch" in your own "send_request" function to send the correct message
   regarding the given "res_type" parameter.
   The fields "server" and "adapter" have to be non-NULL and the field "buffer_length" should
   be greater than zero.
   On success, this function returns the number of opened sockets on the create server. This
   number should be lower than "max_clients".
   Please note that since we have to assign different sockets to each client, we have to use a
   different port for each client. And since the l2cap ports go from  4097 (0x1001) to 32765
   but using only odd numbers, the "port" has to be an odd value at which we add 2 for each
   client's slot.
*/
extern int8_t l2cap_server_create(l2cap_server_t *server, bdaddr_t *adapter, uint16_t port, 
				  uint8_t max_clients, uint16_t buffer_length, 
				  void (*treat_buffer_func)(l2cap_server_t server, uint8_t num_client),
				  void (*send_response_func)(l2cap_server_t server, uint8_t num_client, uint8_t res_type)); 

/* Function "l2cap_server_launch" :
   Starts the server. That is to say that all of its opened socket are waiting for a connection.
   Please note that the server is multi-thread and so all of the sockets will be in the "accept" state
   at the same time and we will be able to treat simultaneously all of the clients.
   The field "timeout" represents the timeout value (in ms) for the server to wait for a request
   from a client on each socket. If it is set to "-1" then the server will never use the timeout
   value.
   The field "max_req" represents the maximum of requests a server can handle for each client.
   On success, a non-negative value should be returned.
   The field "server" has to be a valid reference on a previously created server.
*/
extern int8_t l2cap_server_launch(l2cap_server_t *server, int16_t timeout, uint16_t max_req);

/* Function "l2cap_server_close" :
   Closes and destroys a given server.
   The field "server" has to be a valid reference on a previously created server.
*/
extern void l2cap_server_close(l2cap_server_t * server);

// Fonction pour fermer un client particulier ? => Stocker les threads en global ?

#endif 
