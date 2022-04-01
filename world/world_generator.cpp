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
    Json::Value streets = Json::Value(Json::arrayValue);
    Json::Value poi = Json::Value(Json::arrayValue);
    Json::Value areas = Json::Value(Json::arrayValue);
    std::string world_uuid = generate_new_uuid();

    struct StreetTypeWrapper {
        bool allowed;
        StreetType type;
    };

    std::string generate_new_uuid() {
        return "uuid";  // TODO: actually generate version 4 UUIDs here
    }

    Json::Value make_point(const osmium::geom::Coordinates& coordinates) const {
        if (coordinates.valid()) {
            Json::Value point = Json::Value(Json::arrayValue);
            point.append(coordinates.x);
            point.append(coordinates.y);
            return point;
        }
        std::cerr << "Failed to validate the coordinate at " << coordinates.x << "/" << coordinates.y << std::endl;
        return Json::nullValue;
    }

    Json::Value make_point(const osmium::Location location) const {
        if (location.valid()) {
            Json::Value point = Json::Value(Json::arrayValue);
            point.append(location.lon());
            point.append(location.lat());
            return point;
        }
        std::cerr << "Failed to validate the location at " << location.lon() << "/" << location.lat() << std::endl;
        return Json::nullValue;
    }

    StreetTypeWrapper get_street_type(const osmium::TagList& tags) {
        return StreetTypeWrapper{
                true,  // TODO: determine whether a given street should be included
                StreetType::PATH // TODO: select a valid street type based on the given tags
        };
    }

public:

    Json::Value get_json_data() {
        Json::Value root;
        root["uuid"] = this->world_uuid;
        root["bbox"] = "bbox";
        root["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        root["version"] = FILE_VERSION;
        root["streets"] = this->streets;
        root["poi"] = this->poi;
        root["areas"] = this->areas;
        return root;
    }

    void way(const osmium::Way& way) {
        if (!way.ends_have_same_id() && !way.ends_have_same_location()) {
            StreetTypeWrapper street_type = get_street_type(way.tags());
            if (!street_type.allowed) {
                return;
            }

            Json::Value entry;
            entry["type"] = static_cast<int>(street_type.type);
            entry["oid"] = way.id();
            Json::Value waypoints = Json::Value(Json::arrayValue);
            for (auto& node: way.nodes()) {
                waypoints.append(make_point(node.location()));
            }
            entry["points"] = waypoints;
            this->streets.append(entry);
        }
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
