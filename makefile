CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -c
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot.o io.o vga.o mmu.o idt_gdt.o drivers.o task.o fs.o auth.o shell.o kernel.o
KERNEL_BIN = zeonos.bin

all: $(KERNEL_BIN)

boot.o: boot.asm
	$(AS) $(ASFLAGS) boot.asm -o boot.o

%.o: %.c zeonos.h
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_BIN): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $(KERNEL_BIN) $(OBJS)

run: $(KERNEL_BIN)
	qemu-system-i386 -kernel $(KERNEL_BIN)

clean:
	rm -rf *.o $(KERNEL_BIN)
