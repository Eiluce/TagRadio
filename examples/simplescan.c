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

    dev_id = hci_get_route(NULL); // On récupère un descripteur sur le premier adaptateur bluetooth présent.
    sock = hci_open_dev( dev_id ); // On ouvre une socket sur ledit descripteur.
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }

    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH; // On demande à "flusher" le cache avant d'effectuer une nouvelle recherche.
    // Si l'on avait pas fait ça, on aurait pu avoir des devices fantômes.
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info)); // On créé un tableau d'inquiry_info de taille max_rsp afin de pouvoir recevoir
    // le nombre maximal de réponses attendues.
    
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags); //On lance la requête.
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) { // On analyse les résultats.
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
            name, 0) < 0)
        strcpy(name, "[unknown]");
        printf("%s  %s\n", addr, name);
    }

    free( ii );
    close( sock ); // On ferme la socket.
    return 0;
}
