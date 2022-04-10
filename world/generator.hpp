namespace rustymon {

    class WorldGenerator : public osmium::handler::Handler {
        osmium::Box bbox;
        Json::Value config;
        Json::Value streets = Json::Value(Json::arrayValue);
        Json::Value poi = Json::Value(Json::arrayValue);
        Json::Value areas = Json::Value(Json::arrayValue);

        ThreadSafeQueue<const osmium::Node*> node_work_queue;
        ThreadSafeQueue<const osmium::Way*> way_work_queue;
        ThreadSafeQueue<const osmium::Area*> area_work_queue;

        ThreadSafeQueue<Json::Value> poi_result_queue;
        ThreadSafeQueue<Json::Value> street_result_queue;
        ThreadSafeQueue<Json::Value> area_result_queue;

        std::vector<std::thread> worker_threads;

        struct CheckResult {
            bool allowed;
            int type;
            Json::Value spawns;  // may be null for streets
        };

        static CheckResult get_details(const osmium::TagList &tags, const Json::Value& check_items) {
            for (const Json::Value &item: check_items) {
                bool allowed = true;
                int type = item["type"].asInt();

                Json::Value required = item["required"];
                Json::Value forbidden = item["forbidden"];
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
                    Json::Value required_values = required[required_key.c_str()];
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
                    return CheckResult{allowed, type, item["spawns"]};
                }
            }

            return CheckResult{false, -1, Json::nullValue};
        }

        void check_valid_bbox() {
            if (!this->bbox.valid()) {
                std::cerr << "Invalid bounding box " << this->bbox << "!" << std::endl;
                exit(1);
            }
        }

        void node_worker() {
            // TODO
        }

        void way_worker() {
            // TODO
        }

        void area_worker() {
            // TODO
        }

    public:

        WorldGenerator() {
            this->bbox = osmium::Box(-180, -90, 180, 90);
            this->config = load_config(DEFAULT_CONFIG_FILENAME);
            check_valid_bbox();
        }

        explicit WorldGenerator(osmium::Box bbox) {
            this->bbox = bbox;
            this->config = load_config(DEFAULT_CONFIG_FILENAME);
            check_valid_bbox();
        }

        WorldGenerator(osmium::Box bbox, std::string config_filename) {
            this->bbox = bbox;
            this->config = load_config(std::move(config_filename));
            check_valid_bbox();
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
            root["streets"] = this->streets;
            root["poi"] = this->poi;
            root["areas"] = this->areas;
            return root;
        }

        void node(const osmium::Node &node) {
            node_work_queue.push(&node);
            if (node.visible()) {
                CheckResult result = get_details(node.tags(), this->config["poi"]);
                if (!result.allowed) {
                    return;
                }
                if (result.spawns == Json::nullValue) {
                    result.spawns = Json::Value(Json::arrayValue);
                }

                Json::Value entry;
                entry["point"] = make_point(node.location());
                entry["oid"] = node.id();
                entry["type"] = result.type;
                entry["spawns"] = result.spawns;
                this->poi.append(entry);
            }
        }

        void way(const osmium::Way &way) {
            way_work_queue.push(&way);
            if (!way.ends_have_same_id() && !way.ends_have_same_location()) {
                CheckResult result = get_details(way.tags(), this->config["streets"]);
                if (!result.allowed) {
                    return;
                }

                Json::Value entry;
                entry["type"] = result.type;
                entry["oid"] = way.id();
                Json::Value waypoints = Json::Value(Json::arrayValue);
                for (auto &node: way.nodes()) {
                    waypoints.append(make_point(node.location()));
                }
                entry["points"] = waypoints;
                this->streets.append(entry);
            }
        }

        void area(const osmium::Area &area) {
            area_work_queue.push(&area);
            if (area.visible()) {
                CheckResult result = get_details(area.tags(), this->config["areas"]);
                if (!result.allowed) {
                    return;
                }
                if (result.spawns == Json::nullValue) {
                    result.spawns = Json::Value(Json::arrayValue);
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
                entry["type"] = result.type;
                entry["oid"] = area.id();
                entry["points"] = waypoints;
                entry["spawns"] = result.spawns;
                this->areas.append(entry);
            }
        }

        void start_workers() {
            Json::Value node_workers = config["workers"].get("node", NODE_DEFAULT_WORKER_THREADS);
            Json::Value way_workers = config["workers"].get("way", WAY_DEFAULT_WORKER_THREADS);
            Json::Value area_workers = config["workers"].get("area", AREA_DEFAULT_WORKER_THREADS);

            for (int i = 0; i < node_workers.asInt(); i++) {
                worker_threads.emplace_back(&WorldGenerator::node_worker, this);
            }
            for (int i = 0; i < way_workers.asInt(); i++) {
                worker_threads.emplace_back(&WorldGenerator::way_worker, this);
            }
            for (int i = 0; i < area_workers.asInt(); i++) {
                worker_threads.emplace_back(&WorldGenerator::area_worker, this);
            }
        }
    };

}
