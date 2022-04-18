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
            /**
             * An important note to make here is that arguments passed to an asynchronous call are copied.
             * Under the hood, an asynchronous call through the library’s API is done with std::async.
             * By default, for memory safety, all arguments are copied (or moved if temporary) because there’s
             * no syntax level guarantee that the arguments will live beyond the scope of the request.
             */
            std::vector<std::future<cpr::Response>> responses{};

            for (auto const &y: x.second) {
                std::stringstream body;
                body << y.second;
                std::stringstream position_header;

                position_header << x.first << "," << y.first;
                headers.erase("X-Tile-Position");
                headers.insert({"X-Tile-Position", position_header.str()});
                responses.emplace_back(cpr::PostAsync(cpr::Url{push_url}, cpr::Body{body.str()}, headers));
            }

            int completed_parts = 0;
            for (std::future<cpr::Response>& future: responses) {
                cpr::Response r = future.get();
                if (r.status_code != 200) {
                    errors++;
                    logger << "Received status code " << r.status_code << " while uploading a tile with x=" << x.first << std::endl;
                }
                completed_parts++;
            }
            logger << "Uploaded " << completed_parts << " parts for x=" << x.first << std::endl;
        }

        logger << "Completed uploading of " << counter << " objects with " << errors << " errors." << std::endl;
    }

}
