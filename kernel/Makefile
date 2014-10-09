PLATFORM=i386-elf
CROSSBINDIR=/cross/${PLATFORM}/bin
SRCDIR=./src
INCDIR=./inc
OBJDIR=./obj
BINDIR=./bin

AS=${CROSSBINDIR}/${PLATFORM}-as
GCC=${CROSSBINDIR}/${PLATFORM}-gcc
NASM=nasm
LIBGCC=-lgcc
GCCFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS=-ffreestanding -O2 -nostdlib
NASMFLAGS=-f elf

all: s-build s-link

s-build: s-build-boot s-build-main s-build-terminal s-build-disk

s-build-boot: ${OBJDIR}/boot.o

${OBJDIR}/boot.o:${SRCDIR}/boot/boot.s
	${AS} ${SRCDIR}/boot/boot.s -o ${OBJDIR}/boot.o


s-build-main: ${OBJDIR}/main.o

${OBJDIR}/main.o: ${INCDIR}/* \
	 ${SRCDIR}/main/main.c
	${GCC} -o ${OBJDIR}/main.o ${GCCFLAGS} -c \
	 ${SRCDIR}/main/main.c

s-build-terminal:  ${OBJDIR}/terminal.o

${OBJDIR}/terminal.o: ${INCDIR}/* \
	 ${SRCDIR}/terminal/terminal.c
	${GCC} -o ${OBJDIR}/terminal.o ${GCCFLAGS} -c \
	 ${SRCDIR}/terminal/terminal.c

s-build-disk:  ${OBJDIR}/disk.o ${OBJDIR}/ata.o

${OBJDIR}/disk.o: ${INCDIR}/* \
	 ${SRCDIR}/disk/disk.c
	${GCC} -o ${OBJDIR}/disk.o ${GCCFLAGS} -c \
	 ${SRCDIR}/disk/disk.c

${OBJDIR}/ata.o: ${INCDIR}/* \
	 ${SRCDIR}/disk/ata.asm
	${NASM} ${NASMFLAGS} -o ${OBJDIR}/ata.o \
	 ${SRCDIR}/disk/ata.asm



s-link: ${BINDIR}/phobos.bin

${BINDIR}/phobos.bin: \
	 ${OBJDIR}/boot.o \
	 ${OBJDIR}/main.o \
	 ${OBJDIR}/terminal.o \
	 ${OBJDIR}/disk.o \
	 ${OBJDIR}/ata.o \
	 ${SRCDIR}/linker.ld
	${GCC} -T ${SRCDIR}/linker.ld -o ${BINDIR}/phobos.bin ${LDFLAGS} ${LIBGCC} \
	 ${OBJDIR}/boot.o \
	 ${OBJDIR}/main.o \
	 ${OBJDIR}/terminal.o \
	 ${OBJDIR}/disk.o \
	 ${OBJDIR}/ata.o
clean:
	rm -r -f ${OBJDIR}/*
	rm -r -f ${BINDIR}/*
