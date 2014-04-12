#include "matriceCalibration.h"
#include <string>
#include <unistd.h>
#include <iostream>

using namespace std;


int main(int argc, char *argv[])
{
    


    //Initialisation d'une matrice 5x4.
    MatriceCalibration* m = new MatriceCalibration(5, 4);
    cout << "nb éléms:" << m->getMatrice().size() << endl;

    double elem1[4] = {1, 2, 3, 4};
    double elem2[4] = {1, 2, 3, 5};
    m->setElement(1, 3, elem1);
    m->setElement(0, 2, elem2);

    double* get = m->getElement(1, 3);
    //Vérification que le get a fonctionné.
    if (get == NULL) return 1;
    //Vérification que le set est bien par copie.
    elem1[2] = 0;
    cout << "Récupération de la case (2,3):" << endl;
    cout << get[0] << " " << get[1] << " " << get[2] << " " << get[3] << endl;

    //Test de la recherche de meilleure position.
    pair<int, int> bestPos = m->bestPosition(elem1);
    cout << "Meilleur position: ligne: " << bestPos.first << " colonne: " << bestPos.second << endl;

    cout << "setDistance avec elem1\n";
    m->setDistances(elem1);
    cout << "Distance écrite dans (0,2): " << (m->getElement(0,2))[4] << endl;
    cout << "setDistance avec elem2\n";
    m->setDistances(elem2);
    cout << "Distance écrite dans (0,2): " << (m->getElement(0,2))[4] << endl;

    //Libération des ressources.
    delete m;
    return 0;
}
