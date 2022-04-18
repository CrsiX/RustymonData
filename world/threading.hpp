#include <thread>

#include "osmium/handler.hpp"
#include "osmium/osm/area.hpp"
#include "osmium/osm/node.hpp"
#include "osmium/osm/way.hpp"

#include "queue.hpp"
#include "stash.hpp"

#ifndef RUSTYMON_THREADING
#define RUSTYMON_THREADING
namespace rustymon {

/**
 * An osmium Handler which forwards items (Node, Way, Area) to a pool of
 * threads.
 *
 * Public methods:
 * - `spawn` is used to create and start processing threads
 * - `stop` is sending a stop signal to all threads
 * - `join` joins with all threads
 */
class ThreadedHandler : public osmium::handler::Handler {
private:
  /**
   * A worker is a function object run as thread.
   *
   * It stores pointers to a queue to get items from and to a pool to reuse the
   * items' container.
   * It also stores the function pointer to call with items and an arbitary
   * pointer for arbitary parameters.
   *
   * Since items are send inside a container which is passed as pointer over the
   * queue, a null pointer is used to signal aa stop.
   */
  template <class T> struct Worker {
    ThreadSafeQueue<Stash<T> *> *stash_queue = 0;
    StashPool *stash_pool = 0;
    void (*func)(const T &, void *);
    void *params = 0;

    void operator()();
  };

  /**
   * A SingleHandler wraps the handler logic required for a single item type
   *
   * It holds and fills an item container,
   * sends the full container over its queue to its threads,
   * as well as managing the threads for its item type.
   */
  template <class T> struct SingleHandler {
    StashPool *stash_pool;
    ThreadSafeQueue<Stash<T> *> stash_queue;
    Stash<T> *stash = 0;
    std::vector<std::thread> workers;

    SingleHandler(StashPool *);
    void spawn(void (*)(const T &, void *), void *);
    void handle(const T &);
    void stop();
    void join();
  };

  /**
   * Pool of item containers used ones return to from worker threads to be
   * reused.
   */
  StashPool stash_pool;

  SingleHandler<osmium::Node> node_handler;
  SingleHandler<osmium::Way> way_handler;
  SingleHandler<osmium::Area> area_handler;

public:
  void node(const osmium::Node &);
  void way(const osmium::Way &);
  void area(const osmium::Area &);
  ThreadedHandler();

  /**
   * Spawn a thread for processing an item type (Node, Way, Area) using a
   * handler function. The handler function is repeatedly called with a const
   * reference to an item and an arbitary pointer for additional arguments. This
   * pointer is the second argument to spawn and forwarded to the handler
   * function on every item.
   */
  void spawn(void (*)(const osmium::Node &, void *), void *);
  void spawn(void (*)(const osmium::Way &, void *), void *);
  void spawn(void (*)(const osmium::Area &, void *), void *);

  void stop();

  void join();
};

} // namespace rustymon
#endif
