PLATFORM=i386-elf
CROSSBINDIR=/cross/${PLATFORM}/bin
SRCDIR=./src
INCDIR=./inc
OBJDIR=./obj
BINDIR=./bin

AS=${CROSSBINDIR}/${PLATFORM}-as
GCC=${CROSSBINDIR}/${PLATFORM}-gcc
LIBGCC=-lgcc


all: s-build s-link

s-build: s-build-boot s-build-main s-build-terminal s-build-disk

s-build-boot: ${OBJDIR}/boot.o

${OBJDIR}/boot.o:${SRCDIR}/boot/boot.s
	${AS} ${SRCDIR}/boot/boot.s -o ${OBJDIR}/boot.o


s-build-main: ${OBJDIR}/main.o ${INCDIR}/*

${OBJDIR}/main.o: ${SRCDIR}/main/main.c ${INCDIR}/*
	${GCC} -c ${SRCDIR}/main/main.c -o ${OBJDIR}/main.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

s-build-terminal:  ${OBJDIR}/terminal.o

${OBJDIR}/terminal.o: ${SRCDIR}/terminal/terminal.c ${INCDIR}/*
	${GCC} -c ${SRCDIR}/terminal/terminal.c -o ${OBJDIR}/terminal.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

s-build-disk:  ${OBJDIR}/disk.o ${OBJDIR}/ata.o

${OBJDIR}/disk.o: ${SRCDIR}/disk/disk.c ${INCDIR}/*
	${GCC} -c ${SRCDIR}/disk/disk.c -o ${OBJDIR}/disk.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

${OBJDIR}/ata.o: ${SRCDIR}/disk/ata.asm
	nasm -f elf ${SRCDIR}/disk/ata.asm -o ${OBJDIR}/ata.o



s-link: ${BINDIR}/phobos.bin

${BINDIR}/phobos.bin: ${OBJDIR}/boot.o ${OBJDIR}/main.o ${OBJDIR}/terminal.o ${OBJDIR}/disk.o ${OBJDIR}/ata.o ${SRCDIR}/linker.ld
	${GCC} -T ${SRCDIR}/linker.ld -o ${BINDIR}/phobos.bin -ffreestanding -O2 -nostdlib ${OBJDIR}/boot.o ${OBJDIR}/main.o ${OBJDIR}/terminal.o ${OBJDIR}/disk.o ${OBJDIR}/ata.o ${LIBGCC}

clean:
	rm -r -f ${OBJDIR}/*
	rm -r -f ${BINDIR}/*
