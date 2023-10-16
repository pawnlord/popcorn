#ifndef KSTDLIB_H
#define KSTDLIB_H
#include "stdint.h"



typedef uint32_t PageTable[1024];
typedef uint32_t PageDirectory[1024];

typedef struct BlockInfo {
    size_t sz;
    struct BlockInfo *next;
    char free;
    char allocated;
} BlockInfo;

typedef enum TaskState { TASK_KERNEL, TASK_STOPPED, TASK_RUNNING } TaskState;



typedef struct ProcessState {
    uint8_t processID;
    uint8_t parentID;
    PageDirectory *page_dir;
    unsigned char *brk_ptr;
    uint32_t allocated;
    uint32_t return_addr;
    uint32_t esp;
    TaskState state;
    struct ProcessState* next;  
    struct ProcessState* prev;  
} ProcessState;


PageDirectory *get_next_page_dir_ptr();

void map_page_tables(uint32_t page_dir[], uint32_t vaddr, uint32_t raddr, size_t len);
void idpage(uint32_t addr, uint32_t size, uint32_t page_dir[]);
unsigned char *ksbrk(size_t sz, ProcessState *process_state);

// Task functions 
ProcessState *create_new_task(uint8_t parent_id);
void schedule_task(ProcessState *task);


/*
  kernel level testing
 */
void kasserteq(uint32_t val1, uint32_t val2, const char *msg);
void kassertneq(uint32_t val1, uint32_t val2, const char *msg);

#endif
