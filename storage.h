/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Headerfile for storage-related things 
- Finlay Miller B00675696
- 19 July 2018
*/

#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <stdio.h>
#include <string.h>
#include "cpu.h"

// MEMORY
// define memory size
#define MAX_BYTE_SIZE (1<<16)	// 65536
#define MAX_WORD_SIZE (1<<15)	// 32768

// create new memory
extern union Memory {
	unsigned char byte[MAX_BYTE_SIZE];
	unsigned short word[MAX_WORD_SIZE];
}memory;

// REGISTERS
// define register names
#define reg_count 8	// number of rows
#define RorC 2		// number of columns

// accessible by programmer
extern unsigned short regfile[RorC][reg_count];

#define LR  regfile[0][4]	// link register
#define SP  regfile[0][5]	// stack pointer
#define PSW regfile[0][6]	// program status word
#define PC  regfile[0][7]	// program counter

// PSW bits
// I would have liked to use a union pointer for this but I didn't have time
enum PSW_bits { C, Z, N, SLP, V, PRI1, PRI2, PRI3 };

// inaccessible by programmer
extern unsigned short MAR;
extern unsigned short MBR;
extern unsigned short IR;
extern unsigned short TMP;

void dump(Emulator *emulator);


#endif