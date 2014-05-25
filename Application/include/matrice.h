#ifndef MATRICE_H
#define	MATRICE_H

#include <stdint.h>

#define NB_CAPTEURS 4
#define NB_MESURES 8

int timeReference;

/**
 * Ce que contient chaque case de la matrice.
 */
struct Values {
	int nb_sensors;
	double *table;
};

/**
 * La matrice.
 */
struct Matrix {
	int nb_sensors;
	int nb_lines;
	int nb_columns;
	struct Values **val;
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

extern struct Matrix *create_matrix(int nb_lines,int nb_columns, int nb_sensors);

extern void insert_val(struct Matrix *matrice, int i, int j, int capteur, char *measures);

extern struct Matrix *open_matrix(char *file);

extern int8_t save_matrix(char *file, struct Matrix matrice);

extern void display_values(struct Values v);

extern void display_matrix(struct Matrix *m);

extern struct Values get_element(struct Matrix *m,int line, int column);

extern void set_element(struct Matrix *m, int line, int column, struct Values elem);

extern struct Point *best_position(struct Matrix *m,struct Values* measure);

extern int* get_measures(char* measures);

extern double mean_measures(int* measures);

extern void generate_data(struct Matrix* m, struct Values* measure, const char* file_name);

extern void generate_data_from_measures(struct Matrix* m, const char* file_name,
        char* measures1, char* measures2, char* measures3, char* measures4);

#endif
