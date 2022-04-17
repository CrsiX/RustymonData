#ifndef WORLD_GENERATOR_STRUCTS_HPP
#define WORLD_GENERATOR_STRUCTS_HPP

#include <map>
#include <vector>

namespace rustymon {

    namespace structs {

        struct BoundingBox {
            std::pair<double, double> bottom_left;
            std::pair<double, double> top_right;

            BoundingBox(double x1, double y1, double x2, double y2) {
                this->bottom_left = std::pair<double, double>{x1, y1};
                this->top_right = std::pair<double, double>{x2, y2};
            }

            friend std::ostream& operator << (std::ostream &stream, const BoundingBox &bbox);
        };

        std::ostream& operator << (std::ostream &stream, const BoundingBox &bbox);

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
            const BoundingBox bbox;
            const long timestamp;
            const int version;
            std::vector<POI> poi;
            std::vector<Street> streets;
            std::vector<Area> areas;

            friend std::ostream& operator << (std::ostream &stream, const Tile &tile);
        };

        std::ostream& operator << (std::ostream &stream, const Tile &tile);

        using World = std::map<int, std::map<int, structs::Tile>>;

        std::ostream& operator << (std::ostream &stream, const World &world);

    }

}

#endif //WORLD_GENERATOR_STRUCTS_HPP
