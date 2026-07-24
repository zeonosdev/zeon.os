#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ROLE_OWNER 0
#define ROLE_ADMIN 1
#define ROLE_USER  2

#define OFFICIAL_HASH "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026"
#define PASSWD_FILE "zeon_passwd.txt" // Simulasi file /etc/passwd di VFS

typedef struct {
    int uid;
    char username[32];
    int role;       // 0: OWNER, 1: ADMIN, 2: USER
    int is_banned;  // 0: Active, 1: Banned
} zeon_user_t;

// Variable global untuk Dynamic Database
zeon_user_t* user_db = NULL;
int total_users = 0;

// Helper Nama Role
const char* get_role_name(int role) {
    switch (role) {
        case ROLE_OWNER: return "OWNER";
        case ROLE_ADMIN: return "ADMIN";
        case ROLE_USER:  return "USER";
        default:         return "UNKNOWN";
    }
}

// 1. Fungsi Membaca File Database /etc/passwd secara Dinamis
void load_users_from_file() {
    FILE *file = fopen(PASSWD_FILE, "r");
    
    // Jika file belum ada, buat file awal dengan user default (Owner)
    if (file == NULL) {
        file = fopen(PASSWD_FILE, "w");
        fprintf(file, "0 captain 0 0\n"); // UID:0, username:captain, role:0 (OWNER), status:0
        fclose(file);
        file = fopen(PASSWD_FILE, "r");
    }

    // Reset Memory
    if (user_db != NULL) free(user_db);
    total_users = 0;

    zeon_user_t temp_user;
    // Baca baris demi baris dari file
    while (fscanf(file, "%d %s %d %d", &temp_user.uid, temp_user.username, &temp_user.role, &temp_user.is_banned) != EOF) {
        total_users++;
        // Alokasikan memori dinamis bertambah sesuai jumlah user di file
        user_db = (zeon_user_t*) realloc(user_db, total_users * sizeof(zeon_user_t));
        user_db[total_users - 1] = temp_user;
    }

    fclose(file);
}

// 2. Fitur Registrasi User Baru (Otomatis Nambah ke File & Dashboard)
void register_new_user() {
    char new_username[32];
    printf("\n [ FORM REGISTRASI USER BARU ]\n");
    printf("> Masukkan Username Baru: ");
    scanf("%s", new_username);

    // Cek apakah username sudah dipakai
    for (int i = 0; i < total_users; i++) {
        if (strcmp(user_db[i].username, new_username) == 0) {
            printf("\033[31m[ERROR] Username '%s' sudah terdaftar!\033[0m\n", new_username);
            return;
        }
    }

    // Hitung UID Baru (UID terakhis + 1)
    int new_uid = (total_users == 0) ? 1000 : user_db[total_users - 1].uid + 1;
    if (new_uid < 1000 && total_users > 0) new_uid = 1000; // Standar UID user biasa >= 1000

    // Simpan data user baru ke file
    FILE *file = fopen(PASSWD_FILE, "a");
    if (file != NULL) {
        // Default role untuk user terdaftar baru adalah USER (2) dan status Active (0)
        fprintf(file, "%d %s %d 0\n", new_uid, new_username, ROLE_USER);
        fclose(file);
        
        printf("\033[32m[SUCCESS] User '%s' (UID %d) berhasil terdaftar!\033[0m\n", new_username, new_uid);
        
        // Muat ulang database agar Dashboard langsung ter-update
        load_users_from_file();
    }
}

// 3. Fungsi Menampilkan Tabel User
void print_user_table() {
    printf("\n [ USER MANAGEMENT TABLE (Total: %d Users) ]\n", total_users);
    printf(" ---------------------------------------------------------\n");
    printf("  UID   | Username        | Role     | Status     \n");
    printf(" ---------------------------------------------------------\n");
    for (int i = 0; i < total_users; i++) {
        printf("  %-5d | %-15s | %-8s | %s\n", 
            user_db[i].uid, 
            user_db[i].username, 
            get_role_name(user_db[i].role), 
            user_db[i].is_banned ? "\033[31m[BANNED]\033[0m" : "\033[32m[ACTIVE]\033[0m");
    }
    printf(" ---------------------------------------------------------\n");
}

// 4. Update File saat ada perubahan Status/Role (Ban / Promote)
void save_all_users_to_file() {
    FILE *file = fopen(PASSWD_FILE, "w");
    if (file != NULL) {
        for (int i = 0; i < total_users; i++) {
            fprintf(file, "%d %s %d %d\n", user_db[i].uid, user_db[i].username, user_db[i].role, user_db[i].is_banned);
        }
        fclose(file);
    }
}

void action_ban_user(zeon_user_t* current_user) {
    int target_uid;
    printf("\n> Masukkan UID user yang ingin di-BAN: ");
    scanf("%d", &target_uid);

    for (int i = 0; i < total_users; i++) {
        if (user_db[i].uid == target_uid) {
            if (user_db[i].role == ROLE_OWNER) {
                printf("\033[31m[ERROR] GAGAL: Akun OWNER tidak bisa di-ban!\033[0m\n");
                return;
            }
            user_db[i].is_banned = 1;
            save_all_users_to_file(); // Simpan perubahan ke file
            printf("\033[32m[SUCCESS] User '%s' (UID %d) berhasil DI-BAN!\033[0m\n", user_db[i].username, target_uid);
            return;
        }
    }
    printf("\033[31m[ERROR] UID tidak ditemukan!\033[0m\n");
}

int main() {
    char local_system_hash[] = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026"; 
    
    // Load database dari file saat program pertama kali dibuka
    load_users_from_file();
    
    // User yang login saat ini (Index 0 = Captain / Owner)
    zeon_user_t* current_user = &user_db[0]; 

    int choice;
    while (1) {
        printf("\n=========================================================\n");
        printf("              ZEONOS CONTROL CENTER                      \n");
        if (strcmp(local_system_hash, OFFICIAL_HASH) == 0) {
            printf("              STATUS: \033[32m[OFFICIAL BUILD]\033[0m 🟢          \n");
        } else {
            printf("              STATUS: \033[31m[WARNING: MODIF BUILD]\033[0m 🔴    \n");
        }
        printf("=========================================================\n");
        printf(" Logged in as : %s | Role: \033[33m%s\033[0m\n", current_user->username, get_role_name(current_user->role));
        printf("=========================================================\n");

        print_user_table();

        printf("\n [ MENU ]\n");
        printf(" 1. Register User Baru (Simulasi User Masuk)\n");
        printf(" 2. Ban User\n");
        printf(" 3. Keluar Dashboard\n");
        printf(" Pilih Opsi [1-3]: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                register_new_user();
                break;
            case 2:
                action_ban_user(current_user);
                break;
            case 3:
                printf("\nExiting. Memory freed.\n");
                if (user_db != NULL) free(user_db);
                return 0;
            default:
                printf("\n[ERROR] Opsi tidak valid!\n");
        }
    }
    return 0;
}
