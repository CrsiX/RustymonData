#include "osmium/handler.hpp"

#include "queue.hpp"
#include "stash.cpp"

namespace rustymon {

class ThreadedHandler : public osmium::handler::Handler {
private:
  template <class T> struct Worker {
    ThreadSafeQueue<Stash<T> *> *stash_queue = 0;
    StashPool *stash_pool = 0;
    void (*func)(const T &, void *);
    void *params = 0;

    void operator()() {
      while (true) {
        Stash<T> *stash = 0;
        stash_queue->pop(stash);
        if (stash == 0)
          break;
        for (T &t : *stash) {
          func(t, params);
        }
        stash_pool->delete_stash(stash);
      }
    }
  };

  template <class T> struct SingleHandler {
    StashPool *stash_pool;
    ThreadSafeQueue<Stash<T> *> stash_queue;
    Stash<T> *stash = 0;
    std::vector<std::thread> workers;

    SingleHandler(StashPool *pool)
        : stash_pool(pool), stash_queue(), stash(0) {}

    void spawn(void (*func)(const T &, void *), void *params) {
      Worker<T> worker;
      worker.stash_queue = &stash_queue;
      worker.stash_pool = stash_pool;
      worker.func = func;
      worker.params = params;
      workers.emplace_back(worker);
    }

    void handle(const T &t) {
      if (stash == 0)
        stash = stash_pool->new_stash<T>();
      stash->push_back(t);
      if (stash->size() >= 1024) {
        stash_queue.push(stash);
        stash = 0;
      }
    }

    void stop() {
      for (size_t i = 0; i < workers.size(); i++)
        stash_queue.push(0);
    }

    void join() {
      for (std::thread &worker_thread : workers)
        worker_thread.join();
    }
  };

  StashPool stash_pool;
  SingleHandler<osmium::Node> node_handler;
  SingleHandler<osmium::Way> way_handler;
  SingleHandler<osmium::Area> area_handler;

public:
  ThreadedHandler()
      : stash_pool(), node_handler(&stash_pool), way_handler(&stash_pool),
        area_handler(&stash_pool) {}

  void spawn(void (*func)(const osmium::Node &, void *), void *params) {
    node_handler.spawn(func, params);
  }
  void spawn(void (*func)(const osmium::Way &, void *), void *params) {
    way_handler.spawn(func, params);
  }
  void spawn(void (*func)(const osmium::Area &, void *), void *params) {
    area_handler.spawn(func, params);
  }

  void stop() {
    node_handler.stop();
    way_handler.stop();
    area_handler.stop();
  }
  void join() {
    node_handler.join();
    way_handler.join();
    area_handler.join();
  }

  void node(const osmium::Node &node) { node_handler.handle(node); }
  void way(const osmium::Way &way) { way_handler.handle(way); }
  void area(const osmium::Area &area) { area_handler.handle(area); }
};

} // namespace rustymon
