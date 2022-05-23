#ifndef CLIENT_INCLUDES_GUI_HPP_
#define CLIENT_INCLUDES_GUI_HPP_

#include <string>

#include <nana/gui.hpp>
#include <nana/gui/widgets/toolbar.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/button.hpp>

#include "objects.hpp"
#include "nets.hpp"
#include "utils.hpp"
#include "global.hpp"
#include "common.hpp"

// definition of available actions in on files
#define TOOL_DOWNLOAD   0
#define TOOL_COPY       1
#define TOOL_MOVE       2
#define TOOL_PASTE      3
#define TOOL_RENAME     4
#define TOOL_DELETE     5
#define TOOL_UPLOAD     6

class nana_form : public nana::form {
 private:
    // GUI components
    nana::treebox treeview{(*this), true};
    nana::listbox listview{(*this), true};
    nana::toolbar toolsview{(*this), true};
    nana::label pathview{(*this), ""};

    // handle tcp connections
    client_object co;

    // parse json data
    parser p;

    // current working directory
    std::string current_directory;

    // root path where we began
    std::string root_path;

    // images (assets
    nana::paint::image dir_image;
    nana::paint::image file_image;

    // control variables, control and '..' directory
    bool control_clicked = false;
    bool selected_dotdot = false;

    // control variables
    std::string prev_updated;
    std::string selected_item;
    std::string clipboard_item;
    int clipboard_action = 0;


 public:
    // basic settings for GUI
    void set_form(std::string dir_path, client_object co, parser p);

    // start GUI
    void form_loop_start();

    // insert item to treeview
    int insert_node_to_treeview(std::string file_name
     , std::string directory_name, std::string full_name);

    // insert item to listview
    int insert_item_to_listview(const file_object & fo, std::string file_type);

    // update views with new received data(depth 2)
    void update_views_depth_2(directory_object d_o);

    // update views with new received data(depth 1)
    void update_views_depth_1(std::vector<file_object> files, std::string path);

    // update views with new rewceived data.
    // This calls update_views(directory_object) internally
    void update_views(std::string dir_name);

    // add events to GUI components(click, key, etc)
    void add_events();

    // get path of item
    std::string get_path(nana::treebox::item_proxy ip);

    // intializer(constructor)
    nana_form(std::string dir_path, client_object co, parser p);

    // set icons for listview
    void set_icons_listview();

    // shows msg box for action
    bool make_and_show_msg_box_on_toolview_action(std::string question);

    // handle actions
    void actions(int action_type);
};

#endif  // CLIENT_INCLUDES_GUI_HPP_
