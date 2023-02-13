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

/*
  Command Execution
 */

typedef struct GDT_Entry{
  unsigned short limit1;
  unsigned short base1;
  unsigned char base2;
  unsigned char access;
  unsigned char limit2flags;
  unsigned char base3;
} GDT_Entry;

uint32_t gdt_get_base(GDT_Entry gdt_entry);
uint32_t gdt_get_limit(GDT_Entry gdt_entry);
char gdt_get_flags(GDT_Entry gdt_entry);

void gdt_set_base(GDT_Entry *gdt_entry, uint32_t base);
void gdt_set_limit(GDT_Entry *gdt_entry, uint32_t limit);
void gdt_set_flags(GDT_Entry *gdt_entry, uint32_t flags);

int execvp(const char *command, char *argv[]);
int fork();

#endif
