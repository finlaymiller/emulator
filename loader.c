/*
- S-record Loader for the X-Makina Project
- ECED3403 Assignment 2
- Loader mainline
- Finlay Miller B00675696
- 14 June 2018
*/

#include "loader.h"

struct srec
{
    char type[2];
    char count[2];
    char address[8];
    char data[64];
    char checksum[2];
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Not enough arguments. You probably forgot to drag'n'drop again.\n");
		getchar();
        return 1;
    }


    
    // read input file
    FILE *input = fopen(argv[1], "r");
    int i = 0;
    char *input_raw[INPUT_SIZE];
    char *line = NULL;
    size_t size = sizeof(input_raw)/sizeof(input_raw[0]);

    // open input file
    if (input == NULL)
    {
        printf("Failed to open input file.\n");
        getchar();
        return 1;
    }
    // store file lines in array
    while (fgets(line, sizeof(line), input) != NULL)
    {
        input_raw[i++] = line;
        printf("%s\n", line);
        if (i > INPUT_SIZE)
        {
            printf("Out of room in input array.\n");
            return 1;
        }
    }

    // parse s rec
    char temp[2];
    char *tmp;
    struct srec s_array[INPUT_SIZE];
    int s0 = 0;
    for (int j = 0; j < size; j++)
    {
        strcpy(tmp, input_raw[j]);

        if (strlen(tmp) > S_REC_SIZE)
        {
            printf("S Record #%d is too long.\n", j);
            return 1;
        }

        // get type & count
        strncpy(s_array[j].type, tmp, 2);
        memmove(tmp, tmp + 2, strlen(tmp));
        strncpy(s_array[j].count, tmp, 2);
        memmove(tmp, tmp + 2, strlen(tmp));

        // get address
        strcpy(temp, s_array[j].type);

        switch (temp[1])
        {
            case '0':
                s0 = 1;
                strncpy(s_array[j].address, tmp, 8);
                memmove(tmp, tmp + 8, strlen(tmp));
                break;
            case '1':
                strncpy(s_array[j].address, tmp, 4);
                memmove(tmp, tmp + 4, strlen(tmp));
                break;
            case '2':
                strncpy(s_array[j].address, tmp, 6);
                memmove(tmp, tmp + 6, strlen(tmp));
                break;
            case '3':
                strncpy(s_array[j].address, tmp, 8);
                memmove(tmp, tmp + 8, strlen(tmp));
                break;
            case '5':
                strncpy(s_array[j].address, tmp, 4);
                memmove(tmp, tmp + 6, strlen(tmp));
                break;
            case '7':
                strncpy(s_array[j].address, tmp, 8);
                memmove(tmp, tmp + 8, strlen(tmp));
                break;
            case '8':
                strncpy(s_array[j].address, tmp, 6);
                memmove(tmp, tmp + 6, strlen(tmp));
                break;
            case '9':
                strncpy(s_array[j].address, tmp, 4);
                memmove(tmp, tmp + 4, strlen(tmp));
                break;
            default:
                printf("Invalid S Record type detected on line %d.\n", j);
        }

        // get data
        strncpy(s_array[j].data, tmp, strlen(tmp) - 2);
        memmove(tmp, tmp + strlen(tmp) - 2, strlen(tmp));

        // get checksum
        strncpy(s_array[j].checksum, tmp, 2);
        memmove(tmp, tmp + 2, strlen(tmp));

        printf("Remainder is %d.\n", strlen(tmp));
    }

    fclose(input);
    return 0;
}