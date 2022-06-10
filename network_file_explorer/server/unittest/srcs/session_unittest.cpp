#include <unistd.h>

#include <vector>
#include <sstream>
#include <string>
#include <iostream>

#include "nets.hpp"
#include "objects.hpp"
#include "session.hpp"
#include "gtest/gtest.h"

#define BUF_MAX 2048
TEST (SESSION, MAIN_OPERATION) {
    lock_handler lock = lock_handler();
    
    int pSocket = open("./UNITTEST_SESSION_MAIN_PSEUDO_SOCKET", O_CREAT | O_RDWR);

    char buf[50]; int idx = 0;
    buf[0] = 0x06; idx += 1;
    uint32_t size = 2;
    memcpy(buf + idx, &size, sizeof(uint32_t)); idx += sizeof(uint32_t);
    buf[idx] = '.'; buf[idx + 1] = '/';
    idx += size;

    size = 0;
    memcpy(buf + idx, &size, sizeof(uint32_t));
    

    write(pSocket, buf, sizeof(buf));
    int out_pipe[2];
    int err_pipe[2];
    int saved_stdout, saved_stderr;
    char _stdout[BUF_MAX + 1];
    char _stderr[BUF_MAX + 1];

    saved_stdout = dup(STDOUT_FILENO);
    saved_stderr = dup(STDERR_FILENO);

    int ret;
    if ( (ret = pipe(out_pipe)) != 0) {
        perror("pipe failed");
        ASSERT_EQ(ret, 0);
    }

    if ((ret = pipe(err_pipe)) != 0) {
        perror("pipe failed 2");
        ASSERT_EQ(ret, 0);
    }

    dup2(out_pipe[1], STDOUT_FILENO);
    close(out_pipe[1]);

    dup2(err_pipe[1], STDERR_FILENO);
    close(err_pipe[1]);

    lseek(pSocket, 0, SEEK_SET);
    new session_object(pSocket, &lock);

    ASSERT_TRUE(read(out_pipe[0], _stdout, BUF_MAX) > 0);
    ASSERT_TRUE(read(err_pipe[0], _stderr, BUF_MAX) > 0);

//    close(pSocket);
    dup2(saved_stdout, STDOUT_FILENO);
    printf("STDOUT read :[%s]\n", _stdout);

    dup2(saved_stderr, STDERR_FILENO);
    printf("STDERR read : [%s]\n", _stderr);
}

