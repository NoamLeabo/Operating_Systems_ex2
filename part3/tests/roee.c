#include "buffered_open.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

const char *filename = "extanded_tests.txt";

int test1(){
    // ============================== TEST 1: Simple write to file ==================================
    const char *inputTest1 = "Test1";
    char readBuffer[1024] = {0};
    const char *expectedOutTest1 = "Test1";
    // Open file for writing
    buffered_file_t *bf = buffered_open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }

    // Write text to the file
    if (buffered_write(bf, inputTest1, strlen(inputTest1)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }

    // Flush the buffer
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }

    // Close the buffered file
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }

    // Reopen file for reading with standard I/O to verify contents
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    ssize_t bytes_read = read(fd, readBuffer, sizeof(readBuffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    readBuffer[bytes_read] = '\0';  // Null-terminate the string

    if (strcmp(readBuffer, expectedOutTest1) == 0) {
        printf("\033[0;32mTEST 1: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 1: FAILED\n\033[0m");
        return -1;
    }
    // ============================== END OF TEST 1: Simple write to file ==================================
}

int test2(){
    // ============================= TEST 2: Write to file with preappend flag =============================
    const char *inputTest2 = "Test2";
    char readBuffer[1024] = {0};
    const char *expectedOutTest2 = "Test2Test1";
    // Open file with preappend flag
    buffered_file_t *bf = buffered_open(filename, O_RDWR | O_PREAPPEND, 0);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }
    if (buffered_write(bf, inputTest2, strlen(inputTest2)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }
    // Close the buffered file
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }
    // Reopen file for reading with standard I/O to verify contents
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    ssize_t bytes_read = read(fd, readBuffer, sizeof(readBuffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    readBuffer[bytes_read] = '\0';  // Null-terminate the string

    if (strcmp(readBuffer, expectedOutTest2) == 0) {
        printf("\033[0;32mTEST 2: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 2: FAILED\n\033[0m");
        return -1;
    }
    // ============================= END OF TEST 2: Write to file with preappend flag =============================
}
int test3(){
    // ============================= TEST 3: Write witout preappend with appened flag ==============================
    const char *inputTest3 = "Test3";
    char readBuffer[1024] = {0};
    const char *expectedOutTest3 = "Test2Test1Test3";
    // Open file without preappend flag
    buffered_file_t *bf = buffered_open(filename, O_RDWR | O_APPEND, 0);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }
    if (buffered_write(bf, inputTest3, strlen(inputTest3)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }
    // Close the buffered file
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }
    // Reopen file for reading with standard I/O to verify contents
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    ssize_t bytes_read = read(fd, readBuffer, sizeof(readBuffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    readBuffer[bytes_read] = '\0';  // Null-terminate the string

    if (strcmp(readBuffer, expectedOutTest3) == 0) {
        printf("\033[0;32mTEST 3: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 3: FAILED\n\033[0m");
        return -1;
    }
    // ====================== END OF TEST 3: Write witout preappend with appened flag =====================
}

int test4(){
    // ============================= TEST 4: Read first 4 bytes of file ====================================
    char readBuffer_file[1024] = {0};
    char readBuffer[1024] = {0};
    const char *expectedOutTest4_file = "Test2Test1Test3";
    const char *expectedOutTest4 = "Test";
    // Open file without preappend flag
    buffered_file_t *bf = buffered_open(filename, O_RDWR, 0);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }
    ssize_t bytes_read;
    bytes_read = buffered_read(bf, readBuffer, 4);
    if (bytes_read == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }

    // ======== TEST Empty flush ==========
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }
    // Reopen file for reading with standard I/O to verify contents
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    ssize_t bytes_read_file = read(fd, readBuffer_file, sizeof(readBuffer_file) - 1);
    if (bytes_read_file == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    readBuffer_file[bytes_read_file] = '\0';  // Null-terminate the string

    if (strcmp(readBuffer_file, expectedOutTest4_file) != 0) {
        printf("\033[0;31mTEST 4: FAILED\n\033[0m");
        return -1;
    }
    // Close the buffered file
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }

    readBuffer[bytes_read] = '\0';  // Null-terminate the string

    if (strcmp(readBuffer, expectedOutTest4) == 0) {
        printf("\033[0;32mTEST 4: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 4: FAILED\n\033[0m");
        return -1;
    }
    // ============================= END OF TEST 4: Read first 4 bytes of file ====================================
}
int test5(){
    // =========================== TEST 5: Sequential Write to file with preappend flag ===========================
    const char *inputTest5 = "Test5";
    char readBuffer[1024] = {0};
    const char *expectedOutTest5_1 = "Test5Test5Test5Test2Test1Test3";
    // Open file with preappend flag
    buffered_file_t *bf = buffered_open(filename, O_RDWR | O_PREAPPEND, 0);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }
    if (buffered_write(bf, inputTest5, strlen(inputTest5)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest5, strlen(inputTest5)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest5, strlen(inputTest5)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }
    // Close the buffered file
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }
    // Reopen file for reading with standard I/O to verify contents
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    ssize_t bytes_read = read(fd, readBuffer, sizeof(readBuffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    readBuffer[bytes_read] = '\0';  // Null-terminate the string

    if (strcmp(readBuffer, expectedOutTest5_1) != 0) {
        printf("\033[0;31mTEST 5: FAILED\n\033[0m");
        return -1;
    }
    const char *expectedOutTest5_2 = "Test5Test5Test5Test2Test1Test3Test5Test5Test5Test5";
    bf = buffered_open(filename, O_RDWR | O_APPEND, 0);
    if (buffered_write(bf, inputTest5, strlen(inputTest5)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest5, strlen(inputTest5)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest5, strlen(inputTest5)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest5, strlen(inputTest5)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }
    // Close the buffered file
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }
    // Reopen file for reading with standard I/O to verify contents
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    bytes_read = read(fd, readBuffer, sizeof(readBuffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    readBuffer[bytes_read] = '\0';  // Null-terminate the string

    if (strcmp(readBuffer, expectedOutTest5_2) == 0) {
        printf("\033[0;32mTEST 5: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 5: FAILED\n\033[0m");
        return -1;
    }
    // ======================= END OF TEST 5: Sequential Write to file with preappend flag ========================
}

int test6(){
    // ============================= TEST 6: Sequential Read to file ==============================================
    char readBuffer[1024] = {0};
    const char *expectedOutTest6 = "Test5Test5Test5Test2";
    buffered_file_t *bf = buffered_open(filename, O_RDWR | O_APPEND, 0);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }
    ssize_t bytes_read;
    bytes_read = buffered_read(bf, readBuffer, 5);
    if (bytes_read == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    bytes_read = buffered_read(bf, readBuffer + 5, 5);
    if (bytes_read == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    bytes_read = buffered_read(bf, readBuffer + 10, 5);
    if (bytes_read == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    bytes_read = buffered_read(bf, readBuffer + 15, 5);
    if (bytes_read == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }
    // Close the buffered file
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }
    readBuffer[20] = '\0';  // Null-terminate the string
    if (strcmp(readBuffer, expectedOutTest6) == 0) {
        printf("\033[0;32mTEST 6: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 6: FAILED\n\033[0m");
        return -1;
    }
    // ============================= END OF TEST 6: Sequential Read to file ==============================================
}

int test7(){
    // ============================= TEST 7: Read than write - preappend ==============================================
    char readBuffer[1024] = {0};
    const char *inputTest7 = "Test7";
    const char *expectedOutTest7_2 = "Test5Test7Test5Test7Test5Test7Test2Test1Test3Test5Test5Test5Test5";
    const char *expectedOutTest7_1 = "Test5Test5Test5";
    buffered_file_t *bf = buffered_open(filename, O_RDWR | O_PREAPPEND, 0);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }
    ssize_t bytes_read;
    bytes_read = buffered_read(bf, readBuffer, 5);
    if (bytes_read == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest7, strlen(inputTest7)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    bytes_read = buffered_read(bf, readBuffer + 5, 5);
    if (bytes_read == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest7, strlen(inputTest7)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    bytes_read = buffered_read(bf, readBuffer + 10, 5);
    if (bytes_read == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest7, strlen(inputTest7)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }
    readBuffer[20] = '\0';  // Null-terminate the string
    if (strcmp(readBuffer, expectedOutTest7_1) != 0) {
        printf("\033[0;31mTEST 7: FAILED\n\033[0m");
        return -1;
    }
    // Reopen file for reading with standard I/O to verify contents
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    bytes_read = read(fd, readBuffer, sizeof(readBuffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    readBuffer[bytes_read] = '\0';  // Null-terminate the string

    if (strcmp(readBuffer, expectedOutTest7_2) == 0) {
        printf("\033[0;32mTEST 7: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 7: FAILED\n\033[0m");
        return -1;
    }
    // ============================= END OF TEST 7: Read than write - preappend =======================================
}

int test8(){
    // ============================= TEST 8: Read than write - no preappend ===========================================
    char readBuffer[1024] = {0};
    const char *inputTest8 = "Test8";
    const char *expectedOutTest8_2 = "Test5Test7Test5Test7Test5Test7Test2Test1Test3Test5Test5Test5Test5Test8Test8";
    const char *expectedOutTest8_1 = "Test5";
    buffered_file_t *bf = buffered_open(filename, O_RDWR | O_APPEND, 0);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }
    ssize_t bytes_read;
    bytes_read = buffered_read(bf, readBuffer, 5);
    if (bytes_read == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest8, strlen(inputTest8)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    bytes_read = buffered_read(bf, readBuffer + 5, 5);
    if (bytes_read == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest8, strlen(inputTest8)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if(buffered_flush(bf) == -1){
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }
    readBuffer[10] = '\0';
    if (strcmp(readBuffer, expectedOutTest8_1) != 0) {
        printf("\033[0;31mTEST 8: FAILED\n\033[0m");
        return -1;
    }    // Reopen file for reading with standard I/O to verify contents
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    bytes_read = read(fd, readBuffer, sizeof(readBuffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    readBuffer[bytes_read] = '\0';  // Null-terminate the string

    if (strcmp(readBuffer, expectedOutTest8_2) == 0) {
        printf("\033[0;32mTEST 8: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 8: FAILED\n\033[0m");
        return -1;
    }
    // =========================== END OF TEST 8: Read than write - no preappend ==================================
}
int test9(){
    // ============================= TEST 9: Write than read - preappend ===========================================
    char readBuffer[1024] = {0};
    const char *inputTest9 = "Test9";
    const char *expectedOutTest9 = "Test5Test7";
    buffered_file_t *bf = buffered_open(filename, O_RDWR | O_PREAPPEND, 0);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }
    if (buffered_write(bf, inputTest9, strlen(inputTest9)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_read(bf, readBuffer, 5) == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest9, strlen(inputTest9)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if(buffered_read(bf, readBuffer + 5, 5) == -1){
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }
    readBuffer[10] = '\0';  // Null-terminate the string
    if (strcmp(readBuffer, expectedOutTest9) == 0) {
        printf("\033[0;32mTEST 9: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 9: FAILED\n\033[0m");
        return -1;
    }
    // =========================== END OF TEST 9: Write than read - preappend ==================================
}

int test10(){
    // ============================= TEST 10: Write than read - no preappend ===========================================
    char readBuffer[1024] = {0};
    const char *inputTest10 = "Test10";
    const char *expectedOutTest10 = "";
    buffered_file_t *bf = buffered_open(filename, O_RDWR | O_APPEND, 0);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }
    if (buffered_write(bf, inputTest10, strlen(inputTest10)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_read(bf, readBuffer, 5) == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest10, strlen(inputTest10)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if(buffered_read(bf, readBuffer + 5, 5) == -1){
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if(buffered_write(bf, inputTest10, strlen(inputTest10)) == -1){
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if(buffered_read(bf, readBuffer + 10, 5) == -1){
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }
    readBuffer[15] = '\0';  // Null-terminate the string
    if (strcmp(readBuffer, expectedOutTest10) == 0) {
        printf("\033[0;32mTEST 10: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 10: FAILED\n\033[0m");
        return -1;
    }
    // =========================== END OF TEST 10: Write than read - no preappend ===============================
}

int test11(){
    // ================= TEST 11: Sequential Override write - no preappened and no append =======================
    const char *inputTest11 = "Test11";
    char readBuffer[1024] = {0};
    const char *expectedOutTest11_1 = "Test11est5Test11Test11st5Test7Test5Test7Test2Test1Test3Test5Test5Test5Test5Test8Test8Test10Test10Test10";
    const char *expectedOutTest11_2 = "est5st5T";
    // Open file with preappend flag
    buffered_file_t *bf = buffered_open(filename, O_RDWR, 0);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }
    if (buffered_write(bf, inputTest11, strlen(inputTest11)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_read(bf, readBuffer, 4) == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest11, strlen(inputTest11)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_write(bf, inputTest11, strlen(inputTest11)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }
    if (buffered_read(bf, readBuffer + 4, 4) == -1) {
        perror("buffered_read");
        buffered_close(bf);
        return 1;
    }
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        buffered_close(bf);
        return 1;
    }
    // Close the buffered file
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }
    // Test that the read buffer is correct
    readBuffer[8] = '\0';  // Null-terminate the string
    if (strcmp(readBuffer, expectedOutTest11_2) != 0) {
        printf("\033[0;31mTEST 11: FAILED\n\033[0m");
        return -1;
    }
    // Reopen file for reading with standard I/O to verify contents
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    ssize_t bytes_read = read(fd, readBuffer, sizeof(readBuffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    readBuffer[bytes_read] = '\0';  // Null-terminate the string

    if (strcmp(readBuffer, expectedOutTest11_1) == 0) {
        printf("\033[0;32mTEST 11: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 11: FAILED\n\033[0m");
        return -1;
    }
    
    // ============== END OF TEST 11: Sequential Override write - no preappened and no append ===================
}

int test12(){
    // ============================= TEST 11: Final Test ========================================================
    char readBuffer[1024] = {0};
    const char *expectedOutTest12 = "Test11est5Test11Test11st5Test7Test5Test7Test2Test1Test3Test5Test5Test5Test5Test8Test8Test10Test10Test10";
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    size_t bytes_read = read(fd, readBuffer, sizeof(readBuffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    readBuffer[bytes_read] = '\0';  // Null-terminate the string

    if (strcmp(readBuffer, expectedOutTest12) == 0) {
        printf("\033[0;32mTEST 12: PASSED\n\033[0m");
        return 0;
    } else {
        printf("\033[0;31mTEST 12: FAILED\n\033[0m");
        return -1;
    }
    // =========================== END OF TEST 11: Final Test ===================================================
}

int main() {
    int countTestPassed = 0;
    if (test1() == 0){
        countTestPassed++;
    }
    if (test2() == 0){
        countTestPassed++;
    }
    if (test3() == 0){
        countTestPassed++;
    }
    if (test4() == 0){
        countTestPassed++;
    }
    if (test5() == 0){
        countTestPassed++;
    }
    if(test6() == 0){
        countTestPassed++;
    }
    if(test7() == 0){
        countTestPassed++;
    }
    if(test8() == 0){
        countTestPassed++;
    }
    if(test9() == 0){
        countTestPassed++;
    }
    if(test10() == 0){
        countTestPassed++;
    }
    if(test11() == 0){
        countTestPassed++;
    }
    if(test12() == 0){
        countTestPassed++;
    }
    if (countTestPassed == 12){
        printf("P");
    } else {
        printf("F");
    }
    return countTestPassed;
}