/*
 * file:        homework.c
 * description: skeleton file for CS 5600 system
 *
 * CS 5600, Computer Systems, Northeastern CCIS
 * Peter Desnoyers, November 2019
 *
 * Making a new disk image each time: python gen-disk.py -q disk2.in test2.img
 *
 */

#define BLOCK_SIZE 4096
#define FUSE_USE_VERSION 27
#define _FILE_OFFSET_BITS 64
#define MAX_PATH_LEN 10
#define MAX_NAME_LEN 28

#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fuse.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#include "fs5600.h"
#include <inttypes.h>

/* if you don't understand why you can't use these system calls here,
 * you need to read the assignment description another time
 */
#define stat(a,b) error do not use stat()
#define open(a,b) error do not use open()
#define read(a,b,c) error do not use read()
#define write(a,b,c) error do not use write()

//  Defines debug level
#define DEBUG 0

//  Allocate global memory
struct fs_super *superblock;
struct fs_inode *inode;
unsigned char *bitmap;
int read_count;

/* disk access. All access is in terms of 4KB blocks; read and
 * write functions return 0 (success) or -EIO.
 */
extern int block_read(void *buf, int lba, int nblks);
extern int block_write(void *buf, int lba, int nblks);

/* bitmap functions
 */
void bit_set(unsigned char *map, int i)
{
    map[i/8] |= (1 << (i%8));
}
void bit_clear(unsigned char *map, int i)
{
    map[i/8] &= ~(1 << (i%8));
}
int bit_test(unsigned char *map, int i)
{
    return map[i/8] & (1 << (i%8));
}


/* init - this is called once by the FUSE framework at startup. Ignore
 * the 'conn' argument.
 * recommended actions:
 *   - read superblock
 *   - allocate memory, read bitmaps and inodes
 */
void* fs_init(struct fuse_conn_info *conn){
    //  Initial variables
    int lba, nblks = 1;

    //  Allocate memory
    superblock = malloc(FS_BLOCK_SIZE);
    bitmap = malloc(FS_BLOCK_SIZE);
    inode = malloc(FS_BLOCK_SIZE);

    //  Read superblock
    lba = 0;
    block_read(superblock, lba, nblks);

    //  Read bitmaps
    lba = 1;
    block_read(bitmap, lba, nblks);

    //  Read inodes
    lba = 2;
    block_read(inode, lba, nblks);

    //  Return NULL
    return NULL;
}

/* Note on path translation errors:
 * In addition to the method-specific errors listed below, almost
 * every method can return one of the following errors if it fails to
 * locate a file or directory corresponding to a specified path.
 *
 * ENOENT - a component of the path doesn't exist.
 * ENOTDIR - an intermediate component of the path (e.g. 'b' in
 *           /a/b/c) is not a directory
 */

/* note on splitting the 'path' variable:
 * the value passed in by the FUSE framework is declared as 'const',
 * which means you can't modify it. The standard mechanisms for
 * splitting strings in C (strtok, strsep) modify the string in place,
 * so you have to copy the string and then free the copy when you're
 * done. One way of doing this:
 *
 *    char *_path = strdup(path);
 *    int inum = translate(_path);
 *    free(_path);
 */

/*  translate function: inode # = translate(count, parts) */
int translate(int count, char **parts){
    //  Local variables
    int inum = 2, blknum;

    struct fs_dirent *des = malloc(128 * sizeof(struct fs_dirent));

    if(count == 0){
      free(des);
      return -ENOENT;
    }

    struct fs_inode *temp_inode =malloc(sizeof(struct fs_inode));
    //  For loop through all path parts
    for (int i = 0; i < count; i++){
        //  Read block for inode
        block_read(temp_inode, inum, 1);
        //read_count = 0;

        //  Check if inode is a directory
        if (S_ISREG(temp_inode->mode) != 0){
            free(temp_inode);
            free(des);
            return -ENOTDIR;
        }

        //  Iterate through all integers in inode->ptrs and find the value above 0
        for(int k = 0; k < (BLOCK_SIZE/4 - 5); k++){
            if( temp_inode->ptrs[k] > 0 ){
                blknum = temp_inode->ptrs[k];
                read_count++;
                break;
            }
        }

        //  Block read the blocknumber into des struct
        block_read(des, blknum, 1);

        //  Iterate through all possible  directory entries
        int seen = 0;
        for( int j = 0; j < 128; j++){
            //  If the dir entry is valid, compare the name to the path parts
            if(des[j].valid == 1){

                if (strcmp(des[j].name, parts[i]) == 0){
                    //  Update inum is des name matches inode
                    seen = 1;
                    inum = des[j].inode;
                    read_count++;
                    break;
                }
            }
        }
        if ( seen == 0){
            free(temp_inode);
            free(des);
            return -ENOENT;
        }
    }
    //  free des
    free(des);
    free(temp_inode);

    //  Return inode number
    return inum;
}

/* path splitting */
int parse(char *path_c, char **parts){
    //  Local variables
    char *word;
    word = strtok(path_c, "/");
    int count = 0;

    //  Iterate through all words of path, splitting it into an array for each part
    while( word != NULL){
        //strcat(word, "\0");
        strcpy(parts[count], word);
        word = strtok(NULL, "/");
        count++;
    }


    //  Return the length of the array
    return count;
}

/* getattr - get file or directory attributes. For a description of
 *  the fields in 'struct stat', see 'man lstat'.
 *
 * Note - for several fields in 'struct stat' there is no corresponding
 *  information in our file system:
 *    st_nlink - always set it to 1
 *    st_atime, st_ctime - set to same value as st_mtime
 *
 * success - return 0
 * errors - path translation, ENOENT
 * hint - factor out inode-to-struct stat conversion - you'll use it
 *        again in readdir
 */
int fs_getattr(const char *path, struct stat *sb){
    //  Local variables
    int count = 0, inum = 0, i;
    char **parts = malloc(MAX_PATH_LEN * sizeof(char *));

    //  Initize parts array
    for ( i = 0; i < MAX_PATH_LEN; i++){
        parts[i] = malloc(sizeof(char) * (MAX_NAME_LEN + 1));
    }

    //  Copy path
    char *path_c = strdup(path);

    //  Split path => count, char * parts[]
    count = parse(path_c, parts);

    //  Troubleshooting
    if(DEBUG == 1){
        printf("\nParts: ");
        for( i = 0; i < count; i++){
            printf("%s ", parts[i]);
        }
        printf("\n\n");
    }

    if(strcmp(path_c, "/") == 0){

      inum = 2;

    } else{
      //  inode # = translate(count, parts)
      read_count = 0;
      inum = translate(count, parts);
    }

    //  Free memory
    for ( i = 0; i < MAX_PATH_LEN; i++){
        free(parts[i]);
    }
    free(parts);
    free(path_c);

    if(inum == -ENOENT || inum == -ENOTDIR){
      return inum;
    }

    //  Inode to stat
    struct fs_inode *temp_inode =malloc(sizeof(struct fs_inode));
    block_read(temp_inode, inum, 1);
    sb->st_uid = temp_inode->uid;
    sb->st_gid = temp_inode->gid;
    sb->st_mode = temp_inode->mode;
    sb->st_size = temp_inode->size;
    sb->st_ctim.tv_nsec = temp_inode->ctime;
    sb->st_mtim.tv_nsec = temp_inode->mtime;
    sb->st_blocks = read_count;
    sb->st_nlink = 1;
    free(temp_inode);

    //  Return
    return 0;
}

/* readdir - get directory contents.
 *
 * call the 'filler' function once for each valid entry in the
 * directory, as follows:
 *     filler(buf, <name>, <statbuf>, 0)
 * where <statbuf> is a pointer to a struct stat
 * success - return 0
 * errors - path resolution, ENOTDIR, ENOENT
 *
 * hint - check the testing instructions if you don't understand how
 *        to call the filler function
 */
int fs_readdir(const char *path, void *ptr, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi){
    struct stat* sb = malloc(sizeof(struct stat));
    //  Local variables
    int count = 0, inum, i, blknum = 2;
    char **parts = malloc(MAX_PATH_LEN * sizeof(char *));
    //  Initize parts array
    for ( i = 0; i < MAX_PATH_LEN; i++){
        parts[i] = malloc(sizeof(char) * (MAX_NAME_LEN+1));
        memset(parts[i], '\0', MAX_NAME_LEN);
    }
    //  Copy path
    char *path_c = strdup(path);
    //  Split path => count, char * parts[]
    count = parse(path_c, parts);
    if(strcmp(path_c, "/") == 0){
      inum = 2;
    } else{
      //  inode # = translate(count, parts)
      inum = translate(count, parts);
    }
    //  Free memory
    for ( i = 0; i < MAX_PATH_LEN; i++){
        free(parts[i]);
    }
    free(parts);
    //  Handle errors
    if(inum == -ENOENT){
        free(path_c);
        free(sb);
        return -ENOENT;
    }
    else if(inum == -ENOTDIR){
        free(path_c);
        free(sb);
        return -ENOTDIR;
    }


    struct fs_inode *temp_inode = malloc(BLOCK_SIZE);


    //  Read into inode
    block_read(temp_inode, inum, 1);
    if(S_ISDIR(temp_inode->mode) == 0){
      free(temp_inode);
      free(path_c);
      free(sb);
      return -ENOTDIR;
    }
    //  Initialize dir entrytranslate
    struct fs_dirent *des = malloc(128 * sizeof(struct fs_dirent));
    //  Iterate through all integers in inode->ptrs and find the value above 0
    for(int k = 0; k < (4096/4 - 5); k++){
      if(temp_inode->ptrs[k] > 0 ){
        blknum = temp_inode->ptrs[k];
        break;
      }
    }
    //  Block read the blocknumber into des struct
    block_read(des, blknum, 1);
    int fill_out;
    char* path_with_content = malloc(sizeof(char)* MAX_PATH_LEN * (MAX_NAME_LEN+1));

    for( int j = 0; j < 128; j++){

      if(des[j].valid == 1){
        strcpy(path_with_content, path_c);
        if(strcmp("/", path_with_content) != 0 ){
          strcat(path_with_content, "/");
        }
        strcat(path_with_content, des[j].name);
        fs_getattr(path_with_content, sb);
        fill_out = filler(ptr, des[j].name, sb, (off_t) 0);
        strcpy(path_with_content, "");
        if(fill_out != 0){
          free(path_with_content);
          free(path_c);
          return -ENOMEM;
        }
      }
    }

    //  Free memory
    free(path_with_content);
    free(path_c);
    free(sb);
    free(des);
    free(temp_inode);
    //  Return 0 upon success
    return 0;
}

/* create - create a new file with specified permissions
 *
 * success - return 0
 * errors - path resolution, EEXIST
 *          in particular, for create("/a/b/c") to succeed,
 *          "/a/b" must exist, and "/a/b/c" must not.
 *
 * Note that 'mode' will already have the S_IFREG bit set, so you can
 * just use it directly. Ignore the third parameter.
 *
 * If a file or directory of this name already exists, return -EEXIST.
 * If there are already 128 entries in the directory (i.e. it's filled an
 * entire block), you are free to return -ENOSPC instead of expanding it.
 */
int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi){
    //  Local variables

    if(S_ISDIR(mode) != 0){
      return -EISDIR;
    }
    int count = 0, inum, i, free_block;
    char **parts = malloc(MAX_PATH_LEN * sizeof(char *));

    //  Initize parts array
    for ( i = 0; i < MAX_PATH_LEN; i++){
        parts[i] = malloc(sizeof(char) * (MAX_NAME_LEN+1));
    }
    //  Copy path
    char *path_c = strdup(path);

    //  Split path => count, char * parts[]
    count = parse(path_c, parts);

    //  Troubleshooting
    if(DEBUG == 1){
        printf("\nParts: ");
        for( i = 0; i < count; i++){
            printf("%s ", parts[i]);
        }
        printf("\n\n");
    }

    if(strcmp(path_c, "/") == 0 || count == 1){

      inum = 2;

    } else{
      //  inode # = translate(count, parts)
      inum = translate(count - 1, parts);
    }
    //  File name
    char *file_name = malloc(sizeof(char) * (MAX_NAME_LEN + 1));

    strcpy(file_name, parts[count - 1]);

    //  Handle errors
    if(inum == -ENOENT){
        free(file_name);
        free(path_c);
        return -ENOENT;
    }
    else if(inum == -ENOTDIR){
        free(file_name);
        free(path_c);
        return -ENOTDIR;
    }

    //  Ensure file doesn't exist
    int child_does_exist = translate(count, parts);
    if(child_does_exist > 0){
        free(path_c);
        return -EEXIST;
    }

    //  Free memory
    for ( i = 0; i < MAX_PATH_LEN; i++){
        free(parts[i]);
    }
    free(parts);
    free(path_c);

    //  Find free inode number
    free_block = -1;
    for(i = 0; i < 1024; i++){  // FS_BLOCK_SIZE; i++){
        if( bit_test(bitmap, i) == 0){
            free_block = i;
            break;
        }
    }
    if (free_block == -1){
        printf("No free blocks.\n");
        return -EOPNOTSUPP;
    }

    //  Write directory entry into appropriate dir
    struct fs_inode *dir_inode = malloc(sizeof(struct fs_inode));

    //  Read dir inode into temp
    block_read(dir_inode, inum, 1);

    if(S_ISDIR(dir_inode->mode) == 0){
      free(dir_inode);
      return -ENOTDIR;
    }
    printf("SEG SEARCH 5\n");
    //  Get inode uid and gid
    struct fuse_context ctx = *fuse_get_context();

    //  Write info to inode
    struct fs_inode *temp_inode = malloc(sizeof(struct fs_inode));
    temp_inode->uid = ctx.uid;
    temp_inode->gid = ctx.gid;
    temp_inode->mode = mode;
    temp_inode->size = 0;
    temp_inode->ctime = time(NULL);
    temp_inode->mtime = time(NULL);

    for(int j = 0; j < BLOCK_SIZE/4 - 5; j++){
      temp_inode->ptrs[j] = 0;
    }
    //  Write block[�U
    block_write(temp_inode, free_block, 1);

    struct fs_inode *node_check = malloc(sizeof(struct fs_inode));
    block_read(node_check, free_block, 1);



    //  Initialize dir entrytranslate
    struct fs_dirent *des = malloc(128 * sizeof(struct fs_dirent));

    //  Iterate through all integers in inode->ptrs and find the value above 0
    int blknum;
    for(int k = 0; k < (4096/4 - 5); k++){
      if(dir_inode->ptrs[k] > 0 ){
        blknum = dir_inode->ptrs[k];
        break;
      }
    }

    if(DEBUG == 1)
        printf("File Name: %s   Free Block #: %d    Block #: %d\n", file_name, free_block, blknum);

    //  Block read the blocknumber into des struct
    block_read(des, blknum, 1);

    //  Iterate through des and find empty spot
    for(int k = 0; k < 128; k++){
        if(des[k].valid == 0 ){
            des[k].valid = 1;
            strcpy(des[k].name, file_name);
            des[k].inode = free_block;
            break;
      }
    }

    //  Write block back into des
    block_write(des, blknum, 1);

    //  Set bitmap
    bit_set(bitmap, free_block);

    //  Write bitmap back to fs
    block_write(bitmap, 1, 1);

    //  Free memory
    free(temp_inode);
    free(dir_inode);
    free(des);

    //  Return 0 on success
    return 0;
}

/* mkdir - create a directory with the given mode.
 *
 * WARNING: unlike fs_create, @mode only has the permission bits. You
 * have to OR it with S_IFDIR before setting the inode 'mode' field.
 *
 * success - return 0
 * Errors - path resolution, EEXIST
 * Conditions for EEXIST are the same as for create.
 */
int fs_mkdir(const char *path, mode_t mode){
    //  Local variables
    int count = 0, inum, i, free_block;
    char **parts = malloc(MAX_PATH_LEN * sizeof(char *));
    mode = (S_IFDIR | mode);

    //  Initize parts array
    for ( i = 0; i < MAX_PATH_LEN; i++){
        parts[i] = malloc(sizeof(char) * (MAX_NAME_LEN+1));
    }

    //  Copy path
    char *path_c = strdup(path);

    //  Split path => count, char * parts[]
    count = parse(path_c, parts);

    //  Troubleshooting
    if(DEBUG == 1){
        printf("\nParts: ");
        for( i = 0; i < count; i++){
            printf("%s ", parts[i]);
        }
        printf("\n\n");
    }

    if(strcmp(path_c, "/") == 0 || count == 1){

      inum = 2;

    } else{
      //  inode # = translate(count, parts)
      inum = translate(count - 1, parts);
    }

    //  Handle errors
    if(inum == -ENOENT){
        free(path_c);
        return -ENOENT;
    }
    else if(inum == -ENOTDIR){
        free(path_c);
        return -ENOTDIR;
    }

    //  File name
    char *new_dir_name = malloc(sizeof(char) * (MAX_NAME_LEN + 1));
    strcpy(new_dir_name, parts[count - 1]);



    //  Write directory entry into appropriate dir
    struct fs_inode *parent_inode = malloc(sizeof(struct fs_inode));

    //  Read dir inode into temp
    block_read(parent_inode, inum, 1);

    if(S_ISDIR(parent_inode->mode) == 0){
      free(parent_inode);
      free(path_c);
      return -ENOTDIR;
    }

    //  Ensure file doesn't exist
    int child_does_exist = translate(count, parts);
    if(child_does_exist > 0){
        free(path_c);
        return -EEXIST;
    }

    //  Free memory
    for ( i = 0; i < MAX_PATH_LEN; i++){
        free(parts[i]);
    }
    free(parts);
    free(path_c);

    //  Find free inode number
    free_block = -1;
    for(i = 0; i < 1024; i++){
        if( bit_test(bitmap, i) == 0){
            free_block = i;
            break;
        }
    }
    if (free_block == -1){
        printf("No free blocks.\n");
        return -EOPNOTSUPP;
    }



    //  Set bitmap
    bit_set(bitmap, free_block);

    //  Get inode uid and gid
    struct fuse_context ctx = *fuse_get_context();

    //  Write info to inode
    struct fs_inode *temp_inode = malloc(sizeof(struct fs_inode));
    temp_inode->uid = ctx.uid;
    temp_inode->gid = ctx.gid;
    temp_inode->mode = mode;
    temp_inode->size = 4096;
    temp_inode->ctime = time(NULL);
    temp_inode->mtime = time(NULL);

    for(int j = 0; j < BLOCK_SIZE/4 - 5; j++){
      temp_inode->ptrs[j] = 0;
    }

    //  Search for free entry in bitmap
    int k;
    for ( k = 0; k < 1024; k++ ) {
        if(bit_test(bitmap, k) == 0){
            break;
        }
    }

    temp_inode->ptrs[0] = k;
    //  Set bitmap
    bit_set(bitmap, k);
    //  Write block
    block_write(temp_inode, free_block, 1);

    struct fs_dirent *new_dirent = malloc(128 * sizeof(struct fs_dirent));

    for (int i = 0; i < 128; i++){
      new_dirent[i].valid = 0;
      strcpy(new_dirent[i].name, "");
      new_dirent[i].inode = -1;
    }
    block_write(new_dirent, k, 1);
    free(new_dirent);

    struct fs_inode *node_check = malloc(sizeof(struct fs_inode));
    block_read(node_check, free_block, 1);


    //  Initialize dir entry translate
    struct fs_dirent *des = malloc(128 * sizeof(struct fs_dirent));

    //  Iterate through all integers in inode->ptrs and find the value above 0
    int blknum;
    for(int k = 0; k < (4096/4 - 5); k++){
      if(parent_inode->ptrs[k] > 0 ){
        blknum = parent_inode->ptrs[k];
        break;
      }
    }


    if(DEBUG == 1)
        printf("Dir Name: %s   Free Block #: %d    Block #: %d\n", new_dir_name, free_block, blknum);

    //  Block read the blocknumber into des struct
    block_read(des, blknum, 1);

    //  Iterate through des and find empty spot
    for(int k = 0; k < 128; k++){
        if(des[k].valid == 0 ){
            des[k].valid = 1;
            strcpy(des[k].name, new_dir_name);
            des[k].inode = free_block;
            break;
      }
    }

    //  Write block back into des
    block_write(des, blknum, 1);

    //  Write bitmap back to fs
    block_write(bitmap, 1, 1);

    //  Free memory
    free(temp_inode);
    free(parent_inode);
    free(des);

    //  Return 0 on success
    return 0;
}

/* unlink - delete a file
 *  success - return 0
 *  errors - path resolution, ENOENT, EISDIR
 */
int fs_unlink(const char *path)
{
    //  Local variables
    int count = 0, child_inum, parent_inum, i;
    char **parts = malloc(MAX_PATH_LEN * sizeof(char *));

    //  Initize parts array
    for ( i = 0; i < MAX_PATH_LEN; i++){
        parts[i] = malloc(sizeof(char) * (MAX_NAME_LEN+1));
    }

    //  Copy path
    char *path_c = strdup(path);

    //  Split path => count, char * parts[]
    count = parse(path_c, parts);

    //  Troubleshooting
    if(DEBUG == 1){
        printf("\nParts: ");
        for( i = 0; i < count; i++){
            printf("%s ", parts[i]);
        }
        printf("\n\n");
    }

    if(strcmp(path_c, "/") == 0 || count == 1){

      parent_inum = 2;
      child_inum = translate(count, parts);
    } else{
      parent_inum = translate(count - 1, parts);
      child_inum = translate(count, parts);
    }

    //  Handle errors
    if(parent_inum == -ENOENT || child_inum == -ENOENT){
        free(path_c);
        return -ENOENT;
    }
    else if(parent_inum == -ENOTDIR || child_inum == -ENOTDIR){
        free(path_c);
        return -ENOTDIR;
    }

    struct fs_inode* file_inode = malloc(sizeof(struct fs_inode));
    block_read(file_inode, child_inum, 1);

    if(S_ISDIR(file_inode->mode) != 0){
      free(file_inode);
      free(path_c);
      return -EISDIR;
    }

    //  File name
    char *file_name = malloc(sizeof(char) * (MAX_NAME_LEN + 1));
    strcpy(file_name, parts[count-1]);

    //  Read block of parent inode
    struct fs_inode *parent_inode = malloc(sizeof(struct fs_inode));
    block_read(parent_inode, parent_inum, 1);

    //  Find block number
    int blknum;
    for (int k = 0; k < (4096/4 - 5); k++){
        if(parent_inode->ptrs[k] > 0){
            blknum = parent_inode->ptrs[k];
            break;
        }
    }

    //  Initialize dir entrytranslate
    struct fs_dirent *des = malloc(128 * sizeof(struct fs_dirent));

    //  Block read the blocknumber into des struct
    block_read(des, blknum, 1);

    //  Iterate through des and find empty spot
    for(int k = 0; k < 128; k++){
        if(des[k].valid == 1 && (strcmp(des[k].name, file_name) == 0)){
            des[k].valid = 0;
            strcpy(des[k].name, "");

            break;
      }
    }

    //  Write back to block
    block_write(des, blknum, 1);

    for(int i = 0; i < BLOCK_SIZE/4 - 5; i++){
      if(file_inode->ptrs[i] > 0){
        bit_clear(bitmap, file_inode->ptrs[i]);
      }
    }
    //  Erase bitmap number
    bit_clear(bitmap, child_inum);

    block_write(bitmap, 1, 1);

    //  Return 0 on success
    return 0;
}

/* rmdir - remove a directoryfs_ops.rmdir(path8);
 *  success - return 0
 *  Errors - path resolution, ENOENT, ENOTDIR, ENOTEMPTY
 */
int fs_rmdir(const char *path)
{
  //  Local variables
  int count = 0, child_inum, parent_inum, i;
  char **parts = malloc(MAX_PATH_LEN * sizeof(char *));

  //  Initize parts array
  for ( i = 0; i < MAX_PATH_LEN; i++){
      parts[i] = malloc(sizeof(char) * (MAX_NAME_LEN+1));
  }

  //  Copy path
  char *path_c = strdup(path);

  //  Split path => count, char * parts[]
  count = parse(path_c, parts);

  //  Troubleshooting
  if(DEBUG == 1){
      printf("\nParts: ");
      for( i = 0; i < count; i++){
          printf("%s ", parts[i]);
      }
      printf("\n\n");
  }

  if(strcmp(path_c, "/") == 0 || count == 1){

    parent_inum = 2;
    child_inum = translate(count, parts);

  } else{
    //  inode # = translate(count, parts)
    parent_inum = translate(count - 1, parts);
    child_inum = translate(count, parts);
  }

  //  Handle errors
  if(parent_inum == -ENOENT || child_inum == -ENOENT){
      free(path_c);
      return -ENOENT;
  }
  else if(parent_inum == -ENOTDIR || child_inum == -ENOTDIR){
      free(path_c);
      return -ENOTDIR;
  }

  //  directory name
  char *dir_name = malloc(sizeof(char) * (MAX_NAME_LEN + 1));
  strcpy(dir_name, parts[count-1]);

  struct fs_inode *child_inode = malloc(sizeof(struct fs_inode));

  block_read(child_inode, child_inum, 1);

  if(S_ISREG(child_inode->mode) != 0){
    free(path_c);
    free(dir_name);
    free(child_inode);
    return -ENOTDIR;
  }

  int child_dirent_block;

  for(int i = 0; i < (4096/4 - 5); i++){
    if(child_inode->ptrs[i] > 0){
      child_dirent_block = child_inode->ptrs[i];
      break;
    }
  }


  //  Initialize dir entry
  struct fs_dirent *child_des = malloc(128 * sizeof(struct fs_dirent));
  block_read(child_des, child_dirent_block, 1);

  for(int k = 0; k < 128; k++){
      if(child_des[k].valid == 1){
          free(child_des);
          free(child_inode);
          free(path_c);
          free(dir_name);
          return -ENOTEMPTY;
    }
  }

  struct fs_dirent *cleared_des = malloc(128*sizeof(struct fs_dirent));
  block_write(cleared_des, child_dirent_block, 1);
  free(cleared_des);

  bit_clear(bitmap, child_dirent_block);

  bit_clear(bitmap, child_inum);

  block_write(bitmap, 1, 1);

  //  Read block of parent inode
  struct fs_inode *parent_inode = malloc(sizeof(struct fs_inode));
  block_read(parent_inode, parent_inum, 1);
  //  Find block number
  int blknum;
  for (int k = 0; k < (4096/4 - 5); k++){
      if(parent_inode->ptrs[k] > 0){
          blknum = parent_inode->ptrs[k];
          break;
      }
  }
  //  Initialize dir entrytranslate
  struct fs_dirent *des = malloc(128 * sizeof(struct fs_dirent));


  //  Block read the blocknumber into des struct
  block_read(des, blknum, 1);

  //  Iterate through des and find the directory to delete
  for(int k = 0; k < 128; k++){
      if(des[k].valid == 1 && (strcmp(des[k].name, dir_name) == 0)){
          des[k].valid = 0;
          strcpy(des[k].name, "\0");
          break;
    }
  }

  //  Write back to block
  block_write(des, blknum, 1);
  free(des);
  free(child_des);
  free(child_inode);
  free(parent_inode);
  free(dir_name);
  free(path_c);
  //  Return 0 on success
  return 0;
}

/* rename - rename a file or directory
 * success - return 0
 * Errors - path resolution, ENOENT, EINVAL, EEXIST
 *
 * ENOENT - source does not exist
 * EEXIST - destination already exists
 * EINVAL - source and destination are not in the same directory
 *
 * Note that this is a simplified version of the UNIX rename
 * functionality - see 'man 2 rename' for full semantics. In
 * particular, the full version can move across directories, replace a
 * destination file, and replace an empty directory with a full one.
 */
int fs_rename(const char *src_path, const char *dst_path){
    //  Local variables
    int count = 0, inum, i;
    char **parts_src = malloc(MAX_PATH_LEN * sizeof(char *));

    //  Initize parts array
    for ( i = 0; i < MAX_PATH_LEN; i++){
        parts_src[i] = malloc(sizeof(char) * (MAX_NAME_LEN+1));
        memset(parts_src[i], '\0', MAX_NAME_LEN);
    }

    char **parts_dst = malloc(MAX_PATH_LEN * sizeof(char *));

    //  Initize parts array
    for ( i = 0; i < MAX_PATH_LEN; i++){
        parts_dst[i] = malloc(sizeof(char) * (MAX_NAME_LEN+1));
        memset(parts_dst[i], '\0', MAX_NAME_LEN);
    }

    //  Copy path
    char *path_src_c = strdup(src_path);

    char *path_dst_c = strdup(dst_path);


    //  Split path => count, char * parts[]
    count = parse(path_src_c, parts_src);

    parse(path_dst_c, parts_dst);

    for(i = 0; i < count - 1; i++){
      if(strcmp(parts_dst[i], parts_src[i]) != 0){
        for ( i = 0; i < MAX_PATH_LEN; i++){
            free(parts_dst[i]);
        }

        for ( i = 0; i < MAX_PATH_LEN; i++){
            free(parts_src[i]);
        }

        free(path_src_c);
        free(path_dst_c);
        free(parts_dst);
        free(parts_src);
        return -EINVAL;
      }
    }

    //  inode # = translate(count, parts)
    inum = translate(count-1, parts_src);

    int dst_inum = translate(count, parts_dst);

    if(dst_inum > 0){
      for ( i = 0; i < MAX_PATH_LEN; i++){
          free(parts_dst[i]);
      }

      for ( i = 0; i < MAX_PATH_LEN; i++){
          free(parts_src[i]);
      }

      free(path_src_c);
      free(path_dst_c);
      free(parts_dst);
      free(parts_src);
      return -EEXIST;
    }

    int src_inum = translate(count, parts_src);

    if(src_inum < 0){
      for ( i = 0; i < MAX_PATH_LEN; i++){
          free(parts_dst[i]);
      }

      for ( i = 0; i < MAX_PATH_LEN; i++){
          free(parts_src[i]);
      }

      free(path_src_c);
      free(path_dst_c);
      free(parts_dst);
      free(parts_src);
      return -ENOENT;
    }


    struct fs_dirent *des = malloc(128 * sizeof(struct fs_dirent));
    struct fs_inode *temp_inode = malloc(sizeof(struct fs_inode));

    block_read(temp_inode, inum, 1);

    block_read(des, temp_inode->ptrs[0], 1);

    //  Iterate through all possible  directory entries
    for( int j = 0; j < 128; j++){
        //  If the dir entry is valid, compare the name to the path parts

        if(des[j].valid == 1){
            if (strcmp(des[j].name, parts_src[count-1]) == 0){
                //  Update inum is des name matches inode
                strcpy(des[j].name, parts_dst[count-1]);
                break;
            }
        }
    }

    block_write(des, temp_inode->ptrs[0], 1);

    //  Initize parts array
    for ( i = 0; i < MAX_PATH_LEN; i++){
        free(parts_dst[i]);
    }

    //  Initize parts array
    for ( i = 0; i < MAX_PATH_LEN; i++){
        free(parts_src[i]);
    }

    free(path_src_c);
    free(path_dst_c);
    free(parts_dst);
    free(parts_src);
    free(des);
    free(temp_inode);

    return 0;
}

/* chmod - change file permissions
 * utime - change access and modification times
 *         (for definition of 'struct utimebuf', see 'man utime')
 *
 * success - return 0
 * Errors - path resolution, ENOENT.
 */
int fs_chmod(const char *path, mode_t mode){
    int count = 0, inum;
    char **parts = malloc(MAX_PATH_LEN * sizeof(char *));

    //  Initize parts array
    for (int i = 0; i < MAX_PATH_LEN; i++){
        parts[i] = malloc(sizeof(char) * (MAX_NAME_LEN+1));
        memset(parts[i], '\0', MAX_NAME_LEN);
    }

    //  Copy path
    char *path_c = strdup(path);

    //  Split path => count, char * parts[]
    count = parse(path_c, parts);

    //  inode # = translate(count, parts)
    inum = translate(count, parts);

    if(inum < 0){
      //  Initize parts array
      for (int i = 0; i < MAX_PATH_LEN; i++){
          free(parts[i]);
      }
      free(path_c);
      free(parts);
      return -ENOENT;
    }

    struct fs_inode *temp_inode = malloc(FS_BLOCK_SIZE);

    block_read(temp_inode, inum, 1);

    temp_inode->mode = mode;

    block_write(temp_inode, inum, 1);

    //  Initize parts array
    for (int i = 0; i < MAX_PATH_LEN; i++){
        free(parts[i]);
    }
    free(path_c);
    free(parts);
    free(temp_inode);
    return 0;
}

int fs_utime(const char *path, struct utimbuf *ut)
{
    /* your code here */
    return -EOPNOTSUPP;
}

/* truncate - truncate file to exactly 'len' bytes
 * success - return 0
 * Errors - path resolution, ENOENT, EISDIR, EINVAL
 *    return EINVAL if len > 0.
 */
int fs_truncate(const char *path, off_t len)
{
    /* you can cheat by only implementing this for the case of len==0,
     * and an error otherwise.
     */
    if (len != 0)
	    return -EINVAL;		/* invalid argument */

    //  Local variables
    int count = 0, inum = 0, i;
    char **parts = malloc(MAX_PATH_LEN * sizeof(char *));

    //  Initize parts array
    for ( i = 0; i < MAX_PATH_LEN; i++){
        parts[i] = malloc(sizeof(char) * (MAX_NAME_LEN+1));
    }

    //  Copy path
    char *path_c = strdup(path);

    //  Split path => count, char * parts[]
    count = parse(path_c, parts);

    //  Troubleshooting
    if(DEBUG == 1){
        printf("\nParts: ");
        for( i = 0; i < count; i++){
            printf("%s ", parts[i]);
        }
        printf("\n\n");
    }

    if(strcmp(path_c, "/") == 0){

      inum = 2;

    } else{
      //  inode # = translate(count, parts)
      read_count = 0;
      inum = translate(count, parts);
    }

    //  Free memory
    for ( i = 0; i < MAX_PATH_LEN; i++){
        free(parts[i]);
    }
    free(parts);
    free(path_c);

    //  Error corrections for path resolution
    if (inum == -ENOENT){
        return -ENOENT;
    }
    if (inum == -EISDIR){
        return -EISDIR;
    }

    //  Read into inode
    struct fs_inode *temp_inode =malloc(sizeof(struct fs_inode));
    block_read(temp_inode, inum, 1);

    //  Check if inode mode is dir, error if so
    if(S_ISDIR(temp_inode->mode) != 0){
        return -EISDIR;
    }

    //  Clear bitmap and ptrs value to free blknum
    for ( i = 0; i < (BLOCK_SIZE/4) - 5; i++){
        if( temp_inode->ptrs[i] > 0){
            bit_clear(bitmap, temp_inode->ptrs[i]);
            temp_inode->ptrs[i] = 0;
        }
    }

    //  Resize inode
    temp_inode->size = 0;

    //  Write inode back to fs
    block_write(temp_inode, inum, 1);

    //  Write back to bitmap
    block_write(bitmap, 1, 1);

    //  Return 0 on success
    return 0;
}


/* read - read data from an open file.
 * success: should return exactly the number of bytes requested, except:
 *   - if offset >= file len, return 0
 *   - if offset+len > file len, return #bytes from offset to end
 *   - on error, return <0
 * Errors - path resolution, ENOENT, EISDIR
 */

int fs_read(const char *path, char *buf, size_t len, off_t offset, struct fuse_file_info *fi){
    //  Local Variables
    int inum, i, count, file_size;
    struct stat *sb = malloc(sizeof(struct stat));

    int blocks[(BLOCK_SIZE/4 - 5)];
    char **parts = malloc(MAX_PATH_LEN * sizeof(char *));

    //  Initize parts array
    for ( i = 0; i < MAX_PATH_LEN; i++){
        parts[i] = malloc(sizeof(char) * (MAX_NAME_LEN+1));
        memset(parts[i], '\0', MAX_NAME_LEN);
    }

    //  Copy path
    char *path_c = strdup(path);

    //  Split path => count, char * parts[]
    count = parse(path_c, parts);

    if(strcmp(path_c, "/") == 0){
      inum = 2;
    } else{
      //  inode # = translate(count, parts)
      inum = translate(count, parts);
    }

    //  Free memory
    for ( i = 0; i < MAX_PATH_LEN; i++){
        free(parts[i]);
    }
    free(parts);

    //  Handle errors
    if(inum == -ENOENT){
        free(path_c);
        free(sb);
        return -ENOENT;
    }
    else if(inum == -ENOTDIR){
        free(path_c);
        free(sb);
        return -ENOTDIR;
    }
    //  Get inode attrs
    fs_getattr(path, sb);

    //  File size
    file_size = sb->st_size;

    //  if offset >= file len, return 0
    if ( offset >= file_size ){
        free(path_c);
        free(sb);
        return 0;
    }

    //  Initialize blocks array
    for ( i = 0; i < (BLOCK_SIZE/4 - 5); i++ ){
        blocks[i] = 0;
    }

    //  Create inode
    block_read(inode, inum, 1);

    //  Find blocks
    count = 0;
    for( i = 0; i < (BLOCK_SIZE/4 - 5); i++ ){
        if ( inode->ptrs[i] > 0 ){
            blocks[count] = inode->ptrs[i];
            count++;
        }
    }

    char *tmp = malloc(count * BLOCK_SIZE);
    for(int j = 0; j < count; j++){
      block_read(tmp + j*BLOCK_SIZE, blocks[j], 1);
    }
    if ((offset + len) > sb->st_size){
      memcpy(buf, tmp + offset, sb->st_size - offset);
      //  Free memory
      free(path_c);
      free(tmp);
      free(sb);
      return sb->st_size - offset;
    } else {
      memcpy(buf, tmp + offset, len);
      //  Free memory
      free(path_c);
      free(tmp);
      free(sb);
      return len;
    }




    return len;
}

/* write - write data to a file
 * success - return number of bytes written. (this will be the same as
 *           the number requested, or else it's an error)
 * Errors - path resolution, ENOENT, EISDIR
 *  return EINVAL if 'offset' is greater than current file length.
 *  (POSIX semantics support the creation of files with "holes" in them,
 *   but we don't)
 */
int fs_write(const char *path, const char *buf, size_t len,
	     off_t offset, struct fuse_file_info *fi)
{

    //  Local Variables
    int inum;

    char **parts = malloc(MAX_PATH_LEN * sizeof(char *));

    //  Initize parts array
    for (int i = 0; i < MAX_PATH_LEN; i++){
        parts[i] = malloc(sizeof(char) * (MAX_NAME_LEN+1));
        memset(parts[i], '\0', MAX_NAME_LEN);
    }

    //  Copy path
    char *path_c = strdup(path);

    //  Split path => count, char * parts[]
    int count = parse(path_c, parts);

    if(strcmp(path_c, "/") == 0){
      inum = 2;
    } else{
      //  inode # = translate(count, parts)
      inum = translate(count, parts);
    }

    //  Free memory
    for (int i = 0; i < MAX_PATH_LEN; i++){
        free(parts[i]);
    }
    free(parts);
    free(path_c);
    //  Handle errors
    if(inum == -ENOENT || inum == 2){
        return -ENOENT;
    }
    else if(inum == -ENOTDIR){
        return -ENOTDIR;
    }

    // read inode for file
    struct fs_inode* file_inode = malloc(sizeof(struct fs_inode));

    block_read(file_inode, inum, 1);

    if(offset > file_inode->size){
      return -EINVAL;
    }

    if(S_ISDIR(file_inode->mode)>0){
      return -EISDIR;
    }

    int num_blocks = (offset + len)/BLOCK_SIZE;

    if((offset + len)%BLOCK_SIZE > 0 ){
      num_blocks++;
    }

    char* tmp = malloc(BLOCK_SIZE * num_blocks);

    int block_numbers[num_blocks];

    // read data currently in file into tmp buffer
    int counter = 0;
    for (int i = 0; i < (BLOCK_SIZE/4 - 5); i++){
      if(file_inode->ptrs[i] > 0){
          block_numbers[counter] = file_inode->ptrs[i];
          block_read(tmp + BLOCK_SIZE*counter, file_inode->ptrs[i], 1);
          counter++;
      }
    }
    // allocate new blocks needed as necessary, mark their bits as used in the
    // bit map and then add the new block numbers to the ptrs in inode
    // counter is number of blocks read out of the file at this point
    int i;
    while(counter < num_blocks){
      for(i = 0; i < 1024; i++){
        if(bit_test(bitmap, i) == 0){
          block_numbers[counter] = i;
          bit_set(bitmap, i);
          break;
        }
      }
      for(int j = 0; j < (BLOCK_SIZE/4 - 5); j++){
        if(file_inode->ptrs[j] == 0) {
            file_inode->ptrs[j] = i;
            break;
        }
      }
      counter++;
    }

    // copy new data into tmp buffer which represents the current data for the
    // file

    memcpy(tmp+offset, buf, len);

    // write the updated buffer data back into the file blocks
    for(int i = 0; i < num_blocks; i++){
      // last block, should only rewrite the left over data
      if(i == num_blocks - 1){
        block_write(tmp + i*BLOCK_SIZE, block_numbers[i], (offset + len)%BLOCK_SIZE);
      } else{
        block_write(tmp + i*BLOCK_SIZE, block_numbers[i], 1);
     }
    }
    file_inode->size = offset + len;

    // write inode back to block
    block_write(file_inode, inum, 1);

    // write bitmap back to block
    block_write(bitmap, 1, 1);

    free(file_inode);
    free(tmp);
    return len;
}

/* statfs - get file system statistics
 * see 'man 2 statfs' for description of 'struct statvfs'.
 * Errors - none. Needs to work.
 */
int fs_statfs(const char *path, struct statvfs *buf){
    //  Local variables
    int i;
    long unsigned int used_bytes, block_size, total_blocks, free_blocks, namemax;

    //  Initialize globals
    block_size = 4096;
    namemax = MAX_NAME_LEN - 1;

    //  Loop through bitmap for blocks
    used_bytes = 0;
    total_blocks = 0;
    for(i = 0; i < 4096; i++){
        if(bit_test(bitmap, i) > 0){
            used_bytes++;
        }
    }

    //  Finish variables
    total_blocks  = superblock->disk_size;
    free_blocks = total_blocks - used_bytes;

    //  Update stats
    buf->f_bfree = free_blocks;
    buf->f_bsize = block_size;
    buf->f_blocks = total_blocks;
    buf->f_namemax = namemax;

    //  Return 0 on success
    return 0;
}

/* operations vector. Please don't rename it, or else you'll break things
 */
struct fuse_operations fs_ops = {
    .init = fs_init,            /* read-mostly operations */
    .getattr = fs_getattr,
    .readdir = fs_readdir,
    .rename = fs_rename,
    .chmod = fs_chmod,
    .read = fs_read,
    .statfs = fs_statfs,

    .create = fs_create,        /* write operations */
    .mkdir = fs_mkdir,
    .unlink = fs_unlink,
    .rmdir = fs_rmdir,
    .utime = fs_utime,
    .truncate = fs_truncate,
    .write = fs_write,
};
