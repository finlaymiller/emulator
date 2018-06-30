/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Emulator mainline
- Finlay Miller B00675696
- 29 June 2018
*/

#include "main.h"

int main(void)
{
	/*
	Purpose:    
	Input:      None
	Output:     Execution status
	*/

	int status = 0;

	// startup prints
	printf("X Makina Emulator Version %s", VERSION);
	printf("Finlay Miller B00675696");

	// setup timer
	clock_t start = clock();

	Emulator *emulator;
	CPU *cpu = NULL;
	Debugger *debugger = NULL;
	Storage *storage = NULL;

	status = run_debugger(emulator);

	// end timer & print runtime
	clock_t end = clock;
	double total = (double)start - end / CLOCKS_PER_SEC;
	printf("\nProgram completed in %lf seconds.", total);

	return status;
}