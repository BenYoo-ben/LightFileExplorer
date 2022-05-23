#ifndef CLIENT_INCLUDES_UTILS_HPP_
#define CLIENT_INCLUDES_UTILS_HPP_

#include <string>
#include <vector>

#include "common.hpp"
#include "objects.hpp"

class parser {
 public:
    std::vector<std::string>
     get_splitted_path(std::string input_string, char delim);

    // get directory of file gets 'path' part from ('path'/'filename')
    std::string get_directory(std::string input_string, char delim);

    // parse received data from server to explore files
    std::vector<std::vector<file_object>>
     parse_json_to_list_vectors(std::string input_json);

    std::vector<file_object> parse_json_to_file_vectors(std::string input_json);
};
#endif  // CLIENT_INCLUDES_UTILS_HPP_
