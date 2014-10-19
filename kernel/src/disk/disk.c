#include "../../inc/terminal.h"
#include "../../inc/disk.h"


DISK disks[4];

FILEDESCRIPTOR files[32];

//---------------------------------------------------------------------
int32_t ide_read (uint32_t busnumber, uint32_t secnum,uint16_t nsecs, void* buffer) {
    ata_lba_read(busnumber,secnum,(uint32_t)nsecs,(uint32_t)buffer);
    return nsecs;
}

uint32_t disk_autodetect () {
    MBR mbr;
    uint32_t ndisk = 0;
    for (int n=0;n<4;n++) disks[n].status = DISK_STATUS_DISABLED;
    ide_read(1,0,1,&mbr);
    for (int n=0;n<4;n++) {
        if ((mbr.part[n].total_sectors>0)
                &&(mbr.part[n].total_sectors!=mbr.part[n].first_sector)) {
            ide_read(1,mbr.part[n].first_sector,1,&(disks[ndisk].bootsector));
            if (str_equal((TEXTPOINTER)"PHOBOSFS",disks[ndisk].bootsector.fsid,8)) {
                terminal_printf((TEXTPOINTER)"IDE 1 Partition %u [%u MB] VolumeID: %s\n",
                        (n+1),(mbr.part[n].total_sectors*512)/0x100000,
                        disks[ndisk].bootsector.volumeid);
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

//---------------------------------------------------------------------
int32_t disk_read (DISK* disk,uint32_t sectornumber, uint16_t nsectors, void* buffer) {
    if (disk->status==DISK_STATUS_READY) {
        if (disk->bustype==BUS_TYPE_IDE) {
            return ide_read(disk->busnumber,disk->first_sector+sectornumber,nsectors,buffer);
        }
        else return -DISK_STATUS_UNKNOWN;
    }
    else return -disk->status;
}


//----------------------------------------------------------------------
DIRECTORYENTRY* findentry (DISK* disk, uint8_t *name,DIRECTORYSECTOR* dirsector,uint32_t* dirsectornumber) {
    int n;
    int cont;
    do {
        cont = 0;
        for (n=0;n<4;n++) {
           if((dirsector->entry[n].type!=DELETED_ENTRY)
                   &&(str_equal(name,dirsector->entry[n].name,116))) return &(dirsector->entry[n]);
        }
        if (dirsector->down_sector>0) {
            *dirsectornumber = dirsector->down_sector;
            disk_read(disk,disk->bootsector.sut_size+dirsector->down_sector,1,&dirsector);
            cont=1;
        } 
    }   while(cont);
    return NULL;
}

void getpathelement(TEXTPOINTER path,TEXTPOINTER resultpath,TEXTPOINTER element) {
    int n,m;
    uint8_t d;
    element[0]=0;
    resultpath[0]=0;
    
    if (path[0]=='/') n=1; else n=0;
    
    for (m=0;n<116;n++,m++) {
        d=path[n];
        if ((d=='/')||(d==0)) break;
        else element[m]=d;
    }
    element[m]=0;
    m=0;
    while(d!=0) {
        d=path[n];
        resultpath[m]=d;
        m++;
        n++;
    }
    if(str_equal(resultpath,(TEXTPOINTER)"/",2)) resultpath[0]=0;
}


//---------------------------------------------------------------------
void disk_init () {
    for (int n=0;n<32;n++)files[n].status=FILE_STATUS_CLOSE;
    terminal_printf((TEXTPOINTER)"Detecting disks...\n");
    if (!disk_autodetect()) {
        terminal_printf((TEXTPOINTER)"No disks detected\n");
    }
}

DISK* getdiskbyname(TEXTPOINTER devicename) {
    if(str_equal(devicename,(TEXTPOINTER)"/disk1",6)) {
        if (disks[0].status==DISK_STATUS_DISABLED) return NULL;
        return &disks[0];
    }
    else if(str_equal(devicename,(TEXTPOINTER)"/disk2",6)) {
        if (disks[1].status==DISK_STATUS_DISABLED) return NULL;
        return &disks[1];
    }
    else if(str_equal(devicename,(TEXTPOINTER)"/disk3",6)) {
        if (disks[2].status==DISK_STATUS_DISABLED) return NULL;
        return &disks[2];
    }      
    else if(str_equal(devicename,(TEXTPOINTER)"/disk4",6)) {
        if (disks[3].status==DISK_STATUS_DISABLED) return NULL;
        return &disks[3];
    }    
    return NULL;
}

void listdir(DISK* disk, DIRECTORYSECTOR* dirsector,uint32_t* dirsectornumber) {
    int n;
    int cont;
    do {
        cont = 0;
        for (n=0;n<4;n++) {
            if (dirsector->entry[n].type>0){
                switch(dirsector->entry[n].type){
                    case DIRECTORY_ENTRY: terminal_printf((TEXTPOINTER)"[DIR ] %s\n",dirsector->entry[n].name); break;    
                    case FILE_ENTRY: terminal_printf((TEXTPOINTER)"[FILE] %s [%u bytes]\n",dirsector->entry[n].name,dirsector->entry[n].size); break;   
                    default: terminal_printf((TEXTPOINTER)"[UNK ] %s\n",dirsector->entry[n].name);
                }
                
            }
        }
        if (dirsector->down_sector>0) {
            *dirsectornumber = dirsector->down_sector;
            disk_read(disk,disk->bootsector.sut_size+(*dirsectornumber),1,&dirsector);
            cont=1;
        } 
    }   while(cont);    
}

int32_t file_open (TEXTPOINTER filepath, uint32_t mode) {
    DIRECTORYENTRY* direntry;
    DIRECTORYSECTOR dirsector;
    uint32_t dirsectornumber;
    uint8_t d;
    uint32_t n;
    uint8_t devicefile[1024];
    uint8_t path[1024];
    uint8_t resultpath[1024];
    uint8_t element[116];
    
    for (n=0;n<1024;n++) {
        d=filepath[n];
        if (d==0) {
            terminal_printf((TEXTPOINTER)"Error path without disk.\n");
            return (-1);
        }
        if (d==':') break;
        devicefile[n]=d;
    }
    devicefile[n+1] = 0;
    strcpy(path,&filepath[n+1]);  
    
    DISK* disk ;
    
    disk = getdiskbyname(devicefile);
    
    if (disk==NULL) {
        terminal_printf((TEXTPOINTER)"Disk %s not found\n",devicefile);
        return -1;
    }
    
    dirsectornumber = 1;
    disk_read(disk,disk->bootsector.sut_size+dirsectornumber,1,&dirsector);
    
    strcpy(resultpath,path);
    while (strlen(resultpath)>0) {
        getpathelement(path,resultpath,element);
        if (strlen(element)!=0) {
            if ((direntry = findentry(disk,element,&dirsector,&dirsectornumber))==NULL){
                terminal_printf((TEXTPOINTER)"%s not found\n",element);
                return -1;
            }
            if (direntry->type==DIRECTORY_ENTRY) {
                dirsectornumber = direntry->sector;
                disk_read(disk,disk->bootsector.sut_size+direntry->sector,1,&dirsector);
            }
            else {
                if (strlen(resultpath)>0) {
                    terminal_printf((TEXTPOINTER)"%s is not a directory\n",element);
                    return -1;            
                }
                else {
                    for(int k=0;k<32;k++) {
                        if (files[k].status!=FILE_STATUS_CLOSE) {
                            files[k].status=FILE_STATUS_READY;
                            strcpy(files[k].path,filepath);
                            files[k].mode = mode;
                            files[k].position = 0;
                            files[k].sector = direntry->sector;
                            files[k].size = direntry->size; 
                            return k;
                        }
                    }
                    terminal_printf((TEXTPOINTER)"Not possible to open more files\n");
                    return -1;
                }
            }
        }
        strcpy(path,resultpath);
    }
    
    terminal_printf((TEXTPOINTER)"File not found.");
    
    return -1;
}

void file_close(int32_t fileid) {
    files[fileid].status=FILE_STATUS_CLOSE;
}