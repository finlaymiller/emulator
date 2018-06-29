/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Memory related functions mainline
- Finlay Miller B00675696
- 29 June 2018
*/

#include "memory.h"



void init_mem()
{
	union mem memory;

	memory.bytemem = 65536;
	memory.wordmem = 32768;
}

void uninit_mem()
{
	free(memory);
}