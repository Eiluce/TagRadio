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
} di_table;

// Inquiry duration = scan_duration*1.28s
di_table *scan_devices(uint8_t scan_duration, uint8_t max_rsp, long flags) {

	// Creation of the inquiry_info table :
	inquiry_info *ii = calloc(max_rsp, sizeof(inquiry_info));
	
	// Creation of the descriptor and opening of an hci socket :
	int dev_id = hci_get_route(NULL); // NULL => we take the first present BT adaptator.
	int sock = hci_open_dev(dev_id);
	if (dev_id < 0 || sock < 0) {
		perror("opening socket");
		free(ii);
		return NULL;
	}

	// Starting the inquiry :
	fprintf(stderr, "Starting the inquiry...");
	int nb_rsp = hci_inquiry(dev_id, scan_duration, max_rsp, NULL, &ii, flags); 
	if( nb_rsp < 0 ) {
		perror("hci_inquiry");
		free(ii);
		close(sock);
		fprintf(stderr, " [ERROR]\n");
		return NULL;
	}
	fprintf(stderr, " [DONE]\n");
	
	di_table *ditable = (di_table *)malloc(sizeof(di_table));
	device_info *result = calloc(nb_rsp, sizeof(device_info));

	for (uint16_t i = 0; i < nb_rsp; i++) {
		result[i].mac = ii[i].bdaddr;
		memset(result[i].name, 0, NAME_LENGTH);
		if (hci_read_remote_name(sock, &(ii[i].bdaddr), NAME_LENGTH, result[i].name, 0) < 0) {
			strcpy(result[i].name, "[UNKNOWN]");
		}
	}

	free(ii);
	close(sock); 

	ditable->di = result;
	ditable->length = nb_rsp;

	return ditable;
}


void display_device_info(device_info di) {
	char tmp[18]; 
	ba2str(&(di.mac), tmp);
	fprintf(stdout, "[%s] : %16s \n", tmp, di.name);
}

void getRSSI(bdaddr_t mac, uint8_t duration, uint8_t nb_rsp) {	

	struct rssi_t result;
	result.device_mac = mac;

	// HCI_Filter structure :
	struct hci_filter flt;	
	struct hci_filter old_flt;

	// HCI socket ids :
	int dev_id, sock = 0;

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
	dev_id = hci_get_route(NULL);
	sock = hci_open_dev(dev_id);
	if (dev_id < 0 || sock < 0) {
		perror("Can't open socket");
		return;
	}

	// Creating our filter :
	hci_filter_clear(&flt); // PKT stands for packet ?
	hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
	// hci_filter_set_event(EVT_INQUIRY_RESULT, &flt); Not needed yet, we only need an inquiry with RSSI.
	hci_filter_set_event(EVT_INQUIRY_RESULT_WITH_RSSI, &flt);
	hci_filter_set_event(EVT_INQUIRY_COMPLETE, &flt);
	//hci_filter_all_events(&flt);

	/* Saving the old filter.
	   This can be helpful in a case where we want to use a same socket for different purposes
	   (even with multiple threads), we can replace the old filter and re-use it later.
	*/
	socklen_t old_flt_len = sizeof(old_flt);
	if (getsockopt(sock, SOL_HCI, HCI_FILTER, (void *)&old_flt, &old_flt_len) < 0) {
		perror("Cannot save the old filter");
	}
	
	// Applying the new filter :
	if (setsockopt(sock, SOL_HCI, HCI_FILTER, (void *)&flt, sizeof(flt)) < 0) {
		perror("Can't set HCI filter");
		return;
	}

	// Configuring the inquiry mode on the open HCI socket :
	write_cp.mode = 0x01; // Inquiry Result format with RSSI (cf p878 spec').
	if (hci_send_cmd(sock, OGF_HOST_CTL, OCF_WRITE_INQUIRY_MODE, WRITE_INQUIRY_MODE_CP_SIZE, &write_cp) < 0) {
		perror("Can't set inquiry mode");
		return;
	}

	// Setting the command parameters for our rssi inquiry :
	cp.lap[2] = 0x9e; 
	cp.lap[1] = 0x8b;
	cp.lap[0] = 0x33;
	cp.num_rsp = nb_rsp; 
	cp.length = duration; 


	printf("Starting inquiry with RSSI...\n");

	// Sending the inquiry's command to the HCI socket :
	if (hci_send_cmd (sock, OGF_LINK_CTL, OCF_INQUIRY, INQUIRY_CP_SIZE, &cp) < 0) {
		perror("Can't start inquiry");
		return;
	}

	// Connection poll structure, not initialized yet :
	struct pollfd p;
	// Initializing the connection poll on our HCI socket :
	p.fd = sock;
	p.events = POLLIN | POLLERR | POLLHUP;

	char canceled = 0;

	while(!canceled) {
		p.revents = 0;

		// Polling the BT device for an event :
		if (poll(&p, 1, -1) > 0) {

			// If an event occured, we store the HCI Event packet into our buffer :
			int len = read(sock, buf, sizeof(buf));

			if (len < 0)
				continue;
			else if (len == 0)
				break;

			// WHAT IS buf[0] ? Maybe a parameter added by "read()" ???
			event_header.evt = buf[1];
			event_header.plen = buf[2];
	
			event_parameter = buf + HCI_EVENT_HDR_SIZE + 1;
			
			int nb_results = event_parameter[0];

			switch (event_header.evt) {
			case EVT_INQUIRY_RESULT_WITH_RSSI:
				for (uint16_t i = 0; i < nb_results; i++) {
					// Retrieving the rssi info :
					info_rssi = (inquiry_info_with_rssi *)&(event_parameter[i+1]);
					// Creating the device info :
					device_info di;
					di.mac = info_rssi->bdaddr;
					display_device_info(di);
					fprintf(stdout, "%i \n", info_rssi->rssi);
				}
				break;

			case EVT_INQUIRY_COMPLETE:
				fprintf(stderr, "Inquiry complete ! \n");
				canceled = 1;
				break;
			}
		}
	}

	close(sock);
}


int main(int argc, char **argv)
{
	di_table *ditable = NULL; 
	ditable = scan_devices(5, 50, IREQ_CACHE_FLUSH); // On scan 50 devices max durant 5*1.28s
	if (ditable == NULL) {
		fprintf(stderr, "Error while scanning devices... \n");
		return EXIT_FAILURE;
	}
	for (uint16_t i = 0; i < ditable->length; i++) {
		display_device_info(ditable->di[i]);
	}

	// On va recevoir 10 mesures de RSSI max durant 7*1.28s
	getRSSI(ditable->di[0].mac, 7, 10);

	free(ditable->di);
	free(ditable);

	return EXIT_SUCCESS;
}

/* 
   /!\ A AMELIORER : * chaque fonction ouvre une nouvelle socket !! /!\
   * Struct di_table adaptée ?
   * Passer en C++ !
   * Factoriser...


 */
