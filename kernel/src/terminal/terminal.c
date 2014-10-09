#include "../../inc/terminal.h"

int term_col=0;
int term_row=0;


void terminal_init() {
	int n;
	for (n=0;n<(SCREEN_COLS*SCREEN_ROWS);n+=2) {
		*(SCREEN_BUFFER+n)=0;
		*(SCREEN_BUFFER+n+1)=10;
	}
}

void terminal_printString(char* string) {
	terminal_printf(string);
}

void terminal_printInt(int n) {
	int m;
	int i;
	int n1=n;
	char digit[15];
	if (n==0) {terminal_printf("0"); return;}
	for (m=0;m<15;m++) digit[m]=0;
	if(n<0) n1=-n;
	i=0;
	while(n1>0) {
		m=n1%10;
		digit[13-i]=m+48;
		i++;
		n1=n1/10;
	}
	if (n<0) digit[13-i]='-';

	for (i=0;i<15;i++) {
		if (digit[i]!=0) {
			terminal_printf(digit+i);
			return;
		}
	}
}

void terminal_printUInt(unsigned int n) {
	unsigned int m;
	unsigned int i;
	char digit[15];
	if (n==0) {terminal_printf("0"); return;}
	for (m=0;m<15;m++) digit[m]=0;
	i=0;
	while(n>0) {
		m=n%10;
		digit[13-i]=m+48;
		i++;
		n=n/10;
	}
	for (i=0;i<15;i++) {
		if (digit[i]!=0) {
			terminal_printf(digit+i);
			return;
		}
	}
}

void terminal_printf(const char* string,...) {
	int n=0;
	int desp;
	int i;
	char d=*(string+n);
	va_list arguments;
	va_start(arguments,string);

	while (d!=0){
		switch (d) {
			case '%':
				switch (*(string+n+1)) {
					case 'd': 
						terminal_printInt(va_arg(arguments,int));
						n++;
						break;
					case 'u': 
						terminal_printUInt(va_arg(arguments,unsigned int));
						n++;
						break;
					case 's': 
						terminal_printString(va_arg(arguments,char* ));
						n++;
						break;
				}
				break;
			case 10: term_col=0;term_row++;break;
			default: 
				desp=((term_row*SCREEN_COLS)+term_col)*2;
				*(SCREEN_BUFFER+desp)=d;
				if (term_col<SCREEN_COLS-1) term_col++;
				else {term_col=0;term_row++;}

		}
		if (term_row>SCREEN_ROWS-1) {
			for (i=0;i<SCREEN_COLS*(SCREEN_ROWS-1)*2;i++) 
				*(SCREEN_BUFFER+i)=*(SCREEN_BUFFER+i+(SCREEN_COLS*2));

			desp = ((SCREEN_ROWS-1)*SCREEN_COLS*2);
			for (i=0;i<SCREEN_COLS;i=i+2) { 
				*(SCREEN_BUFFER+desp+i)=0;
				*(SCREEN_BUFFER+desp+i+1)=10;
			}
			term_row--;
		}
		n++;
		d=*(string+n);
	}
	va_end(arguments);

}

int str_equal(char* str1, char* str2 ,unsigned int n) {
	for (unsigned  i=0;i<n;i++) {
		if (*(str1+i)!=(*(str2+i))) return 0;
	}
	return 1;
}
