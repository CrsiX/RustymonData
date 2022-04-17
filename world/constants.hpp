#ifndef WORLD_GENERATOR_CONSTANTS_HPP
#define WORLD_GENERATOR_CONSTANTS_HPP

#include <string>

namespace rustymon {

    static const int FILE_VERSION = 1;
    static const char BBOX_SPLIT_CHAR = '/';
    static const std::string DEFAULT_CONFIG_FILENAME = "config.json";

    static const int QUEUE_MAX_SIZE = 16 * 1024;
    static const int QUEUE_MAX_LOCK_WAIT_MS = 2;

    static const int NODE_DEFAULT_WORKER_THREADS = 4;
    static const int WAY_DEFAULT_WORKER_THREADS = 2;
    static const int AREA_DEFAULT_WORKER_THREADS = 4;

}

#endif //WORLD_GENERATOR_CONSTANTS_HPP
