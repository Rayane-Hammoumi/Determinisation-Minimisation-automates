#include "headers.h"

// fonction qui affiche la table des transitions d'un automate minimalise
//  elle prend en paramètre, le nombre de caractere ds l'alphabet, la table de transition, la taille de ce tab et l'alphabet
Automate minimise_automate(Automate aut)
{

    int etatTrouve = 0;
    int etats = aut.nbEtats;
    int etatAccTrouve = 0;
    for (int i = 0; i < aut.nbEtats; i++)
    {
        etatTrouve = 0;
        etatAccTrouve = 0;

        for (int j = 0; j < aut.nbEtats * aut.nbEtats; j++)
        {
            if (aut.listeTransition[j].etatDepart == i)
            {
                etatTrouve = 1;
            }
            if (aut.etatsAccepteurs[i] != -1 && (aut.etatsAccepteurs[i] == aut.listeTransition[j].etatDepart || aut.etatsAccepteurs[i] == aut.listeTransition[j].etatArrivee))
            {
                etatAccTrouve = 1;
            }
            if (etatTrouve && etatAccTrouve)
            {
                break;
            }
        }
        if (etatAccTrouve == 0)
        {
            aut.etatsAccepteurs[i] = -1;
        }
        if (etatTrouve == 0)
            etats--;
    }
    int sauvePos = -1;
    for (int i = 0; i < aut.nbEtats; i++)
    {
        if (aut.etatsAccepteurs[i] == -1)
        {
            sauvePos = i;
        }
        for (int j = sauvePos + 1; j < aut.nbEtats; j++)
        {
            if (sauvePos != -1 && aut.etatsAccepteurs[j] != -1)
            {
                aut.etatsAccepteurs[sauvePos] = aut.etatsAccepteurs[j];
                aut.etatsAccepteurs[j] = -1;
                sauvePos = -1;
            }
        }
    }

    aut.nbEtats = etats;

    int groupe[aut.nbEtats]; // Le nom des différents groupe créés, gp[0]: 1etat, gp[1]:2etat... = bilan
    initialise_tab_int(groupe, aut.nbEtats, -1);
    int groupeAvant[aut.nbEtats]; // utile pour arrêter l'algo si on trouve 2 bilans identiques
    initialise_tab_int(groupeAvant, aut.nbEtats, -1);
    int groupeEtatAccepteur[aut.nbEtats]; // utile pour sauvegarder l'ancien nom des états accepteurs
    copy_tab(aut.etatsAccepteurs, groupeEtatAccepteur, aut.nbEtats);

    char alphabet[100];
    initialise_tab_char(alphabet, 100);

    // définit le nom des groupe : INITIALISATION
    init_minimisation(aut, groupe);

    /*
    ex:
    ->   INIT | 1   1   3
        --------------------
    */
    int nbAlphabet = get_alphabet(aut, alphabet);   // nombre de caractères dans l'alphabet
    int etapeMinimisation[aut.nbEtats][nbAlphabet]; // les différentes étapes de minimisation => table transition

    for (int i = 0; i < aut.nbEtats; i++)
    {
        for (int j = 0; j < nbAlphabet; j++)
        {
            etapeMinimisation[i][j] = -1;
        }
    }

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
        étape    ->    a  | 1   1   2
                 ->    b  | 2   3   3
        */
        // cette boucle va créer les chiffres à mettre dans les différentes étapes
        for (int i = 0; i < aut.nbEtats; i++)
        {
            for (int j = 0; j < nbAlphabet; j++)
            {
                for (int j = 0; j < nbAlphabet; j++)
                {
                    // on va regarder à quel état d'arrivée (nom groupe) correspond la transition avec état de départ k de la lettre j
                    if ((aut.listeTransition[k].etatDepart == i) && (aut.listeTransition[k].lettre == alphabet[j]))
                    {
                        etapeMinimisation[i][j] = groupe[aut.listeTransition[k].etatArrivee];
                        k++;
                    }
                }
            }
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
        copy_tab(groupe, groupeAvant, aut.nbEtats); // on sauvegarde les nom des anciens groupes

        int tabBoolNomDejaDonne[aut.nbEtats]; // tableau de booléens qui indique si on a déjà donné un nom au groupe
        initialise_tab_int(tabBoolNomDejaDonne, aut.nbEtats, 0);

        // On donne des nouveaux noms aux groupes
        for (int i = 0; i < aut.nbEtats; i++) // on s'occupe des colonnes de la même valeur
        {
            for (int j = 0; j < aut.nbEtats; j++)
            {
                if (i != j && tabBoolNomDejaDonne[j] == 0)
                {
                    // si les tabs sont égaux on garde le même nom
                    if (tab_sont_egaux(etapeMinimisation[i], etapeMinimisation[j], nbAlphabet))
                    {
                        groupe[j] = groupe[i];
                        tabBoolNomDejaDonne[i] = 1;
                        tabBoolNomDejaDonne[j] = 1;
                    }
                }
            }
        }

        int pl = max(groupe, aut.nbEtats);

        for (int i = 0; i < aut.nbEtats; i++)
        {
            if (tabBoolNomDejaDonne[i] == 0)
            {
                // si le nom de la colonne différente est déjà pris
                for (int j = 0; j < aut.nbEtats; j++)
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
        // on arrête l'algo si on a 2 bilans identiques
        if (tab_sont_egaux(groupe, groupeAvant, aut.nbEtats))
            bilanIdentique = 0;
    }
    // on vérifie que tout les etats sont accepteur, car si c'est vrai on a pas besoin d'effectuer de changement
    int comptAccept = 0;
    for (int i = 0; i < aut.nbEtats; i++)
    {
        if (groupe[i] == 0)
            comptAccept++;
    }
    Transition transiTmp;
    transiTmp.etatDepart = -1;
    transiTmp.lettre = '\0';
    transiTmp.etatArrivee = -1;

    if (comptAccept != aut.nbEtats)
    {
        supprime_doublons(groupe, aut.nbEtats);

        // mise à jour des états accepteurs
        for (int i = 0; i < aut.nbEtats; i++)
        {
            if (groupeEtatAccepteur[i] != -1 && aut.etatsAccepteurs[i] != -1)
            {
                aut.etatsAccepteurs[i] = groupe[aut.etatsAccepteurs[i]];
            }
        }

        // on met à jour les transitions de l'automate
        Transition transiTmp;
        int cpt = 0;
        int comptEtat = 0;

        for (int i = 0; i < aut.nbEtats; i++)
        {
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
                        groupe[i] = -1;
                    }
                }
            }
        }
        aut.nbEtats = comptEtat; // on met a jour le nombre d'état
        // on enlève le reste des transitions
        for (int i = cpt; i < aut.nbEtats * aut.nbEtats; i++)
        {
            transiTmp.etatDepart = -1;
            transiTmp.lettre = '\0';
            transiTmp.etatArrivee = -1;
            aut.listeTransition[i] = transiTmp;
        }
    }

    // on va supprimer les transitions qui ne mènent jamais à des états accepteurs s'il y en a
    int cptSuppr = 0;
    int nbEtats = aut.nbEtats;
    int j = 0;
    int etatArrivee = 0;
    for (int i = 0; i < nbEtats * nbEtats; i++)
    {
        if (aut.listeTransition[i].etatArrivee != -1 && aut.listeTransition[i].etatDepart != -1)
        {
            j = 0;
            etatArrivee = aut.listeTransition[i].etatArrivee;
            while (!est_ds_tableau_int(etatArrivee, aut.etatsAccepteurs, aut.nbEtats))
            {
                if (i != j && etatArrivee == aut.listeTransition[j].etatDepart)
                {
                    etatArrivee = aut.listeTransition[j].etatArrivee;
                }

                if (etatArrivee == -1 || aut.listeTransition[j].lettre == '\0')
                {
                    aut.listeTransition[i] = transiTmp;
                    break;
                }
                j++;
            }
        }
    }
    // on va supprimer les états qui ne mènent jamais à des états accepteurs s'il y en a
    etatTrouve = 0;
    etats = aut.nbEtats;
    for (int i = 0; i < aut.nbEtats; i++)
    {
        etatTrouve = 0;

        for (int j = 0; j < aut.nbEtats * aut.nbEtats; j++)
        {
            if (aut.listeTransition[j].etatDepart == i)
            {
                etatTrouve = 1;
                break;
            }
        }
        if (etatTrouve == 0)
            etats--;
    }
    aut.nbEtats = etats;

    ecrit_automate_dans_fichier(aut, "./AFD_minimal");
    affiche_table_transitions(aut, alphabet, nbAlphabet);
    return aut;
}