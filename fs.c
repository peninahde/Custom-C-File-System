#include "fs.h"
#include "helper.h"

//Peninah
int fs_format(const char *disk_path) {
    // TODO: Implement according to requirements 
    // Initialize Superblock
    // Initialize Block Bitmap
    // Initialize Inode Table
    // Initialize Data Blocks
    return 0;
}
//Avi
int fs_mount(const char *disk_path) {
    // TODO: Implement according to requirements
    // read superblock and write it to the memory

    return 0;
}
//Peninah
void fs_unmount() {
    // TODO: Implement according to requirements
    // write any changed data back into the file and clear the memory
}
//Peninah
int fs_create(const char *filename) {
    // TODO: Implement according to requirements
    return 0;
}
//Avi
int fs_delete(const char *filename) {
    // TODO: Implement according to requirements
    return 0;
}
//Avi
int fs_list(char filenames[][MAX_FILENAME], int max_files) {
    // TODO: Implement according to requirements
    return 0;
}
//Avi
int fs_write(const char *filename, const void *data, int size) {
    // TODO: Implement according to requirements
    return 0;
}
//Peninah
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