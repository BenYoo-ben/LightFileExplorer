#ifndef SERVER_INCLUDES_SESSION_HPP_
#define SERVER_INCLUDES_SESSION_HPP_

#include <pthread.h>
#include <unistd.h>

#include <string>
#include <stack>

#include "common.hpp"
#include "global.hpp"
#include "file_scouter.hpp"
#include "objects.hpp"

class session_object{
private:
    std::string ID;
    int c_sock;

    pthread_t session_thread;

public:
    session_object() = default;
    session_object(int established_socket);

    session_object(const session_object& other) {
        this->ID = other.ID;
        this->c_sock = other.c_sock;
    }

    session_object& operator=(const session_object& other) {
        if (this != &other) {
            this->ID = other.ID;
            this->c_sock = other.c_sock;
        }
        return *this;
    }

    session_object(session_object&&) = default;
    session_object& operator=(session_object&&) = default;

    ~session_object() 
    {
        close_socket();
    }

    int handle_request(char type, std::string dir, std::string data);

    void close_socket();
    void *run();

    int handleDownload(session_lock& s_locks, std::string dir, std::string data);
    int handleCopy(session_lock& s_locks, std::string dir, std::string data);
    int handleMove(session_lock& s_locks, std::string dir, std::string data);
    int handleDelete(session_lock& s_locks, std::string dir, std::string data);
    int handleRename(session_lock& s_locks, std::string dir, std::string data);
    int handleDirInfoD1(session_lock& s_locks, std::string dir, std::string data);
    int handleUpload(session_lock& s_locks, std::string dir, std::string data);

};

#endif  // SERVER_INCLUDES_SESSION_HPP_
