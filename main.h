/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Mainline header file
- Finlay Miller B00675696
- 19 July 2018
*/

#ifndef _MAIN_H_
#define _MAIN_H_

// required standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

// define structs
typedef struct Emulator Emulator;
typedef struct Debugger Debugger;
typedef struct CPU CPU;
typedef struct Device Device;
typedef struct Next_Event Next_Event;
typedef struct Cache Cache;

// other files
#include "debugger.h"
#include "cpu.h"
#include "storage.h"
#include "devices.h"
#include "utilities.h"
#include "cache.h"

// definitions
#define VERSION "0.3 (04 JULY 2018)"
#define MAX_FILE_LEN 255
#define DEVCNT 8

extern bool waiting_for_signal;

struct Emulator
{
	// component structures
	Debugger *debugger;
	CPU *cpu;
	Device *device[DEVCNT];
	Next_Event *n_e;
	Cache *cache;

	// flags
	bool waiting_for_signal;
	bool accessed_cache;

	// filenames
	char ifile[MAX_FILE_LEN];
	char ofile[MAX_FILE_LEN];
	char dfile[MAX_FILE_LEN];
	char cfile[MAX_FILE_LEN];
	long int df_pos;	// position in device file, used for fseek
};
#endif // !_MAIN_H_