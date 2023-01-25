#define KBRD_DATA 0x60
#define KBRD_STAT 0x64

#define ENTER_KEY 0x1C
#define BACKSPACE 0x0E

#include "display.h"

typedef struct IOStream{
  int sz;
  int idx_start;
  int idx_end;
  char buf[];
} IOStream;

/* keyboard functions */
void kb_init(void);

void keyboard_handler_main(void);

IOStream *init_io_stream(int sz);

char readch(IOStream *stream);

void writech(IOStream *stream, char ch);

