#include "hci_controler.h"
#include "hci_utils.h"
#include "hci_device.h"
#include <bluetooth/hci_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include "cfuhash.h"

static cfuhash_table_t *hci_devices_table = NULL;

char hci_already_registered_device(bdaddr_t add) {
	if (!hci_devices_table) {
		hci_devices_table = cfuhash_new_with_initial_size(200);
	}
	return (char)(cfuhash_exists(hci_devices_table, batostr(&add)));
}

hci_device_t *hci_register_device(hci_device_t hci_device) {
	if (!hci_devices_table) {
		hci_devices_table = cfuhash_new_with_initial_size(200);
	}
	hci_device_t *tmp = malloc(sizeof(hci_device_t));
	memcpy(tmp, &hci_device, sizeof(hci_device_t));
	return (hci_device_t *)cfuhash_put(hci_devices_table, batostr(&(hci_device.mac)), (void *)tmp);
} 

hci_device_t *hci_get_device(bdaddr_t add) {
	if (!hci_devices_table) {
		hci_devices_table = cfuhash_new_with_initial_size(200);
	}
	return (hci_device_t *)cfuhash_get(hci_devices_table, batostr(&add));
}

void hci_destroy_device_table(void) {
	if (hci_devices_table) {
		cfuhash_clear(hci_devices_table);
		free(hci_devices_table);
	}
	hci_devices_table = NULL;
}

//------------------------------------------------------------------------------------

static inline void check_hci_socket_ptr(hci_socket_t **hci_socket, char *new_socket, char *err) {
	*err = 0;
	if (*hci_socket == NULL) {
		*new_socket = 1;
		*hci_socket = malloc(sizeof(hci_socket_t));
		*(*hci_socket) = open_hci_socket(NULL);
	}
	if ((*hci_socket)->sock < 0) {
		if (*new_socket) {
			free(*hci_socket);
			*err = 1;
		}
	}
}

//------------------------------------------------------------------------------------

static char check_cmd_complete(hci_socket_t *hci_socket) {//http://forum.hardware.fr/hfr/Programmation/C/poll-sockets-sujet_86846_1.htm
	//http://code.metager.de/source/xref/linux/bluetooth/bluez/lib/hci.c
	struct pollfd p;
	int n;
	unsigned char buf[HCI_MAX_EVENT_SIZE];
	hci_event_hdr *hdr;

	uint8_t try = 10;
	while (try--) {
		p.fd = hci_socket->sock; p.events = POLLIN;
		while ((n = poll(&p, 1, 500)) < 0) {
			if (errno == EAGAIN || errno == EINTR)
				continue;
			goto fail;
		}
	
		if (!n) {
			fprintf(stderr, " ???????? \n");
			errno = ETIMEDOUT;
			goto fail;
		}

		int8_t len;
		while ((len = read(hci_socket->sock, buf, sizeof(buf))) < 0) {
			if (errno == EAGAIN || errno == EINTR)
				continue;
			goto fail;
		}

		hdr = (void *) (buf + 1);
		switch (hdr->evt) {
		case EVT_CMD_COMPLETE:
			return 1;
			break;
		default:
			fprintf(stderr, "check_cmd_complete warning : an unknown event occured.\n");
			break;
		}
	}
	errno = ETIMEDOUT;

 fail:
	return 0;

}

//------------------------------------------------------------------------------------

void hci_compute_filter(struct hci_filter *flt, ...) {
	va_list  pa;
	uint32_t event;
	
	if (flt == NULL) {
		fprintf(stderr, "hci_compute_filter warning : invalid filter.\n");
		return;
	}

	hci_filter_clear(flt); 
	hci_filter_set_ptype(HCI_EVENT_PKT, flt); 
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

// RFU (Reserved for Future Use) : the only information available yet is the LE_Encryption...
int8_t hci_LE_read_local_supported_features(hci_socket_t *hci_socket, uint8_t *features) { //p1056

	struct hci_request rq;
	memset(&rq, 0, sizeof(rq));
	le_read_local_supported_features_rp rp;
	memset(&rp, 0, sizeof(rp));
	
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_READ_LOCAL_SUPPORTED_FEATURES;
	rq.rparam = &rp;
	rq.rlen = LE_READ_LOCAL_SUPPORTED_FEATURES_RP_SIZE;
	
	char new_socket = 0;
	if (features == NULL) {
		fprintf(stderr, "hci_LE_read_local_supported_features error : invalid reference.\n");
		return -1;
	}

	char socket_err = 0;
	check_hci_socket_ptr(&hci_socket, &new_socket, &socket_err);
	if (socket_err) {
		return -1;
	}

	if (hci_send_req(hci_socket->sock, &rq, 200) < 0) {
		perror("hci_LE_read_local_supported_features");
		goto fail;
	}

	if (rp.status) {
		fprintf(stderr, "hci_LE_read_supported_features error : 0x%X\n", rp.status);
		goto fail;
	}

	memcpy(features, rp.features, 8);

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} 

	return 0;

 fail:
	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} 
	return -1;

}

//------------------------------------------------------------------------------------

int8_t hci_LE_read_supported_states(hci_socket_t *hci_socket, uint64_t *states) { // p1096

	struct hci_request rq;
	memset(&rq, 0, sizeof(rq));
	le_read_supported_states_rp rp;
	memset(&rp, 0, sizeof(rp));

	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_READ_SUPPORTED_STATES;
	rq.rparam = &rp;
	rq.rlen = LE_READ_SUPPORTED_STATES_RP_SIZE;

	char new_socket = 0;
	if (states == NULL) {
		fprintf(stderr, "hci_LE_read_local_supported_features error : invalid reference.\n");
		return -1;
	}

	char socket_err = 0;
	check_hci_socket_ptr(&hci_socket, &new_socket, &socket_err);
	if (socket_err) {
		return -1;
	}

	if (hci_send_req(hci_socket->sock, &rq, 200) < 0) {
		perror("hci_LE_read_supported_states");
		goto fail;
	}

	if (rp.status) {
		fprintf(stderr, "hci_LE_read_supported_states error : 0x%X\n", rp.status);
		goto fail;
	}

	*states = rp.states;

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} 

	return 0;

 fail:
	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} 
	return -1;

}

//------------------------------------------------------------------------------------

int8_t hci_LE_clear_white_list(hci_socket_t *hci_socket) {
	char new_socket = 0;
	char socket_err = 0;
	check_hci_socket_ptr(&hci_socket, &new_socket, &socket_err);
	if (socket_err) {
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

int8_t hci_LE_add_white_list(hci_socket_t *hci_socket, const hci_device_t hci_device) {
	char new_socket = 0;
	uint8_t add_type = hci_device.add_type;

	char socket_err = 0;
	check_hci_socket_ptr(&hci_socket, &new_socket, &socket_err);
	if (socket_err) {
		return -1;
	}

	if (add_type == UNKNOWN_ADDRESS_TYPE) {
		add_type = PUBLIC_DEVICE_ADDRESS;
	}
	if (hci_le_add_white_list(hci_socket->sock, &(hci_device.mac), add_type, 100) < 0) {
		perror("hci_le_add_white_list");
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}
		return -1;
	}

	if (!hci_already_registered_device(hci_device.mac)) {
		hci_register_device(hci_device);
	}

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} 

	return 0;
}

//------------------------------------------------------------------------------------

int8_t hci_LE_rm_white_list(hci_socket_t *hci_socket, const hci_device_t hci_device) {
	char new_socket = 0;
	uint8_t add_type = hci_device.add_type;

	char socket_err = 0;
	check_hci_socket_ptr(&hci_socket, &new_socket, &socket_err);
	if (socket_err) {
		return -1;
	}

	if (add_type == UNKNOWN_ADDRESS_TYPE) {
		add_type = PUBLIC_DEVICE_ADDRESS;
	}
	if (hci_le_rm_white_list(hci_socket->sock, &(hci_device.mac), add_type, 100) < 0) {
		perror("hci_le_rm_white_list");
		if (new_socket) {
			close_hci_socket(hci_socket);
			free(hci_socket);
		}
		return -1;
	}

	if (!hci_already_registered_device(hci_device.mac)) {
		hci_register_device(hci_device);
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
	char socket_err = 0;
	check_hci_socket_ptr(&hci_socket, &new_socket, &socket_err);
	if (socket_err) {
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

void hci_compute_device_name(hci_device_t *hci_device) {
	if (!hci_device) {
		fprintf(stderr, "hci_compute_device_name error : invalid device's reference.\n");
		return;
	}
	hci_socket_t hci_socket = open_hci_socket(NULL);
	if (hci_socket.sock < 0) {
		strcpy(hci_device->real_name, "[UNKNOWN]");
		return;
	}
	
	if (hci_read_remote_name(hci_socket.sock, &(hci_device->mac), NAME_LENGTH, 
				 hci_device->real_name, 0) < 0) {
		perror("hci_read_remote_name");
		strcpy(hci_device->real_name, "[UNKNOWN]");
	}

	close_hci_socket(&hci_socket);
}

//------------------------------------------------------------------------------------

hci_device_table_t hci_scan_devices(hci_socket_t *hci_socket,
				    uint8_t duration, uint16_t max_rsp, long flags) {
	
	hci_device_table_t res;
	res.device = NULL;
	res.length = 0;
	
	char new_socket = 0;
	char socket_err = 0;
	check_hci_socket_ptr(&hci_socket, &new_socket, &socket_err);
	if (socket_err) {
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
		goto fail;
	}

	fprintf(stderr, " [DONE]\n"); 

	hci_device_t *device_table = calloc(num_rsp, sizeof(hci_device_t));

	for (uint16_t i = 0; i < num_rsp; i++) {
		memset(&(device_table[i]), 0, sizeof(hci_device_t));
		device_table[i].mac = ii[i].bdaddr;
		hci_compute_device_name(&(device_table[i]));
		device_table[i].add_type = UNKNOWN_ADDRESS_TYPE; // Personal code to indicate that this data wasn't available.
		strcpy(device_table[i].custom_name, "UNKNOWN");
		if (!hci_already_registered_device(device_table[i].mac)) {
			hci_register_device(device_table[i]);
		}
	}

	free(ii);

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	}

	res.device = device_table;
	res.length = num_rsp;

	return res;

 fail:
	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	}
	return res;
}

//------------------------------------------------------------------------------------

// TODO : MODIFIER POUR FORCER L'ACCEPTATION DE PAQUETS DUPLIQUES SINON ON AURA TOUT LE TEMPS QU'UNE MESURE AVEC UN DONGLE STYLE BELKIN....
void hci_get_RSSI(hci_socket_t *hci_socket,
		  bdaddr_t *mac, uint8_t duration, uint16_t max_rsp) {
	

	// HCI_Filter structure :
	struct hci_filter flt;	
	struct hci_filter old_flt;

	// Command parameter for an "OCF_WRITE_INQUIRY_MODE" :
	write_inquiry_mode_cp write_cp;
	memset(&write_cp, 0, sizeof(write_cp));

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
	char socket_err = 0;
	check_hci_socket_ptr(&hci_socket, &new_socket, &socket_err);
	if (socket_err) {
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
		goto fail;
	}

	// Configuring the inquiry mode on the open HCI socket :
	fprintf(stderr, "Configuring the inquiry mode...");
	write_cp.mode = 0x01; // Inquiry Result format with RSSI (cf p878 spec').
	if (hci_send_cmd(hci_socket->sock, OGF_HOST_CTL, OCF_WRITE_INQUIRY_MODE, 
			 WRITE_INQUIRY_MODE_CP_SIZE, &write_cp) < 0) {
		fprintf(stderr, " [ERROR]\n");
		perror("Can't set inquiry mode");
		goto fail;
	}
	if (check_cmd_complete(hci_socket)) {
		fprintf(stderr, " [DONE]\n"); 
	} else {
		fprintf(stderr, " [ERROR ?]\n");
	}

	// Setting the command parameters for our rssi inquiry :
	cp.lap[2] = 0x9e; 
	cp.lap[1] = 0x8b;
	cp.lap[0] = 0x33;
	cp.num_rsp = max_rsp; 
	cp.length = duration; 

	fprintf(stderr, "Starting inquiry with RSSI...");

	/* Sending the inquiry's command to the HCI socket :
	   WARNING : According to the spec (p 706), no Command Complete event will be sent to the host by using
	   this command. This will only generate an Inquiry Complete event.
	*/
	if (hci_send_cmd(hci_socket->sock, OGF_LINK_CTL, OCF_INQUIRY, INQUIRY_CP_SIZE, &cp) < 0) {
		fprintf(stderr, " [ERROR]\n");
		perror("Can't start inquiry");
		goto fail;
	}
	fprintf(stderr, " [DONE]\n"); 

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

			switch (event_header.evt) {
			case EVT_CMD_COMPLETE: // Corresponding to the last "hci_send_cmd"
				fprintf(stderr, "hci_get_RSSI warning : untreated Command Complete event.\n");
				break;
			case EVT_INQUIRY_RESULT_WITH_RSSI: // Code 0x22 
				for (uint16_t i = 0; i < num_results; i++) {
					hci_device_t hci_device;
					bdaddr_t *rsp_mac = (bdaddr_t *)(event_parameter + 6*i); // @ on 6 bytes.
					if (!hci_already_registered_device(*rsp_mac)) {
						hci_device.mac = *rsp_mac;	
						hci_compute_device_name(&hci_device);
						strcpy(hci_device.custom_name, "UNKNOWN");
						hci_device.add_type  = UNKNOWN_ADDRESS_TYPE;
						hci_register_device(hci_device);
					} else {
						hci_device = *hci_get_device(*rsp_mac);
					}

					if (mac != NULL && !(hci_compare_addresses(mac, rsp_mac))) {
							continue;
					}

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

	return;

 fail:
	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	}
	return;
}

//------------------------------------------------------------------------------------

// TESTER SI ON RECOIT AUSSI DES PAQUEST NON LE (MODIFIER FILTRE)
void hci_LE_get_RSSI(hci_socket_t *hci_socket,
		     bdaddr_t *mac, uint16_t max_rsp, uint8_t scan_type, uint16_t scan_interval,
		     uint16_t scan_window, uint8_t own_add_type, uint8_t scan_filter_policy) {
  

	FILE *file = NULL;

	fprintf(stderr, "1. Opening socket...");
	char new_socket = 0;
	char socket_err = 0;
	check_hci_socket_ptr(&hci_socket, &new_socket, &socket_err);
	if (socket_err) {
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
		goto fail;
	}
	fprintf(stderr, " [DONE]\n");

	fprintf(stderr, "4. Setting scan parameters...");
	if(hci_le_set_scan_parameters(hci_socket->sock, scan_type, scan_interval, // cf p 1066 spec
				      scan_window, own_add_type, scan_filter_policy, 0) < 0) { 
		// last parameter is timeout (for reaching the controler) 0 = infinity.
		fprintf(stderr, " [ERROR] \n");
		perror("set_scan_parameters");
		goto fail;
	}
	fprintf(stderr, " [DONE]\n"); 

	fprintf(stderr, "5. Enabling scan...");
	if (hci_le_set_scan_enable(hci_socket->sock, 0x01, 0x00, 0) < 0) { // Duplicate filtering ? (cf p1069)
		fprintf(stderr, " [ERROR] \n");
		perror("set_scan_enable");
		goto fail;
	}
	fprintf(stderr, " [DONE]\n"); 

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
					

						// Address field :
						bdaddr_t *rsp_mac = (bdaddr_t *)(event_parameter +
										 num_reports*(1+1) +
										 6*i); // @ on 6 bytes.
						if (!hci_already_registered_device(*rsp_mac)) {
							uint8_t *address_type;
							// Address type field :
							address_type = (uint8_t *)(event_parameter + 1*num_reports + i);
							hci_device.mac = *rsp_mac;
							hci_device.add_type = *address_type;
							hci_compute_device_name(&hci_device);
							strcpy(hci_device.custom_name, "UNKNOWN");
							hci_register_device(hci_device);
						} else {
							hci_device = *hci_get_device(*rsp_mac);
						}

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
	if (hci_le_set_scan_enable(hci_socket->sock, 0x00, 0x00, 0) < 0) {
		fprintf(stderr, " [ERROR] \n");
		perror("set_scan_disable");
		goto fail;
	}
	fprintf(stderr, " [DONE]\n"); 

	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	} else {			
		// Restoring the old filter :
		set_hci_socket_filter(*hci_socket, &old_flt);
	}
	
	fclose(file);

	return;

 fail :
	if (new_socket) {
		close_hci_socket(hci_socket);
		free(hci_socket);
	}
	if (file) {
		fclose(file);
	}
	return;
}

