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
int compare_tableaux(char a[], char b[], int indice_max);
void copie_elements(char a[], char b[], int indice_max);
void ecrit_automate_dans_fichier(Automate aut, char chemin_fichier[]);
void affiche_table_transitions(Automate aut, char caracteres_automate[], int nb_caracteres_automate);
void printf_nb_espaces(int nb_espaces);

#define limite_nb_chiffre_max_par_etat 6
// valeur à augmenter si on travaille sur des automates à plus de 100 000 états