#include "fs.h"

/**
 * Find an inode by filename
 * 
 * @param filename The filename of the inode
 * 
 * @return The index of the inode (in the inode lookup table) or -1 if not found
 */
int find_inode(const char *filename);

/**
 * Find a free inode
 * 
 * @return The index of the found free inode or -1 if there are none
 */
int find_free_inode();

/**
 * Find a free block
 * 
 * @return The index of the found free block or -1 if there are none
 */
int find_free_block();

/**
 * Mark a block as used
 * 
 * @param block_num The block to mark as used
 */
void mark_block_used(int block_num);

/**
 * Mark a block as free
 * 
 * @param block_num The block to mark as free
 */
void mark_block_free(int block_num);

/**
 * Read an inode from disk
 */
void read_inode(int inode_num, inode *target);

/**
 * Write an inode to disk
 */
void write_inode(int inode_num, const inode *source);

/**
 * writes sb back to disk after an update
 */
void write_back_sb();