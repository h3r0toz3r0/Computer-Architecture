/*
 * file:        testing.c
 * description: libcheck test skeleton for file system project
 *
 */

#define _FILE_OFFSET_BITS 64
#define FUSE_USE_VERSION 26
#define DEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include <zlib.h>
#include <fuse.h>
#include <errno.h>

extern struct fuse_operations fs_ops;
extern void block_init(char *file);

struct {
    char *path;
    int uid;
    int gid;
    mode_t mode;
    int  size;
    long int ctime;
    long int mtime;
    int inum;
    unsigned cksum;  /* UNSIGNED. TESTS WILL FAIL IF IT'S NOT */
} table_results[] = {
    {"/",                                   0, 0, 040777, 4096, 1565283152, 1565283167, 2, 0},
    {"/file.1k",                            500, 500, 0100666, 1000, 1565283152, 1565283152, 389, 1786485602},
    {"/file.10",                            500, 500, 0100666, 10, 1565283152, 1565283167, 268, 855202508},
    {"/dir-with-long-name",                 0, 0, 040777, 4096, 1565283152, 1565283167, 253, 0},
    {"/dir-with-long-name/file.12k+",       0, 500, 0100666, 12289, 1565283152, 1565283167, 327, 4101348955},
    {"/dir2",                               500, 500, 040777, 8192, 1565283152, 1565283167, 213, 0},
    {"/dir2/twenty-seven-byte-file-name",   500, 500, 0100666, 1000, 1565283152, 1565283167, 55, 2575367502},
    {"/dir2/file.4k+",                      500, 500, 0100777, 4098, 1565283152, 1565283167, 139, 799580753},
    {"/dir3",                               0, 500, 040777, 4096, 1565283152, 1565283167, 238, 0},
    {"/dir3/subdir",                        0, 500, 040777, 4096, 1565283152, 1565283167, 338, 0},
    {"/dir3/subdir/file.4k-",               500, 500, 0100666, 4095, 1565283152, 1565283167, 188, 4220582896},
    {"/dir3/subdir/file.8k-",               500, 500, 0100666, 8190, 1565283152, 1565283167, 21, 4090922556},
    {"/dir3/subdir/file.12k",               500, 500, 0100666, 12288, 1565283152, 1565283167, 71, 3243963207},
    {"/dir3/file.12k-",                     0, 500, 0100777, 12287, 1565283152, 1565283167, 146, 2954788945},
    {"/file.8k+",                           500, 500, 0100666, 8195, 1565283152, 1565283167, 59, 2112223143},
    {NULL}
};

struct {
    char *name;
    int   seen;
} seen_table[] = {
    {"dir2", 0},
    {"dir3", 0},
    {"dir-with-long-name", 0},
    {"file.10", 0},
    {"file.1k", 0},
    {"file.8k+", 0},
    {"twenty-seven-byte-file-name", 0},
    {"file.4k+", 0},
    {"subdir", 0},
    {"file.12k-", 0},
    {"file.4k-", 0},
    {"file.8k-", 0},
    {"file.12k", 0},
    {"file.12k+", 0},
    {NULL}
};

/* this is the callback function for readdir */
int filler(void *ptr, const char *name, const struct stat *stbuf, off_t offset){
    /* FUSE passes you the entry name and a pointer to a 'struct stat'
    * with the attributes. Ignore the 'ptr' and 'off' arguments
    *
    */
    printf("entered filler from ut1\n");
    //   Print name of path
    if(DEBUG == 1)
        printf("\t\"%s\" ", name);

    //  Update seen table
    for( int i = 0; seen_table[i].name != NULL; i++){
        seen_table[i].seen = 1;
    }

    //  Return 0 upon success
    return 0;
}

//      Individual tests for each file and directory      //
/*
    `fs_getattr` - for each of the files and directories,
    call `getattr` and verify the results against values from the table above.
*/
START_TEST(a_test){
    //  Local Variables
    struct stat *sb = malloc(sizeof(struct stat));
    int i;

    //  Intro
    if (DEBUG == 1)
        printf("\nGetattr:\n");

    //  For loop through all paths
    for ( i = 0; table_results[i].path != NULL; i++ ){
        fs_ops.getattr(table_results[i].path, sb);

        //  Print stats
        if( DEBUG == 1 )
            printf("\t\"%s\", %d, %d, %o, %ld, %ld, %ld\n", table_results[i].path, sb->st_uid, sb->st_gid, sb->st_mode, sb->st_size, sb->st_ctim.tv_nsec, sb->st_mtim.tv_nsec);

        //  Check if matching values
        ck_assert_int_eq(table_results[i].uid, sb->st_uid);
        ck_assert_int_eq(table_results[i].gid, sb->st_gid);
        ck_assert_int_eq(table_results[i].mode, sb->st_mode);
        ck_assert_int_eq(table_results[i].size, sb->st_size);
        ck_assert_int_eq(table_results[i].ctime, sb->st_ctim.tv_nsec);
        ck_assert_int_eq(table_results[i].mtime, sb->st_mtim.tv_nsec);
    }

    //  Free memory
    free(sb);
}
END_TEST

/*
    `fs_getattr` - path translation errors. Here's a list to test, giving error code and path:
    * ENOENT - "/not-a-file"
    * ENOTDIR - "/file.1k/file.0"
    * ENOENT on a middle part of the path - "/not-a-dir/file.0"
    * ENOENT in a subdirectory "/dir2/not-a-file"
*/
START_TEST(b_test){
    //  Local Variables
    struct stat *sb = malloc(sizeof(struct stat));
    int i, rv;

    //  Intro
    if (DEBUG == 1)
        printf("\nGetattr Errors:\n");

    //  Error files
    char *error_names[5] = {"/not-a-file", "/file.1k/file.0", "/file.1k/file.0", "/not-a-dir/file.0", "/dir2/not-a-file"};

    //  Loop through error file names
    for ( i = 0; i < 5; i++ ){
        rv = fs_ops.getattr(error_names[i], sb);

        //  Check if error matches
        if( strcmp(error_names[i], "/not-a-file") == 0)
            ck_assert_int_eq(rv, -ENOENT);
        else if( strcmp(error_names[i], "/file.1k/file.0") == 0)
            ck_assert_int_eq(rv, -ENOTDIR);
        else if( strcmp(error_names[i], "/not-a-dir/file.0") == 0)
            ck_assert_int_eq(rv, -ENOENT);
        else if( strcmp(error_names[i], "/dir2/not-a-file") == 0)
            ck_assert_int_eq(rv, -ENOENT);

        //  Print for debug
        if(DEBUG == 1)
            printf("\tPath: %s\t\tError: %d\n", error_names[i], rv);
    }
}
END_TEST

/*
    `fs_readdir` - check that calling readdir on each directory in
    the table above returns the proper set of entries.
*/
START_TEST(c_test){
    //  Local Variables
    int rv, check = 0;
    void *ptr = malloc(128);

    //  Intro
    if (DEBUG == 1)
        printf("\nReaddir:\n");

    //  For loop through all paths that are dirs
    for ( int i = 0; table_results[i].path != NULL; i++ ){
        if (S_ISREG(table_results[i].mode) == 0){
            printf("\t\"%s\" : ", table_results[i].path);
            rv = fs_ops.readdir(table_results[i].path, ptr, filler, 0, (struct fuse_file_info*) NULL);
            ck_assert(rv >= 0);
            printf("\n");
        }
    }

    //  Check seen table and ensure all values were seen
    for( int i = 0; seen_table[i].name != NULL; i++){
        check = seen_table[i].seen;
        ck_assert(check == 1);
    }
}
END_TEST

/*
    `fs_readdir` errors - call readdir on a file that exists,
    and on a path that doesn't exist, verify you get ENOTDIR and ENOENT respectively.
*/
START_TEST(d_test){
    //  Local Variables
    int i, rv;
    void *ptr = malloc(128);

    //  Error files
    char *error_names[5] = {"/not-a-file", "/file.1k/file.0", "/file.1k/file.0", "/not-a-dir/file.0", "/dir2/not-a-file"};

    //  Debug printing
    if(DEBUG == 1){
        printf("\nReaddir Errors:\n");
    }

    //  Check all error files/dirs
    for(i = 0; i < 5; i++){
        rv = fs_ops.readdir(error_names[i], ptr, filler, 0, (struct fuse_file_info*) NULL);

        //  Debug printing
        if(DEBUG == 1){
            printf("\t%s    :   error code %d\n", error_names[i], rv);
        }

        //  Check if error matches
        if( strcmp(error_names[i], "/not-a-file") == 0)
            ck_assert_int_eq(rv, -ENOENT);
        else if( strcmp(error_names[i], "/file.1k/file.0") == 0)
            ck_assert_int_eq(rv, -ENOTDIR);
        else if( strcmp(error_names[i], "/not-a-dir/file.0") == 0)
            ck_assert_int_eq(rv, -ENOENT);
        else if( strcmp(error_names[i], "/dir2/not-a-file") == 0)
            ck_assert_int_eq(rv, -ENOENT);
    }
}
END_TEST

/*
    `fs_read` - single big read - The simplest test is to read the entire file in
    a single operation and make sure that it matches the correct value.
*/
START_TEST(e_test){
    //  Local Variables
    size_t len;
    off_t offset;
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));
    int i;

    //  Intro
    if (DEBUG == 1)
        printf("\nBig Read:\n");

    //  For loop through all paths
    for ( i = 0; table_results[i].path != NULL; i++ ){
        len = table_results[i].size;
        char *buf = malloc(sizeof(char) * len);
        if (S_ISREG(table_results[i].mode) != 0){
            if(DEBUG == 1)
                printf("\t%s\t", table_results[i].path);

            //  Initialize read variables
            memset(buf, '\0', sizeof(char) * len);
            offset = 0;

            //  Call read
            fs_ops.read(table_results[i].path, buf, len, offset, fi);

            //  Check sum
            unsigned cksum = crc32(0, (const unsigned char *) buf, len);
            if(DEBUG == 1)
                printf("\tCalculated: %u  Expected: %u\n", cksum, table_results[i].cksum);
            ck_assert_int_eq(cksum, table_results[i].cksum);
        }
        //  Free buf
        free(buf);
    }

    //  Free memory
    free(fi);
}
END_TEST

/*
    I would suggest using a buffer bigger than the largest file and passing the
    size of that buffer to `fs_read` - that way you can find out if `fs_read`
    sometimes returns too much data.
*/
START_TEST(f_test){
        //  Local Variables
    size_t len;
    off_t offset;
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));
    int i;

    //  Intro
    if (DEBUG == 1)
        printf("\nError Read:\n");

    //  Specifc path
    i = 12;
    len = 15000;
    char *buf = malloc(sizeof(char) * len);
    if (S_ISREG(table_results[i].mode) != 0){
        if(DEBUG == 1)
            printf("\t%s\t", table_results[i].path);

        //  Initialize read variables
        memset(buf, '\0', sizeof(char) * len);
        offset = 0;

        //  Call read
        fs_ops.read(table_results[i].path, buf, len, offset, fi);

        //  Check sum
        unsigned cksum = crc32(0, (const unsigned char *) buf, len);
        if(DEBUG == 1){
            printf("\t%s\tSize of Buffer: %lu  Calculated: %u  Expected: %u\n", table_results[i].path, len, cksum, table_results[i].cksum);
            printf("\tNote! Checksum is different because fs_read returns failure if len+offset > file len as specified in README.md\n");
        }
        ck_assert(cksum > table_results[i].cksum);
    }
    //  Free memory
    free(buf);
    free(fi);
}
END_TEST

/*
    `fs_read` - multiple small reads - write a function to read a file N bytes
    at a time, and test that you can read each file in different sized chunks and
    that you get the right result. Note that there's no concept of a current
    position in the FUSE interface - you have to use the offset parameter. (e.g
    read (len=17,offset=0), then (len=17,offset=17), etc.)
    (I'll test your code with N=17, 100, 1000, 1024, 1970, and 3000)
*/
START_TEST(g_test){
    //  Local Variables
    size_t len;
    off_t offset;
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));
    int i;
    int numbers[6] = {17, 100, 1000, 1024, 1970, 3000};

    //  Intro
    if (DEBUG == 1)
        printf("\nSmall Reads:\n");

    //  For loop through all paths
    for ( i = 0; table_results[i].path != NULL; i++ ){
        if (S_ISREG(table_results[i].mode) != 0){
            //  For loop through read sizes

            for( int j = 0; j < 6; j++){
                //  Set up read variables
                len = numbers[j];
                offset = 0;
                // Call read
                char *buf = malloc(sizeof(char) * table_results[i].size);

                if(DEBUG == 1){
                    printf("\tPath: %s\n", table_results[i].path);
                    printf("\tLen: %ld    File Size: %d   Offset: %ld\n", len, table_results[i].size, offset);
                }

                int counter = 0;
                while(offset < table_results[i].size){
                  int rv = fs_ops.read(table_results[i].path, buf + counter*len, len, offset, fi);
                  if(len+offset > table_results[i].size){
                      ck_assert_int_eq(rv, (table_results[i].size - offset));
                  }
                  else if(offset > table_results[i].size){
                      ck_assert_int_eq(rv, 0);
                  }
                  offset += len;
                  counter += 1;
                }

                unsigned cksum = crc32(0, (const unsigned char *) buf, table_results[i].size);
                ck_assert_int_eq(cksum, table_results[i].cksum);
                //  Free buf
                free(buf);

            }

        }
    }
    //  Free memory
    free(fi);
}
END_TEST

/*
    `fs_statvfs` - run 'man statvfs' to see a description of the structure. The values for the test image should be:
        * `f_bsize` - 4096 (block size, bytes)
        * `f_blocks` - 1024 (total number of blocks)
        * `f_bfree` -  731 (free blocks)
        * `f_namemax` - 27
*/
START_TEST(h_test){
    //  Local Variables
    struct statvfs *st = malloc(sizeof(struct statvfs));
    char *path = "/";

    //  Intro
    if (DEBUG == 1)
        printf("\nStatvfs:\n");

    //  Run statfs on test.img
    fs_ops.statfs(path, st);


    //  Print stats
    if( DEBUG == 1 ){
        printf("\tf_bsize : %lu\n", st->f_bsize);
        printf("\tf_blocks : %lu\n", st->f_blocks);
        printf("\tf_bfree : %lu\n", st->f_bfree);
        printf("\tf_namemax : %lu\n", st->f_namemax);
    }

    // Check if matching values
    ck_assert_int_eq(st->f_bsize, 4096);
    ck_assert_int_eq(st->f_blocks, 400);
    ck_assert_int_eq(st->f_bfree, 355);
    ck_assert_int_eq(st->f_namemax, 27);

    //  Free memory
    free(st);
}
END_TEST

/*
    `fs_chmod` - change permissions for a file, check that (a) it's still a file,
    and (b) it has the new permissions. Do the same for a directory. (note that
    `chmod` should only change the bottom 9 bits of the mode word)
*/
START_TEST(i_test){
    //  Check chmod for file
    //  Local Variables
    char *file_path = "/dir3/subdir/file.4k-";
    mode_t mode1 = 040777;
    struct stat *sb1 = malloc(sizeof(struct stat));
    struct stat *sb2 = malloc(sizeof(struct stat));

    //  Get old mode
    fs_ops.getattr(file_path, sb1);

    //  Change mode
    fs_ops.chmod(file_path, mode1);

    //  Get new mode
    fs_ops.getattr(file_path, sb2);

    //  Check new mode
    if(DEBUG == 1){
        printf("\nChmod:\n");
        printf("\t%s    Original Mode: %o   New Mode: %o    Expected New Mode: %o\n", file_path, sb1->st_mode, sb2->st_mode, mode1);
    }

    //  Check sum
    ck_assert_int_eq(mode1, sb2->st_mode);

    //  Check still file
    if (S_ISDIR(sb2->st_mode) == 0)
        ck_assert_int_eq(1, 0);
    else{
        ck_assert_int_eq(1, 1);
    }

    //  Check chmod for dir
    char *dir_path = "/dir-with-long-name";
    mode_t mode2 = 0100777;
    struct stat *sb3 = malloc(sizeof(struct stat));
    struct stat *sb4 = malloc(sizeof(struct stat));

    //  Get old mode
    fs_ops.getattr(dir_path, sb3);

    //  Change mode
    fs_ops.chmod(dir_path, mode2);

    //  Get new mode
    fs_ops.getattr(dir_path, sb4);

    //  Check new mode
    if(DEBUG == 1){
        printf("\t%s    Original Mode: %o   New Mode: %o    Expected New Mode: %o\n", dir_path, sb3->st_mode, sb4->st_mode, mode2);
    }

    //  Check sum
    ck_assert_int_eq(mode2, sb4->st_mode);

    //  Check still dir
    if (S_ISDIR(sb4->st_mode) != 0)
        ck_assert_int_eq(1, 0);
    else{
        ck_assert_int_eq(1, 1);
    }

}
END_TEST

/*
    `fs_rename` - try renaming a file and then reading it, renaming a directory
    and then reading a file from the directory.
*/
START_TEST(j_test){
    //  Local Variables
    size_t len;
    off_t offset;
    struct fuse_file_info *fi = malloc(sizeof(struct fuse_file_info));
    struct stat *sb1 = malloc(sizeof(struct stat));
    struct stat *sb2 = malloc(sizeof(struct stat));

    //  Intro
    if (DEBUG == 1)
        printf("\nRename:\n");

    //  Rename a file
    //  Get attributes
    char *file_path = "/dir2/file.4k+";
    char *new_file = "/dir2/hello";
    fs_ops.getattr(file_path, sb1);
    len = sb1->st_size;
    char *buf = malloc(sizeof(char) * len);

    //  Initialize read variables
    memset(buf, '\0', sizeof(char) * len);
    offset = 0;

    //  Call read
    fs_ops.read(file_path, buf, len, offset, fi);

    //  Check sum
    unsigned cksum = crc32(0, (const unsigned char *) buf, len);

    //  Rename file
    fs_ops.rename(file_path, new_file);

    //  Initialize read variables
    memset(buf, '\0', sizeof(char) * len);

    //  Call read
    fs_ops.read(new_file, buf, len, offset, fi);

    //  Check sum
    unsigned cksum2 = crc32(0, (const unsigned char *) buf, len);
    if(DEBUG == 1)
        printf("\t%s -> %s : Calculated: %u  Expected: %u\n", file_path, new_file, cksum2, cksum);
    ck_assert_int_eq(cksum2, cksum);

    //  Free memory
    free(buf);
    free(sb1);

    //  Rename a dir
    //  Get attributes
    char *dir_path = "/dir3/subdir/file.12k";
    char *new_dir = "/dir3/new_dir/file.12k";
    fs_ops.getattr(dir_path, sb2);
    len = sb2->st_size;
    char *buf2 = malloc(sizeof(char) * len);

    //  Initialize read variables
    memset(buf2, '\0', sizeof(char) * len);
    offset = 0;

    //  Call read
    fs_ops.read(dir_path, buf2, len, offset, fi);

    //  Check sum
    cksum = crc32(0, (const unsigned char *) buf2, len);

    //  Rename file
    fs_ops.rename("/dir3/subdir", "/dir3/new_dir");

    //  Initialize read variables
    memset(buf2, '\0', sizeof(char) * len);

    //  Call read
    fs_ops.read(new_dir, buf2, len, offset, fi);

    //  Check sum
    cksum2 = crc32(0, (const unsigned char *) buf2, len);
    if(DEBUG == 1)
        printf("\t%s -> %s : Calculated: %u  Expected: %u\n\n", dir_path, new_dir, cksum2, cksum);
    ck_assert_int_eq(cksum2, cksum);

    //  Free memory
    free(fi);
    free(buf2);
    free(sb2);
}
END_TEST

/* note that your tests will call:
 *  fs_ops.getattr(path, struct stat *sb)
 *  fs_ops.readdir(path, NULL, filler_function, 0, NULL)
 *  fs_ops.read(path, buf, len, offset, NULL);
 *  fs_ops.statfs(path, struct statvfs *sv);
 */

int main(int argc, char **argv){
    //  Reset test image
    system("python gen-disk.py -q disk1.in test.img");

    //  Initialize testing
    block_init("test.img");
    fs_ops.init(NULL);

    //  Create testing suit
    Suite *s = suite_create("CS5600 PA3 Test Suite");

    //  Create testing cases
    TCase *tc1 = tcase_create("\tfs_getattr: results\t");
    TCase *tc2 = tcase_create("\tfs_getattr: errors\t");
    TCase *tc3 = tcase_create("\tfs_readdir: results\t");
    TCase *tc4 = tcase_create("\tfs_readdir: errors\t");
    TCase *tc5 = tcase_create("\tfs_read: big read\t");
    TCase *tc6 = tcase_create("\tfs_read: err read\t");
    TCase *tc7 = tcase_create("\tfs_read: small reads\t");
    TCase *tc8 = tcase_create("\tfs_statvfs\t\t");
    TCase *tc9 = tcase_create("\tfs_chmod\t\t");
    TCase *tc10 = tcase_create("\tfs_rename\t\t");

    //  Add tests
    tcase_add_test(tc1, a_test);
    tcase_add_test(tc2, b_test);
    tcase_add_test(tc3, c_test);
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
    suite_add_tcase(s, tc3);
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

    //  Free test from memory
    srunner_free(sr);

    //  Return
    return (n_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
