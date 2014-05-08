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

extern struct Matrice *CreateMatrice(int nbLines,int nbColumns);

extern void insertVal(struct Matrice *matrice, int i, int j, int capteur, char *mesures);

extern void afficherValeurs(struct Valeurs v);

extern void afficherMatrice(struct Matrice *m);

extern struct Valeurs getElement(struct Matrice *m,int line, int column);

extern void setElement(struct Matrice *m, int line, int column, struct Valeurs elem);

extern struct Point *bestPosition(struct Matrice *m,struct Valeurs* mesure);

extern void setDistances(struct Matrice *m,struct Valeurs* mesure);

extern int* getMesures(char* mesures);

extern double moyenneMesures(int* mesures);

extern void generateData(struct Matrice* m, struct Valeurs* mesure, const char* nomFichier);

extern void generateDataFromMesures(struct Matrice* m, const char* nomFichier,
        char* mesures1, char* mesures2, char* mesures3, char* mesures4);

#endif
