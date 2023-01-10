#ifndef SERVER_INCLUDES_NETS_HPP_
#define SERVER_INCLUDES_NETS_HPP_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

class server_object{
private:
    int s_sock;

public:
    server_object() = default;

    server_object(const server_object&) = default;
    server_object& operator=(const server_object&) = default;

    server_object(server_object&&) = default;
    server_object& operator=(server_object&&) = default;

    ~server_object() = default;

    int server_socket_init();
    int server_socket_bind(uint16_t sPort);
    int server_socket_listen();
    int server_socket_start();
    int server_socket_close();
};

#endif  // SERVER_INCLUDES_NETS_HPP_
