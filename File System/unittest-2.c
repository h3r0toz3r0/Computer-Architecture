/*
 * file:        unittest-2.c
 * description: libcheck test skeleton, part 2
 */

#define _FILE_OFFSET_BITS 64
#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include <zlib.h>
#include <fuse.h>
#include <stdlib.h>
#include <errno.h>

struct {
    char *name;
    int seen;
} seen_table[] = {
    {"",                           0},
    {"new_file1",                  0},
    {"new_file2",                  0},
    {"new_file3",                  0},
    {"dir_1",                      0},
    {"new_file1",            0},
    {"new_dir",              0},
    {"next_file",    0},
    {"file.8k+",                   0},
    {NULL}
};


extern struct fuse_operations fs_ops;

//  Troubleshooting debug
#define DEBUG 1

/* mockup for fuse_get_context. you can change ctx.uid, ctx.gid in
 * tests if you want to test setting UIDs in mknod/mkdir
 */
struct fuse_context ctx = { .uid = 500, .gid = 500};
struct fuse_context *fuse_get_context(void)
{
    return &ctx;
}

/* this is the callback function for readdir */
int filler(void *ptr, const char *name, const struct stat *stbuf, off_t offset){
    /* FUSE passes you the entry name and a pointer to a 'struct stat'
    * with the attributes. Ignore the 'ptr' and 'off' arguments
    *
    */
    printf("\t\tentered ");
    //   Print name of path
    if(DEBUG == 1)
        printf("\"%s\"\n", name);

    //  Update seen table
    for( int i = 0; seen_table[i].name != NULL; i++){
        if (strcmp(name, seen_table[i].name) == 0){
            seen_table[i].seen += 1;
        }
    }

    //  Return 0 upon success
    return 0;
}

//  Makes and deletes multiple subdirs, verfies they are gone -- Tests rmdir/mkdir
START_TEST(a_test){
    //  Local Variables
    char *path1 = "/new_dir1";
    char *path2 = "/new_dir2";
    char *path3 = "/new_dir3";
    char *path4 = "/new_dir1/new_dir1";;
    char *path5 = "/new_dir2/another_dir2";
    char *path6 = "/new_dir3/next";
    char *path7 = "/new_dir3/next/dir_last";
    mode_t mode = 0777;

    //  Testing Variables
    int rv;
    void *ptr = malloc(4096);
    off_t offset = 0;
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));

    //  Make multiple dirs in root
    fs_ops.mkdir(path1, mode);
    fs_ops.readdir("/", ptr, filler, offset, fi);
    rv = fs_ops.readdir(path1, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\nMkdir and Rmdir Tests\n");
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path1, rv);
    }
    ck_assert_int_eq(rv, 0);
    fs_ops.mkdir(path2, mode);
    rv = fs_ops.readdir(path2, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path2, rv);
    }
    ck_assert_int_eq(rv, 0);
    fs_ops.mkdir(path3, mode);
    rv = fs_ops.readdir(path3, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path3, rv);
    }
    ck_assert_int_eq(rv, 0);

    //  Make multiple dirs in sub dir
    fs_ops.mkdir(path4, mode);
    rv = fs_ops.readdir(path4, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path4, rv);
    }
    ck_assert_int_eq(rv, 0);
    fs_ops.mkdir(path5, mode);
    rv = fs_ops.readdir(path5, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path5, rv);
    }
    ck_assert_int_eq(rv, 0);
    fs_ops.mkdir(path6, mode);
    rv = fs_ops.readdir(path6, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path6, rv);
    }
    ck_assert_int_eq(rv, 0);

    //  Make dirs in sub sub dir
    fs_ops.mkdir(path7, mode);
    rv = fs_ops.readdir(path7, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path7, rv);
    }
    ck_assert_int_eq(rv, 0);

    //  Remove dirs
    fs_ops.rmdir(path7);
    rv = fs_ops.readdir(path7, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Rmdir (-2=pass): %d\n", path7, rv);
    }
    ck_assert_int_lt(rv, 0);


    fs_ops.rmdir(path6);
    rv = fs_ops.readdir(path6, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Rmdir (-2=pass): %d\n", path6, rv);
    }
    ck_assert_int_lt(rv, 0);
    fs_ops.rmdir(path5);
    rv = fs_ops.readdir(path5, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Rmdir (-2=pass): %d\n", path5, rv);
    }
    ck_assert_int_lt(rv, 0);
    fs_ops.rmdir(path4);
    rv = fs_ops.readdir(path4, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Rmdir (-2=pass): %d\n", path4, rv);
    }
    ck_assert_int_lt(rv, 0);
    fs_ops.rmdir(path3);
    rv = fs_ops.readdir(path3, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Rmdir (-2=pass): %d\n", path3, rv);
    }
    ck_assert_int_lt(rv, 0);
    fs_ops.rmdir(path2);
    rv = fs_ops.readdir(path2, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Rmdir (-2=pass): %d\n", path2, rv);
    }
    ck_assert_int_lt(rv, 0);
    fs_ops.rmdir(path1);
    rv = fs_ops.readdir(path1, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Rmdir (-2=pass): %d\n", path1, rv);
    }
    ck_assert_int_lt(rv, 0);

    //  Free memory
    free(fi);
    free(ptr);

}
END_TEST

//  Creates and deletes multiple files, verifies they don't show up in readdir -- Tests unlink/create
START_TEST(b_test){
    //  Local Variables
    char *root = "/";
    char *path1 = "/new_file1";
    char *path2 = "/new_file2";
    char *path3 = "/new_file3";
    char *dir_1 = "/dir_1";
    char *path4 = "/dir_1/new_file1";
    char *new_dir = "/dir_1/new_dir";
    char *path5 = "/dir_1/new_dir/next_file";
    mode_t mode = 100666;

    //  Testing Variables
    int rv, check = 0;
    void *ptr = malloc(4096);
    off_t offset = 0;
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));

    //  Create dirs
    fs_ops.mkdir("/dir_1", 0777);
    rv = fs_ops.readdir("/dir_1", ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\nCreate and Unlink Tests\n");
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", "/dir_1", rv);
    }
    ck_assert_int_eq(rv, 0);

    fs_ops.mkdir("/dir_1/new_dir", 0777);
    rv = fs_ops.readdir("/dir_1/new_dir", ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", "/dir_1/new_dir", rv);
    }
    ck_assert_int_eq(rv, 0);

    //  Create files
    fs_ops.create(path1, mode, fi);
    rv = fs_ops.readdir(root, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Create (0=pass): %d\n", path1, rv);
    }


    //  Look through seen table
    check = 0;
    for( int i = 0; seen_table[i].name != NULL; i++){
        if(seen_table[i].seen > 0){
            check++;
        }
    }
    ck_assert_int_eq(check, 3);
    ck_assert_int_eq(rv, 0);
    fs_ops.create(path2, mode, fi);
    rv = fs_ops.readdir(root, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Create (0=pass): %d\n", path2, rv);
    }
    //  Look through seen table
    check = 0;
    for( int i = 0; seen_table[i].name != NULL; i++){
        if(seen_table[i].seen > 0){
            check++;
        }
    }
    ck_assert_int_eq(check, 4);
    ck_assert_int_eq(rv, 0);
    fs_ops.create(path3, mode, fi);
    rv = fs_ops.readdir(root, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Create (0=pass): %d\n", path3, rv);
    }
    //  Look through seen table
    check = 0;
    for( int i = 0; seen_table[i].name != NULL; i++){
        if(seen_table[i].seen > 0){
            check++;
        }
    }
    //ck_assert_int_eq(check, 4);
    ck_assert_int_eq(rv, 0);
    fs_ops.create(path4, mode, fi);
    rv = fs_ops.readdir(dir_1, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Create (0=pass): %d\n", path4, rv);
    }

    //  Look through seen table
    check = 0;
    for( int i = 0; seen_table[i].name != NULL; i++){
        if(seen_table[i].seen > 0){
            check++;
        }
    }
    ck_assert_int_eq(check, 6);
    ck_assert_int_eq(rv, 0);
    fs_ops.create(path5, mode, fi);
    rv = fs_ops.readdir(new_dir, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Create (0=pass): %d\n", path5, rv);
    }
   //  Look through seen table
    check = 0;
    for( int i = 0; seen_table[i].name != NULL; i++){
        if(seen_table[i].seen > 0){
            check++;
        }
    }
    ck_assert_int_eq(check, 7);
    ck_assert_int_eq(rv, 0);

    //  Unlink files
    fs_ops.unlink(path5);
    rv = fs_ops.readdir(new_dir, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Unlink (0=pass): %d\n", path5, rv);
    }
    //  Look through seen table
    check = 0;
    for( int i = 0; seen_table[i].name != NULL; i++){
        if(seen_table[i].seen > 0){
            check++;
        }
    }
    ck_assert_int_eq(check, 7);
    ck_assert_int_eq(rv, 0);

    fs_ops.unlink(path4);
    rv = fs_ops.readdir(dir_1, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Unlink (-2=pass): %d\n", path4, rv);
    }
    //  Look through seen table
    check = 0;
    for( int i = 0; seen_table[i].name != NULL; i++){
        if(seen_table[i].seen > 0){
            check++;
        }
    }
    ck_assert_int_eq(check, 7);
    fs_ops.unlink(path3);
    rv = fs_ops.readdir(root, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Unlink (-2=pass): %d\n", path3, rv);
    }
    //  Look through seen table
    check = 0;
    for( int i = 0; seen_table[i].name != NULL; i++){
        if(seen_table[i].seen > 0){
            check++;
        }
    }
    ck_assert_int_eq(check, 7);
    ck_assert_int_eq(rv, 0);
    fs_ops.unlink(path2);
    rv = fs_ops.readdir(root, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Unlink (-2=pass): %d\n", path2, rv);
    }
    //  Look through seen table
    check = 0;
    for( int i = 0; seen_table[i].name != NULL; i++){
        if(seen_table[i].seen > 0){
            check++;
        }
    }
    ck_assert_int_eq(check, 7);
    ck_assert_int_eq(rv, 0);

    fs_ops.unlink(path1);
    rv = fs_ops.readdir(root, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Unlink (-2=pass): %d\n", path1, rv);
    }
    //  Look through seen table
    check = 0;
    for( int i = 0; seen_table[i].name != NULL; i++){
        if(seen_table[i].seen > 0){
            check++;
        }
    }
    ck_assert_int_eq(check, 7);
    ck_assert_int_eq(rv, 0);

    //  Remove dirs
    fs_ops.rmdir("/dir_1/new_dir");
    rv = fs_ops.readdir("/dir_1/new_dir", ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir rmdir (-2=pass): %d\n", "/dir_1/new_dir", rv);
    }
    ck_assert_int_lt(rv, 0);
    fs_ops.rmdir("/dir_1");
    rv = fs_ops.readdir("/dir_1", ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir rmdir (-2=pass): %d\n", "/dir_1", rv);
    }
    ck_assert_int_lt(rv, 0);

    //  Free Memory
    free(fi);
    free(ptr);

}
END_TEST


//  Tests Errors for Create
START_TEST(d_test){
    //  Testing Variables
    int rv;
    void *ptr = malloc(128);
    off_t offset = 0;
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));

    //  Local Variables
    mode_t mode_file = 100666;
    mode_t mode_dir = 0777;

    //  Create Bad paths /a/b/c
    char *path1 = "/first_dir";
    fs_ops.mkdir(path1, mode_dir);
    rv = fs_ops.readdir(path1, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\nCreate Errors Test:\n");
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path1, rv);
    }
    ck_assert_int_eq(rv, 0);

    //  b doesn't exist
    char *path2 = "/first_dir/second/file";
    rv = fs_ops.create(path2, mode_file, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Create Error (-ENOENT=pass): %d\n", path2, rv);
    }
    ck_assert_int_eq(rv, -ENOENT);

    //  b isn't dir
    char *path3 = "/first_dir/b/2_file";
    rv = fs_ops.create(path3, mode_file, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Create Error (-ENOTDIR=pass): %d\n", path3, rv);
    }
    ck_assert_int_eq(rv, -ENOENT);

    char *path3a = "/first_dir/2_file";
    rv = fs_ops.create(path3a, mode_file, fi);

    char *path4 = "/first_dir/2_file/err";
    rv = fs_ops.create(path4, mode_file, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Create Error (-ENOTDIR=pass): %d\n", path4, rv);
    }
    ck_assert_int_eq(rv, -ENOTDIR);

    //  c exists, is file
    char *path5 = "/first_dir/second";
    fs_ops.mkdir(path5, mode_dir);
    rv = fs_ops.readdir(path5, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path5, rv);
    }
    ck_assert_int_eq(rv, 0);
    char *path6 = "/first_dir/second/file";
    rv = fs_ops.create(path6, mode_file, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Create Pass (0=pass): %d\n", path6, rv);
    }
    ck_assert_int_eq(rv, 0);
    rv = fs_ops.create(path6, mode_file, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Create Error (-EEXIST=pass): %d\n", path6, rv);
    }
    ck_assert_int_eq(rv, -EEXIST);

    //  c exists, is dir
    char *path7 = "/first_dir/second/dir_fi";
    fs_ops.mkdir(path7, mode_dir);
    rv = fs_ops.readdir(path7, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path7, rv);
    }
    ck_assert_int_eq(rv, 0);

    rv = fs_ops.create(path7, mode_file, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Create Error (-EEXIST=pass): %d\n", path7, rv);
    }

    ck_assert_int_eq(rv, -EEXIST);

    //  Too long name
    // char *path8 = "/new_file_with_a_very_vvvvvv";
    // rv = fs_ops.create(path8, mode_file, fi);
    // if(DEBUG == 1){
    //     printf("\tPath: %s     Create Error (-EINVAL=pass): %d\n", path8, rv);
    // }
    // ck_assert_int_eq(rv, -EINVAL);

    //  Free memory
    free(fi);
    free(ptr);

}
END_TEST


//  Tests Errors for Unlink
START_TEST(e_test){
    //  Testing Variables
    int rv;
    void *ptr = malloc(128);
    off_t offset = 0;
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));

    //  Local Variables
    mode_t mode_dir = 0777;

    //  Create Bad paths /a/b/c
    char *path1 = "/1_dir";
    fs_ops.mkdir(path1, mode_dir);
    rv = fs_ops.readdir(path1, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\nUnlink Errors Test:\n");
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path1, rv);
    }
    ck_assert_int_eq(rv, 0);
    char *path2 = "/1_dir/subdir";
    fs_ops.mkdir(path2, mode_dir);
    rv = fs_ops.readdir(path2, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path2, rv);
    }
    ck_assert_int_eq(rv, 0);
    char *path3 = "/1_dir/subdir/subsub";
    fs_ops.mkdir(path3, mode_dir);
    rv = fs_ops.readdir(path3, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path3, rv);
    }
    ck_assert_int_eq(rv, 0);

    //  b doesn't exist
    char *path4 = "/1_dir/file";
    rv = fs_ops.unlink(path4);
    if(DEBUG == 1){
        printf("\tPath: %s     Unlink Error (-ENOENT=pass): %d\n", path4, rv);
    }
    ck_assert_int_eq(rv, -ENOENT);

    //  b isn't dir
    char *path5 = "/1_dir/dir_2/file";
    rv = fs_ops.unlink(path5);
    if(DEBUG == 1){
        printf("\tPath: %s     Unlink Error (-ENOTDIR=pass): %d\n", path5, rv);
    }
    ck_assert_int_eq(rv, -ENOENT);

    //  c doesn't exist
    char *path6 = "/1_dir/subdir/file";
    rv = fs_ops.unlink(path6);
    if(DEBUG == 1){
        printf("\tPath: %s     Unlink Error (-ENOENT=pass): %d\n", path6, rv);
    }
    ck_assert_int_eq(rv, -ENOENT);

    //  c is dir
    char *path7 = "/1_dir/subdir/subsub";
    rv = fs_ops.unlink(path7);
    if(DEBUG == 1){
        printf("\tPath: %s     Unlink Error (-EISDIR=pass): %d\n", path7, rv);
    }
    ck_assert_int_eq(rv, -EISDIR);

    //  Free memory
    free(fi);
    free(ptr);

}
END_TEST

//  Tests Errors for Mkdir
START_TEST(f_test){
    //  Testing Variables
    int rv;
    void *ptr = malloc(128);
    off_t offset = 0;
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));

    //  Local Variables
    mode_t mode_dir = 0777;
    mode_t mode_file = 100666;

    //  Create Bad paths /a/b/c
    //  /top/subdir/subsub  /top/file   /top/subdir/file
    char *path1 = "/top";
    fs_ops.mkdir(path1, mode_dir);
    rv = fs_ops.readdir(path1, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\nMkdir Errors Test:\n");
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path1, rv);
    }
    ck_assert_int_eq(rv, 0);
    char *path2 = "/top/subdir";
    fs_ops.mkdir(path2, mode_dir);
    rv = fs_ops.readdir(path2, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path2, rv);
    }
    ck_assert_int_eq(rv, 0);
    char *path3 = "/top/subdir/subsub";
    fs_ops.mkdir(path3, mode_dir);
    rv = fs_ops.readdir(path3, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path3, rv);
    }
    ck_assert_int_eq(rv, 0);
    char *path4 = "/top/subdir/file";
    fs_ops.create(path4, mode_file, fi);


    char *path5 = "/top/file";
    fs_ops.create(path5, mode_file, fi);

    //  b doesn't exist
    char *path6 = "/top/new_dir/last";
    rv = fs_ops.mkdir(path6, mode_dir);
    if(DEBUG == 1){
        printf("\tPath: %s     Mkdir Error (-ENOENT=pass): %d\n", path6, rv);
    }
    ck_assert_int_eq(rv, -ENOENT);

    //  b isn't dir
    char *path7 = "/top/file/last";
    rv = fs_ops.mkdir(path7, mode_dir);
    if(DEBUG == 1){
        printf("\tPath: %s     Mkdir Error (-ENOTDIR=pass): %d\n", path7, rv);
    }
    ck_assert_int_eq(rv, -ENOTDIR);

    //  c exists, is file
    char *path8 = "/top/subdir/file";
    rv = fs_ops.mkdir(path8, mode_dir);
    if(DEBUG == 1){
        printf("\tPath: %s     Mkdir Error (-EEXIST=pass): %d\n", path8, rv);
    }
    ck_assert_int_eq(rv, -EEXIST);

    //  c exists, is dir
    char *path9 = "/top/subdir/subsub";
    rv = fs_ops.mkdir(path9, mode_dir);
    if(DEBUG == 1){
        printf("\tPath: %s     Mkdir Error (-EEXIST=pass): %d\n", path9, rv);
    }
    ck_assert_int_eq(rv, -EEXIST);

    //  Free memory
    free(fi);
    free(ptr);

}
END_TEST


//  Tests Errors for Rmdir
START_TEST(g_test){
    //  Testing Variables
    int rv;
    void *ptr = malloc(128);
    off_t offset = 0;
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));

    //  Local Variables
    mode_t mode_dir = 0777;
    mode_t mode_file = 100666;

    //  Create Bad paths /a/b/c
    char *path1 = "/first";
    fs_ops.mkdir(path1, mode_dir);
    rv = fs_ops.readdir(path1, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\nRmdir Errors Test:\n");
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path1, rv);
    }
    ck_assert_int_eq(rv, 0);

    char *path2 = "/first/second";
    fs_ops.mkdir(path2, mode_dir);
    rv = fs_ops.readdir(path2, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path2, rv);
    }
    ck_assert_int_eq(rv, 0);

    char *path3 = "/first/second/last";
    fs_ops.mkdir(path3, mode_dir);
    rv = fs_ops.readdir(path3, ptr, filler, offset, fi);
    if(DEBUG == 1){
        printf("\tPath: %s     Readdir Mkdir (0=pass): %d\n", path3, rv);
    }
    ck_assert_int_eq(rv, 0);

    char *path4 = "/first/second/file";
    fs_ops.create(path4, mode_file, fi);

    char *path5 = "/first/file";
    fs_ops.create(path5, mode_file, fi);

    //  b doesn't exist
    char *path6 = "/first/yup";
    rv = fs_ops.rmdir(path6);

    ck_assert_int_eq(rv, -ENOENT);

    //  b isn't dir
    char *path7 = "/first/file";
    rv = fs_ops.rmdir(path7);
    if(DEBUG == 1){
        printf("\tPath: %s     Rmdir Error (-ENOTDIR=pass): %d\n", path7, rv);
    }
    ck_assert_int_eq(rv, -ENOTDIR);

    //  c doesn't exist
    char *path8 = "/first/second/third";
    rv = fs_ops.rmdir(path8);
    if(DEBUG == 1){
        printf("\tPath: %s     Rmdir Error (-ENOENT=pass): %d\n", path8, rv);
    }
    ck_assert_int_eq(rv, -ENOENT);

    //  c is file
    char *path9 = "/first/second/file";
    rv = fs_ops.rmdir(path9);
    if(DEBUG == 1){
        printf("\tPath: %s     Rmdir Error (-ENOTDIR=pass): %d\n", path9, rv);
    }
    ck_assert_int_eq(rv, -ENOTDIR);

    //  dir is not empty
    char *path10 = "/first";
    rv = fs_ops.rmdir(path10);
    if(DEBUG == 1){
        printf("\tPath: %s     Rmdir Error (-ENOTEMPTY=pass): %d\n", path10, rv);
    }
    ck_assert_int_eq(rv, -ENOTEMPTY);

    //  Free memory
    free(fi);
    free(ptr);

}
END_TEST

//  Simple Block Size Writes Test
START_TEST(h_test){
    //  Local Variables
    int i, rv;
    void *ptr = malloc(128);
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));
    unsigned cksum_orig, cksum_new;
    mode_t mode_file = 100666;

    if(DEBUG == 1)
        printf("\nSimple <Block Writes/Deltes - Append/Unlink Tests\n");

    struct statvfs *sb = malloc(sizeof(struct statvfs));
    fs_ops.statfs("/", sb);
    int orig_block_num = sb->f_bfree;
    /*    <1 block write    */
    //  Create file
    char *path1 = "/write_1";
    fs_ops.create(path1, mode_file, fi);
    // //rv = fs_ops.readdir(path1, ptr, filler, offset, fi);
    // if(DEBUG == 1)
    //     printf("\tPath: %s      Readdir Create (0=pass): %d\n", path1, rv);
    // //ck_assert_int_eq(rv, 0);

    //  Write to file
    char *ptr1, *buf1 = malloc(4010); // allocate a bit extra
    for (i=0, ptr1 = buf1; ptr1 < buf1+4000; i++)
        ptr1 += sprintf(ptr1, "%d ", i);

    rv = fs_ops.write(path1, buf1, 4000, 0, NULL);  // 4000 bytes, offset=0
    if(DEBUG == 1)
        printf("\tPath: %s      Write (0=pass): %d\n", path1, rv);
    ck_assert_int_eq(rv, 4000);

    //  Read from file
    char *write_buf1 = malloc(4000*sizeof(char));
    fs_ops.read(path1, write_buf1, 4000, 0, fi);

    //  Checksum
    cksum_orig = crc32(0, (const unsigned char *) buf1, 4000);
    cksum_new = crc32(0, (const unsigned char *) write_buf1, 4000);
    if(DEBUG == 1)
        printf("\tCalculated: %u  Expected: %u\n", cksum_new, cksum_orig);
    ck_assert_int_eq(cksum_new, cksum_orig);

    //  Free memory
    free(buf1);
    free(write_buf1);

    /*    <2 block write    */
    //  Create file
    char *path2 = "/write_2";
    fs_ops.create(path2, mode_file, fi);

    //  Write to file
    char *ptr2, *buf2 = malloc(8010); // allocate a bit extra
    for (i=0, ptr2 = buf2; ptr2 < buf2+8000; i++)
        ptr2 += sprintf(ptr2, "%d ", i);
    rv = fs_ops.write(path2, buf2, 8000, 0, NULL);  // 8000 bytes, offset=0
    if(DEBUG == 1)
        printf("\tPath: %s      Write (0=pass): %d\n", path2, rv);
    ck_assert_int_eq(rv, 8000);

    //  Read from file
    char *write_buf2 = malloc(8000*sizeof(char));
    fs_ops.read(path2, write_buf2, 8000, 0, fi);

    //  Checksum
    cksum_orig = crc32(0, (const unsigned char *) buf2, 8000);
    cksum_new = crc32(0, (const unsigned char *) write_buf2, 8000);
    if(DEBUG == 1)
        printf("\tCalculated: %u  Expected: %u\n", cksum_new, cksum_orig);
    ck_assert_int_eq(cksum_new, cksum_orig);

    //  Free memory
    free(buf2);
    free(write_buf2);

    /*    <3 block write    */
    //  Create file
    char *path3 = "/write_3";
    fs_ops.create(path3, mode_file, fi);

    //  Write to file
    char *ptr3, *buf3 = malloc(12010); // allocate a bit extra
    for (i=0, ptr3 = buf3; ptr3 < buf3+12000; i++)
        ptr3 += sprintf(ptr3, "%d ", i);
    rv = fs_ops.write(path3, buf3, 12000, 0, NULL);  // 12000 bytes, offset=0
    if(DEBUG == 1)
        printf("\tPath: %s      Write (0=pass): %d\n", path3, rv);
    ck_assert_int_eq(rv, 12000);

    //  Read from file
    char *write_buf3 = malloc(12000*sizeof(char));
    fs_ops.read(path3, write_buf3, 12000, 0, fi);

    //  Checksum
    cksum_orig = crc32(0, (const unsigned char *) buf3, 12000);
    cksum_new = crc32(0, (const unsigned char *) write_buf3, 12000);
    if(DEBUG == 1)
        printf("\tCalculated: %u  Expected: %u\n", cksum_new, cksum_orig);
    ck_assert_int_eq(cksum_new, cksum_orig);

    //  Free memory
    free(buf3);
    free(write_buf3);

    //  Check Free blocks after adds
    fs_ops.statfs("/", sb);
    int added_block_num = sb->f_bfree;

    //  Unlink files
    rv = fs_ops.unlink(path1);
    if(DEBUG == 1)
        printf("\tPath: %s      File Unlinked (0=pass): %d\n", path1, rv);
    ck_assert_int_eq(rv, 0);

    rv = fs_ops.unlink(path2);
    if(DEBUG == 1)
        printf("\tPath: %s      File Unlinked (0=pass): %d\n", path2, rv);
    ck_assert_int_eq(rv, 0);

    rv = fs_ops.unlink(path3);
    if(DEBUG == 1)
        printf("\tPath: %s      File Unlinked (0=pass): %d\n", path3, rv);
    ck_assert_int_eq(rv, 0);

    //  Check Free blocks after unlink
    fs_ops.statfs("/", sb);
    int unlinked_blocks = sb->f_bfree;

    //  Check free blocks
    if(DEBUG == 1)
        printf("\tFree Blocks Originally: %d    Free Blocks After Writes: %d    Free Blocks After Unlinks:%d\n", orig_block_num, added_block_num, unlinked_blocks);
    ck_assert_int_ne(added_block_num, orig_block_num);
    ck_assert_int_eq(unlinked_blocks, orig_block_num);

    //  Free memory
    free(fi);
    free(ptr);

    free(sb);

}
END_TEST

//  Simple Block Size Truncate Tests
START_TEST(i_test){
    //  Local Variables
    int i, rv;
    void *ptr = malloc(128);
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));
    unsigned cksum_orig, cksum_new;
    mode_t mode_file = 100666;

    if(DEBUG == 1)
        printf("\nSimple <Block Writes/Truncates - Append/Truncate Tests\n");

    struct statvfs *sb = malloc(sizeof(struct statvfs));
    fs_ops.statfs("/", sb);
    int orig_block_num = sb->f_bfree;

    /*    <1 block write    */
    //  Create file
    char *path1 = "/truncate_1";
    fs_ops.create(path1, mode_file, fi);
    //  Write to file
    char *ptr1, *buf1 = malloc(4010); // allocate a bit extra
    for (i=0, ptr1 = buf1; ptr1 < buf1+4000; i++)
        ptr1 += sprintf(ptr1, "%d ", i);
    rv = fs_ops.write(path1, buf1, 4000, 0, NULL);  // 4000 bytes, offset=0
    if(DEBUG == 1)
        printf("\tPath: %s      Write (0=pass): %d\n", path1, rv);
    ck_assert_int_eq(rv, 4000);

    //  Read from file
    char *write_buf1 = malloc(4000*sizeof(char));
    fs_ops.read(path1, write_buf1, 4000, 0, fi);

    //  Checksum
    cksum_orig = crc32(0, (const unsigned char *) buf1, 4000);
    cksum_new = crc32(0, (const unsigned char *) write_buf1, 4000);
    if(DEBUG == 1)
        printf("\tCalculated: %u  Expected: %u\n", cksum_new, cksum_orig);
    ck_assert_int_eq(cksum_new, cksum_orig);

    //  Free memory
    free(buf1);
    free(write_buf1);

    /*    <2 block write    */
    //  Create file
    char *path2 = "/truncate_2";
    fs_ops.create(path2, mode_file, fi);

    //  Write to file
    char *ptr2, *buf2 = malloc(8010); // allocate a bit extra
    for (i=0, ptr2 = buf2; ptr2 < buf2+8000; i++)
        ptr2 += sprintf(ptr2, "%d ", i);
    rv = fs_ops.write(path2, buf2, 8000, 0, NULL);  // 8000 bytes, offset=0
    if(DEBUG == 1)
        printf("\tPath: %s      Write (0=pass): %d\n", path2, rv);
    ck_assert_int_eq(rv, 8000);

    //  Read from file
    char *write_buf2 = malloc(8000*sizeof(char));
    fs_ops.read(path2, write_buf2, 8000, 0, fi);

    //  Checksum
    cksum_orig = crc32(0, (const unsigned char *) buf2, 8000);
    cksum_new = crc32(0, (const unsigned char *) write_buf2, 8000);
    if(DEBUG == 1)
        printf("\tCalculated: %u  Expected: %u\n", cksum_new, cksum_orig);
    ck_assert_int_eq(cksum_new, cksum_orig);

    //  Free memory
    free(buf2);
    free(write_buf2);

    /*    <3 block write    */
    //  Create file
    char *path3 = "/truncate_3";
    fs_ops.create(path3, mode_file, fi);

    //  Write to file
    char *ptr3, *buf3 = malloc(12010); // allocate a bit extra
    for (i=0, ptr3 = buf3; ptr3 < buf3+12000; i++)
        ptr3 += sprintf(ptr3, "%d ", i);
    rv = fs_ops.write(path3, buf3, 12000, 0, NULL);  // 12000 bytes, offset=0
    if(DEBUG == 1)
        printf("\tPath: %s      Write (0=pass): %d\n", path3, rv);
    ck_assert_int_eq(rv, 12000);

    //  Read from file
    char *write_buf3 = malloc(12000*sizeof(char));
    fs_ops.read(path3, write_buf3, 12000, 0, fi);

    //  Checksum
    cksum_orig = crc32(0, (const unsigned char *) buf3, 12000);
    cksum_new = crc32(0, (const unsigned char *) write_buf3, 12000);
    if(DEBUG == 1)
        printf("\tCalculated: %u  Expected: %u\n", cksum_new, cksum_orig);
    ck_assert_int_eq(cksum_new, cksum_orig);

    //  Free memory
    free(buf3);
    free(write_buf3);

    //  Check Free blocks after adds
    fs_ops.statfs("/", sb);
    int added_block_num = sb->f_bfree;

    //  Unlink files
    rv = fs_ops.truncate(path1, 0);
    if(DEBUG == 1)
        printf("\tPath: %s      File Truncated (0=pass): %d\n", path1, rv);
    ck_assert_int_eq(rv, 0);

    rv = fs_ops.truncate(path2, 0);
    if(DEBUG == 1)
        printf("\tPath: %s      File Truncated (0=pass): %d\n", path2, rv);
    ck_assert_int_eq(rv, 0);

    rv = fs_ops.truncate(path3, 0);
    if(DEBUG == 1)
        printf("\tPath: %s      File Truncated (0=pass): %d\n", path3, rv);
    ck_assert_int_eq(rv, 0);
    fs_ops.unlink(path1);
    fs_ops.unlink(path2);
    fs_ops.unlink(path3);
    //  Check Free blocks after unlink
    fs_ops.statfs("/", sb);
    int unlinked_blocks = sb->f_bfree;

    //  Check free blocks
    if(DEBUG == 1)
        printf("\tFree Blocks Originally: %d    Free Blocks After Writes: %d    Free Blocks After Truncate:%d\n", orig_block_num, added_block_num, unlinked_blocks);
    ck_assert_int_ne(added_block_num, orig_block_num);
    ck_assert_int_eq(unlinked_blocks, orig_block_num);

    //  Free memory
    free(fi);
    free(ptr);
    free(sb);

}
END_TEST

//  Overwrite Test
START_TEST(j_test){
    //  Local Variables
    int i, rv;
    void *ptr = malloc(128);
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));
    unsigned cksum_orig, cksum_new, cksum_2orig;
    mode_t mode_file = 100666;

    if(DEBUG == 1)
        printf("\nOverwrite Block Writes - Write/Unlink Tests\n");

    struct statvfs *sb = malloc(sizeof(struct statvfs));
    fs_ops.statfs("/", sb);
    int orig_block_num = sb->f_bfree;

    /*    <1 block write    */
    //  Create file
    char *path1 = "/overwrite";
    fs_ops.create(path1, mode_file, fi);
    //  Write to file
    char *ptr1, *buf1 = malloc(4010); // allocate a bit extra
    for (i=0, ptr1 = buf1; ptr1 < buf1+4000; i++)
        ptr1 += sprintf(ptr1, "%d ", i);
    rv = fs_ops.write(path1, buf1, 4000, 0, NULL);  // 4000 bytes, offset=0

    if(DEBUG == 1)
        printf("\tPath: %s      Write (0=pass): %d\n", path1, rv);
    ck_assert_int_eq(rv, 4000);

    //  Write second buffer to file
    char *ptr2, *buf2 = malloc(4010); // allocate a bit extra
    for (i=0, ptr2 = buf2; ptr2 < buf2+4000; i++)
        ptr2 += sprintf(ptr2, "%d ", i+307);
    rv = fs_ops.write(path1, buf2, 4000, 0, NULL);  // 4000 bytes, offset=0
    if(DEBUG == 1)
        printf("\tPath: %s      Write (0=pass): %d\n", path1, rv);
    ck_assert_int_eq(rv, 4000);

    //  Read from file
    char *write_buf1 = malloc(4000*sizeof(char));
    fs_ops.read(path1, write_buf1, 4000, 0, fi);

    //  Checksum
    cksum_orig = crc32(0, (const unsigned char *) buf1, 4000);
    cksum_2orig = crc32(0, (const unsigned char *) buf2, 4000);
    cksum_new = crc32(0, (const unsigned char *) write_buf1, 4000);
    if(DEBUG == 1)
        printf("\tCalculated: %u  Expected: %u  Old Data: %u\n", cksum_new, cksum_2orig, cksum_orig);
    ck_assert_int_ne(cksum_new, cksum_orig);
    ck_assert_int_eq(cksum_new, cksum_2orig);

    //  Free memory
    free(buf1);
    free(buf2);
    free(write_buf1);

    //  Check Free blocks after adds
    fs_ops.statfs("/", sb);
    int added_block_num = sb->f_bfree;

    //  Unlink files
    rv = fs_ops.unlink(path1);
    if(DEBUG == 1)
        printf("\tPath: %s      File Unlinked (0=pass): %d\n", path1, rv);
    ck_assert_int_eq(rv, 0);

    //  Check Free blocks after unlink
    fs_ops.statfs("/", sb);
    int unlinked_blocks = sb->f_bfree;

    //  Check free blocks
    if(DEBUG == 1)
        printf("\tFree Blocks Originally: %d    Free Blocks After Writes: %d    Free Blocks After Unlinks:%d\n\n", orig_block_num, added_block_num, unlinked_blocks);
    ck_assert_int_ne(added_block_num, orig_block_num);
    ck_assert_int_eq(unlinked_blocks, orig_block_num);

    //  Free memory
    free(fi);
    free(ptr);

    free(sb);

}
END_TEST

extern struct fuse_operations fs_ops;
extern void block_init(char *file);

int main(int argc, char **argv){
    //  Reset test image
    system("python gen-disk.py -q disk2.in test2.img");

    block_init("test2.img");
    fs_ops.init(NULL);

    //  Create testing suit
    Suite *s = suite_create("fs5600");


    //  Create tests
    TCase *tc1 = tcase_create("mkdir/rmdir test");
    TCase *tc2 = tcase_create("create/unlink test");
    TCase *tc4 = tcase_create("error create test");
    TCase *tc5 = tcase_create("error unlink test");
    TCase *tc6 = tcase_create("error mkdir test");
    TCase *tc7 = tcase_create("error rmdir test");
    TCase *tc8 = tcase_create("simple append write/unlink test");
    TCase *tc9 = tcase_create("simple append write/truncate test");
    TCase *tc10 = tcase_create("overwrite test");

    //  Add tests
    tcase_add_test(tc1, a_test);
    tcase_add_test(tc2, b_test);
    tcase_add_test(tc4, d_test);
    tcase_add_test(tc5, e_test);
    tcase_add_test(tc6, f_test);
    tcase_add_test(tc7, g_test);
    tcase_add_test(tc8, h_test);
    tcase_add_test(tc9, i_test);
    tcase_add_test(tc10, j_test);

    //  Add test cases to suit
    suite_add_tcase(s, tc1);
    suite_add_tcase(s, tc2);
    suite_add_tcase(s, tc4);
    suite_add_tcase(s, tc5);
    suite_add_tcase(s, tc6);
    suite_add_tcase(s, tc7);
    suite_add_tcase(s, tc8);
    suite_add_tcase(s, tc9);
    suite_add_tcase(s, tc10);

    //  Run test cases
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_VERBOSE);
    int n_failed = srunner_ntests_failed(sr);
    printf("%d tests failed\n", n_failed);

    //  Return and free memory
    srunner_free(sr);
    return (n_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
