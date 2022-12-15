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

    std::vector<std::string> fileList;
    fileList.clear();

    int ret = fm.files_in_directory_to_vector("./unittest/testDirectory", &fileList);

    ASSERT_EQ(ret, 0);
    // should contain 1, 2, 3;

    std::stringstream ss;
    for (std::string i : fileList) {
        ss << i;
    }
    std::string str = ss.str();

    bool check123 = true;
    if (str.find("1") == std::string::npos) {
        check123 = false;
    }

    if (str.find("2") == std::string::npos) {
        check123 = false;
    }
    
    if (str.find("3") == std::string::npos) {
        check123 = false;
    }
    ASSERT_EQ(check123, true);
    ASSERT_EQ(fileList.size(), 4);

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
    int ret = fm.get_stat_of_file("./unittest/testDirectory/1", &st);

    ASSERT_EQ(ret, 0);

    ret = fm.get_stat_of_file("./unittest/testEmptyDirectory/0", &st);

    ASSERT_EQ(ret < 0, true);
}

TEST (FILE_MANAGER, STAT_GET_TYPE) {
    struct stat st;

    file_manager fm;
    
    fm.get_stat_of_file("/dev", &st);
    ASSERT_EQ(fm.stat_get_type(&st), "Directory");

    fm.get_stat_of_file("./unittest/testDirectory/1", &st);
    ASSERT_EQ(fm.stat_get_type(&st), "File");

    fm.get_stat_of_file("/dev/null", &st);
    ASSERT_EQ(fm.stat_get_type(&st), "Char Device");
}

TEST (FILE_MANAGER, STAT_IS_DIRECTORY) {
    struct stat st;
    file_manager fm;

    ASSERT_EQ(fm.get_stat_of_file("./unittest/testDirectory", &st), 0);
    ASSERT_EQ(fm.stat_is_directory(&st), true);

    ASSERT_EQ(fm.get_stat_of_file("./unittest/testDirectory/1", &st), 0);
    ASSERT_EQ(fm.stat_is_directory(&st), false);
}

TEST (FILE_MANAGER, STAT_GET_TIME) {
    struct stat st;
    file_manager fm;

    ASSERT_EQ(fm.get_stat_of_file("./unittest/testDirectory/1", &st), 0);
    ASSERT_EQ(fm.stat_get_time(&st).length(), 16);
}

TEST (FILE_MANAGER, STAT_GET_SIZE) {
    struct stat st;
    file_manager fm;

    ASSERT_EQ(fm.get_stat_of_file("./unittest/testDirectory/1", &st), 0);
    ASSERT_EQ(fm.stat_get_size(&st), 512);

    ASSERT_EQ(fm.get_stat_of_file("./unittest/testDirectory/2", &st), 0);
    ASSERT_EQ(fm.stat_get_size(&st), 1024);
}

TEST (FILE_MANAGER, STAT_GET_PERMISSION) {
    struct stat st;
    file_manager fm;

    chmod("./unittest/testDirectory/3", 0777);

    ASSERT_EQ(fm.get_stat_of_file("./unittest/testDirectory/3", &st), 0);
    ASSERT_EQ(fm.stat_get_permission(&st), "rwxrwxrwx");
}




