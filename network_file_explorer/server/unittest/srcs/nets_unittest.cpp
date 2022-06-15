#include <sys/types.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>

#include "gtest/gtest.h"
#include "nets.hpp"

/*********************

Run with root privileges
**********************/

TEST (NETS, SERVER_SOCKET_INIT_TEST) {
    // supress stderr
    EXPECT_EQ(freopen("/dev/null", "w+", stderr) != NULL, true);

    struct rlimit rlim;
    getrlimit(RLIMIT_NOFILE, &rlim);

    // at least 1000 files can be handled
    EXPECT_EQ(rlim.rlim_max > 1000, true);

    server_object successObj;
   
    // no error should exist
    EXPECT_EQ(successObj.server_socket_init(), 0);

    // printf("Open File : %d / %d \n", rlim.rlim_cur, rlim.rlim_max);


    // adjust file open limit
    rlim.rlim_cur = 1;
    rlim.rlim_max = 1;

    server_object failObj;
    EXPECT_EQ(setrlimit(RLIMIT_NOFILE, &rlim) >= 0 , true);
    // printf("Open File : %d / %d \n", rlim.rlim_cur, rlim.rlim_max);

    // call should fail; since fd limit is reached
    EXPECT_EQ(failObj.server_socket_init() < 0, true);

    rlim.rlim_cur = 1000;
    rlim.rlim_max = 1000;
    EXPECT_EQ(setrlimit(RLIMIT_NOFILE, &rlim) >= 0, true);
}

TEST (NETS, SERVER_SOCEKT_BIND_TEST) {
    EXPECT_EQ(freopen("/dev/null", "w+", stderr) != NULL, true);


    srand((unsigned int)time(NULL));
    int randVal = rand() % 25000;

    int testPort = 20000 + randVal;

    
    server_object sObjBind;
    EXPECT_EQ(sObjBind.server_socket_init(), 0);
    perror(":");

    EXPECT_EQ(sObjBind.server_socket_bind(testPort), 0);

    EXPECT_EQ(sObjBind.server_socket_bind(testPort), -1);
}

TEST (NETS, SERVER_SOCKET_LISTEN_TEST) {
    EXPECT_EQ(freopen("/dev/null", "w+", stderr) != NULL, true);

    server_object successObj, badFdObj;

    srand((unsigned int)time(NULL));
    int randVal = rand() % 25000;

    int testPort = 20000 + randVal;
    EXPECT_EQ(successObj.server_socket_init(), 0);
    EXPECT_EQ(successObj.server_socket_bind(testPort), 0);
    EXPECT_EQ(successObj.server_socket_listen(), 0);

    EXPECT_EQ(badFdObj.server_socket_listen() < 0, true);
}
