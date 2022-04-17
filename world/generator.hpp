#ifndef WORLD_GENERATOR_GENERATOR_HPP
#define WORLD_GENERATOR_GENERATOR_HPP




namespace rustymon {

    namespace helpers {

        long get_timestamp();

        Json::Value load_config(const std::string& filename);

        Json::Value make_point(const osmium::geom::Coordinates &coordinates);

        Json::Value make_point(osmium::Location location);

        osmium::Box get_bbox(const std::string& spec);

    }

    class WorldGenerator : public osmium::handler::Handler {
        int x_size_factor;
        int y_size_factor;
        osmium::Box bbox;
        Json::Value config;

        std::map<int, std::map<int, structs::Tile>> tiles;

        static int get_details(const osmium::TagList& tags, const Json::Value& check_items, std::vector<int> &spawns);

        void check_valid_bbox() {
            if (!this->bbox.valid()) {
                std::cerr << "Invalid bounding box " << this->bbox << "!" << std::endl;
                exit(1);
            }
        }

        void ensure_exists_in_world(const int &x_section, const int &y_section);

    public:

        WorldGenerator() {
            this->bbox = osmium::Box(-180, -90, 180, 90);
            this->config = helpers::load_config(DEFAULT_CONFIG_FILENAME);
            check_valid_bbox();
            this->x_size_factor = this->config["size"]["x"].asInt();
            this->y_size_factor = this->config["size"]["y"].asInt();
        }

        explicit WorldGenerator(osmium::Box bbox) {
            this->bbox = bbox;
            this->config = helpers::load_config(DEFAULT_CONFIG_FILENAME);
            check_valid_bbox();
            this->x_size_factor = this->config["size"]["x"].asInt();
            this->y_size_factor = this->config["size"]["y"].asInt();
        }

        explicit WorldGenerator(const std::string &config_filename) {
            this->bbox = osmium::Box(-180, -90, 180, 90);
            this->config = helpers::load_config(DEFAULT_CONFIG_FILENAME);
            check_valid_bbox();
            this->x_size_factor = this->config["size"]["x"].asInt();
            this->y_size_factor = this->config["size"]["y"].asInt();
        }

        WorldGenerator(osmium::Box bbox, const std::string &config_filename) {
            this->bbox = bbox;
            this->config = helpers::load_config(config_filename);
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
            root["timestamp"] = Json::Value::UInt64(static_cast<unsigned long>(helpers::get_timestamp()));
            root["version"] = FILE_VERSION;
            root["poi"] = Json::Value(Json::arrayValue);
            root["streets"] = Json::Value(Json::arrayValue);
            root["areas"] = Json::Value(Json::arrayValue);

                }

            }
        }

        void node(const osmium::Node &node);

        void way(const osmium::Way &way);

        void area(const osmium::Area &area);
    };

}

#endif //WORLD_GENERATOR_GENERATOR_HPP
