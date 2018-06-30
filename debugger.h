/*
- S-record Loader for the X-Makina Project
- ECED3403 Assignment 2
- Debugger header file
- Finlay Miller B00675696
- 29 June 2018
*/

#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define MAX_FILENAME 255

typedef struct Debugger
{
	bool disassemble_mode;
	bool devices;
	bool debug_mode;
	bool quit;

	enum BP_TYPE { LINE, CLOCK };
	unsigned int marker;
};

enum DEBUG_ERRORS
{
	BAD_FILETYPE,
	TOO_SHORT,
	UNEXPECTED_TYPE,
	BAD_TYPE,
	BAD_CHECKSUM
};

char *err_diag_d[] =
{
	(char *) "\nInvalid input file - .xme type required",
	(char *) "\nInvalid S rec - too short",
	(char *) "\nInvalid S rec - detected an S record not of type 0, 1, or 9",
	(char *) "\nInvalid S rec - bad record type",
	(char *) "\nInvalid S rec - error in checksum",
};

void debug_error(enum DEBUG_ERRORS err);
int run_debugger(Emulator *emulator);

#endif