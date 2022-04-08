#include <string>

namespace rustymon {

    static const int FILE_VERSION = 1;
    static const char BBOX_SPLIT_CHAR = '/';
    static const std::string DEFAULT_CONFIG_FILENAME = "config.json";

    static const int QUEUE_MAX_SIZE = 16 * 1024;
    static const int QUEUE_MAX_LOCK_WAIT_MS = 2;

}
