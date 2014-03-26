#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main(int argc, char **argv)
{
    inquiry_info *ii = NULL;
    struct hci_conn_info_req *cr;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    int8_t rssi;
    char addr[19] = { 0 };
    char name[248] = { 0 };

    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }

    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);

    if( num_rsp < 0 ) 
    perror("hci_inquiry");

    hci_write_inquiry_transmit_power_level(sock, 0.1, 1000);

    cr = malloc(sizeof(*cr) + sizeof(struct hci_conn_info));
    if (!cr) {
        perror("Can't allocate memory");
        exit(1);
    }

    bacpy(&cr->bdaddr, &(ii+i)->bdaddr);
    cr->type = ACL_LINK;

    for (i = 0; i < num_rsp; i++) {
        hci_read_rssi(sock, htobs(cr->conn_info->handle), &rssi, 1000);
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
            name, 0) < 0) {
        	strcpy(name, "[unknown]");
    	}
        printf("%s  %s  %i\n", addr, name, rssi);
    }

    free( ii );
    close( sock );
    return 0;
}