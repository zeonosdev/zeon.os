// kernel_main.c
#include "keyboard.h"
#include "vfs.h"

void kernel_main() {
    vfs_init();      // Inisialisasi struktur folder Linux
    keyboard_init(); // Aktifkan driver keyboard
    shell_run();     // Jalankan terminal
}
