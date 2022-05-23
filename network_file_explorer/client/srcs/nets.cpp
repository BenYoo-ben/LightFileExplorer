#include "nets.hpp"
#include "common.hpp"

int client_object::client_socket_init() {
    c_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (c_sock < 0) {
        std::cout << "Socket Generation Fail" << std::endl;
        return -1;
    }
    return 0;
}

int client_object::client_socket_connect
    (std::string server_address_string, unsigned short port) {
    struct sockaddr_in server_address;

    memset(&server_address, 0x0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_address_string.c_str());
    server_address.sin_port = htons(port);

    int ret = connect(c_sock, (struct sockaddr*) &server_address,
                        sizeof(server_address));

    if (ret < 0) {
        std::cout << "Socket Connect Fail" << std::endl;
    }

    return ret;
}

int client_object::get_socket() {
    return c_sock;
}

void client_object::send_data
    (char type, std::string dir, std::string data) {
    uint32_t data_size = data.length();
    uint32_t dir_size = dir.length();

    int send_buffer_size = 1 + 4 + dir_size + 4 + data_size + 1;
    const int kSendBufferSize = send_buffer_size;
    char send_buffer[kSendBufferSize];

    int ptr = 0;

    memcpy(send_buffer+ptr, &type, 1);
    ptr += 1;

    memcpy(send_buffer+ptr, &dir_size, 4);
    ptr += 4;

    snprintf(send_buffer+ptr, sizeof(send_buffer), "%s", dir.c_str());
    ptr += dir_size;

    memcpy(send_buffer+ptr, &data_size, 4);
    ptr += 4;

    snprintf(send_buffer+ptr, sizeof(send_buffer), "%s", data.c_str());

    if (write(c_sock, send_buffer, send_buffer_size) <= 0) {
        std::cout << "Write Failed..." << std::endl;
    }
}

int client_object::recv_data(std::vector<char> & return_data) {
    int data_read = 0;
    uint32_t total_read = 0;
    uint32_t data_size;

    int read_bytes;

    if ((read_bytes = read(c_sock, &data_size, 4)) != 4) {
        std::cout << "Error Occured" << read_bytes << std::endl;
        if (read_bytes == 1) {  // Locked, being used by other data;
            std::cout << "Error Occured(Lock)" << std::endl;
            return -1;
        }
    }
    const int kDataSize  = data_size + 1;
    char recv_buffer[kDataSize];

    std::cout << data_size << std::endl;
    if (data_size > 0) {
        while ((data_read = read(c_sock, recv_buffer, data_size)) > 0) {
            std::cout << "READ : " << data_read << std::endl;
            recv_buffer[data_read] = '\0';
            return_data.insert(return_data.end(),
                            recv_buffer, recv_buffer + data_read);

            total_read += data_read;

            printf("D: %s\n", recv_buffer);

            if (total_read == data_size) {
                break;
            }
        }
    } else {
        return -1;
    }

    return 0;
}

int client_object::recv_data() {
    uint32_t data_size;

    int read_bytes;

    if ((read_bytes = read(c_sock, &data_size, 4)) != 4) {
        std::cout << "Error Occured" << read_bytes << std::endl;
        if (read_bytes == 1) {  // Locked, being used by other data;
            return -1;
        } else if (read_bytes == 2) {
        }
    }

    return 0;
}

void client_object::close_socket() {
    close(c_sock);
}

void client_object::handle_error(int err_case) {
    if (err_case == -1) {
        std::cout <<
            "Requested File is locked(being used by other client)"
        << std::endl;
    }
}
