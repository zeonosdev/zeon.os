# ============================================
# ZEONOS v0.3.0 - MAKEFILE
# Target Architecture: x86 (IA-32) Bare-Metal
# Description: Automated compilation & QEMU launcher
# ============================================

CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -c
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot.o kernel.o
KERNEL_BIN = zeonos.bin
ISO_NAME = zeonos.iso

all: $(KERNEL_BIN)

boot.o: boot.asm
	$(AS) $(ASFLAGS) boot.asm -o boot.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) kernel.c -o kernel.o

$(KERNEL_BIN): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $(KERNEL_BIN) $(OBJS)

iso: $(KERNEL_BIN)
	mkdir -p iso/boot/grub
	cp $(KERNEL_BIN) iso/boot/$(KERNEL_BIN)
	echo 'menuentry "ZEONOS v0.3.0" {' > iso/boot/grub/grub.cfg
	echo '    multiboot /boot/zeonos.bin' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_NAME) iso

run: $(KERNEL_BIN)
	qemu-system-i386 -kernel $(KERNEL_BIN)

run-iso: iso
	qemu-system-i386 -cdrom $(ISO_NAME)

clean:
	rm -rf *.o $(KERNEL_BIN) $(ISO_NAME) iso
