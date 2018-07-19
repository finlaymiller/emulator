/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Headerfile for the various bitmasks required for decoding and executing
- Finlay Miller B00675696
- 19 July 2018
*/

#ifndef _BITMASKS_H_
#define _BITMASKS_H_

// check if bit "pos" is set in "var"
#define CHECK_BIT(var, pos) (((var) & (1 << pos))>>pos)
// set bit "pos" in "var". "val" = [0|1]
#define SET_BIT(var, pos, val) ((var) |= (val << pos))

// sign extension masks
#define SEXT_BL 0xE000		// 1110.0000.0000.0000 -> sign extend from bit 13
#define SEXT_BR 0xF800		// 1111.1000.0000.0000 -> sign extend from bit 10

// byte isolator
#define LO_BYTE 0x00FF
#define HI_BYTE 0xFF00

// device masks
#define IO__BIT 1
#define DBA_BIT 2
#define OVF_BIT 3

// LD & ST INSTRUCTIONS
struct bitmask_LDST
{
	unsigned short dst : 3;		// 000 - 111
	unsigned short src : 3;		// 000 - 111
	unsigned short wb : 2;		// 00 is byte, 01 is word
	unsigned short inc : 1;		// error if both inc
	unsigned short dec : 1;		// & dec are set
	unsigned short prpo : 1;	// 1 - PR | 0 - PO 
	unsigned short type : 3;	// 000 - LD | 001 - ST
	unsigned short cat : 2;		// error if not 10
};

typedef union
{
	unsigned short inst;
	struct bitmask_LDST;
} mi_LDST;

// LDR & STR INSTRUCTIONS
struct bitmask_LDRSTR
{
	unsigned short dst : 3;		// 000 - 111
	unsigned short src : 3;		// 000 - 111
	unsigned short wb : 1;		// 0 is byte, 1 is word
	unsigned short off : 6;
	unsigned short type : 1;	// 0 - LDR | 1 - STR
	unsigned short cat : 2;		// error if not 11
};

typedef union
{
	unsigned short inst;
	struct bitmask_LDRSTR;
}mi_LDRSTR;

// MOVL, MOVLZ, & MOVH INSTRUCTIONS
struct bitmask_MOV
{
	unsigned short dst : 3;		// 000 - 111
	unsigned short byte : 8;	// 0000.0000 - 1111.1111
	unsigned short type : 3;	// 100 - MOVH | 011 - MOVLZ | 010 - MOVL | 001 - ERROR
	unsigned short cat : 2;		// error if not 2
};

typedef union
{
	unsigned short inst;
	struct bitmask_MOV;
}mi_MOV;

// BL, BEQ/BZ, BNE/BNZ, ... , & BAL INSTRUCTIONS
struct bitmask_BRA
{
	unsigned short off : 10;	// 00.0000.0000 - 11.1111.1111
	unsigned short type : 3;	// 000 - 111
	unsigned short other : 1;	// 0 if BL, 1 otherwise
	unsigned short cat : 2;		// error if not 00
};

typedef union
{
	unsigned short inst;
	struct bitmask_BRA;
}mi_BRA;

// ADD, ADDC, SUB, ... , & SXT INSTRUCTIONS
struct bitmask_ALU
{
	unsigned short dst : 3;		// 000 - 111
	unsigned short src : 3;		// 000 - 111
	unsigned short wb : 1;		// 0 is byte, 1 is word
	unsigned short rc : 1;
	unsigned short other : 1;	// 0 if ADD-SWAP, 1 if SRA-SXT
	unsigned short type : 5;	// 0.0000 - 1.0000
	unsigned short cat : 2;		// error if not 01
};

typedef union
{
	unsigned short inst;
	struct bitmask_ALU;
}mi_ALU;

void set_bits(mi_ALU, unsigned short);

// DADD structures
typedef struct bcd_digits
{
	unsigned short nib0 : 4;
	unsigned short nib1 : 4;
	unsigned short nib2 : 4;
	unsigned short nib3 : 4;
}bcd_digits;

union bitmask_bcd
{
	unsigned short word;
	bcd_digits dig;
};

#endif // !_BITMASKS_H_

