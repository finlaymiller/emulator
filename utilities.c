
#include "utilities.h"
//#define DEBUG

void handle_sigint(Emulator *emulator)
{
	/*
	Purpose:	Calls signal handler, sets flag in emulator
	Input:      Emulator pointer & signal
	Output:     None
	*/

	emulator->waiting_for_signal = false;
	signal(SIGINT, (_crt_signal_t)handle_sigint);

	return;
}


void init_structs(Emulator *emulator)
{
	/*
	Purpose:	Clears or sets struct flags as required
	Input:      Emulator pointer
	Output:     None
	*/

	// start at max priority to avoid interruptions
	set_priority(emulator, 7);

	// emulator flags
	emulator->waiting_for_signal = true;
	emulator->n_e->done = false;
	emulator->n_e->used = true;	// initialized to true so that first event
								// wil be loaded.
	// debugger flags
	emulator->debugger->debug_mode = false;
	emulator->debugger->bp_flag = false;
	emulator->debugger->error_flag = false;
	emulator->debugger->quit_flag = false;
	emulator->debugger->loading = false;
	emulator->debugger->device_flag = false;
	// debugger data
	emulator->debugger->bp_count = 0;
	memset(emulator->debugger->bp_addresses, 0, 
		sizeof(emulator->debugger->bp_addresses));

	// cpu flags
	emulator->cpu->running = false;
	emulator->cpu->sleeping = false;
	// cpu data
	emulator->cpu->SYSCLK = 0;
	emulator->cpu->END = 0;

	// cache data
	for (int i = 0; i < CACHE_SIZE; i++)
	{
#ifdef HYBRID
		emulator->cache->cache_mem[i].addr.bits.page = 0;
		emulator->cache->cache_mem[i].addr.bits.line = 0;
#else
		emulator->cache->cache_mem[i].addr.address = 0;
#endif // HYBRID
		emulator->cache->cache_mem[i].contents = 0;
		emulator->cache->cache_mem[i].dirty_bit = 0;

#ifndef DIRECT
		emulator->cache->cache_mem[i].age = i;
#endif // !DIRECT


	}
	// cache flags
	emulator->cache->hit = false;
	emulator->cache->rewr = false;
	emulator->cache->woby = false;
	// clear cache output file
	strcpy(emulator->cfile, "final_cache.txt");
	FILE *cf = fopen(emulator->cfile, "w");
	fclose(cf);

	return;
}

void set_priority(Emulator *emulator, int level)
{
	/*
	Purpose:	Set CPU priority to specified level [0-7]
	Input:      Emulator pointer
	Output:     None
	*/

	int bin[3] = { 0 };	// array to hold binary version of input level
	int i = 0;

	// make sure level is within accepted range
	if ((level > 7) || (level < 0)) db_error(emulator, BAD_LVL);

	// clear PSW bits
	for (int i = 0; i < 3; i++) regfile[0][PSW] &= (bin[i] << (i + 5));

#ifdef DEBUG
	printf("\nConverting level %d to binary ", level);
#endif // DEBUG

	// update CPU before level is used
	emulator->cpu->PRIORITY = level;

	// convert to binary
	while (level > 0)
	{
		bin[i] = level % 2;
		level /= 2;
		i++;
	}

#ifdef DEBUG
	for (int j = 2; j >= 0; j--) printf("%d", bin[j]);
#endif // DEBUG

	// set PSW bits
	regfile[0][PSW] |= (bin[2] << PRI1);
	regfile[0][PSW] |= (bin[1] << PRI2);
	regfile[0][PSW] |= (bin[0] << PRI3);

#ifdef DEBUG
	printf("\nPSW = "PRINTF_BINARY_PATTERN_INT16, 
		PRINTF_BYTE_TO_BINARY_INT16(regfile[0][PSW]));
#endif // DEBUG

	return;
};