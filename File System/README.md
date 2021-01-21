Programming Assignment 3 - read-mostly file system
------------------------------------------------

In this assignment you will implement a mostly-read-only version of a Unix-like file system using the FUSE library.

Assignment Details
------------------

**Materials:** You will be provided with the following materials in your team repository:

- Makefile
- fs5600.h - structure definitions
- homework.c - skeleton code
- unittest.c - test skeleton
- misc.c, hw3fuse.c - support code 
- gen-disk.py, disk1.in - generates file system image

**Deliverables:** There are two parts to this assignment.

In the first part, you will need to implement the following functions in `homework.c`:

[x] `fs_getattr` - get attributes of a file/directory
[x] `fs_readdir` - enumerate entries in a directory
[x] `fs_read` - read data from a file
[x] `fs_statfs` - report file system statistics
[x] `fs_init` - constructor (i.e. put your init code here)
[x] `fs_rename` - rename a file
[x] `fs_chmod` - change file permissions

In the second part you will need to implement these methods:

[x] `fs_create` - create a new (empty) file
[x] `fs_mkdir` - create new (empty) directory
[] `fs_unlink` - remove a file
[] `fs_rmdir` - remove a directory
[] `fs_truncate` - delete the contents of a file
[] `fs_write` - write to a file

(the actual function names don't matter - FUSE will call the function pointers in the `fs_ops` structure, which is initialized at the bottom of the file)

**LIMITATIONS** You can make the following assumptions:
1. Directories are not nested more than 10 deep. You don't need to enforce this - the test scripts will never create directories nested that deep.
2. Directories are never bigger than 1 block
3. `rename` is only used within the same directory - e.g. `rename("/dir/f1", "/dir/f2")`. The test scripts will never try to rename across directories
4. Truncate is only ever called with len=0, so that you delete all the data in the file.

Your code will run under two different frameworks - a C unit test framework (libcheck), and the FUSE library which will run your code as a real file system. In each case your code will read blocks from a “disk” (actually an image file) using the `block_read` function.

Note that your code will **not** use standard file system functions like `open`, `read`, `stat`, `readdir` etc. - your code is responsible for files and directories which are encoded in the data blocks which you access via `block_read` and `block_write`. 

You will be graded on the following code in your repository:

- `homework.c` - implementation
- `unittest-1.c`, `unittest-2.c` - unit tests

Note that you will probably need to install several packages for the included code to work; in Ubuntu you can use the following commands:
```
sudo apt install check
sudo apt install libfuse-dev
sudo apt install zlib1g-dev
```

(i.e. libcheck plus the development libraries and headers for libfuse and zlib)

Additional information
------------------

* [FORMAT.md](FORMAT.md) - file system format specification
* [ADVICE.md](ADVICE.md) - implementation advice. PLEASE READ.
* [TESTING-1.md](TESTING-1.md) - testing advice for Part 1 (describes grading tests)
* [TESTING-2.md](TESTING-2.md) - testing advice for Part 2
