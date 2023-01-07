#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void execute_AFN(int argc, char *argv[]);
void determinise_automate(char *argv[]);
int compare_tableaux(char a[], char b[], int taille_tableaux);
void copie_elements(char a[], char b[], int taille_tableaux);

#define limite_nb_chiffre_max_par_etat 6