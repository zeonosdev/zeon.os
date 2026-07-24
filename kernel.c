#include "zeonos_h"

void hardware_initialize(void) {
    init_gdt();
    init_idt();
    pic_remap(0x20, 0x28);
    init_timer(100);
    init_keyboard();
    init_ata();
    check_pci_bus();
    init_paging();
    asm volatile("sti");
    kernel_print("HW: Hardware Subsystem Ready.\n", COLOR_WHITE);
}

void kernel_main(void) {
    vga_clear_screen();
    kernel_print("========================================\n", COLOR_CYAN);
    kernel_print("        ZEONOS v0.3.0 BOOTING          \n", COLOR_WHITE);
    kernel_print("========================================\n", COLOR_CYAN);

    hardware_initialize();
    scheduler_initialize();
    filesystem_initialize();

    filesystem_create_file("system.log", 1024);
    filesystem_create_file("kernel.bin", 65536);

    shell_start();

    while(1) {
        scheduler_switch_task();
        asm volatile("hlt");
    }
}
