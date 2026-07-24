CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -c
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker

OBJS = boot.o io.o vga.o mmu.o idt_gdt.o drivers.o task.o fs.o shell.o kernel.o
KERNEL_BIN = zeonosbin

all: $(KERNEL_BIN)

boot.o: boot
	$(AS) $(ASFLAGS) boot -o boot.o

%.o: % zeonos_h
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_BIN): $(OBJS) linker
	$(LD) $(LDFLAGS) -o $(KERNEL_BIN) $(OBJS)

run: $(KERNEL_BIN)
	qemu-system-i386 -kernel $(KERNEL_BIN)

clean:
	rm -rf *.o $(KERNEL_BIN)
