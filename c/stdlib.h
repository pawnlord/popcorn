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
    struct BlockInfo *next;
    char free;
    char allocated;
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

void *malloc(size_t size);

void free(void *ptr);

/*
  kernel level testing
 */

void kasserteq(uint32_t val1, uint32_t val2, const char *msg);
void kassertneq(uint32_t val1, uint32_t val2, const char *msg);
#endif
