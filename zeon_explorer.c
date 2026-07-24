#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 50
#define OFFICIAL_HASH "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026"

// Data Structure for File or Directory
typedef struct {
    char name[64];
    int is_dir;          // 1 for Directory, 0 for File
    int size_bytes;      // File size (0 for directory)
    char permissions[5]; // e.g., "rwx" or "r--"
} VFSItem;

// Virtual File System Database for File Explorer
VFSItem explorer_vfs[MAX_ITEMS];
int item_count = 0;

// Display Fust Mascot Banner for File Explorer
void print_fust_explorer() {
    printf("\n");
    printf("       \\ \\___/ /\n");
    printf("      (  (O) (O) )    <-- \"FUST FILE EXPLORER SENTINEL\"\n");
    printf("       \\   u   /\n");
    printf("        \\_____/\n");
    printf("      /         \\\n");
    printf("     (           )\n");
    printf("      \\_________/\n");
}

// Initialize Root Directory
void init_explorer_vfs() {
    // System Directory
    strcpy(explorer_vfs[0].name, "bin");
    explorer_vfs[0].is_dir = 1;
    explorer_vfs[0].size_bytes = 0;
    strcpy(explorer_vfs[0].permissions, "r-x");

    // Captain's Home Directory
    strcpy(explorer_vfs[1].name, "home");
    explorer_vfs[1].is_dir = 1;
    explorer_vfs[1].size_bytes = 0;
    strcpy(explorer_vfs[1].permissions, "rwx");

    // Configuration File
    strcpy(explorer_vfs[2].name, "zeon_passwd.json");
    explorer_vfs[2].is_dir = 0;
    explorer_vfs[2].size_bytes = 1024;
    strcpy(explorer_vfs[2].permissions, "r--");

    // OS Core Kernel File
    strcpy(explorer_vfs[3].name, "kernel_core.bin");
    explorer_vfs[3].is_dir = 0;
    explorer_vfs[3].size_bytes = 524288;
    strcpy(explorer_vfs[3].permissions, "r-x");

    item_count = 4;
}

// Function to List Files and Directories
void list_directory() {
    print_fust_explorer();
    printf("=========================================================\n");
    printf("               ZEONOS ROOT DIRECTORY (/)                 \n");
    printf("=========================================================\n");
    printf(" %-6s | %-25s | %-12s | %-10s\n", "TYPE", "NAME", "SIZE", "PERM");
    printf("---------------------------------------------------------\n");

    for (int i = 0; i < item_count; i++) {
        char type_str[8];
        if (explorer_vfs[i].is_dir) {
            strcpy(type_str, "\x1b[34m[DIR]\x1b[0m");
        } else {
            strcpy(type_str, "\x1b[32m[FILE]\x1b[0m");
        }

        char size_str[16];
        if (explorer_vfs[i].is_dir) {
            strcpy(size_str, "-");
        } else {
            snprintf(size_str, sizeof(size_str), "%d B", explorer_vfs[i].size_bytes);
        }

        printf(" %-6s | %-25s | %-12s | %-10s\n",
               type_str,
               explorer_vfs[i].name,
               size_str,
               explorer_vfs[i].permissions);
    }
    printf("---------------------------------------------------------\n");
}

// Function to Create a New File
void create_new_file() {
    if (item_count >= MAX_ITEMS) {
        printf("\n\x1b[31m[ERROR] VFS Storage Full! Fust denied file creation.\x1b[0m\n");
        return;
    }

    char filename[64];
    int size;

    printf("\n> Enter New File Name: ");
    scanf("%63s", filename);
    printf("> Enter File Size (Bytes): ");
    scanf("%d", &size);

    strcpy(explorer_vfs[item_count].name, filename);
    explorer_vfs[item_count].is_dir = 0;
    explorer_vfs[item_count].size_bytes = size;
    strcpy(explorer_vfs[item_count].permissions, "rw-");
    
    item_count++;
    printf("\n\x1b[32m[SUCCESS] File '%s' created successfully!\x1b[0m\n", filename);
}

// Function to Remove File or Directory
void delete_item() {
    char target[64];
    printf("\n> Enter File/Folder Name to Delete: ");
    scanf("%63s", target);

    // Protected core files
    if (strcmp(target, "kernel_core.bin") == 0 || strcmp(target, "zeon_passwd.json") == 0) {
        printf("\n\x1b[31m[SECURITY ALERT] Fust Denied! Core system files are strictly protected!\x1b[0m\n");
        return;
    }

    int found_index = -1;
    for (int i = 0; i < item_count; i++) {
        if (strcmp(explorer_vfs[i].name, target) == 0) {
            found_index = i;
            break;
        }
    }

    if (found_index != -1) {
        for (int i = found_index; i < item_count - 1; i++) {
            explorer_vfs[i] = explorer_vfs[i + 1];
        }
        item_count--;
        printf("\n\x1b[32m[SUCCESS] '%s' has been deleted.\x1b[0m\n", target);
    } else {
        printf("\n\x1b[31m[ERROR] File or directory '%s' not found!\x1b[0m\n", target);
    }
}

int main() {
    char local_hash[64] = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026";
    int choice;

    init_explorer_vfs();

    while (1) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        list_directory();

        printf("\n [ ZEONOS FILE EXPLORER MENU ]\n");
        printf(" 1. Create New File (Touch)\n");
        printf(" 2. Remove File / Directory (RM)\n");
        printf(" 3. Exit File Explorer\n");
        printf("\n Select Option [1-3]: ");

        if (scanf("%d", &choice) != 1) {
            break;
        }

        switch (choice) {
            case 1:
                create_new_file();
                printf("\nPress Enter to continue...");
                getchar(); getchar();
                break;
            case 2:
                delete_item();
                printf("\nPress Enter to continue...");
                getchar(); getchar();
                break;
            case 3:
                printf("\n\x1b[33m[FUST]\x1b[0m File Explorer closed. See you, Captain!\n");
                return 0;
            default:
                printf("\n\x1b[31m[ERROR] Invalid option!\x1b[0m\n");
                printf("\nPress Enter to continue...");
                getchar(); getchar();
                break;
        }
    }

    return 0;
}
