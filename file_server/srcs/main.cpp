#include "main.hpp"
#include "common.hpp"
#include "nets.hpp"
#include "file_scouter.hpp"
#include "objects.hpp"
#include "global.hpp"

int main(int argc, char *argv[]) {
    server_object so;

    int opt;
    uint16_t sPort = global_server_port;;
    auto numThread = 1; 

    while ((opt = getopt(argc, argv, "pt")) != -1) {
        switch (opt) {
            case 'p':
                sPort = static_cast<uint16_t>(atoi(optarg));
                break;
            case 't':
                numThread = atoi(optarg);
            case '?':
                std::cerr << "Unknown option: " << char(optopt) << "'!'" << std::endl;
        }
    }

    if (so.server_socket_init() < 0) {
        std::cerr << "Server Socket Init Failed..." << std::endl;
        exit(1);
    }

    if (so.server_socket_bind(sPort) < 0) {
        std::cerr << "Server Socket Bind Failed... port:" << sPort << std::endl;
        exit(2);
    }   

    if (so.server_socket_listen() < 0) {
        std::cerr << "Server Socket Listen Failed..." << std::endl;
        exit(3);
    }

    so.server_socket_start(numThread);
}
