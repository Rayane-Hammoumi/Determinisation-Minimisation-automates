#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    printf(" nb d'arguments: %d \n", argc);
    printf("%c \n", argv[1][10]);
    int len = strlen(argv[1]);

    printf("len:   %d \n", len);

    len = strlen(argv[1]) + strlen(argv[2]);

    printf("len:   %d \n", len);

    len--;

    printf("len:   %d \n", len);

    char variable[len];
    strcpy(variable, argv[1] + 1);
    printf("variable : %s\n", variable);

    return 0;
}