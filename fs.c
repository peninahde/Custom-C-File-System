#include "fs.h"
#include "helper.h"

static int disk_fd = -1;
static superblock sb;
static unsigned char bitmap[MAX_BLOCKS / 8];
static inode inode_table[MAX_FILES]; //256 inodes
//Creates and initializes a virtual disk file and prepares the filesystem.
int fs_format(const char *disk_path) {
    // Initialize Superblock (4KB = 1 block)
    sb.total_blocks = MAX_BLOCKS;
    sb.block_size = BLOCK_SIZE;
    sb.free_blocks = 2550; //TODO: can we make it a non-constant?
    sb.total_inodes = MAX_FILES;
    sb.free_inodes = 256; //TODO: can we make it a non-constant?

    // Initialize Block Bitmap (4KB = 1 block)
    for (int i = 0; i < (MAX_BLOCKS / 8); i++){
        bitmap[i] = 0;
    }
    //mark the first 10 blocks as used
    bitmap[0] = 0xFF; //marks the first 8 blocks as used using the first 8 bits
    bitmap[1] = 0x03; //marks the next 2 blocks are used using the first 2 bits of the second byte

    // Initialize Inode Table: 256 inodes, 128 bytes each = 32 KB (8 blocks)
    for (int i = 0; i < MAX_FILES; i++) {
        inode_table[i].used = 0;
        inode_table[i].size = 0;

        for (int j = 0; j < MAX_FILENAME; j++) { //zero out the file name array for each inode
            inode_table[i].name[j] = '\0';
        }

        for (int j = 0; j < MAX_DIRECT_BLOCKS; j++) { //zero out the array of direct block pointers
            inode_table[i].blocks[j] = -1;
        }
    }

    // Initialize Data Blocks (2550 blocks) - open the virtual disk and write until the end of teh blocks
    int disk_fd = open(disk_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (disk_fd == -1) return -1; 

    write(disk_fd, &sb, sizeof(sb)); //write the superblock (block 0)
    lseek(disk_fd, 1 * BLOCK_SIZE, SEEK_SET); //jump to the end of the superblock block

    write(disk_fd, bitmap, sizeof(bitmap)); //write the bitmap (block 1)
    lseek(disk_fd, 2 * BLOCK_SIZE, SEEK_SET); // jump to the end of the bitmap block
    //TODO: Might not be able to do 2*BLOCK_SIZE if not contiguous
    write(disk_fd, inode_table, sizeof(inode_table)); //write the inode table (8 blocks)

    lseek(disk_fd, (MAX_BLOCKS * BLOCK_SIZE) - 1, SEEK_SET); //jump to the last byte of the 10MB file
    write(disk_fd, "\0",1); //write null in the last byte which secures us the full 10MB file size

    close(disk_fd);

    return 0;
}
//Avi
int fs_mount(const char *disk_path) {
    // TODO: Implement according to requirements

    return 0;
}
//Ensures all pending changes are written to disk and closes the filesystem 
void fs_unmount() {
    // write any changed data back into the file and clear the memory
    if (disk_fd == -1) return;

    // read superblock and write it to  memory
    lseek(disk_fd, 0, SEEK_SET); //go to beginning of superblock
    write(disk_fd, &sb, sizeof(sb)); //write the contents of sb to the disk

    // read bitmap and write it to memory
    lseek(disk_fd, 1 * BLOCK_SIZE, SEEK_SET); //go to beginning of bitmap block
    write(disk_fd, bitmap, sizeof(bitmap)); //write the contents of bitmap to the disk

    // read inode table and write it to memory
    lseek(disk_fd, 2 * BLOCK_SIZE, SEEK_SET); //go to beginning of inode_table block
    write(disk_fd, inode_table, sizeof(inode_table)); //write the contents of inode_table to the disk

    //close disk
    close(disk_fd);
    disk_fd = -1;
}

int fs_create(const char *filename) {
    // check if file name is legal
    if (filename == NULL || strlen(filename) >= MAX_FILENAME) {
        return -3;
    }

    // check if the filename already exists
    // returns -1 if file already exist
    if (find_inode(filename) != -1) {
        return -1; //filename is already in use
    }

    // finds a free inode
    // returns -2 if there are no free inodes available
    if (sb.free_inodes <= 0) {
        return -2; // no inodes available
    }

    // get index of the next free inode
    int free_inode_index = find_free_inode();
    if (free_inode_index == -1) {
        return -2; //just in case the sb.free_inodes wasn't working
    }

    // initialize the inode with the filename and zero size
    // clear out old name and put in the new name
    strncpy(inode_table[free_inode_index].name, filename, MAX_FILENAME); //TODO: Why using strncpy and not =?

    //set the size to 0
    inode_table[free_inode_index].size = 0;

    // update the blocks with direct pointers to -1
    for (int j = 0; j < MAX_DIRECT_BLOCKS; j++) {
        inode_table[free_inode_index].blocks[j] = -1;
    }

    // mark as used
    inode_table[free_inode_index].used = 1;

    // updates the superblock (decrease the free_inodes)
    sb.free_inodes--;

    return 0;
}

int fs_delete(const char *filename) { //TODO: Needs to be tested. (Avi)
    if (disk_fd == -1) { //Checks in drive is mounted
        return -2;
    }
    int inode_index = find_inode(filename);
    if (inode_index == -1) { //File not found
        return -1;
    }
    inode* current_node = &inode_table[inode_index];
    for (int i = 0; i < MAX_DIRECT_BLOCKS; i++) {
        int block_ptr = current_node->blocks[i];
        if (block_ptr != -1) {
            mark_block_free(block_ptr);
            current_node->blocks[i] = -1;
            sb.free_blocks++;
        }
    }
    current_node->used = 0;
    current_node->size = 0;
    current_node->name[0] = '\0';
    sb.free_inodes++;
    write_back_sb();
    write_inode(inode_index, current_node);
    write_bitmap();
    return 0;
}

int fs_list(char filenames[][MAX_FILENAME], int max_files) { //TODO: Needs to be tested. (Avi)
    if (disk_fd == -1) { //Checks in drive is mounted
        return -1;
    }
    int count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (inode_table[i].used == 1) {
            if (count == max_files) {
                return count;
            }
            strncpy(filenames[count], inode_table[i].name, MAX_FILENAME);
            count++;

        }
    }
    return count;
}

int fs_write(const char *filename, const void *data, int size) {
    if (disk_fd == -1) { //Checks in drive is mounted
        return -3;
    }
    if (size < 0) { //Invalid size
        return -3;
    }
    int node_idx = find_inode(filename);
    if (node_idx == -1) { //Not a valid filename
        return -1;
    }
    if (size > sb.block_size * MAX_DIRECT_BLOCKS) { //Too large for filesystem
        return -3;
    }
    inode* working_node = &inode_table[node_idx];
    int num_of_blocks = 0;
    int num_blocks_added = 0;
    while(num_of_blocks * sb.block_size < size) { //Initialize all needed blocks
        int block_ptr = working_node->blocks[num_of_blocks];
        if (block_ptr == -1) { //Finds a new block if needed
            block_ptr = find_free_block();
            if (block_ptr == -1) { //Out of space
                while (num_blocks_added > 0) { //Removes all additionally added blocks if not enough memory
                    num_of_blocks--;
                    block_ptr = working_node->blocks[num_of_blocks];
                    working_node->blocks[num_of_blocks] = -1; 
                    mark_block_free(block_ptr);
                    sb.free_blocks++;
                    num_blocks_added--;
                }
                return -2;
            }
            working_node->blocks[num_of_blocks] = block_ptr;
            mark_block_used(block_ptr);
            sb.free_blocks--;
            num_blocks_added++;
        }
        num_of_blocks++;
    }
    for (int i = num_of_blocks; i < MAX_DIRECT_BLOCKS; i++) { //Remove excess blocks
        int block_ptr = working_node->blocks[i];
        if (block_ptr != -1) {
            mark_block_free(block_ptr);
            working_node->blocks[i] = -1;
            sb.free_blocks++;
        }
    }
    working_node->size = size;
    for (int i = 0; i < num_of_blocks; i++) { //Writes data
        void* block_data = (void*) (((char*)data) + (i * sb.block_size));
        //TODO: If not a perfect multiple of block size
        lseek(disk_fd, working_node->blocks[i] * sb.block_size, SEEK_SET);
        write(disk_fd, block_data, sb.block_size);
    }
    write_back_sb();
    write_inode(node_idx, working_node);
    write_bitmap();
    return 0;
}

//Reads the file content into the buffer
int fs_read(const char *filename, void *buffer, int size) {
    // make sure params are legal
    if (filename == NULL || buffer == NULL || size < 0) {
        return -3;
    }

    //find the inode of the file and return -1 if it doesn't exist
    int inode_index = find_inode(filename);
    if (inode_index == -1) {
        return -1; //file does not exist
    } 
    
    // determine how many bytes we can read
    int file_size = inode_table[inode_index].size;
    int bytes_to_read = (size < file_size) ? size : file_size;
    if (bytes_to_read == 0) {
        return 0; // there was nothing to read so we are done 
    }

    int total_bytes_read = 0;

    //read data from the files blocks into the buffer
    while (total_bytes_read < bytes_to_read) {
        //figure out which block we are in
        int block_we_are_in = total_bytes_read / BLOCK_SIZE; // (some number / 4096) should be a number 1 to 12
        int offset_within_block = total_bytes_read % BLOCK_SIZE; //once we are in the right block, go to the right place in it
        int bytes_left_in_current_block = BLOCK_SIZE - offset_within_block; //number of bytes we can read within the current block
        int bytes_left_to_read = bytes_to_read - total_bytes_read; //number of bytes we will have left after writing to this block

        //figure out if we can fit the rest of the file in here or if we'll use the rest of what is left
        int size_to_write = (bytes_left_to_read < bytes_left_in_current_block) ? bytes_left_to_read : bytes_left_in_current_block;

        //go to the block we need to write to
        int physical_block = inode_table[inode_index].blocks[block_we_are_in];

        // check this block is legal
        if (physical_block == -1) {
            break;
        }

        //go to the exact right place in the block
        lseek(disk_fd, (physical_block * BLOCK_SIZE) + offset_within_block, SEEK_SET);

        //read the size_to_write into the users buffer
        int bytes_read = read(disk_fd, (char *)buffer + total_bytes_read, size_to_write);

        if (bytes_read <= 0) {
            return -3; //must be a disk error
        }
        total_bytes_read += bytes_read;
    }

    return total_bytes_read;
}

//Helper functions

int find_inode(const char *filename) {
    for (int i = 0; i < sb.total_inodes; i++) {
        if (inode_table[i].used == 1) {
            if(strcmp(inode_table[i].name, filename) == 0) {
                return i; //index of filename
            }
        }
    }
    return -1; //filename does not exist
}

int find_free_inode() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (inode_table[i].used == 0) return i;
    }
    return -1;
}

int find_free_block() {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (bitmap[i/8] & (1 << (i%8))) {
            return i;
        }
    }
    return -1;
}

void mark_block_used(int block_num) {
    bitmap[block_num/8] |= (1 << (block_num/8));
}

void mark_block_free(int block_num) {
    bitmap[block_num/8] &= ~(1 << (block_num/8));
}

void read_inode(int inode_num, inode *target) {
    // TODO: Implement according to requirements
}

void write_inode(int inode_num, const inode *source) {
    // TODO: Implement according to requirements
}

void write_back_sb() {
    char data[BLOCK_SIZE] = {0};
    memcpy(data, &sb, sizeof(superblock));
    lseek(disk_fd, 0, SEEK_SET);
    write(disk_fd, &sb, BLOCK_SIZE);
}

void write_bitmap() {
    lseek(disk_fd, BLOCK_SIZE, SEEK_SET);
    write(disk_fd, bitmap, sizeof(bitmap));
}