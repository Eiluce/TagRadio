#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

static void print_result(bdaddr_t *bdaddr, char has_rssi, int rssi)
{
	char addr[18];

	ba2str(bdaddr, addr);

	printf("%17s", addr);
	if(has_rssi)
		printf(" RSSI:%d", rssi);
	else
		printf(" RSSI:n/a");
	printf("\n");
	fflush(NULL);
}


static void scanner_start()
{
	int dev_id, sock = 0;
	struct hci_filter flt;
	write_inquiry_mode_cp cp2;
	cp2.mode = 0x01;
	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	hci_event_hdr *hdr;
	char canceled = 0;
	inquiry_info_with_rssi *info_rssi;
	inquiry_info *info;
	int results, i, len;
	struct pollfd p;

	dev_id = hci_get_route(NULL);
	sock = hci_open_dev( dev_id );
	if (dev_id < 0 || sock < 0) {
		perror("Can't open socket");
		return;
	}

	hci_filter_clear(&flt); // PKT stands for packet ?
	hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
	hci_filter_set_event(EVT_INQUIRY_RESULT, &flt);
	hci_filter_set_event(EVT_INQUIRY_RESULT_WITH_RSSI, &flt);
	hci_filter_set_event(EVT_INQUIRY_COMPLETE, &flt);
	//hci_filter_all_events(&flt);
	struct hci_filter old_flt;
	socklen_t old_flt_len = sizeof(old_flt);
	if (getsockopt(sock, SOL_HCI, HCI_FILTER, (void *)&old_flt, &old_flt_len) < 0) {
		perror("Cannot save the old filter");
	}
	if (setsockopt(sock, SOL_HCI, HCI_FILTER, (void *)&flt, sizeof(flt)) < 0) {
		perror("Can't set HCI filter");
		return;
	}

	if (hci_send_cmd(sock, OGF_HOST_CTL, OCF_WRITE_INQUIRY_MODE, WRITE_INQUIRY_MODE_CP_SIZE, &cp2) < 0) {

		perror("Can't set inquiry mode");
		return;
	}

	inquiry_cp cp;
	//	memset (&cp, 0, sizeof(cp));
	cp.lap[2] = 0x9e; 
	cp.lap[1] = 0x8b;
	cp.lap[0] = 0x33;
	cp.num_rsp = 1; // Nombre de réponses illimité
	cp.length = 0x05; // Durée de l'inquiry : length*1.28s

	printf("Starting inquiry with RSSI...\n");

	// NOTE : INQUIRY_CP_SIZE = 5 car 5 parametres !!!!
	if (hci_send_cmd (sock, OGF_LINK_CTL, OCF_INQUIRY, INQUIRY_CP_SIZE, &cp) < 0) {

		perror("Can't start inquiry");
		return;
	}

	p.fd = sock;
	p.events = POLLIN | POLLERR | POLLHUP;

	while(!canceled) {
		p.revents = 0;

		/* poll the BT device for an event */
		if (poll(&p, 1, -1) > 0) {
			len = read(sock, buf, sizeof(buf));

			if (len < 0)
				continue;
			else if (len == 0)
				break;

			// buf[0] = EVENT CODE
			hdr = (void *) (buf + 1); // hdr = @buf[1]
			// Since hdr has two 8bits fields, we can assume that buf[2] is the
			// 2nd filed of it, that's why we incr again :
			ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
			

			results = ptr[0];

			switch (hdr->evt) {
			case EVT_INQUIRY_RESULT:
				for (i = 0; i < results; i++) {
					info = (void *)ptr + (sizeof(*info) * i) + 1;
					print_result(&info->bdaddr, 0, 0);
				}
				break;

			case EVT_INQUIRY_RESULT_WITH_RSSI:
				for (i = 0; i < results; i++) {
					info_rssi = (void *)ptr + (sizeof(*info_rssi) * i) + 1;
					print_result(&info_rssi->bdaddr, 1, info_rssi->rssi);
				}
				break;

			case EVT_INQUIRY_COMPLETE:
				canceled = 1;
				break;
			}
		}
	}
	close(sock);
}

int main(int argc, char **argv)
{
	int i; 
	scanner_start();
	return 0;
}
