#include "gui.hpp"
#include "objects.hpp"

nana_form::nana_form(std::string dir_path, client_object co, parser p)
    : nana::form(nana::API::make_center(1200, 800), nana::appearance {}) {
        set_form(dir_path, co, p);
    }

void nana_form::set_form(std::string dir_path, client_object co, parser p) {
    current_directory = std::string(dir_path);
    root_path = std::string(dir_path);
    selected_item = std::string();
    prev_updated = std::string();

    // set window name
    caption("Network File Explorer");

    // set gui orientation
    div("vertical"
            "<weight=33 toolsview>"
            "<weight=23 pathview>"
            "<<treeview> | 70% <listview>>");

    listview.append_header("name");
    listview.append_header("modified_date");
    listview.append_header("size");
    listview.append_header("type");
    listview.append_header("permission");

    // toolsview.append();
    (*this)["pathview"] << pathview;
    (*this)["toolsview"] << toolsview;
    (*this)["treeview"] << treeview;
    (*this)["listview"] << listview;

    treeview.insert("/", "/");

    this->co = co;
    this->p = p;

    dir_image = nana::paint::image(std::string("assets/directory.png"));
    file_image = nana::paint::image(std::string("assets/file.png"));

    nana::treebox::node_image_type & img = treeview.icon("A");
    img.normal.open("assets/directory.png");

    toolsview.scale(20);
    toolsview.append("download", nana::paint::image("assets/download_s.png"));
    toolsview.append("copy", nana::paint::image("assets/paper-clip1.png"));
    toolsview.append("move", nana::paint::image("assets/export1.png"));
    toolsview.append("paste", nana::paint::image("assets/push-pin1.png"));
    toolsview.append("rename", nana::paint::image("assets/edit1.png"));
    toolsview.append("delete", nana::paint::image("assets/dustbin1.png"));

    add_events();
}

void nana_form::add_events() {
    treeview.events().selected([this](const nana::arg_treebox& arg) {
        if (!arg.operated) return;

        current_directory = get_path(arg.item);

        if (current_directory != prev_updated) {
            printf("selected!\n");
            update_views(this->current_directory);
        }
    });

    treeview.events().expanded([this](const nana::arg_treebox& arg) {
        if (!arg.operated) {
            return;
        }

        current_directory = get_path(arg.item);
        if (current_directory != prev_updated)  {
            printf("expanded! \n");
            update_views(this->current_directory);
        }
    });

    listview.events().key_press([this](const nana::arg_keyboard& arg) {
        // control
        if (arg.key == 17) {
            control_clicked = true;
        } else {
            if (control_clicked) {
                if (arg.key == 67) {  // 'C'
                    actions(TOOL_COPY);
                } else if (arg.key == 68) {  // 'D'
                    actions(TOOL_DOWNLOAD);
                } else if (arg.key == 88) {  // 'X'
                    actions(TOOL_MOVE);
                } else if (arg.key == 86) {  // 'V'
                    actions(TOOL_PASTE);
                } else if (arg.key == 127) {  // DEL
                    actions(TOOL_DELETE);
                }
            }
            control_clicked = false;
        }
    });

    listview.events().mouse_down([this](const nana::arg_mouse& arg) {
        control_clicked = false;
    });

    listview.events().dbl_click([this](const nana::arg_mouse& arg) {
         if (selected_dotdot) {
            this->current_directory =
             this->p.get_directory(current_directory, '/');

            update_views(this->current_directory);
            selected_item = "";
        } else {
            if (selected_item[selected_item.length()-1] == '/') {
                this->current_directory =
                 this->current_directory + this->selected_item;

                update_views(this->current_directory);
                selected_item = "";
            }
        }
        std::cout << this->current_directory << std::endl;
    });

    listview.events().selected([this](const nana::arg_listbox& arg) {
        if (arg.item.text(3) == "Directory") {
            if (arg.item.text(0) == "..") {
                selected_dotdot = true;
            } else {
            this->selected_item = arg.item.text(0) +"/";
            selected_dotdot = false;
            }
        } else {
            this->selected_item = arg.item.text(0);
            selected_dotdot = false;
        }
         std::cout << this->selected_item << std::endl;
    });

    toolsview.events().selected([this](const nana::arg_toolbar& arg) {
        actions(arg.button);
    });
}

void nana_form::form_loop_start() {
    collocate();
    show();
    nana::exec();
}

int nana_form::insert_node_to_treeview(
     std::string file_name, std::string directory_name, std::string full_name) {
    treeview.insert(full_name, file_name).icon("A");
    return 0;
}

int nana_form::insert_item_to_listview
     (const file_object & fo, std::string file_type) {
    file_object fileObj = fo;
    try {
        auto cat = listview.at(0);

        fileObj.set_type(file_type);
        cat.append({fileObj.get_name(),
                    fileObj.get_time(),
                    std::to_string(fileObj.get_size()),
                    fileObj.get_type(),
                    fileObj.get_auth()});

        return 0;
    } catch(std::out_of_range) {
        std::cout << "Out of Range 1" <<std::endl;
    }
}

void nana_form::update_views_depth_2(directory_object d_o) {
    // clear listview
    listview.clear();
    if (d_o.get_directory_path() != root_path) {
        file_object dotdot_file("..", "", "" , "0", "1");
        insert_item_to_listview(dotdot_file, "Directory");
    }
    // get vector of json
    std::vector<std::vector<file_object>> json_vector = d_o.get_vectors();

    bool checked_directory = false;
    std::string parent_directory_name;

    for (std::vector<file_object> d1_node : json_vector) {
        checked_directory = false;
        if (d1_node.size() > 1) {
            // means directory
            for (file_object fo : d1_node) {
                if (!checked_directory) {
                    parent_directory_name = fo.get_name();
                    if (fo.get_is_dir()) {
                        insert_node_to_treeview(fo.get_name(),
                         d_o.get_directory_path(),
                         d_o.get_directory_path() + fo.get_name());
                    }

                    insert_item_to_listview(fo, "Directory");
                    checked_directory = true;
                } else {
                    if (fo.get_is_dir()) {
                        std::string directory_path_concated =
                         d_o.get_directory_path() + parent_directory_name + "/";

                        insert_node_to_treeview(fo.get_name(),
                         directory_path_concated,
                         directory_path_concated + fo.get_name());
                        // insert_item_to_listview(fo);
                    }
                }
            }
        } else {
            // means just a file
            file_object fo = d1_node[0];
            if (fo.get_is_dir()) {
                insert_item_to_listview(fo, "Directory");
            } else {
                insert_item_to_listview(fo, "File");
            }
        }
    }

    set_icons_listview();
    nana::treebox::item_proxy tree_item = treeview.find(d_o.get_directory_path());
    // tree_item.select(true);
    // tree_item.expand(true);
    pathview.caption(current_directory);
}

void nana_form::update_views_depth_1(std::vector<file_object> files, std::string path) {
    // clear listview
    listview.clear();
    if (path != root_path) {
        file_object dotdot_file("..", "", "" , "0", "1");
        insert_item_to_listview(dotdot_file, "Directory");
    }
    // get vector of json

    treeview.erase(path + ".");

    bool checked_directory = false;
    std::string parent_directory_name;

    for (file_object f : files) {
        // means just a file
        if (f.get_is_dir()) {
            insert_item_to_listview(f, "Directory");
            insert_node_to_treeview(f.get_name(), path, path + f.get_name());
            insert_node_to_treeview(".", path + "/" + f.get_name() + "/" , path + "/" + f.get_name() + "/.");

        } else {
            insert_item_to_listview(f, "File");
        }
    }

    set_icons_listview();
    nana::treebox::item_proxy tree_item = treeview.find(path);
    // tree_item.select(true);
    // tree_item.expand(true);
    pathview.caption(current_directory);
}
void nana_form::update_views(std::string dir_name) {
    try {
        printf("update view! of %s\n", current_directory.c_str());
        prev_updated = dir_name;

        this->co.send_data(REQ_TYPE_DIR_INFO_DEPTH_1, dir_name, "");

        std::vector<char> data_vector;
        int ret = this->co.recv_data(data_vector);
        printf("ret : %d\n", ret);

        if (ret < 0) {
            this->co.handle_error(ret);
        } else {
            if (!data_vector.empty()) {
                std::string data(data_vector.begin(), data_vector.end());

                auto parsed_json = this->p.parse_json_to_file_vectors(data);

                update_views_depth_1(parsed_json, dir_name);
            } else {
                // show msg that recv data failed
                std::cout << "recv data failed." << std::endl;
            }
        }
    } catch(std::out_of_range) {
        std::cout << " update views out of range exception " << std::endl;
    }
}

std::string nana_form::get_path(nana::treebox::item_proxy ip) {
    std::string ret;

    while (!ip.empty()) {
        ret = ip.key() +"/"+ ret;
        ip = ip.owner();
    }
    return ret;
}

void nana_form::set_icons_listview() {
    try {
        nana::listbox::item_proxy ip = listview.at(0).at(0);

        while (!ip.empty()) {
            if (ip.text(3) == "Directory") {
                ip.icon(dir_image);
            } else {
                ip.icon(file_image);
            }
            ip++;
        }
    } catch(std::out_of_range) {
        std::cout << "std out of range 2 "<< std::endl;
    }
}

bool nana_form::make_and_show_msg_box_on_toolview_action
     (std::string question) {
    nana::msgbox mb {(*this), "Confirm", nana::msgbox::yes_no};

    mb << question;

    // arg.cancel = (mb.show() != nana::msgbox::pick_no);

    if (mb.show() == nana::msgbox::pick_yes) {
        return true;
    } else {
        return false;
    }
}

void nana_form::actions(int action_type) {
    switch (action_type) {
        case TOOL_DOWNLOAD: {
            std::stringstream ss;
            ss << "Download " << this->selected_item << " ?";
            bool choice = make_and_show_msg_box_on_toolview_action(ss.str());
            if (choice) {
                 this->co.send_data(REQ_TYPE_DOWNLOAD_FILE,
                  this->current_directory, this->selected_item);

                 std::vector<char> data_vector;
                 int ret = this->co.recv_data(data_vector);
                     if (ret < 0) {
                         this->co.handle_error(ret);
                     } else {
                         if (!data_vector.empty()) {
                             std::string data(data_vector.begin(),
                              data_vector.end());

                             ss.str("");
                             ss << "downloaded/" << this->selected_item;
                             int fd = ::open(ss.str().c_str(),
                              O_CREAT | O_TRUNC | O_WRONLY, 0644);

                             uint32_t data_length = data.length();

                             ::write(fd, data.c_str(), data_length);

                             ::close(fd);

                             std::cout << "Download Complete for :"
                              << this->selected_item << std::endl;

                             update_views(this->current_directory);
                         } else {
                             // show alert msg
                             std::cout << "RECV FAIL " << std::endl;
                         }
                     }
                 }
                 break;
        }

        case TOOL_COPY: {
            std::stringstream ss_question;
            ss_question << "Copy " << this->selected_item << " ?";
            bool choice = make_and_show_msg_box_on_toolview_action
             (ss_question.str());

            if (choice) {
                clipboard_action = TOOL_COPY;
                std::stringstream ss_clipboard;
                ss_clipboard << this->current_directory << this->selected_item;
                this->clipboard_item = ss_clipboard.str();
            }
            break;
        }

        case TOOL_MOVE: {
            std::stringstream ss_question;
            ss_question << "Move " << this->selected_item << " ?";
            bool choice = make_and_show_msg_box_on_toolview_action
             (ss_question.str());

            if (choice) {
                clipboard_action = TOOL_MOVE;
                std::stringstream ss_clipboard;
                ss_clipboard << this->current_directory << this->selected_item;
                this->clipboard_item = ss_clipboard.str();
            }
            break;
        }

        case TOOL_PASTE: {
             std::stringstream ss_question;
             ss_question << "Paste "
              << this->clipboard_item << " ?";

             bool choice = make_and_show_msg_box_on_toolview_action
              (ss_question.str());

             if (choice) {
                 if (clipboard_action == TOOL_COPY) {
                     this->co.send_data(REQ_TYPE_COPY_FILE,
                      this->clipboard_item, this->current_directory);

                 } else if (clipboard_action == TOOL_MOVE) {
                     this->co.send_data(REQ_TYPE_MOVE_FILE,
                      this->clipboard_item, this->current_directory);
                 }
             }
             this->co.recv_data();  // ack
             update_views(this->current_directory);
             break;
        }

        case TOOL_RENAME: {
              nana::inputbox::text name("<bold blue>New Name</>", "");

              nana::inputbox inputb {(*this), "Enter new name for " +
               selected_item + " :" , "Rename Box"};
              std::string new_name;
              if (inputb.show_modal(name)) {
                  printf("OK\n");
                  new_name = name.value();  // std::string in UTF-8
                  std::cout << (this->current_directory + "/" + this->selected_item) << std::endl;

                  this->co.send_data(REQ_TYPE_RENAME_FILE, this->current_directory + this->selected_item, this->current_directory + new_name);

                  this->co.recv_data();  // ack
                  update_views(this->current_directory);
              } else {
                  printf("FALSE");
              }
              break;
        }

        case TOOL_DELETE: {
                  std::stringstream ss_question;
                  ss_question << "Delete" << this->selected_item
                      << "? \n This action can't be undone." << std::endl;

                  bool choice = make_and_show_msg_box_on_toolview_action
                   (ss_question.str());

                  if (choice) {
                      this->co.send_data(REQ_TYPE_DELETE_FILE,
                       current_directory, this->selected_item);

                      this->co.recv_data();  // ack
                      update_views(current_directory);
                  }
                  break;
        }
    }
}  // nana_form::actions
