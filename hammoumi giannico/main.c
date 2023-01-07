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
    int nb_espaces;                        // pour l'affichage dans le terminal
    char test_fgetc = 's';

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
                    if (nb_transitions_prises == 1)
                    {

                        nb_espaces = 6 + strlen(argv[mot_execute]);
                    }
                    else
                    {
                        nb_espaces = 6 + 9 * (nb_transitions_prises - 1);

                        for (k = 0; k < nb_transitions_prises; k++)
                        {
                            nb_espaces = nb_espaces + strlen(argv[mot_execute]) - k;
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

                    // calcul du nombre d'espace pour l'affichage dans le terminal
                    if (nb_transitions_prises == 1)
                    {
                        nb_espaces = 6 + strlen(argv[mot_execute]);
                    }
                    else
                    {
                        nb_espaces = 6 + 9 * (nb_transitions_prises - 1);

                        for (k = 0; k < nb_transitions_prises; k++)
                        {

                            nb_espaces = nb_espaces + strlen(argv[mot_execute]) - k;
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

int main(int argc, char *argv[])
{
    execute_mots_sur_automate(argc, argv);
    return 0;
}

// renvoie 0 si les tableaux contiennent les mêmes éléments
int compare_tableaux(char a[], char b[], int taille_tableaux)
{
    for (int i = 0; i < taille_tableaux; i++)
    {
        if (a[i] != b[i])
            return 1;
    }
    return 0;
}

// copie les éléments du tableau b dans le tableau a
void copie_elements(char a[], char b[], int taille_tableaux)
{
    for (int i = 0; i < taille_tableaux; i++)
    {
        a[i] = b[i];
    }
}