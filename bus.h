/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Headerfile for the bus functions
- Finlay Miller B00675696
- 19 July 2018
*/

#ifndef _BUS_H_
#define _BUS_H_

// libraries
#include <errno.h>

// files
#include "storage.h"
#include "cpu.h"

// control enums
enum SIZE { WORD, BYTE };
enum ACTION { READ, WRITE };

// function declarations		(or are they definitions?)
void bus(Emulator *emulator, enum SIZE, enum ACTION);

#endif // !_BUS_H_
