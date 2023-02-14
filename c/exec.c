#include "stdlib.h"

uint32_t gdt_get_base(GDT_Entry gdt_entry){
  return gdt_entry.base1 + (((uint32_t)gdt_entry.base2)<<16) + (((uint32_t)gdt_entry.base3)<<24);
}
uint32_t gdt_get_limit(GDT_Entry gdt_entry){
    return (gdt_entry.limit1) + (((uint32_t)(gdt_entry.limit2flags & 0b1111))<<16);
}
char gdt_get_flags(GDT_Entry gdt_entry){
    return (gdt_entry.limit2flags & 0b11110000)>>4;
}


void gdt_set_base(GDT_Entry *gdt_entry, uint32_t base){
    gdt_entry->base1 = base & 0x0000FFFF;
    gdt_entry->base2 = base & 0x00FF0000;
    gdt_entry->base1 = base & 0xFF000000;
}
void gdt_set_limit(GDT_Entry *gdt_entry, uint32_t limit){
    gdt_entry->limit1 = limit & 0x0FFFF;
    gdt_entry->limit2flags &= 0xF0;
    gdt_entry->limit2flags |= limit & 0xF0000;
        
}
void gdt_set_flags(GDT_Entry *gdt_entry, uint32_t flags){
    gdt_entry->limit2flags &= 0x0F;
    gdt_entry->limit2flags |= flags << 16;
}

int execvp(const char *command, char *argv[]){
    return 0;
}
