#include "buddy.h"
#include "display.h"
uint8_t buddies[BUDDIES_SIZE]; // all buddies possible for full 32bit space

void test_req_page();

void init_buddy_alloc(){
    for(int i = 0; i < BUDDIES; i++){
	uint32_t level_start = BUDDY_LEVEL(i);
	for(int j = 0; j < BUDDY_SIZE(i); j++){
	    buddies[level_start+j] = 0;
	}
	// remove in use pages (id pages, as well as all kernel pages currently)
	for(int j = 0; j < (1024/8)/(1<<i); j++){	    
	    buddies[level_start+j] = 0xFF;
	}
	if((1024/8)/(1<<i) == 0){
	    for(int j = 0; j < (1024)/(1<<i) && j < 8; j++){	    
		buddies[level_start] |= 1<<j;
	    }
	}
    }
    print("Buddy level 0: ");
    print_int(BUDDY_LEVEL(0));
    print_nl();
		
    kasserteq(buddies[0x0], 0xFF);
    kasserteq(buddies[0x7F], 0xFF);
    kasserteq(buddies[BUDDY_LEVEL(4) + 0x0], 0xFF);
    kasserteq(buddies[BUDDY_LEVEL(4)+((0x80)/(1<<4)-1)], 0xFF);
    kasserteq(buddies[BUDDY_LEVEL(7) + 0x0], 0xFF);
    kasserteq(buddies[BUDDY_LEVEL(10) + 0x0], 0x01);
    
    test_req_page();
}

int request_page(size_t size, uint32_t page_table[], int idx){
    int level = 0;
    char round_up = 0; 
    size_t temp_sz = size;
    int min_level = 0; // level we change on. at most level
    int min_blocks = 1; // number of blocks used on min_level, at least one
    while(temp_sz >> 1 != 0){
	/* if the tail bits are not empty, then it must round up to the next level
	   e.g 10000 is going to be level 4, but 10001 should be level 5 as it can not fit into a 4 block
	*/
	if(temp_sz & 1){
	    if(round_up == 0){
		min_level = level;
		min_blocks = temp_sz;
	    }
	    round_up++;
	}
	level += 1;
	temp_sz = temp_sz >> 1;
    }
    if(round_up > 0){
	level += 1;
    }
    if(min_level == 0){
	min_level = level;
    }
    
    print_int(level);
    print(", ");
    print_int(min_level);
    print(", ");
    size_t offset = BUDDY_LEVEL(level);
    size_t buddy_size = BUDDY_SIZE(level);
    // Search
    char in_use = 1, bit = -1;
    uint32_t byte =  (1024/8)/(1<<level); // first byte that can be allocated
    while(in_use && byte < (uint32_t)buddy_size){
	bit++;
	byte += (bit%8)?0:1;
	bit %= 8;
	in_use = buddies[offset+byte] & (1<<bit);
    }
    if(in_use) { return 0; }
    
    uint32_t blocks  = (1 << min_level) * min_blocks;
    uint32_t raddr = (byte*8 + bit) * (1 << level) * 0x1000;
    // assignment to page
    for(uint32_t i = 0; i < blocks; i++){
	page_table[idx + i] = raddr;
	raddr += 0x1000;
    }
    // update allocation bitfield
    buddies[offset+byte] |= (1<<bit);

    // up
    int curr_level;
    for(curr_level = level+1; curr_level < BUDDIES; curr_level++){
	int cbit = (byte*8+bit) >> (curr_level-level);
	int cbyte = cbit/8;
	cbit %= 8;
	buddies[BUDDY_LEVEL(curr_level)+cbyte] |= (1<<cbit);
    }

    // down
    for(curr_level = level-1; curr_level >= 0; curr_level--){
	int cbit = (byte*8+bit) << (level-curr_level);
	int cbyte = cbit/8;
	cbit %= 8;
	int cblocks = (min_blocks * (curr_level <= min_level)) << (curr_level - min_level);
	for(int j = 0; j < (1 << (level-curr_level)) && (j < cblocks || cblocks == 0); j++){
	    buddies[BUDDY_LEVEL(curr_level)+cbyte] |= (1<<cbit);
	    cbit++;
	    cbyte += (cbit%8)?0:1;
	    cbit %= 8;  
	}
    }
    return 1;
}

void test_req_page(){
    uint32_t page_table_test[1024];
    kasserteq(request_page(1024, page_table_test, 0), 1); // requesting one page_table
    for(int i = 0; i < 1024; i++){
	kassertneq(0, page_table_test[i]);
    }
    
}
