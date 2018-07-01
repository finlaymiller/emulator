/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Mainline header file
- Finlay Miller B00675696
- 29 June 2018
*/

#ifndef _MAIN_H_
#define _MAIN_H_

// required standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>

// definitions
#define VERSION "0.1 (29 JUNE 2018)"
#define LR 4	// link register
#define SP 5	// stack pointer
#define PSW 6	// program status word
#define PC 7	// program counter

// create new memory
union Memory memory;

// registerfile
unsigned short regfile[8];

typedef struct Emulator Emulator;

// required custom files
#include "loader.h"
#include "cpu.h"
#include "debugger.h"
#include "memory.h"

// inaccessible by programmer
unsigned short MAR;
unsigned short MDR;
unsigned short IR;
unsigned short TMP;
enum RC { REG, CNST };

struct Emulator
{
	Debugger *debugger;
	CPU *cpu;
	//Union Memory *memory;
};

#endif