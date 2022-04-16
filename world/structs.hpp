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
            std::tuple<int, int> pos;
            std::vector<int> spawns;
        };

        struct Street {
            const long oid;
            const int type;
            std::vector<std::tuple<int, int>> waypoints;
        };

        struct Area {
            const long oid;
            const int type;
            std::vector<std::tuple<int, int>> border;
            std::vector<int> spawns;
        };

        struct Tile {
            const osmium::Box bbox;
            std::vector<POI> poi;
            std::vector<Street> streets;
            std::vector<Area> areas;
        };

    }

}
