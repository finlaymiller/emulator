/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Emulator mainline
- Finlay Miller B00675696
- 19 July 2018
*/

#include "loader.h"
//#define DEBUG

int loader(Emulator *emulator)
{
	/*
	Purpose:    Reads and loads records from input files into memory
	Input:      Input files, both S records and device settings
	Output:
	Flow:		- Read file line by line
				- For each line:	parse into a record object
				 					load into memory
	*/

	emulator->debugger->loading = true;
	FILE *sf;				// S record pointer
	char line[S_REC_SIZE];	// character buffer
	S_Record rec;			// S record struct

	sf = fopen(emulator->ifile, "r");
	if (sf == NULL) db_error(emulator, BAD_SRC);
	
	// handle S records
	// read file line by line
	while (fgets(line, sizeof(line), sf) != NULL)
	{
		strtok(line, "\n");					// remove trailing newline
		rec = read_record(emulator, line);	// parse s record into s record struct
		load_record(emulator, rec);			// load s record into memory
	}
	fclose(sf);	// close S record file

	// handle devices
	if (strcmp(emulator->dfile, "\0") != 0)
	{
#ifdef DEBUG
		printf("\n\nLOADING DEVICE SETTINGS");
#endif // DEBUG

		// write scanned device settigs to the emulator's array
		dev_init(emulator);
	}

	emulator->debugger->loading = false;

	return 0;
}

S_Record read_record(Emulator *emulator, char input[])
{
	/*
	Purpose:    Parses string into record components
	Input:      String to parse
	Output:     S record object with string data in fields
	*/

	S_Record rec;
	unsigned char buff[S_REC_SIZE] = { 0 };
	int len = 0;

	// check length & leading character for validity
	if (strlen(input) < 8)
		srec_error(emulator, TOO_SHORT, input);
	if (input[0] != 'S')
		srec_error(emulator, MISSING_S, input);

	// get S record type. '0' is the ASCII character offset between
	// a character type number the equivalent integer type number
	rec.type = (int)input[1] - '0';
	len = strlen(input) - 10;
	memset(rec.data, 0, sizeof(rec.data));

#ifdef DEBUG
	if (rec.type != 0)
	{
		printf("\n\nReading from record: %s", input);
	}
#endif

	// read record count
	memcpy(buff, &input[2], 2);
	buff[2] = '\0';
	rec.count = h2d(buff);
	memset(buff, 0, sizeof(buff));

	// read record address
	memcpy(buff, &input[4], 4);
	buff[4] = '\0';
	rec.address = h2d(buff);
	memset(buff, 0, sizeof(buff));

	// read record data
	memcpy(buff, &input[8], len);
	buff[len] = '\0';
	strcpy(rec.data, buff);
	memset(buff, 0, sizeof(buff));

	// VERIFY CHECKSUM HERE

#ifdef DEBUG
	if (rec.type != 0)
	{
		printf("\nRECORD READ AS:\nTYPE\t%d\nCOUNT\t%d\nADDRESS\t0x%x\nDATA\t%s", rec.type, rec.count, rec.address, rec.data);
	}
	else printf("\nReading from file: %s", rec.data);
#endif

	return rec;
}

int load_record(Emulator *emulator, S_Record rec)
{
	/*
	Purpose:    loads records from input files into memory
	Input:      S record to load
	Output:
	Flow:		- Get high and low bytes
				- Switch order to Big Endian
				- Convert to decimal
				- Load to memory
	*/

	unsigned int address = rec.address;	// memory address to start from
	unsigned char byteL[3], byteH[3];	// used to convert from little- to
										// big-endian
	int pos = 0;						// stores position in rec data field

	if (rec.type == 1)
	{

#ifdef DEBUG
		if (rec.type != 0) printf("\n\nLoading data into memory\n\tCHAR\tHEX\tADDR");
#endif
		// stop at count/2 because we move in 2 character increments
		for (int i = 0; i < (rec.count / 2); i++)
		{
			// read bytes
			sscanf(rec.data + pos, "%2c%2c", byteL, byteH);

			// add null terminators
			byteL[2] = '\0';
			byteH[2] = '\0';

			// load bytes
			memory.byte[address] = (unsigned char)h2d(byteL);
			memory.byte[address + 1] = (unsigned char)h2d(byteH);

#ifdef DEBUG
			printf("\nBL\t%s\t0x%x\t0x%x\nBH\t%s\t0x%x\t0x%x", byteL, 
				h2d(byteL), address, byteH, h2d(byteH), address + 1);
#endif

			// move to next word
			pos += 4;
			address += 2;
		}

		// increment clock by the amount of lines pushed to memory
		emulator->cpu->END += (address - rec.address);
	}
	else if (rec.type == 9)
	{
		PC = (unsigned short)rec.address;
#ifdef DEBUG
		printf("\nPC set to 0x%x", PC);
#endif
	}

	// difference betweeen starting address and ending address is used to
	// calculate the total number of lines written to memory
	return 0;
}

void srec_error(Emulator *emulator, enum SREC_ERRORS err, char srec[])
{
	/*
	Purpose:    S record error handler
	Input:      Error type  & current string
	Output:     Diagnostic string. Exits program
	*/

	const char *rec_err_diag[] =
	{
		(char *) "\nInvalid S rec - missing 'S' character",
		(char *) "\nInvalid S rec - too short",
		(char *) "\nInvalid S rec - detected an S record not of type 0, 1, or 9",
		(char *) "\nInvalid S rec - bad record type",
		(char *) "\nInvalid S rec - error in checksum",
	};

	printf("\n%s: %s", rec_err_diag[err], srec);
	getchar();
	return;
}

int h2d(char *hex)
{
	/*
	Purpose:	Convert any hex number to a decimal integer
	Input:      A hex string
	Output:     The equivalent integer
	*/

	int dec = 0, tmp, i;
	int base = 1;
	int count = (int)strlen(hex);

	for (i = 0; i < count; i++) {

		if (hex[count - (i + 1)] >= 'A' && hex[count - (i + 1)] <= 'F') {
			tmp = hex[count - (i + 1)] - 55;
		}
		else {
			tmp = hex[count - (i + 1)] - '0';
		}
		dec += tmp * (int)pow(16, i);
	}

	return dec;
}