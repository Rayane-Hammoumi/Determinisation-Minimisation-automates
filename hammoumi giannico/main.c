#include "headers.h"

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