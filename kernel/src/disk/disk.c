#include "../../inc/disk.h"
#include "../../inc/terminal.h"

void init_disk() {
    ata_lba_read(0,1,ADDR_DISK_MBR);
    mbrdisk1=(MBR*)(ADDR_DISK_MBR);
    terminal_printf("IDE 1 Partition1 -> [%u,%u] %u MB\n",mbrdisk1->part1.first_sector,mbrdisk1->part1.total_sectors,(mbrdisk1->part1.total_sectors*512)/0x100000);
    terminal_printf("IDE 1 Partition2 -> [%u,%u] %u MB\n",mbrdisk1->part2.first_sector,mbrdisk1->part2.total_sectors,(mbrdisk1->part2.total_sectors*512)/0x100000);
}
