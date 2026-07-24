#include "zeonos_h"

void shell_start(void) {
    kernel_print("ZEONOS Shell v0.1 Ready.\n", COLOR_LIGHT_BROWN);
    kernel_print("Type 'help' for available commands.\n", COLOR_WHITE);
    kernel_print("zeonos# ", COLOR_LIGHT_GREEN);
}
