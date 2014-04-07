#include "hci_controler.h"
#include "hci_socket.h"
#include "hci_utils.h"
#include "list.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
	//fprintf(stderr, "**************%i %i %i************\n", atoi(argv[1]),atoi(argv[2]), atoi(argv[3]));
	uint16_t num_points, scan_interval, scan_window;
	if (argc < 4) {
		fprintf(stderr, "WARNING : not enough parameters given, the test program will use default values.\n");
		num_points = 200;
		scan_interval = 0x20;
		scan_window = 0x10;
	} else {
		num_points = ((uint16_t)atoi(argv[1]))%1000;
		scan_interval = ((uint16_t)atoi(argv[2]))%0x4000;
		scan_window = ((uint16_t)atoi(argv[3]))%0x4000;
	}		
		
	fprintf(stderr, "-------------------------\n");
	fprintf(stderr, "- OPENING SOCKETS TESTS -\n");
	fprintf(stderr, "-------------------------\n");
	hci_socket_t hci_socket = open_hci_socket(NULL);
	hci_socket_t hci_socket1 = open_hci_socket(NULL);
	display_hci_socket_list();


		fprintf(stderr, "\n-------------------------\n");
	fprintf(stderr, "- SCANNING DEVICES TEST -\n");
	fprintf(stderr, "-------------------------\n");
	hci_device_table_t device_table = hci_scan_devices(&hci_socket, 5, 10, IREQ_CACHE_FLUSH);
	hci_device_table_display(device_table);
	free(device_table.device);

	fprintf(stderr, "\n-----------------------------\n");
	fprintf(stderr, "- GETING RSSI MEASURES TEST -\n");
	fprintf(stderr, "-----------------------------\n");
	hci_get_RSSI(NULL, NULL, 15, 20);

	fprintf(stderr, "\n---------------------------------\n");
	fprintf(stderr, "- GETING RSSI MEASURES TEST (LE)-\n");
	fprintf(stderr, "---------------------------------\n");
	fprintf(stderr, "We will use the following parameters :\n");
	fprintf(stderr, " -> %u points. \n", num_points);
	fprintf(stderr, " -> %ums scan_interval. \n", scan_interval);
	fprintf(stderr, " -> %ums scan_window. \n\n", scan_window);

	hci_device_t test;
	str2ba("1C:BA:8C:20:E9:1E", &(test.mac));
	test.add_type = PUBLIC_DEVICE_ADDRESS;
	strcpy(test.custom_name, "MON SUPER SENSOR !");
	hci_LE_clear_white_list(NULL);
	hci_LE_add_white_list(&hci_socket, test); // 0x00 : PDA 

	hci_LE_get_RSSI(&hci_socket1, NULL, num_points, 0x00, scan_interval, scan_window, 0x00, 0x01);	
	
	fprintf(stderr, "\n-------------------------\n");
	fprintf(stderr, "- CLOSING SOCKETS TESTS -\n");
	fprintf(stderr, "-------------------------\n");
	fprintf(stderr, "Closing the second opened socket. \n");
	close_hci_socket(&hci_socket1);
	display_hci_socket_list();

	fprintf(stderr, "Closing all of the remaining sockets.\n");
	close_all_sockets();
	display_hci_socket_list();

	uint64_t states = 0;
	hci_LE_read_supported_states(NULL, &states);
	hci_display_LE_supported_states(states);

	hci_destroy_device_table();

	return EXIT_SUCCESS;
}
