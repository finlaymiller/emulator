/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Debugger headerfile
- Finlay Miller B00675696
- 19 July 2018
*/

#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

// libraries
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// other files
#include "main.h"
#include "loader.h"
#include "storage.h"
#include "devices.h"
#include "utilities.h"

#define MAX_BREAKPOINTS 255

typedef struct Debugger
{	
	// flags
	bool debug_mode;	// set when debug mode is selected by the user
	bool bp_flag;		// set when a breakpoint is active
	bool bp_type;		// set based on type of breakpoint
	bool error_flag;	// set when an error is detected 
	bool quit_flag;		// set when quit command is used
	bool loading;		// set while loading s records and data
	bool device_flag;	// set when device memory is accessed

	// breakpoint data
	unsigned short bp_addresses[MAX_BREAKPOINTS];
	unsigned short bp;
	signed   int   bp_count;
}Debugger;

// error message-related data
enum DB_ERRORS
{
	BAD_EXE,
	BAD_MRG,	// called from modify_regs
	BAD_DEC,
	DEC_LD,		// called from decode_MEMACCESS
	DEC_LDR,	// called from decode_MEMACCESS
	DEC_BRA_I,	// called from decode_BRA
	DEC_BRA_II,	// called from decode_BRA
	DEC_ALU_I,	// called from decode_ALU
	DEC_ALU_II,	// called from decode_ALU
	DEC_REG,	// called from decode_ALU
	BAD_SRC,	// called from loader
	BAD_DEV,	// called from loader
	EXE_LD,		// called from execute_LDST
	EXE_LDR,	// called from execute_LDRSTR
	EODF,		// called from dev_get_event
	BAD_LVL		// called from set_priority
};

// function declarations
void debug(Emulator *emulator);
int advance(Emulator *emulator);
int modify_regs(Emulator *emulator);
int display_regs(Emulator *emulator);
int display_devs(Emulator *emulator);
int db_init(Emulator *emulator);
int set_breakpoints(Emulator *emulator);
int quit(Emulator *emulator);
void db_error(Emulator *emulator, enum DB_ERRORS);
int clear_flags(Emulator *emulator);
int help(void);

#endif // !_DEBUGGER_H_
