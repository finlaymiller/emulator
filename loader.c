/*
- S-record Loader for the X-Makina Project
- ECED3403 Assignment 2
- Loader mainline. Makes use of code and ideas by Dr. Hughes
- Finlay Miller B00675696
- 29 June 2018
*/

#include "loader.h"

#define DEBUGGING

void loader(char ifile[], char dfile[])
{
	/*
	Purpose:    Reads and loads records from input files into memory
	Input:      Input files, both S records and device settings
	Output:     
	Flow:		- Read file line by line
				- For each line:	parse into a record object
				-					load into memory
	*/

	// SET LOADING FLAG

	FILE *f = fopen(ifile, "r");
	char line[S_REC_SIZE];

	// handle S records
	// read file line by line
	while (fgets(line, sizeof(line), f) != NULL)
	{
		// remove trailing newline
		strtok(line, '\n');
		// parse s record into s record struct
		S_Record rec = read_record(line);
		// load s record into memory
		load_record(rec);
	}

	// handle device files
	if (strlen(dfile) > 0)
	{
		printf("\nDevice support not yet added.");
	}

	fclose(f);
	// CLEAR LOADING FLAG
}

S_Record read_record(char input[])
{
	/*
	Purpose:    Parses string into record components
	Input:      String to parse
	Output:     S record object with string data in fields
	*/

    S_Record rec;

	// access variables
	unsigned int i;
	unsigned int pos;
	// record fields
	unsigned int len;
	unsigned int addr_H, addr_L;
	signed char checksum;
	unsigned int data;
	unsigned int byte;

	// check length & leading character for validity
    if (strlen(input) < 8)
		srec_error(TOO_SHORT, input);
	if (input[0] != 'S')
		srec_error(MISSING_S, input);

	// get S record type. '0' = 48, the ASCII character offset between
	// a character type number the equivalent integer type number
    rec.type = (int) input[1] -'0';

	// get record length and address
	sscanf(&input[2], "%2x%2x%2x", &len, &addr_H, &addr_L);
	rec.count = h2d(len);
	rec.address = h2d(addr_H << 8 | addr_L);
	checksum = len + addr_H + addr_L;

	len -= 3;	// ignore checksum
	pos = 8;	// skip to first data position

	// get record data
	for (i = 0; i < len; i++)
	{
		sscanf(&input[pos], "%2x", &byte);
		// add byte to memory
		checksum += byte & 0xFF;

		data += byte;
		pos += 2;
	}

	strncpy(rec.data, data, sizeof(data));

	// verify checksum
	sscanf(&input[pos], "%2x", &byte);
	checksum += byte;
	if (checksum != -1)
		srec_error(BAD_CHECKSUM, input);

	return rec;
}

void load_record(S_Record rec)
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

	unsigned int address = rec.address;

	if (rec.type == 1)
	{
		char byteH[3], byteL[3];
		int pos = 0;

		// stop at count/2 because we move in 2 character increments
		for (int i = 0; i < (rec.count / 2); i++)
		{
			// read bytes
			sscanf_s(rec.data + pos, "%2s%2s", &byteL, &byteH, 2);
			// add null terminators
			byteL[2] = '\0';
			byteH[2] = '\0';

			// load bytes
			memory.byte[address] = (unsigned char)h2d(byteL);
			memory.byte[address + 1] = (unsigned char)h2d(byteH);

			// move to next word
			pos += 4;
			address += 2;
		}
	}
	else if (rec.type == 9)
	{
		regfile[7] = (unsigned short)address;
	}
#ifdef DEBUGGING
	else if (rec.type == 0)
	{
		printf("\nLoading from file %s", (unsigned short)rec.data);
	}
#endif
}

void srec_error(enum SREC_ERRORS err, char srec[])
{
	/*
	Purpose:    S record error handler
	Input:      Error type  & current string
	Output:     Diagnostic string. Exits program
	*/

	printf("\n%s: %s", err_diag_s[err], srec);
	getchar();
	exit(EXIT_FAILURE);
}

int h2d(char *hex)
{
	/*
	Purpose:	Convert any hex number to a decimal integer 
	Input:      A hex string
	Output:     The equivalent integer
	*/

    int size = strlen(hex);
	int dec = 0, i = 0;

	// init as 16^0
	int base = 1;

	for (i = size; i > 0; i--)
	{
		if (hex[i] >= 'A' && hex[i] <= 'F')
		{
			// 55 is the difference between the ASCII character and its
			// corresponding integer value
			dec += (hex[i] - 55)*base;
			// move to next base
			base = base * 16;
		}
		else
		{
			// '0' = 48, the offset between the ASCII char of the number and the
			// integer of the number
			dec += (hex[i] - '0')*base;
			// move to next base
			base = base * 16;
		}
	}

	return dec;
}