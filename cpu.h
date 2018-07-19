/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- CPU headerfile
- Finlay Miller B00675696
- 19 July 2018
*/

#ifndef _CPU_H_
#define _CPU_H_

// libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>

// other files
#include "main.h"
#include "storage.h"
#include "debugger.h"
#include "execute_functions.h"
#include "decode_functions.h"
#include "bus.h"
#include "devices.h"

// print data as binary macros
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT8(data)    \
    (((data) & 0x80ll) ? '1' : '0'), \
    (((data) & 0x40ll) ? '1' : '0'), \
    (((data) & 0x20ll) ? '1' : '0'), \
    (((data) & 0x10ll) ? '1' : '0'), \
    (((data) & 0x08ll) ? '1' : '0'), \
    (((data) & 0x04ll) ? '1' : '0'), \
    (((data) & 0x02ll) ? '1' : '0'), \
    (((data) & 0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 \
    PRINTF_BINARY_PATTERN_INT8 PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(data) \
    PRINTF_BYTE_TO_BINARY_INT8((data) >> 8), PRINTF_BYTE_TO_BINARY_INT8(data)

// CPU object
typedef struct CPU
{
	// flags
	bool running;
	bool sleeping;

	// variables
	signed int PRIORITY;// cpu priority level
	signed int SYSCLK;	// system clock counter
	signed int END;		// max system clock amount
} CPU;

// function declarations
void run(Emulator *emulator);
void fetch(Emulator *emulator);
void decode(Emulator *emulator);
void execute(Emulator *emulator);
void maintenance(Emulator *emulator);

#endif