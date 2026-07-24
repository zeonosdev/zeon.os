#include "zeonos.h"

#define USER_DB_FILE "users.dat"
#define MAX_USERS 10

static struct UserAccount user_database[MAX_USERS];
static int total_registered_users = 0;

void auth_init(void) {
    memory_set(user_database, 0, sizeof(user_database));
    filesystem_create_file(USER_DB_FILE, sizeof(struct UserAccount) * MAX_USERS);
    kernel_print("AUTH: Database 'users.dat' berhasil di-attach.\\n", COLOR_LIGHT_BLUE);
}

int auth_register_user(const char* username, const char* password) {
    if (total_registered_users >= MAX_USERS) return 0;

    struct UserAccount* new_user = &user_database[total_registered_users];
    memory_set(new_user, 0, sizeof(struct UserAccount));

    int i = 0;
    while (username[i] != '\0' && i < 31) {
        new_user->username[i] = username[i];
        i++;
    }
    new_user->username[i] = '\0';

    i = 0;
    while (password[i] != '\0' && i < 31) {
        new_user->password[i] = password[i];
        i++;
    }
    new_user->password[i] = '\0';
    new_user->is_active = 1;

    total_registered_users++;
    return 1;
}

int auth_login_user(const char* username, const char* password) {
    for (int i = 0; i < total_registered_users; i++) {
        if (user_database[i].is_active &&
            string_compare(username, user_database[i].username) == 0 &&
            string_compare(password, user_database[i].password) == 0) {
            return 1;
        }
    }
    return 0;
}

void auth_prompt_register(void) {
    vga_clear_screen();
    // Warna Menu Teks Biru (COLOR_LIGHT_BLUE)
    kernel_print("=========================================================================\n", COLOR_LIGHT_BLUE);
    kernel_print("                ZEONOS ACCOUNT REGISTRATION (VFS DB)                     \n", COLOR_LIGHT_BLUE);
    kernel_print("=========================================================================\n\n", COLOR_LIGHT_BLUE);

    const char custom_user[] = "Captain";
    const char custom_pass[] = "zeonos2026";

    kernel_print(" [REGISTRATION] Membuat akun baru...\n", COLOR_LIGHT_CYAN);
    kernel_print(" Username Custom : ", COLOR_LIGHT_BLUE);
    kernel_print(custom_user, COLOR_LIGHT_CYAN);
    kernel_print("\n Password        : **********\n\n", COLOR_LIGHT_BLUE);

    if (auth_register_user(custom_user, custom_pass)) {
        kernel_print(" [SUCCESS] Akun berhasil disimpan ke 'users.dat' Database!\n", COLOR_LIGHT_GREEN);
    } else {
        kernel_print(" [ERROR] Gagal menyimpan akun ke Database!\n", COLOR_LIGHT_RED);
    }
}

void auth_prompt_login(void) {
    kernel_print("\n-------------------------------------------------------------------------\n", COLOR_LIGHT_BLUE);
    kernel_print("                      ZEONOS SYSTEM LOGIN                                \n", COLOR_LIGHT_BLUE);
    kernel_print("-------------------------------------------------------------------------\n\n", COLOR_LIGHT_BLUE);

    const char input_user[] = "Captain";
    const char input_pass[] = "zeonos2026";

    kernel_print(" Memverifikasi Login sebagai : ", COLOR_LIGHT_BLUE);
    kernel_print(input_user, COLOR_LIGHT_CYAN);
    kernel_print("\n Memeriksa data dengan database...\n", COLOR_LIGHT_BLUE);

    if (auth_login_user(input_user, input_pass)) {
        kernel_print("\n [AUTHENTICATED] Selamat Datang Kembali, ", COLOR_LIGHT_GREEN);
        kernel_print(input_user, COLOR_LIGHT_GREEN);
        kernel_print("! Akses Diterima.\n\n", COLOR_LIGHT_GREEN);
    } else {
        kernel_print("\n [ACCESS DENIED] Username atau Password Salah!\n\n", COLOR_LIGHT_RED);
    }
}
