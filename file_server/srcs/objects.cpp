#include "objects.hpp"
#include "common.hpp"

file_object::file_object(std::string name, std::string time, std::string auth,
        std::string size, std::string is_dir) {
    this->name = name;
    this->time = time;
    this->auth = auth;

    std::istringstream(size) >> (this->size);
    std::istringstream(is_dir) >> (this->is_dir);
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

int json_handler::directory_to_file_object_vector
(std::string dir_name ,std::vector<file_object> *vfObjPtr) {
    std::vector<file_object> &file_objects = *vfObjPtr;

    file_manager fm;
    std::vector<std::string> file_names;
    
    int ret = fm.files_in_directory_to_vector(dir_name, &file_names);
    if (ret < 0) {
        perror("get file names failure");
        return -1;
    }

    for (std::string s : file_names) {
        struct stat status;
        int statRet = fm.get_stat_of_file(dir_name+"/" + s, &status);

        if (statRet < 0) {
            perror("get stat failure");
            return -1;
        }

        file_object fo(
                s,
                fm.stat_get_time(&status),
                fm.stat_get_permission(&status),
                std::to_string(fm.stat_get_size(&status)),
                std::to_string(fm.stat_is_directory(&status)));

        file_objects.push_back(fo);
    }

    return 0;
}

int json_handler::make_json_object(std::string dir_name, Json::Value *jvPtr) {
    std::vector<file_object> basic_files; 

    int ret = directory_to_file_object_vector(dir_name, &basic_files);

    if (ret < 0) {
        perror("Dir to File Vector failure");
        return -1;
    }

    Json::Value &root = *jvPtr;

    for (file_object s : basic_files) {
        Json::Value elem;
        elem["name"] = s.get_name();
        elem["time"] = s.get_time();
        elem["auth"] = s.get_auth();
        elem["type"] = s.get_type();
        elem["size"] = std::to_string(s.get_size());
        elem["is_dir"] = std::to_string(s.get_is_dir());

        root.append(elem);
    }

    return 0;
}

lock_handler::lock_handler() {
    soft_lock = std::map<std::string, int>();
    hard_lock = std::map<std::string, int>();
}

void lock_handler::add_lock(int lock_no, std::string full_name) {
    std::map<std::string, int> *lock = nullptr;

    if (lock_no == SOFT_LOCK) {
        lock = &soft_lock;
    } else if (lock_no == HARD_LOCK) {
        lock = &hard_lock;
    }

    if ((*lock).count(full_name)) {
        int value = (*lock)[full_name];
        value++;
        (*lock)[full_name] = value;
        return;
    } else {
        (*lock)[full_name] = 1;
        return;
    }
}

void lock_handler::remove_lock(int lock_no, std::string full_name) {
    std::map<std::string, int> *lock = nullptr;

    if (lock_no == SOFT_LOCK) {
        lock = &soft_lock;
    } else if (lock_no == HARD_LOCK) {
        lock = &hard_lock;
    }

    if ((*lock).count(full_name)) {
        int value = (*lock)[full_name];

        if (value == 1) {
            (*lock).erase(full_name);
        } else {
            value--;
            (*lock)[full_name] = value;
        }
        return;
    }
}
bool lock_handler::check_lock(int lock_no, std::string full_name) {
    const std::map<std::string, int> *lock = nullptr;

    if (lock_no == SOFT_LOCK) {
        lock = &soft_lock;
    } else if (lock_no == HARD_LOCK) {
        lock = &hard_lock;
    }

    if ((*lock).count(full_name)) {
        return false;
    } else {
        return true;
    }
}
