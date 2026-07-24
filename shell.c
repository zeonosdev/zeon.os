/*
 * ZEONOS v0.3.0 - Interactive Shell & Auth Integration
 * License: GPLv3
 */

#include "zeonos.h"

void shell_start(void) {
    // Inisialisasi Database Auth File
    auth_init();

    // Jalankan Alur Register -> Simpan DB -> Login
    auth_prompt_register();
    auth_prompt_login();

    kernel_print("ZEONOS Interactive Shell v0.1 Ready.\n", COLOR_LIGHT_BROWN);
    kernel_print("Type 'help' for available commands.\n\n", COLOR_WHITE);
    kernel_print("zeonos# ", COLOR_LIGHT_GREEN);
}
