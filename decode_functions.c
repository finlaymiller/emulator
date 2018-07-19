/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Main file for the various decode-related functions
- Finlay Miller B00675696
- 19 July 2018
*/

#include "decode_functions.h"
#define DEBUG

enum inst decode_inst(Emulator *emulator)
{
	/*
	Purpose:	Decodes instruction type based on the first 2 bits in the IR
	Input:      None
	Output:     None
	*/

	enum instType type = -1;	// type of instruction

	// pointer to each of the decode instruction functions
	enum inst(*instPtr[])(emulator) =
	{
		decode_MEMACCESS,
		decode_BRANCH, decode_ALU
	};

	if (CHECK_BIT(IR, 15))
	{	// IR bit 15 == 1
		type = MEMACCESS;
	}
	else	// IR bit 15 == 0
	{
		if (!CHECK_BIT(IR, 14))	// IR bit 15 == 0 && IR bit 14 == 0
		{
			// identify branching instructions
			type = BRANCH;
		}
		else type =  ALU;	// IR bit 15 == 0 && IR bit 14 == 1

	}

	// if instruction still is unidentified, break
	if (type == -1)	db_error(emulator, BAD_DEC);

#ifdef DEBUG
	printf("\nDecoded instruction of type #%d", type);
#endif // DEBUG


	// return instruction
	return instPtr[type](emulator);
}

enum inst decode_MEMACCESS(Emulator *emulator)
{
	/*
	Purpose:	Determines the exact name of instructions of type MEMACCESS
	Input:      None, but accesses the instruction register
	Output:     Name of instruction as an element of the enum type inst
				i.e. a number between 0 and 32
	Notes:		The mask union for MOV type instructions goes unused here.
				This is because for the purpose of decoding, MOV and LDST type
				instructions have the same format, since we only rely on the
				first 5 bits. The MOV mask union will be used during the
				execute phase.
	*/

	mi_LDST masked_LDST;
	mi_LDRSTR masked_LDRSTR;

	masked_LDST.inst = IR;
	masked_LDRSTR.inst = IR;

	if (CHECK_BIT(IR, 14))	// IR bit 14 == 1
	{
		// confirm category
		if (masked_LDRSTR.cat != 3)	// 3 = 0b11
		{	// type is incorrect
			db_error(emulator, DEC_LDR);
		}
		else
		{
			if (masked_LDRSTR.type == 1)
			{
				return STR;
			}
			else return LDR;
		}
	}	
	else				// IR bit 14 == 0
	{
		switch (masked_LDST.type)
		{
		case 0: return LD;		// 0 = 0b000
		case 1: return ST;		// 1 = 0b001
		case 2: return MOVL;	// 2 = 0b010
		case 3: return MOVLZ;	// 3 = 0b011
		case 4: return MOVH;	// 4 = 0b100
		default: db_error(emulator, DEC_LD);	
		}
	}

	// This return is included since the compiler can't tell that db_error() 
	// quits the program. The function shouldn't ever reach here.
	// Famous last words right?
	return -1;
}

enum inst decode_BRANCH(Emulator *emulator)
{
	/*
	Purpose:	Determines the exact name of instructions of type BRANCH
	Input:      None, but accesses the instruction register
	Output:     Name of instruction as an element of the enum type inst
				i.e. a number between 0 and 32
	*/

	mi_BRA maskedInst;
	maskedInst.inst = IR;

	// confirm type
	if (maskedInst.cat != 0)	// 0 = 0b00
	{	// bad category
		db_error(emulator, DEC_BRA_I);	
	}
	else if (maskedInst.other == 1)
	{
		// instructions BEQ/BZ through BAL
		switch (maskedInst.type)
		{
		case 0: return BEQBZ;	// 0 = 0b000
		case 1: return BNEBNZ;	// 1 = 0b001
		case 2: return BCBHS;	// 2 = 0b010
		case 3: return BNCBLO;	// 3 = 0b011
		case 4: return BN;		// 4 = 0b100
		case 5: return BGE;		// 5 = 0b101
		case 6: return BLT;		// 6 = 0b110
		case 7: return BAL;		// 7 = 0b111
		default: db_error(emulator, DEC_BRA_II);
		}
	}
	else if (maskedInst.other == 0)
	{
		return BL;
	}

	// This return is included since the compiler can't tell that db_error() 
	// quits the program. The function shouldn't ever reach here.
	// Famous last words right?
	return -1;
}

enum inst decode_ALU(Emulator *emulator)
{
	/*
	Purpose:	Determines the exact name of instructions of type REG
	Input:      None, but accesses the instruction register
	Output:     Name of instruction as an element of the enum type inst
				i.e. a number between 0 and 32
	Notes:		I know that the switch statement could be combined, and the
				'other' field be eliminated, but this method is more legible
	*/

	mi_ALU maskedInst;
	maskedInst.inst = IR;

	// confirm type
	if ((maskedInst.cat != 1))	// 1 = 0b01
	{
		db_error(emulator, DEC_ALU_I);
	}
	else if (maskedInst.other == 0)
	{
		// instructions ADD through SWAP
		switch (maskedInst.type)
		{
		case 16: return ADD;	// 16 = 0b10000
		case 17: return ADDC;	// 17 = 0b10001
		case 18: return SUB;	// 18 = 0b10010
		case 19: return SUBC;	// 19 = 0b10011
		case 20: return DADD;	// 20 = 0b10100
		case 21: return CMP;	// 21 = 0b10101
		case 22: return XOR;	// 22 = 0b10110
		case 23: return AND;	// 23 = 0b10111
		case 24: return BIT;	// 24 = 0b11000
		case 25: return BIC;	// 25 = 0b11001
		case 26: return BIS;	// 26 = 0b11010
		case 27: return MOV;	// 27 = 0b11011
		case 28: return SWAP;	// 28 = 0b11100
		default: db_error(emulator, DEC_ALU_II);
		}
	}
	else
	{
		// instructions SRA through SXT
		switch (maskedInst.type)
		{
		case 24: return SRA;	// 24 = 0b11000
		case 25: return RRC;	// 25 = 0b11001
		case 26: return SWPB;	// 26 = 0b11010
		case 27: return SXT;	// 27 = 0b11011
		default: db_error(emulator, DEC_REG);
		}
	}

	// This return is included since the compiler can't tell that db_error() 
	// quits the program. The function shouldn't ever reach here.
	// Famous last words right?
	return -1;
}

void print_alias(enum inst instruction)
{
	/*
	Purpose:	Prints instruction to screen
	Input:      Instruction number [0-32]
	Output:     None
	*/

	switch (instruction)
	{
	case 0: printf("\tLD");
		break;
	case 1: printf("\tST");
		break;
	case 2: printf("\tLDR");
		break;
	case 3: printf("\tSTR");
		break;
	case 4: printf("\tMOVL");
		break;
	case 5: printf("\tMOVLZ");
		break;
	case 6: printf("\tMOVH");
		break;
	case 7: printf("\tBL");
		break;
	case 8: printf("\tBEQ/BZ");
		break;
	case 9: printf("\tBNE/BNZ");
		break;
	case 10: printf("\tBC/BHS");
		break;
	case 11: printf("\tBNC/BLO");
		break;
	case 12: printf("\tBN");
		break;
	case 13: printf("\tBGE");
		break;
	case 14: printf("\tBLT");
		break;
	case 15: printf("\tBAL");
		break;
	case 16: printf("\tADD");
		break;
	case 17: printf("\tADDC");
		break;
	case 18: printf("\tSUB");
		break;
	case 19: printf("\tSUBC");
		break;
	case 20: printf("\tDADD");
		break;
	case 21: printf("\tCMP");
		break;
	case 22: printf("\tXOR");
		break;
	case 23: printf("\tAND");
		break;
	case 24: printf("\tBIT");
		break;
	case 25: printf("\tBIC");
		break;
	case 26: printf("\tBIS");
		break;
	case 27: printf("\tMOV");
		break;
	case 28: printf("\tSWAP");
		break;
	case 29: printf("\tSRA");
		break;
	case 30: printf("\tRRC");
		break;
	case 31: printf("\tSWPB");
		break;
	case 32: printf("\tSXT");
		break;
	default: printf("\tERROR");
		break;
	}

	return;
}