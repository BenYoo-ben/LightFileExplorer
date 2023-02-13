#ifndef SERVER_INCLUDES_OBJECTS_HPP_
#define SERVER_INCLUDES_OBJECTS_HPP_

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

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
    file_object() = default;
    file_object(std::string name, std::string time,
            std::string auth, std::string size, std::string is_dir);
   
    file_object(const file_object&) = default;
    file_object& operator=(const file_object&) = default;

    file_object(file_object&&) = default;
    file_object& operator=(file_object&&) = default;

    ~file_object() = default;

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
    directory_object() = default;
    directory_object(std::vector<std::vector<file_object>> parsed_json,
            std::string directory_path);

    directory_object(const directory_object&) = default;
    directory_object& operator=(const directory_object&) = default;

    directory_object(directory_object&&) = default;
    directory_object& operator=(directory_object&&) = default;

    ~directory_object() = default;

    int get_vectors(std::vector<std::vector<file_object>> *vvfObjPtr);

    std::string get_directory_path();
};

class json_handler {
private:
public:
    json_handler() = default;
    
    json_handler(const json_handler&) = default;
    json_handler& operator=(const json_handler&) = default;

    json_handler(json_handler&&) = default;
    json_handler& operator=(json_handler&&) = default;

    ~json_handler() = default;

    int make_json_object(std::string dir_name, Json::Value *jsv);

    int directory_to_file_object_vector(std::string dir_name, std::vector<file_object> *vfObjPtr);
};

class file_lock {
private:
    int num;
    std::string name;

public:
    file_lock() = default;

    file_lock(int num, std::string name) {
        this->num = num;
        this->name = name;
    }

    file_lock(int&& num, std::string&& name) {
        this->num = num;
        this->name = name;
    }

    file_lock(const file_lock&) = default;
    file_lock& operator=(const file_lock&) = default;

    file_lock(file_lock&&) = default;
    file_lock& operator=(file_lock&&) = default;

    ~file_lock() = default;

    void set_name(std::string&& name) {
        this->name = name;
    }

    void set_num(int&& num) {
        this->num = num;
    }
    
    std::string get_name() {
        return name;
    }

    int get_num() {
        return num;
    }

    bool operator==(const file_lock& other) const {
        return ((this->name == other.name) &&
                this->num == other.num);
    }
};

template <>
struct std::hash<file_lock> {
    std::size_t operator()(const file_lock& l) const
    {
        size_t ret = 17;
        ret = ret * 31 + std::hash<std::string>()(const_cast<file_lock&>(l).get_name());
        ret = ret * 31 + std::hash<int>()(const_cast<file_lock&>(l).get_num());
        return ret;
    }
};

class lock_handler {
private:
    std::unordered_map<file_lock, bool> locks;
    lock_handler() = default;

    lock_handler(const lock_handler&) = default;
    lock_handler& operator=(const lock_handler&) = default;

    lock_handler(lock_handler&&) = default;
    lock_handler& operator=(lock_handler&&) = default;

    ~lock_handler() = default;

public:
    static lock_handler& get_instance() {
        static lock_handler inst;
        return inst;
    } 
    static constexpr int READ = 0;
    static constexpr int WRITE = 1;
    bool add_lock(file_lock f_lock);
    bool remove_lock(file_lock f_Lock);
    bool check_lock(file_lock f_lock);
    void debug_lock();
};

class session_lock {
private:
    std::stack<file_lock> pocket;

public:
    session_lock() {
        pocket = std::stack<file_lock>();
    }

    session_lock(const session_lock& other) {
        this->pocket = std::stack<file_lock>();
        this->pocket = other.pocket;
    }

    session_lock& operator=(const session_lock& other) {
        if (this != &other) {
            this->pocket = std::stack<file_lock>();
            this->pocket = other.pocket;
        }
        return *this;
    }

    session_lock(session_lock&&) = default;
    session_lock& operator=(session_lock&&) = default;

    ~session_lock() {
        while (pocket.empty() == false) {
            auto curLock = pocket.top();
            pocket.pop();
            lock_handler::get_instance().remove_lock(curLock);
        }
    }

    bool add_lock(file_lock f_lock);
};

template <class T>
class TSQueue {
private:
    std::queue<T> q;
    std::mutex* m;
    std::condition_variable* c;

public:
    TSQueue() {
        m = new std::mutex();
        c = new std::condition_variable();
    }

    ~TSQueue() {
        delete(c);
        delete(m);
    }

    void push(T t) {
        std::lock_guard<std::mutex> raii_lock(*m);
        q.push(t);
        c->notify_one();
    }

    T pop() {
        std::lock_guard<std::mutex> raii_lock(*m);
        T data = q.front();
        q.pop();
        c->notify_one();
        return data;
    }

    std::size_t size() {
        std::lock_guard<std::mutex> raii_lock(*m);
        return q.size(); 
    }
};

#endif  // SERVER_INCLUDES_OBJECTS_HPP_
