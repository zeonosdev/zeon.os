#include "zeonos.h"

// Akun default sederhana
const char USERNAME[] = "kominfo";
const char PASSWORD[] = "admin123";

int check_login(void) {
    char input_user[32];
    char input_pass[32];

    vga_clear_screen();
    kernel_print("========================================\n", COLOR_CYAN);
    kernel_print("           WELCOME TO ZEONOS            \n", COLOR_WHITE);
    kernel_print("========================================\n\n", COLOR_CYAN);

    kernel_print("Username: ", COLOR_LIGHT_GREY);
    // [Nanti panggil fungsi baca keyboard untuk username]

    kernel_print("Password: ", COLOR_LIGHT_GREY);
    // [Panggil fungsi baca keyboard dengan sensor bintang *]

    // Jika sesuai
    if (string_compare(input_user, USERNAME) == 0 && 
        string_compare(input_pass, PASSWORD) == 0) {
        kernel_print("\n[SUCCESS] Access Granted!\n", COLOR_LIGHT_GREEN);
        return 1;
    } else {
        kernel_print("\n[ERROR] Invalid Credentials!\n", COLOR_LIGHT_RED);
        return 0;
    }
}
