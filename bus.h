/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Bus function headerfile
- Finlay Miller B00675696
- 1 JULY 2018
*/

#ifndef _BUS_H_
#define _BUS_H_

#include <stdio.h>
#include <string.h>
#include "main.h"

enum BW = { BYTE, WORD };
enum RW = { READ, WRITE };

void bus(unsigned short MAR, unsigned short MDR, enum BW, enum RW);

#endif