#include <sys/types.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

#include <thread>

#include "gtest/gtest.h"
#include "nets.hpp"

/*********************
Run with root privileges
**********************/

TEST (NETS, SERVER_SOCKET_INIT_TEST) {
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
    srand((unsigned int)time(NULL));
    int randVal = rand() % 25000;

    int testPort = 20000 + randVal;

    server_object sObjBind;
    EXPECT_EQ(sObjBind.server_socket_init(), 0);

    EXPECT_EQ(sObjBind.server_socket_bind(testPort), 0);

    EXPECT_EQ(sObjBind.server_socket_bind(testPort) < 0, true);

    sObjBind.server_socket_close();

    EXPECT_EQ(sObjBind.server_socket_bind(testPort) < 0, true);
}

TEST (NETS, SERVER_SOCKET_LISTEN_TEST) {
    server_object successObj, badFdObj1, badFdObj2;

    srand((unsigned int)time(NULL));
    int randVal = rand() % 25000;

    int testPort = 20000 + randVal;
    EXPECT_EQ(successObj.server_socket_init(), 0);
    EXPECT_EQ(successObj.server_socket_bind(testPort), 0);
    EXPECT_EQ(successObj.server_socket_listen(), 0);

    EXPECT_EQ(badFdObj1.server_socket_listen() < 0, true);

    EXPECT_EQ(badFdObj2.server_socket_init(), 0);
    EXPECT_EQ(badFdObj2.server_socket_bind(testPort) < 0, true);
    EXPECT_EQ(badFdObj2.server_socket_listen() < 0, true);
}

    
TEST (NETS, SERVER_SOCKET_START) {
    uint16_t randPort = rand() % 25000 + 20000;
    
    // prepare socket for test;
    int c_sock1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in dest_address;
    memset(&dest_address, 0x00 , sizeof(struct sockaddr_in));

    int c_sock2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    dest_address.sin_family = AF_INET;
    dest_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    dest_address.sin_port = htons(randPort);

    int retServer = 0;
    server_object* server = new server_object();
    std::thread t1([randPort, &server, &retServer](){
        EXPECT_EQ(server->server_socket_init(), 0);
        EXPECT_EQ(server->server_socket_bind(randPort), 0);
        EXPECT_EQ(server->server_socket_listen(), 0);

        retServer = server->server_socket_start();
    });

    // wait til server is initialized;
    sleep(2);

    // should be successful
    ASSERT_EQ(retServer, 0);
    EXPECT_EQ(connect(c_sock1, (struct sockaddr *)&dest_address, sizeof(dest_address)) >= 0 , true);
    ASSERT_EQ(retServer, 0);
   
    // should fail since listening socket is closed
    EXPECT_EQ(server->server_socket_close(), 0);
    EXPECT_EQ(connect(c_sock2, (struct sockaddr *)&dest_address, sizeof(dest_address)) < 0 , true);
    t1.join();

    ASSERT_EQ(retServer < 0, true);
}

TEST (NETS, SERVER_SOCKET_CLOSE) {
    server_object server;
    uint16_t randPort = rand() % 25000 + 20000;
    EXPECT_EQ(server.server_socket_init(), 0);
    EXPECT_EQ(server.server_socket_bind(randPort), 0);
    EXPECT_EQ(server.server_socket_listen(), 0);
    ASSERT_EQ(server.server_socket_close(), 0);
}
