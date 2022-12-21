/**
   Various functions for memory management
*/
#include "display.h"
#include "stdlib.h"
#include "buddy.h"


extern unsigned char *get_heap_space(void);
extern unsigned char *get_stack_space(void);
extern unsigned char *get_stack_ptr(void);
extern unsigned char *get_kernel_start(void);
extern void load_page_directory(unsigned int*);
extern void enable_paging();

// Paging definitions
uint32_t page_directory[1024] __attribute__((aligned(4096)));
//uint32_t first_page_table[1024] __attribute__((aligned(4096)));

/*
  notes on page tables:
  every page table reprensents 1024*4096 = 4.19 megabytes of data
  ksbrk works on id pages to store page tables. it is 
  kernel is mapped to 0xC0000000-0xC0400000 for all tables
  
 */

unsigned char *heap_start;
unsigned char *brk_ptr;

buddy_t *head;

// kernel sbrk
// should only be run by functions doing memory management (this and buddy.c)
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

void map_page_tables(uint32_t page_dir[], uint32_t vaddr, uint32_t raddr, size_t len){
	uint32_t num_tables = (len / (0x400000)) + (len%(0x400000))?1:0; // slow AND useless
	uint32_t first_entry = (vaddr & (0x3FF<<22))>>22;

        vaddr = vaddr & ~0xFFF;
	raddr = raddr & ~0xFFF;

	
	for(int i = 0; i < num_tables; i++){
		uint32_t *table = (uint32_t*)ksbrk(4096, 4096);
	        for(int j = 0; j < 1024 && j < len; j++){
			if(j == 515){
				print_int(((raddr + 0x1000*j) | 0b111));
			}
			table[j] = ((raddr + 0x1000*j) | 0b111);
		}
		
		page_directory[first_entry+i] = (uint32_t)table;
		len -= 0x400000;
	}
}

// generates identity paging for the given list of addresses
void idpage(uint32_t addr, uint32_t size, uint32_t page_dir[]){
	uint32_t first_entry = (addr & (0x3FF<<22))>>22;
	if(page_dir[addr] == EMPTY_TABLE){
		page_dir[addr] = (uint32_t)ksbrk(4096, 4096);
	}
	size = size / 0x1000;
	if(size > 1024) size = 1024;
	addr = addr & (~0xFFF);
	uint32_t *page = (uint32_t*)page_dir[addr];
	for(uint32_t i = 0; i < size; i++){
		page[i] = (addr + i*0x1000) | 3;
	}
	page_dir[addr] = (uint32_t)page | 3;
}

uint32_t *v2raddr(uint32_t page_dir[], uint32_t vaddr) {
	uint32_t *table = (uint32_t*)page_dir[(vaddr & (0x3FF<<22))>>22];
	print("tbl_no ");
	print_int((vaddr & (0x3FF<<22))>>22);
	print_nl();
	print("tbl_ent ");
	print_int((vaddr & (0x3FF<<12))>>12);
	print_nl();
	return  (uint32_t*)table[(vaddr & (0x3FF<<12))>>12];
}

void mem_init(){
	
	brk_ptr = get_heap_space();
	for(int i = 0; i < 1024; i++)
	{
                // This sets the following flags to the pages:
                // -Supervisor: Only kernel-mode can access them
                // -Write Enabled: It can be both read from and written to
                // -Not Present: The page table is not
		page_directory[i] = EMPTY_TABLE;
	}

	uint32_t *first_page_table;
	// Sets up identity page for 0x00000000-0x00000fff
	idpage(0x00000000, 1024*0x1000, page_directory);	

	// Maps kernel to 0xC0000000
	map_page_tables(page_directory, 0xC0000000, (uint32_t)get_kernel_start(), 0x400000);

	load_page_directory((unsigned int*)page_directory);
	enable_paging();

	init_buddy_alloc();
	uint32_t brkpp = (uint32_t)&brk_ptr;
	kasserteq(brkpp | 0b111, (uint32_t)v2raddr(page_directory, (brkpp & 0x3FFFFF) + 0xC0000000 - 0x100000));
}

/*
  general memory allocation
  specifically designed with the kernel in mind
 */
void *kmalloc(size_t size){
  
  
}

void kfree(void *ptr);
