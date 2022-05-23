#include "objects.hpp"
#include "common.hpp"

file_object::file_object(std::string name, std::string time,
                        std::string auth, std::string size,
                        std::string is_dir) {
    this->name = name;
    this->time = time;
    this->auth = auth;

    std::istringstream(size) >> (this->size);
    std::istringstream(is_dir) >> (this->is_dir);
}

void file_object::print_members() {
    // debug
    std::cout<< "name : " << name << "\ntime : " << time <<
                "\nfile_type : " << type << "\nauth :" << auth <<
                "\nsize : " << size << "\nis_dir : " << is_dir
                << std::endl << std::endl;
}

std::string file_object::get_name() {
    return name;
}

std::string file_object::get_time() {
    return time;
}

std::string file_object::get_type() {
    return type;
}

std::string file_object::get_auth() {
    return auth;
}

uint64_t file_object::get_size() {
    return size;
}

bool file_object::get_is_dir() {
    return is_dir;
}
void file_object::set_type(std::string new_type) {
    type = new_type;
}

directory_object::directory_object
    (std::vector<std::vector<file_object>> parsed_json,
     std::string directory_path) {
    this->dir = parsed_json;
    this->directory_path = directory_path;
}

std::vector<std::vector<file_object>> directory_object::get_vectors() {
    return dir;
}

std::string directory_object::get_directory_path() {
    return directory_path;
}
