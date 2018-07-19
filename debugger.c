/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Debugger mainline
- Finlay Miller B00675696
- 19 July 2018
*/

#include "debugger.h"
#define DEBUG

int db_init(Emulator *emulator)
{
	/*
	Purpose:	Gets file information from user and calls the S record loader
	Input:      None, but gets up to 3 filenames from the user
	Output:     Returns the number of lines written to memory
	*/

	char df_conf[2];
	int i = 0;

	// init debugger, cpu, and emulator struct elements
	init_structs(emulator);

	// get input filename
	printf("\nInput S record filename\n> ");
	scanf("%s", emulator->ifile);

	// if the user wants to use a device file, get device filename
	printf("Device file? [y|n]\n> ");
	scanf("%s", df_conf);
	if (strcmp(df_conf, "y") == 0)
	{
		printf("Input device filename\n> ");
		scanf("%s", emulator->dfile);
	}

	// get device output filename, if device file is present
	if (emulator->dfile != NULL)
	{
		printf("Device output file\n> ");
		scanf("%s", emulator->ofile);
	}

#ifdef DEBUG
	strcpy(emulator->ifile, "PolledDevices.xme");
	strcpy(emulator->ofile, "out.txt");
	strcpy(emulator->dfile, "dev_in_test.txt");
#endif // DEBUG

	// load data into memory
	loader(emulator);

	return 0;
}

void debug(Emulator *emulator)
{
	/*
	Purpose:	Prompts user for control input
	Input:      None
	Output:     None
	*/

	// data entered by user to specify an action
	int response;
	// function to call
	int(*debug_functions[])(Emulator *emulator) =
	{
		advance, modify_regs, display_regs,
		display_devs, set_breakpoints, db_init, 
		clear_flags, quit
	};

	// get user input
	printf("\nDebugging mode active...\nEnter a command");
	help();
	scanf("%d", &response);

	// call specified function
	debug_functions[response](emulator);

	// drop priority to allow for interrupts
	set_priority(emulator, 0);

	return;
}

int advance(Emulator *emulator)
{
	/*
	Purpose:	Continue to execute program
	Input:      None
	Output:     None
	*/
	return 0;
}

int modify_regs(Emulator *emulator)
{
	/*
	Purpose:	Modifies user-specified registers
	Input:      User input specifying a register number & new data
	Output:     0. This is done to make it accessible by function pointer,
				as certain debugger functions are required to return an integer
	*/

	unsigned int regNum = 0;
	unsigned short regVal = 0;
	bool DEC_REG = false;
	char *yn = '\0';	// empty string

	while (regNum <= 9)
	{
		// prompt for register number to modify
		printf("Enter register number to modify. Options are:");
		printf("\n0  - GP   1  - GP   2  - GP\n3  - GP   4  - LR   5  - SP\n6  - PSW  7  - PC   8  - IR\n9  - TMP  10 - EXIT\n> ");
		scanf("%d", &regNum);
		// if register number if valid, prompt for data to load into it
		if (regNum <= 9)
		{
			printf("Enter the hex value to load into R%d. [0000 - FFFF]\n> ", regNum);
			scanf("%hx", &regVal);
		}

		// load into register
		if (regNum <= 7)
		{
			// normal registers
			regfile[0][regNum] = regVal;
		}
		else
		{
			// normally unaccessible registers
			switch (regNum)
			{
			case 8: IR = regVal;
				break;
			case 9: TMP = regVal;
				break;
			case 10: DEC_REG = true;
				break;
			default: db_error(emulator, BAD_MRG);		// handle unidentified registers
				DEC_REG = true; regNum = 0;
				break;
			}
		}
		
		if (!DEC_REG) printf("0x%hx loaded into R%d\n", regVal, regNum);
		// NOTE: This printf won't look right if the register being written to is
		// one of the normally-inaccessible ones. Produces output like
		// "0xABCD loaded into RTMP"
		//					   ^ not right
	}

	// confirm final values
	printf("\nFinal register values are:");
	display_regs(emulator);
	printf("\nMake changes? [y|n]\n> ");
	scanf("%s", yn);

	if (strcmp(yn, "y")) modify_regs(emulator);	// recursion!

	return 0;
}

int display_regs(Emulator *emulator)
{
	/*
	Purpose:	Displays register file, hidden registers, and constants (for completions sake)
	Input:      None
	Output:     0. This is done to make it accessible by function pointer,
				as certain debugger functions are required to return an integer
	*/

	// make some fresh local pointers
	CPU *cpu = emulator->cpu;

	printf("\n");
	printf("\t\t\tSYSCLK = %d", cpu->SYSCLK);
	printf("\n--------------------- USER ACCESSIBLE ---------------------");
	for (int i = 0; i < reg_count; i++)
	{
		printf("\nR%d\t\t"PRINTF_BINARY_PATTERN_INT16, i, PRINTF_BYTE_TO_BINARY_INT16(regfile[0][i]));
		printf("->0x%04x | %d", regfile[0][i], regfile[1][i]);
	}
	printf("\n-------------------- USER INACCESSIBLE --------------------");
	printf("\nMAR\t\t"PRINTF_BINARY_PATTERN_INT16"->0x%04x", PRINTF_BYTE_TO_BINARY_INT16(MAR), MAR);
	printf("\nMBR\t\t"PRINTF_BINARY_PATTERN_INT16"->0x%04x", PRINTF_BYTE_TO_BINARY_INT16(MBR), MBR);
	printf("\nIR\t\t" PRINTF_BINARY_PATTERN_INT16"->0x%04x", PRINTF_BYTE_TO_BINARY_INT16(IR), IR);
	printf("\nTMP\t\t"PRINTF_BINARY_PATTERN_INT16"->0x%04x", PRINTF_BYTE_TO_BINARY_INT16(TMP), TMP);
	printf("\n");

	return 0;
}

int display_devs(Emulator *emulator)
{
	/*
	Purpose:	Displays device memory
	Output:     0. This is done to make it accessible by function pointer,
				as certain debugger functions are required to return an integer
	*/

	printf("---------------------- DEVICE MEMORY ----------------------");
	for (int j = 0; j < DEVCNT; j++)
	{
		printf("\nDevice #%d\t"PRINTF_BINARY_PATTERN_INT16, j, PRINTF_BYTE_TO_BINARY_INT16(memory.word[j]));
		printf("->\'%c\'", memory.byte[j * 2 + 1]);
	}

	return 0;
}

int set_breakpoints(Emulator *emulator)
{
	/*
	Purpose:	Set emulator breakpoints based on memory location
	Input:      None
	Output:     0. This is done to make it accessible by function pointer,
				as certain debugger functions are required to return an integer
	*/

	// create some fresh pointers
	Debugger *deb = emulator->debugger;

	char view_conf[2];	// only really matters if 'y' is entered
	int *temp_bool = 0;

	printf("\n\nSettings breakpoints:\n Enter 0 to break on memory address or 1 to break on clock time\n> ");
	scanf("%d", temp_bool);

	if (temp_bool) deb->bp_flag = true;	// There must be a better way of doing this...

	while (deb->bp >= 0)
	{
		if (deb->bp_flag)
		{
			// time-based breakpoint
			printf("\nTime-based breakpoint selected.");
		}	// location-based breakpoint
		else printf("\nLocation-based breakpoint selected.");

		printf(" Enter the first marker to break at or -1 to exit\n> ");
		scanf("%hu", (unsigned short)deb->bp);
		if (deb->bp >= 0) deb->bp_addresses[deb->bp_count++] = deb->bp;
	}

	// see if user wants to print breakpoint list
	printf("\n%d breakpoints set. View breakpoints? [y|n]\n> ", deb->bp_count);
	scanf("%s", view_conf);

	// print breakpoint list if requested
	if (strcmp(view_conf, "y") == 0)
		for (int i = 0; i < deb->bp_count; i++)
			printf("\nBP#%d @ %d", i, deb->bp_addresses[i]);
	
	return 0;
}

int wipe_mem(void)
{
	/*
	Purpose:	Clear emulator memory. Registers included
	Input:      None, but accesses memory and registers
	Output:     0. This is done to make it accessible by function pointer,
	as certain debugger functions are required to return an integer
	*/

	char wipe_conf[2];	// only really matters if 'y' is entered

	printf("\nWiping memory (Registers included).\nAre you sure? [y|n]\n> ");
	scanf("%s", wipe_conf);

	if (strcmp(wipe_conf, "y"))
	{
		printf("\nWiping registers...");
		
		for (int i = 0; i < reg_count; i++) regfile[0][i] = 0;
		MAR = 0; MBR = 0; IR = 0; TMP = 0;

		printf("\nDone wiping registers. Wiping memory...\nThis may take some time...");

		for (int i = 0; i < MAX_BYTE_SIZE; i++) memory.byte[i] = 0;
	}

	printf("\nFinished wiping memory");

	return 0;
}

int quit(Emulator *emulator)
{
	/*
	Purpose:	Exits emulator early
	Input:      None
	Output:     None
	*/

	printf("\nProgram terminated at line 0x%x", PC);

	emulator->debugger->quit_flag = true;

	exit(EXIT_SUCCESS);

	// this is only here to make the function accessible by function pointer
	return 0;
}

int help(void)
{
	/*
	Purpose:	Prints debugger function data to screen
	Input:      None
	Output:     None
	*/

	printf(
		"\n* 0 - RUN  - [Run program until breakpoint or program end]"\
		"\n* 1 - MREG - [Modify register]"\
		"\n* 2 - DREG - [View registers]"\
		"\n* 3 - DBG  - [Toggle debugging mode (verbose screen output)]"\
		"\n* 4 - BKPT - [Add a breakpoint]"\
		"\n* 5 - INIT - [Reset all memory and registers]"\
		"\n* 6 - CLRF - [Clears all flags]"\
		"\n* 7 - QUIT - [Exit the program]\n> ");

	return 0;
}

void db_error(Emulator *emulator, enum DB_ERRORS dberr)
{
	/*
	Purpose:	Prints error dialogs
	Input:      Current error number. See debugger.h for details
	Output:     None
	*/

	emulator->debugger->error_flag = true;

	// strings to print. Linker throws errors if this is in the headerfile
	char *db_dialog[] =
	{
		(char*) "\nERROR : Failed to identify instruction in execute function",
		(char*) "\nERROR : Failed to identify register specified in modify register function",
		(char*) "\nERROR : Failed to decode instruction type",
		(char*) "\nERROR : Got to decode LDST instruction but type was incorrect",
		(char*) "\nERROR : Got to decode LDRSTR instruction but category was incorrect",
		(char*) "\nERROR : Got to decode BRANCH instruction but category was incorrect",
		(char*) "\nERROR : Got to decode BRANCH instruction but type was incorrect",
		(char*) "\nERROR : Got to decode ALU instruction but category was incorrect",
		(char*) "\nERROR : Got to decode ALU instruction but type was incorrect",
		(char*) "\nERROR : Got to decode REG instruction but type was incorrect",
		(char*) "\nERROR : Unable to open S record file",
		(char*) "\nERROR : Unable to open device file",
		(char*) "\nERROR : Got to execute LDST instruction but type was incorrect",
		(char*) "\nERROR : Got to execute LDRSTR instruction but category was incorrect",
		(char*) "\nERROR : Attempted to read past end of device input file",
		(char*) "\nERROR : Attempted to set PSW priority to a level != [0-7]"
	};

	// print error dialog
	printf("%s\nIR = "PRINTF_BINARY_PATTERN_INT16, db_dialog[dberr], 
					  PRINTF_BYTE_TO_BINARY_INT16(IR));
	display_regs(emulator);
	display_devs(emulator);
	getchar(); getchar();	// two in case of leftover characters in stdin
	exit(EXIT_FAILURE);
}

int clear_flags(Emulator *emulator)
{
	/*
	Purpose:	Clear all emulator flags
	Input:      Emulator pointer
	Output:     None
	*/

	// create fresh pointers
	Debugger *debugger = emulator->debugger;
	CPU * cpu = emulator->cpu;

	// reset flags
	emulator->waiting_for_signal = false;
	debugger->debug_mode = false;
	debugger->bp_flag = false;
	debugger->bp_type = false;
	debugger->error_flag = false;
	debugger->quit_flag = false;
	debugger->device_flag = false;
	debugger->loading = false;
	cpu->running = false;
	cpu->sleeping = false;

	// reset breakpoint information
	for (int i = 0; i < MAX_BREAKPOINTS; i++)
		debugger->bp_addresses[i] = 0;
	debugger->bp = 0;
	debugger->bp_count = 0;

	return 0;
}

// carry & overflow tables for PSW bit settings functions
unsigned carry[2][2][2] = { 0, 0, 1, 0, 1, 0, 1, 1 };
unsigned overflow[2][2][2] = { 0, 1, 0, 0, 0, 0, 1, 0 };

void set_bits(mi_ALU inst, unsigned short result)
{
	/*
	Purpose:	Sets bits in PSW after instruction execution
	Input:      Instruction & operands
	Result of operation
	Output:     None, but accesses the register file
	*/

	unsigned int msb, msd, mss, msr;

	// position of MSB depends on length of operation | 0 = word, 1 = byte
	(inst.wb == 0) ? (msb = 15) : (msb = 7);
					 // word	  // byte
	// get most significant bits of the source, destination, and result words
	mss = CHECK_BIT(regfile[inst.rc][inst.src], msb) >> msb;
	msd = CHECK_BIT(regfile[0][inst.dst], msb) >> msb;
	msr = CHECK_BIT(result, msb) >> msb;

#ifdef DEBUG
	printf("\nSetting bits:\nMSS = %d s = "PRINTF_BINARY_PATTERN_INT16"\tcarry %d", mss, PRINTF_BYTE_TO_BINARY_INT16(regfile[inst.rc][inst.src]), carry[mss][msd][msr]);
	printf("\nMSD = %d d = "PRINTF_BINARY_PATTERN_INT16, msd, PRINTF_BYTE_TO_BINARY_INT16(regfile[0][inst.dst]));
	printf("\nMSR = %d r = "PRINTF_BINARY_PATTERN_INT16"\toverflow %d", msr, PRINTF_BYTE_TO_BINARY_INT16(result), overflow[mss][msd][msr]);
#endif

	// carry
	SET_BIT(PSW, C, carry[mss][msd][msr]);
#ifdef DEBUG
	printf("\nCarry bit %s", (carry[mss][msd][msr]) ? "set" : "cleared");
#endif

	// zero
	if ((result == 0) && (inst.wb == 0))
	{							  // 0 = word
		// set Zero bit for word instruction
		SET_BIT(PSW, Z, 1);
#ifdef DEBUG
		printf("\tZero bit set");
#endif
	}
	// need to clear upper byte for byte instructions in case of something
	// like 0x0001 + 0x00FF == 0x0100
	else if ((result & LO_BYTE) == 0)
	{
		// set Zero bit for byte instruction
		SET_BIT(PSW, Z, 1);
#ifdef DEBUG
		printf("\tZero bit set");
#endif
	}
	else
	{
		SET_BIT(PSW, Z, 0);
#ifdef DEBUG
		printf("\tZero bit cleared");
#endif
	}

	// negative
	SET_BIT(PSW, N, msr);
#ifdef DEBUG
	if (msr) printf("\nNegative bit set");
	else printf("\nNegative bit cleared");
#endif

	// overflow
	SET_BIT(PSW, C, overflow[mss][msd][msr]);

#ifdef DEBUG
	if (overflow[mss][msd][msr]) printf("\tOverflow bit set");
	else printf("\tOverflow bit cleared");
#endif // DEBUG



	return;
}