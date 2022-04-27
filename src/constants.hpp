#ifndef WORLD_GENERATOR_CONSTANTS_HPP
#define WORLD_GENERATOR_CONSTANTS_HPP

#include <string>
#include <thread>

namespace rustymon {

    static const int FILE_VERSION = 1;
    static const char BBOX_SPLIT_CHAR = '/';
    static const std::string DEFAULT_CONFIG_FILENAME = "config.json";  // NOLINT

    static const int QUEUE_MAX_SIZE = 16 * 1024;
    static const int QUEUE_MAX_LOCK_WAIT_MS = 2;

    static const int X_SIZE_FACTOR_DEFAULT = 10000;
    static const int Y_SIZE_FACTOR_DEFAULT = 10000;

    static const int NODE_DEFAULT_WORKER_THREADS = static_cast<int>(std::thread::hardware_concurrency());
    static const int WAY_DEFAULT_WORKER_THREADS = 2 * static_cast<int>(std::thread::hardware_concurrency());
    static const int AREA_DEFAULT_WORKER_THREADS = 4 * static_cast<int>(std::thread::hardware_concurrency());
    static const int UPLOAD_DEFAULT_WORKER_THREADS = 2 * static_cast<int>(std::thread::hardware_concurrency());

}

#endif //WORLD_GENERATOR_CONSTANTS_HPP
