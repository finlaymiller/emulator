/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- CPU headerfile
- Finlay Miller B00675696
- 29 June 2018
*/

#ifndef _CPU_H_
#define _CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

// CPU structure
typedef struct CPU
{
	bool running;

	unsigned int pc, sp;
	unsigned int sr;
	unsigned int cg2;

	unsigned int r0, r1, r2, r3;
};


#endif