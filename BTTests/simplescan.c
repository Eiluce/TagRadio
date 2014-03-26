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
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };

    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }

    len  = 4;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    
    //  for (uint8_t j = 0; j < 3; j++) {
    //	    printf("Making the %u try...", j+1);
	    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	    printf(" [DONE]\n");
	    if( num_rsp < 0 ) perror("hci_inquiry");

	    for (i = 0; i < num_rsp; i++) {
		    ba2str(&(ii+i)->bdaddr, addr);
		    memset(name, 0, sizeof(name));
		    if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
					     name, 0) < 0)
			    strcpy(name, "[unknown]");
		    printf("-> %s  %s\n", addr, name);
		    printf("  * uint8_t pscan_rep_mode : %u\n", ii[i].pscan_rep_mode);
		    printf("  * uint8_t pscan_period_mode : %u\n", ii[i].pscan_period_mode);
		    printf("  * uint8_t pscan_mode : %u\n", ii[i].pscan_mode);
		    printf("  * uint8_t dev_class[3] : \n");
		    printf("    - uint8_t dev_class[0] : %u\n", ii[i].dev_class[0]);
		    printf("    - uint8_t dev_class[1] : %u\n", ii[i].dev_class[1]);
		    printf("    - uint8_t dev_class[2] : %u\n", ii[i].dev_class[2]);
		    printf("  * uint16_t clock_offset %u\n", ii[i].clock_offset);
	    }
	    //  }

    free( ii );
    close( sock );
    return 0;
}
