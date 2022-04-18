#ifndef WORLD_GENERATOR_EXPORTER_HPP
#define WORLD_GENERATOR_EXPORTER_HPP

#include <string>
#include <fstream>
#include <iostream>

#include <cpr/api.h>

#include "structs.hpp"

namespace rustymon {

    void export_world_to_file(const structs::World &world, const std::string &filename, std::ostream &logger = std::cout);

    void export_world_to_files(const structs::World &world, const std::string &directory, std::ostream &logger = std::cout);

    void export_world_to_http(const structs::World &world, const std::string &push_url, const std::string &auth_info = "", std::ostream &logger = std::cout);

}

#endif //WORLD_GENERATOR_EXPORTER_HPP
