#include "hci_socket.h"
#include "trace.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <bluetooth/hci_lib.h>


// If controler == NULL, we take the first present available BT adaptator.
hci_socket_t open_hci_socket(bdaddr_t *controller) {
	hci_socket_t result;
	memset(&result, 0, sizeof(result));

	if (controller) {
		char add[18];
		ba2str(controller, add);
		result.dev_id = hci_devid(add);
	} else {
		result.dev_id = hci_get_route(NULL);
	}
			
	if (result.dev_id < 0) {
		perror("open_hci_socket");
		result.sock = -1;
		return result;
	}

	result.sock = hci_open_dev(result.dev_id);
	if (result.sock < 0) {
		perror("open_hci_socket");
		result.sock = -1;
		return result;	
	}

	return result;		
}

//------------------------------------------------------------------------------------

void close_hci_socket(hci_socket_t *hci_socket) {
	if (hci_socket->sock < 0) {
		print_trace(TRACE_WARNING, "close_hci_socket : already closed socket.\n");
		return;
	}
	hci_close_dev(hci_socket->sock);
	hci_socket->sock = -1;

	return;
}

//------------------------------------------------------------------------------------

void close_all_hci_sockets(list_t **hci_socket_list) {
	if (hci_socket_list == NULL) {
		print_trace(TRACE_ERROR, "close_all_hci_sockets : invalid reference.\n");
		return;
	}

	if (*hci_socket_list == NULL) {
		print_trace(TRACE_WARNING, "close_all_hci_sockets : no socket to close.\n");
		return;
	}
       
	hci_socket_t *hci_socket = NULL;

	while (*hci_socket_list != NULL) {
		hci_socket = (hci_socket_t *)list_pop(hci_socket_list);
		if (hci_socket->sock >= 0) {
			hci_close_dev(hci_socket->sock);
		} else {
			print_trace(TRACE_WARNING, "close_all_hci_sockets : already closed socket.\n");
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

void display_hci_socket_list(list_t *hci_socket_list) {
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

