#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>
#include <fcntl.h>
#include <linux/fs.h>

#include "../../../kernel/inc/filesystem.h"

BOOTSECTOR bootsector;
DIRECTORYSECTOR dirsector;

int fd; 

int main (int argc, char **argv) {
    unsigned long totalsectors;

    if (argc<3) {
        printf("usage: ls.phobos <device> <path>\n",argv[1]);
        return -1;
    }
    
    fd = open(argv[1],O_RDWR);

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

DIRECTORYENTRY* findEntry (DIRECTORYSECTOR* dirsector,uint8_t *name) {
    int n;
    int cont;
    do {
        cont = 0;
        for (n=0;n<4;n++) {
           if(!strcmp(name,dirsector->entry[n].name)) return &(dirsector->entry[n]);
        }
        if (dirsector->down_sector>0) {
            if (lseek(fd,512+(bootsector.sut_size*512)+((dirsector->down_sector-1)*512),SEEK_SET)<0){
                close(fd);
                perror("lseek error: ");
                exit(-1);
            }
            if(read(fd,&dirsector,512)<0) {
                close(fd);
                perror("read error: ");
                exit(-1);
            }
            cont=1;
        } 
    }   while(cont);
    
    return NULL;
}
