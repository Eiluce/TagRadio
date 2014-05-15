#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "matrice.h"

/**
 * Initialise une matrice.
 * @param nbLines Nombre de lignes.
 * @param nbColumns Nombre de colonnes.
 * @return La matrice.
 */
struct Matrice *CreateMatrice(int nbLines, int nbColumns, int nbCapteurs) {
    struct Matrice *result = calloc(1,sizeof (struct Matrice));
    result->nbColumns = nbColumns;
    result->nbLines = nbLines;	
	result->nbCapteurs = nbCapteurs;
    result->val = calloc(nbLines, sizeof (double *));

    for (int i = 0; i < nbLines; i++) {
        result->val[i] = calloc(nbColumns, sizeof (struct Valeurs));
        for (int j = 0; j < nbColumns; j++) {
		result->val[i][j].nbCapteurs = nbCapteurs;
            result->val[i][j].table = calloc(nbCapteurs, sizeof(double));
        }
    }
    return result;
}

void insertVal(struct Matrice *matrice, int i, int j, int capteur, char *mesures) {
	matrice->val[i][j].table[capteur] = moyenneMesures(getMesures(mesures));
}

struct Matrice *ouvrirMatrice(char *file) {

	FILE *fichier = fopen(file, "r");
	if (!fichier) {
		perror("open matrix");
		return NULL;
	}

	struct Matrice *res = NULL;
	int nbLines = 0;
	int nbCol = 0;
	int nbCap = 0;
	char temp[50];
	fscanf(fichier, "%i %i\n", &(nbLines), &(nbCol));
	fgets(temp, 50, fichier);
	fscanf(fichier, "%i\n\n", &(nbCap));
	fgets(temp, 50, fichier);
	res = CreateMatrice(nbLines, nbCol, nbCap);	

	int temp1, temp2;
	for (int i = 0; i < nbLines; i++) {
        	for (int j = 0; j < nbCol; j++) {
			fscanf(fichier, "%i %i\n", &temp1, &temp2);
			fgets(temp, 50, fichier);
			for (int k = 0; k < nbCap; k++) {
       			  	fscanf(fichier, "%f\n", (float *)&(res->val[i][j].table[k]));
				fgets(temp, 50, fichier);
    			}
        	}
    	}
	fclose(fichier);
	return res;

	
}

int8_t sauvegarderMatrice(char *file, struct Matrice matrice) {
	FILE *fichier = fopen(file, "w");
	if (!fichier) {
		return -1;
	}

	fprintf(fichier, "%i %i\n", matrice.nbLines, matrice.nbColumns);
	fprintf(fichier, "%i\n\n", matrice.nbCapteurs);
	for (int i = 0; i < matrice.nbLines; i++) {
        	for (int j = 0; j < matrice.nbColumns; j++) {
			fprintf(fichier, "%i %i\n", i, j);
			for (int k = 0; k < matrice.val[i][j].nbCapteurs; k++) {
       			    fprintf(fichier, "%f\n", (float)matrice.val[i][j].table[k]);
    			}
        	}
    	}
	fclose(fichier);
	return 0;
}

/**
 * Affiche les coefficients d'une structure Valeurs.
 * @param v
 */
void afficherValeurs(struct Valeurs v) {
    for (int i = 0; i < v.nbCapteurs; i++) {
        printf("Valeur : %f\n", v.table[i]);
    }
}

/**
 * Affiche toute la matrice.
 * @param m
 */
void afficherMatrice(struct Matrice *m) {
    for (int i = 0; i < m->nbLines; i++) {
        for (int j = 0; j < m->nbColumns; j++) {
            printf("Ligne n° :%i ", i);
            printf("Colonne n° :%i\n", j);
            afficherValeurs(m->val[i][j]);
        }
    }
}

/**
 * Retourne l'élément de la matrice à la ligne line et la colonne column.
 * @param m
 * @param line
 * @param column
 * @return 
 */
struct Valeurs getElement(struct Matrice *m, int line, int column) {
    return m->val[line][column];
}

/**
 * Met elem dans la case line column de la matrice m.
 * @param m
 * @param line
 * @param column
 * @param elem
 */
void setElement(struct Matrice *m, int line, int column, struct Valeurs elem) {
    m->val[line][column] = elem;
}

/**
 * Retourne la distance euclidienne entre v1 et v2.
 * @param v1
 * @param v2
 * @return 
 */
static double distance(struct Valeurs* v1, struct Valeurs* v2) {
    double dist = 0;
    for (int i = 0; i < v1->nbCapteurs; i++) {
        dist += (v1->table[i] - v2->table[i]) * (v1->table[i] - v2->table[i]);
    }
    return sqrt(dist);
}

/**
 * Retourne la position dans la matrice correspondant le plus à la mesure.
 * @param m
 * @param mesure
 * @return 
 */
struct Point *bestPosition(struct Matrice *m, struct Valeurs* mesure) {
    struct Point *coordBestCase = malloc(sizeof (struct Point));
    double distMin = INFINITY;
    double distCour;


    for (int i = 0; i < m->nbColumns; i++) {
        for (int j = 0; j < m->nbLines; j++) {
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

/**
 * Transforme les mesures brutes en un tableau d'entiers.
 * @param mesures
 * @return Le tableau d'entiers.
 */
int* getMesures(char* mesures) {
    int* res = (int*) calloc(NB_MESURES, sizeof (int));
    int tmp = 0;
    int i = 0; //position dans la chaine
    int j = 0; //position dans le tableau

    while (mesures[i] != '\0') {
        if (mesures[i] == '-') {
            i++;
            while (mesures[i] != '\0' && mesures[i] != ';') {
                tmp = tmp * 10 - ((int) mesures[i] - '0');
                i++;
            }
        }
        res[j] = tmp;
        tmp = 0;
        j++;
        if (mesures[i] != '\0') i++;
    }

    return res;
}

/**
 * Calcule la moyenne des mesures.
 * @param mesures
 * @return La moyenne.
 */
double moyenneMesures(int* mesures) {
    double res = 0;
    for (int i = 0; i < NB_MESURES; i++) {
        res += mesures[i];
    }
    res /= NB_MESURES;
    return res;
}

/**
 * Donne la taille de la liste.
 * @param list
 * @return 
 */
static int sizeList(struct listPoints* list) {
    //printf("sizeList\n");
    int size = 0;
    struct listPoints* posCour = list;
    while (posCour != NULL) {
        posCour = posCour->next;
        size++;
    }
    //printf("%i\n", size);
    return size;
}

/**
 * Enlève la tête de la liste.
 * Retourne la distance de l'élément enlevé.
 * @param list
 */
static double removeHead(struct listPoints** list) {
    //printf("removeHead\n");
    struct listPoints* posCour = *list;
    double res = (*list)->point.proba;
    *list = (*list)->next;
    free(posCour);
    return res;
}

/**
 * Met les probas entre 0 et 1.
 * @param list
 */
static void formatProba(struct listPoints* list, double distMax) {
    //printf("formatProba\n");
    struct listPoints* posCour = list;
    while (posCour != NULL) {
        posCour->point.proba = 1 - posCour->point.proba / distMax;
        posCour = posCour->next;
    }
}

static void freeList(struct listPoints* list) {
    //printf("freeList\n");
    while (list != NULL) {
        free(list);
        list = list->next;
    }
}

static double addPoint(struct listPoints** list, struct Point point, int size) {
    //printf("addPoint\n");
    double res;
    if (*list == NULL) { //Cas liste vide
        //printf("liste vide\n");
        *list = (struct listPoints*) malloc(sizeof (struct listPoints));
        (*list)->point = point;
        (*list)->next = NULL;
    } else {
        int pos = 1;
        struct listPoints* posCour = *list;
        struct listPoints* posPred = *list;
        /*On avance tant qu'on est plus petit*/
        while (posCour != NULL && point.proba < posCour->point.proba) {
            posPred = posCour;
            posCour = posCour->next;
            pos++;
        }
        if (posCour == NULL) { //insertion en queue
            //printf("queue\n");
            posPred->next = (struct listPoints*) malloc(sizeof (struct listPoints));
            posPred->next->point = point;
            posPred->next->next = NULL;
        } else {
            if (pos == 1) { //insertion en tête
                //printf("tete\n");
                struct listPoints* newHead = (struct listPoints*) malloc(sizeof (struct listPoints));
                newHead->point = point;
                newHead->next = *list;
                *list = newHead;
            } else { //insertion ailleur
                //printf("ailleurs\n");
                struct listPoints* newPoint = (struct listPoints*) malloc(sizeof (struct listPoints));
                posPred->next = newPoint;
                newPoint->point = point;
                newPoint->next = posCour;
            }
        }
        //Supression de la tête si la liste est trop grande.
        if (size < sizeList(*list)) {
            res = removeHead(list);
        }
    }
    //printf("fin add\n");
    return res;
}

/**
 * Retourne le temps écoulé en ms depuis le lancement du main.
 * N'a pas de précision meilleure que la seconde.
 * Ne pas oublier d'initialiser timeReference au début du main avec time(NULL)
 */
static int getTimeInMs() {
    return (time(NULL) - timeReference)*1000;
}

/**
 * Écrit dans un fichiers les cases les plus probables à un instant donné.
 * @param m
 * @param mesure
 * @param nomFichier Fichier dans lequel on écrit.
 */
void generateData(struct Matrice* m, struct Valeurs* mesure, const char* nomFichier) {

    struct listPoints* list = NULL;
    double distMax;
    int nbPoints = 5; //nombre de points sélectionnés

    for (int i = 0; i < m->nbColumns; i++) {
        for (int j = 0; j < m->nbLines; j++) {
            struct Point point;
            point.x = i;
            point.y = j;
            point.proba = distance(&m->val[i][j], mesure);
            distMax = addPoint(&list, point, nbPoints);
        }
    }

    formatProba(list, distMax);

    FILE* fichier = NULL;
    fichier = fopen(nomFichier, "a");
    if (fichier != NULL) {
        //printf("fichier != NULL\n");
        fprintf(fichier, "d %i\n\n", getTimeInMs());
        struct listPoints* posCour = list;
        while (posCour != NULL) {
            //printf("dans boucle\n");
            fprintf(fichier, "%i %i %lf\n", posCour->point.x, posCour->point.y, posCour->point.proba);
            posCour = posCour->next;
        }
        fprintf(fichier, "\n");
        fclose(fichier);
    }

    freeList(list);
}


/**
 * À partir de la matrice de calibration et de 4 séries de mesures,
 * la fonction écrit dans nomFichier les n cases les plus probables.
 * @param m La matrice de calibration.
 * @param nomFichier Fichier où enregistrer les données.
 * @param mesures1 Série de mesures 1.
 * @param mesures2 Série de mesures 2.
 * @param mesures3 Série de mesures 3.
 * @param mesures4 Série de mesures 4.
 */
void generateDataFromMesures(struct Matrice* m, const char* nomFichier,
        char* mesures1, char* mesures2, char* mesures3, char* mesures4) {
    //Conversion en entiers
    int* mesures1Int = getMesures(mesures1);
    int* mesures2Int = getMesures(mesures2);
    int* mesures3Int = getMesures(mesures3);
    int* mesures4Int = getMesures(mesures4);

    //Moyenne des différentes mesures stoquées dans un seul tableau.
    struct Valeurs mesure;
    mesure.table[0] = moyenneMesures(mesures1Int);
    mesure.table[1] = moyenneMesures(mesures2Int);
    mesure.table[2] = moyenneMesures(mesures3Int);
    mesure.table[3] = moyenneMesures(mesures4Int);
    
    //Génération des données dans un fichier texte
    generateData(m, &mesure, nomFichier);

    //Libération des ressources.
    free(mesures1Int);
    free(mesures2Int);
    free(mesures3Int);
    free(mesures4Int);

}
