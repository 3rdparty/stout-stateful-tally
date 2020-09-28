#pragma once

#include <atomic>
#include <utility> // For 'std::pair'.

#include "stout/atomic-backoff.h"

#include "glog/logging.h"

namespace stout {

template <typename S>
struct stateful_tally
{
  static_assert(
      sizeof(S) == sizeof(unsigned char),
      "State must be representable in a single byte");

  stateful_tally(S s)
  {
    value.store(size_t(s) << ((sizeof(size_t) - 1) * 8));
  }

  size_t count() const
  {
    return (value.load() << 8) >> 8;
  }

  S state() const
  {
    return S(value.load() >> ((sizeof(size_t) - 1) * 8));
  }

  template <typename Predicate>
  std::pair<S, size_t> wait(Predicate&& predicate)
  {
    for (atomic_backoff b; ; b.pause()) {
      size_t loaded = value.load();

      size_t count = (loaded << 8) >> 8;
      size_t state = loaded >> ((sizeof(size_t) - 1) * 8);

      if (predicate(state, count)) {
        return std::make_pair(S(state), count);
      }
    }
  }

  bool increment(S& expected)
  {
    size_t loaded = value.load();
    bool load = false;

    for (atomic_backoff b; ; b.pause()) {
      if (load) {
        loaded = value.load();
        load = false;
      }

      size_t count = (loaded << 8) >> 8;
      size_t state = loaded >> ((sizeof(size_t) - 1) * 8);

      if (state & 128) {
        load = true;
        // std::cout << "waiting on reset (increment)\n";
        continue;
      }

      if (state != size_t(expected)) {
        expected = S(state);
        // std::cout << "state != expected (increment)\n";
        return false;
      }

      CHECK(count + 1 < (size_t(1) << ((sizeof(size_t) - 1) * 8)))
        << "Count overflow";

      if (!value.compare_exchange_weak(
              loaded,
              (size_t(state) << ((sizeof(size_t) - 1) * 8)) | (count + 1))) {
        continue;
      }

      return true;
    }
  }

  std::pair<S, size_t> decrement()
  {
    size_t loaded = value.load();

    for (atomic_backoff b; ; b.pause()) {
      size_t count = (loaded << 8) >> 8;
      size_t state = loaded >> ((sizeof(size_t) - 1) * 8);

      CHECK(count != 0) << "Count is 0";

      count -= 1;

      if (!value.compare_exchange_weak(
              loaded,
              (size_t(state) << ((sizeof(size_t) - 1) * 8)) | count)) {
        continue;
      }

      return std::make_pair(S(state), count);
    }
  }

  bool update(S& expected, S desired)
  {
    CHECK(size_t(desired) <= 127) << "State is unrepresentable";

    size_t loaded = value.load();
    bool load = false;

    for (atomic_backoff b; ; b.pause()) {
      if (load) {
        loaded = value.load();
        load = false;
      }

      size_t count = (loaded << 8) >> 8;
      size_t state = loaded >> ((sizeof(size_t) - 1) * 8);

      // TODO(benh): valid to do an update when resetting?
      if (state & 128) {
        load = true;
        // std::cout << "waiting on reset (update)\n";
        continue;
      }

      if (state != size_t(expected)) {
        expected = S(state);
        return false;
      }

      if (!value.compare_exchange_weak(
              loaded,
              (size_t(desired) << ((sizeof(size_t) - 1) * 8)) | count)) {
        continue;
      }

      return true;
    }
  }

  bool update(S& expected, size_t& countexpected, S desired, size_t countdesired)
  {
    CHECK(size_t(desired) <= 127) << "State is unrepresentable";

    size_t loaded = value.load();
    bool load = false;

    for (atomic_backoff b; ; b.pause()) {
      if (load) {
        loaded = value.load();
        load = false;
      }

      size_t count = (loaded << 8) >> 8;
      size_t state = loaded >> ((sizeof(size_t) - 1) * 8);

      if (state & 128) {
        load = true;
        // std::cout << "waiting on reset (update v2)\n";
        continue;
      }

      if (state != size_t(expected)) {
        expected = S(state);
        return false;
      }

      if (count != countexpected) {
        countexpected = count;
        return false;
      }

      if (!value.compare_exchange_weak(
              loaded,
              (size_t(desired) << ((sizeof(size_t) - 1) * 8)) | countdesired)) {
        continue;
      }

      return true;
    }
  }

  template <typename F>
  bool reset(S& expected, S desired, F&& f)
  {
    CHECK(size_t(desired) <= 127) << "State is unrepresentable";

    size_t loaded = value.load();
    bool load = false;

    for (atomic_backoff b; ; b.pause()) {
      if (load) {
        loaded = value.load();
        load = false;
      }

      size_t count = (loaded << 8) >> 8;
      size_t state = loaded >> ((sizeof(size_t) - 1) * 8);

      if (state & 128) {
        load = true;
        // std::cout << "waiting on reset (reset)\n";
        continue;
      }

      if (state != size_t(expected)) {
        expected = S(state);
        return false;
      }

      if (!value.compare_exchange_weak(
              loaded,
              ((state | 128) << ((sizeof(size_t) - 1) * 8)) | count)) {
        continue;
      }

      for (atomic_backoff b; ; b.pause()) {
        if (count > 0) {
          loaded = value.load();
          count = (loaded << 8) >> 8;
        } else {
          break;
        }
      }

      f();

      loaded = (state | 128) << ((sizeof(size_t) - 1) * 8);

      CHECK(
          value.compare_exchange_weak(
              loaded,
              (size_t(desired) << ((sizeof(size_t) - 1) * 8))));

      return true;
    }
  }

  std::atomic<size_t> value = 0;
};

} // namespace stout {
