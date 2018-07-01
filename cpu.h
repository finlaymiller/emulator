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

//check if bit "pos" is set in "var"
#define CHECK_BIT(var, pos) ((var) & (1<<(pos)))

//set bit "pos" in "var"
#define SET_BIT(var, pos) ((var) |= (1<<(pos)))

//define PSW bit numbers
#define C 0
#define Z 1
#define N 2
#define SLP 3
#define V 4

// CPU structure
typedef struct CPU
{
	bool running;
	bool sleeping;
	unsigned int clock;
} CPU;

// instruction types
enum instType { LDST, MOV, BRANCHING, ALU, REGCON };

// instruction mnemonics
enum instMnem
{
	LD, ST, LDR, STR, MOVL, MOVLZ, MOVH,
	BL, BEQBZ, BNEBNZ, BCBHS, BNCBLO, BN,
	BGE, BLT, BAL, ADD, ADDC, SUB, SUBC,
	DADD, CMP, XOR, AND, BIT, BIC, BIS,
	MOV, SWAP, SRA, RRC, SWPB, SXT
};
enum instMnem iMnemonic;

// function declarations
void fetch(void);
void decode(void);
void execute(void);
enum instType decode_type(void);
enum instMnem decode_LDST(void);
enum instMnem decode_MOV(void);
enum instMnem decode_BRANCHING(void);
enum instMnem decode_ALU(void);
enum instMnem decode_REGCON(void);

#endif