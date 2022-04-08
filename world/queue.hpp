namespace rustymon {

    template<typename T>
    class ThreadSafeQueue {

        static const std::size_t max_size = QUEUE_MAX_SIZE;
        constexpr static const std::chrono::milliseconds max_wait{QUEUE_MAX_LOCK_WAIT_MS};

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
            if (max_size) {
                while (size() >= max_size) {
                    std::unique_lock<std::mutex> lock{mutex};
                    space_available.wait_for(lock, max_wait, [this] {
                        return queue.size() < max_size;
                    });

                }
            }
            std::lock_guard<std::mutex> lock{mutex};
            queue.push(std::move(value));
            data_available.notify_one();
        }

        /**
         * Pop an element from the queue. Block until data is available.
         * The element will be moved to the given space.
         */
        void pop(T &value) {
            std::unique_lock<std::mutex> lock{mutex};
            data_available.wait(lock, [this] {
                return !queue.empty();
            });
            if (!queue.empty()) {
                value = std::move(queue.front());
                queue.pop();
                lock.unlock();
                if (max_size) {
                    space_available.notify_one();
                }
            }
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
