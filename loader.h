#ifndef _LOADER_H_
#define _LOADER_H_

// libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

// other files
#include "devices.h"
#include "storage.h"

#define INPUT_SIZE 1000
#define S_REC_SIZE 256

typedef struct S_Record
{
	int type;
	int count;
	int address;
	char data[S_REC_SIZE];
} S_Record;

enum SREC_ERRORS
{
	MISSING_S,
	TOO_SHORT,
	UNEXPECTED_TYPE,
	BAD_TYPE,
	BAD_CHECKSUM
};

int loader(Emulator *emulator);
S_Record read_record(Emulator *emulator, char input[]);
int load_record(Emulator *emulator, S_Record rec);
void srec_error(Emulator *emulator, enum SREC_ERRORS err, char srec[]);
int h2d(char *h);

#endif // !_LOADER_H_