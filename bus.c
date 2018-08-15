/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Bus function mainline
- Finlay Miller B00675696
- 19 July 2018
*/

#include "bus.h"
#define DEBUG

void bus(Emulator *emulator, enum SIZE BW, enum ACTION RW)
{
	/*
	Purpose:	Passes information from the CPU to memory and vice versa
	Input:      Amount of data to work with [8 | 16 bits]
				Action to perform [Read | Write]
	Output:     None, but memory or registers may be modified. System clock is
				updated as well.
	*/

	// create some fresh pointers
	Device *dev[DEVCNT] = { NULL };
	for (int i = 0; i < DEVCNT; i++)
		dev[i] = emulator->device[i];

#ifdef DEBUG
	printf("\nBus called with settings:\n%s, %s, MAR = 0x%x, MBR = 0x%x",
		(BW) ? "BYTE" : "WORD", (RW) ? "READ" : "WRITE", MAR, MBR);
#endif // DEBUG


	if (MAR < 16)
	{
		// device memory accessed
#ifdef DEBUG
		printf("\nDevice memory accessed with bus settings\nMAR:\t%d\tMBR:\t%hu\nBW:\t%d\tRW:\t%d", MAR, (unsigned int)MBR, RW, BW);
#endif // DEBUG

		int dev_num = MAR / 2;	// get device number
		if (RW == READ)
		{
			if (dev[dev_num]->io)	// input
			{	// clear DBA
				dev[dev_num]->dba = 0;		// modify struct
				memory.byte[MAR] &= db_mask;	// modify memory
			}

			MBR = memory.byte[MAR];
		}
		else	// write
		{
			if (!dev[dev_num]->io)	// output
			{	// clear DBA and send output
				dev[dev_num]->dba = 0;
				memory.byte[MAR] = (unsigned char)MBR;
				dev[dev_num]->time_l = dev[dev_num]->time_p;
				dev[dev_num]->data = (unsigned char)MBR;
			}
		}
#ifdef DEBUG
		printf("\nDevice #%d accessed. Data = 0x%x", dev_num, memory.byte[MAR]);
		printf(" & Port = "PRINTF_BINARY_PATTERN_INT8, PRINTF_BYTE_TO_BINARY_INT8(memory.byte[MAR - 1]));
#endif // DEBUG
		return;
	}
	else
	{
		// normal memory accessed
		if (RW == READ)
		{
			// read from memory
			MBR = (BW == BYTE) ? memory.byte[MAR] : memory.word[MAR / 2];
		}
		else
		{
			// write to memory
			if (BW == BYTE) {
				// get low byte
				memory.byte[MAR] = (unsigned char)MBR;
			}
			else {
				memory.word[MAR / 2] = MBR;
			}
		}
	}

	emulator->cpu->SYSCLK += 2;

	return;
}
