//NOT WORKING YET

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fs.h>

#include "../../../kernel/inc/filesystem.h"

void checkarguments(int argc,char **argv) {
    if (argc<3) {
        printf("usage: cp.phobos <sourcepath> <destinationpath>\n");
        exit (-1);
    }
}

int main(int argc, char** argv) {
    printf("cp.phobos\n");
    checkarguments(argc,argv);
    return (0);
}

