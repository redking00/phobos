#include <stdint.h>
#include <stdarg.h>

#define SCREEN_BUFFER (uint8_t*)0XB8000
#define SCREEN_COLS 80
#define SCREEN_ROWS 25


void terminal_init();

void terminal_printf(const char* string,...);

int str_equal(char*,char*,unsigned int);
