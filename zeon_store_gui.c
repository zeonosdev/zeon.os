#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simulated GUI App Structure
typedef struct {
    int id;
    char name[32];
    char category[16];
    char version[10];
    int size_mb;
    int is_installed;
    int is_verified_by_fust;
} AppItem;

#define TOTAL_APPS 4
AppItem store_apps[TOTAL_APPS];

void init_store_data() {
    // App 1
    store_apps[0].id = 1;
    strcpy(store_apps[0].name, "Zeon Terminal");
    strcpy(store_apps[0].category, "System");
    strcpy(store_apps[0].version, "v1.2");
    store_apps[0].size_mb = 12;
    store_apps[0].is_installed = 1;
    store_apps[0].is_verified_by_fust = 1;

    // App 2
    store_apps[1].id = 2;
    strcpy(store_apps[1].name, "File Explorer GUI");
    strcpy(store_apps[1].category, "Utility");
    strcpy(store_apps[1].version, "v2.0");
    store_apps[1].size_mb = 45;
    store_apps[1].is_installed = 0;
    store_apps[1].is_verified_by_fust = 1;

    // App 3
    store_apps[2].id = 3;
    strcpy(store_apps[2].name, "Fust Security Sentinel");
    strcpy(store_apps[2].category, "Security");
    strcpy(store_apps[2].version, "v3.1");
    store_apps[2].size_mb = 28;
    store_apps[2].is_installed = 1;
    store_apps[2].is_verified_by_fust = 1;

    // App 4
    store_apps[3].id = 4;
    strcpy(store_apps[3].name, "Zeon Code Editor");
    strcpy(store_apps[3].category, "Development");
    strcpy(store_apps[3].version, "v1.0");
    store_apps[3].size_mb = 85;
    store_apps[3].is_installed = 0;
    store_apps[3].is_verified_by_fust = 1;
}

// GUI Layout & Window Simulator Rendering
void render_zeon_store_gui() {
    printf("\n");
    printf("+-------------------------------------------------------------------------+\n");
    printf("|  [O][X]  ZEON STORE - Official App Center                               |\n");
    printf("+-------------------------------------------------------------------------+\n");
    printf("|  / \\___/ \\   |  Welcome to Zeon Store, Captain!                        |\n");
    printf("| (  (O)(O) )  |  All apps are verified by Fust Security Guard 👁️⚡       |\n");
    printf("|  \\   u   /   |                                                         |\n");
    printf("+-------------------------------------------------------------------------+\n");
    printf("|  ID | APP NAME            | CATEGORY    | SIZE  | FUST CHECK | STATUS   |\n");
    printf("+-----+---------------------+-------------+-------+------------+----------+\n");

    for (int i = 0; i < TOTAL_APPS; i++) {
        char status_btn[20];
        if (store_apps[i].is_installed) {
            strcpy(status_btn, "[INSTALLED]");
        } else {
            strcpy(status_btn, "[INSTALL]");
        }

        char fust_badge[10] = "PASSED";

        printf("|  %-2d | %-19s | %-11s | %-2d MB | %-10s | %-8s |\n",
               store_apps[i].id,
               store_apps[i].name,
               store_apps[i].category,
               store_apps[i].size_mb,
               fust_badge,
               status_btn);
    }
    printf("+-------------------------------------------------------------------------+\n");
}

void install_app_gui(int app_id) {
    if (app_id < 1 || app_id > TOTAL_APPS) {
        printf("\n\x1b[31m[GUI ERROR] Invalid Application ID!\x1b[0m\n");
        return;
    }

    int index = app_id - 1;
    if (store_apps[index].is_installed) {
        printf("\n\x1b[33m[INFO] '%s' is already installed on ZEONOS!\x1b[0m\n", store_apps[index].name);
    } else {
        printf("\n\x1b[32m[FUST SCAN] Verifying checksum for '%s'...\x1b[0m\n", store_apps[index].name);
        printf("\x1b[32m[SUCCESS] '%s' (%d MB) installed successfully!\x1b[0m\n", store_apps[index].name, store_apps[index].size_mb);
        store_apps[index].is_installed = 1;
    }
}

int main() {
    int choice;
    init_store_data();

    while (1) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        render_zeon_store_gui();

        printf("\n [ GUI STORE ACTIONS ]\n");
        printf(" 1. Click App ID to Install/Check\n");
        printf(" 2. Close Zeon Store Window\n");
        printf("\n Select GUI Action [1-2]: ");

        if (scanf("%d", &choice) != 1) break;

        if (choice == 1) {
            int target_id;
            printf("> Enter App ID to Click/Install: ");
            scanf("%d", &target_id);
            install_app_gui(target_id);
            printf("\nPress Enter to continue...");
            getchar(); getchar();
        } else if (choice == 2) {
            printf("\n\x1b[33m[FUST]\x1b[0m Closing Zeon Store Window...\n");
            break;
        }
    }

    return 0;
}
