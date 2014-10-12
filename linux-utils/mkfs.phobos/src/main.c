#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <fcntl.h>
#include <linux/fs.h>

#include "../../../kernel/inc/filesystem.h"

int main (int argc, char** argv) {
    
    BOOTSECTOR bootsector;
    
    uint8_t emptysector[512];
    
    int n;
    
    unsigned long totalsectors;
    unsigned long datasectors;
    
    for (n=0;n<512;n++) emptysector[n]=0;
    
    printf("mkfs.phobos\n");

    if (argc<3) {
        printf("usage: mkfs.phobos <device> <volumeid>\n",argv[1]);
        return -1;
    }
    
    int fd = open(argv[1],O_RDWR);

    if (fd<0) {
        printf("Error opening %s\n",argv[1]);
        return -1;
    }
    
    if ((ioctl(fd,BLKGETSIZE,&totalsectors)<0)||(totalsectors==0)) {
        close(fd);
        printf("Error getting block info in %s, is device mounted?\n",argv[1]);
        return -1;
    }
    
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

    if (lseek(fd,0,SEEK_SET)<0){
        close(fd);
        printf("Error seeking in %s\n",argv[1]);
        perror("lseek: ");
        return -1;
    }
    
    if (write(fd,&bootsector,512)<0) {
        close(fd);
        printf("Error writting bootsector in %s\n",argv[1]);
        perror("write: ");
        return -1;
    }
    printf("Write bootsector ok\n");
    uint8_t b = 1;
    if (write(fd,&b,1)<0) {
        close(fd);
        printf("Error writting SUT sector1 %u\n",n);
        perror("write: ");
        return -1;
    }
    b=0;
    for (n=0;n<511;n++) {
        if (write(fd,&b,1)<0) {
            close(fd);
            printf("Error writting SUT sector1 %u\n",n);
            perror("write: ");
            return -1;
        }
    }
    
    for (n=0;n<bootsector.sut_size-1;n++) {
        if (write(fd,&emptysector,512)<0) {
            close(fd);
            printf("Error writting SUT sector %u\n",n);
            perror("write: ");
            return -1;
        }
    }
    printf("Write SUT ok\n");

    DIRECTORYSECTOR dirsector;
    
    dirsector.up_sector=0;
    dirsector.down_sector=0;
    dirsector.up_index=0;
    dirsector.entry[0].type=DELETED_ENTRY;
    dirsector.entry[1].type=DELETED_ENTRY;
    dirsector.entry[2].type=DELETED_ENTRY;
    dirsector.entry[3].type=DELETED_ENTRY;
    
    if (write(fd,&dirsector,512)<0) {
        close(fd);
        printf("Error writting ROOT sector %u\n",n);
        perror("write: ");
        return -1;
    }

    printf("Write root ok\n");
    
    close(fd);

    printf("Terminated ok\n");
    
}

