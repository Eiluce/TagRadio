/* 
 * File:   matriceCalibration.cpp
 * Author: Cédric
 * 
 * Created on 25 mars 2014, 18:00
 */

#include <stdlib.h>
#include <math.h>

#include "matriceCalibration.h"
#define NB_SENSORS 4

/**
 * Constructeur de matriceCalibration.
 * @param nbLines Nombre de lignes de la matrice.
 * @param nbColumns Nombre de colonnes de la matrice.
 */
MatriceCalibration::MatriceCalibration(int nbLines, int nbColumns) : nbLines(nbLines), nbColumns(nbColumns) {
    matrice = vector<double*>(nbLines * nbColumns);
}

/**
 * Destructeur de matriceCalibration. 
 */
MatriceCalibration::~MatriceCalibration() {
    for (int i = 0; i < this->matrice.size(); i++) {
        free(this->matrice[i]);
    }
}

/**
 * Retourne le nombre de lignes de la matrice de calibration.
 * @return L'attribut nbLines.
 */
int MatriceCalibration::getNbLines() {
    return this->nbLines;
}

/**
 * Retourne le nombre de colonnes de la matrice de calibration.
 * @return L'attribut nbColumns.
 */
int MatriceCalibration::getNbColumns() {
    return this->nbColumns;
}

/**
 * Retourne le vector correspondant à la matrice de calibration.
 * @return L'attribut matrice.
 */
vector<double*> MatriceCalibration::getMatrice() {
    return this->matrice;
}

/**
 * Retourne le tableau de double de la case à la ligne line et à la colonne column.
 * Première ligne = ligne 0.
 * Première collonne = colonne 0.
 * @param line Ligne de la case voulue.
 * @param column Colonne de la case voulue.
 * @return Le tableau voulu.
 * @return NULL si la case est vide ou n'existe pas.
 */
double* MatriceCalibration::getElement(int line, int column) {
    try {
        if (line >= this->getNbLines() || column >= this->getNbColumns()
                || line < 0 || column < 0) {
            throw 0;
        }
    } catch (int code) {
        cerr << "Error: MatriceCalibration::getElement out of bounds.\n";
        return NULL;
    }
    return this->matrice[line * this->getNbColumns() + column];
}

/**
 * Insère elem dans la case à la ligne line et la colonne column.
 * Première ligne = ligne 0.
 * Première collonne = colonne 0.
 * L'insertion est par copie.
 * @param line Ligne de la case visée.
 * @param column Colonne de la case visée.
 * @param elem Element à mettre dans la case.
 * @return 0 si l'insertion c'est bien passée.
 * @return 1 si l'insertion a échoué.
 */
int MatriceCalibration::setElement(int line, int column, double* elem) {
    try {
        if (line >= this->getNbLines() || column >= this->getNbColumns()
                || line < 0 || column < 0) {
            throw 0;
        }
    } catch (int code) {
        cerr << "Error: MatriceCalibration::setElement out of bounds.\n";
        return 1;
    }
    int numCase = line * this->getNbColumns() + column;
    this->matrice[numCase] = (double*) calloc(NB_SENSORS + 1, sizeof (double_t));
    double* matriceCase = this->matrice[numCase];
    for (int i = 0; i < NB_SENSORS; i++) {
        matriceCase[i] = elem[i];
    }
    return 0;
}

/**
 * Calcule la distance entre deux vecteurs.
 * @param v1 Le premier vecteur.
 * @param v2 Le second vecteur.
 * @return La distance entre v1 et v2.
 */
double MatriceCalibration::distance(double* v1, double* v2) {
    double dist = 0;
    for (int i = 0; i < NB_SENSORS; i++) {
        dist += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    return sqrt(dist);
}

/**
 * Cherche la case correspondant le plus au vecteur mesure.
 * @param mesure La mesure faite.
 * @return Les coordonnées de la case la plus probable.
 */
pair<int, int> MatriceCalibration::bestPosition(double* mesure) {
    pair<int, int> coordBestCase;
    int bestCase;
    double distMin = INFINITY;
    double distCour;
    int matSize = this->matrice.size();
    for (int i = 0; i < matSize; i++) {
        if (this->matrice[i] != NULL) {
            distCour = MatriceCalibration::distance(mesure, this->matrice[i]);
            if (distCour < distMin) {
                distMin = distCour;
                bestCase = i;
            }
        }
    }
    coordBestCase.first = bestCase / this->getNbColumns();
    coordBestCase.second = bestCase % this->getNbColumns();

    return coordBestCase;
}

/**
 * Met à jour la distance de chaque case par rapport à le mesure.
 * La distance est stoquée dans la dernière case de chaque tableau.
 * @param mesure La mesure faite.
 * @return 
 */
void MatriceCalibration::setDistances(double* mesure) {
    int matSize = this->matrice.size();
    for (int i = 0; i < matSize; i++) {
        if (this->matrice[i] != NULL) {
            this->matrice[i][NB_SENSORS] = MatriceCalibration::distance(this->matrice[i], mesure);
        }
    }
}