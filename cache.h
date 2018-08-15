/*
- Emulator of the X-Makina ISA
- ECED3403 Assignment 2
- Cache headerfile
- Finlay Miller B00675696
- 30 July 2018
*/

#ifndef _CACHE_H_
#define _CACHE_H_

// libraries
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// other files
#include "bus.h"
#include "main.h"

// definitions
#define CACHE_SIZE	32		// number of lines in cache
#define PAGE_SIZE	4		// number of lines on page
#define PAGE_COUNT	8		// number of pages in cache
#define PAGE_MASK	0x001C	// 0b11100 - isolates page of hybrid cache
#define ADDR_MASK	0x001F	// 0b11111 - isolates full address of hybrid cache
//#define DIRECT
//#define ASSOCIATIVE
#define HYBRID
#define WRITETHROUGH
//#define WRITEBACK

/* 
	Cache line for direct mapping. 3 types of structure are emulated:
													
					32--------16---------1-----------0
	DIRECT MAPPING: | address | contents | dirty bit |
					 --------------------------------
					38--------22---------6-----1-----------0
	ASSOC MAPPING:	| address | contents | age | dirty bit |
					 --------------------------------------
					27------25-----22---------6-----1-----------0
	HYBRID MAPPING: | page | line | contents | age | dirty bit |
					 -------------------------------------------
*/

// used for word/byte access
typedef struct Overlay
{
	unsigned short odd_even : 1;			// LSB. 1  on odd, 0 on even
	unsigned short cache_address : 15;	// essentially MAR>>1
} Overlay;

typedef union addr_overlay
{
	unsigned short address;		// the actual address
	Overlay bits;				// the struct from 3 lines ago
}addr_overlay;

typedef struct hybrid_overlay
{
	unsigned short page : 3;	// 0 - 7
	unsigned short line : 2;	// 0 - 3
}hybrid_overlay;

typedef union hybrid_addr_overlay
{
	unsigned short address : 5;
	hybrid_overlay bits;
}hybrid_addr_overlay;

typedef struct cache_line
{
	// hybrid addressing has line and page fields instead of an address field
#ifndef HYBRID
	// address overlay for dealing with bytes & odd address
	addr_overlay addr;
#endif // !HYBRID

#ifdef HYBRID
	hybrid_addr_overlay addr;	// hopefully this doesn't get too confusing
#endif // HYBRID

	// direct addressing is the only form that does not use aging
#ifdef ASSOCIATIVE
	unsigned int age : 5;	// 0 - 31
#elif defined HYBRID
	unsigned int age : 2;	// 0 - 3
#endif // !DIRECT

	// every addressing type has both contents and dirty bit fields
	unsigned short contents;
	unsigned int dirty_bit : 1;
}c_line;

typedef struct Cache
{
	c_line cache_mem[CACHE_SIZE];	// cache data

	bool hit;	// true on hit,   false on miss
	bool rewr;	// true on write, false on read
	bool woby;	// true on byte,  false on word
}Cache;

// function declarations
void check_cache(Emulator *emulator, enum SIZE BW, enum ACTION RW);
int find_match(Emulator *emulator);
int get_oldest_line(Emulator *emu);
void data_birthday(Emulator *emulator, int line);
void cache_write(Emulator *emulator, int line);
void cache_read(Emulator *emulator, int line);
void hybri_write(Emulator *emulator, int line);
void hybri_read(Emulator *emulator, int line);
void display_cache(Emulator *emulator);

#endif // !_CACHE_H_
