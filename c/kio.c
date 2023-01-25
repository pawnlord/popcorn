#include "kio.h"
#include "stdlib.h"

extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned short data);

char press_flag = 0; // alternates between 0 and 1 as keyboard is pressed

IOStream *stdin;

void test_io_stream(void){
    writech(stdin, 'h');
    kasserteq(stdin->buf[1], 'h', "writech check 1");
    kasserteq(stdin->idx_start, 0, "writech check 2");
    kasserteq(stdin->idx_end, 1, "writech check 3");

    char ch = readch(stdin);

    kasserteq(ch, 'h', "readch check 1");
    kasserteq(stdin->idx_start, 1, "readch check 2");
    kasserteq(stdin->idx_end, 1, "readch check 3");
    
    ch = readch(stdin);

    kasserteq(ch, 0, "readch check 4");
    kasserteq(stdin->idx_start, 1, "readch check 5");
    kasserteq(stdin->idx_end, 1, "readch check 6");

}

/* keyboard functions */
void kb_init(void){
    stdin = init_io_stream(10);
    test_io_stream();
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
	press_flag = !press_flag;
    	writech(stdin, keycode);
    }
}



IOStream *init_io_stream(int sz){
    kasserteq(sz > 1, 1, "invalid size for stream");
    IOStream *stream = malloc(sizeof stream  + sz);
    stream->sz = sz;
    stream->idx_start = 0;
    stream->idx_end = 0;
    return stream;
}

char readch(IOStream *stream){
    if(stream->idx_start == stream->idx_end){
	return 0;
    }
    stream->idx_start += 1;
    stream->idx_start %= stream->sz;

    char ch = stream->buf[stream->idx_start];
    return ch;
}

void writech(IOStream *stream, char ch){
    stream->idx_end += 1;
    stream->idx_end %= stream->sz;
    if(stream->idx_end == stream->idx_start){
	stream->idx_start += 1;
	stream->idx_start %= stream->sz;
    }
    stream->buf[stream->idx_end] = ch;
}
