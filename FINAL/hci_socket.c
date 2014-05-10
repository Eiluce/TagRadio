#include "hci_socket.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <bluetooth/hci_lib.h>


static list_t *hci_socket_list = NULL;
// ATTENTION : CE POINTEUR DOIT TOUJOURS REFERENCER LA TETE DE LA LISTE !!!

// If controler == NULL, we take the first present available BT adaptator.
hci_socket_t open_hci_socket(bdaddr_t *controler) {
	hci_socket_t result;
	memset(&result, 0, sizeof(result));

	if (controler) {
		char add[18];
		ba2str(controler, add);
		result.dev_id = hci_devid(add);
	} else {
		result.dev_id = hci_get_route(NULL);
	}
			
	if (result.dev_id < 0) {
		perror("opening hci socket");
		result.sock = -1;
		return result;
	}

	result.sock = hci_open_dev(result.dev_id);
	if (result.sock < 0) {
		perror("opening hci socket");
		result.sock = -1;
		return result;	
	}

	list_push(&hci_socket_list, &result, sizeof(hci_socket_t));

	return result;		
}

//------------------------------------------------------------------------------------

void close_hci_socket(hci_socket_t *hci_socket) {
	if (hci_socket->sock < 0) {
		fprintf(stderr, "close_hci_socket warning : already closed socket.\n");
		return;
	}
	close(hci_socket->sock);
	hci_socket_t *listed_socket = list_search(&hci_socket_list,
						  (const void *)hci_socket,
						  sizeof(hci_socket_t));
	hci_socket->sock = -1;
	if (listed_socket == NULL) {
		fprintf(stderr, "close_hci_scoket warning : this socket wasn't referenced yet.\n");
		return;
	}
	free(listed_socket);
	return;
}

//------------------------------------------------------------------------------------

list_t *get_hci_socket_list(void) {
	return hci_socket_list;
}

//------------------------------------------------------------------------------------

void close_all_hci_sockets(void) {
	if (hci_socket_list == NULL) {
		fprintf(stderr, "close_all_hci_sockets error : no socket to close.\n");
		return;
	}
       
	hci_socket_t *hci_socket = NULL;

	while (hci_socket_list != NULL) {
		hci_socket = (hci_socket_t *)list_pop(&hci_socket_list);
		if (hci_socket->sock >= 0) {
			close(hci_socket->sock);
		}
		free(hci_socket);
	}
}

//------------------------------------------------------------------------------------

int8_t get_hci_socket_filter(hci_socket_t hci_socket, struct hci_filter *old_flt) {
	int8_t err_code;
	socklen_t old_flt_len = sizeof(struct hci_filter);
	err_code = getsockopt(hci_socket.sock, SOL_HCI, HCI_FILTER, (void *)old_flt, &old_flt_len);
	if (err_code < 0) {
		perror("get_hci_socket_filter : cannot save the old filter");
		memset((void *)old_flt, 0, sizeof(struct hci_filter));
	}
	return err_code;
}	

//------------------------------------------------------------------------------------

int8_t set_hci_socket_filter(hci_socket_t hci_socket, struct hci_filter *flt) {
	int8_t err_code = setsockopt(hci_socket.sock, SOL_HCI, HCI_FILTER, (void *)flt,
				     sizeof(struct hci_filter));
	if (err_code < 0) {
		perror("set_hci_socket_filter : can't set HCI filter");
	}
	return err_code;
}

//------------------------------------------------------------------------------------

void display_hci_socket_list(void) {
	list_t *tmp = hci_socket_list;
	hci_socket_t val;
	fprintf(stdout, "\nState of the current opened sockets list :\n");
	while (tmp != NULL) {
		val = *((hci_socket_t *)tmp->val);
		fprintf(stdout, "  -> dev_id : %u | socket : %u \n", val.dev_id, val.sock);
		tmp = tmp->next;
	}
	fprintf(stdout, "\n");
}

//------------------------------------------------------------------------------------

/*int main(int argc, char **argv) {
	hci_socket_t test = open_hci_socket(NULL);
	hci_socket_t test2 = open_hci_socket(NULL);
	hci_socket_t test3 = open_hci_socket(NULL);
	hci_socket_t test4 = open_hci_socket(NULL);
	hci_socket_t test5 = open_hci_socket(NULL);

	fprintf(stderr, "%i %i \n", test.sock, test.dev_id);
	close_hci_socket(&test);
	fprintf(stderr, "%i %i \n", test.sock, test.dev_id);

	close_all_sockets();
	}*/
