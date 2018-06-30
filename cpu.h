/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- CPU headerfile
- Finlay Miller B00675696
- 29 June 2018
*/

#ifndef _CPU_H_
#define _CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

// CPU structure
typedef struct CPU
{
	bool running;
	bool sleeping;
	unsigned int clock;
};

// instruction types
enum insttype
{
	ALU,
	BRANCHING,
	LDSTMOV,
	BIT
};

// instruction mnemonics
enum instmnem
{
	LD, ST, LDR, STR, MOVL, MOVLZ, MOVH,
	BL, BEQBZ, BNEBNZ, BCBHS, BNCBLO, BN,
	BGE, BLT, BAL, ADD, ADDC, SUB, SUBC,
	DADD, CMP, XOR, AND, BIT, BIC, BIS,
	MOV, SWAP, SRA, RRC, SWPB, SXT
};

// function declarations
void fetch(void);
void decode(void);
void execute(void);

#endif