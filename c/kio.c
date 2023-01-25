#include "kio.h"
#include "keyboard_map.h"

extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned short data);

char press_flag = 0; // alternates between 0 and 1 as keyboard is pressed

/* keyboard functions */
void kb_init(void){
    unsigned char val = (unsigned char)read_port(0x21);
    val &= ~2;
    write_port(0x21, val);
}

void keyboard_handler_main(void){
    unsigned char status;
    char keycode;
    write_port(0x20, 0x20);

    status = read_port(KBRD_STAT);

    if(status & 0x01){
	keycode = read_port(KBRD_DATA);
	if(keycode < 0)
	    return;
	press_flag = !press_flag;
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
}
