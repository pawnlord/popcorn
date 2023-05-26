#define KERNEL_DS 0x10
#define KERNEL_CS 0x8
#define USER_DS 0x20
#define USER_CS 0x18

#define MAX_TASKS 255

#include "stdlib.h"
#include "string.h"

typedef struct GDT_Entry{
  unsigned short limit1;
  unsigned short base1;
  unsigned char base2;
  unsigned char access;
  unsigned char limit2flags;
  unsigned char base3;
} GDT_Entry;


typedef struct ProtectedTSS {
    unsigned short link;
    unsigned short res1; // cannot be touched
    uint32_t ESP0;
    unsigned short SS0;
    unsigned short res2; // cannot be touched
    uint32_t ESP1;
    unsigned short SS1;
    unsigned short res3; // cannot be touched
    uint32_t ESP2;
    unsigned short SS2;
    unsigned short res4; // cannot be touched
    uint32_t ESP3;
    uint32_t CR3;
    uint32_t EIP;
    uint32_t EFLAGS;
    uint32_t EAX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t EBX;
    uint32_t ESP;
    uint32_t EBP;
    uint32_t ESI;
    uint32_t EDI;
    unsigned short ES;
    unsigned short res5; // cannot be touched
    unsigned short CS;
    unsigned short res6; // cannot be touched
    unsigned short SS;
    unsigned short res7; // cannot be touched
    unsigned short DS;
    unsigned short res8; // cannot be touched
    unsigned short FS;
    unsigned short res9; // cannot be touched
    unsigned short GS;
    unsigned short res10; // cannot be touched
    unsigned short LDTR;
    unsigned short res11; // cannot be touched
    unsigned short res12; // cannot be touched
    unsigned short IOPB;    
    uint32_t SSP;
    unsigned long io_bitmap[33];
} ProtectedTSS;



typedef struct PushedRegs{
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t eflags;
} PushedRegs;

typedef enum TaskState{
  TASK_STOPPED,
  TASK_RUNNING
} TaskState;
typedef struct Task {
  uint32_t num;
  uint32_t pid;
  uint32_t *page_dir;
  uint32_t ESP; // esp for task
  uint32_t ESP0; // esp for privilege escalation
  TaskState state;
  struct Task *next_task;
} Task;

void init_multitasking();
void init_task(Task *task);

uint32_t gdt_get_base(GDT_Entry gdt_entry);
uint32_t gdt_get_limit(GDT_Entry gdt_entry);
char gdt_get_flags(GDT_Entry gdt_entry);

void gdt_set_base(GDT_Entry *gdt_entry, uint32_t base);
void gdt_set_limit(GDT_Entry *gdt_entry, uint32_t limit);
void gdt_set_flags(GDT_Entry *gdt_entry, uint32_t flags);

void gdt_init(uint32_t esp0);

Task *get_task(void);
