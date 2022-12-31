#include "headers.h"

//

void affiche_AFN()
{
    char caractere_lu;

    FILE *fichier = fopen("AFN.txt", "r");

    if (fichier != NULL)
    {
        caractere_lu = fgetc(fichier);

        while (caractere_lu != EOF)
        {
            printf("%c", caractere_lu);
            caractere_lu = fgetc(fichier);
        }
    }
    else
    {
        printf("erreur fichier");
    }

    printf("\n\nFin d'affichage de l'AFN\n\n");

    fclose(fichier);
}

//

void execute_AFN(int argc, char *argv[])
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

    char etats_accepteurs[20] = {'s',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's',
                                 's'};

    fseek(fichier, 2, SEEK_SET); // on se positionne en 2e ligne
    char caractere_lu = fgetc(fichier);

    int i = 0, j = 0, k = 0;
    int position_transitions = 3; // indique à quelle position dans le fichier commence
                                  // la première transition (calculée plus bas)

    // on remplit etats_accepteurs[]
    while (caractere_lu != '\n')
    {
        if (caractere_lu != ' ')
        {
            etats_accepteurs[j] = caractere_lu;
            j++;
        }

        caractere_lu = fgetc(fichier);
        position_transitions++;
    }

    j = 0;
    char etat_actuel = '0';
    int mot_accepte = 0;
    int mot_execute = 2; // incrémente quand on a finit d'exécuter un mot sur l'automate

    // à chaque transition, on note:
    long historique_positions[100]; // sa position dans le fichier grâce à ftell
    char historique_etats[100];     // l'état emprunté

    int nb_transitions_prises = 0;
    int positions_interdites[100]; // contient les positions dans le fichier qui mènent à des

    for (k = 0; k < 100; k++)
    {
        positions_interdites[k] = 0;
        historique_etats[k] = 's';
        historique_positions[k] = 0;
    }
    // transitions qu'on a déjà testé et qui ne fonctionnent pas
    int compteur_positions_interdites = 0;
    int compteur = 0;
    int est_interdite = 0;
    int nb_espaces;
    int sauter_ligne;

    fseek(fichier, position_transitions + 2, SEEK_SET); // on replace le curseur au début des transitions au 1er caractère
    caractere_lu = fgetc(fichier);

    // pour chaque mot en entrée sauf le nom de ce programme
    for (mot_execute = 2; mot_execute < argc; mot_execute++)
    {
        printf("\n(0, %s) |-", argv[mot_execute]);

        // on break si on a accepté le mot ou si on est à l'état 0 et qu'on a finit de lire le fichier
        while (1)
        {
            // si on lit le caractere recherché
            if (caractere_lu == argv[mot_execute][i])
            {
                fseek(fichier, -3, SEEK_CUR);
                caractere_lu = fgetc(fichier);
                fseek(fichier, 2, SEEK_CUR); // on revient à la position juste après le caractère

                if (caractere_lu == etat_actuel)
                {
                    while (positions_interdites[compteur] != 0)
                    {

                        if (positions_interdites[compteur] == ftell(fichier))
                        {
                            est_interdite = 1;
                            break;
                        }
                        compteur++;
                    }
                    compteur = 0;
                }

                // si pour y accéder, la transistion part bien de l'etat actuel et que la position n'est pas interdite
                if ((caractere_lu == etat_actuel) && (est_interdite == 0))
                {
                    historique_positions[nb_transitions_prises] = ftell(fichier);
                    fseek(fichier, 1, SEEK_CUR); // on revient à la position juste avant l'état de destination
                    caractere_lu = fgetc(fichier);
                    etat_actuel = caractere_lu; // l'état actuel devient la destination de la transition
                    historique_etats[nb_transitions_prises] = etat_actuel;
                    fseek(fichier, position_transitions + 2, SEEK_SET); // on replace le curseur au début des transitions au 1er caractère
                    nb_transitions_prises++;

                    printf(" (%c, ", etat_actuel);
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
                    fseek(fichier, 5, SEEK_CUR); // on deplace le curseur de 5
                    est_interdite = 0;
                }
            }

            // sinon si on a finit de lire le fichier
            else if (caractere_lu == EOF)
            {
                if (nb_transitions_prises == 0) // si on est toujours à l'état initial, le mot ne peut pas être accepté
                {

                    etat_actuel = '0';
                    est_interdite = 0;
                    printf(" ko\n");
                    fseek(fichier, position_transitions + 2, SEEK_SET); // on replace le curseur au début des transitions au 1er caractère
                    i = 0;
                    nb_transitions_prises = 0;
                    compteur_positions_interdites = 0;
                    for (k = 0; k < 100; k++)
                    {
                        positions_interdites[k] = 0;
                        historique_etats[k] = 's';
                        historique_positions[k] = 0;
                    }

                    fseek(fichier, position_transitions + 2, SEEK_SET); // on replace le curseur au début des transitions au 1er caractère
                    caractere_lu = fgetc(fichier);                      // on lit le caractère suivant
                    break;                                              // on passe au mot suivant
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

                    // la position actuelle devient interdite
                    positions_interdites[compteur_positions_interdites] = historique_positions[nb_transitions_prises - 1];
                    compteur_positions_interdites++;
                    nb_transitions_prises--;
                    if (nb_transitions_prises == 0)
                    {
                        etat_actuel = '0';
                    }
                    else
                    {
                        etat_actuel = historique_etats[nb_transitions_prises - 1];
                    }

                    i--;
                    fseek(fichier, position_transitions + 2, SEEK_SET); // on replace le curseur au début des transitions au 1er caractère
                }
            }
            else
            {
                fseek(fichier, 5, SEEK_CUR); // on deplace le curseur de 5
            }

            if (i >= strlen(argv[mot_execute])) // si on a lu tout le mot
            {
                // on verifie si etat_actuel est contenu dans etats_accepteurs, si oui ok sinon ko
                while (etats_accepteurs[j] != 's')
                {
                    if (etat_actuel == etats_accepteurs[j])
                    {
                        printf(" ok\n");
                        mot_accepte = 1;
                        break;
                    }

                    j++;
                }

                j = 0;

                if (mot_accepte == 1)
                {
                    mot_accepte = 0;
                    est_interdite = 0;
                    etat_actuel = '0';
                    i = 0;
                    nb_transitions_prises = 0;
                    compteur_positions_interdites = 0;
                    for (k = 0; k < 100; k++)
                    {
                        positions_interdites[k] = 0;
                        historique_etats[k] = 's';
                        historique_positions[k] = 0;
                    }

                    fseek(fichier, position_transitions + 2, SEEK_SET); // on replace le curseur au début des transitions au 1er caractère
                    caractere_lu = fgetc(fichier);                      // on lit le caractère suivant
                    break;
                }
                else // on a réussi à lire le mot mais on ne tombe pas dans un état accepteur
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

                    positions_interdites[compteur_positions_interdites] = historique_positions[nb_transitions_prises - 1];
                    compteur_positions_interdites++;
                    nb_transitions_prises--;
                    if (nb_transitions_prises == 0)
                    {
                        etat_actuel = '0';
                    }
                    else
                    {
                        etat_actuel = historique_etats[nb_transitions_prises - 1];
                    }
                    i--;
                    fseek(fichier, position_transitions + 2, SEEK_SET); // on replace le curseur au début des transitions au 1er caractère
                }
            }

            caractere_lu = fgetc(fichier); // on lit le caractère suivant
        }
    }

    fclose(fichier);
}

int main(int argc, char *argv[])
{
    affiche_AFN();
    execute_AFN(argc, argv);

    return 0;
}