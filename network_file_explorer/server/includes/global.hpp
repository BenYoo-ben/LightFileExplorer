#ifndef SERVER_INCLUDES_GLOBAL_HPP_
#define SERVER_INCLUDES_GLOBAL_HPP_

static unsigned int global_server_port = 55551;
static unsigned int global_server_listen_maximum = 100000;
static unsigned int global_expected_MTU = 1500;

#define REQ_TYPE_DIR_INFO_DEPTH_2 0
#define REQ_TYPE_DOWNLOAD_FILE 1
#define REQ_TYPE_COPY_FILE 2
#define REQ_TYPE_MOVE_FILE 3
#define REQ_TYPE_DELETE_FILE 4
#define REQ_TYPE_RENAME_FILE 5
#define REQ_TYPE_DIR_INFO_DEPTH_1 6
#endif  // SERVER_INCLUDES_GLOBAL_HPP_
