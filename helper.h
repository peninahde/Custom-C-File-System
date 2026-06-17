#include "fs.h"

/**
 * Find an inode by filename
 */
int find_inode(const char *filename);

/**
 * Find a free inode
 */
int find_free_inode();

/**
 * Find a free block
 */
int find_free_block();

/**
 * Mark a block as used
 */
void mark_block_used(int block_num);

/**
 * Mark a block as free
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