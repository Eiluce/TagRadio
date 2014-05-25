#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
/*
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
 */
#include "matrice.h"

int main() {
    timeReference = time(NULL);

    struct Matrix *m = create_matrix(5, 4, 5);

    struct Values val;
    val.table = calloc(4, sizeof(double));
    val.table[0] = -1;
    val.table[1] = -2;
    val.table[2] = -3;
    val.table[3] = -4;

    set_element(m, 1, 1, val);

    struct Values res = get_element(m, 1, 1);

    struct Values v1;
    v1.table = calloc(4, sizeof(double));
    struct Values v2;
    v2.table = calloc(4, sizeof(double));
    v1.table[0] = -1;
    v1.table[1] = -2;
    v1.table[2] = -3;
    v1.table[3] = -4;
    v2.table[0] = -1;
    v2.table[1] = -2;
    v2.table[2] = -3;
    v2.table[3] = -5;

    set_element(m, 1, 3, v1);
    set_element(m, 0, 2, v2);

    struct Values res1;
    res1 = get_element(m, 1, 3);

    v1.table[2] = 0;

    //Test de la recherche de meilleure position.

    struct Point *bestPos;
    bestPos = best_position(m, &v1);
    printf("Point x : %i\n", bestPos->x);
    printf("Point y : %i\n", bestPos->y);

    printf("setDistance avec elem1\n");
    //set_distances(m, &v1);
    printf("Distance écrite dans (0,2): %f\n", get_element(m, 0, 2).table[4]);
    printf("setDistance avec elem2\n");
    //set_distances(m, &v2);
    printf("Distance écrite dans (0,2): %f\n", get_element(m, 0, 2).table[4]);
    
    printf("Test de conversion de mesures brutes:\n");
    int* mesures = get_measures("-12;-13;-152;-112;-08");
    printf("On devrait lire -12 -13 -152 -112 -8:\n");
    for (int i = 0; i < NB_MESURES; i++) {
        printf("%i ", mesures[i]);
    }
    printf("\n");

    generate_data(m, &v2, "testMatrice.txt");

    sleep(3);

    generate_data(m, &v1, "testMatrice.txt");
    
    sleep(2);
    
    generate_data_from_measures(m, "testMatrice.txt", "-1;-1;-1;-1;-1", "-2;-2;-2;-2;-2", "-3;-3;-3;-3;-3", "-4;-4;-4;-4;-4");
    
    printf("\nData générées dans testMatrice.txt\n");

    return 0;
}
