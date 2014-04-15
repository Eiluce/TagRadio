#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

#include "matrice.h"

struct Matrice *CreateMatrice(int nbLines,int nbColumns)
{
	struct Matrice *result = malloc(sizeof(struct Matrice));
	result->nbColumns = nbColumns;
	result->nbLines = nbLines;
	result->val = calloc(nbLines,sizeof(double *));

	for (int i = 0; i < nbLines; i++) {
		result->val[i] = calloc(nbColumns, sizeof(struct Valeurs));
		for (int j = 0; j < nbColumns; j++) {
			struct Valeurs val ;
			for (int k = 0; k < NB_CAPTEURS; k++) {
				val.table[k] = 0;
			}
			result->val[i][j] = val;
		}
	}
	return result;
}

void afficherValeurs(struct Valeurs v)
{
	for (int i =0; i < NB_CAPTEURS + 1; i++) {
		printf("Valeur : %f\n",v.table[i]);
	}
}


void afficherMatrice(struct Matrice *m)
{
	for (int i=0; i < m->nbLines; i++) {
		for (int j=0; j < m->nbColumns; j++) {
		        printf("Ligne n° :%i ",i);
			printf("Colonne n° :%i\n",j);
			afficherValeurs(m->val[i][j]);
		}
	}
}

struct Valeurs getElement(struct Matrice *m,int line, int column)
{
	return m->val[line][column];
}

void setElement(struct Matrice *m, int line, int column, struct Valeurs elem)
{
	m->val[line][column] = elem;
}

static double distance(struct Valeurs* v1, struct Valeurs* v2)
{
	double dist = 0;
    for (int i = 0; i < NB_CAPTEURS; i++) {
        dist += (v1->table[i] - v2->table[i]) * (v1->table[i] - v2->table[i]);
    }
    return sqrt(dist);
}

struct Point *bestPosition(struct Matrice *m,struct Valeurs* mesure)
{
	struct Point *coordBestCase = malloc(sizeof(struct Point));
    int bestCase;
    double distMin = INFINITY;
    double distCour;

    int matSize = m->nbColumns * m->nbLines;

    for (int i = 0; i < m->nbColumns; i++) {
    	for (int j=0; j < m->nbLines; j++) {
    		if (m->val[i] != NULL) {
            distCour = distance(mesure, &m->val[i][j]);
	            if (distCour < distMin) {
	                distMin = distCour;
	                coordBestCase->x = i;
	                coordBestCase->y = j;
	            }
        	}
    	}
    }
    //coordBestCase->x = bestCase / m->nbColumns;
    //coordBestCase->y = bestCase % m->nbColumns;

    return coordBestCase;
}

void setDistances(struct Matrice *m,struct Valeurs* mesure) {
    int matSize = m->nbColumns * m->nbLines;
    for (int i = 0; i < m->nbLines; i++) {
    	for (int j=0; j < m->nbColumns; j++) {
	        if (m->val[i] != NULL) {
	            m->val[i][j].table[NB_CAPTEURS] = distance(&m->val[i][j], mesure);
	        }
	    }
    }
}
