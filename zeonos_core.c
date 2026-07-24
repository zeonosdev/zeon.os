#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 10
#define MAX_LOGS 20
#define OFFICIAL_HASH "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026"

// ---------------------------------------------------------
// 1. STRUKTUR DATA (VFS & SYSTEM LOGS)
// ---------------------------------------------------------

typedef struct {
    char name[32];
    int size_kb;
    char owner[16];
    int is_readonly;
} VFSFile;

typedef struct {
    int log_id;
    char timestamp[10];
    char user[16];
    char action[64];
    char status[12]; // "SUCCESS" atau "BLOCKED"
} SystemLog;

// Database Global VFS & Log
VFSFile vfs_files[MAX_FILES];
int file_count = 0;

SystemLog sys_logs[MAX_LOGS];
int log_count = 0;

// ---------------------------------------------------------
// 2. FUNGSI MASCOT & SYSTEM HELPER
// ---------------------------------------------------------

void print_fust_c_banner() {
    printf("\n");
    printf("       \\ \\___/ /\n");
    printf("      (  (O) (O) )    <-- \"FUST C-KERNEL SENTINEL ACTIVE\"\n");
    printf("       \\   u   /\n");
    printf("        \\_____/\n");
    printf("      /         \\\n");
    printf("     (           )\n");
    printf("      \\_________/\n");
}

void add_log(const char* user, const char* action, const char* status) {
    if (log_count < MAX_LOGS) {
        sys_logs[log_count].log_id = log_count + 1;
        strcpy(sys_logs[log_count].timestamp, "12:00:00");
        strcpy(sys_logs[log_count].user, user);
        strcpy(sys_logs[log_count].action, action);
        strcpy(sys_logs[log_count].status, status);
        log_count++;
    }
}

void init_vfs() {
    // Menyiapkan file-file bawaan VFS ZEONOS
    strcpy(vfs_files[0].name, "zeon_passwd.json");
    vfs_files[0].size_kb = 4;
    strcpy(vfs_files[0].owner, "captain");
    vfs_files[0].is_readonly = 1;

    strcpy(vfs_files[1].name, "kernel_core.bin");
    vfs_files[1].size_kb = 512;
    strcpy(vfs_files[1].owner, "system");
    vfs_files[1].is_readonly = 1;

    file_count = 2;

    add_log("SYSTEM", "VFS Subsystem Initialized", "SUCCESS");
    add_log("SYSTEM", "Security Log Sentinel Ready", "SUCCESS");
}

// ---------------------------------------------------------
// 3. FUNGSI UTAMA VFS & LOG MONITOR
// ---------------------------------------------------------

void show_vfs() {
    printf("\n=========================================================\n");
    printf("           ZEONOS VIRTUAL FILE SYSTEM (VFS)             \n");
    printf("=========================================================\n");
    printf(" %-4s | %-20s | %-8s | %-10s | %-8s\n", "ID", "File Name", "Size", "Owner", "Access");
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < file_count; i++) {
        char access_str[10];
        if (vfs_files[i].is_readonly) {
            strcpy(access_str, "[RO]");
        } else {
            strcpy(access_str, "[RW]");
        }
        printf(" %-4d | %-20s | %-5d KB | %-10s | %-8s\n", 
               i + 1, vfs_files[i].name, vfs_files[i].size_kb, vfs_files[i].owner, access_str);
    }
    printf("---------------------------------------------------------\n");
}

void create_file(const char* current_user) {
    if (file_count >= MAX_FILES) {
        printf("\n\x1b[31m[ERROR] Disk VFS Penuh! Fust menolak pembuatan file.\x1b[0m\n");
        add_log(current_user, "Create File Attempt", "BLOCKED");
        return;
    }

    char filename[32];
    int size;

    printf("\n> Masukkan Nama File Baru: ");
    scanf("%31s", filename);
    printf("> Masukkan Ukuran File (KB): ");
    scanf("%d", &size);

    strcpy(vfs_files[file_count].name, filename);
    vfs_files[file_count].size_kb = size;
    strcpy(vfs_files[file_count].owner, current_user);
    vfs_files[file_count].is_readonly = 0;

    file_count++;

    printf("\x1b[32m[SUCCESS] File '%s' berhasil dibuat di VFS!\x1b[0m\n", filename);
    
    char log_msg[64];
    snprintf(log_msg, sizeof(log_msg), "Created file '%s'", filename);
    add_log(current_user, log_msg, "SUCCESS");
}

void show_logs() {
    printf("\n=========================================================\n");
    printf("              ZEONOS SECURITY LOG SENTINEL               \n");
    printf("=========================================================\n");
    printf(" %-4s | %-10s | %-10s | %-25s | %-8s\n", "ID", "Time", "User", "Action", "Status");
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < log_count; i++) {
        printf(" %-4d | %-10s | %-10s | %-25s | %s\n",
               sys_logs[i].log_id,
               sys_logs[i].timestamp,
               sys_logs[i].user,
               sys_logs[i].action,
               sys_logs[i].status);
    }
    printf("---------------------------------------------------------\n");
}

// ---------------------------------------------------------
// 4. MAIN ENTRY POINT
// ---------------------------------------------------------

int main() {
    char current_user[16] = "captain";
    char local_hash[64] = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026";
    int choice;

    init_vfs();

    while (1) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        print_fust_c_banner();
        printf("=========================================================\n");
        printf("            ZEONOS C-KERNEL DUAL MODULE                  \n");
        if (strcmp(local_hash, OFFICIAL_HASH) == 0) {
            printf("            STATUS: \x1b[32m[OFFICIAL BUILD] 🟢\x1b[0m\n");
        } else {
            printf("            STATUS: \x1b[31m[MODIFIED BUILD] 🔴\x1b[0m\n");
        }
        printf("=========================================================\n");
        printf(" Logged in as: %s (OWNER)\n", current_user);
        printf("=========================================================\n");

        printf("\n [ MENU OPERASI C-KERNEL ]\n");
        printf(" 1. Lihat Virtual File System (VFS)\n");
        printf(" 2. Buat File Baru di VFS\n");
        printf(" 3. Lihat Audit System Logs (Fust Sentinel)\n");
        printf(" 4. Keluar Kernel\n");
        printf("\n Pilih menu [1-4]: ");
        
        if (scanf("%d", &choice) != 1) {
            break;
        }

        switch (choice) {
            case 1:
                show_vfs();
                printf("\nTekan Enter untuk lanjut...");
                getchar(); getchar();
                break;
            case 2:
                create_file(current_user);
                printf("\nTekan Enter untuk lanjut...");
                getchar(); getchar();
                break;
            case 3:
                show_logs();
                printf("\nTekan Enter untuk lanjut...");
                getchar(); getchar();
                break;
            case 4:
                add_log(current_user, "Shutdown Kernel", "SUCCESS");
                printf("\n\x1b[33m[FUST]\x1b[0m C-Kernel Shutdown. Sampai jumpa, Captain!\n");
                return 0;
            default:
                printf("\n\x1b[31m[ERROR] Opsi tidak valid!\x1b[0m\n");
                printf("\nTekan Enter untuk lanjut...");
                getchar(); getchar();
                break;
        }
    }

    return 0;
}
