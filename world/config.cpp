#include "config.hpp"

namespace rustymon {

    namespace config {

        Config load_config_from_json(const Json::Value &data) {
            Workers workers{
                .node = data.get("workers", Json::objectValue).get("node", rustymon::NODE_DEFAULT_WORKER_THREADS).asInt(),
                .way = data.get("workers", Json::objectValue).get("node", rustymon::WAY_DEFAULT_WORKER_THREADS).asInt(),
                .area = data.get("workers", Json::objectValue).get("node", rustymon::AREA_DEFAULT_WORKER_THREADS).asInt(),
                .upload = data.get("workers", Json::objectValue).get("node", rustymon::UPLOAD_DEFAULT_WORKER_THREADS).asInt()
            };

            Size size{
                .x = data.get("size", Json::objectValue).get("x", rustymon::X_SIZE_FACTOR_DEFAULT).asInt(),
                .y = data.get("size", Json::objectValue).get("y", rustymon::Y_SIZE_FACTOR_DEFAULT).asInt()
            };

            auto convert_object_to_map = [](const Json::Value& object){
                std::map<std::string, std::vector<std::string>> map;
                for (const std::string &key: object.getMemberNames()) {
                    std::vector<std::string> values{};
                    if (!object[key.c_str()].empty()) {
                        for (const Json::Value &value: object[key.c_str()]) {
                            values.push_back(value.asString());
                        }
                    }
                    map.insert(std::pair<std::string, std::vector<std::string>>{key, std::move(values)});
                }
                return map;
            };

            std::vector<ObjectProcessorEntry> poi;
            try {
                for (const Json::Value &v: data.get("poi", Json::arrayValue)) {
                    std::vector<int> spawns;
                    for (const Json::Value &s: v.get("spawns", Json::arrayValue)) {
                        spawns.push_back(s.asInt());
                    }

                    poi.push_back(ObjectProcessorEntry{
                            .type = v["type"].asInt(),
                            .spawns = spawns,
                            .required = convert_object_to_map(v.get("required", Json::objectValue)),
                            .forbidden = convert_object_to_map(v.get("forbidden", Json::objectValue))
                    });
                }
            } catch (Json::LogicError &error) {
                std::cerr << "Config error (section 'poi'): " << error.what() << std::endl;
                exit(1);
            }

            std::vector<ObjectProcessorEntry> streets;
            try {
                for (const Json::Value &v: data.get("streets", Json::arrayValue)) {
                    streets.push_back(ObjectProcessorEntry{
                            .type = v["type"].asInt(),
                            .spawns = {},
                            .required = convert_object_to_map(v.get("required", Json::objectValue)),
                            .forbidden = convert_object_to_map(v.get("forbidden", Json::objectValue))
                    });
                }
            } catch (Json::LogicError &error) {
                std::cerr << "Config error (section 'streets'): " << error.what() << std::endl;
                exit(1);
            }

            std::vector<ObjectProcessorEntry> areas;
            try {
                for (const Json::Value &v: data.get("areas", Json::arrayValue)) {
                    std::vector<int> spawns;
                    for (const Json::Value &s: v.get("spawns", Json::arrayValue)) {
                        spawns.push_back(s.asInt());
                    }

                    areas.push_back(ObjectProcessorEntry{
                            .type = v["type"].asInt(),
                            .spawns = std::move(spawns),
                            .required = convert_object_to_map(v.get("required", Json::objectValue)),
                            .forbidden = convert_object_to_map(v.get("forbidden", Json::objectValue))
                    });
                }
            } catch (Json::LogicError &error) {
                std::cerr << "Config error (section 'areas'): " << error.what() << std::endl;
                exit(1);
            }

            return Config{
                .workers = workers,
                .size = size,
                .poi = poi,
                .streets = streets,
                .areas = areas
            };
        }

        Config load_config_from_file(const std::string &filename) {
            Json::Value config;
            std::ifstream config_stream(filename, std::ifstream::binary);
            if (!config_stream.is_open()) {
                std::cerr << "Configuration file " << filename << " not found. Exiting." << std::endl;
                exit(1);
            }
            config_stream >> config;
            return load_config_from_json(config);
        }

    }

}
