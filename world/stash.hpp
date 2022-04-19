#include <mutex>

#include "osmium/memory/item.hpp"
#include "osmium/storage/item_stash.hpp"

#ifndef RUSTYMON_STASH
#define RUSTYMON_STASH
namespace rustymon {

/**
 * Wraps osmium's ItemStash to behave more like a vector.
 */
template <class T> class Stash {
  typedef osmium::ItemStash::handle_type handle_t;

private:
  osmium::ItemStash stash;
  std::vector<handle_t> handles;

  T &lookup(const handle_t h) const { return stash.get<T>(h); }

public:
  // Read only access to underlying ItemStash
  const osmium::ItemStash &get_stash() { return stash; }

  // Iterator extending std's
  class iterator : public std::vector<handle_t>::iterator {

  private:
    const Stash<T> *stash;

  public:
    iterator(iterator const &iter)
        : std::vector<handle_t>::iterator(iter), stash(iter.stash) {}
    iterator(Stash<T> const *s, std::vector<handle_t>::iterator const &c)
        : std::vector<handle_t>::iterator(c), stash(s) {}

    T &operator*() {
      const handle_t h = std::vector<handle_t>::iterator::operator*();
      return stash->lookup(h);
    }
  };

  // Iterators:
  iterator begin() { return iterator(this, handles.begin()); }
  iterator end() { return iterator(this, handles.end()); }

  // Capacity:
  size_t size() { return handles.size(); }

  // Element Access:
  T &operator[](size_t n) { return lookup(handles[n]); }
  T &at(size_t n) { return lookup(handles.at(n)); }
  T &front() { return lookup(handles.front()); }
  T &back() { return lookup(handles.back()); }

  // Modifiers
  void push_back(const T &t) { handles.push_back(stash.add_item(t)); }
  void pop_back() {
    stash.remove_item(handles.back());
    handles.pop_back();
  }
  void clear() {
    stash.clear();
    handles.clear();
  }
};

/**
 * Synced pool of Stashes
 *
 * It optimises the following workflow, be recycling old Stahes and avoid
 * unnecessary allocations:
 * - populate new Stash with items in osmium::Handler
 * - send it to a worker thread over some queue
 * - delete it once processed
 */
class StashPool {
private:
  std::mutex lock;
  std::vector<Stash<osmium::memory::Item> *> unused;

public:
  template <class T> Stash<T> *new_stash() {
    Stash<T> *stash;
    std::unique_lock<std::mutex> u_lock(lock);
    if (unused.empty()) {
      u_lock.unlock(); // Unlock early since heap allocation doesn't need to block
      stash = new Stash<T>;
    } else {
      Stash<osmium::memory::Item> *i_stash = unused.back();
      stash = reinterpret_cast<Stash<T> *>(i_stash);
      unused.pop_back();
    }
    return stash;
  }
  template <class T> void delete_stash(Stash<T> *stash) {
    stash->clear();
    std::lock_guard<std::mutex> lock_guard(lock);
    unused.push_back(reinterpret_cast<Stash<osmium::memory::Item> *>(stash));
  }
};

} // namespace rustymon
#endif
