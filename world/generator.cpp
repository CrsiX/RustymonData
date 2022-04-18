#include "generator.hpp"

namespace rustymon {

    namespace helpers {

        long get_timestamp() {
            return std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
        }

        Json::Value load_config(const std::string &filename) {
            Json::Value config;
            std::ifstream config_stream(filename, std::ifstream::binary);
            if (!config_stream.is_open()) {
                std::cerr << "Configuration file " << filename << " not found. Exiting." << std::endl;
                exit(1);
            }
            config_stream >> config;
            return config;
        }

        Json::Value make_point(const osmium::geom::Coordinates &coordinates) {
            if (coordinates.valid()) {
                Json::Value point = Json::Value(Json::arrayValue);
                point.append(coordinates.x);
                point.append(coordinates.y);
                return point;
            }
            std::cerr << "Failed to validate the coordinate at " << coordinates.x << "/" << coordinates.y << std::endl;
            return Json::nullValue;
        }

        Json::Value make_point(const osmium::Location location) {
            if (location.valid()) {
                Json::Value point = Json::Value(Json::arrayValue);
                point.append(location.lon());
                point.append(location.lat());
                return point;
            }
            std::cerr << "Failed to validate the location at " << location.lon() << "/" << location.lat() << std::endl;
            return Json::nullValue;
        }

        osmium::Box get_bbox(const std::string& spec) {
            std::stringstream spec_stream = std::stringstream(spec);
            std::string segment;
            std::vector<double> bounding_box_values;

            while (std::getline(spec_stream, segment, BBOX_SPLIT_CHAR)) {
                try {
                    bounding_box_values.push_back(std::stod(segment));
                } catch (std::invalid_argument&) {
                    std::cerr << "The value " << segment << " doesn't seem to be a floating point value." << std::endl;
                    exit(2);
                }
            }
            if (bounding_box_values.size() != 4) {
                std::cerr << "The bounding box has to contain exactly four values for minX, minY, maxX and maxY."
                          << std::endl;
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

    }

    int WorldGenerator::get_details(const osmium::TagList& tags, const Json::Value& check_items, std::vector<int> &spawns) {
        for (const Json::Value &item: check_items) {
            bool allowed = true;
            int type = item["type"].asInt();

            const Json::Value &required = item["required"];
            const Json::Value &forbidden = item["forbidden"];
            if (required.empty() && forbidden.empty()) {
                continue;
            }

            for (const std::string& forbidden_key: forbidden.getMemberNames()) {
                if (tags.has_key(forbidden_key.c_str())) {
                    if (forbidden[forbidden_key.c_str()].empty()) {
                        allowed = false;
                        break;
                    }
                    for (const Json::Value& forbidden_value: forbidden[forbidden_key.c_str()]) {
                        if (forbidden_value.asString() == tags.get_value_by_key(forbidden_key.c_str())) {
                            allowed = false;
                        }
                    }
                }
            }

            if (!allowed) {
                continue;
            }

            for (const std::string& required_key: required.getMemberNames()) {
                if (!tags.has_key(required_key.c_str())) {
                    allowed = false;
                    break;
                }
                const Json::Value &required_values = required[required_key.c_str()];
                bool found = required_values.empty();
                for (const Json::Value& required_value: required_values) {
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
                for (const Json::Value &s : item["spawns"]) {
                    spawns.push_back(s.asInt());
                }
                return type;
            }
        }

        return -1;
    }

    void WorldGenerator::ensure_exists_in_world(const int &x_section, const int &y_section) {
        tiles.insert(std::pair<int, std::map<int, structs::Tile>>{x_section, std::map<int, structs::Tile>()});
        tiles.at(x_section).insert(std::pair<int, structs::Tile>{y_section, structs::Tile{
                structs::BoundingBox(
                        static_cast<double>(x_section) / x_size_factor,
                        static_cast<double>(y_section) / y_size_factor,
                        (static_cast<double>(x_section) + 1) / x_size_factor,
                        (static_cast<double>(y_section) + 1) / y_size_factor
                ),
                std::vector<structs::POI>{},
                std::vector<structs::Street>{},
                std::vector<structs::Area>{}
        }});
    }

    void WorldGenerator::node(const osmium::Node &node) {
        if (node.visible()) {
            std::vector<int> spawns;
            int type = get_details(node.tags(), this->config["poi"], spawns);
            if (type < 0) {
                return;
            }

            int pos_x = std::floor(node.location().lon() * x_size_factor);
            int pos_y = std::floor(node.location().lat() * y_size_factor);
            ensure_exists_in_world(pos_x, pos_y);
            tiles.at(pos_x).at(pos_y).poi.push_back(structs::POI{
                    node.id(),
                    type,
                    std::pair<double, double>{node.location().lon(), node.location().lat()},
                    std::move(spawns)
            });
        }
    }

    void WorldGenerator::way(const osmium::Way &way) {
        if (!way.ends_have_same_id() && !way.ends_have_same_location()) {
            std::vector<int> spawns;
            int type = get_details(way.tags(), this->config["streets"], spawns);
            if (type < 0) {
                return;
            }

            Json::Value entry;
            entry["type"] = type;
            entry["oid"] = Json::Value::UInt64(static_cast<unsigned long>(way.id()));
            Json::Value waypoints = Json::Value(Json::arrayValue);
            for (auto &node: way.nodes()) {
                waypoints.append(helpers::make_point(node.location()));
            }
            entry["points"] = waypoints;

        }
    }

    void WorldGenerator::area(const osmium::Area &area) {
        if (area.visible()) {
            std::vector<int> spawns;
            int type = get_details(area.tags(), this->config["areas"], spawns);
            if (type < 0) {
                return;
            }

            int outer_rings = 0;
            int inner_rings = 0;
            for (const auto &item: area) {
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
            for (const auto &item: area) {
                if (item.type() == osmium::item_type::outer_ring) {
                    const auto &ring = static_cast<const osmium::OuterRing &>(item);
                    osmium::Location last_location;
                    for (const osmium::NodeRef &node: ring) {
                        if (last_location != node.location()) {
                            last_location = node.location();
                            waypoints.append(helpers::make_point(last_location));
                        }
                    }
                }
            }

            Json::Value entry;
            entry["type"] = type;
            entry["oid"] = Json::Value::UInt64(static_cast<unsigned long>(area.id()));
            entry["points"] = waypoints;
        }
    }
}
