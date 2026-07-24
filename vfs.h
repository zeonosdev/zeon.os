// vfs.h
#ifndef VFS_H
#define VFS_H

#define MAX_FILENAME 32
#define MAX_CHILDREN 16
#define FS_FILE 1
#define FS_DIR  2

typedef struct vfs_node {
    char name[MAX_FILENAME];
    uint8_t type;
    char content[256];
    struct vfs_node* parent;
    struct vfs_node* children[MAX_CHILDREN];
    uint8_t child_count;
} vfs_node_t;

void vfs_init();
vfs_node_t* vfs_mkdir(vfs_node_t* parent, const char* name);
vfs_node_t* vfs_create_file(vfs_node_t* parent, const char* name, const char* content);
vfs_node_t* vfs_get_root();

#endif
