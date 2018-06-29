/*
- S-record Loader for the X-Makina Project
- ECED3403 Assignment 2
- Loader mainline. Makes use of code and ideas by Dr. Hughes
- Finlay Miller B00675696
- 29 June 2018
*/

#include "loader.h"

#define DEBUG

struct S_Record read_record(char input[])
{
	/*
	Purpose:    Parses string into record components
	Input:      String to parse
	Output:     S record object with string data in fields
	*/

    struct S_Record rec;

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

#ifdef DEBUG
		printf("%2x", byte);
#endif

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