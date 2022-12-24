#ifndef STDLIB_H
#define STDLIB_H
/**
   Various functions for memory management
*/

typedef int size_t;
typedef unsigned long uint32_t;
typedef unsigned char uint8_t;

typedef struct BlockInfo{
    size_t sz;
    struct block_info *next;
    char free;
} BlockInfo;

typedef struct ProcessState{
    uint32_t *page_dir;
    unsigned char *brk_ptr;
    uint32_t allocated;
} ProcessState;

#define META_SZ sizeof(block_info_t)

#define EMPTY_TABLE 0x00000002

unsigned char *sbrk(size_t sz);

unsigned char *align_sbrk(size_t sz, int align);

void mem_init(void);

void *kmalloc(size_t size);

void kfree(void *ptr);

/*
  kernel level testing
 */

void kasserteq(uint32_t val1, uint32_t val2);
void kassertneq(uint32_t val1, uint32_t val2);
#endif
