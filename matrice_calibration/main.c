#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
/*
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
 */
#include "matrice.h"

int main() {
    timeReference = time(NULL);

    struct Matrice *m = CreateMatrice(5, 4);

    struct Valeurs val;
    val.table[0] = 1;
    val.table[1] = 2;
    val.table[2] = 3;
    val.table[3] = 4;

    setElement(m, 1, 1, val);

    struct Valeurs res = getElement(m, 1, 1);

    struct Valeurs v1;
    struct Valeurs v2;
    v1.table[0] = 1;
    v1.table[1] = 2;
    v1.table[2] = 3;
    v1.table[3] = 4;
    v2.table[0] = 1;
    v2.table[1] = 2;
    v2.table[2] = 3;
    v2.table[3] = 5;

    setElement(m, 1, 3, v1);
    setElement(m, 0, 2, v2);

    struct Valeurs res1;
    res1 = getElement(m, 1, 3);

    v1.table[2] = 0;

    //Test de la recherche de meilleure position.

    struct Point *bestPos;
    bestPos = bestPosition(m, &v1);
    printf("Point x : %i\n", bestPos->x);
    printf("Point y : %i\n", bestPos->y);

    printf("setDistance avec elem1\n");
    setDistances(m, &v1);
    printf("Distance écrite dans (0,2): %f\n", getElement(m, 0, 2).table[4]);
    printf("setDistance avec elem2\n");
    setDistances(m, &v2);
    printf("Distance écrite dans (0,2): %f\n", getElement(m, 0, 2).table[4]);

    generateData(m, &v2, "testMatrice.txt");

    sleep(3);

    generateData(m, &v1, "testMatrice.txt");
    
    printf("Data générées dans testMatrice.txt\n");

    return 0;
}
