#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fs.h>

#include "../../../kernel/inc/filesystem.h"

BOOTSECTOR bootsector;
DIRECTORYSECTOR dirsector;
uint32_t dirsectornumber;
DIRECTORYENTRY* direntry;

unsigned long totalsectors;

char path[1024];
char resultpath[1024];
char element[116];

char devicefile[1024];

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

DIRECTORYENTRY* findentry (uint8_t *name) {
    int n;
    int cont;
    do {
        cont = 0;
        for (n=0;n<4;n++) {
           if((dirsector.entry[n].type!=DELETED_ENTRY)
                   &&(!strcmp(name,dirsector.entry[n].name))) return &(dirsector.entry[n]);
        }
        if (dirsector.down_sector>0) {
            dirsectornumber = dirsector.down_sector;
            readsector(&dirsector,bootsector.sut_size+dirsector.down_sector);
            cont=1;
        } 
    }   while(cont);
    
    return NULL;
}

void listdir() {
    int n;
    int cont;
    do {
        cont = 0;
        for (n=0;n<4;n++) {
            if (dirsector.entry[n].type>0){
                switch(dirsector.entry[n].type){
                    case DIRECTORY_ENTRY: printf("[DIR] %s\n",dirsector.entry[n].name); break;    
                    case FILE_ENTRY: printf("[FILE] %s\n",dirsector.entry[n].name); break;   
                    default: printf("[UNKN] %s\n",dirsector.entry[n].name);
                }
                
            }
        }
        if (dirsector.down_sector>0) {
            dirsectornumber = dirsector.down_sector;
            readsector(&dirsector,bootsector.sut_size+dirsectornumber);
            cont=1;
        } 
    }   while(cont);    
}

void getpathelement(char* path,char* resultpath,char* element) {
    int n,m;
    char d;
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
    if(!strcmp(resultpath,"/")) resultpath[0]=0;
}

void checkarguments(int argc,char **argv) {
    int n;
    char d;
    if (argc<2) {
        printf("usage: ls.phobos <device> <path>\n");
        exit (-1);
    }
    for (n=0;n<1024;n++) {
        d=argv[1][n];
        if (d==0) {
            printf("Error bad formatted path. Example: /dev/nbd2:/\n");
            exit(-1);
        }
        if (d==':') break;
        devicefile[n]=d;
    }
    devicefile[n+1] = 0;
    strcpy(path,&argv[1][n+1]);
}

void opendevicefile(char* devicefilepath) {
    fd = open(devicefilepath,O_RDONLY);

    if (fd<0) {
        printf("Error opening %s\n",devicefilepath);
        exit (-1);
    }
    
    if ((ioctl(fd,BLKGETSIZE,&totalsectors)<0)||(totalsectors==0)) {
        close(fd);
        printf("Error getting block info in %s, is device mounted?\n",devicefilepath);
        exit(-1);
    }
}

int main (int argc, char **argv) {

    checkarguments(argc,argv);
    
    opendevicefile(devicefile);
    
    readsector(&bootsector,0);

    dirsectornumber = 1;
    
    readsector(&dirsector,bootsector.sut_size+dirsectornumber);

    strcpy(resultpath,path);

    while (strlen(resultpath)>0) {
        getpathelement(path,resultpath,element);
        if (strlen(element)!=0) {
            if ((direntry = findentry(element))==NULL){
                close(fd);
                printf("%s not found\n",element);
                return -1;
            }
            if (direntry->type==DIRECTORY_ENTRY) {
                dirsectornumber = direntry->sector;
                readsector(&dirsector,bootsector.sut_size+direntry->sector);
            }
            else {
                close(fd);
                printf("%s is not a directory\n",element);
                return -1;            
            }
        }
        strcpy(path,resultpath);
    }

    listdir();
    
    close(fd);
    
}
