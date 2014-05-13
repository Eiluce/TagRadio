#include "l2cap_socket.h"
#include "trace.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

static list_t *l2cap_socket_list = NULL;
// ATTENTION : CE POINTEUR DOIT TOUJOURS REFERENCER LA TETE DE LA LISTE !!!

// If controler == NULL, we take the first present available BT adaptator.
l2cap_socket_t open_l2cap_socket(bdaddr_t *adapter, uint16_t port, char to_bind) {
	l2cap_socket_t result;
	memset(&result, 0, sizeof(result));
  	(result.sockaddr).l2_family = AF_BLUETOOTH;
	(result.sockaddr).l2_psm = htobs(port);
	if (!adapter) {
		adapter = BDADDR_ANY;
	}
	(result.sockaddr).l2_bdaddr = *adapter;
	//result.l2_cid ???

	result.sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (result.sock < 0) {
		perror("opening socket");
		result.sock = -1;
		return result;
	}

	if (to_bind) {
		if (bind(result.sock, (const struct sockaddr *)&(result.sockaddr), sizeof(result.sockaddr)) < 0) {
			perror("binding socket");
			close(result.sock);
			result.sock = -1;
			return result;
		}		
	}

	list_push(&l2cap_socket_list, &result, sizeof(l2cap_socket_t));

	return result;		
}

//------------------------------------------------------------------------------------

void close_l2cap_socket(l2cap_socket_t *l2cap_socket) {
	if (l2cap_socket->sock < 0) {
		print_trace(TRACE_WARNING, "close_l2cap_socket : already closed socket.\n");
		return;
	}
	close(l2cap_socket->sock);
	l2cap_socket_t *listed_socket = list_search(&l2cap_socket_list,
						  (const void *)l2cap_socket,
						  sizeof(l2cap_socket_t));
	l2cap_socket->sock = -1;
	if (listed_socket == NULL) {
		print_trace(TRACE_WARNING, "close_l2cap_scoket : this socket wasn't referenced yet.\n");
		return;
	}
	free(listed_socket);
	return;
}

//------------------------------------------------------------------------------------

list_t *get_l2cap_socket_list(void) {
	return l2cap_socket_list;
}

//------------------------------------------------------------------------------------

void close_all_l2cap_sockets(void) {
	if (l2cap_socket_list == NULL) {
		print_trace(TRACE_ERROR, "close_all_l2cap_sockets : no socket to close.\n");
		return;
	}
       
	l2cap_socket_t *l2cap_socket = NULL;

	while (l2cap_socket_list != NULL) {
		l2cap_socket = (l2cap_socket_t *)list_pop(&l2cap_socket_list);
		if (l2cap_socket->sock >= 0) {
			close(l2cap_socket->sock);
		}
		free(l2cap_socket);
	}
}

//------------------------------------------------------------------------------------

void display_l2cap_socket_list(void) {
	list_t *tmp = l2cap_socket_list;
	l2cap_socket_t val;
	fprintf(stdout, "\nState of the current opened sockets list :\n");
	while (tmp != NULL) {
		val = *((l2cap_socket_t *)tmp->val);
		char add[18];
		ba2str((const bdaddr_t *)&((val.sockaddr).l2_bdaddr), add);
		fprintf(stdout, "  -> device : %s | socket : %u \n", add, val.sock);
		tmp = tmp->next;
	}
	fprintf(stdout, "\n");
}

//------------------------------------------------------------------------------------

/*int main(int argc, char **argv) {
	l2cap_socket_t test = open_l2cap_socket(NULL);
	l2cap_socket_t test2 = open_l2cap_socket(NULL);
	l2cap_socket_t test3 = open_l2cap_socket(NULL);
	l2cap_socket_t test4 = open_l2cap_socket(NULL);
	l2cap_socket_t test5 = open_l2cap_socket(NULL);

	fprintf(stderr, "%i %i \n", test.sock, test.dev_id);
	close_l2cap_socket(&test);
	fprintf(stderr, "%i %i \n", test.sock, test.dev_id);

	close_all_sockets();
	}*/

