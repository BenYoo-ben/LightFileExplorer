#include "gtest/gtest.h"

#include <vector>
#include <sstream>
#include <string>
#include <iostream>

#include "file_scouter.hpp"

TEST (FILE_MANAGER, FILES_IN_DIRECTORY_TO_VECTOR){
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
    ASSERT_EQ(fileList.size(), 3);

    fileList.clear();

    // test for empty directory

    ret = fm.files_in_directory_to_vector("./unittest/testEmptyDirectory", &fileList);
    ASSERT_EQ(ret, 0);

    ASSERT_EQ(fileList.size(), 0);

    fileList.clear();

    ret = fm.files_in_directory_to_vector("./unittest/testNonExistingDirectory____", &fileList);
    ASSERT_EQ(ret, -1);

    ASSERT_EQ(fileList.size(), 0);


}




