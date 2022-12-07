/**
   Various functions for memory management
*/

typedef int size_t;
typedef unsigned long uint32_t;

typedef struct block_info{
  size_t sz;
  struct block_info *next;
  char free;
  
} block_info_t;

typedef struct buddy{
  unsigned char size; // size of block (in bits)
  char block1 : 1;
  char block2 : 2;
  struct buddy *parent;
  struct buddy *children;
} buddy_t; // Smallest block is 1 page, or 4096 bytes


#define META_SZ sizeof(block_info_t)

void mem_init(void);

void *kmalloc(size_t size);

unsigned char *read_brk(void);

void kfree(void *ptr);