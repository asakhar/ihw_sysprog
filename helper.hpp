#pragma once

template <typename Iter>
auto max(Iter begin, Iter end) {
  auto cur = begin;
  auto maximum = *cur++;
  while (cur != end) {
    if (*cur > maximum) maximum = *cur;
    ++cur;
  }
  return maximum;
}

template <typename Iterable>
auto max(Iterable iterable) {
  return max(iterable.begin(), iterable.end());
}

template <typename Iter, typename Func>
auto foreach (Iter beginIter, Iter endIter, Func func) {
  struct ForEachIterable {
    Iter beginIt;
    Iter const endIt;
    Func functor;
    struct Iterator {
      Iter it;
      ForEachIterable& iterable;
      auto operator!=(Iterator const& other) { return it != other.it; }

      auto operator*() const { return iterable.functor(*it); }

      auto operator++() {
        ++it;
        return *this;
      }
      auto operator++(int) {
        Iterator newit{it, iterable};
        ++it;
        return newit;
      }
    };

    auto begin() { return Iterator{beginIt, *this}; }
    auto end() { return Iterator{endIt, *this}; }
  };
  return ForEachIterable{beginIter, endIter, func};
}