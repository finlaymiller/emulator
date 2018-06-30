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

// required emulator files
#include "loader.h"
#include "cpu.h"
#include "debugger.h"
#include "memory.h"

// definitions
#define VERSION "0.1 (29 JUNE 2018)"
// register declaration
#define LR 4
#define SP 5
#define PSW 6
#define PC 7

// create new memory
union Memory memory;

// registerfile
unsigned short regfile[8];

// inaccessible by programmer
unsigned short MAR;
unsigned short MDR;
unsigned short IR;
unsigned short TMP;
enum RC { REG, CNST };

// Typedefs so I don't have to keep writing struct
typedef struct CPU CPU;
typedef struct Storage Storage;
typedef struct Debugger Debugger;
typedef struct Emulator Emulator;

struct Emulator
{
	Debugger *debugger;
	CPU *cpu;
	Memory *memory;
};

#endif