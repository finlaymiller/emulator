<<<<<<< HEAD
/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Debugger mainline
- Finlay Miller B00675696
- 29 June 2018
*/

#include "debugger.h"
#include "main.h"

union Memory memory;
unsigned short regfile[8];
unsigned short MDR;
unsigned short MAR;

int run_debugger(Emulator *emulator)
{
	init();

	return 0;
}

int init()
{
	/*
	Purpose:	Initialize emulator settings
	Input:      None
	Output:     Execution status T|F
	*/
	
	char readconf;		// either y or n. Program only responds to y
	char inputfile[MAX_FILENAME] = NULL;
	char devicefile[MAX_FILENAME] = NULL;
	char ext[4];		// input file extension for type verification

	// Ask for S record file
	printf("\nLoad S Record? y/n\t");
	scanf_s("%c", readconf);
	if (strcmp(readconf, "y") == 0)
	{
		printf("\nFilename: ");
		scanf_s("%s", &inputfile);
		// check to make sure that the provided S record file is a .xme file
		strncpy(ext, inputfile + sizeof(inputfile) - 4, 4);
		if (strcmp(ext, ".xme") != 0)
			debug_error(BAD_FILETYPE);
	}

	// Ask for Device file
	printf("\nLoad Device file? y/n\t");
	scanf_s("%c", readconf);
	
	if (strcmp(readconf, "y") == 0)
	{
		printf("\nFilename: ");
		scanf_s("%s", &devicefile);
	}

	if (inputfile == NULL && devicefile == NULL)
	{
		debug_error(NO_FILES);
	}
	else
	{
		loader(inputfile, devicefile);
	}
	
	
}

void debug_error(enum DEBUG_ERRORS err)
{
	/*
	Purpose:    Debugger error handler
	Input:      Error type
	Output:     Diagnostic string. Exits program
	*/

	printf("\n%s: %s", err_diag_d[err]);
	getchar();
	exit(EXIT_FAILURE);
=======
/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Debugger mainline
- Finlay Miller B00675696
- 29 June 2018
*/

#include "debugger.h"
#include "main.h"

int run_debugger(Emulator *emulator)
{
	init();

	return 0;
}

int init()
{
	/*
	Purpose:	Initialize emulator settings
	Input:      None
	Output:     Execution status T|F
	*/
	
	char readconf;		// either y or n. Program only responds to y
	char inputfile[MAX_FILENAME] = NULL;
	char devicefile[MAX_FILENAME] = NULL;
	char ext[4];		// input file extension for type verification

	// Ask for S record file
	printf("\nLoad S Record? y/n\t");
	scanf_s("%c", readconf);
	if (strcmp(readconf, "y") == 0)
	{
		printf("\nFilename: ");
		scanf_s("%s", &inputfile);
		// check to make sure that the provided S record file is a .xme file
		strcpy(ext, inputfile + sizeof(inputfile) - 4, 4);
		if (strcmp(ext, ".xme") != 0)
			debug_error(BAD_FILETYPE);
	}

	// Ask for Device file
	printf("\nLoad Device file? y/n\t");
	scanf_s("%c", readconf);
	
	if (strcmp(readconf, "y") == 0)
	{
		printf("\nFilename: ");
		scanf_s("%s", &devicefile);
	}

	loader()
}

void debug_error(enum DEBUG_ERRORS err)
{
	/*
	Purpose:    Debugger error handler
	Input:      Error type
	Output:     Diagnostic string. Exits program
	*/

	printf("\n%s: %s", err_diag_d[err]);
	getchar();
	exit(EXIT_FAILURE);
>>>>>>> 278da0870bc211b7e3ccb4e0cda1249e0ec582e5
}