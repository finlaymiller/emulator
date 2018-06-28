/*
- S-record Loader for the X-Makina Project
- ECED3403 Assignment 2
- Loader mainline
- Finlay Miller B00675696
- 26 June 2018
*/

#include "loader.h"

struct S_Record read_record(char *input)
{
    struct S_Record rec;
    char *tmp = (char*) malloc(INPUT_SIZE);
    int tmpi;

    if (strlen(input) < 8)
    {
        printf("Detected an S record that was too short.\n");
        exit(EXIT_FAILURE);
    }

    rec.type = (int) input[1] - '0';
    strncpy(tmp, input + 2, 2);

}

void loader(CPU &cpu, int *memory, char* filename)
{

}

int h2d(char *h)
{
    int size = strlen(h);
    int tmp, sum = 0;
}

