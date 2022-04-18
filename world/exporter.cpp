#include "exporter.hpp"

namespace rustymon {

    void export_world_to_file(const structs::World &world, const std::string &filename, std::ostream &logger) {
        std::ofstream output_file_stream(filename);
        structs::stream(output_file_stream, world);
        output_file_stream.close();
    }

    void export_world_to_files(const structs::World &world, const std::string &directory, std::ostream &logger) {
        // TODO: Implement this function
    }

    void export_world_to_http(const structs::World &world, const std::string &push_url, const std::string &auth_info, std::ostream &logger) {
        cpr::Header headers{{"Content-Type", "application/json"}};
        if (!auth_info.empty()) {
            headers.insert({"Authorization", auth_info});
        }

        int counter = 0;
        for (auto const& x : world) {
            for (auto const &y: x.second) {
                counter++;
            }
        }
        logger << "Counted " << counter << " objects to upload..." << std::endl;

        int errors = 0;
        for (auto const& x : world) {
            for (auto const &y: x.second) {
                std::stringstream body;
                body << y.second;
                std::stringstream position_header;

                position_header << x.first << "," << y.first;
                headers.erase("X-Tile-Position");
                headers.insert({"X-Tile-Position", position_header.str()});

                cpr::Response r = cpr::Post(cpr::Url{push_url}, cpr::Body{body.str()}, headers);
                if (r.status_code != 200) {
                    errors++;
                    logger << "Received status code " << r.status_code << " while uploading Tile " << position_header.str() << std::endl;
                }
            }
        }

        logger << "Completed uploading of " << counter << " objects with " << errors << " errors." << std::endl;
    }

}
