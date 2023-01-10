#include "session.hpp"

template<class C, void* (C::* thread_run)()>
void* pthread_member_wrapper(void* data) {
    C* obj = static_cast<C*>(data);
    return (obj->*thread_run)();
}

session_object::session_object(int established_socket) {
    c_sock = established_socket;
    pthread_create(&session_thread,
            NULL,
            pthread_member_wrapper<session_object,
            &session_object::run>,
            this);
}

int session_object::handle_request(char type, std::string dir, std::string data) {
    session_lock s_lock = session_lock();

    switch (type) {
        case REQ_TYPE_DOWNLOAD_FILE:
            return handleDownload(s_lock, dir, data);
        case REQ_TYPE_COPY_FILE:
            return handleCopy(s_lock, dir, data);
        case REQ_TYPE_MOVE_FILE:
            return handleMove(s_lock, dir, data);
        case REQ_TYPE_DELETE_FILE: 
            return handleDelete(s_lock, dir, data);
        case REQ_TYPE_RENAME_FILE: 
            return handleRename(s_lock, dir, data);
        case REQ_TYPE_DIR_INFO_DEPTH_1: 
            return handleDirInfoD1(s_lock, dir, data);
        case REQ_TYPE_UPLOAD_FILE: 
            return handleUpload(s_lock, dir, data);
        default: 
            std::cout << "Unrecognized Code : " << type << std::endl;
            break;
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

            if (handle_request(type, dir_str, data_str) < 0) {
                // errCode = 7
                uint32_t errCode = 0x07;
                perror("handle_request failed: 07");

                if (write(c_sock, &errCode, sizeof(uint32_t)) < 0) {
                    std::cout << "Write Failed(HARD LOCK FAIL)" << std::endl;
                }
            }
        }
    }
    delete(this);
    return NULL;
}

void session_object::close_socket() {
    close(c_sock);
}

int session_object::handleDownload(session_lock& s_lock, std::string dir, std::string data) {
    std::stringstream ss;
    ss << dir << data;
    std::string full_file = ss.str();

    auto check = lock_handler::get_instance().check_lock(file_lock(lock_handler::WRITE, full_file));

    if (check == true) {
        fprintf(stderr, "%s lock check failed\n", __func__);
        return -1;
    }

    if (s_lock.add_lock(file_lock(lock_handler::READ, full_file)) != true) {
        fprintf(stderr, "%s lock add failed\n", __func__);
        return -1;
    }

    file_manager fm;
    struct stat file_stat; 

    int fRet = fm.get_stat_of_file(full_file.c_str(), &file_stat);
    if (fRet < 0) {
        fprintf(stderr, "get_stat_of_file failed for %s\n", full_file.c_str());
        return -1;
    }

    uint32_t fTotalSize = fm.stat_get_size(&file_stat);

    if (write(c_sock, &fTotalSize, sizeof(uint32_t)) != sizeof(uint32_t)) {
        perror("Writing File Size Failed");
        return -1;
    }

    uint32_t ackVal = -1;

    if (read(c_sock, &ackVal, sizeof(uint32_t)) < 0) {
        perror("Getting File Size Ack failed");
        return -1;
    }

    if (ackVal != 0) {
        perror("Recvd File Size ack is not 0");
    }

    FILE *filePtr = fopen(full_file.c_str(), "rb");

    if (filePtr == nullptr) {
        perror("download open file fail");
        return -1;
    }

    char sendBuffer[global_window_size];

    uint32_t fSum = 0;
    ssize_t readBytes = -1;

    while (fSum < fTotalSize) {
        readBytes = fread(sendBuffer, 1, global_window_size, filePtr);

        if (readBytes <= 0) {
            perror("READ FAILED WHILE READING FILE");
            return -1;

        }

        fSum += readBytes;

        if (write(c_sock, sendBuffer, readBytes) != readBytes) {
            perror("WRITE FAILED ON STREAM(download)");
            fclose(filePtr);
            return -1;
        }
    }

    if (fclose(filePtr) == EOF) {
        perror("download file close err");
        return -1;
    }
    return 0;
}

int session_object::handleCopy(session_lock& s_lock, std::string dir, std::string data) {
    auto check_dir_write = lock_handler::get_instance().check_lock(file_lock(lock_handler::WRITE, dir));
    auto check_data_read = lock_handler::get_instance().check_lock(file_lock(lock_handler::READ, data));
    auto check_data_write = lock_handler::get_instance().check_lock(file_lock(lock_handler::WRITE, data));

    if ((check_dir_write | check_data_read | check_data_write) == true) {
        perror("lock check failed");
        return -1;
    }

    if (s_lock.add_lock(file_lock(lock_handler::READ, dir)) != true) {
        perror("get lock dir read failed");
        return -1;
    }

    if (s_lock.add_lock(file_lock(lock_handler::WRITE, data)) != true) {
        perror("get lock data write failed");
        return -1;
    }

    FILE *fromFile = fopen(dir.c_str(), "rb");
    if (fromFile == nullptr) {
        perror("fromFile open failure");
        return -1;
    }

    FILE *toFile = fopen(data.c_str(), "wb");
    if (toFile == nullptr) {
        perror("toFile open failure");
        return -1;
    }

    char buff[global_window_size];
    uint32_t rSize, wSize;

    // read --> write(chunk size)
    while ((rSize = fread(buff, 1, global_window_size, fromFile)) == global_window_size) {
        wSize = fwrite(buff, 1, rSize, toFile);

        if (wSize != rSize) {
            perror("copy write fail");
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
            return -1;
        }
    }

    if (fclose(fromFile) == EOF) {
        perror("fromfile fclose err");
        return -1;
    }

    if (fclose(toFile) == EOF) {
        perror("tofile fclose err");
        return -1;
    }

    char ackSendBuffer[4] = { 0, };
    size_t ret = write(c_sock, ackSendBuffer, 4);
    if (ret != 4) {
        perror(" REQ_TYPE_COPY_FILE, write != 4");
        return -1;
    }
    return 0;
}
int session_object::handleMove(session_lock& s_lock, std::string dir, std::string data) {
    auto check_dir_read = lock_handler::get_instance().check_lock(file_lock(lock_handler::READ, dir));
    auto check_dir_write = lock_handler::get_instance().check_lock(file_lock(lock_handler::WRITE, dir));
    auto check_data_read = lock_handler::get_instance().check_lock(file_lock(lock_handler::READ, data));
    auto check_data_write = lock_handler::get_instance().check_lock(file_lock(lock_handler::WRITE, data));

    if ((check_dir_read | check_dir_write | check_data_read | check_data_write) == true) {
        perror("check failed");
        return -1;
    }
    // need read and write lock
    if (s_lock.add_lock(file_lock(lock_handler::WRITE, dir)) != true) {
        perror("get write lock dir failed");
        return -1;
    }

    if (s_lock.add_lock(file_lock(lock_handler::WRITE, data)) != true) {
        perror("get write lock data failed");
        return -1;
    }

    if (rename(dir.c_str(), data.c_str()) != 0) {
        perror("rename in move failed");
        return -1;
    }

    char ackSendBuffer[4] = { 0, };
    size_t ret = write(c_sock, ackSendBuffer, 4);
    if (ret != 4) {
        perror(" REQ_TYPE_MOVE_FILE, write != 4");
        return -1;
    }

    return 0;
}

int session_object::handleDelete(session_lock& s_lock, std::string dir, std::string data) {
    std::stringstream ss;
    ss << dir << data;
    std::string full_string = ss.str();

    auto check_read = lock_handler::get_instance().check_lock(file_lock(lock_handler::READ, full_string));
    auto check_write = lock_handler::get_instance().check_lock(file_lock(lock_handler::WRITE, full_string));

    if ((check_read | check_write) == true) {
        perror("check failed");
        return -1;
    }

    if (remove(full_string.c_str()) != 0) {
        perror("delete remove failed");
    }

    char ackSendBuffer[4] = { 0, };
    size_t ret = write(c_sock, ackSendBuffer, 4);
    if (ret != 4) {
        perror(" REQ_TYPE_COPY_FILE, write != 4");
        return -1;
    }
    
    return 0;
}

int session_object::handleRename(session_lock& s_lock, std::string dir, std::string data) {
    auto check_dir_read = lock_handler::get_instance().check_lock(file_lock(lock_handler::READ, dir));
    auto check_dir_write = lock_handler::get_instance().check_lock(file_lock(lock_handler::WRITE, dir));

    if ((check_dir_read | check_dir_write) == true) {
        perror("check failed");
        return -1;
    }

    if (s_lock.add_lock(file_lock(lock_handler::WRITE, dir)) != true) {
        perror("get write lock dir failed");
        return -1;
    }

    if (rename(dir.c_str(), data.c_str()) != 0) {
        perror("rename rename failed");
        return -1;
    }

    char ackSendBuffer[4] = { 0, };
    size_t ret = write(c_sock, ackSendBuffer, 4);
    if (ret != 4) {
        perror(" REQ_TYPE_COPY_FILE, write != 4");
        return -1;
    }

    return 0;
}

int session_object::handleDirInfoD1(session_lock& s_lock, std::string dir, std::string data) {
    auto check_dir_write = lock_handler::get_instance().check_lock(file_lock(lock_handler::WRITE, dir));

    if (check_dir_write == true) {
        perror("check failed");
        return -1;
    }

    if (s_lock.add_lock(file_lock(lock_handler::READ, dir)) != true) {
        perror("get read lock dir failed");
        return -1;
    }
    json_handler jh;
    

    Json::Value dir_json_object(Json::arrayValue);
    int ret = jh.make_json_object(dir, &dir_json_object);
    if (ret < 0) {
        perror("make json object failure");
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
        return -1;
    }

    return 0;
}
int session_object::handleUpload(session_lock& s_lock, std::string dir, std::string data) {
    std::stringstream ss;
    ss << dir << data;
    std::string full_file = ss.str();

    auto check_read = lock_handler::get_instance().check_lock(file_lock(lock_handler::READ, full_file));
    auto check_write = lock_handler::get_instance().check_lock(file_lock(lock_handler::WRITE, full_file));

    if ((check_read | check_write) == true) {
        perror("check failed");
        return -1;
    }

    if (s_lock.add_lock(file_lock(lock_handler::WRITE, full_file)) != true) {
        perror("get write lock failed");
        return -1;
    }

    struct stat dummyStat;
    // check if file exists
    if (stat(full_file.c_str(), &dummyStat) >= 0) {
        perror("Requested file already exists!");
        // file is already present
        return -1;
    }

    // send ack, server is ready for data stream
    uint32_t temp = 0;
    memset(&temp, 0x00, sizeof(uint32_t));
    size_t ret = write(c_sock, &temp, sizeof(uint32_t));

    if (ret != sizeof(uint32_t)) {
        perror("REQ_TYPE_UPLOAD_FILE, write size != sizeof(uint32_t))");
        return -1;
    }

    ret = read(c_sock, &temp, sizeof(uint32_t));

    if (ret != sizeof(uint32_t)) {
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
            fclose(file);
            return -1;
        }

        fSum += readBytes;

        if (fwrite(recvBuffer, 1, readBytes, file) != readBytes) {
            perror("WRITE FAILED WHILE PROCESSING STREAM");
            fclose(file);
            return -1;
        }
    }

    if (fclose(file) == EOF) {
        perror("upload file close fail");
        return -1;
    }

    char ackSendBuffer[4] = { 0, };
    ret = write(c_sock, ackSendBuffer, 4);
    if (ret != 4) {
        perror(" REQ_TYPE_COPY_FILE, write != 4");
        return -1;
    }

    return 0;
}
