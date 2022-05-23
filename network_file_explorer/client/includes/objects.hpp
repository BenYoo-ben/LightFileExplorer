#ifndef CLIENT_INCLUDES_OBJECTS_HPP_
#define CLIENT_INCLUDES_OBJECTS_HPP_

#include <string>
#include <vector>

#include "common.hpp"
#include "objects.hpp"


// An object to handle virtual files in the program.
class file_object {
 private:
    std::string name;
    std::string time;
    std::string type;
    std::string auth;
    uint64_t size;
    bool is_dir;

 public:
    // constructor creates new file object
    file_object(std::string name, std::string time
     , std::string auth, std::string size, std::string is_dir);

    // for deubg purpose
    void print_members();

    // get specified members defined private
    std::string get_name();
    std::string get_time();
    std::string get_type();
    std::string get_auth();
    std::uint64_t get_size();
    bool get_is_dir();

    // set type data member
    void set_type(std::string new_type);
};

// An object to andle virtual directory in the program
class directory_object {
 private:
    // list of files in directory
    std::vector<std::vector<file_object>> dir;

    // path of this directory
    std::string directory_path;

 public:
    // create new directory object from parsed json
    directory_object(std::vector<std::vector<file_object>> parsed_json
     , std::string directory_path);

    // return vector of files
    std::vector<std::vector<file_object>> get_vectors();

    // return path of this directory
    std::string get_directory_path();
};

#endif  // CLIENT_INCLUDES_OBJECTS_HPP_
