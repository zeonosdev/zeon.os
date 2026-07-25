CC = x86_64-w64-mingw32-gcc
AS = x86_64-w64-mingw32-as
CFLAGS = -I/usr/include/efi -I/usr/include/efi/x86_64 -fno-stack-protector -fshort-wchar -mno-red-zone

all: build_iso

boot.o: boot.s
	$(AS) boot.s -o boot.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

build_efi: boot.o main.o
	mkdir -p iso_root/EFI/BOOT
	$(CC) -shared -Bsymbolic -e efi_main -o iso_root/EFI/BOOT/BOOTX64.EFI boot.o main.o -lgnuefi -lefi

build_iso: build_efi
	xorriso -as mkisofs -R -f -e EFI/BOOT/BOOTX64.EFI -no-emul-boot -o zeon_os.iso iso_root

clean:
	rm -rf *.o iso_root zeon_os.iso
