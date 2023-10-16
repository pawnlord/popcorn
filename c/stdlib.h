#ifndef STDLIB_H
#define STDLIB_H
#include "stdint.h"
/**
   Various functions for memory management
*/

#define META_SZ sizeof(block_info_t)

#define EMPTY_TABLE 0x00000006

#define KERNEL_START_ADDR 0xC0000000

unsigned char *sbrk(size_t sz);

unsigned char *align_sbrk(size_t sz, int align);

void mem_init(void);

void *malloc(size_t size);

void free(void *ptr);

/*
  Command Execution
 */

int execvp(const char *command, char *argv[]);
int fork();

#endif
