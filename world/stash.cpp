#include "osmium/storage/item_stash.hpp"

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
  void pop_back(const T &t) {
    stash.remove_item(handles.back());
    handles.pop_back();
  }
};

} // namespace rustymon
