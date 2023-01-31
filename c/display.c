#include "display.h"
#include "kio.h"
#include "stdlib.h"

unsigned int curr = 0;
char *vidptr = (char*)0xb8000;
extern IOStream *stdout;

/* display functions */
void clear(void){
    unsigned int j = 0;
    while(j < LINES*CLMN_IN_LN*BYTES_PER_ELM){
	vidptr[j] = ' ';
	vidptr[j+1] = 0x07;
	j = j + 2;
    }
    curr = 0;
}


void shiftup(int lines){
    int offset = BYTES_PER_ELM * CLMN_IN_LN * lines;
    unsigned int j = 0;
    while(j < LINES*CLMN_IN_LN*BYTES_PER_ELM - offset){
	vidptr[j] = vidptr[j+offset];
	vidptr[j+1] = vidptr[j+1+offset];
	j = j + 2;
    }
    while(j < LINES*CLMN_IN_LN*BYTES_PER_ELM){
	vidptr[j] = ' ';
	vidptr[j+1] = 0x0;
	j = j + 2;
    }
    curr -= offset;
}

void kputc(char c){
    writech(stdout, c);
}


void print(const char *str){
    unsigned int j = 0;
    while(str[j] != '\0'){
	kputc(str[j]);
	++j;
    }
}
void println(const char *str){
    print(str);
    print_nl();
}
void print_int(int num){
    int count = 0;
    int temp = num;
    do{
	temp = temp / 10;
	count+=1;
    } while(temp > 0);
    char *str = malloc(count+1);
    str[count] = '\0';
    while(count > 0){
	count--;
	char dig = (num % 10)+'0';
	str[count] = dig;
	num = num / 10;
    };
    print(str);
    free(str);
}

void println_int(int num){
    print_int(num);
    print_nl();
}
void print_nl(void){
    writech(stdout, '\n');
}

void itos(int num, char *str, int size){
    int count = 0;
    int temp = num;
    do{
	temp = temp / 10;
	count+=1;
    } while(temp > 0);
    if(count >= size)
	count = size-1;
    else
	str[count+1] = '\0';
    while(count > 0){
	count--;
	char dig = (num % 10)+'0';
	str[count] = dig;
	num = num / 10;
    }
    
}
void backspace(void){
    curr = curr - 2;
    vidptr[curr] = '\0';
    vidptr[curr + 1] = 0;
}
void handle_stdout(void){
    char c;
    static unsigned int line_size = BYTES_PER_ELM * CLMN_IN_LN;
    while((c = readch(stdout))){
	if(c == '\n'){
	    curr = curr + (line_size - curr % (line_size));
	    if (curr > LINES*CLMN_IN_LN*BYTES_PER_ELM){
		shiftup(1);
	    }
	    continue;
	}
	if (curr + 2 > LINES*CLMN_IN_LN*BYTES_PER_ELM){
	    shiftup(1);
	}

	vidptr[curr++] = c;
	vidptr[curr++] = 0x07;

    }
}
