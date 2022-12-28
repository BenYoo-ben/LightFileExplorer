#include "gtest/gtest.h"

#include <vector>
#include <sstream>
#include <string>
#include <iostream>

#include "file_scouter.hpp"
#include "nets.hpp"

TEST (FILE_MANAGER, FILES_IN_DIRECTORY_TO_VECTOR) {
    // using testDirectory, which has empty file 1, 2, 3
    file_manager fm;

    std::vector<std::string> fileList(0);

    FILE* fp;
    char cnt[PATH_MAX] = {0, };
    // get # of files in /tmp/
    fp = popen("/bin/ls /etc/ | wc -l", "r");
    ASSERT_NE(fp, nullptr);
    ASSERT_NE(fgets(cnt, sizeof(cnt), fp), nullptr);
    pclose(fp);

    int ret = fm.files_in_directory_to_vector("/etc", &fileList);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(fileList.size(), atoi(cnt) + 1);
    fileList.clear();

    // test for empty directory
    mkdir("./unittest/testEmptyDirectory", 0644);
    ret = fm.files_in_directory_to_vector("./unittest/testEmptyDirectory", &fileList);
    ASSERT_EQ(ret, 0);
    rmdir("./unittest/testEmptyDirectory");

    ASSERT_EQ(fileList.size(), 0);
    fileList.clear();

    ret = fm.files_in_directory_to_vector("./unittest/testNonExistingDirectory____", &fileList);
    ASSERT_EQ(ret, -1);
    ASSERT_EQ(fileList.size(), 0);
}

TEST (FILE_MANAGER, GET_STAT_OF_FILE) {
    file_manager fm;
    struct stat st;
    int ret = fm.get_stat_of_file("./unittest/srcs/file_scouter_unittest.cpp", &st);

    ASSERT_EQ(ret, 0);

    ret = fm.get_stat_of_file("./unittest/00()00", &st);

    ASSERT_EQ(ret < 0, true);
}

TEST (FILE_MANAGER, STAT_GET_TYPE) {
    struct stat st;

    file_manager fm;
    
    fm.get_stat_of_file("/dev", &st);
    ASSERT_EQ(fm.stat_get_type(&st), "Directory");
    st = {0};

    fm.get_stat_of_file("./unittest/srcs/file_scouter_unittest.cpp", &st);
    ASSERT_EQ(fm.stat_get_type(&st), "File");
    st = {0};

    fm.get_stat_of_file("/dev/null", &st);
    ASSERT_EQ(fm.stat_get_type(&st), "Char Device");
    st = {0};

    fm.get_stat_of_file("", &st);
    ASSERT_EQ(fm.stat_get_type(&st), "");
    st = {0};
}

TEST (FILE_MANAGER, STAT_IS_DIRECTORY) {
    struct stat st;
    file_manager fm;

    ASSERT_EQ(fm.get_stat_of_file("./unittest/", &st), 0);
    ASSERT_EQ(fm.stat_is_directory(&st), true);

    ASSERT_EQ(fm.get_stat_of_file("./unittest/srcs/file_scouter_unittest.cpp", &st), 0);
    ASSERT_EQ(fm.stat_is_directory(&st), false);
}

TEST (FILE_MANAGER, STAT_GET_TIME) {
    struct stat st;
    file_manager fm;
    
    ASSERT_EQ(system("touch ./unittest/testTouch"), 0);
    
    ASSERT_EQ(fm.get_stat_of_file("./unittest/testTouch", &st), 0);
    ASSERT_EQ(fm.stat_get_time(&st).length(), 16);

    remove("./unittest/testTouch");
}

TEST (FILE_MANAGER, STAT_GET_SIZE) {
    struct stat st;
    file_manager fm;
    
    ASSERT_EQ(system("fallocate -l 1024 ./unittest/testSize"), 0);

    ASSERT_EQ(fm.get_stat_of_file("./unittest/testSize", &st), 0);
    ASSERT_EQ(fm.stat_get_size(&st), 1024);

    ASSERT_EQ(system("fallocate -l 2048 ./unittest/testSize2"), 0);
    ASSERT_EQ(fm.get_stat_of_file("./unittest/testSize2", &st), 0);
    ASSERT_EQ(fm.stat_get_size(&st), 2048);

    remove("./unittest/testSize");
    remove("./unittest/testSize2");

    ASSERT_EQ(fm.get_stat_of_file("./unittest/testSize", &st), -1);
}

TEST (FILE_MANAGER, STAT_GET_PERMISSION) {
    struct stat st;
    file_manager fm;
    
    ASSERT_EQ(system("touch ./unittest/permTest"), 0);
    chmod("./unittest/permTest", 0777);

    ASSERT_EQ(fm.get_stat_of_file("./unittest/permTest", &st), 0);
    ASSERT_EQ(fm.stat_get_permission(&st), "rwxrwxrwx");

    remove("./unittest/permTest");
}




