#include "main.hpp"
#include "common.hpp"
#include "nets.hpp"
#include "file_scouter.hpp"
#include "objects.hpp"
#include "global.hpp"

int main(int argc, char *argv[]) {
    server_object so;

    if (argc > 2) {
        std::cerr << "This program can only have 1 argument or no argument\n $" << argv[0]
                  << "[optional:Port Number]" << std::endl;
    }

    if (so.server_socket_init() < 0) {
        std::cerr << "Server Socket Init Failed..." << std::endl;
        exit(1);
    }

    if (argc == 1) {
        if (so.server_socket_bind(global_server_port) < 0) {
            std::cerr << "Server Socket Bind Failed..." << std::endl;
            exit(2);
        }
    } else if (argc == 2) {
        uint16_t sPort = (uint16_t)(atoi(argv[1]));
        if (so.server_socket_bind(sPort) < 0) {
            std::cerr << "Server Socket Bind Failed..." << std::endl;
            exit(2);
        }   
    }

    if (so.server_socket_listen() < 0) {
        std::cerr << "Server Socket Listen Failed..." << std::endl;
        exit(3);
    }

    so.server_socket_start();
}
