#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Transition
{
    int etatDepart;
    char lettre;
    int etatArrivee;
} Transition;

typedef struct Automate
{
    int nbEtat;
    int *etatsAccepteur;
    Transition *listeTransition;
} Automate;

void execute_mots_sur_automate(int argc, char *argv[]);
void determinise_automate(Automate aut);
void affiche_automate(Automate aut);
int compare_tableaux(char a[], char b[], int indice_max);
void copie_elements(char a[], char b[], int indice_max);
void ecrit_automate_dans_fichier(Automate aut, char chemin_fichier[]);

#define limite_nb_chiffre_max_par_etat 6