/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Headerfile for interrupt-related things
- Finlay Miller B00675696
- 19 July 2018
*/

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

// required structs for interrupt vectors
struct vec_bits
{
	unsigned int C : 1;
	unsigned int Z : 1;
	unsigned int N : 1;
	unsigned int SLP:1;
	unsigned int V : 1;
	unsigned int PRI:3;
	unsigned int RES:8;
};

struct vec_tab
{
	struct vec_bits iv_bits;
	unsigned short PC;
};

union vec_overlay
{
	struct vec_tab iv;		// 32 bits
	unsigned short mem[2];	// 2 words in memory
};

// function declarations
extern union vec_overlay *iv_ptr;

#endif // !_INTERRUPTS_H_
