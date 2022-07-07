#ifndef SERVER_INCLUDES_OBJECTS_HPP_
#define SERVER_INCLUDES_OBJECTS_HPP_

#include <vector>
#include <map>
#include <string>

#include "common.hpp"
#include "file_scouter.hpp"

class directory_object;
class file_object;

class file_object {
 private:
    std::string name;
    std::string time;
    std::string type;
    std::string auth;
    uint64_t size;
    bool is_dir;

 public:
    file_object(std::string name, std::string time,
                std::string auth, std::string size, std::string is_dir);

    void print_members();

    std::string get_name();

    std::string get_time();

    std::string get_type();

    std::string get_auth();

    std::uint64_t get_size();

    bool get_is_dir();

    void set_type(std::string new_type);
};

class directory_object {
 private:
    std::vector<std::vector<file_object>> dir;
    std::string directory_path;

 public:
    directory_object(std::vector<std::vector<file_object>> parsed_json,
                     std::string directory_path);

    int get_vectors(std::vector<std::vector<file_object>> *vvfObjPtr);

    std::string get_directory_path();
};

class json_handler{
 private:
 public:
    int make_json_object(std::string dir_name, Json::Value *jsv);

    int directory_to_file_object_vector(std::string dir_name, std::vector<file_object> *vfObjPtr);
};

class lock_handler{
 private:
    std::map<std::string, int> soft_lock;  // read
    std::map<std::string, int> hard_lock;  // write
 public:
    int SOFT_LOCK = 1;
    int HARD_LOCK = 2;
    lock_handler();
    void add_lock(int lock_no, std::string full_name);
    void remove_lock(int lock_no, std::string full_name);
    bool check_lock(int lock_no, std::string full_name);
    void debug_lock();
};
#endif  // SERVER_INCLUDES_OBJECTS_HPP_
   /* Sample json object:
    {
        [
            {
                "name":"system.so",
                "time":"2021-12-15-10-04-01",
                "size":"1723123" (in bytes),
                "is_dir":"0",
            },
            
            [
                //first object is directory itself.
                {
                "name":"bin"
                "time":"",
                "size":"",
                "is_dir":"1"
                },
                
                {
                "name":"iptables",
                "time":"2011-10-30-10-04-01",
                "size":"12345" (in bytes),
                "is_dir":"0",
                },

                                {
                "name":"ebtables",
                "time":"2011-10-30-10-04-01",
                "size":"12345" (in bytes),
                "is_dir":"0",
                },

                                {
                "name":"test",
                "time":"2011-10-30-10-04-01",
                "size":"12345" (in bytes),
                "is_dir":"0",
                }


            ]
        ]
            
    }
    */
