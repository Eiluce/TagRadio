#include "hci_controler.h"
#include "hci_socket.h"
#include "list.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
	fprintf(stderr, "-------------------------\n");
	fprintf(stderr, "- OPENING SOCKETS TESTS -\n");
	fprintf(stderr, "-------------------------\n");
	hci_socket_t hci_socket = open_hci_socket(NULL);
	hci_socket_t hci_socket1 = open_hci_socket(NULL);
	display_hci_socket_list();


	/*	fprintf(stderr, "\n-------------------------\n");
	fprintf(stderr, "- SCANNING DEVICES TEST -\n");
	fprintf(stderr, "-------------------------\n");
	hci_device_table_t device_table = hci_scan_devices(&hci_socket, 5, 10, IREQ_CACHE_FLUSH);
	hci_device_table_display(device_table);
	free(device_table.device);

	fprintf(stderr, "\n-----------------------------\n");
	fprintf(stderr, "- GETING RSSI MEASURES TEST -\n");
	fprintf(stderr, "-----------------------------\n");
	hci_get_RSSI(NULL, NULL, 15, 20);*/

	fprintf(stderr, "\n---------------------------------\n");
	fprintf(stderr, "- GETING RSSI MEASURES TEST (LE)-\n");
	fprintf(stderr, "---------------------------------\n");
	bdaddr_t test;
	str2ba("1C:BA:8C:20:E9:1E", &test);
	hci_LE_clear_white_list(NULL);
	hci_LE_add_white_list(&hci_socket1, &test, 0x00); // 0x00 : PDA 
	hci_LE_get_RSSI(&hci_socket1, NULL, 10, 0x00, 0x10, 0x10, 0x00, 0x01); // LE DERNIER 0x01 indique qu'on ne prend que les reponses matchant la liste blanche.
	

	fprintf(stderr, "\n-------------------------\n");
	fprintf(stderr, "- CLOSING SOCKETS TESTS -\n");
	fprintf(stderr, "-------------------------\n");
	fprintf(stderr, "Closing the second opened socket. \n");
	close_hci_socket(&hci_socket1);
	display_hci_socket_list();

	fprintf(stderr, "Closing all of the remaining sockets.\n");
	close_all_sockets();
	display_hci_socket_list();

	return EXIT_SUCCESS;
}
