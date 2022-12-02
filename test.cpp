#include <stdio.h>
#include <stdlib.h>

int main()
{
    char* code = (char*) calloc(20, sizeof(char));
    code[0] = 1;
    *(int*)(code + 1) = 267;
    *(int*)(code + 5) = 2;

    fprintf(stderr, "%d %d %d\n", code[0], *(int*)(code + 1), *(int*)(code + 1) - 256);

    free(code);

    return 0;
}
