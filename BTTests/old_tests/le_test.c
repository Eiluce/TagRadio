#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
/*
#define OGF_LE_CTL		0x08
#define OCF_LE_SET_ADVERTISING_DATA		0x0008
#define LE_SET_ADVERTISING_DATA_CP_SIZE 32
typedef struct {
	uint8_t		length;
	uint8_t		data[31];
} __attribute__ ((packed)) le_set_advertising_data_cp;

#define OCF_LE_SET_SCAN_RESPONSE_DATA		0x0009
typedef struct {
	uint8_t		length;
	uint8_t		data[31];
} __attribute__ ((packed)) le_set_scan_response_data_cp;
#define LE_SET_SCAN_RESPONSE_DATA_CP_SIZE 32

#define OCF_LE_SET_SCAN_PARAMETERS		0x000B
typedef struct {
	uint8_t		type;
	uint16_t	interval;
	uint16_t	window;
	uint8_t		own_bdaddr_type;
	uint8_t		filter;
} __attribute__ ((packed)) le_set_scan_parameters_cp;
#define LE_SET_SCAN_PARAMETERS_CP_SIZE 7

#define OCF_LE_SET_SCAN_ENABLE			0x000C
typedef struct {
	uint8_t		enable;
	uint8_t		filter_dup;
} __attribute__ ((packed)) le_set_scan_enable_cp;
#define LE_SET_SCAN_ENABLE_CP_SIZE 2

#define EVT_LE_META_EVENT	0x3E
typedef struct {
	uint8_t		subevent;
	uint8_t		data[0];
} __attribute__ ((packed)) evt_le_meta_event;
#define EVT_LE_META_EVENT_SIZE 1

#define EVT_LE_ADVERTISING_REPORT	0x02
typedef struct {
	uint8_t		evt_type;
	uint8_t		bdaddr_type;
	bdaddr_t	bdaddr;
	uint8_t		length;
	uint8_t		data[0];
} __attribute__ ((packed)) le_advertising_info;
#define LE_ADVERTISING_INFO_SIZE 9

#define EVT_CMD_COMPLETE 		0x0E
typedef struct {
	uint8_t		ncmd;
	uint16_t	opcode;
} __attribute__ ((packed)) evt_cmd_complete;
#define EVT_CMD_COMPLETE_SIZE 3

#define EVT_CMD_STATUS 			0x0F
typedef struct {
	uint8_t		status;
	uint8_t		ncmd;
	uint16_t	opcode;
} __attribute__ ((packed)) evt_cmd_status;
#define EVT_CMD_STATUS_SIZE 4

typedef struct {
	uint16_t	opcode;		// OCF & OGF 
	uint8_t		plen;
} __attribute__ ((packed))	hci_command_hdr;
#define HCI_COMMAND_HDR_SIZE 	3

#define cmd_opcode_pack(ogf, ocf)	(uint16_t)((ocf & 0x03ff)|(ogf << 10))
#define cmd_opcode_ogf(op)		(op >> 10)
#define cmd_opcode_ocf(op)		(op & 0x03ff)

int hci_open_dev(int dev_id);
int hci_send_cmd(int dd, uint16_t ogf, uint16_t ocf, uint8_t plen, void *param);
int hci_devba(int dev_id, bdaddr_t *bdaddr);
int hci_devid(const char *str);

int hci_le_set_scan_enable(int dev_id, uint8_t enable, uint8_t filter_dup, int to);
int hci_le_set_scan_parameters(int dev_id, uint8_t type, uint16_t interval,
					uint16_t window, uint8_t own_type,
					uint8_t filter, int to);
int hci_le_set_advertise_enable(int dev_id, uint8_t enable, int to);

*/
#define NAME_LENGTH 50

typedef struct {
	bdaddr_t mac;
	char name[NAME_LENGTH];
} device_info;

typedef struct {
	int sock;
	int dev_id;
} hci_socket;

// If controler == NULL, we take the first present available BT adaptator.
hci_socket open_hci_socket(bdaddr_t *controler) {
	hci_socket result;
  	result.dev_id = hci_get_route(controler); // NULL => we take the first present BT adaptator.
	result.sock = hci_open_dev(result.dev_id);
	
	if (result.dev_id < 0 || result.sock < 0) {
		perror("opening socket");
		result.sock = -1;
	}
	
	return result;		
}

void close_hci_socket(hci_socket s) {
	close(s.sock);
}

int check_cmd_complete(hci_socket hci_sock) { // ATTENTION: LA SOCKET DOIT AVOIR LE BON FILTRE..... A AMELIORER SI ON GARDE !!!!!!!!!!!!!!!!!!!!

	struct pollfd p;
	p.fd = hci_sock.sock;
	p.events = POLLIN | POLLERR | POLLHUP;
	unsigned char buf[HCI_MAX_EVENT_SIZE]; 
	/*If none of the events requested (and no error) has occurred for any of the file 
	  descriptors, then poll() blocks until one of the events occurs. */
	if (poll(&p, 1, 5000) > 0) { // poll will wait only 5s for an event to occur.
		fprintf(stderr,"COPAIN !!!!\n");
		int len = read(hci_sock.sock, buf, sizeof(buf));

		if (len < 0) {
			return -1;
		} else if (len == 0) {
			return 0;
		}
		
		switch (buf[1]) {
		case EVT_CMD_COMPLETE:
			return 0;
			break;
		default:
			fprintf(stderr, "Unknown event... %X\n", buf[1]);
			return -1;
			break;
		}
	}
	return -1;
}

int main(int argc, char **argv) {
	
	int nb_rsp = 200;

	FILE *file = NULL;
	file = fopen("./res_rssi.txt", "w");

	fprintf(stderr, "1. Opening socket...");
	hci_socket hci_sock = open_hci_socket(NULL);
	if (hci_sock.sock < 0) {
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, " [DONE]\n");

	struct pollfd p;
	// Initializing the connection poll on our HCI socket :
	p.fd = hci_sock.sock;
	p.events = POLLIN | POLLERR | POLLHUP;
	unsigned char buf[HCI_MAX_EVENT_SIZE]; 

	hci_event_hdr event_header;
	unsigned char *event_parameter;

	struct hci_filter flt;	
	struct hci_filter old_flt;

	// Creating our filter :
	hci_filter_clear(&flt); 
	hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
	//	hci_filter_set_event(EVT_CMD_COMPLETE, &flt);
	hci_filter_set_event(EVT_LE_META_EVENT, &flt);
	hci_filter_set_event(EVT_LE_ADVERTISING_REPORT, &flt);

	/* Saving the old filter.
	   This can be helpful in a case where we want to use a same socket for different purposes
	   (even with multiple threads), we can replace the old filter and re-use it later.
	*/
	fprintf(stderr, "2. Saving old filter...");
	socklen_t old_flt_len = sizeof(old_flt);
	if (getsockopt(hci_sock.sock, SOL_HCI, HCI_FILTER, (void *)&old_flt, &old_flt_len) < 0) {
		perror("Cannot save the old filter");
	}
	fprintf(stderr, " [DONE]\n");

	// Applying the new filter :
	fprintf(stderr, "3. Applying new filter...");
	if (setsockopt(hci_sock.sock, SOL_HCI, HCI_FILTER, (void *)&flt, sizeof(flt)) < 0) {
		perror("Can't set HCI filter");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, " [DONE]\n");

	fprintf(stderr, "4. Setting scan parameters...");
	if(hci_le_set_scan_parameters(hci_sock.sock, 0x00, 0x0020, // cf p 1066 spec
		0x0010, 0x00, 0x00, 100) < 0) {
		perror("set_scan_parameters");
		close_hci_socket(hci_sock);
		exit(EXIT_FAILURE);
	}
	//	if (check_cmd_complete(hci_sock) == 0) {// ATTENTION : ON RECOIT PEUT ETRE CE PAQUET INSTANTANEMENT (A TESTER), IL FAUDRAIT DONC ENVOYER LA REQUETE LORSQU'ON A DEJA INITIALISE LE POLL, POUR QU'IL PUISSE CAPTER UN EVENTUEL EVENT AU MOMENT OU ON ENVOIE LA CMD !!!!!!!!!!!!!
		fprintf(stderr, " [DONE]\n"); 
		//	}

	fprintf(stderr, "5. Enabling scan...");
	if (hci_le_set_scan_enable(hci_sock.sock, 0x01, 0x01, 0) < 0) {
		perror("set_scan_enable");
		close_hci_socket(hci_sock);
		exit(EXIT_FAILURE);
	}
	//	if (check_cmd_complete(hci_sock) == 0) {
		fprintf(stderr, " [DONE]\n"); 
		//	}

	char canceled = 0;
	int k = 0;

	fprintf(stderr, "6. Checking response events...\n");
	while(!canceled && k < nb_rsp) {
		p.revents = 0;

		//	fprintf(stderr, "******** %i ********\n", k);
		// Polling the BT device for an event :
		if (poll(&p, 1, -1) > 0) {

			// If an event occured, we store the HCI Event packet into our buffer :
			int len = read(hci_sock.sock, buf, sizeof(buf));

			if (len < 0) {
				continue;
			} else if (len == 0) {
				break;
			}

			// WHAT IS buf[0] ? Maybe a parameter added by "read()" ???
			event_header.evt = buf[1];
			event_header.plen = buf[2];
			
			event_parameter = (void *)(buf + HCI_EVENT_HDR_SIZE + 1);
			uint8_t subevent_code = *(event_parameter);
			event_parameter = (void *)(event_parameter + 1);
			int num_reports = *event_parameter;
			event_parameter = (void *)(event_parameter + 1);
		
			switch (event_header.evt) {
			case EVT_LE_META_EVENT: // Code 0x3E
				switch (subevent_code) {
				case EVT_LE_ADVERTISING_REPORT:
					k++;
					for (uint16_t i = 0; i < num_reports; i++) {
						device_info di;
						uint8_t *address_type;
						char tmp[18]; 

						// Address type field :
						char address_type_mess[6] = {0};
						address_type = (uint8_t *)(event_parameter + 1*num_reports + i);
						if (*address_type == 0x00) { // Public Device Address
							strcpy(address_type_mess, "[PDA]");
						} else if (*address_type == 0x01) { // Random Device Address
							strcpy(address_type_mess, "[RDA]");
						} else {
							strcpy(address_type_mess, "[???]");
						}

						// Address field :
						bdaddr_t *rsp_mac = (bdaddr_t *)(event_parameter +
										 num_reports*(1+1) +
										 6*i); // @ on 6 bytes.
						di.mac = *rsp_mac;

						// Length_data field :
						uint8_t *length_data = (uint8_t *)(event_parameter +
										   num_reports*(1+1+6) + i);

						// RSSI field :
						int8_t *rssi = (int8_t *)(event_parameter + 
									  (1+1+6+1+(*length_data))*num_reports + i); // rssi on 1byte

						// Display info :
						ba2str(&(di.mac), tmp);
						fprintf(stdout, "%s [%s] : %idb \n", address_type_mess, tmp, *rssi);
						fprintf(file, "%i\n", *rssi);
					}
					break;
					
				default:
					fprintf(stderr, "An unknown LE sub-event occured : 0x%X \n", subevent_code);
					break;
				}
				break;
			default:
				fprintf(stderr, "An unknown event occured : 0x%X\n", event_header.evt);
				break;
			}
		}
	}
	fprintf(stderr, "Scan complete !\n");
	
	fprintf(stderr, "7. Disabling scan...");
	if (hci_le_set_scan_enable(hci_sock.sock, 0x00, 0x01, 0) < 0) {
		perror("set_scan_disable");
		close_hci_socket(hci_sock);
		exit(EXIT_FAILURE);
	}
	//	if (check_cmd_complete(hci_sock) == 0) {
		fprintf(stderr, " [DONE]\n"); 
		//	}

	fprintf(stderr, "8. Closing socket...");
	close_hci_socket(hci_sock);
	fprintf(stderr, " [DONE]\n");

	fclose(file);

	return EXIT_SUCCESS;
}
