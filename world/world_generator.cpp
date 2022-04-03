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
#include <osmium/osm/area.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/relations/relations_manager.hpp>

#include "poke_world_enums.hpp"
#include "poke_world_fields.hpp"


static const int FILE_VERSION = 1;
static const int JSON_ENUM_OFFSET = 1;
static const char BBOX_SPLIT_CHAR = '/';
static const std::string DEFAULT_CONFIG_FILENAME = "config.json";


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
    osmium::Box bbox;
    Json::Value config;
    Json::Value streets = Json::Value(Json::arrayValue);
    Json::Value poi = Json::Value(Json::arrayValue);
    Json::Value areas = Json::Value(Json::arrayValue);
    std::string world_uuid = generate_new_uuid();

    struct CheckResult {
        bool allowed;
        int type;
        Json::Value spawns;  // may be null for streets
    };

    struct POIWrapper {
        bool allowed;
        POIType type;
        Json::Value spawns;
    };

    struct StreetWrapper {
        bool allowed;
        StreetType type;
    };

    struct AreaWrapper {
        bool allowed;
        AreaType type;
        Json::Value spawns;
    };

    std::string generate_new_uuid() {
        return "00000000-0000-0000-0000-000000000000";  // TODO: actually generate version 4 UUIDs here
    }

    Json::Value load_config(std::string filename) {
        Json::Value config;
        std::ifstream config_stream(filename, std::ifstream::binary);
        if (!config_stream.is_open()) {
            std::cerr << "Configuration file " << filename << " not found. Exiting." << std::endl;
            exit(1);
        }
        config_stream >> config;
        return config;
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

    CheckResult get_details(const osmium::TagList& tags, Json::Value check_items) {
        for (Json::Value item: check_items) {
            bool allowed = true;
            int type = item["type"].asInt();

            Json::Value forbidden = item["forbidden"];
            for (std::string forbidden_key: forbidden.getMemberNames()) {
                if (tags.has_key(forbidden_key.c_str())) {
                    if (forbidden[forbidden_key.c_str()].size() == 0) {
                        allowed = false;
                        break;
                    }
                    for (Json::Value forbidden_value: forbidden[forbidden_key.c_str()]) {
                        if (forbidden_value.asString().compare(tags.get_value_by_key(forbidden_key.c_str())) == 0) {
                            allowed = false;
                        }
                    }
                }
            }

            if (!allowed) {
                continue;
            }

            Json::Value required = item["required"];
            for (std::string required_key: required.getMemberNames()) {
                if (!tags.has_key(required_key.c_str())) {
                    allowed = false;
                    break;
                }
                Json::Value required_values = required[required_key.c_str()];
                bool found = (required_values.size() == 0);
                for (Json::Value required_value: required_values) {
                    if (strcmp(tags.get_value_by_key(required_key.c_str()), required_value.asCString()) == 0) {
                        found = true;
                    }
                }
                allowed = allowed && found;
                if (!found) {
                    allowed = false;
                }
            }

            if (allowed) {
                return CheckResult{allowed, type, item["spawns"]};
            }
        }

        return CheckResult{false, -1, Json::nullValue};
    }

    POIWrapper get_poi_details(const osmium::TagList& tags) {
        CheckResult result = get_details(tags, this->config["poi"]);
        POIType poi_type = static_cast<POIType>(result.type - JSON_ENUM_OFFSET);
        if (result.spawns == Json::nullValue) {
            result.spawns = Json::Value(Json::arrayValue);
        }
        return POIWrapper{result.allowed, poi_type, result.spawns};
    }

    StreetWrapper get_street_details(const osmium::TagList& tags) {
        CheckResult result = get_details(tags, this->config["streets"]);
        StreetType street_type = static_cast<StreetType>(result.type - JSON_ENUM_OFFSET);
        return StreetWrapper{result.allowed, street_type};
    }

    AreaWrapper get_area_details(const osmium::TagList& tags) {
        CheckResult result = get_details(tags, this->config["areas"]);
        AreaType area_type = static_cast<AreaType>(result.type - JSON_ENUM_OFFSET);
        if (result.spawns == Json::nullValue) {
            result.spawns = Json::Value(Json::arrayValue);
        }
        return AreaWrapper{result.allowed, area_type, result.spawns};
    }

    void check_valid_bbox() {
        if (!this->bbox.valid()) {
            std::cerr << "Invalid bounding box " << this->bbox << "!" << std::endl;
            exit(1);
        }
    }

public:

    WorldGenerator() {
        this->bbox = osmium::Box(-180, -90, 180, 90);
        this->config = load_config(DEFAULT_CONFIG_FILENAME);
        check_valid_bbox();
    }

    WorldGenerator(osmium::Box bbox) {
        this->bbox = bbox;
        this->config = load_config(DEFAULT_CONFIG_FILENAME);
        check_valid_bbox();
    }

    WorldGenerator(osmium::Box bbox, std::string config_filename) {
        this->bbox = bbox;
        this->config = load_config(config_filename);
        check_valid_bbox();
    }

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

    void node(const osmium::Node& node) {
        if (node.visible()) {
            POIWrapper poi_details = get_poi_details(node.tags());
            if (!poi_details.allowed) {
                return;
            }

            Json::Value entry;
            entry["point"] = make_point(node.location());
            entry["oid"] = node.id();
            entry["type"] = static_cast<int>(poi_details.type) + JSON_ENUM_OFFSET;
            entry["spawns"] = poi_details.spawns;
            this->poi.append(entry);
        }
    }

    void way(const osmium::Way& way) {
        if (!way.ends_have_same_id() && !way.ends_have_same_location()) {
            StreetWrapper street_details = get_street_details(way.tags());
            if (!street_details.allowed) {
                return;
            }

            Json::Value entry;
            entry["type"] = static_cast<int>(street_details.type) + JSON_ENUM_OFFSET;
            entry["oid"] = way.id();
            Json::Value waypoints = Json::Value(Json::arrayValue);
            for (auto& node: way.nodes()) {
                waypoints.append(make_point(node.location()));
            }
            entry["points"] = waypoints;
            this->streets.append(entry);
        }
    }

    void area(const osmium::Area& area) {
        if (area.visible()) {
            AreaWrapper area_details = get_area_details(area.tags());
            if (!area_details.allowed) {
                return;
            }

            int outer_rings = 0;
            int inner_rings = 0;
            for (const auto& item: area) {
                if (item.type() == osmium::item_type::outer_ring) {
                    outer_rings++;
                } else if (item.type() == osmium::item_type::inner_ring) {
                    inner_rings++;
                }
            }

            if (outer_rings < 1) {
                std::cerr << "Invalid area definition found in area " << area.id() << std::endl;
                return;
            } else if (area.is_multipolygon() || outer_rings > 1 || inner_rings > 0) {
                // TODO: add support for multi polygons
                std::cerr << "Unsupported multi polygon in area " << area.id() << std::endl;
                return;
            }

            Json::Value waypoints = Json::Value(Json::arrayValue);
            for (const auto& item: area) {
                if (item.type() == osmium::item_type::outer_ring) {
                    const auto& ring = static_cast<const osmium::OuterRing&>(item);
                    osmium::Location last_location;
                    for (const osmium::NodeRef& node : ring) {
                        if (last_location != node.location()) {
                            last_location = node.location();
                            waypoints.append(make_point(last_location));
                        }
                    }
                }
            }

            Json::Value entry;
            entry["type"] = static_cast<int>(area_details.type) + JSON_ENUM_OFFSET;
            entry["oid"] = area.id();
            entry["points"] = waypoints;
            entry["spawns"] = area_details.spawns;
            this->areas.append(entry);
        }
    }
};


void generate_world(std::string in_file, std::string out_file, osmium::Box bbox, std::string config_file) {
    const osmium::io::File input_file{in_file};

    osmium::area::Assembler::config_type assembler_config;
    assembler_config.create_empty_areas = false;

    osmium::area::MultipolygonManager<osmium::area::Assembler> mp_manager{assembler_config};

    osmium::relations::read_relations(input_file, mp_manager);
    index_type index;

    location_handler_type location_handler{index};
    location_handler.ignore_errors();

    WorldGenerator data_handler = WorldGenerator(bbox, config_file);
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


osmium::Box get_bbox(std::string spec) {
    std::stringstream spec_stream(spec);
    std::string segment;
    std::vector<double> bounding_box_values;

    while (std::getline(spec_stream, segment, BBOX_SPLIT_CHAR)) {
        bounding_box_values.push_back(std::stod(segment));
    }
    if (bounding_box_values.size() != 4) {
        std::cerr << "The bounding box has to contain exactly four values for minX, minY, maxX and maxY." << std::endl;
        exit(2);
    }

    osmium::Box bbox(
            bounding_box_values.at(0),
            bounding_box_values.at(1),
            bounding_box_values.at(2),
            bounding_box_values.at(3)
    );
    return bbox;
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

    std::string config_file = DEFAULT_CONFIG_FILENAME;
    if (argc == 5) {
        config_file = argv[4];
    } else if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <InputFile> <OutputFile> <BoundingBox> [<ConfigFile>]" << std::endl;
        return 2;
    }

    osmium::Box bbox = get_bbox(argv[3]);
    std::cout << "Using bounding box " << bbox << "." << std::endl;
    generate_world(argv[1], argv[2], bbox, config_file);
    return 0;
}
