namespace rustymon {

    class WorldGenerator : public osmium::handler::Handler {
        int x_size_factor;
        int y_size_factor;
        osmium::Box bbox;
        Json::Value config;

        std::map<int, std::map<int, structs::Tile>> tiles;

        static int get_details(const osmium::TagList& tags, const Json::Value& check_items, std::vector<int> &spawns) {
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

        void check_valid_bbox() {
            if (!this->bbox.valid()) {
                std::cerr << "Invalid bounding box " << this->bbox << "!" << std::endl;
                exit(1);
            }
        }

        void ensure_exists_in_world(const int &x_section, const int &y_section) {
            tiles.insert(std::pair<int, std::map<int, structs::Tile>>{x_section, std::map<int, structs::Tile>()});
            tiles.at(x_section).insert(std::pair<int, structs::Tile>{y_section, structs::Tile{
                    osmium::Box(
                            static_cast<double>(x_section) / x_size_factor,
                            static_cast<double>(y_section) / y_size_factor,
                            (static_cast<double>(x_section) + 1) / x_size_factor,
                            (static_cast<double>(y_section) + 1) / y_size_factor
                    ),
                    get_timestamp(),
                    FILE_VERSION,
                    std::vector<structs::POI>{},
                    std::vector<structs::Street>{},
                    std::vector<structs::Area>{}
            }});
        }

    public:

        WorldGenerator() {
            this->bbox = osmium::Box(-180, -90, 180, 90);
            this->config = load_config(DEFAULT_CONFIG_FILENAME);
            check_valid_bbox();
            this->x_size_factor = this->config["size"]["x"].asInt();
            this->y_size_factor = this->config["size"]["y"].asInt();
        }

        explicit WorldGenerator(osmium::Box bbox) {
            this->bbox = bbox;
            this->config = load_config(DEFAULT_CONFIG_FILENAME);
            check_valid_bbox();
            this->x_size_factor = this->config["size"]["x"].asInt();
            this->y_size_factor = this->config["size"]["y"].asInt();
        }

        explicit WorldGenerator(const std::string &config_filename) {
            this->bbox = osmium::Box(-180, -90, 180, 90);
            this->config = load_config(DEFAULT_CONFIG_FILENAME);
            check_valid_bbox();
            this->x_size_factor = this->config["size"]["x"].asInt();
            this->y_size_factor = this->config["size"]["y"].asInt();
        }

        WorldGenerator(osmium::Box bbox, const std::string &config_filename) {
            this->bbox = bbox;
            this->config = load_config(config_filename);
            check_valid_bbox();
            this->x_size_factor = this->config["size"]["x"].asInt();
            this->y_size_factor = this->config["size"]["y"].asInt();
        }

        Json::Value get_json_data() {
            Json::Value bbox_json = Json::Value(Json::arrayValue);
            bbox_json[0] = this->bbox.bottom_left().lon();
            bbox_json[1] = this->bbox.bottom_left().lat();
            bbox_json[2] = this->bbox.top_right().lon();
            bbox_json[3] = this->bbox.top_right().lat();

            Json::Value root;
            root["bbox"] = bbox_json;
            root["timestamp"] = get_timestamp();
            root["version"] = FILE_VERSION;
            root["poi"] = Json::Value(Json::arrayValue);
            root["streets"] = Json::Value(Json::arrayValue);
            root["areas"] = Json::Value(Json::arrayValue);
            return root;
        }

        void node(const osmium::Node &node) {
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

        void way(const osmium::Way &way) {
            if (!way.ends_have_same_id() && !way.ends_have_same_location()) {
                std::vector<int> spawns;
                int type = get_details(way.tags(), this->config["streets"], spawns);
                if (type < 0) {
                    return;
                }

                Json::Value entry;
                entry["type"] = type;
                entry["oid"] = way.id();
                Json::Value waypoints = Json::Value(Json::arrayValue);
                for (auto &node: way.nodes()) {
                    waypoints.append(make_point(node.location()));
                }
                entry["points"] = waypoints;
            }
        }

        void area(const osmium::Area &area) {
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
                                waypoints.append(make_point(last_location));
                            }
                        }
                    }
                }

                Json::Value entry;
                entry["type"] = type;
                entry["oid"] = area.id();
                entry["points"] = waypoints;
            }
        }
    };

}
