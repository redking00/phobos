#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <fcntl.h>
#include <linux/fs.h>

#include "../../../kernel/inc/filesystem.h"

int main (int argc, char **argv) {
    unsigned long totalsectors;
    BOOTSECTOR bootsector;
    ENTRYSECTOR rootsector;

    if (argc<3) {
        printf("usage: ls.phobos <device> <path>\n",argv[1]);
        return -1;
    }
    
    int fd = open(argv[1],O_RDONLY);

    if (fd<0) {
        printf("Error opening %s\n",argv[1]);
        return -1;
    }
    
    if ((ioctl(fd,BLKGETSIZE,&totalsectors)<0)||(totalsectors==0)) {
        close(fd);
        printf("Error getting block info in %s, is device mounted?\n",argv[1]);
        return -1;
    }
    if (lseek(fd,0,SEEK_SET)<0){
        close(fd);
        printf("Error seeking in %s\n",argv[1]);
        perror("lseek: ");
        return -1;
    }
    
    if(read(fd,&bootsector,512)<0) {
        close(fd);
        printf("Error reading in %s\n",argv[1]);
        perror("read: ");
        return -1;
    }

    if (lseek(fd,512+(bootsector.sut_size*512),SEEK_SET)<0){
        close(fd);
        printf("Error seeking in %s\n",argv[1]);
        perror("lseek: ");
        return -1;
    }

    if(read(fd,&rootsector,512)<0) {
        close(fd);
        printf("Error reading in %s\n",argv[1]);
        perror("read: ");
        return -1;
    }    
    
    int n;
    for (n=0;n<4;n++) {
        if (rootsector.entry[n].type>0)
            printf("%s\n",rootsector.entry[n].name);
    }

    
    close(fd);
    
}
