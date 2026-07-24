#include "zeonos.h"

void shell_start(void) {
    auth_init();
    auth_prompt_register();
    auth_prompt_login();

    kernel_print("ZEONOS Interactive Shell v0.1 Ready.\n", COLOR_LIGHT_CYAN);
    kernel_print("Ketik 'help' untuk daftar perintah.\n\n", COLOR_LIGHT_BLUE);
    kernel_print("zeonos# ", COLOR_LIGHT_GREEN);
}
