#INSTALL_VDI_PATH = /home/diego/VirtualBox\ VMs/phobos/phobos.vdi
INSTALL_VDI_PATH = /WinDesktop/phobos.vdi

all: s-kernel-build s-linux-utils-build s-kernel-install

s-kernel-build:
	cd kernel && make

s-kernel-install: s-mount s-copy s-umount

s-linux-utils-build:
	cd linux-utils && make

s-mount:
	modprobe nbd
	qemu-nbd -c /dev/nbd0 ${INSTALL_VDI_PATH}
	qemu-nbd -c /dev/nbd1 ${INSTALL_VDI_PATH} -P1
	mount /dev/nbd1 /mnt/phobos

s-copy: 
	cp kernel/bin/phobos.bin /mnt/phobos/boot/phobos.bin

s-umount:
	sync
	umount /mnt/phobos
	qemu-nbd -d /dev/nbd1
	qemu-nbd -d /dev/nbd0

clean: clean-kernel clean-linux-utils

clean-kernel:
	cd kernel && make clean
	
clean-linux-utils:
	cd linux-utils && make clean



