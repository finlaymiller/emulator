/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Device-related function mainline
- Finlay Miller B00675696
- 19 July 2018
*/

#include "devices.h"

#define DEBUG

void dev_init(Emulator *emulator)
{
	/*
	Purpose:	Initialize device settings
	Input:      Array of device types & processing times
	Output:     None, but accesses global device array
	*/

	// create some fresh pointers
	Next_Event *event = emulator->n_e;
	Device *dev[DEVCNT] = { NULL };
	for (int i = 0; i < DEVCNT; i++)
		dev[i] = emulator->device[i];

	char buffer[10], *tok;		// raw character buffer & token pointer
	// open device file
	FILE *df = fopen(emulator->dfile, "r");
	if (df == NULL) db_error(emulator, BAD_DEV);

	// set debugger flag
	emulator->debugger->device_flag = true;

	// load initial device settings into struct
	for (int i = 0; i < DEVCNT; i++)	
	{
		fgets(buffer, sizeof(buffer), df);	// read line
		tok = strtok(buffer, " ");			// get first token

		// set values
		dev[i]->io = atoi(tok);				// send converted token to io bit
		tok = strtok(NULL, " ");			// get next token
		dev[i]->time_p = atoi(tok);			// send converted token to time_p
		dev[i]->dba = 0;					// clear DBA bit
		dev[i]->oV = 0;						// clear oVerflow bit
		dev[i]->time_l = dev[i]->time_p;	// reset time remaining
		dev[i]->data = '\0';				// clear data

		// clear buffer
		strcpy(buffer, "");

#ifdef DEBUG
		printf("\n%d %d %d\tread to device %d", 
			dev[i]->io, dev[i]->time_p, dev[i]->time_l, i);
#endif // DEBUG

	}
	
	// save position in file
	emulator->df_pos = ftell(df);
#ifdef DEBUG
	printf("\nFinished loading at file position %d", emulator->df_pos);
#endif // DEBUG

	// write device settings to memory
	for (int i = 0; i < DEVCNT; i++)
	{
		SET_BIT(memory.byte[i * 2], IO__BIT, dev[i]->io);
		SET_BIT(memory.byte[i * 2], DBA_BIT, dev[i]->dba);
		SET_BIT(memory.byte[i * 2], OVF_BIT, dev[i]->oV);
		memory.byte[i * 2 + 1] = dev[i]->data;
	}

	// get first event from device input file
	//dev_get_event(emulator);

#ifdef DEBUG
	// print init settings
	printf("\nDevice array initialized with the following settings:");
	for (int j = 0; j < DEVCNT; j++)
	{
		printf("\nDevice #%d\t\t\t|  memory.word[ %d ]", j, j);
		printf("\ninput: %s\toV: %s\t\t|  "PRINTF_BINARY_PATTERN_INT16,
			dev[j]->io ? "input" : "output", dev[j]->oV ? "yes" : "no", 
			PRINTF_BYTE_TO_BINARY_INT16(memory.word[j]));
		printf("\ndba: %d\t\tdata: %c\t\t|", dev[j]->dba, dev[j]->data);
		printf("\ntime_p: %d\ttime_l: %d\t|", dev[j]->time_p, dev[j]->time_l);
	}
#endif // DEBUG

	fclose(df);

	return;
}

void dev_o_check(Emulator *emulator)
{
	/*
	Purpose:	Check to see if any output devices are ready to, well, output
	Input:      Emulator pointer
	Output:     None, but accesses global device array
	*/

	// create some fresh pointers
	CPU *cpu = emulator->cpu;
	Device *dev[DEVCNT] = { NULL };
	for (int i = 0; i < DEVCNT; i++)
		dev[i] = emulator->device[i];

	FILE *of = fopen(emulator->ofile, "a");

	for (int i = 0; i < DEVCNT; i++)
	{
		// check for overflow
		dev[i]->oV = !dev[i]->dba;
		SET_BIT(memory.byte[i * 2], OVF_BIT,
			CHECK_BIT(memory.byte[i * 2], DBA_BIT));

		if (!dev[i]->io)
		{
			if (dev[i]->time_l > 0)
				dev[i]->time_l--;
					  
			if (dev[i]->time_l <= 0)
			{
				// write output to file
				fprintf(of, "\n%d\t%d\t%c",cpu->SYSCLK, i, dev[i]->data);
				
				// reset dba bit
				dev[i]->dba = 1;
				SET_BIT(memory.byte[i * 2], DBA_BIT, 1);
			}
		}
	}

	fclose(of);

	return;
}

void dev_i_check(Emulator *emulator)
{
	/*
	Purpose:	Check to see if any devices have input waiting
	Input:      Emulator pointer
	Output:     None, but accesses global device array
	*/

	// set up some fresh pointers
	CPU *cpu = emulator->cpu;
	Next_Event *event = emulator->n_e;

	// check for waiting input events
	while ((cpu->SYSCLK >= event->time) && !event->done)
	{
		event = emulator->n_e;
		Device *dev = emulator->device[event->dev];
		//	   ^ points to device of next event

#ifdef DEBUG
		printf("\nChecking for event on device #%d", event->dev);
#endif // DEBUG

		// check for overflow
#ifdef DEBUG
		if (dev->dba) printf("\nOverflow detected");
#endif // DEBUG
		dev->oV = dev->dba;
		SET_BIT(memory.byte[event->dev * 2], OVF_BIT,
			CHECK_BIT(memory.byte[event->dev * 2], DBA_BIT));

		// get character
		dev->data = event->character;
		memory.byte[(event->dev * 2) + 1] = event->character;

#ifdef DEBUG
		printf("\nSetting dba");
#endif // DEBUG

		// set dba
		dev->dba = 1;
		SET_BIT(memory.byte[event->dev * 2], DBA_BIT, 1);

		// get next event
		event->used = true;
		dev_get_event(emulator);
	}

	return;
}

void dev_get_event(Emulator *emulator)
{
	/*
	Purpose:	Get next device input event from file
	Input:      Emulator pointer
	Output:     None
	*/

	// create some fresh pointers
	Next_Event *event = emulator->n_e;

	char buffer[10];	// input buffer
	char *tok;			// pointer to token from raw input
	int sz, cp;			// device file size & current position		

	// open device file and move to last position
	FILE *df = fopen(emulator->dfile, "r");
	fseek(df, emulator->df_pos, SEEK_SET);
	cp = ftell(df);

	// make sure that we aren't at the end of the file
	fseek(df, 0L, SEEK_END);
	sz = ftell(df);
	if (cp >= sz)
	{	// if past end of file, break
		event->done = true;
		return;
	}
	else
	{	// return to last position
		fseek(df, emulator->df_pos, SEEK_SET);
		cp = ftell(df);
	}

	if (event->used == true)
	{
		fgets(buffer, sizeof(buffer), df);// get raw line

		event->used = false;		// clear used flag
		tok = strtok(buffer, " \t");// get first token

		event->time = atoi(tok);	// get time of event
		tok = strtok(NULL, " \t");	// get next token
		event->dev = atoi(tok);		// get device of event
		tok = strtok(NULL, " \t");	// get next token
		event->character = *tok;	// get char of event
	}

	// update file pointer
	emulator->df_pos = ftell(df);

#ifdef DEBUG
	printf("\nFinished loading event with the following settings:");
	printf("\nDevice #%d at time %d gets \'%c\'", event->dev, event->time,
		event->character);
#endif // DEBUG

	fclose(df);

	return;
}