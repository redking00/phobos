#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fs.h>

#include "../../../kernel/inc/filesystem.h"


BOOTSECTOR bootsector;
DIRECTORYSECTOR dirsector;
uint8_t emptysector[512];
unsigned long totalsectors;
unsigned long datasectors;

int fd;


void readsector(void* sec,uint32_t secnum) {
    if (lseek(fd,secnum*512,SEEK_SET)<0){
        close(fd);
        perror("lseek error: ");
        exit(-1);
    }
    if(read(fd,sec,512)<0) {
        close(fd);
        perror("read error: ");
        exit(-1);
    }    
}

void writesector(void* sec,uint32_t secnum) {
    if (lseek(fd,secnum*512,SEEK_SET)<0){
        close(fd);
        perror("lseek error: ");
        exit(-1);
    }
    if(write(fd,sec,512)<0) {
        close(fd);
        perror("read error: ");
        exit(-1);
    }    
}

void checkarguments(int argc, char **argv) {
    if (argc<3) {
        printf("usage: mkfs.phobos <device> <volumeid>\n");
        exit(-1);
    }
}

void opendevicefile(char* devicefilename) {
    fd = open(devicefilename,O_RDWR);
    if (fd<0) {
        printf("Error opening %s\n",devicefilename);
        exit(-1);
    }
    if ((ioctl(fd,BLKGETSIZE,&totalsectors)<0)||(totalsectors==0)) {
        close(fd);
        printf("Error getting block info in %s, is device mounted?\n",devicefilename);
        exit (-1);
    }
}

int main (int argc, char** argv) {
    int n;

    for (n=0;n<512;n++) emptysector[n]=0;

    printf("mkfs.phobos\n");

    checkarguments(argc,argv);
    opendevicefile(argv[1]);
    
    printf("Total blocks: %lu\n",totalsectors);
   
    strcpy((char*)bootsector.fsid,"PHOBOSFS");
    strcpy((char*)bootsector.volumeid,argv[2]);
    bootsector.sut_first_sector=1;

    bootsector.sut_size=((totalsectors-1)/513);
    datasectors=totalsectors-bootsector.sut_size-1;

    printf("FSID = %s\n",bootsector.fsid);
    printf("VOLUMEID = %s\n",bootsector.volumeid);
    printf("SUT size = %u sectors for %u sectors of data\n",bootsector.sut_size,bootsector.sut_size*512);
    printf("%lu sectors lost.\n",datasectors-(bootsector.sut_size*512));

    
    writesector(&bootsector,0);

    printf("Write bootsector ok\n");
    

    emptysector[0] = 1;
    writesector(&emptysector,1);
    emptysector[0] = 0;
    for (n=0;n<bootsector.sut_size-1;n++) {
        writesector(&emptysector,2+n);
    }
    printf("Write SUT ok\n");

    
    dirsector.up_sector=0;
    dirsector.down_sector=0;
    dirsector.up_index=0;
    dirsector.entry[0].type=DELETED_ENTRY;
    dirsector.entry[1].type=DELETED_ENTRY;
    dirsector.entry[2].type=DELETED_ENTRY;
    dirsector.entry[3].type=DELETED_ENTRY;
    
    writesector(&dirsector,1+bootsector.sut_size);

    printf("Write root ok\n");
    
    close(fd);

    printf("Terminated ok\n");
    
}

