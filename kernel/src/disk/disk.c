#include "../../inc/disk.h"
#include "../../inc/terminal.h"
#include "../../inc/filesystem.h"

void init_disk() {

    ata_lba_read(0,1,ADDR_DISK_MBR);
    mbrdisk1=(MBR*)(ADDR_DISK_MBR);
    
    BOOTSECTOR* bootsector;
    
    for (int n=0;n<4;n++) {
        if ((mbrdisk1->part[n].total_sectors>0)
                &&(mbrdisk1->part[n].total_sectors!=mbrdisk1->part[n].first_sector)) {
            ata_lba_read(mbrdisk1->part[n].first_sector,1,ADDR_DISK_DAT_0);
            bootsector = (BOOTSECTOR*) ADDR_DISK_DAT_0;
            if (str_equal((TEXTPOINTER)"PHOBOSFS",bootsector->fsid,8)) {
                terminal_printf((TEXTPOINTER)"IDE 1 Partition %u [%u MB] VolumeID: %s\n",(n+1),(mbrdisk1->part[n].total_sectors*512)/0x100000,bootsector->volumeid);
            }
        }
    }
    
}
