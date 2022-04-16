#include <chrono>
#include <fstream>
#include <iostream>

#include <jsoncpp/json/json.h>
#include <jsoncpp/json/writer.h>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_manager.hpp>
#include <osmium/geom/coordinates.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/index/map/flex_mem.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/osm/area.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/relations/relations_manager.hpp>

#include "structs.hpp"
#include "rustymon_constants.hpp"
#include "rustymon_enums.hpp"
#include "queue.hpp"
#include "generator_helpers.hpp"
#include "generator.hpp"
#include "exporter.hpp"




}


void print_help() {
    std::cout << "Help is not implemented yet." << std::endl;
}


int main(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
    }

    std::string generic_usage = "Usage: " + std::string(argv[0]) + " {http,file,test} [Options...]";
    if (argc < 2) {
        std::cerr << generic_usage << std::endl;
        return 2;
    } else if (strcmp(argv[1], "test") == 0) {
        std::cout << "Tests are not implemented yet." << std::endl;
        return 0;
    } else if (strcmp(argv[1], "http") == 0) {
        std::string usage = "Usage: " + std::string(argv[0]) + " http <InputFile> <PushResultURL> <AuthorizationInfo> [<ConfigFile>]";
        std::string config_file = rustymon::DEFAULT_CONFIG_FILENAME;
        if (argc == 6) {
            config_file = argv[5];
        } else if (argc != 5) {
            std::cerr << usage << std::endl;
            return 2;
        }
        std::cout << "HTTP mode is not implemented yet." << std::endl;
        rustymon::export_to_http(argv[2], argv[3], argv[4], config_file);
        return 0;
    } else if (strcmp(argv[1], "file") == 0) {
        std::string usage = "Usage: " + std::string(argv[0]) + " file <InputFile> <OutputFile> <BoundingBox> [<ConfigFile>]";
        std::string config_file = rustymon::DEFAULT_CONFIG_FILENAME;
        if (argc == 6) {
            config_file = argv[5];
        } else if (argc != 5) {
            std::cerr << usage << std::endl;
            return 2;
        }

        osmium::Box bbox = rustymon::get_bbox(argv[4]);
        std::cout << "Using bounding box " << bbox << "." << std::endl;
        rustymon::export_to_file(argv[2], argv[3], bbox, config_file);
        return 0;
    } else {
        std::cerr << generic_usage << std::endl;
        return 2;
    }
}
