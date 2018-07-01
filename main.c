/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Emulator mainline
- Finlay Miller B00675696
- 29 June 2018
*/

#include "main.h"

// TODO
// MAKE MASKING FUNCTION
// USE MASKING FUNCTION TO IMPROVE DECODE MNEMONIC FUNCTIONS
// ADD DECODER FUNCTIONS
// ADD EXECUTE FUNCTIONS
// ADD DEVICE SUPPORT
// ADD INTERRUPT SUPPORT

int main(void)
{
	/*
	Purpose:    
	Input:      None
	Output:     Execution status
	*/

	int status = 0;
	char inputfile[60];

	// startup prints
	printf("X Makina Emulator Version %s", VERSION);
	printf("Finlay Miller B00675696");

	// setup timer
	clock_t start = clock();

	Emulator *emulator;
	CPU *cpu = NULL;
	Debugger *debugger = NULL;

	emulator->cpu = (CPU *) calloc(1, sizeof(CPU));
  	emulator->debugger  = (Debugger *) calloc(1, sizeof(Debugger));
  	cpu = emulator->cpu;
  	debugger = emulator->debugger;

	//status = run_debugger(emulator);
	printf("\nInput filename:\t");
	scanf("%s", inputfile);

	loader(inputfile, NULL);

	// end timer & print runtime
	clock_t end = clock();
	double total = (double)start - end / CLOCKS_PER_SEC;
	printf("\nProgram completed in %lf seconds.", total);

	return status;
}