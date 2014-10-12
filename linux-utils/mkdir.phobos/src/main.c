#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <fcntl.h>
#include <linux/fs.h>

#include "../../../kernel/inc/filesystem.h"

int main (int argc, char **argv) {
    unsigned long totalsectors;
    BOOTSECTOR bootsector;

    if (argc<3) {
        printf("usage: ls.phobos <device> <path>\n",argv[1]);
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

    DIRECTORYSECTOR dirsector;

    if(read(fd,&dirsector,512)<0) {
        close(fd);
        printf("Error reading in %s\n",argv[1]);
        perror("read: ");
        return -1;
    }    

    int n,m;
    char dirname[103];
    char d;
    char *path = &argv[2][0];
    
    for (n=0;n<103;n++)dirname[n]=0;
    if (path[0]=='/') n=1; else n=0;
    for (m=0;n<103;n++,m++) {
        d=path[n];
        if ((d=='/')||(d==0)) break;
        else dirname[m]=d;
    }
    
    
    close(fd);
    
}
