#include "matrice.h"
#include "simulation_data.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>



int main(int argc, char **argv) {

	struct Matrice *matrice = CreateMatrice(NB_LIGNES,NB_COLONNES);


	for (uint8_t i = 0; i < NB_LIGNES; i++) {
		for (uint8_t j = 0; j < NB_COLONNES; j++) {
			for (uint8_t k = 0; k < 4; k++) {
				insertVal(matrice, i, j, k, "-44;-44;-45;-43;-44;");
			}
		}
	}

	afficherMatrice(matrice);
	return 0;

}

