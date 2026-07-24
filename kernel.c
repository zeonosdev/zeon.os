#include "zeonos.h"

void kernel_main(void) {
    vga_clear_screen();
    
    // Teks Layar Penyambut Awal Menggunakan Warna Biru (COLOR_LIGHT_BLUE)
    kernel_print("=========================================================================\n", COLOR_LIGHT_BLUE);
    kernel_print("                 ZEONOS v0.3.0 OPERATING SYSTEM BOOTING                  \n", COLOR_LIGHT_BLUE);
    kernel_print("=========================================================================\n\n", COLOR_LIGHT_BLUE);

    init_gdt();
    init_idt();
    pic_remap(0x20, 0x28);
    filesystem_initialize();

    shell_start();

    while(1) {
        asm volatile("hlt");
    }
}
