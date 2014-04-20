#ifndef MATRICE_H
#define	MATRICE_H

#define NB_CAPTEURS 4
#define NB_MESURES 5

int timeReference;

/**
 * Ce que contient chaque case de la matrice.
 */
struct Valeurs {
	double table[NB_CAPTEURS+1];
};

/**
 * La matrice.
 */
struct Matrice {
	int nbLines;
	int nbColumns;
	struct Valeurs **val;
};

/**
 * Un point de la matrice. Avec la "proba" d'être sur ce point.
 */
struct Point {
	int x;
	int y;
        double proba;
};

struct listPoints{
    struct Point point;
    struct listPoints* next;
};

struct Matrice *CreateMatrice(int nbLines,int nbColumns);

void afficherValeurs(struct Valeurs v);

void afficherMatrice(struct Matrice *m);

struct Valeurs getElement(struct Matrice *m,int line, int column);

void setElement(struct Matrice *m, int line, int column, struct Valeurs elem);

struct Point *bestPosition(struct Matrice *m,struct Valeurs* mesure);

void setDistances(struct Matrice *m,struct Valeurs* mesure);

int* getMesures(char* mesures);

double moyenneMesures(int* mesures);

void generateData(struct Matrice* m, struct Valeurs* mesure, const char* nomFichier);

#endif
