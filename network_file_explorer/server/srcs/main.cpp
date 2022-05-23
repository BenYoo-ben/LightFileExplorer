#include "main.hpp"
#include "common.hpp"
#include "nets.hpp"
#include "file_scouter.hpp"
#include "objects.hpp"

int main() {
    server_object so;

    if (so.server_socket_init() < 0) {
        std::cout << "Server Socket Init Failed..." << std::endl;
        exit(1);
    }

    if (so.server_socket_bind() < 0) {
        std::cout << "Server Socket Bind Failed..." << std::endl;
        exit(2);
    }

    if (so.server_socket_listen() < 0) {
        std::cout << "Server Socket Listen Failed..." << std::endl;
        exit(3);
    }

    so.server_socket_start();
}
