#ifndef _CONTAINERS_OSTREAM_HPP_
#define _CONTAINERS_OSTREAM_HPP_

#include <iostream>

template <typename T, typename U>
inline std::ostream &operator<<(std::ostream &, const std::pair<T,U> &);
inline std::ostream &operator<<(std::ostream &, const std::tuple<> &);
template <typename T, typename ...Ts>
inline std::ostream &operator<<(std::ostream &, const std::tuple<T,Ts...> &);

template <typename T, typename U>
inline std::ostream &operator<<(std::ostream &os, const std::pair<T,U> &p) 
{
  os << '(' << p.first << ',' << p.second << ')';
  return os;
}

// ostream overload for C++ standard containers
template <template <typename, typename...> class TT, typename T, typename ...Ts>
inline std::ostream &operator<<(std::ostream &os, const TT<T,Ts...> &xs) 
{
  if (xs.empty()) { os << "{}"; return os; }

  auto it = xs.begin();
  os << '{' << *it++;
  for (; it != xs.end(); ++it) { os << ',' << *it; }
  os << '}';
  return os;
}

#endif // _CONTAINERS_OSTREAM_HPP_
