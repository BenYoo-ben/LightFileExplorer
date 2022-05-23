#ifndef CLIENT_INCLUDES_COMMON_HPP_
#define CLIENT_INCLUDES_COMMON_HPP_

#include <json/json.h>  // jsoncpp to parse json data to string and vice versa

#include <time.h>  // get time
#include <stdint.h>  // to use byte specified integers
#include <sys/wait.h>  // for massive client testing(term between requests)
#include <string.h>  // memset and etc
#include <unistd.h>  // read, write, etc
#include <fcntl.h>  // open and etc

#include <vector>  // std vector
#include <string>  // std string
#include <iostream>  // stringstream and cout and etc.

#endif  // CLIENT_INCLUDES_COMMON_HPP_
