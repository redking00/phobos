#include "../../inc/ata.h"
#include "../../inc/terminal.h"
#include "../../inc/filesystem.h"
#include "../../inc/disk.h"

MBR mbr;
BOOTSECTOR bootsector;
DIRECTORYSECTOR dirsector;

DISK disks[4];


int32_t ide_read(uint32_t busnumber, uint32_t secnum,uint16_t nsecs, void* buffer) {
    ata_lba_read(busnumber,secnum,(uint32_t)nsecs,(uint32_t)buffer);
    return nsecs;
}

uint32_t disk_autodetect() {
    uint32_t ndisk = 0;
    ide_read(1,0,1,&mbr);
    for (int n=0;n<4;n++) {
        if ((mbr.part[n].total_sectors>0)
                &&(mbr.part[n].total_sectors!=mbr.part[n].first_sector)) {
            ide_read(1,mbr.part[n].first_sector,1,&bootsector);
            if (str_equal((TEXTPOINTER)"PHOBOSFS",bootsector.fsid,8)) {
                terminal_printf((TEXTPOINTER)"IDE 1 Partition %u [%u MB] VolumeID: %s\n",
                        (n+1),(mbr.part[n].total_sectors*512)/0x100000,bootsector.volumeid);
                disks[ndisk].status = DISK_STATUS_READY;
                disks[ndisk].bustype = BUS_TYPE_IDE;
                disks[ndisk].busnumber=1;
                disks[ndisk].partition_number = n;
                disks[ndisk].first_sector = mbr.part[n].first_sector;
                disks[ndisk].total_sectors = mbr.part[n].total_sectors;
                ndisk++;
            }
        }
    }
    return ndisk;
}


int32_t disk_read(DISK* disk,uint32_t sectornumber, uint16_t nsectors, void* buffer) {
    if (disk->status==DISK_STATUS_READY) {
        if (disk->bustype==BUS_TYPE_IDE) {
            return ide_read(disk->busnumber,disk->first_sector+sectornumber,nsectors,buffer);
        }
        else return -DISK_STATUS_UNKNOWN;
    }
    else return -disk->status;
}

int32_t file_open(TEXTPOINTER path, uint32_t mode) {
    return -1;
}

void disk_init() {
    terminal_printf((TEXTPOINTER)"Detecting disks...\n");
    for (int n=0;n<4;n++) disks[n].status = DISK_STATUS_DISABLED;
    if (!disk_autodetect()) {
        terminal_printf((TEXTPOINTER)"No disks detected\n");
    }
}
