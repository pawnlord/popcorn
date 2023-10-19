#include "display.h"

#include "kio.h"
#include "stdlib.h"

char *vidptr = (char *)0xb8000;
extern IOStream *stdout;
extern void display_int(void);
extern void write_port(unsigned short port, unsigned short data);

/* display functions */
void clear(void) {
    unsigned int j = 0;
    while (j < LINES * CLMN_IN_LN * BYTES_PER_ELM) {
        vidptr[j] = ' ';
        vidptr[j + 1] = 0x07;
        j = j + 2;
    }
    get_task()->curr = 0;
}

void shiftup(int lines) {
    int offset = BYTES_PER_ELM * CLMN_IN_LN * lines;
    unsigned int j = 0;
    while (j < LINES * CLMN_IN_LN * BYTES_PER_ELM - offset) {
        vidptr[j] = vidptr[j + offset];
        vidptr[j + 1] = vidptr[j + 1 + offset];
        j = j + 2;
    }
    while (j < LINES * CLMN_IN_LN * BYTES_PER_ELM) {
        vidptr[j] = ' ';
        vidptr[j + 1] = 0x0;
        j = j + 2;
    }
    get_task()->curr -= offset;
}

void kputc(char c) {
    static int is_in_escape = 0;
    writech(stdout, c);
    if(c == '\x1b'){
        is_in_escape = 1;
    }
    if(c == 'm' || c == '\n'){
        is_in_escape = 0;
    }
    if(!is_in_escape){
        handle_stdout();
    }
}

void print(const char *str) {
    unsigned int j = 0;
    while (str[j] != '\0') {
        kputc(str[j]);
        ++j;
    }
}
void println(const char *str) {
    print(str);
    print_nl();
}
void print_int(int num) {
    int count = 0;
    int temp = num;
    do {
        temp = temp / 10;
        count += 1;
    } while (temp > 0);
    char *str = malloc(count + 1);
    str[count] = '\0';
    while (count > 0) {
        count--;
        char dig = (num % 10) + '0';
        str[count] = dig;
        num = num / 10;
    }
    print(str);
    free(str);
}

void println_int(int num) {
    print_int(num);
    print_nl();
}
void print_nl(void) { writech(stdout, '\n'); }

void itos(int num, char *str, int size) {
    int count = 0;
    int temp = num;
    do {
        temp = temp / 10;
        count += 1;
    } while (temp > 0);
    if (count >= size)
        count = size - 1;
    else
        str[count + 1] = '\0';
    while (count > 0) {
        count--;
        char dig = (num % 10) + '0';
        str[count] = dig;
        num = num / 10;
    }
}


int stoi(char *str, int size) {
    int count = 0;
    int num = 0;
    for(int i = 0; i < size; i++){
        int mult = 1;
        for(int j = 0; j < size - i - 1; j++){
            mult *= 10;
        }
        num += (str[i] - 0x30) * mult;
    }
    return num;
}
void backspace(void) {
    get_task()->curr = get_task()->curr - 2;
    vidptr[get_task()->curr] = '\0';
    vidptr[get_task()->curr + 1] = 0;
}

void handle_stdout(void) {
    char c;
    write_port(0x20, 0x20);

    static unsigned int line_size = BYTES_PER_ELM * CLMN_IN_LN;
    while ((c = readch(stdout))) {
        if (c == '\n') {
            get_task()->curr = get_task()->curr + (line_size - get_task()->curr % (line_size));
            if (get_task()->curr > LINES * CLMN_IN_LN * BYTES_PER_ELM) {
                shiftup(1);
            }
            continue;
        }
        if (get_task()->curr + 2 > LINES * CLMN_IN_LN * BYTES_PER_ELM) {
            shiftup(1);
        }
        if(c == '\x1b' && readch(stdout) == '[') {
            char *code = (char*)malloc(10);
            int i = 0;
            while((c = readch(stdout)) && c != 'm' && i < 10){
                code[i] = c;
                i++;
            }
            if(i >= 2){
                int num = stoi(code, 2);
                int newX, newY;
                switch (num)
                {
                case 0:
                    clear();
                    break;
                case 1:
                    newX = stoi(code+2, 2);
                    newY = stoi(code+4, 2);
                    get_task()->curr = (CLMN_IN_LN * BYTES_PER_ELM) * newY + (newX * BYTES_PER_ELM);
                    break;
                default:
                    break;
                }
            }
            continue;
        }

        vidptr[get_task()->curr++] = c;
        vidptr[get_task()->curr++] = 0x07;
    }
}
