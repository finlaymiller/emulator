/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- CPU mainline
- Finlay Miller B00675696
- 19 July 2018
*/

#include "cpu.h"
#define DEBUG

void run(Emulator *emulator)
{
	/*
	Purpose:	Runs fetch-decode-execute-check cycle
	Input:      Number of memory lines to run through
	Output:     None
	*/

	// create some fresh local pointers
	CPU *cpu = emulator->cpu;
	Debugger *deb = emulator->debugger;

	cpu->END = 200;
	printf("\nRunning until time %d", cpu->END);

	debug(emulator);

	bool waiting_for_signal = true;
	cpu->running = true;

	while (cpu->SYSCLK <= cpu->END)
	{
		fetch(emulator);
		decode(emulator);
		execute(emulator);
		maintenance(emulator);
	}

	cpu->running = false;
	getchar(); getchar();
	return;
}

void fetch(Emulator *emulator)
{
	/*
	Purpose:	Retrieve word from memory
	Input:      None, but reads from PC
	Output:     None, but PC is modified and bus is called
	*/

	MAR = PC;

#ifdef DEBUG
	printf("\n\nFETCHING FROM LOCATION 0x%x", MAR);
#endif // !DEBUG

	// read from PC & store in instruction register
	bus(emulator, WORD, READ);
	IR = MBR;

#ifdef DEBUG
	printf("\nmemory[PC/2] = 0x%x\tIR = "PRINTF_BINARY_PATTERN_INT16,memory.word[PC/2], PRINTF_BYTE_TO_BINARY_INT16(IR));
#endif // !DEBUG

	// increment PC, ensuring that address is even
	PC += 2;
	SET_BIT(PC, 0, 0);
	
	return;
}

void decode(Emulator *emulator)
{
	/*
	Purpose:	Identify opcode and operands contained in the IR
	Input:      None
	Output:     None
	*/
	instruction = decode_inst(emulator);

#ifdef DEBUG
	printf("\n\nDECODING...\nInstruction:");
	print_alias(instruction);
#endif

	return;
}

void execute(Emulator *emulator)
{
	/*
	Purpose:	Perform operation specified by opcode & operands
	Input:      None, but reads decoded instruction
	Output:     None
	*/

	switch (instruction) {
	case LD:
		execute_LDST(emulator);
		break;
	case ST:
		execute_LDST(emulator);
		break;
	case LDR:
		execute_LDRSTR(emulator);
		break;
	case STR:
		execute_LDRSTR(emulator);
		break;
	case MOVL:
		execute_MOV(emulator);
		break;
	case MOVH:
		execute_MOV(emulator);
		break;
	case MOVLZ:
		execute_MOV(emulator);
		break;
	case BL:
		execute_BRA(emulator);
		break;
	case BEQBZ:
		execute_BRA(emulator);
		break;
	case BNEBNZ:
		execute_BRA(emulator);
		break;
	case BCBHS:
		execute_BRA(emulator);
		break;
	case BNCBLO:
		execute_BRA(emulator);
		break;
	case BN:
		execute_BRA(emulator);
		break;
	case BGE:
		execute_BRA(emulator);
		break;
	case BLT:
		execute_BRA(emulator);
		break;
	case BAL:
		execute_BRA(emulator);
		break;
	case ADD:
		execute_ALU(emulator);
		break;
	case ADDC:
		execute_ALU(emulator);
		break;
	case SUB:
		execute_ALU(emulator);
		break;
	case SUBC:
		execute_ALU(emulator);
		break;
	case DADD:
		execute_ALU(emulator);
		break;
	case CMP:
		execute_ALU(emulator);
		break;
	case XOR:
		execute_ALU(emulator);
		break;
	case AND:
		execute_ALU(emulator);
		break;
	case BIT:
		execute_ALU(emulator);
		break;
	case BIC:
		execute_ALU(emulator);
		break;
	case BIS:
		execute_ALU(emulator);
		break;
	case MOV:
		execute_ALU(emulator);
		break;
	case SWAP:
		execute_ALU(emulator);
		break;
	case SRA:
		execute_REG(emulator);
		break;
	case RRC:
		execute_REG(emulator);
		break;
	case SWPB:
		execute_REG(emulator);
		break;
	case SXT:
		execute_REG(emulator);
		break;
	default:
		db_error(emulator, BAD_EXE);
		break;
	}
}

void maintenance(Emulator *emulator)
{
	/*
	Purpose:	Performs  the various per-cycle checks required by the CPU
	Input:      Emulator pointer
	Output:     None
	*/
	
	// create some fresh local pointers
	CPU *cpu = emulator->cpu;
	Debugger *deb = emulator->debugger;
	Next_Event *n_e = (Next_Event *)calloc(1, sizeof(Next_Event));

	// check breakpoints
	if (deb->bp_flag == true)
	{
		for (int i = 0; i < deb->bp_count; i++)
		{
			if (PC == deb->bp_addresses[i])
			{
				cpu->running = false;
				deb->debug_mode = true;

				printf("\n\nBreakpoint #%d hit", i + 1);
			}
		}
	}
	
	// check devices
	if (deb->device_flag == true)
	{
		dev_i_check(emulator);
		dev_o_check(emulator);
	}

	display_regs(emulator);
	if (deb->device_flag) display_devs(emulator);
	
	emulator->cpu->SYSCLK++;

	return;
}