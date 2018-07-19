/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Storage-related functions mainline
- Finlay Miller B00675696
- 19 July 2018
*/

#include "storage.h"

unsigned short regfile[RorC][reg_count] =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0 },		// init register values
	{ 0, 1, 2, 4, 8, 32, 48, -1 }	// constants
};