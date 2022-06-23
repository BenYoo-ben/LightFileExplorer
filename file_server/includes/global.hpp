#ifndef SERVER_INCLUDES_GLOBAL_HPP_
#define SERVER_INCLUDES_GLOBAL_HPP_

constexpr unsigned int global_server_port = 55551;
constexpr unsigned int global_server_listen_maximum = 100000;
constexpr unsigned int global_expected_MTU = 1500;

constexpr unsigned int global_window_size = ((1 << 16) - 1);

// #define REQ_TYPE_DIR_INFO_DEPTH_2 0

#define REQ_TYPE_DOWNLOAD_FILE 1
#define REQ_TYPE_COPY_FILE 2
#define REQ_TYPE_MOVE_FILE 3
#define REQ_TYPE_DELETE_FILE 4
#define REQ_TYPE_RENAME_FILE 5
#define REQ_TYPE_DIR_INFO_DEPTH_1 6
#define REQ_TYPE_UPLOAD_FILE 7
#endif  // SERVER_INCLUDES_GLOBAL_HPP_
