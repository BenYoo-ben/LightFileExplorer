cmake_minimum_required(VERSION 3.15)

set(cmake_verbose_makefile ON)

project (
    LightFileExplorer
    VERSION 1.0
    DESCRIPTION "Portable File Server"
    LANGUAGES CXX
)

file (GLOB fs_server_SRC
    "./srcs/*cpp"
)

file (GLOB fs_server_lib_SRC
    "./srcs/*.cpp"
)

file (GLOB fs_server_test_SRC
    "./unittest/srcs/*.cpp"
)

list(FILTER fs_server_lib_SRC EXCLUDE REGEX "main\.cpp")

find_library(LIB_JSONCPP jsoncpp)
find_library(LIB_GTEST gtest)
find_library(LIB_PTHREAD pthread)

if (NOT LIB_JSONCPP)
    message (FATAL_ERROR "jsoncpp library not found")
endif()

if (NOT LIB_GTEST)
    message (FATAL_ERROR "gtest library not found")
endif()

if (NOT LIB_PTHREAD)
    message (FATAL_ERROR "pthread library not found")
endif()

add_library (fs_server_lib STATIC ${fs_server_lib_SRC})

add_executable(fs_server ${fs_server_SRC})
add_executable(fs_server_test ${fs_server_test_SRC})

target_include_directories(fs_server PUBLIC includes)
target_link_libraries(fs_server PUBLIC pthread jsoncpp)

target_compile_options(fs_server_lib PUBLIC -g -fprofile-arcs -ftest-coverage -Wall -Wno-psabi)
target_include_directories(fs_server_lib PUBLIC includes)
target_link_libraries(fs_server_lib PUBLIC pthread jsoncpp gcov)

target_include_directories(fs_server_test PUBLIC includes unittest/includes)
target_link_libraries(fs_server_test PUBLIC fs_server_lib gtest gcov pthread jsoncpp libgtest_main.a libgtest.a)
