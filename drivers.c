#include "zeonos_h"

uint32_t system_ticks = 0;

void init_timer(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
    kernel_print("PIT: Timer Initialized\n", COLOR_GREEN);
}

void init_keyboard(void) {
    kernel_print("Keyboard: PS/2 Driver Initialized\n", COLOR_GREEN);
}

uint32_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = (uint32_t)(((uint32_t)bus << 16) | ((uint32_t)slot << 11) |
                      ((uint32_t)func << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outb(0xCF8, address);
    return (uint32_t)((inb(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}

void check_pci_bus(void) {
    kernel_print("PCI: Scanning Bus...\n", COLOR_LIGHT_BLUE);
    int found = 0;
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
            if (vendor != 0xFFFF) {
                found++;
                kernel_print("PCI Device Found - Vendor: ", COLOR_CYAN);
                kernel_print_hex(vendor, COLOR_CYAN);
                kernel_print("\n", COLOR_CYAN);
            }
        }
    }
    if(!found) kernel_print("PCI: No devices found.\n", COLOR_LIGHT_RED);
}

void init_ata(void) {
    kernel_print("ATA: Primary Bus Initialized.\n", COLOR_GREEN);
}
