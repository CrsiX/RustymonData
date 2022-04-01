#include <chrono>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/writer.h>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_manager.hpp>
#include <osmium/geom/factory.hpp>
#include <osmium/geom/geojson.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/index/map/flex_mem.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/area.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/relations/relations_manager.hpp>

#include "poke_world_enums.hpp"
#include "poke_world_fields.hpp"


static const int FILE_VERSION = 1;

using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>;
using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;


class CounterHandler : public osmium::handler::Handler {
public:
    int nodes = 0;
    int ways = 0;
    int areas = 0;
    int relations = 0;

    void node(const osmium::Node& node) {
        this->nodes++;
    }

    void way(const osmium::Way& way) {
        this->ways++;
    }

    void area(const osmium::Area& area) {
        this->areas++;
    }

    void relation(const osmium::Relation& relation) {
        this->relations++;
    }
};


class WorldGenerator : public osmium::handler::Handler {

public:

    Json::Value get_json_data() {
        Json::Value root;
        root["uuid"] = "uuid";
        root["bbox"] = "bbox";
        root["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        root["version"] = FILE_VERSION;
        root["streets"] = "streets";
        root["poi"] = "poi";
        root["areas"] = "areas";
        return root;
    }
};


void generate_world(const char *in_file, const char *out_file, const osmium::Box bbox) {
    const osmium::io::File input_file{in_file};

    osmium::area::Assembler::config_type assembler_config;
    assembler_config.create_empty_areas = false;

    osmium::area::MultipolygonManager<osmium::area::Assembler> mp_manager{assembler_config};

    osmium::relations::read_relations(input_file, mp_manager);
    index_type index;

    location_handler_type location_handler{index};
    location_handler.ignore_errors();

    WorldGenerator data_handler;
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

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <InputFile> <OutputFile> <BoundingBox>" << std::endl;
        return 2;
    }

    osmium::Box bbox;
    generate_world(argv[1], argv[2], bbox);
    return 0;
}
