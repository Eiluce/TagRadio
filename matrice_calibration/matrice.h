#ifndef MATRICE_H
#define	MATRICE_H

#define NB_CAPTEURS 4

struct Valeurs {
	double table[NB_CAPTEURS];
};


struct Matrice {
	int nbLines;
	int nbColumns;
	struct Valeurs **val;
};

struct Point {
	int x;
	int y;
};

struct Matrice *CreateMatrice(int nbLines,int nbColumns);

void afficherValeurs(struct Valeurs v);

void afficherMatrice(struct Matrice *m);

struct Valeurs getElement(struct Matrice *m,int line, int column);

void setElement(struct Matrice *m, int line, int column, struct Valeurs elem);

static double distance(struct Valeurs *v1, struct Valeurs* v2);

struct Point *bestPosition(struct Matrice *m,struct Valeurs* mesure);

void setDistances(struct Matrice *m,struct Valeurs* mesure);

    

    

#endif