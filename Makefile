all: s-kernel-build s-kernel-install

s-kernel-build:
	cd kernel && make

s-kernel-install: s-mount s-copy s-umount

s-mount:
	modprobe nbd
	qemu-nbd -c /dev/nbd0 /WinDesktop/phobos.vdi
	qemu-nbd -c /dev/nbd1 /WinDesktop/phobos.vdi -P1
	mount /dev/nbd1 /mnt/phobos

s-copy:
	cp kernel/bin/phobos.bin /mnt/phobos/boot/phobos.bin

s-umount:
	sync
	umount /mnt/phobos
	qemu-nbd -d /dev/nbd1
	qemu-nbd -d /dev/nbd0

clean: clean-kernel

clean-kernel:
	cd kernel && make clean



