#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file-cow.h"

int make_test_file() {
    system("dd if=/dev/random of=/tmp/cow_test bs=1M count=1 2> /dev/null");
}

int remove_test_file() {
    system("rm /tmp/cow_test");
}

int run(void* test) {
    make_test_file();
    
    int (*test_func)() = test;
    int result = test_func();

    remove_test_file();
    result ? printf("failed %d\n", result) : printf("passed\n");
    return result;
}

int test_cow_simple() {
    printf("test_cow_simple... ");
    
    cow_file *cow = cow_open("/tmp/cow_test");
    char* buf = malloc(1024);
    char* buf2 = malloc(1024);

    cow_read(cow, buf, 0, 1024);
    cow_write(cow, "hello", 0, 5);
    cow_read(cow, buf2, 0, 1024);

    if(memcmp(buf, buf2, 1024) == 0) return 1;
    if(memcmp(buf2, "hello", 5) != 0) return 2;

    cow_close(cow);
    free(buf); free(buf2);
    return 0;
}

int test_cow_open_failure() {
    printf("test_cow_open_failure... ");

    cow_file *cow = cow_open("/tmp/cow_test_nonexistent");
    if(cow != NULL) return 1;

    return 0;
}

int test_cow_multiple() {
    printf("test_cow_multiple... ");

    cow_file *cow = cow_open("/tmp/cow_test");
    char* buf = malloc(1024);
    char* buf2 = malloc(1024);

    cow_read(cow, buf, 0, 1024);
    cow_write(cow, "hello", 0, 5);
    cow_write(cow, "world", 5, 5);
    cow_read(cow, buf2, 0, 1024);

    if(memcmp(buf, buf2, 1024) == 0) return 1;
    if(memcmp(buf2, "helloworld", 10) != 0) return 2;

    cow_close(cow);
    free(buf); free(buf2);
    return 0;
}

int test_cow_multiple_non_adjacent() {
    printf("test_cow_multiple_non_adjacent... ");

    cow_file *cow = cow_open("/tmp/cow_test");
    char* buf = malloc(1024);
    char* buf2 = malloc(1024);

    cow_read(cow, buf, 0, 1024);
    cow_write(cow, "hello", 0, 5);
    cow_write(cow, "world", 10, 5);
    cow_read(cow, buf2, 0, 1024);

    if(memcmp(buf, buf2, 1024) == 0) return 1;
    if(memcmp(buf2, "hello", 5) != 0) return 2;
    if(memcmp(buf2 + 10, "world", 5) != 0) return 3;

    cow_close(cow);
    free(buf); free(buf2);
    return 0;
}

int test_cow_size() {
    printf("test_cow_size... ");

    cow_file *cow = cow_open("/tmp/cow_test");
    
    if(cow_size(cow) != 1024 * 1024) return 1;

    cow_close(cow);
    return 0;
}

int test_cow_no_persist() {
    printf("test_cow_no_persist... ");

    cow_file *cow = cow_open("/tmp/cow_test");

    cow_write(cow, "hello", 0, 5);
    cow_commit(cow);
    cow_close(cow);

    cow = cow_open("/tmp/cow_test");
    char* buf = malloc(5);
    cow_read(cow, buf, 0, 5);

    if(memcmp(buf, "hello", 5) == 0) return 1;

    cow_close(cow);
    free(buf);
    return 0;
}

int test_cow_commit() {
    printf("test_cow_commit... ");

    cow_file *cow = cow_open("/tmp/cow_test");

    cow_write(cow, "hello", 0, 5);
    cow_unprotect(cow);
    cow_commit(cow);
    cow_close(cow);

    cow = cow_open("/tmp/cow_test");
    char* buf = malloc(5);
    cow_read(cow, buf, 0, 5);

    if(memcmp(buf, "hello", 5) != 0) return 1;

    cow_close(cow);
    free(buf);
    return 0;
}

int main() {
    return
        run(test_cow_simple) ||
        run(test_cow_open_failure) ||
        run(test_cow_multiple) ||
        run(test_cow_multiple_non_adjacent) ||
        run(test_cow_size) ||
        run(test_cow_no_persist) ||
        run(test_cow_commit);
}