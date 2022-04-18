#include <chrono>
#include <string>
#include <cstring>
#include <iostream>

#include "structs.hpp"
#include "constants.hpp"
#include "generator.hpp"
#include "exporter.hpp"


void print_help() {
    // TODO: Implement this method
    std::cout << "Help is not implemented yet." << std::endl;
}


int main(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
    }

    if (argc >= 2 && strcmp(argv[1], "help") == 0) {
        print_help();
        return 0;
    } else if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        std::cout << "Tests are not implemented yet." << std::endl;
        return 0;
    } else if (argc >= 2 && strcmp(argv[1], "http") == 0) {
        const std::string usage = "Usage: " + std::string(argv[0]) + " http <InputFile> <PushResultURL> [<AuthorizationInfo>] [<ConfigFile>]";
        std::string config_file = rustymon::DEFAULT_CONFIG_FILENAME;
        std::string auth_info;
        if (argc == 6) {
            auth_info = argv[4];
            config_file = argv[5];
        } else if (argc == 5) {
            auth_info = argv[4];
        } else if (argc != 4) {
            std::cerr << usage << std::endl;
            return 2;
        }

        std::cout << "HTTP mode is not implemented yet." << std::endl;
        rustymon::WorldGenerator generator(config_file);
        rustymon::reader::read_from_file(generator, argv[2]);
        rustymon::export_world_to_http(generator.get_world(), argv[3], auth_info);
        return 0;
    } else if (argc >= 2 && strcmp(argv[1], "dir") == 0) {
        // TODO: add directory support
        std::cout << "Directory support is not implemented yet." << std::endl;
        return 1;
    } else if (argc >= 2 && strcmp(argv[1], "file") == 0) {
        const std::string usage = "Usage: " + std::string(argv[0]) + " file <InputFile> <OutputFile> <BoundingBox> [<ConfigFile>]";
        std::string config_file = rustymon::DEFAULT_CONFIG_FILENAME;
        if (argc == 6) {
            config_file = argv[5];
        } else if (argc != 5) {
            std::cerr << usage << std::endl;
            return 2;
        }

        osmium::Box bbox = rustymon::helpers::get_bbox(argv[4]);
        std::cout << "Using bounding box " << bbox << "." << std::endl;
        rustymon::WorldGenerator generator(bbox, config_file);
        rustymon::reader::read_from_file(generator, argv[2]);
        rustymon::structs::World &world = generator.get_world();
        rustymon::export_world_to_file(world, argv[3]);
        return 0;
    } else {
        std::cerr << "Usage: " << std::string(argv[0]) << " {help,dir,file,http,test} [Options...]" << std::endl;
        return 2;
    }
}
