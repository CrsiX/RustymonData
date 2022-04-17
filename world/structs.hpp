#ifndef WORLD_GENERATOR_STRUCTS_HPP
#define WORLD_GENERATOR_STRUCTS_HPP

namespace rustymon {

    namespace structs {

        struct POI {
            const long oid;
            const int type;
            std::pair<double, double> pos;
            std::vector<int> spawns;

            friend std::ostream& operator << (std::ostream &stream, const POI &poi);
        };

        std::ostream& operator << (std::ostream &stream, const POI &poi);

        struct Street {
            const long oid;
            const int type;
            std::vector<std::pair<double, double>> waypoints;

            friend std::ostream& operator << (std::ostream &stream, const Street &street);
        };

        std::ostream& operator << (std::ostream &stream, const Street &street);

        struct Area {
            const long oid;
            const int type;
            std::vector<std::pair<double, double>> border;
            std::vector<int> spawns;

            friend std::ostream& operator << (std::ostream &stream, const Area &area);
        };

        std::ostream& operator << (std::ostream &stream, const Area &area);

        struct Tile {
            const osmium::Box bbox;
            const long timestamp;
            const int version;
            std::vector<POI> poi;
            std::vector<Street> streets;
            std::vector<Area> areas;

            friend std::ostream& operator << (std::ostream &stream, const Tile &tile);
        };

        std::ostream& operator << (std::ostream &stream, const Tile &tile);

    }

}

#endif //WORLD_GENERATOR_STRUCTS_HPP
