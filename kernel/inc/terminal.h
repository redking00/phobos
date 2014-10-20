#include <stdint.h>
#include <stdarg.h>

#define ASCIISTRING uint8_t*

#define SCREEN_BUFFER ((ASCIISTRING) 0xB8000)
#define SCREEN_COLS 80
#define SCREEN_ROWS 25
#define TERMINAL_DEFAULT_STYLE ((uint8_t)10)


void terminal_init();

void terminal_printf(const ASCIISTRING,...);

uint16_t str_equal(ASCIISTRING,ASCIISTRING,uint16_t);

void strcpy(ASCIISTRING, ASCIISTRING);

uint32_t strlen(ASCIISTRING);