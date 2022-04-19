#ifndef WORLD_GENERATOR_CONFIG_HPP
#define WORLD_GENERATOR_CONFIG_HPP

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <json/json.h>

#include "constants.hpp"

namespace rustymon {

    namespace config {

        struct Workers {
            const int node;
            const int way;
            const int area;
            const int upload;
        };

        struct Size {
            const int x;
            const int y;
        };

        struct POI {
            const int type;
            const std::vector<int> spawns;
            const std::map<std::string, std::vector<std::string>> required;
            const std::map<std::string, std::vector<std::string>> forbidden;
        };

        struct Street {
            const int type;
            const std::map<std::string, std::vector<std::string>> required;
            const std::map<std::string, std::vector<std::string>> forbidden;
        };

        struct Area {
            const int type;
            const std::vector<int> spawns;
            const std::map<std::string, std::vector<std::string>> required;
            const std::map<std::string, std::vector<std::string>> forbidden;
        };

        struct Config {
            const Workers workers;
            const Size size;
            const std::vector<POI> poi;
            const std::vector<Street> streets;
            const std::vector<Area> areas;
        };

        Config load_config_from_json(const Json::Value &data);

        Config load_config_from_file(const std::string &filename);

    }

}

#endif //WORLD_GENERATOR_CONFIG_HPP
