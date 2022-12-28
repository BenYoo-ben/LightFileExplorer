#ifndef SERVER_INCLUDES_SESSION_HPP_
#define SERVER_INCLUDES_SESSION_HPP_

#include <pthread.h>
#include <unistd.h>

#include <string>

#include "common.hpp"
#include "global.hpp"
#include "file_scouter.hpp"
#include "objects.hpp"

class session_object{
private:
    std::string ID;
    int c_sock;

    pthread_t session_thread;
    lock_handler *lock;

public:
    session_object(int established_socket, lock_handler *lock);
    int handle_request(char type, std::string dir, std::string data,
            int client_socket);
    void close_socket();
    void *run();
};

#endif  // SERVER_INCLUDES_SESSION_HPP_
