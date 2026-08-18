# Custom File System (C)

A custom file system implementation written in C, designed to handle raw file storage, data block allocation, and low-level file operations.

## Core Files
- `fs.c` - The main implementation file containing the logic for file system management, data allocation, and input/output operations.
- `helper.h` - Auxiliary structures and helper definitions supporting the core file system architecture.

## Features
- **Low-Level Storage Management:** Handles the translation of file data into raw storage blocks and manages disk space allocation.
- **System Call Emulation:** Implements foundational file operations (such as reading, writing, and data manipulation) at the system level.
- **Metadata Handling:** Manages the underlying structures required to track file locations, sizes, and layout. 

## Usage
Compile `fs.c` with your main application to interact directly with the simulated file system and manage data block storage.
