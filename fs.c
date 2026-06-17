#include "fs.h"
#include "helper.h"

int fs_format(const char *disk_path) {
    // TODO: Implement according to requirements
    return 0;
}

int fs_mount(const char *disk_path) {
    // TODO: Implement according to requirements
    return 0;
}

void fs_unmount() {
    // TODO: Implement according to requirements
}

int fs_create(const char *filename) {
    // TODO: Implement according to requirements
    return 0;
}

int fs_delete(const char *filename) {
    // TODO: Implement according to requirements
    return 0;
}

int fs_list(char filenames[][MAX_FILENAME], int max_files) {
    // TODO: Implement according to requirements
    return 0;
}

int fs_write(const char *filename, const void *data, int size) {
    // TODO: Implement according to requirements
    return 0;
}

int fs_read(const char *filename, void *buffer, int size) {
    // TODO: Implement according to requirements
    return 0;
}

//Helper functions

int find_inode(const char *filename) {
    // TODO: Implement according to requirements
    return 0;
}

int find_free_inode() {
    // TODO: Implement according to requirements
    return 0;
}

int find_free_block() {
    // TODO: Implement according to requirements
    return 0;
}

void mark_block_used(int block_num) {
    // TODO: Implement according to requirements
}

void mark_block_free(int block_num) {
    // TODO: Implement according to requirements
}


void read_inode(int inode_num, inode *target) {
    // TODO: Implement according to requirements
}

void write_inode(int inode_num, const inode *source) {
    // TODO: Implement according to requirements
}