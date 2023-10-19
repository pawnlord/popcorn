/**
   Various functions for memory management
*/
#include "buddy.h"
#include "display.h"
#include "stdlib.h"
#include "string.h"
#include "kstdlib.h"

extern unsigned char *get_heap_space(void);
extern unsigned char *get_stack_space(void);
extern unsigned char *get_stack_ptr(void);
extern unsigned char *get_kernel_start(void);
extern void load_page_directory(unsigned int *);
extern void enable_paging();


// Paging definitions

uint32_t kernel_page_dir[1024] __attribute__((aligned(4096)));

uint32_t page_dirs[1024][1024] __attribute__((aligned(4096)));

uint32_t allocated_dirs[32];

// all page tables we will ever need
uint32_t page_tables[1024][1024] __attribute__((aligned(4096)));

PageTable *next_page_table =
    (PageTable *)&page_tables;  // pointer to next page table in the above array

PageTable *get_next_page_table_ptr() { return next_page_table++; }

PageDirectory *get_next_page_dir_ptr() {
    int dir = 0;
    for(int i = 0; i < 32; i++){
        if((allocated_dirs[i]^0) != 0) { // => allocated_dirs[i] != 0xFF..FF
            dir += i * 32;
            int rel_dir = 0;
            int dir_flags = allocated_dirs[i];
            while(dir_flags & 1){
                rel_dir += 1;
                dir_flags >>= 1;
            }
            allocated_dirs[i] |= (1 << rel_dir);
            break;
        }
    }
    return &page_dirs[dir];
}


extern ProcessState *kernel_task;
extern ProcessState *active_task;

// uint32_t first_page_table[1024] __attribute__((aligned(4096)));

/*
  notes on page tables:
  every page table reprensents 1024*4096 = 4.19 megabytes of data
  ksbrk works on id pages to store page tables. it is
  kernel is mapped to 0xC0000000-0xC0400000 for all tables

*/

unsigned char *heap_start;

unsigned char paging_enabled = 0;

// page_dir: page directory to map to
// vaddr: virtual address to map to
// raddr: real address to map from
// len: number of bits
void map_page_tables(uint32_t page_dir[], uint32_t vaddr, uint32_t raddr,
                     size_t len) {
    uint32_t num_tables =
        (len / (0x400000)) + (len % (0x400000)) ? 1 : 0;  // slow AND useless
    uint32_t first_entry = (vaddr & (0x3FF << 22)) >> 22;

    vaddr = vaddr & ~0xFFF;
    raddr = raddr & ~0xFFF;

    for (int i = 0; i < num_tables; i++) {
        uint32_t *table = (uint32_t *)align_sbrk(4096, 4096);
        for (int j = 0; j < 1024 && j < len; j++) {
            table[j] = ((raddr + 0x1000 * j) | 0b111);
        }
        kernel_page_dir[first_entry + i] = (uint32_t)table;
        len -= 0x400000;
    }
}


unsigned char *ksbrk(size_t sz, ProcessState *task) {
    if (!paging_enabled) {
        unsigned char *ptr = active_task->brk_ptr;
        active_task->brk_ptr += sz;
        return ptr;
    } else {
        unsigned char *ptr = active_task->brk_ptr;
        active_task->brk_ptr += sz;
        if ((uint32_t)task->brk_ptr > task->allocated) {
            uint32_t first_entry =
                ((uint32_t)task->brk_ptr & (0x3FF << 22)) >> 22;  // should be the index into the page_directory
            while ((*task->page_dir)[first_entry] != EMPTY_TABLE) {
                first_entry++;
                task->allocated += 1024 * 4096;
                task->brk_ptr += 1024 * 4096;
            }
            PageTable *table = get_next_page_table_ptr();
            request_page(1024, *table, 0);
            (*task->page_dir)[first_entry] = (uint32_t)table;
            task->allocated += 1024 * 4096;
        }
        return ptr;
    }
}

// uint32_t vaddr_to_raddr(uint32_t vaddr, ProcessState *active_task) {
//     return (*active_task->page_dir)[vaddr >> (32-10)][(vaddr >> (32-20)) & 0x3FF] & ~(0b111);
// }

unsigned char *sbrk(size_t sz) {
    return ksbrk(sz, active_task);
}
// sbrk with alignment
unsigned char *align_sbrk(size_t sz, int align) {
    if (!paging_enabled) {
        unsigned char *ptr = active_task->brk_ptr;
        uint32_t offset = align - ((uint32_t)ptr % align);
        if ((uint32_t)ptr % align > 0) {
            ptr = ptr + align - ((uint32_t)ptr % align);
        }
        active_task->brk_ptr += sz + offset;
        return ptr;
    } else {
        unsigned char *ptr = active_task->brk_ptr;
        uint32_t offset = align - ((uint32_t)ptr % align);
        if ((uint32_t)ptr % align > 0) {
            ptr = ptr + align - ((uint32_t)ptr % align);
        }
        active_task->brk_ptr += sz + offset;
        return ptr;
    }
}

void sbrk_test() {
    unsigned char *test = sbrk(0x1000);
    println("sbrk_test");
    print_int((uint32_t)test);
    print_nl();
    print_int((uint32_t)active_task->brk_ptr);
    print_nl();
    print_int(active_task->allocated);
    print_nl();
    for (int i = 0; i < 1024; i++) test = sbrk(0x1000);
    print_int((uint32_t)test);
    print_nl();
    print_int((uint32_t)active_task->brk_ptr);
    print_nl();
    print_int(active_task->allocated);
    print_nl();
}

// generates identity paging for the given list of addresses
void idpage(uint32_t addr, uint32_t size, uint32_t page_dir[]) {
    uint32_t first_entry = (addr & (0x3FF << 22)) >> 22;
    if (page_dir[first_entry] == EMPTY_TABLE) {
        page_dir[first_entry] = (uint32_t)get_next_page_table_ptr();
    }
    size = size / 0x1000;
    // only allow one page table worth
    if (size > 1024) {
        size = 1024;
    }
    addr = addr & (~0xFFF);
    uint32_t *page_table = (uint32_t *)page_dir[first_entry];
    for (uint32_t i = 0; i < size; i++) {
        page_table[i] = (addr + i * 0x1000) | 0b111;
    }
    page_dir[first_entry] = ((uint32_t)page_table) | 0b111;
}

void test_malloc() {
    kasserteq(active_task->current_block->sz, 0, "test_malloc: start block size != 0");
    kasserteq(active_task->current_block->free, 1, "test_malloc: start block is not free");
    kasserteq(active_task->current_block->allocated, 0,
              "test_malloc: start block is allocated");
    kasserteq((uint32_t)active_task->current_block, (uint32_t)active_task->block_start,
              "test_malloc: current block != start block");
    void *tmp = malloc(25);
    kassertneq((uint32_t)active_task->current_block, (uint32_t)active_task->block_start,
               "test_malloc: current block == start block");
    kasserteq((uint32_t)active_task->current_block, (uint32_t)active_task->block_start->next,
              "test_malloc: current block != start block");
    kasserteq(active_task->block_start->free, 0, "test_malloc: starting block is free");
    kasserteq(active_task->block_start->allocated, 1,
              "test_malloc: starting block not allocated");
    kasserteq(active_task->block_start->sz, 25, "test_malloc: block size !=  25");
    kasserteq(active_task->block_start->sz + sizeof *active_task->block_start,
              ((uint32_t)active_task->current_block) - ((uint32_t)active_task->block_start),
              "test_malloc: distance between blocks unexpected");
    void *tmp2 = malloc(25);
    free(tmp);
    kasserteq(active_task->current_block->free, 1, "test_malloc: freed block is not free");
    kasserteq(active_task->current_block->allocated, 1,
              "test_malloc: freed block is not allocated");
    kasserteq(active_task->current_block->sz, 25,
              "test_malloc: freed block is not the correct size");
    kasserteq(active_task->current_block->sz,
              ((uint32_t)tmp2) - ((uint32_t)tmp) - sizeof *active_task->current_block,
              "test_malloc: current block not in the correct location");
    kasserteq((uint32_t)active_task->current_block->next,
              ((uint32_t)tmp2) - sizeof *active_task->current_block,
              "test_malloc: freed block does not point to last block");
    kasserteq(((uint32_t)tmp) - sizeof *active_task->current_block, (uint32_t)active_task->current_block,
              "test_malloc: current block not in the correct location");
    void *tmp3 = malloc(25);
    kasserteq((uint32_t)tmp, (uint32_t)tmp3,
              "test_malloc: reallocation not working");
    free(tmp2);
    free(tmp3);
    kasserteq((uint32_t)active_task->current_block, (uint32_t)active_task->block_start,
              "test_malloc(clean): current block != start block");
}

void mem_init() {
    kernel_task->brk_ptr = get_heap_space();
    kernel_task->allocated = 1024 * 0x1000;
    kernel_task->page_dir = &kernel_page_dir;
    for (int i = 0; i < 1024; i++) {
        // This sets the following flags to the pages:
        // -Supervisor: Only kernel-mode can access them
        // -Write Enabled: It can be both read from and written to
        // -Not Present: The page table is not
        kernel_page_dir[i] = EMPTY_TABLE;
    }

    // Sets up identity page for 0x00000000-0x00000fff
    idpage(0x0, 1024 * 0x1000, kernel_page_dir);
    kasserteq(((uint32_t *)(kernel_page_dir[0] & ~0b111))[0], 0x0 | 0b111,
              "kernel page 0 not identity paged");
    idpage(1024 * 0x1000, 1024 * 0x1000, kernel_page_dir);
    kasserteq(((uint32_t *)(kernel_page_dir[1] & ~0b111))[0],
              1024 * 0x1000 | 0b111, "kernel page 1 not identity paged");
    idpage(1024 * 0x1000 * 2, 1024 * 0x1000, kernel_page_dir);

    // Maps kernel to 0xC0000000
    map_page_tables(kernel_page_dir, KERNEL_START_ADDR, (uint32_t)get_kernel_start(),
                    0x400000);
    kasserteq(((uint32_t *)kernel_page_dir[768])[0],
              (uint32_t)get_kernel_start() | 0b111, "kernel page map failed");

    init_buddy_alloc();

    load_page_directory((unsigned int *)kernel_page_dir);
    enable_paging();
    paging_enabled = 1;

    active_task->block_start = (BlockInfo *)sbrk(sizeof *active_task->block_start);
    active_task->current_block = active_task->block_start;
    active_task->current_block->sz = 0;
    active_task->current_block->next = (BlockInfo *)sbrk(0);
    active_task->current_block->free = 1;
    active_task->current_block->allocated = 0;
    test_malloc();
}


/*
  general memory allocation
*/
void *kmalloc(size_t size, ProcessState *task) {
    void *ptr = 0;
    while (!task->current_block->free && task->current_block->next) {
        task->current_block = task->current_block->next;
    }
    while (ptr == 0) {
        if (!task->current_block->allocated) {
            if (task->current_block->next != (BlockInfo *)sbrk(0)) {
                task->current_block->next = (BlockInfo *)sbrk(
                    sizeof *task->block_start);  // sbrk has been moved, but
                                           // we didn't know about it
                task->current_block = task->current_block->next;
                task->current_block->sz = 0;
                task->current_block->next = (BlockInfo *)sbrk(0);
                task->current_block->free = 1;
                task->current_block->allocated = 0;
            }

            task->current_block->sz = size;
            ptr = (void *)sbrk(size);
            task->current_block->next = (BlockInfo *)sbrk(sizeof *task->block_start);
            task->current_block->free = 0;
            task->current_block->allocated = 1;
            task->current_block = task->current_block->next;
            task->current_block->sz = 0;
            task->current_block->next = (BlockInfo *)sbrk(0);
            task->current_block->free = 1;
            task->current_block->allocated = 0;
            break;
        } else {  // we have allocated this block
            if (size <= task->current_block->sz - sizeof *task->current_block) {
                task->current_block->sz = size;
                BlockInfo *next =
                    (BlockInfo *)(((uint32_t)task->current_block) +
                                  task->current_block->sz + sizeof *task->current_block);
                next->next = task->current_block->next;
                next->sz = task->current_block->sz - size - sizeof *next;
                next->free = 1;
                next->allocated = 1;
                ptr = (BlockInfo *)(((uint32_t)task->current_block) +
                                    sizeof *task->current_block);
                task->current_block->next = next;
                task->current_block->free = 0;
                task->current_block->allocated = 1;
                task->current_block = task->current_block->next;
                break;
            } else if (size <= task->current_block->sz) {
                task->current_block->sz = size;
                task->current_block->free = 0;
                ptr = (BlockInfo *)(((uint32_t)task->current_block) +
                                    sizeof *task->current_block);
                task->current_block = task->current_block->next;
                break;
            }
        }
        task->current_block = task->current_block->next;
    }
    return ptr;
}

/**
 * memory allocation for the current task
*/
void *malloc(size_t size){
    return kmalloc(size, active_task);
}

void free(void *ptr) {
    BlockInfo *block =
        (BlockInfo *)(((uint32_t)ptr) - ((uint32_t)sizeof(BlockInfo)));

    kasserteq(block->free, 0, "Attempted to free block that's already free");
    kassertneq(block->allocated, 0, "Attempted to free unallocated block");

    block->free = 1;
    if (block <= active_task->current_block) {  // TODO: make sure this block is in the chain
        active_task->current_block = block;
    }
}

void memset(void *vbuf, char s, uint32_t sz) {
    char *buf = (char *)vbuf;
    for (uint32_t i = 0; i < sz; i++) {
        buf[i] = s;
    }
}
