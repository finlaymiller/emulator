/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Headerfile for the various execute functions
- Finlay Miller B00675696
- 19 July 2018
*/

#ifndef _EXECUTE_FUNCTION_H_
#define _EXECUTE_FUNCTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "cpu.h"
#include "bitmasks.h"
#include "bus.h"

unsigned short DST, SRC;

// generic type functions
void execute_BRA(Emulator *emulator);
void execute_ALU(Emulator *emulator);
void execute_REG(Emulator *emulator);

// instruction specific functions
void execute_LDST(Emulator *emulator);
void execute_LDRSTR(Emulator *emulator);
void execute_MOV(Emulator *emulator);
void execute_ADD(Emulator *emulator, mi_ALU);
void execute_DADD(Emulator *emulator, mi_ALU);
void execute_XOR(Emulator *emulator, mi_ALU);
void execute_AND(Emulator *emulator, mi_ALU);

unsigned short bcd_add(unsigned short, unsigned short, unsigned short *);

#endif // !_EXECUTE_FUNCTION_H_
