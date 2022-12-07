#include "display.h"

unsigned int curr = 0;
char *vidptr = (char*)0xb8000;

/* display functions */
void clear(void){
	unsigned int j = 0;
	while(j < LINES*CLMN_IN_LN*BYTES_PER_ELM){
		vidptr[j] = ' ';
		vidptr[j+1] = 0x07;
		j = j + 2;
	}
}

void kputc(char c){
       	vidptr[curr++] = c;
       	vidptr[curr++] = 0x07;
}

void print(const char *str){
	unsigned int j = 0;
	while(str[j] != '\0'){
		vidptr[curr] = str[j];
		vidptr[curr+1] = 0x07;
		curr=curr+2;
		++j;
	}
}
void print_int(int num){
	int count = 0;
	int temp = num;
	int temp_curr = curr;
	do{
		temp = temp / 10;
		count+=1;
	} while(temp > 0);
	curr += count*2;
	while(count > 0){
		count--;
		char dig = (num % 10)+'0';
		vidptr[temp_curr + (count * 2)] = dig;
		vidptr[temp_curr + (count * 2) + 1] = 0x07;
		num = num / 10;
	}
}
void print_nl(void)
{
	static unsigned int line_size = BYTES_PER_ELM * CLMN_IN_LN;
	curr = curr + (line_size - curr % (line_size));
}

void backspace(void){
  curr = curr - 2;
  vidptr[curr] = '\0';
  vidptr[curr + 1] = 0;
}