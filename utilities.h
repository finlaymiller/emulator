/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Headerfile for functions that don't quite fit elsewhere
- Finlay Miller B00675696
- 19 July 2018
*/

#ifndef _UTILITIES_H_
#define _UTILITIES_H_

// libraries
#include <signal.h>

// other files
#include "cpu.h"
#include "debugger.h"
#include "main.h"

// function declarations
void handle_sigint(Emulator *emulator);
void init_structs(Emulator *emulator);
void set_priority(Emulator *emulator, int level);

#endif // !_UTILITIES_H_
