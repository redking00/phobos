#include <stdint.h>

#include "../../inc/main.h"
#include "../../inc/terminal.h"
#include "../../inc/disk.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_ALIAS "Hello"


//  00100000 - 0010FFFF API POINTERS
//  00110000 - 001FFFFF KERNEL
//  00200000 - FFFFFFFF USER

void export_api() {
    // INIT KERNEL API
    *(ADDR_TERMINAL_PRINTF) = (uint32_t) terminal_printf;
    *(ADDR_FILE_OPEN) = (uint32_t) file_open;
    *(ADDR_FILE_CLOSE) = (uint32_t) file_close;
}

void kernel_main()
{
    terminal_init();

    terminal_printf((TEXTPOINTER)"Phobos v%u.%u - %s\n",VERSION_MAJOR,VERSION_MINOR,VERSION_ALIAS);

    disk_init();
   
}
