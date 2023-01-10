#ifndef SERVER_INCLUDES_FILE_SCOUTER_HPP_
#define SERVER_INCLUDES_FILE_SCOUTER_HPP_
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include <vector>
#include <string>

#include "common.hpp"

class file_manager{
private:
public:
    file_manager() = default;
   
    file_manager(const file_manager&) = default;
    file_manager& operator=(const file_manager&) = default;

    file_manager(file_manager&&) = default;
    file_manager& operator=(file_manager&&) = default;
    
    ~file_manager() = default;

    int files_in_directory_to_vector(std::string string_dir_name, std::vector<std::string> *vStr);

    int get_stat_of_file(std::string file_name, struct stat *st);

    std::string stat_get_type(struct stat *st);

    bool stat_is_directory(struct stat *st);

    std::string stat_get_time(struct stat *st);

    int stat_get_size(struct stat *st);

    std::string stat_get_permission(struct stat *st);
};


#endif  // SERVER_INCLUDES_FILE_SCOUTER_HPP_
