/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Headerfile for the various decode functions
- Finlay Miller B00675696
- 19 July 2018
*/

#ifndef _DECODE_FUNCTIONS_H_
#define _DECODE_FUNCTIONS_H_

// files
#include "cpu.h"
#include "bitmasks.h"

// instruction types
// MEMACCESS is instructions from LD to MOVH
// BRANCH is instructions from BL to BAL
// ALU is instructions from ADD to SWAP
// REG is instructions from SRA to SXT
// it is worth noting that REG type instructions use the same bitmasks
// as ALU, the R/C and S/C fields are left unused
enum instType { MEMACCESS, BRANCH, ALU, REG }instType;

// instructions
enum inst
{
	LD, ST, LDR, STR, MOVL, MOVLZ, MOVH,
	BL, BEQBZ, BNEBNZ, BCBHS, BNCBLO, BN,
	BGE, BLT, BAL, ADD, ADDC, SUB, SUBC,
	DADD, CMP, XOR, AND, BIT, BIC, BIS,
	MOV, SWAP, SRA, RRC, SWPB, SXT
} instruction;

// function declarations
enum inst decode_inst(Emulator *emulator);
enum inst decode_instruction(enum instType);
enum inst decode_MEMACCESS(Emulator *emulator);
enum inst decode_BRANCH(Emulator *emulator);
enum inst decode_ALU(Emulator *emulator);
void print_alias(enum inst);

#endif // !_DECODE_FUNCTIONS_H_
