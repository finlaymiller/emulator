<<<<<<< HEAD
/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Memory related functions headerfile
- Finlay Miller B00675696
- 29 June 2018
*/

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

unsigned int *DEVICES;
unsigned int *CODE;
unsigned int *IVT;

// Memory setup
#define MAX_BYTE_SIZE (1<<16)	// 65536
#define MAX_WORD_SIZE (1<<15)	// 32768

union Memory {
	unsigned char byte[MAX_BYTE_SIZE];
	unsigned short word[MAX_WORD_SIZE];
};

// make memory components accessible program-wide
extern union Memory memory;
extern unsigned short regfile[8];
extern unsigned short MDR;
extern unsigned short MAR;
extern unsigned short IR;

void init_mem();

#endif

=======
/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Memory related functions headerfile
- Finlay Miller B00675696
- 29 June 2018
*/

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

unsigned int *DEVICES;
unsigned int *CODE;
unsigned int *IVT;

// Memory setup
#define MAX_BYTE_SIZE (1<<16)	// 65536
#define MAX_WORD_SIZE (1<<15)	// 32768

union Memory {
	unsigned char byte[MAX_BYTE_SIZE];
	unsigned short word[MAX_WORD_SIZE];
};

// make memory components accessible program-wide
extern union Memory memory;
extern unsigned short regfile[8];
extern unsigned short MDR;
extern unsigned short MAR;

void init_mem();

#endif

>>>>>>> 278da0870bc211b7e3ccb4e0cda1249e0ec582e5
