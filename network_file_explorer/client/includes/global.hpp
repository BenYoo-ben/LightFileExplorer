#ifndef CLIENT_INCLUDES_GLOBAL_HPP_
#define CLIENT_INCLUDES_GLOBAL_HPP_

#include <string>

// use port 55551
#define global_server_port 55551

// maximum # of clients
#define global_server_listen_maximum 100000

// expected MTU between client and server.
#define global_expected_MTU 1500

// server address(now local)
#define global_server_address "192.168.0.19"

// protocol definitions
#define REQ_TYPE_DIR_INFO_DEPTH_2 0
#define REQ_TYPE_DOWNLOAD_FILE 1
#define REQ_TYPE_COPY_FILE 2
#define REQ_TYPE_MOVE_FILE 3
#define REQ_TYPE_DELETE_FILE 4
#define REQ_TYPE_RENAME_FILE 5
#define REQ_TYPE_DIR_INFO_DEPTH_1 6
#endif  // CLIENT_INCLUDES_GLOBAL_HPP_
