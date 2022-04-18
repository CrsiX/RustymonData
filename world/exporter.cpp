#include "exporter.hpp"

namespace rustymon {

    void export_world_to_file(const structs::World &world, const std::string &filename) {
        std::ofstream output_file_stream(filename);
        structs::stream(output_file_stream, world);
        output_file_stream.close();
    }

    void export_world_to_files(const structs::World &world, const std::string &directory) {
        // TODO: Implement this function
    }

    void export_world_to_http(const structs::World &world, const std::string &push_url) {
        // TODO: Implement this function
    }

    void export_world_to_http(const structs::World &world, const std::string &push_url, const std::string &auth_info) {
        // TODO: Implement this function
    }

}
