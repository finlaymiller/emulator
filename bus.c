/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Bus function mainline
- Finlay Miller B00675696
- 01 JULY 2018
*/

#include "bus.h"

void bus(unsigned short MAR, unsigned short MDR, enum BW BW, enum RW RW)
{
    if (MAR < 16)
    {
        // device memory accessed
        printf("Device memory accessed with bus settings\nMAR:\t%d\nMDR:\t%d\nBW:\t%d\nRW:\t%d", MAR, MDR, RW, BW);
        getchar();
        exit(EXIT_FAILURE);
    }
    else
    {
        // normal memory accessed
        if (RW == READ)
        {
            // read from memory
            if (BW == BYTE)
            {
                MDR = (unsigned char) memory.byte[MAR];
            }
            else
            {
                MDR = memory.word[MAR/2];
            }
        }
        else
        {
            // write to memory
            if (bw == BYTE) {
                // get low byte
				memory.byte[MAR] = MDR & 0x00FF;
			}
			else {
				memory.word[MAR/2] = MDR;
			}
        }
    }

    return;
}