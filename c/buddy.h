/*
  buddy allocation system, used to remember what real addresses we have available
 */
#include "stdlib.h"

// one less than actual space as to not have an overflow
// TODO: Custom amount so that we are not just assuming we have memory.
#define ADDR_SPACE 0xFFFFFFFF
// number of levels used
// currently alligned to guarantee 8 bits for every layer
#define BUDDIES 16
#define MAX_BUDDIES 21
#define RMVD_BUDS MAX_BUDDIES - BUDDIES
// number of bytes used total
#define BUDDIES_SIZE 0x40000 - ((1 << BUDDIES)/8)

#define BUDDY_LEVEL(l) 0x40000 - ((1 << (RMVD_BUDS+BUDDIES-l)))/8  
#define BUDDY_SIZE(l) BUDDIES_SIZE / (1<<l)

void init_buddy_alloc();

// returns new index on succes, 0 on failure
// size in pages, idx into page_table
int request_page(size_t size, uint32_t page_table[], int idx);

// frees one page when it is no longer being used
// returns 1 on success, 0 on failure
int free_page(uint32_t* page);
