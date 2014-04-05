#include "hci_controler.h"
#include "hci_utils.h"
#include <bluetooth/hci_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/ioctl.h>

void hci_compute_filter(struct hci_filter *flt, ...) {
	va_list  pa;
	uint32_t event;
	
	if (flt == NULL) {
		fprintf(stderr, "hci_compute_filter warning : invalid filter.\n");
		return;
	}

	hci_filter_clear(flt); 
	hci_filter_set_ptype(HCI_EVENT_PKT, flt); // ATTENTION PEUT ËTRE A CHANGER : DONNER LE CHOIX CAR SUREMENT PAS ASSEZ GENERAL ? 	
	// HCI_EVENT_PKT = HCI Event Packet.

	va_start(pa, flt);
	event = va_arg(pa, uint32_t);
	while (event != 0) {
		hci_filter_set_event(event, flt);
		event = va_arg(pa, uint32_t);
	}
	va_end(pa);
}

//------------------------------------------------------------------------------------

int8_t hci_LE_clear_white_list(hci_socket_t *hci_socket) {
	char new_socket = 0;
	if (hci_socket == NULL) {
		new_socket = 1;
		hci_socket = malloc(sizeof(hci_socket_t));
		*hci_socket = open_hci_socket(NULL);
	}
	if (hci_socket->sock < 0) {
		if (new_socket) {
			free(hci_socket);
		}
		return -1;
	}

	if (hci_le_clear_white_list(hci_socket->sock, 100) < 0) {
		perror("hci_le_clear_white_list");
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}
		return -1;
	}

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} 

	return 0;
}

//------------------------------------------------------------------------------------

int8_t hci_LE_add_white_list(hci_socket_t *hci_socket, const bdaddr_t *add, uint8_t add_type) {
	char new_socket = 0;
	
	if (add == NULL) {
		fprintf(stderr, "hci_LE_add_white_list error : NULL address.\n");
		return -1;
	}

	if (hci_socket == NULL) {
		new_socket = 1;
		hci_socket = malloc(sizeof(hci_socket_t));
		*hci_socket = open_hci_socket(NULL);
	}
	if (hci_socket->sock < 0) {
		if (new_socket) {
			free(hci_socket);
		}
		return -1;
	}


	if (hci_le_add_white_list(hci_socket->sock, add, add_type, 100) < 0) {
		perror("hci_le_add_white_list");
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}
		return -1;
	}

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} 

	return 0;
}

//------------------------------------------------------------------------------------

int8_t hci_LE_rm_white_list(hci_socket_t *hci_socket, const bdaddr_t *add, uint8_t add_type) {
	char new_socket = 0;

	if (add == NULL) {
		fprintf(stderr, "hci_LE_rm_white_list error : NULL address.\n");
		return -1;
	}

	if (hci_socket == NULL) {
		new_socket = 1;
		hci_socket = malloc(sizeof(hci_socket_t));
		*hci_socket = open_hci_socket(NULL);
	}
	if (hci_socket->sock < 0) {
		if (new_socket) {
			free(hci_socket);
		}
		return -1;
	}

	if (hci_le_rm_white_list(hci_socket->sock, add, add_type, 100) < 0) {
		perror("hci_le_rm_white_list");
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}
		return -1;
	}

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} 

	return 0;
}

//------------------------------------------------------------------------------------

int8_t hci_LE_get_white_list_size(hci_socket_t *hci_socket, uint8_t *size) {
	char new_socket = 0;
	if (hci_socket == NULL) {
		new_socket = 1;
		hci_socket = malloc(sizeof(hci_socket_t));
		*hci_socket = open_hci_socket(NULL);
	}
	if (hci_socket->sock < 0) {
		if (new_socket) {
			free(hci_socket);
		}
		return -1;
	}

	if (hci_le_read_white_list_size(hci_socket->sock, size, 100) < 0) {
		perror("hci_le_get_white_list_size");
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}
		return -1;
	}

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} 

	return 0;
}

//------------------------------------------------------------------------------------

hci_device_table_t hci_scan_devices(hci_socket_t *hci_socket,
				    uint8_t duration, uint16_t max_rsp, long flags) {
	
	hci_device_table_t res;
	res.device = NULL;
	res.length = 0;
	
	char new_socket = 0;
	if (hci_socket == NULL) {
		new_socket = 1;
		hci_socket = malloc(sizeof(hci_socket_t));
		*hci_socket = open_hci_socket(NULL);
	}
	if (hci_socket->sock < 0) {
		if (new_socket) {
			free(hci_socket);
		}
		return res;
	}

	// Creation of the inquiry_info table :
	inquiry_info *ii = calloc(max_rsp, sizeof(inquiry_info));
	
	// Starting the inquiry :
	fprintf(stderr, "Starting the scanning inquiry...");

	int16_t num_rsp = hci_inquiry(hci_socket->dev_id, duration, max_rsp, NULL, &ii, flags);
	if(num_rsp <= 0) {
		fprintf(stderr, " No device found.\n");
		free(ii);
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}
		return res;
	}

	fprintf(stderr, " [DONE]\n"); // PAS DONE ICI, IL FAUDRAIT LE METTRE QUAND ON A 
	//RECU L'EVENT DE FIN DE COMMAND !!! (ICI, INQUIRY COMPLETE !).

	hci_device_t *device_table = calloc(num_rsp, sizeof(hci_device_t));

	for (uint16_t i = 0; i < num_rsp; i++) {
		memset(&(device_table[i]), 0, sizeof(hci_device_t));
		device_table[i].mac = ii[i].bdaddr;
		if (hci_read_remote_name(hci_socket->sock, &(device_table[i].mac), NAME_LENGTH, 
					 device_table[i].real_name, 0) < 0) {
			perror("hci_read_remote_name");
			strcpy(device_table[i].real_name, "[UNKNOWN]");
		}
		device_table[i].add_type = 0x12; // Personal code to indicate that this data wasn't available.
		strcpy(device_table[i].custom_name, device_table[i].real_name);
	}

	free(ii);

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	}

	res.device = device_table;
	res.length = num_rsp;

	return res;
}

//------------------------------------------------------------------------------------

void hci_get_RSSI(hci_socket_t *hci_socket,
		  bdaddr_t *mac, uint8_t duration, uint16_t max_rsp) {
	

	// HCI_Filter structure :
	struct hci_filter flt;	
	struct hci_filter old_flt;

	// Command parameter for an "OCF_WRITE_INQUIRY_MODE" :
	write_inquiry_mode_cp write_cp;

	// Command parameter for a standard inquiry : "OCF_INQUIRY" :
	inquiry_cp cp;
	memset (&cp, 0, sizeof(cp));

	/* Events buffer : table of unsigned char (range 0-255) allowing us
	   to catch an HCI-event packet coming in response to one of our
	   commands packet (thanks to the "read()" function).
	*/
	unsigned char buf[HCI_MAX_EVENT_SIZE];
	unsigned char *event_parameter;
 
	hci_event_hdr event_header;

	char new_socket = 0;
	if (hci_socket == NULL) {
		new_socket = 1;
		hci_socket = malloc(sizeof(hci_socket_t));
		*hci_socket = open_hci_socket(NULL);
	}
	if (hci_socket->sock < 0) {
		if (new_socket) {
			free(hci_socket);
		}
		return;
	}
	
	// Creating our filter :
	hci_compute_filter(&flt, EVT_CMD_COMPLETE,
			   EVT_INQUIRY_RESULT_WITH_RSSI,
			   EVT_INQUIRY_COMPLETE,
			   0);

	/* Saving the old filter.
	   This can be helpful in a case where we want to use a same socket for different purposes
	   (even with multiple threads), we can replace the old filter and re-use it later.
	*/
	if (!new_socket) {
		old_flt = get_hci_socket_filter(*hci_socket);
	}
	
	// Applying the new filter :
	if (set_hci_socket_filter(*hci_socket, &flt) < 0) {
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}
		return;
	}

	// Configuring the inquiry mode on the open HCI socket :
	fprintf(stderr, "Configuring the inquiry mode...");
	write_cp.mode = 0x01; // Inquiry Result format with RSSI (cf p878 spec').
	if (hci_send_cmd(hci_socket->sock, OGF_HOST_CTL, OCF_WRITE_INQUIRY_MODE, 
			 WRITE_INQUIRY_MODE_CP_SIZE, &write_cp) < 0) {
		fprintf(stderr, " [ERROR]\n");
		perror("Can't set inquiry mode");
		return;
	}
	//	if (check_cmd_complete(hci_sock) == 0) { // ATTENTION : ON RECOIT PEUT ETRE CE PAQUET INSTANTANEMENT (A TESTER), IL FAUDRAIT DONC ENVOYER LA REQUETE LORSQU'ON A DEJA INITIALISE LE POLL, POUR QU'IL PUISSE CAPTER UN EVENTUEL EVENT AU MOMENT OU ON ENVOIE LA CMD !!!!!!!!!!!!!
	fprintf(stderr, " [DONE]\n"); 
	//	}

	// Setting the command parameters for our rssi inquiry :
	cp.lap[2] = 0x9e; 
	cp.lap[1] = 0x8b;
	cp.lap[0] = 0x33;
	cp.num_rsp = max_rsp; 
	cp.length = duration; 


	fprintf(stderr, "Starting inquiry with RSSI...");

	// Sending the inquiry's command to the HCI socket :
	if (hci_send_cmd(hci_socket->sock, OGF_LINK_CTL, OCF_INQUIRY, INQUIRY_CP_SIZE, &cp) < 0) {
		fprintf(stderr, " [ERROR]\n");
		perror("Can't start inquiry");
		return;
	}

	// Connection poll structure, not initialized yet :
	struct pollfd p;
	// Initializing the connection poll on our HCI socket :
	p.fd = hci_socket->sock;
	p.events = POLLIN | POLLERR | POLLHUP;

	char canceled = 0;

	while(!canceled) {
		p.revents = 0;

		// Polling the BT device for an event :
		if (poll(&p, 1, -1) > 0) {

			// If an event occured, we store the HCI Event packet into our buffer :
			int len = read(hci_socket->sock, buf, sizeof(buf));

			if (len < 0) {
				continue;
			} else if (len == 0) {
				break;
			}

			// WHAT IS buf[0] ? Maybe a parameter added by "read()" ???
			event_header.evt = buf[1];
			event_header.plen = buf[2];
	
			event_parameter = buf + HCI_EVENT_HDR_SIZE + 1;
			
			int num_results = event_parameter[0];
			event_parameter = (void *)(event_parameter + 1); // We switch to the next byte.

			// MAINTENIR CORRESPONDANCE DEVICES DEJA RENCONTRES MAIS AU NIVEAU LE PLUS GLOBAL POSSIBLE POUR QU'ELLE SOIT ACCESSIBLE A TRAVERS TOUTES NOS FONCTIONS (gagne en efficacité !!) !!!!!!!!!!!!!

			switch (event_header.evt) {
			case EVT_CMD_COMPLETE:
				fprintf(stderr, " [DONE] \n");
				break;
			case EVT_INQUIRY_RESULT_WITH_RSSI: // Code 0x22 
				for (uint16_t i = 0; i < num_results; i++) {
					hci_device_t hci_device;
				
					bdaddr_t *rsp_mac = (bdaddr_t *)(event_parameter + 6*i); // @ on 6 bytes.
					hci_device.mac = *rsp_mac;
					if (mac != NULL && !(hci_compare_addresses(mac, rsp_mac))) {
							continue;
					}
					strcpy(hci_device.real_name, "UNKNOWN");
					strcpy(hci_device.custom_name, "UNKNOWN");
					hci_device.add_type  = 0x12;
					/* To understand the meaning of the following offsets, please refer to
					   the spec' p.1002. The structure of the HCI Packet "inquiry result with
					   RSSI" is shown and explained. To sum up, during the inquiry period,
					   several devices may respond and one event packet may contain several
					   informations. But these informations are not stored device per device
					   but rather "feature by feature". That is to say that the event packet
					   contains first all of the mac@, then all of the 
					   "page_scan_repetition_mode" etc...
					*/
					int8_t *rssi = (int8_t *)(event_parameter + (6+1+1+3+2)*num_results + i); 
			
					hci_device_display(hci_device);
					fprintf(stdout, "%idb \n", *rssi);
				}
				break;

			case EVT_INQUIRY_COMPLETE:
				fprintf(stderr, "Inquiry complete !\n");
				canceled = 1;
				break;
				
			default:
				fprintf(stderr, "An unknown event occurred : 0x%X\n", event_header.evt);
				break;
			}
			
		}
	}

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} else {
		// Restoring the old filter :
		set_hci_socket_filter(*hci_socket, &old_flt);
	}
}

//------------------------------------------------------------------------------------

// TESTER SI ON RECOIT AUSSI DES PAQUEST NON LE (MODIFIER FILTRE)
void hci_LE_get_RSSI(hci_socket_t *hci_socket,
		     bdaddr_t *mac, uint16_t max_rsp, uint8_t scan_type, uint16_t scan_interval,
		     uint16_t scan_window, uint8_t own_add_type, uint8_t scan_filter_policy) {
  

	FILE *file = NULL;

	fprintf(stderr, "1. Opening socket...");
	char new_socket = 0;
	if (hci_socket == NULL) {
		new_socket = 1;
		hci_socket = malloc(sizeof(hci_socket_t));
		*hci_socket = open_hci_socket(NULL);
	}
	if (hci_socket->sock < 0) {
		if (new_socket) {
			free(hci_socket);
		}
		return;
	}
	fprintf(stderr, " [DONE]\n");

	struct pollfd p;
	// Initializing the connection poll on our HCI socket :
	p.fd = hci_socket->sock;
	p.events = POLLIN | POLLERR | POLLHUP;
	unsigned char buf[HCI_MAX_EVENT_SIZE]; 

	hci_event_hdr event_header;
	unsigned char *event_parameter;

	struct hci_filter flt;	
	struct hci_filter old_flt;

	// Creating our filter :
	hci_compute_filter(&flt, EVT_CMD_COMPLETE,
			   EVT_LE_META_EVENT,
			   EVT_LE_ADVERTISING_REPORT,
			   0);

	//hci_filter_all_events(&flt);

	/* Saving the old filter.
	   This can be helpful in a case where we want to use a same socket for different purposes
	   (even with multiple threads), we can replace the old filter and re-use it later.
	*/
	fprintf(stderr, "2. Saving old filter...");
	if (!new_socket) {
		old_flt = get_hci_socket_filter(*hci_socket);
	}
	fprintf(stderr, " [DONE]\n");

	// Applying the new filter :
	fprintf(stderr, "3. Applying new filter...");
	if (set_hci_socket_filter(*hci_socket, &flt) < 0) {
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}
		return;
	}
	fprintf(stderr, " [DONE]\n");

	fprintf(stderr, "4. Setting scan parameters...");
	if(hci_le_set_scan_parameters(hci_socket->sock, scan_type, scan_interval, // cf p 1066 spec
				      scan_window, own_add_type, scan_filter_policy, 0) < 0) { 
		// last parameter is timeout (for reaching the controler) 0 = infinity.
		fprintf(stderr, " [ERROR] \n");
		perror("set_scan_parameters");
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}
		return;
	}
	//	if (check_cmd_complete(hci_sock) == 0) {// ATTENTION : ON RECOIT PEUT ETRE CE PAQUET INSTANTANEMENT (A TESTER), IL FAUDRAIT DONC ENVOYER LA REQUETE LORSQU'ON A DEJA INITIALISE LE POLL, POUR QU'IL PUISSE CAPTER UN EVENTUEL EVENT AU MOMENT OU ON ENVOIE LA CMD !!!!!!!!!!!!!
	fprintf(stderr, " [DONE]\n"); 
		//	}

	fprintf(stderr, "5. Enabling scan...");
	if (hci_le_set_scan_enable(hci_socket->sock, 0x01, 0x01, 0) < 0) { // Duplicate filtering ? (cf p1069)
		fprintf(stderr, " [ERROR] \n");
		perror("set_scan_enable");
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}

		return;
	}
	//	if (check_cmd_complete(hci_sock) == 0) {
	fprintf(stderr, " [DONE]\n"); 
	//	}

	char canceled = 0;
	int k = 0;
	file = fopen("./stats/res_rssi.txt", "w");
	//if (file == NULL) {

	fprintf(stderr, "6. Checking response events...\n");
	while(!canceled && (!(max_rsp > 0) || (k < max_rsp))) {
		p.revents = 0;

		// Polling the BT device for an event :
		if (poll(&p, 1, 5000) > 0) {

			// If an event occured, we store the HCI Event packet into our buffer :
			int len = read(hci_socket->sock, buf, sizeof(buf));

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
					for (uint16_t i = 0; i < num_reports; i++) {
						hci_device_t hci_device;
						uint8_t *address_type;

						// Address type field :
						address_type = (uint8_t *)(event_parameter + 1*num_reports + i);

						// Address field :
						bdaddr_t *rsp_mac = (bdaddr_t *)(event_parameter +
										 num_reports*(1+1) +
										 6*i); // @ on 6 bytes.
						
					
						if (mac != NULL && !(hci_compare_addresses(mac, rsp_mac))) {
							continue;
						}

						// Length_data field :
						uint8_t *length_data = (uint8_t *)(event_parameter +
										   num_reports*(1+1+6) + i);

						// RSSI field :
						int8_t *rssi = (int8_t *)(event_parameter + 
									  (1+1+6+1+(*length_data))*num_reports + i); // rssi on 1byte
						if (*rssi == 127) {
							fprintf(stderr, "hci_le_get_rssi warning : RSSI measure unavailable.\n");
						} else if (*rssi >= 21) {
							fprintf(stderr, "hci_le_get_rssi error : Error while reading RSSI measure.\n");
						}

						// Display info :
						hci_device.mac = *rsp_mac;
						hci_device.add_type = *address_type;
						strcpy(hci_device.real_name,"UNKNOWN");
						strcpy(hci_device.custom_name, "UNKNOWN");
						hci_device_display(hci_device);
						fprintf(stdout, "%idb\n", *rssi);
						fprintf(file, "%i\n", *rssi);
						k++;
					}
					break;
					
				default:
					fprintf(stderr, "An unknown LE sub-event occured : 0x%X \n",
						subevent_code);
					break;
				}
				break;
			default:
				fprintf(stderr, "An unknown event occured : 0x%X\n", event_header.evt);
				break;
			}
		} else {
			fprintf(stderr, "hci_get_le_rssi warning : timeout expired.\n");
			break;
		}
	}
	fprintf(stderr, "Scan complete !\n");
	
	fprintf(stderr, "7. Disabling scan...");
	if (hci_le_set_scan_enable(hci_socket->sock, 0x00, 0x01, 0) < 0) {
		perror("set_scan_disable");
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}	
		fclose(file);
		return;
	}
	//	if (check_cmd_complete(hci_sock) == 0) {
	fprintf(stderr, " [DONE]\n"); 
	//	}

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} else {			
		// Restoring the old filter :
		set_hci_socket_filter(*hci_socket, &old_flt);
	}
	
	fclose(file);

}
