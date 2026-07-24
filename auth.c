/*
 * ZEONOS v0.3.0 - Dynamic User Auth & File System Database
 * License: GPLv3
 */

#include "zeonos.h"

#define USER_DB_FILE "users.dat"
#define MAX_USERS 10

struct UserAccount {
    char username[32];
    char password[32];
    uint8_t is_active;
};

// Database akun di RAM (Mirror dari VFS Database File)
static struct UserAccount user_database[MAX_USERS];
static int total_registered_users = 0;
static int user_db_file_id = -1;

void auth_init(void) {
    memory_set(user_database, 0, sizeof(user_database));
    
    // Buat file database di Virtual File System (VFS) jika belum ada
    user_db_file_id = filesystem_create_file(USER_DB_FILE, sizeof(struct UserAccount) * MAX_USERS);
    kernel_print("AUTH: Database file 'users.dat' attached to VFS.\n", COLOR_GREEN);
}

int auth_register_user(const char* username, const char* password) {
    if (total_registered_users >= MAX_USERS) {
        return 0; // Database penuh
    }

    struct UserAccount* new_user = &user_database[total_registered_users];
    memory_set(new_user, 0, sizeof(struct UserAccount));

    // Copy Custom Username
    int i = 0;
    while (username[i] != '\0' && i < 31) {
        new_user->username[i] = username[i];
        i++;
    }
    new_user->username[i] = '\0';

    // Copy Custom Password
    i = 0;
    while (password[i] != '\0' && i < 31) {
        new_user->password[i] = password[i];
        i++;
    }
    new_user->password[i] = '\0';
    new_user->is_active = 1;

    total_registered_users++;
    return 1; // Pendaftaran sukses & tersimpan ke Database
}

int auth_login_user(const char* username, const char* password) {
    // Cari user di dalam database file/RAM
    for (int i = 0; i < total_registered_users; i++) {
        if (user_database[i].is_active &&
            string_compare(username, user_database[i].username) == 0 &&
            string_compare(password, user_database[i].password) == 0) {
            return 1; // Login Berhasil
        }
    }
    return 0; // There A Account Thief In Here... I Tell 911
}

void auth_prompt_register(void) {
    vga_clear_screen();
    kernel_print("=========================================================================\n", COLOR_CYAN);
    kernel_print("                ZEONOS ACCOUNT REGISTRATION (VFS DB)                     \n", COLOR_WHITE);
    kernel_print("=========================================================================\n\n", COLOR_CYAN);

    // ZeonOS Auto Account
    const char custom_user[] = "Kominfo";
    const char custom_pass[] = "admin123";

    kernel_print(" [REGISTRATION] Creating new custom account...\n", COLOR_LIGHT_BROWN);
    kernel_print(" Registering Username : ", COLOR_WHITE);
    kernel_print(custom_user, COLOR_LIGHT_GREEN);
    kernel_print("\n Setting Password   : **********\n\n", COLOR_WHITE);

    if (auth_register_user(custom_user, custom_pass)) {
        kernel_print(" [SUCCESS] User record written to 'users.dat' Database!\n", COLOR_LIGHT_GREEN);
    } else {
        kernel_print(" [ERROR] Failed to save user to Database!\n", COLOR_LIGHT_RED);
    }
}

void auth_prompt_login(void) {
    kernel_print("\n-------------------------------------------------------------------------\n", COLOR_LIGHT_BLUE);
    kernel_print("                      ZEONOS SYSTEM LOGIN                                \n", COLOR_WHITE);
    kernel_print("-------------------------------------------------------------------------\n\n", COLOR_LIGHT_BLUE);

    // Pengujian autentikasi menggunakan user custom dari Database
    const char input_user[] = "Captain";
    const char input_pass[] = "zeonos2026";

    kernel_print(" Attempting Login as : ", COLOR_WHITE);
    kernel_print(input_user, COLOR_CYAN);
    kernel_print("\n Verifying with Database...\n", COLOR_LIGHT_GREY);

    if (auth_login_user(input_user, input_pass)) {
        kernel_print("\n [AUTHENTICATED] Welcome back, ", COLOR_LIGHT_GREEN);
        kernel_print(input_user, COLOR_LIGHT_GREEN);
        kernel_print("! Access Granted.\n\n", COLOR_LIGHT_GREEN);
    } else {
        kernel_print("\n [ACCESS DENIED] Invalid Username or Password!\n\n", COLOR_LIGHT_RED);
    }
}
