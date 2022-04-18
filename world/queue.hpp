#include "constants.hpp"

namespace rustymon {

    template<typename T>
    class ThreadSafeQueue {

        static const std::size_t max_size = QUEUE_MAX_SIZE;

        mutable std::mutex mutex;
        std::queue<T> queue;

        /// Used to signal consumers when data is available in the queue.
        std::condition_variable data_available;

        /// Used to signal producers when queue is not full.
        std::condition_variable space_available;

    public:

        /**
         * Push an element onto the queue. Block until space is available.
         * The element will be moved from the given space.
         */
        void push(T value) {
            std::unique_lock<std::mutex> lock{mutex};
            while (queue.size() >= max_size) {
                space_available.wait_for(lock, std::chrono::milliseconds{QUEUE_MAX_LOCK_WAIT_MS});
            }
            queue.push(std::move(value));
            data_available.notify_one();
        }

        /**
         * Pop an element from the queue. Block until data is available and the predicate is true.
         * Throw a std::runtime_error when the queue is empty while the predicate became false.
         * The element will be moved to the given space.
         */
        template <typename P>
        void pop(T &value, P predicate = [](){return true;}) {
            std::unique_lock<std::mutex> lock{mutex};
            while (queue.empty() && predicate()) {
                data_available.wait_for(lock, std::chrono::milliseconds{QUEUE_MAX_LOCK_WAIT_MS});
            }
            if (queue.empty()) {
                throw std::runtime_error("empty queue");
            }
            value = std::move(queue.front());
            queue.pop();
            space_available.notify_one();
        }

        /**
         * Pop an element from the queue. Block until data is available
         * The element will be moved to the given space.
         */
        void pop(T &value) {
            return pop(value, [](){return true;});
        }

        /**
         * Check if the queue is empty.
         */
        bool empty() const {
            std::lock_guard<std::mutex> lock{mutex};
            return queue.empty();
        }

        /**
         * Get the size of the queue.
         */
        std::size_t size() const {
            std::lock_guard<std::mutex> lock{mutex};
            return queue.size();
        }
    };

}
