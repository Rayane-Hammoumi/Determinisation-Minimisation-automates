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
    int nbEtats;
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
Automate cree_automate(char nomFichier[]);
Automate minimise_automate(Automate aut);
int get_alphabet(Automate aut, char alphabet[]);
void initialise_tab_int(int tab[], int taille, int valeur);
void initialise_tab_char(char tab[], int taille);
void copy_tab(int tab1[], int tab2[], int tailleTab);
void init_minimisation(Automate aut, int groupe[]);
int tab_sont_egaux(int tab1[], int tab2[], int tailleTab);
int max(int tab[], int tailleTab);
void supprime_doublons(int tab[], int tailleTab);
int est_ds_tableau_int(int e, int tab[], int tailleTab);

#define limite_nb_chiffre_max_par_etat 6
// valeur à augmenter si on travaille sur des automates à plus de 100 000 états