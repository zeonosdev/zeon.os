# ==========================================
# ZEON OS BAREMETAL MAKEFILE BUILD SYSTEM
# ==========================================

ARCH            = x86_64
OBJS            = main.o
TARGET          = BOOTX64.EFI
ISO_NAME        = ZEON_OS_v2.5.iso

# UEFI Include & Library Paths (GNU-EFI)
EFIINC          = /usr/include/efi
EFIINCS         = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
LIB             = /usr/lib
EFILIB          = /usr/lib
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

# Compiler Flags
CFLAGS          = $(EFIINCS) -fno-stack-protector -fpic \
                  -fshort-wchar -mno-red-zone -Wall -Wextra -O2
LDFLAGS         = -nostdlib -znocombreloc -shared \
                  -Bsymbolic -L$(EFILIB) -L$(LIB) \
                  -T $(EFI_LDS) $(EFI_CRT_OBJS)

# Rules
all: $(TARGET) iso

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

main.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lgnuefi -lefi

$(TARGET): main.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $< $@

iso: $(TARGET)
	@echo "Creating ISO Image for ZEON OS..."
	mkdir -p iso_root/EFI/BOOT
	cp $(TARGET) iso_root/EFI/BOOT/
	cp startup.nsh iso_root/ 2>/dev/null || true
	genisoimage -e EFI/BOOT/$(TARGET) -no-emul-boot -o $(ISO_NAME) iso_root
	@echo "Build Finished! Output ISO: $(ISO_NAME)"

clean:
	rm -rf *.o *.so *.EFI iso_root $(ISO_NAME)
