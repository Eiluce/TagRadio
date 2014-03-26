/* 
 * File:   matriceCalibration.h
 * Author: Cédric
 *
 * Created on 25 mars 2014, 18:00
 */

#ifndef MATRICECALIBRATION_H
#define	MATRICECALIBRATION_H
#include <vector>
#include <iostream>
using namespace std;


class MatriceCalibration {
    
    /*Méthodes publiques*/
public:
    MatriceCalibration(int nbLines = 0, int nbColumns = 0);
    virtual ~MatriceCalibration();
    int getNbLines();
    int getNbColumns();
    vector<double*> getMatrice();
    double* getElement(int line, int column);
    int setElement(int line, int column, double* elem);
    pair<int, int> bestPosition(double* mesure);
    
    /*Méthodes privées*/
private:
    static double distance(double* v1, double* v2);
    
    /*Attributs privés*/
private:
    vector<double*> matrice;
    int nbColumns;
    int nbLines;

};

#endif	/* MATRICECALIBRATION_H */

