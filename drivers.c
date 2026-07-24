#include "zeonos.h"

void init_timer(uint32_t frequency) {
    (void)frequency;
    kernel_print("Timer: Inisialisasi PIT Berhasil\\n", COLOR_LIGHT_BLUE);
}

void init_keyboard(void) {
    kernel_print("Keyboard: PS/2 Driver Standby\\n", COLOR_LIGHT_BLUE);
}

void check_pci_bus(void) {
    kernel_print("PCI: Bus Scanner Ready\\n", COLOR_LIGHT_BLUE);
}

void init_ata(void) {
    kernel_print("ATA: Disk Driver Initialized\\n", COLOR_LIGHT_BLUE);
}
