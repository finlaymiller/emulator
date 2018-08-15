/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Cache mainline
- Finlay Miller B00675696
- 30 July 2018
*/

#include "cache.h"

#define DEBUG

void check_cache(Emulator *emulator, enum SIZE BW, enum ACTION RW)
{
	/*
	Purpose:	Checks whether or not the requested data is in the cache
	Input:      Emulator pointer, data access size & action to perform
	Output:     None, but accesses memory and bus
	*/

	// create some fresh pointers
	Cache *cache = emulator->cache;
	emulator->accessed_cache = true;
	cache->rewr = RW; cache->woby = BW;

	unsigned int line_num;		// cache line being searched for

#ifdef DEBUG
	if (PC > 0x20A) return;
	printf("\n\nPC = 0x%03x\tSYSCLK = %03d", PC, emulator->cpu->SYSCLK);
	printf("\nSearching cache for address 0x%x with BW = %s RW = %s",
		MAR,
		(cache->woby) ? "Byte" : "Word",
		(cache->rewr) ? "Write" : "Read");
#endif // DEBUG

	// prevent device data from being stored in cache
	if (MAR < 0x10)
	{
#ifdef DEBUG
		printf("\nDevice data accessed, bypassing cache");
#endif // DEBUG
		bus(emulator, cache->woby, cache->rewr);
		return;
	}

	// look for match
	line_num = find_match(emulator);

	if (cache->hit)	// HIT
	{
#ifdef DEBUG
#ifndef HYBRID
		// hybrid cache format doesn't use an address field
		printf("\nCache HIT on line 0x%04x\nCache addr: 0x%04x Data: 0x%04x DB: %d", line_num,
			cache->cache_mem[line_num].addr.bits.cache_address,
			cache->cache_mem[line_num].contents,
			cache->cache_mem[line_num].dirty_bit);
#ifndef DIRECT 
		printf(" Age: %d", cache->cache_mem[line_num].age);
#endif
#elif defined HYBRID
		printf("\nCache HIT on line 0x%04x\nCache addr: 0x%04x Data: 0x%04x DB: %d Age: %d", line_num,
			cache->cache_mem[line_num].addr.address,
			cache->cache_mem[line_num].contents,
			cache->cache_mem[line_num].dirty_bit,
			cache->cache_mem[line_num].age);
#endif	// HYBRID
#endif	// DEBUG

		if (cache->rewr)	// RW == write
		{
#ifdef HYBRID
			hybri_write(emulator, line_num);
#else
			cache_write(emulator, line_num);
#endif // HYBRID

#ifdef DEBUG
			printf("\nLine 0x%x written with data 0x%x", line_num,
				cache->cache_mem[line_num].contents);
#endif // DEBUG
		}					// RW == read	
		else
		{
#ifdef HYBRID
			hybri_read(emulator, line_num);
#else
			cache_read(emulator, line_num);
#endif // HYBRID
#ifdef DEBUG
			printf("\nData 0x%x read from line 0x%x",
				cache->cache_mem[line_num].contents, line_num);
#endif // DEBUG
		}
	}
	else			// MISS
	{
#ifdef DEBUG
		printf("\nCache MISS looking for address 0x%04x at line 0x%02x", MAR, line_num);
#ifndef HYBRID
		printf("\nInstead, this was found:\naddr = 0x%x caddr = 0x%x data = 0x%x",
			cache->cache_mem[line_num].addr.address,
			cache->cache_mem[line_num].addr.bits.cache_address,
			cache->cache_mem[line_num].contents);
#elif defined HYBRID
		printf("\nInstead, this was found:\naddr = 0x%x data = 0x%x",
			cache->cache_mem[line_num].addr.address,
			cache->cache_mem[line_num].contents);
#endif	// HYBRID
#endif	// DEBUG

#ifndef DIRECT
		// get oldest line if mapping method is not direct
		// If it is direct the line will already be set
		line_num = get_oldest_line(emulator);
#endif	// DIRECT

		// use bus to send requested data to MBR. Not necessary if writing
		if (!cache->rewr)	// RW == read
			bus(emulator, READ, WORD);
		
		// either way, write data to new cache line_num
#ifndef HYBRID
		cache->cache_mem[line_num].addr.address = MAR;
#elif defined HYBRID
		cache->cache_mem[line_num].addr.address = MAR & ADDR_MASK;
#endif
		cache->cache_mem[line_num].contents = MBR;
		cache->cache_mem[line_num].dirty_bit = 0;

#ifdef DEBUG
#ifndef HYBRID
		printf("\nNew cache line added at position 0x%04x:\nAddr: 0x%x cAddr: 0x%x Data: 0x%x",
			line_num, cache->cache_mem[line_num].addr.address,
			cache->cache_mem[line_num].addr.bits.cache_address,
			cache->cache_mem[line_num].contents);
#elif defined HYBRID
		printf("\nNew cache line added at position 0x%04x:\nAddr: 0x%x Data: 0x%x",
			line_num, cache->cache_mem[line_num].addr.address,
			cache->cache_mem[line_num].contents);
#endif // HYBRID
#endif // DEBUG

	}	// HIT|MISS

#ifndef DIRECT
	// age data
	data_birthday(emulator, line_num);
	cache->cache_mem[line_num].age = 31;	// set accessed line age to youngest
#endif // !DIRECT

	display_cache(emulator);

	return;
}

int find_match(Emulator *emulator)
{
	/*
	Purpose:	Check cache for an address which matches that of the MAR
	Input:      Emulator pointer
	Output:     Line number in cache of match. 
				Also sets cache.hit when appropriate
	*/

	// fresh pointer
	Cache *cache = emulator->cache;
	unsigned short mem_address = MAR >> 1;	// get 15-bit MAR
	unsigned int line_num = 0;		// cache line being searched for

	// check cache for line
#ifdef DIRECT
	// if mapping type is direct, use hashing function
	line_num = (mem_address % CACHE_SIZE);
	cache->hit = (mem_address == (cache->cache_mem[line_num].addr.bits.cache_address));
	// yeeeaaaaaah these nested structs and stuff are gonna get really long
#elif defined ASSOCIATIVE
	// use linear search
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		cache->hit = (mem_address == cache->cache_mem[i].addr.bits.cache_address);
		if (cache->hit)
		{
			line_num = i;
			break;
		}
	}
#elif defined HYBRID
	// use a combination of direct and associative
	mem_address = MAR & ADDR_MASK;	// get last 5 bits of MAR

	// use direct mapping to find which page the line will be on
	unsigned int page = (MAR & PAGE_MASK) % PAGE_COUNT;

	// use associative mapping to find which line of the page the line will be on
	for (int i = 0; i < PAGE_SIZE; i++)
	{
#ifdef DEBUG
		printf("\nSearching for page %d and line %d", page, page * PAGE_SIZE + i);
#endif // DEBUG

		// compare last 5 bits of MAR to address of cache line
		cache->hit = (mem_address == cache->cache_mem[page * PAGE_SIZE + i].addr.address);
		if (cache->hit)
		{
			line_num = i;
			break;
		}
	}
#endif

	return line_num;
}

// Associative & Hybrid specific functions
#ifndef DIRECT
int get_oldest_line(Emulator *emulator)
{
	/*
	Purpose:	Gets position of oldest line in cache
	Input:      Emulator pointer
	Output:     Line number
	*/

	// create some fresh pointers
	Cache *cache = emulator->cache;

	int line = 0;		// starting line
	int size;			// number of lines to search through

#ifdef ASSOCIATIVE
	size = CACHE_SIZE;
	int eldest = 31;	// start from youngest line
#elif defined HYBRID
	size = PAGE_SIZE;
	int eldest = 3;		// start from youngest line
#endif

	for (int i = 0; i < (signed int)size; i++)
	{
		if ((signed int)cache->cache_mem[i].age < eldest)
		{
			eldest = cache->cache_mem[i].age;
			line = i;
		}
	}

#ifdef DEBUG
	printf("\nOldest line is 0x%02x at age = %d", line, eldest);
#endif // DEBUG

	return line;
}

void data_birthday(Emulator *emulator, int line)
{
	/*
	Purpose:	Age all cache lines which are younger than 
				the cache line being replaced. 0 is oldest, 31 is youngest.
	Input:      Emulator pointer, line being replaced
	Output:     None
	*/

	// create shorter pointer
	Cache *cache = emulator->cache;

	unsigned int size;

#ifdef ASSOCIATIVE
	size = CACHE_SIZE;
#elif defined HYBRID
	size = PAGE_SIZE;
#endif

	// decrement age of each line in cache
	for (int i = 0; i < (signed int)size; i++)
		if (cache->cache_mem[i].age > cache->cache_mem[line].age)
			cache->cache_mem[i].age--;

	return;
}
#endif // !DIRECT

// Non-hybrid specific functions
#ifndef HYBRID
void cache_write(Emulator *emulator, int line)
{
	/*
	Purpose:	Writes data to cache and potentially to memory
	Input:      Emulator pointer, line to write
	Output:     None
	*/

	// create local pointer
	Cache *cache = emulator->cache;
	// byte to work with
	unsigned short data_byte;

#ifdef WRITEBACK
	if (cache->cache_mem[line].dirty_bit)
	{	// if dirty bit is set write new data to memory
#ifdef DEBUG
		printf("\nWriting line 0x%04x back with data 0x%04x", line,
			MBR);
#endif // DEBUG
		bus(emulator, WORD, WRITE);
	}

	// set dirty bit
	cache->cache_mem[line].dirty_bit = 1;
#else	// WRITETHROUGH
	// write straight to memory if using writethrough
#ifdef DEBUG
	printf("\nWriting line 0x%04x through with data 0x%04x", line,
		MBR);
#endif // DEBUG
	bus(emulator, WORD, WRITE);
#endif

	if (cache->woby)	// byte access
	{
		if (cache->cache_mem[line].addr.bits.odd_even)
		{	// if odd_even bit is set, write to high byte of word
			data_byte = (MBR & HI_BYTE);
			cache->cache_mem[line].contents &= LO_BYTE;	// clear high byte
		}
		else
		{	// if odd_even bit is cleared, write to low byte of word
			data_byte = (MBR & LO_BYTE);
			cache->cache_mem[line].contents &= HI_BYTE;	// clear low byte
		}
		printf("\nData byte = "PRINTF_BINARY_PATTERN_INT8, PRINTF_BYTE_TO_BINARY_INT8(data_byte));
		// send data to cache line
		cache->cache_mem[line].contents |= data_byte;
	}
	else				// word access
	{
		cache->cache_mem[line].contents = MBR;
	}

	// just some debugging text
#ifdef DEBUG
	printf("\nWriting from cache line 0x%02x", line);
	if (cache->woby && cache->cache_mem[line].addr.bits.odd_even)
	{
		printf("\nWriting high byte: 0x%x", (cache->cache_mem[line].contents & HI_BYTE) >> 8);
	}
	else if (cache->woby && !cache->cache_mem[line].addr.bits.odd_even)
	{
		printf("\nWriting low byte: 0x%x", (cache->cache_mem[line].contents & LO_BYTE) >> 8);
	}
	else if (!cache->woby)
	{
		printf("\nWriting word: 0x%x", cache->cache_mem[line].contents);
	}

	printf("\nmem[0x%04x] = 0x%04x", MAR,
		(cache->woby) ? memory.byte[MAR] : memory.word[MAR / 2]);
#endif // DEBUG

	return;
}

void cache_read(Emulator *emulator, int line)
{
	/*
	Purpose:	Reads data from cache
	Input:      Emulator pointer, line to read from
	Output:     None
	*/

	// create local pointer
	Cache *cache = emulator->cache;
	// byte to work with
	unsigned short data_byte;

	// just some more debugging text
#ifdef DEBUG
	printf("\nReading from cache line 0x%02x", line);
	if (cache->woby && cache->cache_mem[line].addr.bits.odd_even)
	{
		printf("\nReading high byte: 0x%x", (cache->cache_mem[line].contents & HI_BYTE) >> 8);
	}
	else if (cache->woby && !cache->cache_mem[line].addr.bits.odd_even)
	{
		printf("\nReading low byte: 0x%x", (cache->cache_mem[line].contents & LO_BYTE) >> 8);
	}
	else if (!cache->woby)
	{
		printf("\nReading word: 0x%x", cache->cache_mem[line].contents);
	}
#endif // DEBUG


	if (cache->woby)	// byte access
	{
		if (cache->cache_mem[line].addr.bits.odd_even)
		{	// if odd_even bit is set, read from high byte of word
			data_byte = (cache->cache_mem[line].contents & HI_BYTE);
			data_byte >>= 8;	// shift data down to low byte of MBR
		}
		else
		{	// if odd_even bit is cleared, read from low byte of word
			data_byte = (cache->cache_mem[line].contents & LO_BYTE);
		}

		MBR = data_byte;	// send data to MBR
	}
	else				// word access
	{
		MBR = cache->cache_mem[line].contents;
	}

	return;
}
#else
void hybri_write(Emulator *emulator, int line)
{
	/*
	Purpose:	Writes data to cache and potentially to memory
	Input:      Emulator pointer, line to write
	Output:     None
	*/

	// create local pointer
	Cache *cache = emulator->cache;
	// byte to work with
	unsigned short data_byte;

#ifdef WRITEBACK
	if (cache->cache_mem[line].dirty_bit)
	{	// if dirty bit is set write new data to memory
#ifdef DEBUG
		printf("\nWriting line 0x%04x back with data 0x%04x", line,
			MBR);
#endif // DEBUG
		bus(emulator, WORD, WRITE);
	}

	// set dirty bit
	cache->cache_mem[line].dirty_bit = 1;
#else	// WRITETHROUGH
	// write straight to memory if using writethrough
#ifdef DEBUG
	printf("\nWriting line 0x%04x through with data 0x%04x", line,
		MBR);
#endif // DEBUG
	bus(emulator, WORD, WRITE);
#endif

	if (cache->woby)	// byte access
	{
		if (cache->cache_mem[line].addr.bits.line % 2)
		{	// if line is odd, write to high byte of word
			data_byte = (MBR & HI_BYTE);
			cache->cache_mem[line].contents &= LO_BYTE;	// clear high byte
		}
		else
		{	// if line is even, write to low byte of word
			data_byte = (MBR & LO_BYTE);
			cache->cache_mem[line].contents &= HI_BYTE;	// clear low byte
		}
		printf("\nData byte = "PRINTF_BINARY_PATTERN_INT8, PRINTF_BYTE_TO_BINARY_INT8(data_byte));
		// send data to cache line
		cache->cache_mem[line].contents |= data_byte;
	}
	else				// word access
	{
		cache->cache_mem[line].contents = MBR;
	}

	// just some debugging text
#ifdef DEBUG
	printf("\nWriting from cache line 0x%02x", line);
	if (cache->woby && (cache->cache_mem[line].addr.bits.line % 2))
	{				//  ^ odd/even check
		printf("\nWriting high byte: 0x%x", (cache->cache_mem[line].contents & HI_BYTE) >> 8);
	}
	else if (cache->woby && !(cache->cache_mem[line].addr.bits.line % 2))
	{					//    ^ odd/even check
		printf("\nWriting low byte: 0x%x", (cache->cache_mem[line].contents & LO_BYTE) >> 8);
	}
	else if (!cache->woby)
	{
		printf("\nWriting word: 0x%x", cache->cache_mem[line].contents);
	}

	printf("\nmem[0x%04x] = 0x%04x", MAR,
		(cache->woby) ? memory.byte[MAR] : memory.word[MAR / 2]);
#endif // DEBUG

	return;
}

void hybri_read(Emulator *emulator, int line)
{
	/*
	Purpose:	Reads data from cache
	Input:      Emulator pointer, line to read from
	Output:     None
	*/

	// create local pointer
	Cache *cache = emulator->cache;
	// byte to work with
	unsigned short data_byte;

	// just some more debugging text
#ifdef DEBUG
	printf("\nReading from cache line 0x%02x", line);
	if (cache->woby && (cache->cache_mem[line].addr.bits.line % 2))
	{
		printf("\nReading high byte: 0x%x", (cache->cache_mem[line].contents & HI_BYTE) >> 8);
	}
	else if (cache->woby && !(cache->cache_mem[line].addr.bits.line % 2))
	{
		printf("\nReading low byte: 0x%x", (cache->cache_mem[line].contents & LO_BYTE) >> 8);
	}
	else if (!cache->woby)
	{
		printf("\nReading word: 0x%x", cache->cache_mem[line].contents);
	}
#endif // DEBUG


	if (cache->woby)	// byte access
	{
		if (cache->cache_mem[line].addr.bits.line % 2)
		{	// if line is odd, read from high byte of word
			data_byte = (cache->cache_mem[line].contents & HI_BYTE);
			data_byte >>= 8;	// shift data down to low byte of MBR
		}
		else
		{	// if line is even, read from low byte of word
			data_byte = (cache->cache_mem[line].contents & LO_BYTE);
		}

		MBR = data_byte;	// send data to MBR
	}
	else				// word access
	{
		MBR = cache->cache_mem[line].contents;
	}

	return;
}
#endif // !HYBRID

void display_cache(Emulator *emulator)
{
	/*
	Purpose:	Prints cache contents to screen
	Input:      Emulator pointer
	Output:     None
	*/

	// create shorter pointer
	Cache *cache = emulator->cache;
	// open file
	FILE *cf = fopen(emulator->cfile, "a");
	
	// print header
	fprintf(cf, "\n- PC = 0x%02x - CACHE - SYSCLK = %03d -", PC, emulator->cpu->SYSCLK);
	fprintf(cf, "\nPOS\t\t\tADDR\t\tDATA\tDB");
#ifndef DIRECT
	fprintf(cf, "\tAGE");
#endif // DIRECT

	// print cache data
#ifndef HYBRID
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		fprintf(cf, "\n0x%02x\t\t0x%04x\t\t0x%04x\t%d", i, 
			cache->cache_mem[i].addr.address,
			cache->cache_mem[i].contents, 
			cache->cache_mem[i].dirty_bit);
#ifdef ASSOCIATIVE
		fprintf(cf, "\t%d", cache->cache_mem[i].age);
#endif // ASSOCIATIVE

	}
#else	//!HYBRID
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		fprintf(cf, "\n0x%02x\t\t%d%d\t\t0x%04x\t%d\t%d", i,
			cache->cache_mem[i].addr.bits.page,
			cache->cache_mem[i].addr.bits.line,
			cache->cache_mem[i].contents,
			cache->cache_mem[i].dirty_bit,
			cache->cache_mem[i].age);
	}
#endif	//HYBRID

	fclose(cf);

	return;
}