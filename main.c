/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Emulator mainline
- Finlay Miller B00675696
- 19 July 2018
*/

#include "main.h"
#define DEBUG

// initialize global memory
union Memory memory;
unsigned short regfile[RorC][reg_count];
unsigned short MAR = 0;
unsigned short MBR = 0;
unsigned short IR = 0;
unsigned short TMP = 0;

bool waiting_for_signal;

int main(void)
{
	/*
	Purpose:	Create objects, start global program timer,
				initialize debugger and start cpu
	Input:      None
	Output:     Execution status
	*/

	// setup timer
	clock_t start = clock();

	// create structs & allocate memory
	Emulator *emulator = (Emulator *)calloc(1, sizeof(Emulator));
	Debugger *debugger = NULL;
	CPU *cpu = NULL;
	Cache *cache = NULL;
	Device *device[DEVCNT] = { NULL };
	Next_Event *n_e = NULL;
	emulator->debugger = (Debugger *)calloc(1, sizeof(Debugger));
	emulator->cpu      = (CPU *)calloc(1, sizeof(CPU));
	for (int i = 0; i < DEVCNT; i++) 
		emulator->device[i] = (Device *)calloc(1, sizeof(Device));
	emulator->n_e = (Next_Event *)calloc(1, sizeof(Next_Event));
	emulator->cache = (Cache *)calloc(1, sizeof(Cache));

	// startup print
	printf("X Makina Emulator Version %s\n", VERSION);

	db_init(emulator);	// start debugger
	run(emulator);		// run CPU

	// end timer & print runtime
	clock_t end = clock();
	double total = (double)start - end / CLOCKS_PER_SEC;

	display_cache(emulator);
	display_regs(emulator);

	printf("\nProgram completed in %d ms.\n", (int)total);

	// endless loop
	while (waiting_for_signal);
	getchar(); getchar();

	exit(EXIT_SUCCESS);
}
