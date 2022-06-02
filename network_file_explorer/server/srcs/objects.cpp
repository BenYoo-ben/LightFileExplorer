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

void file_object::print_members() {
    // debug
    std::cout<< "name : " << name << "\ntime : " << time <<
        "\nfile_type : " << type <<
        "\nauth :" << auth << "\nsize : " <<
        size << "\nis_dir : " << is_dir <<
        std::endl << std::endl;
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

directory_object::directory_object(std::vector<std::vector<file_object>> parsed_json, std::string directory_path) {
    this->dir = parsed_json;
    this->directory_path = directory_path;
}

std::vector<std::vector<file_object>> directory_object::get_vectors() {
    return dir;
}

std::string directory_object::get_directory_path() {
    return directory_path;
}

std::vector<file_object> json_handler::directory_to_file_object_vector
(std::string dir_name) {
    std::vector<file_object> file_objects;

    file_manager fm;
    std::vector<std::string> file_names =
        fm.files_in_directory_to_vector(dir_name);

    for (std::string s : file_names) {
        struct stat status = fm.get_stat_of_file(dir_name+"/"+s);
        file_object fo(
                s,
                fm.stat_get_time(&status),
                fm.stat_get_permission(&status),
                std::to_string(fm.stat_get_size(&status)),
                std::to_string(fm.stat_is_directory(&status)));

        file_objects.push_back(fo);
    }

    return file_objects;
}

Json::Value json_handler::make_json_object(std::string dir_name, int depth) {
    std::vector<file_object> basic_files = directory_to_file_object_vector(dir_name);

    if (depth == 2) {
        std::vector<std::vector<file_object>> data;

        for (file_object s : basic_files) {
            std::vector<file_object> elem;

            if (s.get_is_dir()) {
                elem.push_back(s);
                std::vector<file_object> files_in_directory = directory_to_file_object_vector(dir_name + s.get_name());
                elem.insert(elem.end(),
                        files_in_directory.begin(),
                        files_in_directory.end());
            } else {
                elem.push_back(s);
            }

            data.push_back(elem);
            elem.clear();
        }
        std::cout << data.size() <<std::endl;
        Json::Value root(Json::arrayValue);

        for (std::vector<file_object> inner_vector : data) {
            if (inner_vector.size() > 1) {
                Json::Value content(Json::arrayValue);

                for (file_object fo : inner_vector) {
                    Json::Value elem;

                    elem["name"] = fo.get_name();
                    elem["time"] = fo.get_time();
                    elem["auth"] = fo.get_auth();
                    elem["type"] = fo.get_type();
                    elem["size"] = std::to_string(fo.get_size());
                    elem["is_dir"] = std::to_string(fo.get_is_dir());

                    content.append(elem);
                }

                root.append(content);
            } else {
                Json::Value elem;

                file_object fo = inner_vector.at(0);
                elem["name"] = fo.get_name();
                elem["time"] = fo.get_time();
                elem["auth"] = fo.get_auth();
                elem["type"] = fo.get_type();
                elem["size"] = std::to_string(fo.get_size());
                elem["is_dir"] = std::to_string(fo.get_is_dir());

                root.append(elem);
            }
        }

        return root;
    } else if (depth == 1) {
        Json::Value root(Json::arrayValue);

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

        return root;
    } else {
        perror("Unknown depth ! \n");
        return Json::Value(Json::arrayValue);
    }
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

void lock_handler::debug_lock() {
    std::map<std::string, int>::iterator it;

    std::cout << " < SOFT > " << std::endl;
    for (it = soft_lock.begin(); it != soft_lock.end(); it++) {
        std::cout
            << it->first    // string (key)
            << ':'
            << it->second   // string's value
            << std::endl;
    }
    std::map<std::string, int>::iterator it2;

    std::cout << " < HARD > " << std::endl;
    for (it2 = hard_lock.begin(); it2 != hard_lock.end(); it2++) {
        std::cout
            << it2->first    // string (key)
            << ':'
            << it2->second   // string's value
            << std::endl;
    }
}
