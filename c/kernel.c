

#define IDT_SIZE 256
#define GDT_SIZE 6

#include "stdlib.h"
#include "kio.h"
#include "display.h"
#include "ksh.h"
extern void keyboard_handler(void);
extern void out_handler(void);
extern void error_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned short data);
extern void load_idt(unsigned long *idt_ptr);
extern void load_gdt(unsigned long *gdt_ptr);
extern void load_tss(unsigned char tss_descriptor);
extern unsigned char *get_heap_space(void);
extern unsigned char *get_stack_space(void);
extern IOStream *stdin;
extern char press_flag;
extern void disable_ints(void);
extern void enable_ints(void);
char local_press_flag;

/* IDT/interrupt functions */
struct IDT_entry{
    unsigned short int offset_low;
    unsigned short int selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_high;
};
struct IDT_entry IDT[IDT_SIZE];

void add_idt_entry(unsigned long handler_addr, int entry_num, unsigned short int selector, unsigned char attr){
    IDT[entry_num].offset_low = handler_addr & 0xFFFF;
    IDT[entry_num].selector = selector;
    IDT[entry_num].zero = 0;
    IDT[entry_num].type_attr = attr;
    IDT[entry_num].offset_high = (handler_addr & 0xFFFF0000) >> 16;
    
}

void idt_init(void){
    unsigned long keyboard_addr, out_addr;
    unsigned long idt_addr;
    unsigned long idt_ptr[2];

    add_idt_entry((unsigned long)keyboard_handler, 0x21, 0x08, 0x8e);
    add_idt_entry((unsigned long)out_handler, 0x80, 0x08, 0x8e);


    /*     Ports
     *	         PIC1	PIC2
     *Command    0x20	0xA0
     *Data	 0x21	0xA1 */
	
    write_port(0x20, 0x11);
    write_port(0xA0, 0x11);

	
    write_port (0x21, 0x20);
    write_port (0xA1, 0x28);

	
    write_port (0x21, 0x00);
    write_port (0xA1, 0x00);
	
    write_port (0x21, 0x01);
    write_port (0xA1, 0x01);

	
    write_port (0x21, 0xFF);
    write_port (0xA1, 0xFF); // Turn PIC off

    idt_addr = (unsigned long) IDT;
    idt_ptr[0] = (sizeof(struct IDT_entry) * IDT_SIZE) + ((idt_addr & 0xFFFF) << 16);
    idt_ptr[1] = idt_addr >> 16;

    load_idt(idt_ptr);
}

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
} ProtectedTSS;

ProtectedTSS TSS;
struct GDT_Entry GDT[GDT_SIZE];

void init_gdt(){
    unsigned long gdt_ptr[2];
    kasserteq(sizeof(GDT_Entry), 8, "Size of GDT Entry");
    disable_ints();
    gdt_set_base(&GDT[0], 0);
    gdt_set_limit(&GDT[0], 0);
    GDT[1].access = 0x0;
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
    
    gdt_set_base(&GDT[5], (uint32_t)&TSS);
    gdt_set_limit(&GDT[5], sizeof(TSS));
    GDT[5].access = 0x89;
    gdt_set_flags(&GDT[5], 0x40);

    unsigned long gdt_addr = (unsigned long) GDT;
    gdt_ptr[0] = (sizeof(struct IDT_entry) * GDT_SIZE) + ((gdt_addr & 0xFFFF) << 16);
    gdt_ptr[1] = gdt_addr >> 16;


    load_gdt(gdt_ptr);
    load_tss(0x28);
    enable_ints();
}

void handle_io(){
    char c = readch(stdin);

    if(c == 0)
	return;
    if(c <= 0)
	return;
    if(c == '\n'){
	print_nl();
	return;
    }
    if(c == '\b'){
	backspace();
	return;
    }
    kputc(c);
}


void kmain(void) {
    const char *str = "popcorn colonel V1.0.0\0";
    clear();
    mem_init();
    idt_init();
    kb_init();
    local_press_flag = press_flag;
    // End init

    print(str);
    print_nl();
    println_int(1);

    
    sh();
    /* while(1){ */
    /* 	if(press_flag != local_press_flag){ */
    /* 	    handle_io(); */
    /* 	    local_press_flag = press_flag; */
    /* 	} */
    /* 	handle_stdout(); */
    /* } */
    return;
}
