#ifndef SERVER_INCLUDES_SESSION_HPP_
#define SERVER_INCLUDES_SESSION_HPP_

#include <unistd.h>

#include <string>
#include <stack>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>

#include "common.hpp"
#include "global.hpp"
#include "file_scouter.hpp"
#include "objects.hpp"

class session_object{
private:
    std::string ID;
    int c_sock;

    pthread_t session_thread;

    std::shared_ptr<TSQueue<int>> sockQueue;
    std::shared_ptr<std::mutex> mt;
    std::shared_ptr<std::condition_variable> cv;

public:
    session_object() = default;
    session_object(std::shared_ptr<TSQueue<int>> sockQueue,
            std::shared_ptr<std::mutex> mt,
            std::shared_ptr<std::condition_variable> cv);

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
    void run();
    void doTask(int c_sock);

    int handleDownload(session_lock& s_locks, std::string dir, std::string data);
    int handleCopy(session_lock& s_locks, std::string dir, std::string data);
    int handleMove(session_lock& s_locks, std::string dir, std::string data);
    int handleDelete(session_lock& s_locks, std::string dir, std::string data);
    int handleRename(session_lock& s_locks, std::string dir, std::string data);
    int handleDirInfoD1(session_lock& s_locks, std::string dir, std::string data);
    int handleUpload(session_lock& s_locks, std::string dir, std::string data);

};

#endif  // SERVER_INCLUDES_SESSION_HPP_
