#include "common.hpp"
#include "objects.hpp"
#include "utils.hpp"

std::vector<std::string> parser::get_splitted_path
    (std::string input_string, char delim) {
    std::string line;
    std::vector<std::string> vect;
    std::stringstream ss(input_string);

    while (std::getline(ss, line, delim)) {
        vect.push_back(line);
    }
    return vect;
}

std::string parser::get_directory(std::string input_string, char delim) {
    std::string dir_string;
    int string_length = input_string.length();

    int i, encounter_count = 0;
    for (i = string_length; i >= 0 && encounter_count < 2 ; i--) {
        if (input_string[i] == delim) {
            encounter_count++;
        }
    }
    return input_string.substr(0, i+2);
}

// parse json of depth 2
std::vector<std::vector<file_object>>
parser::parse_json_to_list_vectors(std::string input_json) {
    Json::Value root;
    Json::Reader reader;

    reader.parse(input_json, root);

    std::vector<std::vector<file_object>> list_of_vectors;

    for (Json::Value::ArrayIndex i = 0; i != root.size(); i++) {
        std::vector<file_object> dir;

        if (root[i].isArray()) {
            for (Json::Value::ArrayIndex j=0; j != root[i].size(); j++) {
                file_object fo(root[i][j]["name"].asString(),
                                root[i][j]["time"].asString(),
                                root[i][j]["auth"].asString(),
                                root[i][j]["size"].asString(),
                                root[i][j]["is_dir"].asString());

                dir.push_back(fo);
            }
        } else {
            file_object fo(root[i]["name"].asString(),
                            root[i]["time"].asString(),
                            root[i]["auth"].asString(),
                            root[i]["size"].asString(),
                            root[i]["is_dir"].asString());

            dir.push_back(fo);
        }

        list_of_vectors.push_back(dir);
    }
    return list_of_vectors;
}

// parse json of depth 1
std::vector<file_object> parser::parse_json_to_file_vectors(std::string input_json) {
    Json::Value root;
    Json::Reader reader;

    reader.parse(input_json, root);

    std::vector<file_object> file_vector;

    for (Json::Value::ArrayIndex i = 0; i != root.size(); i++) {
        file_object fo(root[i]["name"].asString(),
                root[i]["time"].asString(),
                root[i]["auth"].asString(),
                root[i]["size"].asString(),
                root[i]["is_dir"].asString());

        file_vector.push_back(fo);
    }
    return file_vector;
}
