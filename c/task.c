#include "task.h"

#include "display.h"
#define GDT_SIZE 6

int next_pid = 1;
int current_gdt_entry = 6;
ProtectedTSS tss;  // TSS for first CPU
Task tasks[MAX_TASKS];
Task *kernel_task = &tasks[0];
Task *current_task;
struct GDT_Entry GDT[GDT_SIZE];

extern void load_idt(unsigned long *idt_ptr);
extern void load_gdt(unsigned long *gdt_ptr);
extern void load_tss(unsigned short tss_descriptor);
extern void set_current_esp0(void);
extern void print_esp(void);

extern unsigned char *get_kernel_start(void);

extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned short data);

extern void disable_ints(void);
extern void enable_ints(void);
extern void reload_segments(void);

extern char press_flag;

extern unsigned char *get_esp(void);

Task *get_task(void) { return &tasks[next_pid]; }

uint32_t gdt_get_base(GDT_Entry gdt_entry) {
    return gdt_entry.base1 + (((uint32_t)gdt_entry.base2) << 16) +
           (((uint32_t)gdt_entry.base3) << 24);
}
uint32_t gdt_get_limit(GDT_Entry gdt_entry) {
    return (gdt_entry.limit1) +
           (((uint32_t)(gdt_entry.limit2flags & 0b1111)) << 16);
}
char gdt_get_flags(GDT_Entry gdt_entry) {
    return (gdt_entry.limit2flags & 0b11110000) >> 4;
}

void gdt_set_base(GDT_Entry *gdt_entry, uint32_t base) {
    gdt_entry->base1 = base & 0x0000FFFF;
    gdt_entry->base2 = ((base & 0x00FF0000) >> (8 * 2));
    gdt_entry->base3 = ((base & 0xFF000000) >> (8 * 3));
}
void gdt_set_limit(GDT_Entry *gdt_entry, uint32_t limit) {
    gdt_entry->limit1 = limit & 0x0FFFF;
    gdt_entry->limit2flags &= 0xF0;
    gdt_entry->limit2flags |= ((limit & 0xF0000) >> (16));
}
void gdt_set_flags(GDT_Entry *gdt_entry, uint32_t flags) {
    gdt_entry->limit2flags &= 0x0F;
    gdt_entry->limit2flags |= (flags << 4);
}

void gdt_init(uint32_t esp0) {
    uint32_t gdt_ptr[2];
    kasserteq(sizeof(GDT_Entry), 8, "Size of GDT Entry");
    disable_ints();
    gdt_set_base(&GDT[0], 0);
    gdt_set_limit(&GDT[0], 0);
    GDT[0].access = 0x0;
    gdt_set_flags(&GDT[0], 0);

    gdt_set_base(&GDT[1], 0);
    gdt_set_limit(&GDT[1], 0xFFFFF);
    GDT[1].access = 0x9A;
    gdt_set_flags(&GDT[1], 0xC);

    gdt_set_base(&GDT[2], 0);
    gdt_set_limit(&GDT[2], 0xFFFFF);
    GDT[2].access = 0x92;
    gdt_set_flags(&GDT[2], 0xC);

    gdt_set_base(&GDT[3], 0);
    gdt_set_limit(&GDT[3], 0xFFFFF);
    GDT[3].access = 0xFA;
    gdt_set_flags(&GDT[3], 0xC);

    gdt_set_base(&GDT[4], 0);
    gdt_set_limit(&GDT[4], 0xFFFFF);
    GDT[4].access = 0xF2;
    gdt_set_flags(&GDT[4], 0xC);

    memset(&tss, 0, sizeof(tss));
    tss.SS0 = KERNEL_DS;
    tss.ESP0 = esp0;
    println("esp values");
    println_int(tss.ESP0);
    print_esp();
    gdt_set_base(&GDT[5], (uint32_t)&tss);
    gdt_set_limit(&GDT[5], sizeof(tss));
    GDT[5].access = 0x89;
    gdt_set_flags(&GDT[5], 0x0);

    unsigned long gdt_addr = (unsigned long)GDT;
    gdt_ptr[0] = (sizeof(GDT_Entry) * GDT_SIZE) + ((gdt_addr & 0xFFFF) << 16);
    gdt_ptr[1] = gdt_addr >> 16;

    load_gdt(gdt_ptr);
    load_tss(0x2B);

    enable_ints();
}
