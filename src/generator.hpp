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
#include "config.hpp"

namespace rustymon {

    namespace helpers {

        inline long get_timestamp();

        Json::Value load_config(const std::string& filename);

        Json::Value make_point(const osmium::geom::Coordinates &coordinates);

        Json::Value make_point(osmium::Location location);

        osmium::Box get_bbox(const std::string& spec);

    }

    class WorldGenerator : public osmium::handler::Handler {
        int x_size_factor = X_SIZE_FACTOR_DEFAULT;
        int y_size_factor = Y_SIZE_FACTOR_DEFAULT;

        osmium::Box bbox;
        config::Config config;
        structs::World tiles;

        static int get_details(const osmium::TagList &tags, const std::vector<config::ObjectProcessorEntry> &check_items, std::vector<int> &spawns);

        inline void check_valid_bbox() {
            if (!this->bbox.valid()) {
                std::cerr << "Invalid bounding box " << this->bbox << "!" << std::endl;
                exit(1);
            }
        }

        inline void ensure_exists_in_world(const int &x_section, const int &y_section);

    public:

        explicit WorldGenerator() :
            config(config::load_config_from_file(DEFAULT_CONFIG_FILENAME)),
            bbox(osmium::Box(-180, -90, 180, 90)) {
            check_valid_bbox();
            x_size_factor = (config.size.x > 0) ? config.size.x : X_SIZE_FACTOR_DEFAULT;
            y_size_factor = (config.size.y > 0) ? config.size.y : Y_SIZE_FACTOR_DEFAULT;
        }

        explicit WorldGenerator(const config::Config &config, const osmium::Box &bbox = osmium::Box(-180, -90, 180, 90)) :
            config(config),
            bbox(bbox) {
            check_valid_bbox();
            x_size_factor = (config.size.x > 0) ? config.size.x : X_SIZE_FACTOR_DEFAULT;
            y_size_factor = (config.size.y > 0) ? config.size.y : Y_SIZE_FACTOR_DEFAULT;
        }

        explicit WorldGenerator(const std::string &config_filename, const osmium::Box &bbox = osmium::Box(-180, -90, 180, 90)) :
            config(config::load_config_from_file(config_filename)),
            bbox(bbox) {
            check_valid_bbox();
            x_size_factor = (config.size.x > 0) ? config.size.x : X_SIZE_FACTOR_DEFAULT;
            y_size_factor = (config.size.y > 0) ? config.size.y : Y_SIZE_FACTOR_DEFAULT;
        }

        inline structs::World& get_world() {
            return this->tiles;
        }

        void node(const osmium::Node &node);

        void way(const osmium::Way &way);

        void area(const osmium::Area &area);
    };

    namespace reader {

        using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>;
        using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

        void read_from_file(WorldGenerator &data_handler, const std::string &in_file);

    }

}

#endif //WORLD_GENERATOR_GENERATOR_HPP
