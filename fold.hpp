#ifndef _FOLD_HPP_
#define _FOLD_HPP_

#include <numeric>
#include <functional>

template <typename F>
inline auto flip(const F &f) {
  using namespace std::placeholders;
  return std::bind(f,_2,_1);
}

template <typename F, typename T, typename C>
inline T foldl(const F &f, const T &z, const C &c) {
  return std::accumulate(c.cbegin(), c.cend(), z, f);
}

template <typename F, typename T, typename C>
inline T foldr(const F &f, const T &z, const C &c) {
  return std::accumulate(c.crbegin(), c.crend(), z, flip(f));
}

#endif // _FOLD_HPP_
