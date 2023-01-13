#include "common.hpp"
#include "nets.hpp"
#include "global.hpp"
#include "session.hpp"

int server_object::server_socket_init() {
    s_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (s_sock < 0) {
        std::cerr << "Socket Generation failed" << std::endl;
        return -1;
    }
    return 0;
}

int server_object::server_socket_bind(uint16_t sPort) {
    struct sockaddr_in server_address;

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(sPort);

    int ret = bind(s_sock, (struct sockaddr *) &server_address,
                    sizeof(server_address));

    if (ret < 0) {
        if (server_socket_close() < 0) {
            std::cerr << "Server Socket close failed" << std::endl;
            return -2;
        }
        std::cerr << "Server Bind failed" << std::endl;
        return -1;
    }

    return 0;
}

int server_object::server_socket_listen() {
    int ret = listen(s_sock, global_server_listen_maximum);

    if (ret < 0) {
        if (server_socket_close() < 0) {
            std::cerr << "Server Socket closed failed" << std::endl;
            return -2;
        }
        std::cerr << "Socket Listen failed" << std::endl;
        return -1;
    }
    return 0;
}


int server_object::server_socket_start() {
    socklen_t client_addr_size = sizeof(struct sockaddr_in);

    // prevent server from dying when writing to closed sockets.
    signal(SIGPIPE, SIG_IGN);

    while (true) {
        struct sockaddr_in client_addr;

        int client_socket = accept(s_sock, (struct sockaddr*) &client_addr,
                                    &client_addr_size);


        if (client_socket < 0) {
            server_socket_close();
            return -1;
        } else {
            char clientIpAddrStr[global_expected_ip_length] = {0, };
            if (inet_ntop(AF_INET, &client_addr, clientIpAddrStr, sizeof(clientIpAddrStr)) != NULL) {
                std::cout << "Connected From :[" << clientIpAddrStr << "]" << std::endl;
                new session_object(client_socket);
            } else {
                std::cerr << "Unidentified Client Socket IP" << std::endl;
                return -1;
            }
        }
    }
    return 0;
}

int server_object::server_socket_close() {
    return close(this->s_sock);
}
