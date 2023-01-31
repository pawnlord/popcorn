

#define IDT_SIZE 256

#include "stdlib.h"
#include "kio.h"
#include "display.h"
#include "keyboard_map.h"

extern void keyboard_handler(void);
extern void error_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned short data);
extern void load_idt(unsigned long *idt_ptr);
extern unsigned char *get_heap_space(void);
extern unsigned char *get_stack_space(void);
extern IOStream *stdin;
extern char press_flag;
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

void idt_init(void){
    unsigned long keyboard_addr;
    unsigned long idt_addr;
    unsigned long idt_ptr[2];

    keyboard_addr = (unsigned long)keyboard_handler;
    IDT[0x21].offset_low = keyboard_addr & 0xFFFF;
    IDT[0x21].selector = 0x08;
    IDT[0x21].zero = 0;
    IDT[0x21].type_attr = 0x8e;
    IDT[0x21].offset_high = (keyboard_addr & 0xFFFF0000) >> 16;


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

void handle_io(){
    char keycode = readch(stdin);

    if(keycode == 0)
	return;
    if(keycode <= 0)
	return;
    if(keycode == ENTER_KEY){
	print_nl();
	return;
    }
    if(keycode == BACKSPACE){
	backspace();
	return;
    }
    kputc(keyboard_map[(unsigned char) keycode]);
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


    while(1){
	if(press_flag != local_press_flag){
	    handle_io();
	    local_press_flag = press_flag;
	}
	handle_stdout();
    }
    return;
}
