namespace rustymon {

    namespace structs {

        struct CheckResult {
            const bool allowed;
            const int type;
            std::vector<int> spawns;
        };

        struct POI {
            const long oid;
            const int type;
            std::pair<int, int> pos;
            std::vector<int> spawns;

            friend std::ostream& operator << (std::ostream &stream, const POI &poi);
        };

        std::ostream& operator << (std::ostream &stream, const POI &poi) {
            stream << "{"
                   << "\"type\":" << poi.type << ","
                   << "\"oid\":" << poi.oid << ","
                   << "\"point\":[" << poi.pos.first << "," << poi.pos.second << "],"
                   << "\"spawns\":[";
            int i = 0;
            for (; i + 1 < poi.spawns.size(); i++) {
                stream << poi.spawns.at(i) << ",";
            }
            if (!poi.spawns.empty()) {
                stream << poi.spawns.at(i);
            }
            stream << "]}";
            return stream;
        }

        struct Street {
            const long oid;
            const int type;
            std::vector<std::pair<int, int>> waypoints;

            friend std::ostream& operator << (std::ostream &stream, const Street &street);
        };

        struct Area {
            const long oid;
            const int type;
            std::vector<std::pair<int, int>> border;
            std::vector<int> spawns;

            friend std::ostream& operator << (std::ostream &stream, const Area &area);
        };

        struct Tile {
            const osmium::Box bbox;
            const long timestamp;
            const int version;
            std::vector<POI> poi;
            std::vector<Street> streets;
            std::vector<Area> areas;

            friend std::ostream& operator << (std::ostream &stream, const Tile &tile);
        };

    }

}
