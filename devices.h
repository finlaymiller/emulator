/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Headerfile for device-related functions
- Finlay Miller B00675696
- 19 July 2018
*/

#ifndef _DEVICES_H_
#define _DEVICES_H_

// libraries
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// other files
#include "main.h"
#include "cpu.h"
#include "bitmasks.h"
#include "debugger.h"

// definitions
#define DEVCNT 8			// maximum number of devices
#define db_mask 0xFFFB		// 0xFFFB = 1111.1111.1111.1011, which isolates
							// the DBA bit
extern FILE *df;

// Device structure
typedef struct Device
{
	unsigned int io : 1;	// 1 if device is input, 0 if output
	unsigned int dba : 1;	// data [buffer|byte] available
	unsigned int oV : 1;	// overflow bit
	int time_p;		// time required to process output
	int time_l;		// time remaining for current output
	char data;		// byte to read or write
}Device;

struct devS
{
	unsigned int IE : 1;
	unsigned int IO : 1;
	unsigned int DBA: 1;
	unsigned int OV : 1;
	unsigned int RES: 3;
	unsigned short data : 8;
};

typedef struct Next_Event
{
	bool used;			// set when input event has completed. Cleared when
						// event is loaded into emulator
	bool done;			// set when all events have been read
	signed int time;	// time of next event
	unsigned int dev;	// device which the event occurs on
	char character;		// character which will be stored on the device
}Next_Event;

// global device array
extern struct Device device[DEVCNT];

// function declarations
void dev_init(Emulator *emulator);
void dev_o_check(Emulator *emulator);
void dev_i_check(Emulator *emulator);
void dev_get_event(Emulator *emulator);

#endif // !_DEVICES_H_
