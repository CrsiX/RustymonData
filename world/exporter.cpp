#include "exporter.hpp"

namespace rustymon {

    namespace detail {

        std::pair<int, int> export_world_to_http_worker(const structs::World &world, const std::string &push_url, const std::string &auth_info, std::ostream &logger, const int worker_count, const int my_modulo) {
            cpr::Header headers{{"Content-Type", "application/json"}};
            if (!auth_info.empty()) {
                headers.insert({"Authorization", auth_info});
            }

            cpr::Session session;
            session.SetUrl(cpr::Url{push_url});

            int errors = 0;
            int total_requests = 0;
            for (auto const& x : world) {
                if (x.first % worker_count != my_modulo) {
                    continue;
                }

                for (auto const &y: x.second) {
                    std::stringstream body;
                    body << y.second;
                    std::stringstream position_header;

                    position_header << x.first << "," << y.first;
                    headers.erase("X-Tile-Position");
                    headers.insert({"X-Tile-Position", position_header.str()});
                    session.SetBody(cpr::Body{body.str()});
                    session.SetHeader(headers);

                    cpr::Response r = session.Post();
                    total_requests++;
                    if (r.status_code != 200) {
                        errors++;
                        logger << "Received status code " << r.status_code << " while uploading Tile " << x.first << "," << y.first << std::endl;
                    }
                }
            }

            return std::pair<int, int>{total_requests, errors};
        }

    }

    void export_world_to_file(const structs::World &world, const std::string &filename, std::ostream &logger) {
        std::ofstream output_file_stream(filename);
        structs::stream(output_file_stream, world);
        output_file_stream.close();
    }

    void export_world_to_files(const structs::World &world, const std::string &directory, std::ostream &logger) {
        // TODO: Implement this function
    }

    void export_world_to_http(const structs::World &world, const std::string &push_url, const std::string &auth_info, std::ostream &logger, const int worker_threads) {
        int error_count;
        int total_requests;

        std::mutex result_mutex;
        std::vector<std::thread> thread_pool;
        thread_pool.reserve(worker_threads);
        for (int i = 0; i < worker_threads; i++) {
            thread_pool.emplace_back([&world, &push_url, &auth_info, &logger, worker_threads, i, &result_mutex, &error_count, &total_requests](){
                logger << "Starting upload worker thread " << i << " of " << worker_threads << " with ID " << std::this_thread::get_id() << std::endl;
                std::pair<int, int> result = detail::export_world_to_http_worker(world, push_url, auth_info, logger, worker_threads, i);
                {
                    std::unique_lock<std::mutex> lock(result_mutex);
                    total_requests += result.first;
                    error_count += result.second;
                }
            });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        for (std::thread &t: thread_pool) {
            logger << "Joining thread ID " << t.get_id() << "..." << std::endl;
            t.join();
        }
        logger << "Completed uploading of " << total_requests << " objects with " << error_count << " errors." << std::endl;
    }

}
