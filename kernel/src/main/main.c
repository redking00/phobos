#include <stdint.h>

#include "../../inc/main.h"
#include "../../inc/terminal.h"
#include "../../inc/disk.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_ALIAS "Hello"


//	00100000 - 0010FFFF API POINTERS
//	00110000 - 001FFFFF KERNEL
//	00200000 - 0024FFFF DISK
//	00250000 - FFFFFFFF USER

void export_api() {
	// INIT KERNEL API
	*(ADDR_TERMINAL_PRINTF)=(int)terminal_printf;

}

void kernel_main()
{
	terminal_init();

	terminal_printf("Phobos v%u.%u - %s\n",VERSION_MAJOR,VERSION_MINOR,VERSION_ALIAS);

	init_disk();
}
