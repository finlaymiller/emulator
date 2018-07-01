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
#include "main.h"

#define MAX_FILENAME 255

typedef struct Debugger
{
	bool disassemble_mode;
	bool devices;
	bool debug_mode;
	bool quit;

	enum BP_TYPE { LINE, CLOCK } BP_TYPE;
	unsigned int marker;
} Debugger;

enum DEBUG_ERRORS
{
	BAD_FILETYPE,
	NO_FILES
};

char *err_diag_d[] =
{
	(char *) "\nInvalid input file - .xme type required",
	(char *) "\nNo input files provided",
	(char *) "\nInvalid S rec - detected an S record not of type 0, 1, or 9",
	(char *) "\nInvalid S rec - bad record type",
	(char *) "\nInvalid S rec - error in checksum",
};

void debug_error(enum DEBUG_ERRORS err);
int run_debugger(Emulator *emulator);

#endif