SRCDIR=./src
INCDIR=./inc
OBJDIR=./obj
BINDIR=./../bin

GCC=gcc

all: s-build 

s-build: ${BINDIR}/cp.phobos

${BINDIR}/cp.phobos: ${SRCDIR}/main.c
	${GCC} ${SRCDIR}/main.c -o ${BINDIR}/cp.phobos

clean:
	rm -r -f ${OBJDIR}/*
