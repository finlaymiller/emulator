/*
- S-record Loader for the X-Makina Project
- ECED3403 Assignment 2
- Loader header file
- Finlay Miller B00675696
- 14 June 2018
*/

#ifndef _LOADER_H_
#define _LOADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "main.h"

#define INPUT_SIZE 1000
#define S_REC_SIZE 256

struct S_Record
{
    int type;
    int count;
    int address;
    char data[S_REC_SIZE];
};

enum SREC_ERRORS
{
	MISSING_S,
	TOO_SHORT,
	UNEXPECTED_TYPE,
	BAD_TYPE,
	BAD_CHECKSUM
};

char *err_diag_s[] =
{
	(char *) "\nInvalid S rec - missing 'S' character",
	(char *) "\nInvalid S rec - too short",
	(char *) "\nInvalid S rec - detected an S record not of type 0, 1, or 9",
	(char *) "\nInvalid S rec - bad record type",
	(char *) "\nInvalid S rec - error in checksum",
};

unsigned short regfile[8];

typedef struct S_Record S_Record;

S_Record read_record(char input[]);
void srec_error(enum SREC_ERRORS err, char srec[]);
int h2d(char *h);

#endif