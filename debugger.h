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

typedef struct Debugger
{
	bool disassemble_mode;
	bool devices;
	bool debug_mode;
	bool quit;

	enum BP_TYPE { LINE, CLOCK };
	unsigned int marker;
};

#endif