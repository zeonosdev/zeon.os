// shell.c
#include "keyboard.h"
#include "vfs.h"
#include <stdio.h>
#include <string.h>

static vfs_node_t* current_dir;

void cmd_neofetch() {
    printf("\n");
    printf("    /\033[36m    user\033[0m@\033[36mzeonos\033[0m\n");
    printf("   /  \\   ---------------\n");
    printf("  / /\\ \\  OS: ZEONOS x86 Kernel\n");
    printf(" / /__\\ \\ Kernel: 1.0.0-zeon-generic\n");
    printf("/______/\\ Shell: zeon-shell v1.0\n");
    printf("  \\____\\/ Memory: 64MB / 512MB RAM\n\n");
}

void cmd_ls() {
    for (int i = 0; i < current_dir->child_count; i++) {
        vfs_node_t* child = current_dir->children[i];
        if (child->type == FS_DIR) printf("\033[34m%s/\033[0m  ", child->name);
        else printf("%s  ", child->name);
    }
    printf("\n");
}

void cmd_cat(const char* filename) {
    for (int i = 0; i < current_dir->child_count; i++) {
        vfs_node_t* child = current_dir->children[i];
        if (strcmp(child->name, filename) == 0) {
            if (child->type == FS_FILE) printf("%s\n", child->content);
            else printf("cat: %s: Is a directory\n", filename);
            return;
        }
    }
    printf("cat: %s: No such file or directory\n", filename);
}

void shell_run() {
    current_dir = vfs_get_root();
    char line[128];
    int pos = 0;

    printf("\nWelcome to ZEONOS Linux Workstation v1.0!\n");
    printf("Type 'neofetch' or 'ls' to start exploring.\n\n");

    while (1) {
        printf("\033[32mcaptain@zeonos\033[0m:\033[34m%s\033[0m$ ", current_dir->name);
        pos = 0;

        while (1) {
            char c = keyboard_getchar();
            if (c == '\n') {
                printf("\n");
                line[pos] = '\0';
                break;
            } else if (c == '\b') {
                if (pos > 0) {
                    pos--;
                    printf("\b \b");
                }
            } else {
                line[pos++] = c;
                putchar(c);
            }
        }

        if (strcmp(line, "neofetch") == 0) cmd_neofetch();
        else if (strcmp(line, "ls") == 0) cmd_ls();
        else if (strncmp(line, "cat ", 4) == 0) cmd_cat(line + 4);
        else if (strcmp(line, "whoami") == 0) printf("YOU IS ADMIN\n");
        else if (strncmp(line, "touch ", 6) == 0) vfs_create_file(current_dir, line + 6, "");
        else if (strncmp(line, "mkdir ", 6) == 0) vfs_mkdir(current_dir, line + 6);
        else if (pos > 0) printf("zeon-shell: command not found: %s\n", line);
    }
}
print("error1.1");
