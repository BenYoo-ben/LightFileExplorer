#ifndef CLIENT_INCLUDES_NETS_HPP_
#define CLIENT_INCLUDES_NETS_HPP_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "common.hpp"

// class to handle socket operations
class client_object {
 private:
    // socket variable
    int c_sock;

 public:
    // intialize socket
    int client_socket_init();

    // connect socket to the server
    int client_socket_connect(std::string server_address, unsigned short port);

    // return socket
    int get_socket();

    // send data with pre-defined protocol
    void send_data(char type, std::string dir, std::string data);

    // recv data from server(general)
    int recv_data(std::vector<char> & return_data);

    // recv data from server(short responses only)
    int recv_data();

    // closes socket
    void close_socket();

    // handle error cases
    void handle_error(int err_case);
};

#endif  // CLIENT_INCLUDES_NETS_HPP_
