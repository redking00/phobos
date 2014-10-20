#include <stdint.h>
#include "filesystem.h"
#include "ata.h"

#pragma pack(1)

#define NULL                    0

#define BUS_TYPE_IDE            1

#define DISK_STATUS_DISABLED    0
#define DISK_STATUS_UNKNOWN     1
#define DISK_STATUS_BUSY        2
#define DISK_STATUS_READY       3

#define FILE_STATUS_CLOSE       0
#define FILE_STATUS_BUSY        1
#define FILE_STATUS_READY       2


typedef struct partition_t{
    uint8_t  trash[8];
    uint32_t first_sector;
    uint32_t total_sectors;
}PARTITION;

typedef struct mbr_t{
    uint8_t   code[446];
    PARTITION part[4];
    uint8_t   signature[2];
}MBR;

typedef struct disk_t {
    uint32_t status;
    uint32_t bustype;
    uint32_t busnumber;
    uint32_t partition_number;
    uint32_t first_sector;
    uint32_t total_sectors;
    BOOTSECTOR bootsector;
}DISK;

typedef struct filedescriptor_t {
    uint32_t status;
    uint8_t path[1024];
    uint32_t mode;
    uint32_t entry_sector;
    uint32_t entry_index;
    uint32_t first_sector;
    uint32_t current_sector;
    uint32_t current_position;
    uint32_t size;
}FILEDESCRIPTOR;


void disk_init();

int32_t file_open(TEXTPOINTER path,uint32_t mode);

void file_close(int32_t fileid);