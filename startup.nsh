@echo -off
echo "=========================================="
echo "    BOOTING ZEON OS BAREMETAL KERNEL...   "
echo "=========================================="
FS0:
cd EFI\BOOT
BOOTX64.EFI
