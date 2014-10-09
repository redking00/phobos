SRCDIR=./src
INCDIR=./inc
OBJDIR=./obj
BINDIR=./bin

GCC=gcc

all: s-build 

s-build: ${BINDIR}/mkfs.phobos

${BINDIR}/mkfs.phobos: ${SRCDIR}/main.c
	${GCC} ${SRCDIR}/main.c -o ${BINDIR}/mkfs.phobos

clean:
	rm -r -f ${OBJDIR}/*
	rm -r -f ${BINDIR}/*
