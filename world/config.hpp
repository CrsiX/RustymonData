#ifndef WORLD_GENERATOR_CONFIG_HPP
#define WORLD_GENERATOR_CONFIG_HPP

#include <map>
#include <string>
#include <vector>

namespace rustymon {

    namespace config {

        struct Workers {
            int node;
            int way;
            int area;
            int upload;
        };

        struct Size {
            int x;
            int y;
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
            Workers workers;
            Size size;
            std::vector<POI> poi;
            std::vector<Street> streets;
            std::vector<Area> areas;
        };

    }

}

#endif //WORLD_GENERATOR_CONFIG_HPP
