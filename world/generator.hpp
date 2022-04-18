#ifndef WORLD_GENERATOR_GENERATOR_HPP
#define WORLD_GENERATOR_GENERATOR_HPP

#include <chrono>
#include <fstream>
#include <iostream>

#include <json/json.h>
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

#include "constants.hpp"
#include "structs.hpp"

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

        structs::World tiles;

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

        structs::World& get_world() {
            return this->tiles;
        }

        void node(const osmium::Node &node);

        void way(const osmium::Way &way);

        void area(const osmium::Area &area);
    };

    namespace reader {

        using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>;
        using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

        template <class H> void read_from_file(H &data_handler, const std::string &in_file) {
            const osmium::io::File input_file{in_file};

            osmium::area::Assembler::config_type assembler_config;
            assembler_config.create_empty_areas = false;

            osmium::area::MultipolygonManager <osmium::area::Assembler> mp_manager{assembler_config};

            osmium::relations::read_relations(input_file, mp_manager);
            index_type index;

            location_handler_type location_handler{index};
            location_handler.ignore_errors();

            osmium::io::Reader reader{input_file, osmium::io::read_meta::no};

            osmium::apply(reader, location_handler, data_handler,
                          mp_manager.handler([&data_handler](const osmium::memory::Buffer &area_buffer) {
                              osmium::apply(area_buffer, data_handler);
                          }));

            reader.close();
        }

    }

}

#endif //WORLD_GENERATOR_GENERATOR_HPP
