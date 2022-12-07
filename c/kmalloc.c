/**
   Various functions for memory management
*/
#include "stdlib.h"

extern unsigned char *get_heap_space(void);
extern void load_page_directory(unsigned int*);
extern void enable_paging();

// Paging definitions
uint32_t page_directory[1024] __attribute__((aligned(4096)));
//uint32_t first_page_table[1024] __attribute__((aligned(4096)));

unsigned char *heap_start;
extern unsigned char *brk_ptr;

buddy_t *head;

// kernel sbrk
// should only be run by functions in this file
// should not be run directly.
unsigned char *ksbrk(size_t sz, int align){
	unsigned char *ptr = brk_ptr;
	uint32_t offset = align - ((uint32_t)ptr%align);
	if((uint32_t)ptr % align > 0){
		ptr = ptr + align - ((uint32_t)ptr%align);
	}
	brk_ptr += sz + offset;
	return ptr;
}

// generates identity paging for the given list of addresses
void idpage(uint32_t start_addr, uint32_t size, uint32_t **pg_tbl_ptr){
	if(*pg_tbl_ptr == 0){
		*pg_tbl_ptr = (uint32_t*)ksbrk(4096, 4096);
	}
	uint32_t *page_table = *pg_tbl_ptr;
	if(size > 1024) size = 1024;
	start_addr = start_addr & (~0xFFF);
	for(uint32_t i = 0; i < size; i++){
		page_table[i] = (start_addr + i*0x1000) | 3;
	}
	
}

void mem_init(){
	
	
	for(int i = 0; i < 1024; i++)
	{
                // This sets the following flags to the pages:
                //   Supervisor: Only kernel-mode can access them
                //   Write Enabled: It can be both read from and written to
                //   Not Present: The page table is not
		page_directory[i] = 0x00000002;
	}
	uint32_t *first_page_table;
	idpage(0x00000000, 1024, &first_page_table);
	// Sets up identity page for 0x00000000-0x00000fff
	
	page_directory[0] = ((uint32_t)first_page_table) | 3;
	load_page_directory((unsigned int*)page_directory);
	enable_paging();
	
}

/*
  general memory allocation
  specifically designed with the kernel in mind
 */
void *kmalloc(size_t size){
  
  
}

void kfree(void *ptr);
