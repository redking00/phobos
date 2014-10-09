all: s-kernel-build s-utils-linux-build

s-kernel-build:
	cd kernel && make

s-kernel-install: s-mount s-copy s-umount

s-utils-linux-build:
	cd linux-utils && make

s-mount: ./kernel/bin/phobos.bin
	modprobe nbd
	qemu-nbd -c /dev/nbd0 /WinDesktop/phobos.vdi
	qemu-nbd -c /dev/nbd1 /WinDesktop/phobos.vdi -P1
	mount /dev/nbd1 /mnt/phobos

s-copy: ./kernel/bin/phobos.bin
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



