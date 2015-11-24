#ifndef _INJECT_HPP_
#define _INJECT_HPP_

#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <set>

#include <type_traits>

template <typename, typename>
  struct inject;

template <typename C, typename T>
using inject_t = typename inject<C,T>::type;

template <
  template <typename, typename...> class TT,
  typename T,
  typename ...Ts,
  typename U
>
struct inject<TT<T,Ts...>,U>     { using type = TT<U,inject_t<Ts,U>...>; };

template <typename T, std::size_t N, typename U>
struct inject<std::array<T,N>,U> { using type = std::array<U,N>;         };

template <typename T, std::size_t N, typename U>
struct inject<T[N],U>            { using type = U[N];                    };

namespace static_asserts {

using namespace std;
static_assert(is_same<inject_t<int[42],char>, char[42]>::value,"");
static_assert(is_same<inject_t<array<int,43>,char>, array<char,43>>::value,"");
static_assert(is_same<inject_t<vector<int>,double>, vector<double>>::value,"");
static_assert(is_same<inject_t<deque<char>,bool>,   deque<bool>>::value,"");
static_assert(is_same<inject_t<list<int>,char>,     list<char>>::value,"");
static_assert(is_same<inject_t<set<bool>,short>,    set<short>>::value,"");
static_assert(is_same<
                inject_t<forward_list<vector<int>>,bool>,
                forward_list<bool>
              >::value,"");

} // namespace static_asserts

#endif // _INJECT_HPP_
