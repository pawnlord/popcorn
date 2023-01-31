#define LINES 25
#define CLMN_IN_LN 80
#define BYTES_PER_ELM 2

#ifndef DISPLAY_H
#define DISPLAY_H


/* display functions */
void clear(void);
void kputc(char c);
void print(const char *str);
void print_int(int num);
void println(const char *str);
void println_int(int num);
void itos(int num, char *str, int size);
void print_nl(void);
void backspace(void);
#endif
