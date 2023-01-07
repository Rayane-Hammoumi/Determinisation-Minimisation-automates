#include <stdio.h>
#include <string.h>

int main(void)
{
    char ch[2];
    ch[0] = 'h';
    ch[1] = 'i';

    int k = strlen(ch);
    printf("%d\n%s\n", k, ch);

    return 0;
}
