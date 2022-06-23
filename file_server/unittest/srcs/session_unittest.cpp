#include <unistd.h>

#include <vector>
#include <sstream>
#include <string>
#include <iostream>

#include "nets.hpp"
#include "objects.hpp"
#include "session.hpp"
#include "gtest/gtest.h"

TEST (SESSION, RUN) {
    srand(time(NULL));

    uint16_t randPort = rand() % 25000 + 20000;

    char buffer[global_expected_MTU];

    int c_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serverInfo;

    memset(&serverInfo, 0x00, sizeof(serverInfo));

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverInfo.sin_port = htons(randPort);

//    int ret = connect(c_sock, (struct sockaddr*)&serverInfo, sizeof(serverInfo));
//  TO-DO: Finish Session Test
    server_object* so = new server_object();
    ASSERT_EQ(so->server_socket_init() < 0, false);
    ASSERT_EQ(so->server_socket_bind(randPort) < 0, false);
    ASSERT_EQ(so->server_socket_listen() < 0, false);

    lock_handler lock = lock_handler();
    delete(so);
}

