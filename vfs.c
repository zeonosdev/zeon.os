// vfs.c
#include "vfs.h"
#include <string.h>

static vfs_node_t root;
static vfs_node_t nodes[64];
static int node_index = 0;

vfs_node_t* vfs_get_root() { return &root; }

vfs_node_t* vfs_mkdir(vfs_node_t* parent, const char* name) {
    vfs_node_t* dir = &nodes[node_index++];
    strcpy(dir->name, name);
    dir->type = FS_DIR;
    dir->parent = parent;
    dir->child_count = 0;
    if (parent && parent->child_count < MAX_CHILDREN) {
        parent->children[parent->child_count++] = dir;
    }
    return dir;
}

vfs_node_t* vfs_create_file(vfs_node_t* parent, const char* name, const char* content) {
    vfs_node_t* file = &nodes[node_index++];
    strcpy(file->name, name);
    file->type = FS_FILE;
    strcpy(file->content, content);
    file->parent = parent;
    if (parent && parent->child_count < MAX_CHILDREN) {
        parent->children[parent->child_count++] = file;
    }
    return file;
}

void vfs_init() {
    strcpy(root.name, "/");
    root.type = FS_DIR;
    root.parent = NULL;

    // Pohon folder standar Linux
    vfs_node_t* bin = vfs_mkdir(&root, "bin");
    vfs_node_t* etc = vfs_mkdir(&root, "etc");
    vfs_node_t* home = vfs_mkdir(&root, "home");
    vfs_mkdir(home, "captain");

    // File Konfigurasi Unix & User Auth
    vfs_create_file(etc, "passwd", "root:x:0:0:Superuser:/root:/bin/zsh\ncaptain:x:1000:1000:Captain User:/home/captain:/bin/zeon-shell");
    vfs_create_file(bin, "sh", "ZEON-Shell Binary Executable");
}
