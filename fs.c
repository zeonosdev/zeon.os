#include "zeonos.h"

struct FileSystem main_filesystem;

void filesystem_initialize(void) {
    memory_set(&main_filesystem, 0, sizeof(main_filesystem));
    main_filesystem.next_free_sector = 100;
    main_filesystem.is_mounted = 1;
    kernel_print("Filesystem: Virtual File System (VFS) Mounted\\n", COLOR_LIGHT_BLUE);
}

int filesystem_create_file(const char* filename, uint32_t size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!main_filesystem.files[i].is_used) {
            main_filesystem.files[i].is_used = 1;
            main_filesystem.files[i].file_size = size;
            int len = 0;
            while(filename[len] != '\0' && len < MAX_FILENAME_LEN - 1) {
                main_filesystem.files[i].filename[len] = filename[len];
                len++;
            }
            main_filesystem.files[i].filename[len] = '\0';
            return i;
        }
    }
    return -1;
}
