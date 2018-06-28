/*
- S-record Loader for the X-Makina Project
- ECED3403 Assignment 2
- Loader header file
- Finlay Miller B00675696
- 14 June 2018
*/

#ifndef LOADER_H
#define LOADER_H

#define INPUT_SIZE 1000
#define S_REC_SIZE 78

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct S_Record
{
    char type[2];
    int  count;
    char address[8];
    char data[64];
    char checksum[2];
};

struct S_Record read_record(char *srec);
void loader(CPU &cpu, int *memory, char* filename);
int h2d(char *h);

#endif