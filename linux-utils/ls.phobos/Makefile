SRCDIR=./src
INCDIR=./inc
OBJDIR=./obj
BINDIR=./../bin

GCC=gcc

all: s-build 

s-build: ${BINDIR}/ls.phobos

${BINDIR}/ls.phobos: ${SRCDIR}/main.c
	${GCC} ${SRCDIR}/main.c -o ${BINDIR}/ls.phobos

clean:
	rm -r -f ${OBJDIR}/*
