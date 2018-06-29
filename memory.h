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


// Memory
union mem
{
	unsigned int bytemem;
	unsigned int wordmem;
};

void init_mem();
void uninit_mem();



#endif