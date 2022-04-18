#include <thread>

#include "threading.hpp"

namespace rustymon {

// Worker<T>
template <class T> void ThreadedHandler::Worker<T>::operator()() {
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

// SingleHandler<T>
template <class T>
ThreadedHandler::SingleHandler<T>::SingleHandler(StashPool *pool)
    : stash_pool(pool), stash_queue(), stash(0) {}

template <class T>
void ThreadedHandler::SingleHandler<T>::spawn(void (*func)(const T &, void *),
                                              void *params) {
  ThreadedHandler::Worker<T> worker;
  worker.stash_queue = &stash_queue;
  worker.stash_pool = stash_pool;
  worker.func = func;
  worker.params = params;
  workers.emplace_back(worker);
}

template <class T> void ThreadedHandler::SingleHandler<T>::handle(const T &t) {
  if (stash == 0)
    stash = stash_pool->new_stash<T>();
  stash->push_back(t);
  if (stash->size() >= 1024) {
    stash_queue.push(stash);
    stash = 0;
  }
}

template <class T> void ThreadedHandler::SingleHandler<T>::stop() {
  for (size_t i = 0; i < workers.size(); i++)
    stash_queue.push(0);
}

template <class T> void ThreadedHandler::SingleHandler<T>::join() {
  for (std::thread &worker_thread : workers)
    worker_thread.join();
}

// ThreadedHandler
ThreadedHandler::ThreadedHandler()
    : stash_pool(), node_handler(&stash_pool), way_handler(&stash_pool),
      area_handler(&stash_pool) {}

void ThreadedHandler::spawn(void (*func)(const osmium::Node &, void *),
                            void *params) {
  node_handler.spawn(func, params);
}
void ThreadedHandler::spawn(void (*func)(const osmium::Way &, void *),
                            void *params) {
  way_handler.spawn(func, params);
}
void ThreadedHandler::spawn(void (*func)(const osmium::Area &, void *),
                            void *params) {
  area_handler.spawn(func, params);
}

void ThreadedHandler::stop() {
  node_handler.stop();
  way_handler.stop();
  area_handler.stop();
}
void ThreadedHandler::join() {
  node_handler.join();
  way_handler.join();
  area_handler.join();
}

void ThreadedHandler::node(const osmium::Node &node) {
  node_handler.handle(node);
}
void ThreadedHandler::way(const osmium::Way &way) { way_handler.handle(way); }
void ThreadedHandler::area(const osmium::Area &area) {
  area_handler.handle(area);
}

} // namespace rustymon
