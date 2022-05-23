#include "main.hpp"
#include "utils.hpp"
#include "common.hpp"
#include "objects.hpp"
#include "gui.hpp"
#include "nets.hpp"
#include "global.hpp"

int main(int argc, char *argv[]) {
    // root directory where client is going to stay
    std::string dir_path = std::string("/");

    // GUI Mode(normal operation)
    if (argc == 1) {
        // handle socket operation and json parsing
        client_object co;
        parser p;

        // intialize and try connection to server
        co.client_socket_init();
        co.client_socket_connect(global_server_address, global_server_port);

        // request root directroy data from server
        co.send_data(0, dir_path, "");

        // vector to store data
        std::vector<char> data_vector;

        // store received data into the vector
        int ret = co.recv_data(data_vector);

        if (ret < 0) {
            // if something went wrong
            co.handle_error(ret);
        } else {
            // vector to string
            std::string data(data_vector.begin(), data_vector.end());

            // debug
            // std::cout << data.length()<< std::endl;

            // parse json data to vector of files
            // auto parsed_json = p.parse_json_to_list_vectors(data);

            // start GUI
            nana_form gui_panel(dir_path, co, p);

            // update view
            gui_panel.update_views(dir_path);

            // start GUI
            gui_panel.form_loop_start();
        }
    // main thread blocked.
    } else if (argc == 4) {
        // CLI Mode(testing connection and data send/reecive)
        std::stringstream num_of_process_str(argv[1]);
        std::stringstream timeout_duration_str(argv[2]);
        std::stringstream continue_count_str(argv[3]);

        int timeout_duration;
        int num_of_process;
        int continue_count;

        int num_process_count = 0;

        timeout_duration_str >> timeout_duration;
        num_of_process_str >> num_of_process;
        continue_count_str >> continue_count;

        printf("PROC : %d\nTIMEOUT : %d\nLOOP: %d\n",
         num_of_process, timeout_duration, continue_count);

        int pid_status;
        while (num_process_count <  num_of_process) {
            int pid = fork();
            if (pid < 0) {
                break;
            }

            if (pid == 0) {
                int i = 0;
                client_object co;

                co.client_socket_init();
                co.client_socket_connect
                 (global_server_address, global_server_port);

                while (i < continue_count) {
                    sleep(timeout_duration);
                    co.send_data(0, dir_path, "");

                    std::vector<char> data_vector;
                    int ret = co.recv_data();

                    if (ret < 0) {
                        co.handle_error(ret);
                    } else {
                        std::string data(data_vector.begin(),
                         data_vector.end());
                        i++;
                    }
                }
                co.close_socket();
                exit(1);
            } else if (pid > 0) {
                num_process_count++;
            }
        }
    }
}  // main
