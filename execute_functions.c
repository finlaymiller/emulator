/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Execute function mainline
- Finlay Miller B00675696
- 19 July 2018
*/

#include "execute_functions.h"
#define DEBUG

void execute_LDST(Emulator *emulator)
{
	/*
	Purpose:	Executes direct loading and storing type instructions
	Input:      None, but accesses instruction register
	Output:     None
	*/

	mi_LDST maskedInst;
	maskedInst.inst = IR;	// instruction register
	unsigned short EA;		// effective address

#ifdef DEBUG
	printf("\n\nExecuting LDST type instruction");
	printf("\nDST %d SRC %d WB %d INC %d DEC %d PRPO %d TYPE %d CAT %d",
			maskedInst.dst, maskedInst.src, maskedInst.wb, maskedInst.inc,
			maskedInst.dec, maskedInst.prpo, maskedInst.type, maskedInst.cat);
#endif

	// check for unexpected bit values
	if ((maskedInst.cat != 2) || (maskedInst.inc && maskedInst.dec))
	{					// 2 = 0b10
		db_error(emulator, EXE_LD);
	}
	else if (maskedInst.type == LD)	/**************** LD ****************/
	{
		EA = maskedInst.src;

		// handle pre-increment or pre-decrement
		if (maskedInst.prpo)
		{	// handle increment or decrement
			if (maskedInst.inc)
			{	// handle byte [1] or word [0]
				(maskedInst.wb) ? (regfile[0][EA] += 1) : (regfile[0][EA] += 2);
			}
			else if (maskedInst.dec)
			{	// handle byte [1] or word [0]
				(maskedInst.wb) ? (regfile[0][EA] -= 1) : (regfile[0][EA] -= 2);
			}
		}

		// send source data to destination
#ifdef DEBUG
		printf("\nLoading from R%d mem[0x%x] to R%d (0x%x)", EA, regfile[0][EA], DST, regfile[0][DST]);
#endif
		MAR = regfile[0][EA];
		MBR = regfile[0][maskedInst.dst];
		bus(emulator, maskedInst.wb, READ);

		// preserve upper byte of register if instruction specifies byte operation
		regfile[0][maskedInst.dst] &= HI_BYTE;
		(maskedInst.wb == 0) ? (regfile[0][maskedInst.dst] = MBR) : (regfile[0][maskedInst.dst] |= MBR);
		// 0 = word

		// handle post-increment or post-decrement
		if (maskedInst.prpo == 0)
		{	// handle increment or decrement
			if (maskedInst.inc)
			{	// handle byte [1] or word [0]
				(maskedInst.wb) ? (regfile[0][EA] += 1) : (regfile[0][EA] += 2);
			}
			else if (maskedInst.dec)
			{	// handle byte [1] or word [0]
				(maskedInst.wb) ? (regfile[0][EA] -= 1) : (regfile[0][EA] -= 2);
			}
		}
	}
	else if (maskedInst.type == ST)	/************** ST **************/
	{
		EA = maskedInst.dst;	// Effective Address

		// handle pre-increment or -decrement
		if (maskedInst.prpo)
		{	// handle increment or decrement
			if (maskedInst.inc)
			{	// handle byte [1] or word [0]
				(maskedInst.wb) ? (regfile[0][EA] += 1) : (regfile[0][EA] += 2);
			}
			else if (maskedInst.dec)
			{	// handle byte [1] or word [0]
				(maskedInst.wb) ? (regfile[0][EA] -= 1) : (regfile[0][EA] -= 2);
			}
		}
		
		// send source data to destination
#ifdef DEBUG
		printf("\nStoring R%d (0x%x) in location 0x%x mem[0x%x]", 
			SRC, regfile[0][SRC], EA, regfile[0][EA]);
#endif
		MAR = regfile[0][EA];
		MBR = regfile[0][maskedInst.src];
		bus(emulator, maskedInst.wb, WRITE);

		// handle post-increment or post-decrement
		if (maskedInst.prpo == 0)
		{	// handle increment or decrement
			if (maskedInst.inc)
			{	// handle byte [1] or word [0]
				(maskedInst.wb) ? (regfile[0][EA] += 1) : (regfile[0][EA] += 2);
			}
			else if (maskedInst.dec)
			{	// handle byte [1] or word [0]
				(maskedInst.wb) ? (regfile[0][EA] -= 1) : (regfile[0][EA] -= 2);
			}
		}
	}
	else printf("\nUnhelpful error message.");

	return;
}

void execute_LDRSTR(Emulator *emulator)
{
	/*
	Purpose:	Executes load and store relative type instructions
	Input:      None, but accesses the IR and TMP registers
	Output:     None
	*/

	mi_LDRSTR maskedInst;
	maskedInst.inst = IR;
	unsigned short EA;

#ifdef DEBUG
	printf("\nExecuting LDRSTR type instruction");
	printf("\nDST %d SRC %d OFF %d TYPE %d CAT %d",
		maskedInst.dst, maskedInst.src, maskedInst.off, 
		maskedInst.type, maskedInst.cat);
#endif

	// check to confirm instruction category
	if (maskedInst.cat != 3)
	{				   // 3 = 0b11
		db_error(emulator, EXE_LDR);
	}
	else
	{
		TMP = maskedInst.off;	// send offset to TMP register
#ifdef DEBUG
		printf("\nTMP = "PRINTF_BINARY_PATTERN_INT16, PRINTF_BYTE_TO_BINARY_INT16(TMP));
#endif
		// sign extension if 5th bit is set
		if (CHECK_BIT(TMP, 5))
		{
			TMP |= 0xFFE0;
#ifdef DEBUG
			printf("\nTMP = "PRINTF_BINARY_PATTERN_INT16, PRINTF_BYTE_TO_BINARY_INT16(TMP));
#endif
		}

		if (maskedInst.type) /***************** STR ****************/
		{
			// calculate effective address
			EA = regfile[0][maskedInst.dst] + TMP;
			// get registers required by bus function ready
			MAR = EA;
			MBR = regfile[0][maskedInst.src];
			bus(emulator, maskedInst.wb, WRITE);
#ifdef DEBUG
			printf("\nStored to mem[EA] = "PRINTF_BINARY_PATTERN_INT16, PRINTF_BYTE_TO_BINARY_INT16(memory.word[EA]));
#endif // DEBUG
		}
		else /*************************** LDR ***************************/
		{
			// calculate effective address
			EA = regfile[0][maskedInst.src] + TMP;
#ifdef DEBUG
			printf("\nLoading from mem[EA] = "PRINTF_BINARY_PATTERN_INT16, PRINTF_BYTE_TO_BINARY_INT16(memory.word[EA]));
#endif // DEBUG
			// get registers required by bus function ready
			MAR = EA;
			MBR = regfile[0][maskedInst.dst];
			bus(emulator, maskedInst.wb, READ);

			// preserve upper byte of register if instruction specifies byte operation
			if (maskedInst.wb == 1) regfile[0][maskedInst.dst] &= 0xFF00;
			(maskedInst.wb == 0) ? (regfile[0][maskedInst.dst] = MBR) : (regfile[0][maskedInst.dst] |= MBR);
		}				   // 0 = word
	}

	return;
}

void execute_MOV(Emulator *emulator)
{
	/*
	Purpose:	Executes MOV type instructions
	Input:      None, but accesses register file
	Output:     None, but accesses register file
	*/

	mi_MOV maskedInst;
	maskedInst.inst = IR;
	unsigned short temp;

#ifdef DEBUG
	printf("\n\nExecuting MOV type instruction");
	printf("\nDST %d BYTE %d TYPE %d CAT %d",
			maskedInst.dst, maskedInst.byte,
			maskedInst.type, maskedInst.cat);
#endif

	// confirm type
	if (maskedInst.cat != 2)
	{					//2 = 0b10
		printf("\nError: Got to MOV instruction but category was incorrect");
		getchar();
		return;
	}
	else if (maskedInst.type == 2)	/************** MOVL **************/
	{						//  2 = 0b010
		// clear low byte
		regfile[0][maskedInst.dst] &= HI_BYTE;
		// add user-specified byte to register LSB
		regfile[0][maskedInst.dst] |= maskedInst.byte;
	}
	else if (maskedInst.type == 3)	/************** MOVLZ *************/
	{						 // 3 = 0b011
		// clear register
		regfile[0][maskedInst.dst] &= 0x0000;
		// add user-specified byte to register LSB
		regfile[0][maskedInst.dst] |= maskedInst.byte;
	}
	else if (maskedInst.type == 4)	/************** MOVH **************/
	{						 // 4 = 0b100
		// clear high byte of register
		regfile[0][maskedInst.dst] &= LO_BYTE;
		// add user-specified byte to register MSB
		temp = maskedInst.byte;
#ifdef DEBUG
		printf("\ntemp: "PRINTF_BINARY_PATTERN_INT16, PRINTF_BYTE_TO_BINARY_INT16(temp));
#endif
		temp <<= 8;
#ifdef DEBUG
		printf("\t"PRINTF_BINARY_PATTERN_INT16, PRINTF_BYTE_TO_BINARY_INT16(temp));
#endif
		regfile[0][maskedInst.dst] |= temp;
	}
	return;
}

void execute_BRA(Emulator *emulator)
{
	/*
	Purpose:	Executes offset type instructions
	Input:      None, but accesses register file
	Output:     None, but accesses register file
	*/

	mi_BRA maskedInst;
	maskedInst.inst = IR;

#ifdef DEBUG
	printf("\n\nExecuting BRA type instruction");
	printf("\nOFF 0x%x TYPE %d OTHER %d CAT %d",
		maskedInst.off, maskedInst.type,
		maskedInst.other, maskedInst.cat);
#endif

	// confirm type
	if (maskedInst.cat != 0)
	{				   // 0 = 0b00
		printf("\nError: Got to BRA instruction but category was incorrect");
		getchar();
		return;
	}
	else if (maskedInst.other == 0)	/****************** BL ******************/
	{						  // 0 = 0b00
		// create offset
		TMP = maskedInst.type;	// get bits 12-10 (put into TMP bits 0-2)
		TMP <<= 10;				// shift into correct position
		TMP |= maskedInst.off;	// TMP now holds entire 13-bit offset
		TMP <<= 1;				// ensure value is even
		if (CHECK_BIT(TMP, 13)) TMP &= SEXT_BL;	// sign extend

		// store PC then update PC
		LR = PC;
		PC += TMP;
	}
	else if (maskedInst.other)
	{
		// create offset
		TMP = 0;		// clear TMP
		TMP |= maskedInst.off;	// TMP now holds entire 10-bit offset
		TMP <<= 1;				// ensure value is even
		if (CHECK_BIT(TMP, 10)) TMP |= SEXT_BR;	// sign extend

		switch (maskedInst.type)
		{ // 0 = 0b000 -> BEQ/BZ
		case 0: PC = (CHECK_BIT(PSW, Z) ? (PC + TMP) : PC);
			break;				
		  // 1 = 0b001 -> BNE/BNZ
		case 1: PC = (!CHECK_BIT(PSW, Z) ? (PC + TMP) : PC);
			break;				
		  // 2 = 0b010 -> BC/BHS
		case 2: PC = (CHECK_BIT(PSW, C) ? (PC + TMP) : PC);
			break;				
		  // 3 = 0b011 -> BNC/BLO
		case 3: PC = (!CHECK_BIT(PSW, C) ? (PC + TMP) : PC);
			break;				
		  // 4 = 0b100 -> BN
		case 4: PC = (CHECK_BIT(PSW, N) ? (PC + TMP) : (PC + 2));
			break;				
		  // 5 = 0b101 -> BGE
		case 5: PC = (!(CHECK_BIT(PSW, N) ^ CHECK_BIT(PSW, V)) ? 
								(PC + TMP) : PC);
			break;				
		  // 6 = 0b110 -> BLT
		case 6: PC = ((CHECK_BIT(PSW, N) ^ CHECK_BIT(PSW, V)) ?
								(PC + TMP) : PC);
			break;				
		  // 7 = 0b111 -> BAL
		case 7: PC += TMP;
			break;
		default: printf("\nError: Unidentified BRA instruction");
			getchar();
			return;
		}
	}

}

void execute_ALU(Emulator *emulator)
{
	/*
	Purpose:	Executes instructions involving the CPUs ALU or bitwise
				operations. This is possible because the instructions SRA
				through SXT have the same format as the other ALU instructions,
				we can just ignore the R/C and SRC fields.
	Input:      None
	Output:     None
	*/

	mi_ALU maskedInst;
	maskedInst.inst = IR;
	unsigned short result;

#ifdef DEBUG
	printf("\nDST %d SRC %d WB %d RC %d OTHER %d TYPE %d CAT %d",
		maskedInst.dst, maskedInst.src, maskedInst.wb, maskedInst.rc,
		maskedInst.other, maskedInst.type, maskedInst.cat);
#endif

	if (maskedInst.cat != 1)
	{					// 1 = 0b01
		printf("\nError: Got to ALU instruction but category was incorrect");
		getchar();
		return;
	}
	else
	{
		switch (maskedInst.type)
		{
		case ADD: execute_ADD(emulator, maskedInst);
			break;
		case ADDC: execute_ADD(emulator, maskedInst);
			break;
		case SUB: execute_ADD(emulator, maskedInst);
			break;
		case SUBC: execute_ADD(emulator, maskedInst);
			break;
		case DADD: execute_DADD(emulator, maskedInst);
			break;
		case CMP: execute_ADD(emulator, maskedInst);
			break;
		case XOR: execute_XOR(emulator, maskedInst);
			break;
		case AND: execute_AND(emulator, maskedInst);
			break;
		case BIT: (maskedInst.wb == 0) ? 
			(result = regfile[maskedInst.rc][maskedInst.dst] & 
				regfile[maskedInst.rc][maskedInst.src]) :
			(result = regfile[maskedInst.rc][maskedInst.dst] & 
			(regfile[maskedInst.rc][maskedInst.src] & LO_BYTE));
			set_bits(maskedInst, result);
			break;
		case BIC: (maskedInst.wb == 0) ? 
			(regfile[maskedInst.rc][maskedInst.dst] &= 
				~regfile[maskedInst.rc][maskedInst.src]) :
			(regfile[maskedInst.rc][maskedInst.dst] ^= 
				~(regfile[maskedInst.rc][maskedInst.src] & LO_BYTE));
			break;
		case BIS: (maskedInst.wb == 0) ? 
			(regfile[maskedInst.rc][maskedInst.dst] |= 
				regfile[maskedInst.rc][maskedInst.src]) :
			(regfile[maskedInst.rc][maskedInst.dst] |= 
			(regfile[maskedInst.rc][maskedInst.src] & LO_BYTE));
			break;
		case MOV: (maskedInst.wb == 0) ?
			(regfile[maskedInst.rc][maskedInst.dst] = 
				regfile[maskedInst.rc][maskedInst.src]) :
			(regfile[maskedInst.rc][maskedInst.dst] = 
			(regfile[maskedInst.rc][maskedInst.src] & LO_BYTE));
			break;
		case SWAP: TMP = regfile[0][maskedInst.dst];
			regfile[0][maskedInst.dst] = regfile[0][maskedInst.src];
			regfile[0][maskedInst.dst] = TMP;
			break;
		}
	}
}

void execute_REG(Emulator *emulator)
{
	/*
	Purpose:	Emulates the execution of one-operand instructions
	Input:      None, but accesses register file
	Output:     None, but accesses register file
	*/

	mi_ALU maskedInst;
	maskedInst.inst = IR;
	unsigned short temp_byte;
	unsigned int msb, temp_bit;

#ifdef DEBUG
	printf("\nDST %d SRC %d WB %d RC %d OTHER %d TYPE %d CAT %d",
		maskedInst.dst, maskedInst.src, maskedInst.wb, maskedInst.rc,
		maskedInst.other, maskedInst.type, maskedInst.cat);
#endif

	// set most significant bit based on word [0] or byte [1] operation;
	(maskedInst.wb == 0) ? (msb = 15) : (msb = 7);
				 
	switch (maskedInst.type)
	{
	case SRA: SET_BIT(PSW, C, CHECK_BIT(regfile[0][maskedInst.dst], 0));
		regfile[0][maskedInst.dst] >>= 1;
		SET_BIT(regfile[0][maskedInst.dst], msb, CHECK_BIT(regfile[0][maskedInst.dst], msb));
		break;
	case RRC: temp_bit = CHECK_BIT(PSW, C);	// save PSW.C bit
		SET_BIT(PSW, C, CHECK_BIT(regfile[0][maskedInst.dst], 0)); // DST.LSB -> PSW.C
		regfile[0][maskedInst.dst] >>= 1;	// shift right
		SET_BIT(regfile[0][maskedInst.dst], msb, temp_bit);// PSW.C -> DST.MSB
		break;
	case SWPB: temp_byte = regfile[0][maskedInst.dst] & LO_BYTE;// save low byte
		regfile[0][maskedInst.dst] >>= 8;	// shift high byte to low byte
		regfile[0][maskedInst.dst] |= (temp_byte << 8); // set high byte to low byte
		break;
	case SXT: msb = 7;		// check most significant bit of low byte
		// extend sign based on MSB
		if (CHECK_BIT(regfile[0][maskedInst.dst], msb)) regfile[0][maskedInst.dst] |= HI_BYTE;
		else regfile[0][maskedInst.dst] &= LO_BYTE;
		break;
	}
}

void execute_ADD(Emulator *emulator, mi_ALU inst)
{
	/*
	Purpose:	Emulates all instructions that use addition
	Input:      16 bit instruction
	Output:     None, but accesses the register file
	*/

	// use temporary variables to preserve source state
	unsigned short s = regfile[inst.rc][inst.src];	// source
	unsigned short d = regfile[0][inst.dst];		// destination
	unsigned short r;								// result

	// transform source to negative if required
	if ((inst.type == SUB) || (inst.type == SUBC) || (inst.type == CMP))
	{
		s = ~s;						// 1's complement
		if (inst.type != SUBC) s++;	// 2's complement
	}

	// handle word/byte operations
	if (inst.wb == 1)
	{	// byte
		// perform the addition -- Carry bit SHOULD be 0 if operation does not use it
		r = (d & LO_BYTE) + (s & LO_BYTE) + CHECK_BIT(PSW, C);
		// set necessary bits in PSW
		set_bits(inst, r);

		// ensure that high byte in destination register is preserved
		if (inst.type != CMP)
		{	// preserve high byte
			regfile[0][inst.dst] &= HI_BYTE;
			regfile[0][inst.dst] |= (r & LO_BYTE);
		}

	}
	else
	{	// word
		r = d + s + CHECK_BIT(PSW, C);

		// set necessary bits in PSW
		set_bits(inst, r);

		if (inst.type != CMP) regfile[0][inst.dst] = r;

	}

	return;
}

void execute_DADD(Emulator *emulator, mi_ALU inst)
{
	/*
	Purpose:	Decimal add using BCD
	Input:      DADD opcode and operands -> 16-bit word
	Output:     Sets or clears PSW.C bit depending on last carry value
	*/
	
	unsigned short carry = CHECK_BIT(PSW, C); // initial carry
	union bitmask_bcd dest, source;
	unsigned short result = 0;	// final 16-bit result

	// load data into dest and source bitmask unions
	dest.word = regfile[0][inst.dst];
	source.word = regfile[inst.rc][inst.src];

	// perform addition
	dest.dig.nib0   = bcd_add(dest.dig.nib0, source.dig.nib0, &carry);
	source.dig.nib1 = bcd_add(dest.dig.nib1, source.dig.nib1, &carry);
	
	// add upper byte if instruction is DADD.W
	if (inst.wb == 0)
	{			// 0 = word
		dest.dig.nib2   = bcd_add(dest.dig.nib2, source.dig.nib2, &carry);
		source.dig.nib3 = bcd_add(dest.dig.nib3, source.dig.nib3, &carry);
	}

	// set flags
	SET_BIT(PSW, C, carry);

	// send result to register
	regfile[0][inst.dst] = result;

	return;

}

unsigned short bcd_add(unsigned short nibble1, unsigned short nibble2, unsigned short *carry)
{
	/*
	Purpose:	Adds two 4-bit bcd numbers
	Input:      Two 4-bit numbers and a carry
	Output:     The result of the addition. The carry is written as well.
	*/

	unsigned char result;

	result = nibble1 + nibble2 + *carry;

	if (result >= 10)
	{
		result -= 10;
		*carry = 1;
	}
	else *carry = 0;

	return result;
}

void execute_XOR(Emulator *emulator, mi_ALU inst)
{
	/*
	Purpose:	Emulates the XOR instruction
	Input:      16 bit XOR instruction
	Output:     None, but accesses the register file
	*/

	unsigned short byte; // used for byte operation

	if (inst.wb == 0)
	{
		regfile[0][inst.dst] ^= regfile[inst.rc][inst.src];
	}
	else
	{
		byte = regfile[0][inst.dst] & LO_BYTE;	// isolate low byte
		byte ^= regfile[inst.rc][inst.src];		// XOR with source
		regfile[0][inst.dst] ^= byte;		// copy to register low byte
	}

	return;
}

void execute_AND(Emulator *emulator, mi_ALU inst)
{
	/*
	Purpose:	Emulates the AND instruction
	Input:      16 bit AND instruction
	Output:     None, but accesses the register file
	*/

	unsigned short byte; // used for byte operation

	if (inst.wb == 0)	// 0 = word
	{
		regfile[0][inst.dst] &= regfile[inst.rc][inst.src];
	}
	else
	{
		byte = regfile[0][inst.dst] & LO_BYTE;	// isolate low byte
		byte &= regfile[inst.rc][inst.src];		// AND with source
		regfile[0][inst.dst] &= byte;		// copy to register low byte
	}

	set_bits(inst, regfile[0][inst.dst]);

	return;
}

