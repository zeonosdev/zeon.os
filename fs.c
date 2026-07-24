#include "zeonos_h"

struct FileSystem main_filesystem;

void filesystem_initialize(void) {
    memory_set(&main_filesystem, 0, sizeof(main_filesystem));
    main_filesystem.next_free_sector = 100;
    main_filesystem.is_mounted = 1;
    char label[] = "ZEONOS_ROOT";
    for(int i = 0; label[i] != '\0'; i++) main_filesystem.volume_label[i] = label[i];
    kernel_print("Filesystem: Initialized Successfully\n", COLOR_GREEN);
}

int filesystem_create_file(const char* filename, uint32_t size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!main_filesystem.files[i].is_used) {
            main_filesystem.files[i].is_used = 1;
            main_filesystem.files[i].start_sector = main_filesystem.next_free_sector;
            main_filesystem.files[i].file_size = size;
            int len = 0;
            while(filename[len] != '\0' && len < MAX_FILENAME_LEN - 1) {
                main_filesystem.files[i].filename[len] = filename[len];
                len++;
            }
            main_filesystem.files[i].filename[len] = '\0';
            main_filesystem.next_free_sector += (size + SECTOR_SIZE - 1) / SECTOR_SIZE;
            return i;
        }
    }
    return -1;
}

void filesystem_list_files(void) {
    kernel_print("--- File System Directory ---\n", COLOR_LIGHT_CYAN);
    for (int i = 0; i < MAX_FILES; i++) {
        if (main_filesystem.files[i].is_used) {
            kernel_print("File: ", COLOR_WHITE);
            kernel_print(main_filesystem.files[i].filename, COLOR_WHITE);
            kernel_print(" | Size: ", COLOR_WHITE);
            kernel_print_dec(main_filesystem.files[i].file_size, COLOR_WHITE);
            kernel_print(" bytes\n", COLOR_WHITE);
        }
    }
}
