// NOT FULLY IMPLEMENTED
//  AVAILABLE vfs -> phobos
//  NOT AVAILABLE phobos -> vfs
//  NOT AVAILABLE phobos -> phobos
//  NEVER AVAILABLE vfs -> vfs



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
uint32_t dirsectorindex;
DIRECTORYSECTOR exdirsector;
uint32_t exdirsectornumber;
DIRECTORYSECTOR elementdirsector;
uint32_t elementdirsectornumber;
DIRECTORYENTRY* direntry;

uint8_t sutsector[512];
uint32_t sutesectornumber;

unsigned long totalsectors;

char devicefile[1024];
char path[1024];
char resultpath[1024];
char element[116];

int sourcefile=-1;
int fd=-1;

void closefiles() {
    if (fd>=0) close(fd);
    if (sourcefile>=0) close(sourcefile);
}

void readsector(void* sec,uint32_t secnum) {
//    printf("reading sector: %u\n",secnum);
    if (lseek(fd,secnum*512,SEEK_SET)<0){
        closefiles();
        perror("lseek error: ");
        exit(-1);
    }
    if(read(fd,sec,512)<0) {
        closefiles();
        perror("read error: ");
        exit(-1);
    }    
}

void writesector(void* sec,uint32_t secnum) {
//    printf("writting sector: %u\n",secnum);
    if (lseek(fd,secnum*512,SEEK_SET)<0){
        closefiles();
        perror("lseek error: ");
        exit(-1);
    }
    if(write(fd,sec,512)<0) {
        closefiles();
        perror("write error: ");
        exit(-1);
    }
    if(fsync(fd)<0) {
        closefiles();
        perror("fsync error: ");
        exit(-1);
    }
}

void checkisphobosfs(){
    if(strcmp(bootsector.fsid,"PHOBOSFS")){
        closefiles();
        printf("No phobos filesystem detected\n");
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

DIRECTORYENTRY* findemptyentry () {
    int n;
    int cont;
    do {
        cont = 0;
        for (n=0,dirsectorindex=0;n<4;n++,dirsectorindex++) {
           if (dirsector.entry[n].type==DELETED_ENTRY) return &(dirsector.entry[n]);
        }
        if (dirsector.down_sector>0) {
            dirsectornumber = dirsector.down_sector;
            readsector(&dirsector,bootsector.sut_size+dirsector.down_sector);
            cont=1;
        }
    }   while(cont);
    
    return NULL;
}

void findfreesector() {
//    printf("findfreesector\n");
    int n,m;
    exdirsectornumber = 0;
    for (n=0;n<bootsector.sut_size;n++) {
        readsector(&sutsector,1+n);
        for (m=0;m<512;m++) if (sutsector[m]==0) {
            sutsector[m]=1;
            writesector(&sutsector,1+n);
            exdirsectornumber = ((n*512)+m)+1;
            return;
        }
    }
    closefiles();
    printf("Error no more space in disk\n");
    exit(-1);
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
    if (argc<3) {
        printf("usage: cp.phobos <device> <path>\n");
        exit (-1);
    }
    for (n=0;n<1024;n++) {
        d=argv[2][n];
        if (d==0) {
            printf("Error bad formatted destination, example: /dev/nbd2:/\n");
            exit(-1);
        }
        if (d==':') break;
        devicefile[n]=d;
    }
    devicefile[n+1] = 0;
    strcpy(path,&argv[2][n+1]);
    
}

void opensourcefile(char* filepath) {
    sourcefile = open(filepath,O_RDONLY);
    if (sourcefile<0) {
        printf("Error opening %s\n",filepath);
        exit (-1);
    }
}

void opendevicefile(char* devicefilepath) {
    fd = open(devicefilepath,O_RDWR);

    if (fd<0) {
        printf("Error opening %s\n",devicefilepath);
        exit (-1);
    }
    
    if ((ioctl(fd,BLKGETSIZE,&totalsectors)<0)||(totalsectors==0)) {
        closefiles();
        printf("Error getting block info in %s, is device mounted?\n",devicefilepath);
        exit(-1);
    }
}

int main(int argc, char** argv) {
//    printf("cp.phobos\n");
    checkarguments(argc,argv);
    opensourcefile(argv[1]);
    opendevicefile(devicefile);

    readsector(&bootsector,0);

    dirsectornumber = 1;
    
    readsector(&dirsector,bootsector.sut_size+dirsectornumber);
    
    checkisphobosfs();
    
    strcpy(resultpath,path);

    while (strlen(resultpath)>0) {
        
        getpathelement(path,resultpath,element);
        
        memcpy(&elementdirsector,&dirsector,512);
        
        if ((direntry = findentry(element))==NULL){
            if (strlen(resultpath)>0) {
                closefiles();
                printf("%s not found\n",element);
                return -1;
            }
            else {
                printf("copying file %s\n",argv[1]);
                memcpy(&dirsector,&elementdirsector,512);
                if ((direntry = findemptyentry())==NULL) {
                    findfreesector();
                    if (!exdirsectornumber) {
                        closefiles();
                        printf("Error no more space in disk\n");
                        return -1;
                    }
                    dirsector.down_sector = exdirsectornumber;
                    writesector(&dirsector,bootsector.sut_size+dirsectornumber);
                    exdirsector.up_sector = dirsectornumber;
                    exdirsector.up_index = 0;
                    exdirsector.down_sector = 0;
                    exdirsector.entry[0].type=DELETED_ENTRY;
                    exdirsector.entry[1].type=DELETED_ENTRY;
                    exdirsector.entry[2].type=DELETED_ENTRY;
                    exdirsector.entry[3].type=DELETED_ENTRY;
                    memcpy (&dirsector,&exdirsector,512);
                    dirsectornumber=exdirsectornumber;
                    dirsectorindex=0;
                    direntry = &(dirsector.entry[0]);
                }

                uint32_t sourcesize;
                sourcesize = lseek(sourcefile,0,SEEK_END);
                lseek(sourcefile,0,SEEK_SET);
                
                int steps = sourcesize/500;
                int remainder = sourcesize-(steps*500);
                int n;

                findfreesector();
                if (!exdirsectornumber) {
                    closefiles();
                    printf("Error no more space in disk\n");
                    return -1;
                }                    
                strcpy(direntry->name,element);
                direntry->sector=exdirsectornumber;
                direntry->type=FILE_ENTRY;
                direntry->size=sourcesize;
                writesector(&dirsector,bootsector.sut_size+dirsectornumber);
//                printf("write file entry\n");
                
                ((DATASECTOR*)&exdirsector)->up_sector = dirsectornumber;
                ((DATASECTOR*)&exdirsector)->up_index = dirsectorindex;
                ((DATASECTOR*)&exdirsector)->down_sector = 0;
                read(sourcefile,&(((DATASECTOR*)&exdirsector)->data),(sourcesize>500)? 500:sourcesize);
                writesector(&exdirsector,bootsector.sut_size+exdirsectornumber);
//                printf("write datasector 0\n");

                
                if (steps>0)
                for (n=0;n<steps-1;n++){
                    memcpy (&elementdirsector,&exdirsector,512);
                    elementdirsectornumber=exdirsectornumber;
                    findfreesector();
                    if (!exdirsectornumber) {
                        closefiles();
                        printf("Error no more space in disk\n");
                        return -1;
                    }

                    ((DATASECTOR*)&exdirsector)->up_sector = elementdirsectornumber;
                    ((DATASECTOR*)&exdirsector)->up_index = 0;
                    ((DATASECTOR*)&exdirsector)->down_sector = 0;
                    read(sourcefile,&(((DATASECTOR*)&exdirsector)->data),500);
                    writesector(&exdirsector,bootsector.sut_size+exdirsectornumber);
//                    printf("write datasector %u\n",n+1);
                    ((DATASECTOR*)&elementdirsector)->down_sector = exdirsectornumber;
                    writesector(&elementdirsector,bootsector.sut_size+elementdirsectornumber);
//                    printf("update datasector %u\n",n);
                }
                if(remainder>0) {
                    memcpy (&elementdirsector,&exdirsector,512);
                    elementdirsectornumber=exdirsectornumber;
                    findfreesector();
                    if (!exdirsectornumber) {
                        closefiles();
                        printf("Error no more space in disk\n");
                        return -1;
                    }
                    ((DATASECTOR*)&exdirsector)->up_sector = elementdirsectornumber;
                    ((DATASECTOR*)&exdirsector)->up_index = 0;
                    ((DATASECTOR*)&exdirsector)->down_sector = 0;
                    read(sourcefile,&(((DATASECTOR*)&exdirsector)->data),remainder);
                    writesector(&exdirsector,bootsector.sut_size+exdirsectornumber);
//                    printf("write datasector remainder\n");
                    ((DATASECTOR*)&elementdirsector)->down_sector = exdirsectornumber;
                    writesector(&elementdirsector,bootsector.sut_size+elementdirsectornumber);
//                    printf("update datasector remainder-1\n");
                }
                return 0;
            }
        }
        if (direntry->type==DIRECTORY_ENTRY) {
            dirsectornumber = direntry->sector;
            readsector(&dirsector,bootsector.sut_size+direntry->sector);
        }
        else {
            closefiles();
            printf("%s is not a directory\n",element);
            return -1;            
        }
        strcpy(path,resultpath);
    }
    
    closefiles();
    return (0);
}

