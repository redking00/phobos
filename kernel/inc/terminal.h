#include <stdint.h>
#include <stdarg.h>

#define TEXTPOINTER uint8_t*

#define SCREEN_BUFFER ((TEXTPOINTER) 0xB8000)
#define SCREEN_COLS 80
#define SCREEN_ROWS 25
#define TERMINAL_DEFAULT_STYLE ((uint8_t)10)


void terminal_init();

void terminal_printf(const TEXTPOINTER,...);

uint16_t str_equal(TEXTPOINTER,TEXTPOINTER,uint16_t);

void strcpy(TEXTPOINTER, TEXTPOINTER);

uint32_t strlen(TEXTPOINTER);