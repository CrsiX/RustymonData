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

#include "rustymon_constants.hpp"
#include "rustymon_enums.hpp"
#include "queue.hpp"
#include "generator_helpers.hpp"
#include "generator.hpp"


namespace rustymon {

    using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>;
    using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

    void generate_world(std::string in_file, std::string out_file, osmium::Box bbox, std::string config_file) {
        const osmium::io::File input_file{in_file};

        osmium::area::Assembler::config_type assembler_config;
        assembler_config.create_empty_areas = false;

        osmium::area::MultipolygonManager <osmium::area::Assembler> mp_manager{assembler_config};

        osmium::relations::read_relations(input_file, mp_manager);
        index_type index;

        location_handler_type location_handler{index};
        location_handler.ignore_errors();

        WorldGenerator data_handler(bbox, config_file);
        data_handler.start_workers();
        osmium::io::Reader reader{input_file, osmium::io::read_meta::no};

        osmium::apply(reader, location_handler, data_handler,
                      mp_manager.handler([&data_handler](const osmium::memory::Buffer &area_buffer) {
                          osmium::apply(area_buffer, data_handler);
                      }));

        reader.close();

        static Json::StreamWriterBuilder builder;
        builder["commentStyle"] = "None";
        builder["indentation"] = "  ";
        builder["enableYAMLCompatibility"] = true;

        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        std::ofstream output_file_stream(out_file);
        writer->write(data_handler.get_json_data(), &output_file_stream);
        output_file_stream.close();

    }

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

    std::string generic_usage = "Usage: " + std::string(argv[0]) + " {http,file} [Options...]";
    if (argc < 2) {
        std::cerr << generic_usage << std::endl;
        return 2;
    } else if (strcmp(argv[1], "http") == 0) {
        std::string usage = "Usage: " + std::string(argv[0]) + " http <InputFile> <GetBoxesURL> <PushResultURL> <AuthorizationInfo> [<ConfigFile>]";
        std::string config_file = rustymon::DEFAULT_CONFIG_FILENAME;
        if (argc == 7) {
            config_file = argv[6];
        } else if (argc != 6) {
            std::cerr << usage << std::endl;
            return 2;
        }
        std::cout << "HTTP mode is not implemented yet." << std::endl;
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
        rustymon::generate_world(argv[2], argv[3], bbox, config_file);
        return 0;
    } else {
        std::cerr << generic_usage << std::endl;
        return 2;
    }
}
