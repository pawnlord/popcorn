#include "buddy.h"
#include "display.h"
uint8_t buddies[BUDDIES_SIZE]; // all buddies possible for full 32bit space

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
	}
	print("Buddy level 0: ");
	print_int(BUDDY_LEVEL(0));
	print_nl();
		
	kasserteq(buddies[0x0], 0xFF);
	kasserteq(buddies[0x7F], 0xFF);
	kasserteq(buddies[BUDDY_LEVEL(4) + 0x0], 0xFF);
	kasserteq(buddies[BUDDY_LEVEL(4)+((0x80)/(1<<4)-1)], 0xFF);
}

