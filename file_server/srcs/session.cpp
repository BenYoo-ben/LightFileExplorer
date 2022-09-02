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
        case REQ_TYPE_DOWNLOAD_FILE: {
            std::stringstream ss;
            ss << dir << data;
            std::string full_file = ss.str();

            if (lock->check_lock(lock->HARD_LOCK, full_file)) {
                lock->add_lock(lock->SOFT_LOCK, full_file);

                file_manager fm;
                struct stat file_stat; 
                
                int fRet = fm.get_stat_of_file(full_file.c_str(), &file_stat);
                if (fRet < 0) {
                    fprintf(stderr, "get_stat_of_file failed for %s\n", full_file.c_str());
                    lock->remove_lock(lock->SOFT_LOCK, full_file);
                    return -1;
                }

                uint32_t fTotalSize = fm.stat_get_size(&file_stat);

                if (write(c_sock, &fTotalSize, sizeof(uint32_t)) != sizeof(uint32_t)) {
                    perror("Writing File Size Failed");
                    lock->remove_lock(lock->SOFT_LOCK, full_file);
                    return -1;
                }

                uint32_t ackVal = -1;

                if (read(c_sock, &ackVal, sizeof(uint32_t)) < 0) {
                    perror("Getting File Size Ack failed");
                    lock->remove_lock(lock->SOFT_LOCK, full_file);
                    return -1;
                }

                if (ackVal != 0) {
                    perror("Recvd File Size ack is not 0");
                    lock->remove_lock(lock->SOFT_LOCK, full_file);
                }

                FILE *filePtr = fopen(full_file.c_str(), "rb");

                if (filePtr == nullptr) {
                    perror("download open file fail");
                    lock->remove_lock(lock->SOFT_LOCK, full_file);
                    return -1;
                }

                char sendBuffer[global_window_size];

                uint32_t fSum = 0;
                ssize_t readBytes = -1;
                while (fSum < fTotalSize) {
                    readBytes = fread(sendBuffer, 1, global_window_size, filePtr);

                    if (readBytes <= 0) {
                        perror("READ FAILED WHILE READING FILE");
                        lock->remove_lock(lock->SOFT_LOCK, full_file);
                        return -1;
                   
                    }

                   fSum += readBytes;

                   if (write(c_sock, sendBuffer, readBytes) != readBytes) {
                       perror("WRITE FAILED ON STREAM(download)");
                       lock->remove_lock(lock->SOFT_LOCK, full_file);
                       fclose(filePtr);
                       return -1;
                   }
                }

                if (fclose(filePtr) == EOF) {
                    perror("download file close err");
                    lock->remove_lock(lock->SOFT_LOCK, full_file);
                    return -1;
                }
                
                lock->remove_lock(lock->SOFT_LOCK, full_file);
            } else {
                // HARD LOCKED, can't read
                return -1;
            }
        break;
        }
        case REQ_TYPE_COPY_FILE: {
            if (lock->check_lock(lock->HARD_LOCK, dir)
                && lock->check_lock(lock->SOFT_LOCK, data)
                && lock->check_lock(lock->HARD_LOCK, data)) {
                lock->add_lock(lock->SOFT_LOCK, dir);
                lock->add_lock(lock->HARD_LOCK, data);

                FILE *fromFile = fopen(dir.c_str(), "rb");
                if (fromFile == nullptr) {
                    perror("fromFile open failure");
                    lock->remove_lock(lock->SOFT_LOCK, dir);
                    lock->remove_lock(lock->HARD_LOCK, data);
                    return -1;
                }

                FILE *toFile = fopen(data.c_str(), "wb");
                if (toFile == nullptr) {
                    perror("toFile open failure");
                    lock->remove_lock(lock->SOFT_LOCK, dir);
                    lock->remove_lock(lock->HARD_LOCK, data);
                    return -1;
                }

                char buff[global_window_size];
                uint32_t rSize, wSize;
                
                // read --> write(chunk size)
                while ((rSize = fread(buff, 1, global_window_size, fromFile)) == global_window_size) {
                    wSize = fwrite(buff, 1, rSize, toFile);

                    if (wSize != rSize) {
                        perror("copy write fail");
                        lock->remove_lock(lock->SOFT_LOCK, dir);
                        lock->remove_lock(lock->HARD_LOCK, data);
                        return -1;
                    }

                    rSize = 0;
                    wSize = 0;
                }
                
                // remaining read data
                if (rSize > 0) {
                    wSize = fwrite(buff, 1, rSize, toFile);
                    
                    if (wSize != rSize) {
                        perror("copy write fail 2");
                        lock->remove_lock(lock->SOFT_LOCK, dir);
                        lock->remove_lock(lock->HARD_LOCK, data);
                        return -1;
                    }
                }

                if (fclose(fromFile) == EOF) {
                    perror("fromfile fclose err");
                    lock->remove_lock(lock->SOFT_LOCK, dir);
                    lock->remove_lock(lock->HARD_LOCK, data);
                    return -1;
                }

                if (fclose(toFile) == EOF) {
                    perror("tofile fclose err");
                    lock->remove_lock(lock->SOFT_LOCK, dir);
                    lock->remove_lock(lock->HARD_LOCK, data);
                    return -1;
                }

                char ackSendBuffer[4] = { 0, };
                size_t ret = write(c_sock, ackSendBuffer, 4);
                if (ret != 4) {
                    perror(" REQ_TYPE_COPY_FILE, write != 4");
                    lock->remove_lock(lock->SOFT_LOCK ,dir);
                    lock->remove_lock(lock->HARD_LOCK, data);
                    return -1;
                }

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

                if (rename(dir.c_str(), data.c_str()) != 0) {
                    perror("rename in move failed");
                    lock->remove_lock(lock->HARD_LOCK, dir);
                    lock->remove_lock(lock->HARD_LOCK, data);
                    return -1;
                }

                char ackSendBuffer[4] = { 0, };
                size_t ret = write(c_sock, ackSendBuffer, 4);
                if (ret != 4) {
                    perror(" REQ_TYPE_MOVE_FILE, write != 4");
                    lock->remove_lock(lock->SOFT_LOCK ,dir);
                    lock->remove_lock(lock->HARD_LOCK, data);
                    return -1;
                }

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
                lock->add_lock(lock->HARD_LOCK, full_string);

                if (remove(full_string.c_str()) != 0) {
                    perror("delete remove failed");
                    lock->remove_lock(lock->HARD_LOCK, full_string);
                }

                char ackSendBuffer[4] = { 0, };
                size_t ret = write(c_sock, ackSendBuffer, 4);
                if (ret != 4) {
                    perror(" REQ_TYPE_COPY_FILE, write != 4");
                    lock->remove_lock(lock->HARD_LOCK ,dir);
                    return -1;
                }

                lock->remove_lock(lock->HARD_LOCK, full_string);

                break;
            }
        }
        case REQ_TYPE_RENAME_FILE: {
            if (lock->check_lock(lock->SOFT_LOCK, dir)
                && lock->check_lock(lock->HARD_LOCK, dir)) {
                lock->add_lock(lock->HARD_LOCK, dir);

                if (rename(dir.c_str(), data.c_str()) != 0) {
                    perror("rename rename failed");
                    lock->remove_lock(lock->HARD_LOCK, dir);
                    return -1;
                }
                
                char ackSendBuffer[4] = { 0, };
                size_t ret = write(c_sock, ackSendBuffer, 4);
                if (ret != 4) {
                    perror(" REQ_TYPE_COPY_FILE, write != 4");
                    lock->remove_lock(lock->SOFT_LOCK ,dir);
                    return -1;
                }

                lock->remove_lock(lock->HARD_LOCK, dir);
                break;
            }
        }
        case REQ_TYPE_DIR_INFO_DEPTH_1: {
            json_handler jh;

            if (lock->check_lock(lock->HARD_LOCK, dir)) {
                lock->add_lock(lock->SOFT_LOCK, dir);

                Json::Value dir_json_object(Json::arrayValue);
                int ret = jh.make_json_object(dir, &dir_json_object);
                if (ret < 0) {
                    perror("make json object failure");
                    lock->remove_lock(lock->SOFT_LOCK, dir);
                    return -1;
                }

                std::string json_str = dir_json_object.toStyledString();

                int buffer_size = json_str.length();

                const int kBufSize = 4 + buffer_size + 1;
                char send_buffer[kBufSize];

                memset(send_buffer, 0x0, buffer_size);
                uint32_t data_size = buffer_size;

                memcpy(send_buffer, &data_size, 4);

                snprintf(send_buffer + 4, sizeof(send_buffer) - 4,
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
        case REQ_TYPE_UPLOAD_FILE: {
         std::stringstream ss;
            ss << dir << data;
            std::string full_file = ss.str();

            if (lock->check_lock(lock->SOFT_LOCK, full_file) 
                && lock->check_lock(lock->HARD_LOCK, full_file)) {
                lock->add_lock(lock->HARD_LOCK, full_file);

                struct stat dummyStat;
                // check if file exists
                if (stat(full_file.c_str(), &dummyStat) >= 0) {
                    perror("Requested file already exists!");
                    // file is already present
                    lock->remove_lock(lock->HARD_LOCK, full_file);
                    return -1;
                }

                // send ack, server is ready for data stream
                uint32_t temp = 0;
                memset(&temp, 0x00, sizeof(uint32_t));
                size_t ret = write(c_sock, &temp, sizeof(uint32_t));
                
                if (ret != sizeof(uint32_t)) {
                    lock->remove_lock(lock->HARD_LOCK, full_file);
                    perror("REQ_TYPE_UPLOAD_FILE, write size != sizeof(uint32_t))");
                    return -1;
                }

                ret = read(c_sock, &temp, sizeof(uint32_t));
                
                if (ret != sizeof(uint32_t)) {
                    lock->remove_lock(lock->HARD_LOCK, full_file);
                    perror("REQ_TYPE_UPLOAD_FILE, read size != sizeof(uint32_t))");
                    return -1;
                }

                FILE *file = fopen(full_file.c_str(), "wb");

                char recvBuffer[global_window_size];

                uint32_t fSum = 0;
                size_t readBytes = -1;
                while (fSum < temp) {
                   readBytes = read(c_sock, recvBuffer, global_window_size);
                   if (readBytes < 0) {
                       perror("READ FAILED WHILE READING STREAM");
                       lock->remove_lock(lock->HARD_LOCK, full_file);
                       fclose(file);
                       return -1;
                   }

                   fSum += readBytes;
                   
                   if (fwrite(recvBuffer, 1, readBytes, file) != readBytes) {
                       perror("WRITE FAILED WHILE PROCESSING STREAM");
                       lock->remove_lock(lock->HARD_LOCK, full_file);
                       fclose(file);
                       return -1;
                   }
                }
                
                if (fclose(file) == EOF) {
                    perror("upload file close fail");
                    lock->remove_lock(lock->HARD_LOCK, full_file);
                    return -1;
                }

                char ackSendBuffer[4] = { 0, };
                ret = write(c_sock, ackSendBuffer, 4);
                if (ret != 4) {
                    perror(" REQ_TYPE_COPY_FILE, write != 4");
                    lock->remove_lock(lock->SOFT_LOCK ,dir);
                    return -1;
                }
                
                lock->remove_lock(lock->HARD_LOCK, full_file);
            } else {
                // HARD LOCKED, can't read
                return -1;
            }
            break;
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
        if (bytes_read <= 0) {
            perror("SocketReadERR");
            break;
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
                std::cout << "Socket recvd EOF" << std::endl;
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

            if (handle_request(type, dir_str, data_str, c_sock) < 0) {
                // errCode = 7
                uint32_t errCode = 0x07;
                perror("handle_request failed: 07");

                if (write(c_sock, &errCode, sizeof(uint32_t)) < 0) {
                    std::cout << "Write Failed(HARD LOCK FAIL)" << std::endl;
                }
            }
        }
    }
    close_socket();
    delete(this);
    return NULL;
}

void session_object::close_socket() {
    close(c_sock);
}
