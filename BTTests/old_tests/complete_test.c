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

/*struct hci_filter {
         unsigned long type_mask;
         unsigned long event_mask[2];
         __le16 opcode;
 };
*/

/*#define OCF_INQUIRY             0x0001
 struct hci_cp_inquiry { // cf spec' p706
         __u8     lap[3]; // LAP = Link Access Procedure (Protocol ?)
         __u8     length; // Inquiry Duration : length*1.28s, max duration : 0x30*1.28s.
         __u8     num_rsp; // Max responses number (0 => unlimitated)
 } __attribute__ ((packed));
*/

/*struct hci_event_hdr { // Stands for event_header ?
         __u8    evt;
         __u8    plen;
 } __packed;
*/

/*#define HCI_EV_INQUIRY_RESULT_WITH_RSSI 0x22
 struct inquiry_info_with_rssi {
         bdaddr_t bdaddr;
         __u8     pscan_rep_mode;
         __u8     pscan_period_mode;
         __u8     dev_class[3];
         __le16   clock_offset;
         __s8     rssi;
 } __packed;
*/

#define NAME_LENGTH 50

struct rssi_t {
	bdaddr_t device_mac;
	int16_t rssi;
};

typedef struct {
	bdaddr_t mac;
	char name[NAME_LENGTH];
} device_info;

typedef struct {
	device_info *di;
	uint16_t length;
} device_table;

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

	if (poll(&p, 1, -1) > 0) {
		int len = read(hci_sock.sock, buf, sizeof(buf));

		switch (buf[1]) {
		case EVT_CMD_COMPLETE:
			return 0;
			break;
		default:
			fprintf(stderr, "Unknown event...\n");
			return -1;
			break;
		}
	}
}

// Inquiry duration = scan_duration*1.28s
device_table *scan_devices(uint8_t scan_duration, uint8_t max_rsp, long flags) {

	// Creation of the inquiry_info table :
	inquiry_info *ii = calloc(max_rsp, sizeof(inquiry_info));
	
	// Creation of the descriptor and opening of an hci socket :
	hci_socket hci_sock = open_hci_socket(NULL);
	if (hci_sock.sock < 0) {
		free(ii);
		return NULL;
	}

	// Starting the inquiry :
	fprintf(stderr, "Starting the scanning inquiry...");
	int nb_rsp = hci_inquiry(hci_sock.dev_id, scan_duration, max_rsp, NULL, &ii, flags);
	if(nb_rsp < 0) {
		perror("hci_inquiry");
		free(ii);
		close_hci_socket(hci_sock);
		fprintf(stderr, " [ERROR]\n");
		return NULL;
	}
	fprintf(stderr, " [DONE]\n"); // PAS DONE ICI, IL FAUDRAIT LE METTRE QUAND ON A RECU L'EVENT DE FIN DE COMMAND !!! (ICI, INQUIRY COMPLETE !).
	
	device_table *ditable = (device_table *)malloc(sizeof(device_table));
	device_info *result = calloc(nb_rsp, sizeof(device_info));

	for (uint16_t i = 0; i < nb_rsp; i++) {
		result[i].mac = ii[i].bdaddr;
		memset(result[i].name, 0, NAME_LENGTH);
		if (hci_read_remote_name(hci_sock.sock, &(ii[i].bdaddr), NAME_LENGTH, result[i].name, 0) < 0) {
			strcpy(result[i].name, "[UNKNOWN]");
		}
	}

	free(ii);
	close_hci_socket(hci_sock); 

	ditable->di = result;
	ditable->length = nb_rsp;

	return ditable;
}


void display_device_info(device_info di) {
	char tmp[18]; 
	ba2str(&(di.mac), tmp);
	fprintf(stdout, "[%s] : %16s \n", tmp, di.name);
}

void getRSSI(bdaddr_t *mac, uint8_t duration, uint8_t nb_rsp) {	

	struct rssi_t result;
	//result.device_mac = *mac;

	// HCI_Filter structure :
	struct hci_filter flt;	
	struct hci_filter old_flt;

	// HCI socket ids :
	hci_socket hci_sock;

	// Command parameter for an "OCF_WRITE_INQUIRY_MODE" :
	write_inquiry_mode_cp write_cp;

	// Command parameter for a (standard ?) inquiry : "OCF_INQUIRY" :
	inquiry_cp cp;
	memset (&cp, 0, sizeof(cp));

	/* Events buffer : table of unsigned char (range 0-255) allowing us
	   to catch an HCI-event packet coming in response to one of our
	   commands packet (thanks to the "read()" function).
	*/
	unsigned char buf[HCI_MAX_EVENT_SIZE]; // HCI_MAX_EVENT_SIZE      260
	unsigned char *event_parameter;
 
	hci_event_hdr event_header;

	// Inquiry allowing us to deal with a RSSI value :
	inquiry_info_with_rssi *info_rssi;

	// Opening a socket :
	hci_sock = open_hci_socket(NULL);
	if (hci_sock.sock < 0) {
		return;
	}

	// Creating our filter :
	hci_filter_clear(&flt); 
	// HCI_EVENT_PKT = HCI Event Packet.
	hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
	// hci_filter_set_event(EVT_INQUIRY_RESULT, &flt); Not needed yet, we only need an inquiry with RSSI.
	hci_filter_set_event(EVT_INQUIRY_RESULT_WITH_RSSI, &flt);
	hci_filter_set_event(EVT_INQUIRY_COMPLETE, &flt);
	hci_filter_set_event(EVT_CMD_COMPLETE, &flt);
	//hci_filter_all_events(&flt);

	/* Saving the old filter.
	   This can be helpful in a case where we want to use a same socket for different purposes
	   (even with multiple threads), we can replace the old filter and re-use it later.
	*/
	socklen_t old_flt_len = sizeof(old_flt);
	if (getsockopt(hci_sock.sock, SOL_HCI, HCI_FILTER, (void *)&old_flt, &old_flt_len) < 0) {
		perror("Cannot save the old filter");
	}
	
	// Applying the new filter :
	if (setsockopt(hci_sock.sock, SOL_HCI, HCI_FILTER, (void *)&flt, sizeof(flt)) < 0) {
		perror("Can't set HCI filter");
		return;
	}

	// Configuring the inquiry mode on the open HCI socket :
	fprintf(stderr, "Configuring the inquiry mode...");
	write_cp.mode = 0x01; // Inquiry Result format with RSSI (cf p878 spec').
	if (hci_send_cmd(hci_sock.sock, OGF_HOST_CTL, OCF_WRITE_INQUIRY_MODE, 
			 WRITE_INQUIRY_MODE_CP_SIZE, &write_cp) < 0) {
		perror("Can't set inquiry mode");
		return;
	}
	if (check_cmd_complete(hci_sock) == 0) { // ATTENTION : ON RECOIT PEUT ETRE CE PAQUET INSTANTANEMENT (A TESTER), IL FAUDRAIT DONC ENVOYER LA REQUETE LORSQU'ON A DEJA INITIALISE LE POLL, POUR QU'IL PUISSE CAPTER UN EVENTUEL EVENT AU MOMENT OU ON ENVOIE LA CMD !!!!!!!!!!!!!
		fprintf(stderr, " [DONE]\n"); 
	}

	// Setting the command parameters for our rssi inquiry :
	cp.lap[2] = 0x9e; 
	cp.lap[1] = 0x8b;
	cp.lap[0] = 0x33;
	cp.num_rsp = nb_rsp; 
	cp.length = duration; 


	printf("Starting inquiry with RSSI...\n");

	// Sending the inquiry's command to the HCI socket :
	if (hci_send_cmd(hci_sock.sock, OGF_LINK_CTL, OCF_INQUIRY, INQUIRY_CP_SIZE, &cp) < 0) {
		perror("Can't start inquiry");
		return;
	}

	// Connection poll structure, not initialized yet :
	struct pollfd p;
	// Initializing the connection poll on our HCI socket :
	p.fd = hci_sock.sock;
	p.events = POLLIN | POLLERR | POLLHUP;

	char canceled = 0;

	while(!canceled) {
		p.revents = 0;

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
	
			event_parameter = buf + HCI_EVENT_HDR_SIZE + 1;
			
			int nb_results = event_parameter[0];
			event_parameter = (void *)(event_parameter + 1); // We switch to the next byte.

			// MAINTENIR UNE TABLE DES DEVICES DEJA RENCONTRES POUR NE PAS A AVOIR A CHAQUE FOIS A RECRER DES "device_info"...

			switch (event_header.evt) {
			case EVT_INQUIRY_RESULT_WITH_RSSI: // Code 0x22 
				for (uint16_t i = 0; i < nb_results; i++) {
					device_info di;
					char tmp[18]; 

					bdaddr_t *rsp_mac = (bdaddr_t *)(event_parameter + 6*i); // @ on 6 bytes.
					di.mac = *rsp_mac;

					/* To understand the meaning of the following offsets, please refer to
					   the spec' p.1002. The structure of the HCI Packet "inquiry result with
					   RSSI" is shown and explained. To sum up, during the inquiry period,
					   several devices may respond and one event packet may contain several
					   informations. But these informations are not stored device per device
					   but rather "feature by feature". That is to say that the event packet
					   contains first all of the mac@, then all of the 
					   "page_scan_repetition_mode" etc...
					*/
					int8_t *rssi = (int8_t *)(event_parameter + (6+1+1+3+2)*nb_results + i); 

					ba2str(&(di.mac), tmp);
					fprintf(stdout, "[%s] : %idb \n", tmp, *rssi);
				}
				break;

			case EVT_INQUIRY_COMPLETE:
				fprintf(stderr, "Inquiry complete ! \n");
				canceled = 1;
				break;
				
			default:
				fprintf(stderr, "An unknown event occurred... \n");
				break;
			}
			
		}
	}

	close_hci_socket(hci_sock);
}


int main(int argc, char **argv)
{
	device_table *ditable = NULL; 
	fprintf(stdout, "--------------------\n");
	fprintf(stdout, "- DEVICES SCANNING -\n");
	fprintf(stdout, "--------------------\n\n");
	ditable = scan_devices(5, 50, IREQ_CACHE_FLUSH); // On scan 50 devices max durant 5*1.28s
	if (ditable == NULL) {
		fprintf(stderr, "Error while scanning devices... \n");
		return EXIT_FAILURE;
	}
	for (uint16_t i = 0; i < ditable->length; i++) {
		display_device_info(ditable->di[i]);
	}

	fprintf(stdout, "\n--------------------\n");
	fprintf(stdout, "- RSSI MEASUREMENT -\n");
	fprintf(stdout, "--------------------\n\n");
	// On va recevoir 10 mesures de RSSI max durant 7*1.28s
	getRSSI(NULL, 7, 10);

	free(ditable->di);
	free(ditable);

	return EXIT_SUCCESS;
}

/* 
   /!\ A AMELIORER : * chaque fonction ouvre une nouvelle socket !! /!\
   * Struct device_table adaptée ?
   * Passer en C++ !
   * Factoriser...
   * Faire une liste/table des sockets ouvertes (+ correspondance avec device traité) ?

 */
