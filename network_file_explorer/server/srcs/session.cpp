#include "session.hpp"

template<class C, void* (C::* thread_run)()>
void* pthread_member_wrapper(void* data) {
  C* obj = static_cast<C*>(data);
  return (obj->*thread_run)();
}

session_object::session_object(int established_socket,
                               lock_handler *lock) {
    this->lock = lock;
    c_sock = established_socket;
    pthread_create(&session_thread,
                    NULL,
                    pthread_member_wrapper<session_object,
                                            &session_object::run>,
                                            this);
}

int session_object::handle_request(char type,
                                    std::string dir,
                                    std::string data,
                                    int c_sock) {
    switch (type) {
        case REQ_TYPE_DIR_INFO_DEPTH_2: {
            json_handler jh;

            if (lock->check_lock(lock->HARD_LOCK, dir)) {
                lock->add_lock(lock->SOFT_LOCK, dir);

                Json::Value dir_json_object = jh.make_json_object(dir, 2);

                std::string json_str = dir_json_object.toStyledString();

                int buffer_size = json_str.length();

                std::cout << "SENT :" << buffer_size << std::endl;

                // disable printing contents --> stdout
                // std::cout << "CONTENT:" << json_str << std::endl;

                const int kBufSize = 4 + buffer_size + 1;
                char send_buffer[kBufSize];

                memset(send_buffer, 0x0, buffer_size);
                uint32_t data_size = buffer_size;

                memcpy(send_buffer, &data_size, 4);

                printf("JSON_STR LEN: %u\n", json_str.length());

                snprintf(send_buffer + 4, sizeof(send_buffer) -4,
                        "%s", json_str.c_str());
                
                if (write(c_sock, send_buffer, 4 + buffer_size) < 0) {
                    std::cout << "Write Failed...\n" << std::endl;
                    lock->remove_lock(lock->SOFT_LOCK, dir);
                    return -1;
                }

                lock->remove_lock(lock->SOFT_LOCK, dir);
            } else {
                // HARD LOCKED, can't read
                return -1;
            }
        break;
        }
        case REQ_TYPE_DOWNLOAD_FILE: {
            std::stringstream ss;
            ss << dir << data;
            std::string full_file = ss.str();

            if (lock->check_lock(lock->HARD_LOCK, full_file)) {
                lock->add_lock(lock->SOFT_LOCK, full_file);

                file_manager fm;
                struct stat file_stat = fm.get_stat_of_file(full_file.c_str());
                int file_size = fm.stat_get_size(&file_stat);
                const int kBufSize = 4 + file_size + 1;
                char send_buffer[kBufSize];

                int fd = open(full_file.c_str(), O_RDONLY);

                uint32_t data_size = file_size;
                memset(send_buffer, 0x0, kBufSize);
                memcpy(send_buffer, &data_size, 4);

                int read_size = 0;
                if ((read_size = read(fd, send_buffer+4, file_size)) < 0) {
                    std::cout << "File Read Failed...\n" << std::endl;
                    lock->remove_lock(lock->SOFT_LOCK, full_file);
                    return -1;
                } else {
                    if (write(c_sock, send_buffer, 4 + file_size) < 0) {
                        std::cout<< "Write Failed...\n" << std::endl;
                        lock->remove_lock(lock->SOFT_LOCK, full_file);
                        return -1;
                    }
                    close(fd);
                    lock->remove_lock(lock->SOFT_LOCK, full_file);
                }
            } else {
                // HARD LOCKED, can't read
                return -1;
            }
        break;
        }
        case REQ_TYPE_COPY_FILE: {
            // need only readlock(copy basically makes a new file)
            // check lock for file to be copied and a directory it's going into.
            if (lock->check_lock(lock->HARD_LOCK, dir)
                && lock->check_lock(lock->SOFT_LOCK, data)
                && lock->check_lock(lock->HARD_LOCK, data)) {
                lock->add_lock(lock->SOFT_LOCK, dir);
                lock->add_lock(lock->HARD_LOCK, data);

                // temporary using system command
                printf("COPY <%s> to <%s> \n", dir.c_str(), data.c_str());
                char command[1024];
                snprintf(command, sizeof(command),
                        "cp -r %s %s", dir.c_str(), data.c_str());

                if (system(command) < 0) {
                    std::cout << "system(copy) failed" << std::endl;
                    lock->remove_lock(lock->SOFT_LOCK, dir);
                    lock->remove_lock(lock->HARD_LOCK, data);
                    return -1;
                }
                memset(command, 0x0, 4);
                write(c_sock, command, 4);

                lock->remove_lock(lock->SOFT_LOCK, dir);
                lock->remove_lock(lock->HARD_LOCK, data);

                break;
            }
        }
        case REQ_TYPE_MOVE_FILE: {
            // need read and write lock
            if (lock->check_lock(lock->SOFT_LOCK, dir)
                && lock->check_lock(lock->HARD_LOCK, dir)
                && lock->check_lock(lock->SOFT_LOCK, data)
                && lock->check_lock(lock->HARD_LOCK, data)) {
                lock->add_lock(lock->HARD_LOCK, dir);
                lock->add_lock(lock->HARD_LOCK, data);

                // temporary using system command
                printf("MOVE <%s> to <%s> \n", dir.c_str(), data.c_str());
                char command[1024];
                snprintf(command, sizeof(command),
                        "mv %s %s", dir.c_str(), data.c_str());

                if (system(command) <0) {
                    std::cout << "system(move) failed" << std::endl;
                    lock->remove_lock(lock->HARD_LOCK, dir);
                    lock->remove_lock(lock->HARD_LOCK, data);
                    return -1;
                }
                memset(command, 0x0, 4);
                write(c_sock, command, 4);

                lock->remove_lock(lock->HARD_LOCK, dir);
                lock->remove_lock(lock->HARD_LOCK, data);

                break;
            }
        }
        case REQ_TYPE_DELETE_FILE: {
            std::stringstream ss;
            ss << dir << data;
            std::string full_string = ss.str();

            if (lock->check_lock(lock->SOFT_LOCK, full_string)
                && lock->check_lock(lock->HARD_LOCK, full_string)) {
                // temporary using system command
                printf("DELETE <%s> to <%s> \n", dir.c_str(), data.c_str());
                char command[1024];
                snprintf(command, sizeof(command),
                        "rm -rf %s%s", dir.c_str(), data.c_str());

                if (system(command) < 0) {
                    std::cout << "system(remove) failed " << std::endl;
                    return -1;
                }
                memset(command, 0x0, 4);
                write(c_sock, command, 4);
                break;
            }
        }
        case REQ_TYPE_RENAME_FILE: {
            if (lock->check_lock(lock->SOFT_LOCK, dir)
                && lock->check_lock(lock->HARD_LOCK, dir)) {
                printf("RENAME <%s> to <%s> \n", dir.c_str(), data.c_str());
                char command[1024];
                snprintf(command, sizeof(command),
                        "mv %s %s", dir.c_str(), data.c_str());
                if (system(command) < 0) {
                    std::cout << "system(rename) failed" << std::endl;
                    return -1;
                }
                memset(command, 0x0, 4);
                write(c_sock, command, 4);
                break;
            }
        }
        case REQ_TYPE_DIR_INFO_DEPTH_1: {
            json_handler jh;

            if (lock->check_lock(lock->HARD_LOCK, dir)) {
                lock->add_lock(lock->SOFT_LOCK, dir);

                Json::Value dir_json_object = jh.make_json_object(dir, 1);

                std::string json_str = dir_json_object.toStyledString();

                int buffer_size = json_str.length();

                std::cout << "SENT :" << buffer_size << std::endl;

                // disable printing contents --> stdout
                // std::cout << "CONTENT:" << json_str << std::endl;

                const int kBufSize = 4 + buffer_size + 1;
                char send_buffer[kBufSize];

                memset(send_buffer, 0x0, buffer_size);
                uint32_t data_size = buffer_size;

                memcpy(send_buffer, &data_size, 4);

                printf("JSON_STR LEN: %u\n", json_str.length());

                snprintf(send_buffer + 4, sizeof(send_buffer) -4,
                        "%s", json_str.c_str());
                
                if (write(c_sock, send_buffer, 4 + buffer_size) < 0) {
                    std::cout << "Write Failed...\n" << std::endl;
                    lock->remove_lock(lock->SOFT_LOCK, dir);
                    return -1;
                }

                lock->remove_lock(lock->SOFT_LOCK, dir);
            } else {
                // HARD LOCKED, can't read
                return -1;
            }
        }
        default: {
            std::cout << "Unrecognized Code : " << type << std::endl;
            break;
        }
    }
    return 0;
}

void *session_object::run() {
    const int MTU = global_expected_MTU;
    char buffer[MTU];
    int bytes_read;

    while (true) {
        memset(buffer, 0x0, global_expected_MTU);
        bytes_read = 0;

        bytes_read = read(c_sock, buffer, global_expected_MTU);
        if (bytes_read < 0) {
            std::cerr << "Invalid Read" << std::endl;
            close_socket();
            return NULL;
        } else if (bytes_read == 0) {
            // test if socket is open(without this check,
            // this thread tries to read to closd socket
            // reaching 100% cpu usage).
            char test_buffer[2];
            snprintf(test_buffer, sizeof(test_buffer), "0");

            int err = write(c_sock, test_buffer, 1);
            if (err < 0) {
                std::cout<< "Socket Closed on Client Side ! \n" << std::endl;
                break;
            } else {
                continue;
            }
        } else {
            // valid data is read
            // parse data(type(1), dir_length(4), dir, data_length(4), data)
            char type;
            uint32_t dir_size;
            uint32_t data_size;

            std::string dir_str;
            std::string data_str;

            int ptr = 0;

            type = buffer[ptr];
            ptr += 1;

            dir_size = *(buffer + ptr);
            ptr += 4;

            const int kBufferSize  = dir_size + 1;
            char dir_buffer[kBufferSize];
            memcpy(dir_buffer, buffer+ptr, dir_size);
            dir_buffer[kBufferSize - 1] = '\0';
            dir_str = std::string(dir_buffer);
            ptr += dir_size;

            data_size = *(buffer + ptr);
            ptr += 4;

            const int kBufferSize2 = data_size + 1;
            char data_buffer[kBufferSize2];
            memcpy(data_buffer, buffer + ptr, data_size);
            data_buffer[kBufferSize2 - 1] ='\0';
            data_str = std::string(data_buffer);

            // Debug parse input protocol
            std::cout << "RECEIVED:"
            << "\ntype: " << static_cast<int>(type)
            << "\ndir_size: " << dir_size
            << "\ndir : " << dir_str
            << "\ndata_size : " << data_size
            << "\ndata : "<< data_str
            << std::endl;

            if (handle_request(type, dir_str, data_str, c_sock) < 0) {
                char send_buffer[4];
                memset(send_buffer, 0x0, 4);
                if (write(c_sock, send_buffer, 4) < 0) {
                    std::cout << "Write Failed(HARD LOCK FAIL)" << std::endl;
                }
            }
        }
    }
    close_socket();
    std::cout<< "Socket Closed ! \n" << std::endl;
    delete(this);
    std::cout << "Delete Memory Allocated ! \n" << std::endl;

    return NULL;
}

void session_object::close_socket() {
    close(c_sock);
}
