#include "headers.h"

void determinise_automate(Automate aut)
{
    int indices_transitions_a_supprimer[aut.nbEtats * aut.nbEtats];
    int etats_a_fusionner[aut.nbEtats];

    char caracteres_automate[100];
    int compteur_transitions_non_deterministes = 0;
    int minimum = 0;               // quand on fusionne des états, on les remplace tous par le plus petit numéro parmi ces états
    int minimum_est_accepteur = 0; // booléen
    int num_etat, num_caractere, indice_transition, i, j, z, h, x;

    initialise_tab_int(etats_a_fusionner, aut.nbEtats, -1);

    // on stocke les différents caracteres de l'automate dans caracteres_automate et on récupère le nombre de caractères différents
    int nb_caracteres_automate = get_alphabet(aut, caracteres_automate);

    // déterminisation

    // pour chaque numéro d'état
    for (num_etat = 0; num_etat < aut.nbEtats; num_etat++)
    {
        // pour chaque caractère de l'automate
        for (num_caractere = 0; num_caractere < nb_caracteres_automate; num_caractere++)
        {
            // pour chaque transition de l'automate
            for (indice_transition = 0; indice_transition < aut.nbEtats * aut.nbEtats; indice_transition++)
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
                    if ((etats_a_fusionner[i] != -1) && (etats_a_fusionner[i] < minimum))
                    {
                        minimum = etats_a_fusionner[i];
                    }
                }

                aut.listeTransition[indices_transitions_a_supprimer[0]].etatArrivee = minimum;

                // on parcourt les transitions pour remplacer les états fusionnés par le minimum
                for (i = 0; i < aut.nbEtats * aut.nbEtats; i++)
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
                    aut.listeTransition[indices_transitions_a_supprimer[i]].etatDepart = -1;
                    aut.listeTransition[indices_transitions_a_supprimer[i]].etatArrivee = -1;
                }

                for (x = 0; x < compteur_transitions_non_deterministes; x++)
                {
                    if ((etats_a_fusionner[x] != -1) && (est_accepteur(etats_a_fusionner[x], aut)))
                    {
                        minimum_est_accepteur = 1;
                    }
                }

                if (minimum_est_accepteur)
                {
                    // si il y a un accepteur dans etats_a_fusionner et si il n'y a pas déjà minimum dans les états accepteurs, on l'ajoute
                    for (h = 0; h < aut.nbEtats; h++)
                    {
                        if (!est_ds_tableau_int(minimum, aut.etatsAccepteurs, aut.nbEtats))
                        {
                            if (aut.etatsAccepteurs[h] == -1)
                            {
                                aut.etatsAccepteurs[h] = minimum;
                                break;
                            }
                        }

                        // si un état accepteur a été fusionné et n'est pas le minimum, alors on le met à -1
                        if (aut.etatsAccepteurs[h] != -1)
                        {
                            if ((aut.etatsAccepteurs[h] != minimum) && (est_ds_tableau_int(aut.etatsAccepteurs[h], etats_a_fusionner, aut.nbEtats)))
                            {
                                aut.etatsAccepteurs[h] = -1;
                            }
                        }
                    }
                }
            }

            // on réinitialise les variables nécessaires pour le prochain caractère
            compteur_transitions_non_deterministes = 0;
            initialise_tab_int(etats_a_fusionner, aut.nbEtats, -1);
        }
    }

    ecrit_automate_dans_fichier(aut, "./AFD");
    affiche_table_transitions(aut, caracteres_automate, nb_caracteres_automate);
}