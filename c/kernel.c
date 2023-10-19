

#define IDT_SIZE 256

#include "display.h"
#include "kio.h"
#include "ksh.h"
#include "kstdlib.h"
#include "stdlib.h"
#include "string.h"
#include "task.h"
extern void keyboard_handler(void);
extern void out_handler(void);
extern void error_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned short data);
extern void load_idt(unsigned long *idt_ptr);
extern void load_gdt(unsigned long *gdt_ptr);
extern void load_tss(unsigned short tss_descriptor);
extern unsigned char *get_heap_space(void);
extern unsigned char *get_stack_space(void);
extern unsigned char *get_stack_ptr(void);
extern IOStream *stdin;
extern char press_flag;
extern void disable_ints(void);
extern void enable_ints(void);
extern void jump_usermode(void (*fp)(void));
extern void test_user_function_fail(void);
extern void print_esp(void);
extern uint32_t get_esp(void);
extern ProtectedTSS tss;
void test_user_function_success(void) { println("Hello!!"); }
char local_press_flag;

/* IDT/interrupt functions */
struct IDT_entry {
    unsigned short int offset_low;
    unsigned short int selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_high;
};
struct IDT_entry IDT[IDT_SIZE];

void error_handler_main(uint32_t ercode) {
    write_port(0x20, 0x20);
    println("Exception!");
    print("ercode: ");
    println_int(ercode);
    while (1)
        ;
    // char temp = press_flag;
    // while(temp == press_flag){} // TODO: Move to IO, make it work
    return;
}

void add_idt_entry(unsigned long handler_addr, int entry_num,
                   unsigned short int selector, unsigned char attr) {
    IDT[entry_num].offset_low = handler_addr & 0xFFFF;
    IDT[entry_num].selector = selector;
    IDT[entry_num].zero = 0;
    IDT[entry_num].type_attr = attr;
    IDT[entry_num].offset_high = (handler_addr & 0xFFFF0000) >> 16;
}

void idt_init(void) {
    unsigned long keyboard_addr, out_addr;
    unsigned long idt_addr;
    unsigned long idt_ptr[2];
    disable_ints();
    for (int i = 0; i < 32; i++) {
        // STUPID HACK ALERT !!!!!
        add_idt_entry((unsigned long)(error_handler) + 4 * i, i, 0x08, 0x8F);
    }
    add_idt_entry((unsigned long)keyboard_handler, 0x21, 0x08, 0x8e);
    add_idt_entry((unsigned long)out_handler, 0x80, 0x08, 0xEE);

    /*     Ports
     *	         PIC1	PIC2
     *Command    0x20	0xA0
     *Data	 0x21	0xA1 */

    write_port(0x20, 0x11);
    write_port(0xA0, 0x11);

    write_port(0x21, 0x20);
    write_port(0xA1, 0x28);

    write_port(0x21, 0x00);
    write_port(0xA1, 0x00);

    write_port(0x21, 0x01);
    write_port(0xA1, 0x01);

    write_port(0x21, 0xFF);
    write_port(0xA1, 0xFF);  // Turn PIC off

    idt_addr = (unsigned long)IDT;
    idt_ptr[0] =
        (sizeof(struct IDT_entry) * IDT_SIZE) + ((idt_addr & 0xFFFF) << 16);
    idt_ptr[1] = idt_addr >> 16;

    load_idt(idt_ptr);
    enable_ints();
}

void handle_io() {
    char c = readch(stdin);

    if (c == 0) return;
    if (c <= 0) return;
    if (c == '\n') {
        print_nl();
        return;
    }
    if (c == '\b') {
        backspace();
        return;
    }
    kputc(c);
}

void kmain(void) {
    const char *str = "popcorn colonel V1.0.0\0";
    init_task_manager();
    
    clear();
    mem_init();
    idt_init();
    kb_init();
    int esp0 = (int)malloc(10000);
    // println_int(esp0);
    gdt_init(esp0);

    local_press_flag = press_flag;
    // print_esp();

    // End init
    const char *str2 = "\x1b[011000mHello! V1.0.0\0";

    // print(str);
    // print_nl();
    schedule_task(create_new_task(0));
    print(str2);
    handle_task_switch();
    jump_usermode(test_user_function_success);
    sh();
    // while(1){}
    /* while(1){ */
    /* 	if(press_flag != local_press_flag){ */
    /* 	    handle_io(); */
    /* 	    local_press_flag = press_flag; */
    /* 	} */
    /* 	handle_stdout(); */
    /* } */
    return;
}
