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

};


class MyManager : public osmium::relations::RelationsManager<MyManager, true, true, true> {
private:
    int node_count = 0;
    int way_count = 0;
    int relation_count = 0;
    Json::Value root;
    Json::Value nodes = Json::Value(Json::arrayValue);
    Json::Value ways = Json::Value(Json::arrayValue);
    Json::Value relations = Json::Value(Json::arrayValue);

    /// Determine whether a given relation should be ignored in the Poke World
    bool ignore_relation(const osmium::Relation& relation) {
        if (relation.tags().has_key("type")) {
            std::string type = (std::string) relation.tags().get_value_by_key("type");
            for (std::string s : IGNORED_RELATION_TYPES) {
                if (type == s) {
                    return true;
                }
            }
        } else {
            std::cerr << "Relation " << relation.id() << " has no 'type' tag." << std::endl;
        }
        return false;
    }

    /// Print an OSM object and its tags to stdout
    void print_object(const osmium::OSMObject& object) {
        std::cout << "Object " << object.id() << std::endl;
        for (auto &t : object.tags()) {
            std::cout << "  " << t.key() << "=" << t.value() << std::endl;
        }
    }

public:
    Json::Value get_root() {
        this->root["nodes"] = nodes;
        this->root["ways"] = ways;
        this->root["relations"] = relations;
        return this->root;
    }

    bool new_relation(const osmium::Relation& relation) {
        return true;
    }

    bool new_member(const osmium::Relation& relation, const osmium::RelationMember& member, std::size_t) {
        return true;
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


void generate_world(const char *in_file, const char *out_file) {
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


int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <InputFile> <OutputFile>" << std::endl;
        return 2;
    }
    generate_world(argv[1], argv[2]);
    return 0;
}
