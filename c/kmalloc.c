/**
   Various functions for memory management
*/
#include "stdlib.h"

extern unsigned char *get_heap_space(void);

unsigned char* heap_start;
unsigned char* brk_ptr;

void init_mem(void ){
  heap_start = get_heap_space();
  brk_ptr = heap_start + META_SZ;
}

/*
  general memory allocation
  specifically designed with the kernel in mind
 */
void *kmalloc(size_t size){

}

void kfree(void *ptr);
