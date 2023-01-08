#include "headers.h"

// execute les mots de la commande sur le terminal specifié dans la commande
void execute_mots_sur_automate(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("Veuillez préciser au moins un mot à exécuter\n\n");
        exit(1);
    }

    FILE *fichier = fopen(argv[1], "r");

    if (fichier == NULL)
    {
        printf("erreur à l'ouverture du fichier");
        fclose(fichier);
        exit(1);
    }

    int nb_chiffres_max_par_etat = 0;
    int compteur_indice_caractere_lu = 0;
    char chiffres_du_nb_etats_AFN[limite_nb_chiffre_max_par_etat + 1]; // tableau qui contiendra les chiffres du nombre d'etats de l'AFN
                                                                       // modifier sa taille si l'automate contient plus de 100 000 états

    fseek(fichier, 0, SEEK_SET); // on place la tête de lecture en début de fichier

    chiffres_du_nb_etats_AFN[compteur_indice_caractere_lu] = fgetc(fichier);
    nb_chiffres_max_par_etat++; // on compte le nombre de chiffres contenu dans le 1er nombre du fichier (le nombre d'états)

    while (fgetc(fichier) != '\n')
    {
        fseek(fichier, -1, SEEK_CUR);
        compteur_indice_caractere_lu++; // on compte le nombre de chiffres contenu dans le 1er nombre du fichier (le nombre d'états)
        if (compteur_indice_caractere_lu > limite_nb_chiffre_max_par_etat)
        {
            printf("pour faire fonctionner ce programme avec des automates à plus de 100 000 etats, veuillez augmenter la valeur du define dans headers.h] \n");
            exit(1);
        }
        chiffres_du_nb_etats_AFN[compteur_indice_caractere_lu] = fgetc(fichier); // on stocke le chiffre
        nb_chiffres_max_par_etat++;
    }
    compteur_indice_caractere_lu++;
    chiffres_du_nb_etats_AFN[compteur_indice_caractere_lu] = '\0';
    int nb_etats_AFN = atoi(chiffres_du_nb_etats_AFN); // on convertit les chiffres en nombre

    int i = 0, j = 0, k = 0;
    char string_s[nb_chiffres_max_par_etat]; // string qui ne contient que des 's'. Il sert a réinitialiser les strings

    for (k = 0; k < nb_chiffres_max_par_etat; k++)
    {
        string_s[k] = 's';
    }

    char etats_accepteurs[nb_etats_AFN][nb_chiffres_max_par_etat];
    int position_transitions = nb_chiffres_max_par_etat + 1; /* indique à quelle position dans le fichier commence
                                                             la première transition (calculée plus bas)*/
    compteur_indice_caractere_lu = 0;
    char caractere_lu[nb_chiffres_max_par_etat]; // string qui va stocker les valeurs de renvoi de fgetc()
    strcpy(caractere_lu, string_s);

    // on remplit etats_accepteurs[]
    while (fgetc(fichier) != '\n')
    {
        fseek(fichier, -1, SEEK_CUR); // on recule la tête de lecture d'une position pour compenser le fgetc(fichier) dans la condition du while

        while (fgetc(fichier) != ' ')
        {
            fseek(fichier, -1, SEEK_CUR); // même raison
            if (fgetc(fichier) == '\n')
            {
                fseek(fichier, -1, SEEK_CUR); // même raison
                break;
            }
            fseek(fichier, -1, SEEK_CUR);                                // même raison
            caractere_lu[compteur_indice_caractere_lu] = fgetc(fichier); // on stocke le chiffre de l'etat accepteur
            position_transitions++;
            compteur_indice_caractere_lu++;
        }

        position_transitions++;
        strcpy(etats_accepteurs[j], caractere_lu); // on copie les chiffres contenus dans caractere_lu dans etats_accepteurs[j]
        strcpy(caractere_lu, string_s);
        compteur_indice_caractere_lu = 0;
        j++; // on passe à l'état accepteur suivant
    }

    int nb_etats_accepteurs = j; // va nous servir pour ne parcourir que les string remplis par des etats accepteurs

    j = 0;
    char etat_actuel[nb_chiffres_max_par_etat];
    strcpy(etat_actuel, string_s);
    etat_actuel[0] = '0';

    int mot_accepte = 0; // booleen
    int mot_execute = 2; // incrémente quand on a finit d'exécuter un mot sur l'automate
    int nb_caracteres_a_chercher = 0;

    for (k = 2; k <= argc - 1; k++)
    {
        nb_caracteres_a_chercher += strlen(argv[k]);
    }

    // à chaque transition, on note:
    long historique_positions[nb_caracteres_a_chercher];                       // -> sa position dans le fichier grâce à ftell
    char historique_etats[nb_caracteres_a_chercher][nb_chiffres_max_par_etat]; // -> l'état emprunté

    int nb_transitions_prises = 0;                                                    // incrémente à chaque transition prise et décrémente quand on revient à un état précédent
    int positions_interdites[nb_caracteres_a_chercher + nb_etats_AFN * nb_etats_AFN]; // contient les positions dans le fichier qui mènent à des ko

    // on initialise historique_etats à des strings remplis de 's'
    // on initialise positions_interdites et historique_positions à des tableaux remplis de 0
    for (k = 0; k < nb_caracteres_a_chercher; k++)
    {
        strcpy(historique_etats[k], string_s);
        historique_positions[k] = 0;
    }

    for (k = nb_caracteres_a_chercher; k < nb_caracteres_a_chercher + nb_etats_AFN * nb_etats_AFN; k++)
    {
        positions_interdites[k] = 0;
    }

    int compteur_positions_interdites = 0; // compte le nombre de transitions qu'on a déjà testé et qui ne fonctionnent pas
    int est_interdite = 0;                 // booleen qui passe à 1 si la position actuelle de la tête de lecture est contenue dans positions_interdites[]
    int nb_espaces = 0;                    // pour l'affichage dans le terminal
    int somme_nb_chiffres_etats_empruntes = 0;
    char test_fgetc = 's';
    char copie_etat_actuel[nb_chiffres_max_par_etat + 1]; // sert pour le calcul du nombre d'espace dans l'affichage du terminal
    int nb_chiffres_etat_actuel = 0;
    // on replace le curseur au début des transitions au 1er caractère
    fseek(fichier, position_transitions + 1, SEEK_SET);
    while ((test_fgetc = fgetc(fichier) != ' '))
    {
    }

    strcpy(caractere_lu, string_s); // on réinitialise caractere_lu
    caractere_lu[0] = fgetc(fichier);

    // pour chaque mot en entrée sauf le nom de ce programme
    for (mot_execute = 2; mot_execute < argc; mot_execute++)
    {
        printf("\n(0, %s) |-", argv[mot_execute]);

        // on break si on a accepté le mot ou si on est à l'état 0 et qu'on a finit de lire le fichier
        while (1)
        {
            // si on lit le caractere recherché
            if (caractere_lu[0] == argv[mot_execute][i])
            {
                // on se replace juste avant l'état de départ de la transition
                fseek(fichier, -4, SEEK_CUR);
                while ((test_fgetc = fgetc(fichier)) != '\n')
                {
                    fseek(fichier, -2, SEEK_CUR);
                }

                compteur_indice_caractere_lu = 0;
                strcpy(caractere_lu, string_s);
                caractere_lu[compteur_indice_caractere_lu] = fgetc(fichier);
                compteur_indice_caractere_lu++;

                while (fgetc(fichier) != ' ')
                {
                    fseek(fichier, -1, SEEK_CUR);
                    caractere_lu[compteur_indice_caractere_lu] = fgetc(fichier);
                    compteur_indice_caractere_lu++;
                }

                fseek(fichier, 1, SEEK_CUR); // on revient à la position juste après le caractère

                // si la transition part bien de l'état actuel, on vérifie si la position de la tête de lecture est contenue dans positions_interdites[]
                if (compare_tableaux(caractere_lu, etat_actuel, nb_chiffres_max_par_etat) == 0)
                {

                    k = 0;
                    while (positions_interdites[k] != 0)
                    {

                        if (positions_interdites[k] == ftell(fichier))
                        {
                            est_interdite = 1;
                            break;
                        }
                        k++;
                    }
                    k = 0;
                }

                // si pour y accéder, la transistion part bien de l'etat actuel et que la position n'est pas interdite
                if ((est_interdite == 0) && (compare_tableaux(caractere_lu, etat_actuel, nb_chiffres_max_par_etat) == 0))
                {
                    historique_positions[nb_transitions_prises] = ftell(fichier);
                    fseek(fichier, 1, SEEK_CUR); // on revient à la position juste avant l'état de destination
                    compteur_indice_caractere_lu = 0;
                    strcpy(caractere_lu, string_s);
                    caractere_lu[compteur_indice_caractere_lu] = fgetc(fichier);
                    compteur_indice_caractere_lu++;

                    // on stocke l'etat de destination dans caractere_lu
                    while ((test_fgetc = fgetc(fichier)) != '\n')
                    {
                        if (test_fgetc == EOF)
                        {
                            break;
                        }
                        fseek(fichier, -1, SEEK_CUR);
                        caractere_lu[compteur_indice_caractere_lu] = fgetc(fichier);
                        compteur_indice_caractere_lu++;
                    }

                    fseek(fichier, 1, SEEK_CUR);                                         // on revient à la position juste après le caractère
                    copie_elements(etat_actuel, caractere_lu, nb_chiffres_max_par_etat); // l'état actuel devient la destination de la transition
                    copie_elements(historique_etats[nb_transitions_prises], etat_actuel, nb_chiffres_max_par_etat);

                    // on augmente le nombre d'espaces à afficher dans le terminal
                    nb_chiffres_etat_actuel = snprintf(copie_etat_actuel, nb_chiffres_max_par_etat + 1, "%d", atoi(etat_actuel));
                    somme_nb_chiffres_etats_empruntes += nb_chiffres_etat_actuel;

                    // on replace le curseur au début des transitions au 1er caractère
                    fseek(fichier, position_transitions + 1, SEEK_SET);
                    while ((test_fgetc = fgetc(fichier) != ' '))
                    {
                    }

                    nb_transitions_prises++;

                    printf(" (");

                    for (k = 0; k < nb_chiffres_max_par_etat; k++)
                    {
                        if (caractere_lu[k] != 's')
                        {
                            printf("%c", caractere_lu[k]);
                        }
                    }

                    printf(", ");
                    i++; // on passe au caractère suivant
                    j = i;

                    // on affiche les caractères pas encore lus du mot en exécution
                    while ((argv[mot_execute][j] != ' ') && (argv[mot_execute][j] != NULL))
                    {
                        printf("%c", argv[mot_execute][j]);
                        j++;
                    }

                    printf(") |-");
                    j = 0;
                }

                else
                {
                    // on déplace la tête de lecture juste avant le caractere de la transition suivante
                    fseek(fichier, 2, SEEK_CUR);
                    while ((test_fgetc = fgetc(fichier)) != '\n')
                    {
                        if (test_fgetc == EOF)
                        {
                            break;
                        }
                    }
                    if (test_fgetc != EOF)
                    {
                        while ((caractere_lu[0] = fgetc(fichier)) != ' ')
                        {
                        }
                    }

                    est_interdite = 0;
                }
            }

            // sinon si on a finit de lire le fichier
            else if (caractere_lu[0] == EOF)
            {

                if (nb_transitions_prises == 0) // si on est toujours à l'état initial, le mot ne peut pas être accepté
                {
                    printf(" ko\n");

                    // s'il reste au moins 1 mot à exécuter
                    if (mot_execute + 1 < argc)
                    {
                        //  on réinitialise toutes les variables nécessaires pour l'exécution du mot suivant
                        strcpy(etat_actuel, string_s);
                        etat_actuel[0] = '0';
                        compteur_indice_caractere_lu = 0;
                        est_interdite = 0;

                        // on replace le curseur au début des transitions au 1er caractère
                        fseek(fichier, position_transitions + 1, SEEK_SET);
                        while ((test_fgetc = fgetc(fichier) != ' '))
                        {
                        }

                        i = 0;
                        nb_transitions_prises = 0;
                        compteur_positions_interdites = 0;

                        for (k = 0; k < nb_caracteres_a_chercher; k++)
                        {

                            strcpy(historique_etats[k], string_s);
                            historique_positions[k] = 0;
                        }

                        for (k = nb_caracteres_a_chercher; k < nb_caracteres_a_chercher + nb_etats_AFN * nb_etats_AFN; k++)
                        {
                            positions_interdites[k] = 0;
                        }

                        strcpy(caractere_lu, string_s);
                        caractere_lu[0] = fgetc(fichier); // on lit le caractère suivant
                    }
                    break; // on passe au prochain mot s'il en reste 1 à exécuter
                }

                else // sinon on revient à la transition précédente
                {
                    printf(" ko\n");

                    // calcul du nombre d'espaces pour l'affichage dans le terminal
                    nb_chiffres_etat_actuel = snprintf(copie_etat_actuel, nb_chiffres_max_par_etat + 1, "%d", atoi(historique_etats[nb_transitions_prises - 1]));
                    somme_nb_chiffres_etats_empruntes -= nb_chiffres_etat_actuel;
                    nb_espaces = 6 + strlen(argv[mot_execute]);
                    if (nb_transitions_prises != 1)
                    {
                        nb_espaces += (somme_nb_chiffres_etats_empruntes + 8 * (nb_transitions_prises - 1));

                        for (k = 1; k < nb_transitions_prises; k++)
                        {
                            nb_espaces += (strlen(argv[mot_execute]) - k);
                        }
                    }

                    for (k = 0; k < nb_espaces; k++)
                    {
                        printf(" ");
                    }

                    printf("|-");

                    // la position du caractere de la transition précédente devient interdite
                    nb_transitions_prises--;
                    positions_interdites[compteur_positions_interdites] = historique_positions[nb_transitions_prises];
                    compteur_positions_interdites++;

                    if (nb_transitions_prises == 0)
                    {
                        strcpy(etat_actuel, string_s);
                        etat_actuel[0] = '0';
                    }
                    else
                    {
                        strcpy(etat_actuel, string_s);
                        copie_elements(etat_actuel, historique_etats[nb_transitions_prises - 1], nb_chiffres_max_par_etat);
                        // l'état actuel devient l'état de départ de la dernière transition empruntée
                    }

                    i--; // on repasse au caractère précédent dans argv[i]

                    // on replace le curseur au début des transitions au 1er caractère
                    fseek(fichier, position_transitions + 1, SEEK_SET);
                    while ((test_fgetc = fgetc(fichier) != ' '))
                    {
                    }
                }
            }
            else
            {
                // on déplace la tête de lecture juste avant le caractere de la transition suivante
                fseek(fichier, 2, SEEK_CUR);
                while ((test_fgetc = fgetc(fichier)) != '\n')
                {
                    if (test_fgetc == EOF)
                    {
                        break;
                    }
                }
                if (test_fgetc != EOF)
                {
                    while ((caractere_lu[0] = fgetc(fichier)) != ' ')
                    {
                    }
                }
            }

            if (i >= strlen(argv[mot_execute])) // si on a lu tout le mot
            {
                // on verifie si etat_actuel est contenu dans etats_accepteurs, si oui ok sinon ko
                for (k = 0; k < nb_etats_accepteurs; k++)
                {
                    if (compare_tableaux(etat_actuel, etats_accepteurs[k], nb_chiffres_max_par_etat) == 0)
                    {
                        printf(" ok\n");
                        mot_accepte = 1;
                        break;
                    }
                }

                if (mot_accepte == 1)
                {
                    // s'il reste au moins 1 mot à exécuter
                    if (mot_execute + 1 < argc)
                    {
                        //  on réinitialise toutes les variables nécessaires pour l'exécution du mot suivant
                        mot_accepte = 0;
                        est_interdite = 0;
                        strcpy(etat_actuel, string_s);
                        etat_actuel[0] = '0';
                        i = 0;
                        nb_transitions_prises = 0;
                        compteur_positions_interdites = 0;

                        for (k = 0; k < nb_caracteres_a_chercher; k++)
                        {

                            strcpy(historique_etats[k], string_s);
                            historique_positions[k] = 0;
                        }

                        for (k = nb_caracteres_a_chercher; k < nb_caracteres_a_chercher + nb_etats_AFN * nb_etats_AFN; k++)
                        {

                            positions_interdites[k] = 0;
                        }

                        // on replace le curseur au début des transitions au 1er caractère
                        fseek(fichier, position_transitions + 1, SEEK_SET);
                        while ((test_fgetc = fgetc(fichier) != ' '))
                        {
                        }

                        strcpy(caractere_lu, string_s);
                        caractere_lu[0] = fgetc(fichier); // on lit le caractère suivant
                    }
                    break;
                }

                else // on a réussi à lire le mot mais on ne tombe pas dans un état accepteur
                {
                    printf(" ko\n");

                    // calcul du nombre d'espaces pour l'affichage dans le terminal
                    somme_nb_chiffres_etats_empruntes -= nb_chiffres_etat_actuel;
                    nb_espaces = 6 + strlen(argv[mot_execute]);
                    if (nb_transitions_prises != 1)
                    {
                        nb_espaces += (somme_nb_chiffres_etats_empruntes + 8 * (nb_transitions_prises - 1));

                        for (k = 1; k < nb_transitions_prises; k++)
                        {
                            nb_espaces += (strlen(argv[mot_execute]) - k);
                        }
                    }

                    for (k = 0; k < nb_espaces; k++)
                    {
                        printf(" ");
                    }
                    printf("|-");

                    nb_transitions_prises--;
                    // la position actuelle devient interdite, on revient à l'état de départ de la transition précédemment empruntée
                    positions_interdites[compteur_positions_interdites] = historique_positions[nb_transitions_prises];
                    compteur_positions_interdites++;

                    // on ne passe pas par l'historique des etats dans on n'a pas pris de transition car il ne stocke pas l'état initial
                    if (nb_transitions_prises == 0)
                    {
                        strcpy(etat_actuel, string_s);
                        etat_actuel[0] = '0';
                    }
                    else
                    {
                        strcpy(etat_actuel, string_s);
                        copie_elements(etat_actuel, historique_etats[nb_transitions_prises - 1], nb_chiffres_max_par_etat);
                    }

                    i--;

                    // on replace le curseur au début des transitions au 1er caractère
                    fseek(fichier, position_transitions + 1, SEEK_SET);
                    while ((test_fgetc = fgetc(fichier) != ' '))
                    {
                    }
                }
            }

            strcpy(caractere_lu, string_s);   // on réinitialise caractere_lu
            caractere_lu[0] = fgetc(fichier); // on lit le caractère suivant
        }
    }

    fclose(fichier);
}

//----------------------------------------------------------------------------------
// MINIMISATION

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
            // printf("%s", txt);
            if (i == 0) // 1ere ligne du fichier correspond au nombre d'etat
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
            if (i == 1) // 2eme ligne du fichier correspond aux etats accepteur
            {
                char *token = strtok(txt, " ");
                // on va extraire les differents jetons et les mettre dans le tableau des etatsAccepteur
                while (token != NULL)
                {
                    // printf("%d %s\n", k, token);
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
                    // printf(" %s\n", token);
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

// fonction qui determine si un etat de l'automate donnee est accepteur, renvoie 1 si oui 0 sinon
// elle prend en parametre, l'etat, et l'automate
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

// fonction qui permet de récuperer tout les caractere de l'automate pour former l'alphabet
// cette fonction renvoie le nombre de caractere de l'alphabet.
// elle prend en parametre, l'automate, et le tableau de l'alphabet
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
// elle prend en parametre, l'automate, et le tableau des groupes
void init_minimisation(Automate aut, int groupe[])
{
    // init // 0: etat 0, 1: etat 1...
    int nomGroupe = 0; // 0 = accepteur 1 sinon
    // printf("INIT\n");
    for (int i = 0; i < aut.nbEtat; i++)
    {
        if (est_accepteur(i, aut))
            groupe[i] = 0;
        else
            groupe[i] = 1;

        // printf("[%d] = groupe %d\n", i, groupe[i]);
    }
    // printf("\n");
    ///////
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
// elle prend en parametre, le tableau, et sa taille
void initialise_tab_char(char tab[], int taille)
{
    for (int i = 0; i < taille; i++)
    {
        tab[i] = ';';
    }
}

// fonction qui renvoie 1 si les tableaux sont egaux 0 sinon
// elle prend en parametre, les 2 tableaux, et la taille de ces 2 tableaux
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
// elle prend en parametre, le tableau, et sa taille
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

// fonction qui affiche la table des transitions d'un automate minimalise
//  elle prend en parametre, le nombre de caractere ds l'alphabet, la table de transition, la taille de ce tab et l'alphabet
void affiche_table_transition_mini(int nbAlphabet, int tableTransi[][nbAlphabet], int tailleTab1, char alphabet[])
{
    printf("table transition mini:\n");
    for (int i = 0; i < nbAlphabet; i++)
        printf("   %c", alphabet[i]);
    printf("\n");

    for (int i = 0; i < tailleTab1; i++)
    {
        printf("%d  ", i);
        for (int j = 0; j < nbAlphabet; j++)
        {
            printf("%d   ", tableTransi[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// fonction qui minimise l'automate, elle prend en parametre l'automate
Automate minimise_automate(Automate aut)
{
    int groupe[aut.nbEtat]; // Le nom des differents groupe creer, gp[0]: 1etat, gp[1]:2etat... = bilan
    initialise_tab_int(groupe, aut.nbEtat, -1);
    int groupeAvant[aut.nbEtat]; // utile pour arreter l'algo si on trouve 2 bilan identique
    initialise_tab_int(groupeAvant, aut.nbEtat, -1);
    int groupeEtatAccepteur[aut.nbEtat]; // utile pour sauvegarder l'ancien nom des etats Accepteurs
    copy_tab(aut.etatsAccepteur, groupeEtatAccepteur, aut.nbEtat);

    char alphabet[100];
    initialise_tab_char(alphabet, 100);

    // definit le nom des groupe : INITIALISATION
    init_minimisation(aut, groupe);
    /*
        ex:
        ->   INIT | 1   1   3
            --------------------
    */
    int nbAlphabet = get_alphabet(aut, alphabet);  // nombre de caractere dans l'alphabet
    int etapeMinimisation[aut.nbEtat][nbAlphabet]; // les differentes etapes de minimisation => table transition

    // EXECUTION
    // remplir tab
    int k = 0;
    int bilanIdentique = 1;
    while (bilanIdentique)
    {

        /*
        ex:
                     INIT | 1   1   3
                 --------------------
        etape    ->    a  | 1   1   2
                 ->    b  | 2   3   3
        */
        // cette boucle va creer les chiffres a mettre ds les differentes etapes
        for (int i = 0; i < aut.nbEtat; i++)
        {
            printf("Etat %d\n", i);
            for (int j = 0; j < nbAlphabet; j++)
            {
                for (int j = 0; j < nbAlphabet; j++)
                {
                    // on va regarder a quel etat d'arrive(nom groupe) correspond la transition avec etat de depart k de la lettre j
                    if ((aut.listeTransition[k].etatDepart == i) && (aut.listeTransition[k].lettre == alphabet[j]))
                    {
                        etapeMinimisation[i][j] = groupe[aut.listeTransition[k].etatArrivee];
                        printf("%d  ", etapeMinimisation[i][j]);
                        k++;
                    }
                }
            }
            printf("\n");
        }
        k = 0;

        //
        // BILAN
        /*
        ex:
            INIT | 1   1   3
            --------------------
              a  | 1   1   2
              b  | 2   3   3
            --------------------
        -> BILAN | 1   1   3
        */
        copy_tab(groupe, groupeAvant, aut.nbEtat); // on sauvegarde les nom des anciens groupes

        int tabBoolNomDejaDonne[aut.nbEtat]; // tableau de booleen qui indique si on a déjà donné un nom au groupe
        initialise_tab_int(tabBoolNomDejaDonne, aut.nbEtat, 0);

        // printf("\n");

        // On donne des nouveaux noms aux groupes
        for (int i = 0; i < aut.nbEtat; i++) // on s'occupe des colonnes de la meme valeur
        {
            for (int j = 0; j < aut.nbEtat; j++)
            {
                if (i != j && tabBoolNomDejaDonne[j] == 0)
                {
                    /*printf("etape i = %d, %d\n", etapeMinimisation[i][0], etapeMinimisation[i][1]);
                    printf("etape j = %d, %d\n", etapeMinimisation[j][0], etapeMinimisation[j][1]);
                    printf("---\n");*/
                    // si les tabs sont egaux on garde le meme nom
                    if (tab_sont_egaux(etapeMinimisation[i], etapeMinimisation[j], nbAlphabet))
                    {
                        printf("Tab egaux\n");
                        groupe[j] = groupe[i];
                        tabBoolNomDejaDonne[i] = 1;
                        tabBoolNomDejaDonne[j] = 1;
                    }
                }
            }
        }
        printf("tabBool = ");
        for (int i = 0; i < aut.nbEtat; i++)
        {
            printf("%d ", tabBoolNomDejaDonne[i]);
        }
        printf("\n");
        int pl = max(groupe, aut.nbEtat);

        for (int i = 0; i < aut.nbEtat; i++)
        {
            if (tabBoolNomDejaDonne[i] == 0)
            {
                // si le nom de la colonne differente est deja pris
                for (int j = 0; j < aut.nbEtat; j++)
                {
                    if (i != j)
                    {
                        if (groupe[i] == groupe[j])
                        {
                            groupe[i] = pl;
                            tabBoolNomDejaDonne[i] = 1;
                            pl++;
                        }
                        else // sinon le nom est dispo on le change pas
                            tabBoolNomDejaDonne[i] = 1;
                    }
                }
            }
        }
        // on arrete l'algo si on a 2 bilans identiques
        if (tab_sont_egaux(groupe, groupeAvant, aut.nbEtat))
            bilanIdentique = 0;

        printf("groupe : ");
        for (int i = 0; i < aut.nbEtat; i++)
        {
            printf("%d  ", groupe[i]);
        }
        printf("\n");
        printf("groupeAvant : ");
        for (int i = 0; i < aut.nbEtat; i++)
        {
            printf("%d  ", groupeAvant[i]);
        }
        printf("\n");
    }
    //

    printf("\n");

    supprime_doublons(groupe, aut.nbEtat);
    // mise a jour des etats accepteurs
    for (int i = 0; i < aut.nbEtat; i++)
    {
        if (groupeEtatAccepteur[i] != -1 && aut.etatsAccepteur[i] != -1)
        {
            aut.etatsAccepteur[i] = groupe[aut.etatsAccepteur[i]];
        }
        printf("etat accepteur : %d\n", aut.etatsAccepteur[i]);
    }

    // on met a jour les transitions de l'automate
    Transition transiTmp;
    int cpt = 0;
    int grpSupp = 0;
    int comptEtat = 0;
    for (int i = 0; i < aut.nbEtat; i++)
    {
        grpSupp = groupe[i];

        for (int j = 0; j < nbAlphabet; j++)
        {
            if (groupe[i] != -1)
            {
                transiTmp.etatDepart = groupe[i];
                transiTmp.lettre = alphabet[j];
                transiTmp.etatArrivee = etapeMinimisation[i][j];
                aut.listeTransition[cpt] = transiTmp;
                cpt++;
                if (j == 0)
                {
                    comptEtat++;
                }
                if (j == nbAlphabet - 1)
                {

                    if (grpSupp == groupe[k])
                        groupe[k] = -1;
                }
            }
        }
    }
    aut.nbEtat = comptEtat; // on met a jour le nombre d'etat

    // on enleve le reste des transitions
    for (int i = 0; i < aut.nbEtat * aut.nbEtat; i++)
    {
        transiTmp.etatDepart = -1;
        transiTmp.lettre = '\0';
        transiTmp.etatArrivee = -1;
        aut.listeTransition[cpt] = transiTmp;
    }

    ecrit_automate_dans_fichier(aut, "./AFD_minimal");
    // TODO:affiche_table_transitions(aut, caracteres_automate, nb_caracteres_automate);
    return aut;
}
//----------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    // exécution sur l'AFN
    execute_mots_sur_automate(argc, argv);

    // déterminisation
    determinise_automate(cree_automate(argv[1]));

    // exécution sur l'AFD
    argv[1] = "./AFD";
    execute_mots_sur_automate(argc, argv);

    // minimisation
    minimise_automate(cree_automate(argv[1]));

    // exécution sur l'AFD minimal
    argv[1] = "./AFD_minimal";
    execute_mots_sur_automate(argc, argv);

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
    printf("ligne %d\n", __LINE__);

    fclose(AFD);
}

void determinise_automate(Automate aut)
{
    int indices_transitions_a_supprimer[aut.nbEtat * aut.nbEtat];
    int etats_a_fusionner[aut.nbEtat];

    char caracteres_automate[100];
    int compteur_transitions_non_deterministes = 0;
    int minimum = 0;
    int num_etat, num_caractere, indice_transition, i, j, z;

    for (z = 0; z < aut.nbEtat; z++)
    {
        etats_a_fusionner[z] = 0;
    }

    // on stocke les différents caracteres de l'automate dans caracteres_automate et on récupère le nombre de caractères différents
    int nb_caracteres_automate = get_alphabet(aut, caracteres_automate);

    // déterminisation
    for (num_etat = 0; num_etat < aut.nbEtat; num_etat++)
    {
        for (num_caractere = 0; num_caractere < nb_caracteres_automate; num_caractere++)
        {
            for (indice_transition = 0; indice_transition < aut.nbEtat * aut.nbEtat; indice_transition++)
            {
                // sinon si on trouve une transition avec le meme état de départ et le meme caractère que recherché
                if ((aut.listeTransition[indice_transition].etatDepart == num_etat) && (aut.listeTransition[indice_transition].lettre == caracteres_automate[num_caractere]))
                {
                    // on stocke l'indice de la transition
                    indices_transitions_a_supprimer[compteur_transitions_non_deterministes] = indice_transition;

                    // on stocke l'état à potentiellement fusionner
                    etats_a_fusionner[compteur_transitions_non_deterministes] = aut.listeTransition[indice_transition].etatArrivee;
                    compteur_transitions_non_deterministes++;
                }
            }

            if (compteur_transitions_non_deterministes > 1)
            {
                minimum = etats_a_fusionner[0];
                // on cherche le minimum dans etats_a_fusionner
                for (i = 0; i < compteur_transitions_non_deterministes; i++)
                {
                    if (etats_a_fusionner[i] < minimum)
                    {
                        minimum = etats_a_fusionner[i];
                    }
                }

                aut.listeTransition[indices_transitions_a_supprimer[0]].etatArrivee = minimum;

                // on parcourt les transitions pour remplacer les états fusionnés par le minimum
                for (i = 0; i < aut.nbEtat * aut.nbEtat; i++)
                {
                    for (j = 0; j < compteur_transitions_non_deterministes; j++)
                    {
                        if (etats_a_fusionner[j] == (aut.listeTransition[i].etatArrivee))
                        {
                            aut.listeTransition[i].etatArrivee = minimum;
                        }
                        if (etats_a_fusionner[j] == (aut.listeTransition[i].etatDepart))
                        {
                            aut.listeTransition[i].etatDepart = minimum;
                        }
                    }
                }

                for (i = 1; i < compteur_transitions_non_deterministes; i++)
                {
                    aut.listeTransition[indices_transitions_a_supprimer[i]].lettre = '\0';
                }
            }

            // on réinitialise les variables nécessaires pour le prochain caractère
            compteur_transitions_non_deterministes = 0;
            for (z = 0; z < aut.nbEtat; z++)
            {
                etats_a_fusionner[z] = 0;
            }
        }
    }

    ecrit_automate_dans_fichier(aut, "./AFD");
    affiche_table_transitions(aut, caracteres_automate, nb_caracteres_automate);
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