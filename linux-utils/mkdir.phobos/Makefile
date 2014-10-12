SRCDIR=./src
INCDIR=./inc
OBJDIR=./obj
BINDIR=./../bin

GCC=gcc

all: s-build 

s-build: ${BINDIR}/mkdir.phobos

${BINDIR}/mkdir.phobos: ${SRCDIR}/main.c
	${GCC} ${SRCDIR}/main.c -o ${BINDIR}/mkdir.phobos

clean:
	rm -r -f ${OBJDIR}/*
