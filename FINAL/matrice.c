#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "matrice.h"

/**
 * Initialise une matrice.
 * @param nb_lines Nombre de lignes.
 * @param nb_columns Nombre de colonnes.
 * @return La matrice.
 */
struct Matrix *create_matrix(int nb_lines, int nb_columns, int nb_sensors) {
	struct Matrix *result = calloc(1,sizeof (struct Matrix));
	result->nb_columns = nb_columns;
	result->nb_lines = nb_lines;	
	result->nb_sensors = nb_sensors;
	result->val = calloc(nb_lines, sizeof (double *));

	for (int i = 0; i < nb_lines; i++) {
		result->val[i] = calloc(nb_columns, sizeof (struct Values));
		for (int j = 0; j < nb_columns; j++) {
			result->val[i][j].nb_sensors = nb_sensors;
			result->val[i][j].table = calloc(nb_sensors, sizeof(double));
		}
	}
	return result;
}

/**
 * Insère une valeur dans la matrice.
 */
void insert_val(struct Matrix *matrice, int i, int j, int capteur, char *measures) {
	matrice->val[i][j].table[capteur] = mean_measures(get_measures(measures));
}

/**
 * Ouvre une matrice depuis sa sauvegarde.
 */
struct Matrix *open_matrix(char *file) {

	FILE *fichier = fopen(file, "r");
	if (!fichier) {
		perror("open matrix");
		return NULL;
	}

	struct Matrix *res = NULL;
	int nb_lines = 0;
	int nb_col = 0;
	int nb_sens = 0;
	char temp[50];
	fscanf(fichier, "%i %i", &(nb_lines), &(nb_col));
	fscanf(fichier, "%i", &(nb_sens));
	res = create_matrix(nb_lines, nb_col, nb_sens);
	int temp1, temp2;
	for (int i = 0; i < nb_lines; i++) {
        	for (int j = 0; j < nb_col; j++) {
			fscanf(fichier, "%i %i", &temp1, &temp2);
			for (int k = 0; k < nb_sens; k++) {
				float temp;
				fscanf(fichier, "%f", &temp);
				res->val[i][j].table[k] = temp;
    			}
        	}
    	}
	fclose(fichier);
	return res;

	
}

/**
 * Sauvegarde une matrice dans un fichier.
 */
int8_t save_matrix(char *file, struct Matrix matrice) {
	FILE *fichier = fopen(file, "w");
	if (!fichier) {
		return -1;
	}

	fprintf(fichier, "%i %i\n", matrice.nb_lines, matrice.nb_columns);
	fprintf(fichier, "%i\n\n", matrice.nb_sensors);
	for (int i = 0; i < matrice.nb_lines; i++) {
        	for (int j = 0; j < matrice.nb_columns; j++) {
			fprintf(fichier, "%i %i\n", i, j);
			for (int k = 0; k < matrice.val[i][j].nb_sensors; k++) {
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
void display_values(struct Values v) {
    for (int i = 0; i < v.nb_sensors; i++) {
        printf("Valeur : %f\n", v.table[i]);
    }
}

/**
 * Affiche toute la matrice.
 * @param m
 */
void display_matrix(struct Matrix *m) {
    for (int i = 0; i < m->nb_lines; i++) {
        for (int j = 0; j < m->nb_columns; j++) {
            printf("Ligne n° :%i ", i);
            printf("Colonne n° :%i\n", j);
            display_values(m->val[i][j]);
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
struct Values get_element(struct Matrix *m, int line, int column) {
    return m->val[line][column];
}

/**
 * Met elem dans la case line column de la matrice m.
 * @param m
 * @param line
 * @param column
 * @param elem
 */
void set_element(struct Matrix *m, int line, int column, struct Values elem) {
    m->val[line][column] = elem;
}

/**
 * Retourne la distance euclidienne entre v1 et v2.
 * @param v1
 * @param v2
 * @return 
 */
static double distance(struct Values* v1, struct Values* v2) {
    double dist = 0;
    for (int i = 0; i < v1->nb_sensors; i++) {
        dist += (v1->table[i] - v2->table[i]) * (v1->table[i] - v2->table[i]);
    }
    return sqrt(dist);
}

/**
 * Retourne la position dans la matrice correspondant le plus à la mesure.
 * @param m
 * @param measure
 * @return 
 */
struct Point *best_position(struct Matrix *m, struct Values* measure) {
    struct Point *coord_best_case = malloc(sizeof (struct Point));
    double min_dist = INFINITY;
    double current_dist;


    for (int i = 0; i < m->nb_columns; i++) {
        for (int j = 0; j < m->nb_lines; j++) {
            if (m->val[i] != NULL) {
                current_dist = distance(measure, &m->val[i][j]);
                if (current_dist < min_dist) {
                    min_dist = current_dist;
                    coord_best_case->x = i;
                    coord_best_case->y = j;
                }
            }
        }
    }
    //coord_best_case->x = bestCase / m->nb_columns;
    //coord_best_case->y = bestCase % m->nb_columns;

    return coord_best_case;
}

/**
 * Transforme les mesures brutes en un tableau d'entiers.
 * @param measures
 * @return Le tableau d'entiers.
 */
int* get_measures(char* measures) {
    int* res = (int*) calloc(NB_MESURES, sizeof (int));
    int tmp = 0;
    int i = 0; //position dans la chaine
    int j = 0; //position dans le tableau

    while (measures[i] != '\0') {
        if (measures[i] == '-') {
            i++;
            while (measures[i] != '\0' && measures[i] != ';') {
                tmp = tmp * 10 - ((int) measures[i] - '0');
                i++;
            }
        }
        res[j] = tmp;
        tmp = 0;
        j++;
        if (measures[i] != '\0') i++;
    }

    return res;
}

/**
 * Calcule la moyenne des mesures.
 * @param measures
 * @return La moyenne.
 */
double mean_measures(int* measures) {
    double res = 0;
    for (int i = 0; i < NB_MESURES; i++) {
        res += measures[i];
    }
    res /= NB_MESURES;
    return res;
}

/**
 * Donne la taille de la liste.
 * @param list
 * @return 
 */
static int size_list(struct listPoints* list) {
    //printf("size_list\n");
    int size = 0;
    struct listPoints* current_pos = list;
    while (current_pos != NULL) {
        current_pos = current_pos->next;
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
static double remove_head(struct listPoints** list) {
    //printf("remove_head\n");
    struct listPoints* current_pos = *list;
    double res = (*list)->point.proba;
    *list = (*list)->next;
    free(current_pos);
    return res;
}

/**
 * Met les probas entre 0 et 1.
 * @param list
 */
static void format_proba(struct listPoints* list, double distMax) {
    //printf("format_proba\n");
    struct listPoints* current_pos = list;
    while (current_pos != NULL) {
        current_pos->point.proba = 1 - current_pos->point.proba / distMax;
        current_pos = current_pos->next;
    }
}

static void free_list(struct listPoints* list) {
    //printf("free_list\n");
    while (list != NULL) {
        free(list);
        list = list->next;
    }
}

static double add_point(struct listPoints** list, struct Point point, int size) {
    //printf("add_point\n");
    double res = INFINITY;
    if (*list == NULL) { //Cas liste vide
        //printf("liste vide\n");
        *list = (struct listPoints*) malloc(sizeof (struct listPoints));
        (*list)->point = point;
        (*list)->next = NULL;
    } else {
        int pos = 1;
        struct listPoints* current_pos = *list;
        struct listPoints* pred_pos = *list;
        /*On avance tant qu'on est plus petit*/
        while (current_pos != NULL && point.proba < current_pos->point.proba) {
            pred_pos = current_pos;
            current_pos = current_pos->next;
            pos++;
        }
        if (current_pos == NULL) { //insertion en queue
            //printf("queue\n");
            pred_pos->next = (struct listPoints*) malloc(sizeof (struct listPoints));
            pred_pos->next->point = point;
            pred_pos->next->next = NULL;
        } else {
            if (pos == 1) { //insertion en tête
                //printf("tete\n");
                struct listPoints* new_head = (struct listPoints*) malloc(sizeof (struct listPoints));
                new_head->point = point;
                new_head->next = *list;
                *list = new_head;
            } else { //insertion ailleur
                //printf("ailleurs\n");
                struct listPoints* new_point = (struct listPoints*) malloc(sizeof (struct listPoints));
                pred_pos->next = new_point;
                new_point->point = point;
                new_point->next = current_pos;
            }
        }
        //Supression de la tête si la liste est trop grande.
        if (size < size_list(*list)) {
            res = remove_head(list);
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
static int get_time_in_ms() {
    return (time(NULL) - timeReference)*1000;
}

/**
 * Écrit dans un fichiers les cases les plus probables à un instant donné.
 * @param m
 * @param measure
 * @param file_name Fichier dans lequel on écrit.
 */
void generate_data(struct Matrix* m, struct Values* measure, const char* file_name) {

    struct listPoints* list = NULL;
    double max_dist = -1;
    int nb_points = 5; //nombre de points sélectionnés

    for (int i = 0; i < m->nb_lines; i++) {
        for (int j = 0; j < m->nb_columns; j++) {
	    double tmp_max_dist = INFINITY;
            struct Point point;
            point.x = i;
            point.y = j;
            point.proba = distance(&m->val[i][j], measure);
	    //on garde la plus petite des distances non retenues pour formater après la liste des distances.
	    if (max_dist != -1) {
		tmp_max_dist = add_point(&list, point, nb_points);
	    } else {
		max_dist = add_point(&list, point, nb_points);
	    }
	    if (tmp_max_dist < max_dist) {
		max_dist = tmp_max_dist;
	    }
        }
    }

    format_proba(list, max_dist);

    FILE* fichier = NULL;
    fichier = fopen(file_name, "a");
    if (fichier != NULL) {
        //printf("fichier != NULL\n");
        fprintf(fichier, "d %i\n\n", get_time_in_ms());
	fprintf(fichier, "reset\n\n");
        struct listPoints* current_pos = list;
        while (current_pos != NULL) {
            //printf("dans boucle\n");
            fprintf(fichier, "%i %i %lf\n", current_pos->point.x, current_pos->point.y, current_pos->point.proba);
            current_pos = current_pos->next;
        }
        fprintf(fichier, "\n");
        fclose(fichier);
    }

    free_list(list);
}


/**
 * À partir de la matrice de calibration et de 4 séries de mesures,
 * la fonction écrit dans file_name les n cases les plus probables.
 * @param m La matrice de calibration.
 * @param file_name Fichier où enregistrer les données.
 * @param measures1 Série de mesures 1.
 * @param measures2 Série de mesures 2.
 * @param measures3 Série de mesures 3.
 * @param measures4 Série de mesures 4.
 */
void generate_data_from_measures(struct Matrix* m, const char* file_name,
        char* measures1, char* measures2, char* measures3, char* measures4) {
    //Conversion en entiers
    int* measures1_int = get_measures(measures1);
    int* measures2_int = get_measures(measures2);
    int* measures3_int = get_measures(measures3);
    int* measures4_int = get_measures(measures4);

    //Moyenne des différentes mesures stoquées dans un seul tableau.
    struct Values measure;
    measure.table[0] = mean_measures(measures1_int);
    measure.table[1] = mean_measures(measures2_int);
    measure.table[2] = mean_measures(measures3_int);
    measure.table[3] = mean_measures(measures4_int);
    
    //Génération des données dans un fichier texte
    generate_data(m, &measure, file_name);

    //Libération des ressources.
    free(measures1_int);
    free(measures2_int);
    free(measures3_int);
    free(measures4_int);

}
