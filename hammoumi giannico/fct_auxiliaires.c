#include "headers.h"

// fonction qui cree un automate a partir d'un fichier
// elle prend en parametre le nom du fichier
// elle retourne l'automate
Automate cree_automate(char nomFichier[])
{
    Automate aut;
    char txt[1000];
    int i = 0;

    FILE *fichier = fopen(nomFichier, "r");

    if (fichier == NULL)
    {
        printf("erreur à l'ouverture du fichier");
        fclose(fichier);
        exit(1);
    }
    else
    {
        int j = 0, k = 0;
        while (fgets(txt, 1000, fichier) != NULL)
        {
            if (i == 0) // 1ère ligne du fichier correspond au nombre d'etat
            {
                aut.nbEtat = atoi(txt);
                aut.etatsAccepteur = malloc(aut.nbEtat * sizeof(int));

                aut.listeTransition = malloc((aut.nbEtat * aut.nbEtat) * sizeof(Transition));

                for (int i = 0; i <= aut.nbEtat * aut.nbEtat; i++) // met le du tab a -1
                {
                    aut.listeTransition[i].etatDepart = -1;
                    aut.listeTransition[i].lettre = '\0';
                    aut.listeTransition[i].etatArrivee = -1;
                }
            }
            if (i == 1) // 2ème ligne du fichier correspond aux etats accepteur
            {
                char *token = strtok(txt, " ");
                // on va extraire les differents jetons et les mettre dans le tableau des etatsAccepteur
                while (token != NULL)
                {
                    aut.etatsAccepteur[k] = atoi(token);
                    token = strtok(NULL, " ");
                    k++;
                }
                for (int i = k; i < aut.nbEtat; i++) // met le reste du tab a -1
                    aut.etatsAccepteur[i] = -1;
            }
            if (i >= 2) // a partir de la 3eme ligne il y a que les transitions
            {
                j = 0;
                k = 0;
                char *token = strtok(txt, " ");
                // on va extraire les differents jetons et les mettres dans le tableau/variable correspondant
                //  jeton ds etatDepart ou lettre ou arrivee
                while (token != NULL)
                {
                    if (j == 0) // ds etatDepart
                    {
                        aut.listeTransition[i - 2].etatDepart = atoi(token);
                    }
                    if (j == 1) // ds lettre
                        aut.listeTransition[i - 2].lettre = token[0];
                    if (j == 2) // ds etatArrivee
                        aut.listeTransition[i - 2].etatArrivee = atoi(token);

                    token = strtok(NULL, " ");
                    j++;
                }
            }
            i++;
        }
        printf("\n");
    }

    fclose(fichier);

    return aut;
}

// fonction qui détermine si un état de l'automate donnée est accepteur, renvoie 1 si oui 0 sinon
// elle prend en paramètre, l'état, et l'automate
int est_accepteur(int etat, Automate aut)
{
    for (int i = 0; i < aut.nbEtat; i++)
    {
        if (etat == aut.etatsAccepteur[i])
            return 1;
    }
    return 0;
}

// fonction qui renvoie 1 si l'element e est dans le tableau, 0 sinon
// elle prend en parametre le caratere e, le tableau, et sa taille
int est_ds_tableau(char e, char tab[], int tailleTab)
{
    for (int i = 0; i < tailleTab; i++)
    {
        if (e == tab[i])
            return 1;
    }
    return 0;
}

// fonction qui permet de récuperer tout les caractère de l'automate pour former l'alphabet
// cette fonction renvoie le nombre de caractere de l'alphabet.
// elle prend en paramètre, l'automate, et le tableau de l'alphabet
int get_alphabet(Automate aut, char alphabet[])
{
    int i = 0, j = 0;

    alphabet[0] = aut.listeTransition[0].lettre;
    j = 1;
    while (aut.listeTransition[i].lettre != '\0')
    {
        if (!est_ds_tableau(aut.listeTransition[i].lettre, alphabet, 100))
        {
            alphabet[j] = aut.listeTransition[i].lettre;
            j++;
        }
        i++;
    }

    alphabet[i] = aut.listeTransition[i].lettre;

    return j;
}

// fonction qui initialise la minimisation en affectant des noms aux groupes
// elle prend en paramètre, l'automate, et le tableau des groupes
void init_minimisation(Automate aut, int groupe[])
{
    // init // 0: etat 0, 1: etat 1...
    int nomGroupe = 0; // 0 = accepteur 1 sinon
    for (int i = 0; i < aut.nbEtat; i++)
    {
        if (est_accepteur(i, aut))
            groupe[i] = 0;
        else
            groupe[i] = 1;
    }
}

// fonction qui initialise un tableau de type int
// elle prend en parametre, le tableau, et sa taille
void initialise_tab_int(int tab[], int taille, int valeur)
{
    for (int i = 0; i < taille; i++)
    {
        tab[i] = valeur;
    }
}

// fonction qui initialise un tableau de type char
// elle prend en paramètre, le tableau, et sa taille
void initialise_tab_char(char tab[], int taille)
{
    for (int i = 0; i < taille; i++)
    {
        tab[i] = ';';
    }
}

// fonction qui renvoie 1 si les tableaux sont égaux 0 sinon
// elle prend en paramètre, les 2 tableaux, et la taille de ces 2 tableaux
int tab_sont_egaux(int tab1[], int tab2[], int tailleTab)
{
    for (int i = 0; i < tailleTab; i++)
    {
        if (tab1[i] != tab2[i])
            return 0;
    }
    return 1;
}

// fonction qui renvoie le maximum d'un tableau
// elle prend en paramètre, le tableau, et sa taille
int max(int tab[], int tailleTab)
{
    int max = tab[0];
    for (int i = 1; i < tailleTab; i++)
    {
        if (max < tab[i])
            max = tab[i];
    }
    return max;
}

// fonction qui copie tab1 ds tab2
void copy_tab(int tab1[], int tab2[], int tailleTab)
{
    for (int i = 0; i < tailleTab; i++)
    {
        tab2[i] = tab1[i];
    }
}

void supprime_doublons(int tab[], int tailleTab)
{
    for (int i = 0; i < tailleTab; i++)
    {
        for (int j = 0; j < tailleTab; j++)
        {
            if ((tab[i] != -1 && tab[j] != -1) && (i != j) && (tab[i] == tab[j]))
                tab[j] = -1;
        }
    }
}
// fonction qui renvoie 1 si l'element e est dans le tableau, 0 sinon
// elle prend en paramètre le caratère e, le tableau, et sa taille
int est_ds_tableau_int(int e, int tab[], int tailleTab)
{
    for (int i = 0; i < tailleTab; i++)
    {
        if (e == tab[i])
            return 1;
    }
    return 0;
}

// renvoie 0 si les tableaux contiennent les mêmes éléments d'indice inférieur à indice_max
int compare_tableaux(char a[], char b[], int indice_max)
{
    for (int i = 0; i < indice_max; i++)
    {
        if (a[i] != b[i])
            return 1;
    }
    return 0;
}

// copie les éléments d'indice inférieur à indice_max du tableau b dans le tableau a
void copie_elements(char a[], char b[], int indice_max)
{
    for (int i = 0; i < indice_max; i++)
    {
        a[i] = b[i];
    }
}

//----------------------------------------------------------------------------------

void ecrit_automate_dans_fichier(Automate aut, char chemin_fichier[])
{
    FILE *AFD = fopen(chemin_fichier, "w+");

    if (AFD == NULL)
    {
        printf("erreur à l'ouverture du fichier");
        fclose(AFD);
        exit(1);
    }

    fprintf(AFD, "%d\n", aut.nbEtat); // on écrit le nombre d'états dans le fichier AFD
    int i;
    // on écrit les états accepteurs dans le fichier AFD
    for (i = 0; i < aut.nbEtat; i++)
    {
        if (aut.etatsAccepteur[i] == -1)
        {
            break;
        }
        else
        {
            fprintf(AFD, "%d", aut.etatsAccepteur[i]);

            if (i != aut.nbEtat - 1)
            {
                if (aut.etatsAccepteur[i + 1] != -1)
                {
                    fprintf(AFD, " ");
                }
            }
        }
    }

    // on écrit les transitions
    for (i = 0; i < aut.nbEtat * aut.nbEtat; i++)
    {
        if (aut.listeTransition[i].lettre != '\0')
        {
            fprintf(AFD, "\n");
            fprintf(AFD, "%d ", aut.listeTransition[i].etatDepart);
            fprintf(AFD, "%c ", aut.listeTransition[i].lettre);
            fprintf(AFD, "%d", aut.listeTransition[i].etatArrivee);
        }
    }

    fclose(AFD);
}

void affiche_table_transitions(Automate aut, char caracteres_automate[], int nb_caracteres_automate)
{
    // on affiche le nombre d'états
    printf("%d\n", aut.nbEtat);

    int i;
    int au_moins_une_transition_vers_ce_car = 0;

    // on affiche les états accepteurs
    for (i = 0; i < aut.nbEtat; i++)
    {
        if (aut.etatsAccepteur[i] == -1)
        {
            break;
        }
        else
        {
            printf("%d ", aut.etatsAccepteur[i]);
        }
    }

    printf("\n");

    for (i = 0; i < nb_caracteres_automate; i++)
    {
        printf("    %c", caracteres_automate[i]);
    }

    // pour chaque état de l'automate
    for (i = 0; i < aut.nbEtat; i++)
    {
        printf("\n%d", i);
        // pour chaque caractère de l'automate
        for (int j = 0; j < nb_caracteres_automate; j++)
        {
            printf("   ");

            if (j != 0)
            {
                printf(" ");
            }
            if ((j != 0) && (au_moins_une_transition_vers_ce_car == 0))
            {
                printf(" ");
            }
            else
            {
                au_moins_une_transition_vers_ce_car = 0;
            }

            // pour chaque transition
            for (int k = 0; k < aut.nbEtat * aut.nbEtat; k++)
            {
                // si la transition existe vraiment
                if (aut.listeTransition[k].lettre != '\0')
                {
                    if (aut.listeTransition[k].etatDepart == i)
                    {
                        if (aut.listeTransition[k].lettre == caracteres_automate[j])
                        {
                            printf("%d", aut.listeTransition[k].etatArrivee);
                            au_moins_une_transition_vers_ce_car = 1;
                        }
                    }
                }
            }
        }
    }

    printf("\n\nNB: si, pour un certain état, il n'y a pas de transition vers un certain caractère, on n'affiche aucun état destinataire\n");
    ;
}