#include "structs.hpp"

namespace rustymon {

    namespace structs {

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

        std::ostream& operator << (std::ostream &stream, const Street &street) {
            stream << "{"
                   << "\"type\":" << street.type << ","
                   << "\"oid\":" << street.oid << ","
                   << "\"points\":[";
            int i = 0;
            for (; i + 1 < street.waypoints.size(); i++) {
                const std::pair<double, double> &point = street.waypoints.at(i);
                stream << "[" << point.first << "," << point.second << "],";
            }
            if (!street.waypoints.empty()) {
                const std::pair<double, double> &point = street.waypoints.at(i);
                stream << "[" << point.first << "," << point.second << "]";
            }
            stream << "]}";
            return stream;
        }

        std::ostream& operator << (std::ostream &stream, const Area &area) {
            stream << "{"
                   << "\"type\":" << area.type << ","
                   << "\"oid\":" << area.oid << ","
                   << "\"spawns\":[";
            int i = 0;
            for (; i + 1 < area.spawns.size(); i++) {
                stream << area.spawns.at(i) << ",";
            }
            if (!area.spawns.empty()) {
                stream << area.spawns.at(i);
            }
            stream << "],\"points\":[";
            i = 0;
            for (; i + 1 < area.border.size(); i++) {
                const std::pair<double, double> &point = area.border.at(i);
                stream << "[" << point.first << "," << point.second << "],";
            }
            if (!area.border.empty()) {
                const std::pair<double, double> &point = area.border.at(i);
                stream << "[" << point.first << "," << point.second << "]";
            }
            stream << "]}";
            return stream;
        }

        std::ostream& operator << (std::ostream &stream, const Tile &tile) {
            stream << "{";
            if (tile.bbox.valid()) {
                stream << "\"bbox\":["
                       << tile.bbox.bottom_left().lon() << ","
                       << tile.bbox.bottom_left().lat() << ","
                       << tile.bbox.top_right().lon() << ","
                       << tile.bbox.top_right().lat() << "],";
            }
            stream << "\"timestamp\":" << tile.timestamp << ","
                   << "\"version\":" << tile.version << ","
                   << "\"poi\":[";
            int i = 0;
            for (; i + 1 < tile.poi.size(); i++) {
                stream << tile.poi.at(i) << ",";
            }
            if (!tile.poi.empty()) {
                stream << tile.poi.at(i);
            }
            stream << "],\"streets\":[";
            i = 0;
            for (; i + 1 < tile.streets.size(); i++) {
                stream << tile.streets.at(i) << ",";
            }
            if (!tile.streets.empty()) {
                stream << tile.streets.at(i);
            }
            stream << "],\"areas\":[";
            i = 0;
            for (; i + 1 < tile.areas.size(); i++) {
                stream << tile.areas.at(i) << ",";
            }
            if (!tile.areas.empty()) {
                stream << tile.areas.at(i);
            }
            stream << "]}";
            return stream;
        }

    }

}
