#include "file_scouter.hpp"

std::vector<std::string> file_manager::files_in_directory_to_vector
    (std::string string_dir_name) {
    const char *dir_name = string_dir_name.c_str();

    std::vector<std::string> file_names;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dir_name)) != NULL) {
  // print all the files and directories within directory
        while ((ent = readdir(dir)) != NULL) {
            // printf("FILE[%s] : %s\n",string_dir_name.c_str(),ent->d_name);
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                file_names.push_back(std::string(ent->d_name));
            }
        }
        closedir(dir);
    } else {
  // could not open directory
        perror(":ERROR");
    }
    return file_names;
}

struct stat file_manager::get_stat_of_file(std::string file_name) {
    const char *file_name_in_char_array = file_name.c_str();

    struct stat status;
    lstat(file_name_in_char_array, &status);
    return status;
}

std::string file_manager::stat_get_type(struct stat *st) {
    if (S_ISDIR(st->st_mode)) {
        return std::string("Directory");
    } else if (S_ISREG(st->st_mode)) {
        return std::string("File");
    } else if (S_ISSOCK(st->st_mode)) {
        return std::string("Socket");
    } else if (S_ISCHR(st->st_mode)) {
        return std::string("Char Device");
    } else if (S_ISFIFO(st->st_mode)) {
        return std::string("FIFO");
    } else if (S_ISBLK(st->st_mode)) {
        return std::string("Block Device");
    } else if (S_ISLNK(st->st_mode)) {
        return std::string("Link");
    }
    return NULL;
}

bool file_manager::stat_is_directory(struct stat *st) {
    if (S_ISDIR(st->st_mode)) {
        return true;
    } else {
        return false;
    }
}

std::string file_manager::stat_get_time(struct stat *st) {
    struct tm *mtime;
    // Thread Safe (localtime --> localtime_r)
    mtime = (struct tm *)malloc(sizeof(struct tm));

    localtime_r(&st->st_mtime, mtime);

    char str_buffer[256];

    snprintf(str_buffer, sizeof(str_buffer), "%04d-%02d-%02d %02d:%02d",
        mtime->tm_year+1900,
        mtime->tm_mon+1,
        mtime->tm_mday,
        mtime->tm_hour,
        mtime->tm_min);

    free(mtime);
    return std::string(str_buffer);
}

int file_manager::stat_get_size(struct stat *st) {
    return st->st_size;
}

std::string file_manager::stat_get_permission(struct stat *st) {
    char str_buffer[10];

    mode_t permission_mode = st->st_mode;

    str_buffer[0] = (permission_mode & S_IRUSR) ? 'r' : '-';
    str_buffer[1] = (permission_mode & S_IWUSR) ? 'w' : '-';
    str_buffer[2] = (permission_mode & S_IXUSR) ? 'x' : '-';
    str_buffer[3] = (permission_mode & S_IRGRP) ? 'r' : '-';
    str_buffer[4] = (permission_mode & S_IWGRP) ? 'w' : '-';
    str_buffer[5] = (permission_mode & S_IXGRP) ? 'x' : '-';
    str_buffer[6] = (permission_mode & S_IROTH) ? 'r' : '-';
    str_buffer[7] = (permission_mode & S_IWOTH) ? 'w' : '-';
    str_buffer[8] = (permission_mode & S_IXOTH) ? 'x' : '-';
    str_buffer[9] = '\0';

    return std::string(str_buffer);
}