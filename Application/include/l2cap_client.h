#ifndef __L2CAP_CLIENT__
#define __L2CAP_CLIENT__

/* Package "l2cap_client".
 * Description : this package defines the main useful functions to create and use
 a l2cap-based client.
 * Author : FabLab ENSIMAG team : BERTAULD Thomas, BIANCHERI Cedric, BRELOT Lucie, FONT Ludovic.
 * Last modification : 23/05/2014.
 */

#include "l2cap_socket.h"
#include <bluetooth/bluetooth.h>
#include <stdint.h>

/* --------------
   - STRUCTURES -
   --------------
*/

/* L2CAP client structure : */
typedef struct l2cap_client_t {
	char *buffer; // Buffer containing the messages left to read.
	uint16_t buffer_length; // Length of the previous buffer.
	l2cap_socket_t l2cap_socket; // L2CAP socket used for the communication with the server.
	char connected; // Boolean statuts indicating if the client is connected.

	void (*treat_buffer)(struct l2cap_client_t client); // Function called on the buffer when a message is received.
	void (*send_request)(struct l2cap_client_t client, uint8_t req_type); // Function called when we need to send a message to the server.
} l2cap_client_t;

//------------------------------------------------------------------------------------

/* --------------
   - PROTOTYPES -
   --------------
*/

/* Function "l2cap_client_create" :
   Creates and initializes a new client to be connected on the server identified
   by the "server_add" reference and the port "port".
   When it is created, it is possible to redefine the default functions used
   to communicate with the server by passing their references as "treat_buffer_func"
   and "send_request_func". If one (or the two) of these references is NULL, we keep
   the default functions :
   * For treat_buffer it justs prints the trace of the received message on the standard output.
   * For send_request it prints "Request echo" on the standard output.
   You may want to define your own communication protocol by associating request numbers to
   requests and use a "switch" in your own "send_request" function to send the correct message
   regarding the given "req_type" parameter.
   The fields "client" and "server_add" have to be non-NULL and the field "buffer_length" should
   be greater than zero.
   On success, this function returns a non-negative value.
*/
extern int8_t l2cap_client_create(l2cap_client_t *client, bdaddr_t *server_add, uint16_t port, 
			   uint16_t buffer_length, 
			   void (*treat_buffer_func)(l2cap_client_t client),
			   void (*send_request_func)(l2cap_client_t client, uint8_t req_type));

/* Function "l2cap_client_connect" :
   Tries to connect the client referenced by "client" to its bounded server.
   The field "client" has to be a valid reference on an already created client.
   On success, this function returns a non-negative value.
*/
extern int8_t l2cap_client_connect(l2cap_client_t *client);

/* Function "l2cap_client_send" :
   Sends the protocol defined message corresponding to the request number "req_type" to the server.
   If the message has correctly been sent, this function will wait "timeout" ms to receive
   an answer from the server.
   The field "client" has to be a valid reference on an already connected client.
   On success, this function returns a non-negative value.
*/
extern int8_t l2cap_client_send(l2cap_client_t *client, int16_t timeout, uint8_t req_type);

/* Function "l2cap_client_close" :
   Closes and destroys a given client.
   The field "client" has to be a valid reference on an already created client.
*/
extern void l2cap_client_close(l2cap_client_t *client);


#endif 
