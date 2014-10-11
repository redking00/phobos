#include <stdint.h>

#pragma pack(1)

#define ADDR_DISK_MBR   (uint32_t)0x200000
#define ADDR_DISK_DAT_0 (uint32_t)0x200000+512


typedef struct partition_t{
    uint8_t  trash[8];
    uint32_t first_sector;
    uint32_t total_sectors;
}PARTITION;

typedef struct mbr_t{
    uint8_t   code[446];
    PARTITION part1;
    PARTITION part2;
    PARTITION part3;
    PARTITION part4;
    uint8_t   signature[2];
}MBR;


MBR* mbrdisk1;

extern void ata_lba_read(uint32_t address,uint32_t sectors,uint32_t buffer);

void init_disk();
