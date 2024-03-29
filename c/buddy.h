#ifndef BUDDY_H
#define BUDDY_H
/*
  buddy allocation system, used to remember what real addresses we have
  available
 */
#include "stdlib.h"
#include "kstdlib.h"

// one less than actual space as to not have an overflow
// TODO: Custom amount so that we are not just assuming we have memory.
#define ADDR_SPACE 0xFFFFFFFF
// number of levels used
// currently alligned to guarantee 8 bits for every layer
#define BUDDIES 16
#define MAX_BUDDIES 21
#define RMVD_BUDS MAX_BUDDIES - BUDDIES
// number of bytes used total

// (unsigned int)(0x40000*((1-(1.0/(1<<BUDDIES)))/(1-(1.0/2))))
#define BUDDIES_SIZE (524280/8)


void init_buddy_alloc();

// returns 1 on success, 0 on failure
// size in pages, idx into page_table
int request_page(size_t size, uint32_t page_table[], int idx);

// frees one page when it is no longer being used
// returns 1 on success, 0 on failure
int free_page(size_t size, uint32_t page_table[], int idx);
#endif