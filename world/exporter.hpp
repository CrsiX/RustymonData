#ifndef WORLD_GENERATOR_EXPORTER_HPP
#define WORLD_GENERATOR_EXPORTER_HPP

#include <chrono>
#include <string>
#include <thread>
#include <fstream>
#include <iostream>

#include <cpr/api.h>

#include "constants.hpp"
#include "structs.hpp"

namespace rustymon {

    namespace detail {

        std::pair<int, int> export_world_to_http_worker(const structs::World &world, const std::string &push_url, const std::string &auth_info, std::ostream &logger, int worker_count, int my_modulo);

    }

    void export_world_to_file(const structs::World &world, const std::string &filename, std::ostream &logger = std::cout);

    void export_world_to_files(const structs::World &world, const std::string &directory, std::ostream &logger = std::cout);

    void export_world_to_http(const structs::World &world, const std::string &push_url, const std::string &auth_info = "", std::ostream &logger = std::cout, int worker_threads = UPLOAD_DEFAULT_WORKER_THREADS);

}

#endif //WORLD_GENERATOR_EXPORTER_HPP
