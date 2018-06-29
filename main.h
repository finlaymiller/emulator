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

typedef struct CPU CPU;
typedef struct Debugger Debugger;

typedef struct Emulator
{
	Debugger *debugger;
	CPU *cpu;
};

#endif