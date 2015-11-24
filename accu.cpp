#include "inject.hpp"
#include "container_iostream.hpp"
#include "fold.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <utility>

template <template <typename, typename...> class TT = std::vector, typename T>
inline auto mk(){ return TT<T>{}; } 

template <
  template <typename, typename...> class TT = std::vector,
  typename T,
  typename ...Ts
>
inline auto mk(const T x, const Ts ...xs){ return TT<T>{x,xs...}; } 

static_assert(std::is_same<decltype(mk(1,2,3)), std::vector<int>>::value,"");

int main(int argc, char *argv[])
{
  using std::cout;
  std::list<int>         xs{1,2,3,4};

  auto sum     = [](auto xs){ return foldr(std::plus<>{},        0,     xs); };
  auto product = [](auto xs){ return foldr(std::multiplies<>{},  1,     xs); };
  auto and_    = [](auto xs){ return foldr(std::logical_and<>{}, true,  xs); };
  auto or_     = [](auto xs){ return foldr(std::logical_or<>{},  false, xs); };
  cout << "sum:      " << sum(mk(1,2,3,4))     << '\n';
  cout << "product:  " << product(mk(1,2,3,4)) << '\n';
  cout << "and:      " << and_(mk(true,false)) << '\n';
  cout << "or:       " << or_(mk(true,false))  << '\n';

  bool is_foldrA = foldr([](auto x, auto){ return x; }, false, mk(true));
  bool is_foldrB = foldl([](auto x, auto){ return x; }, false, mk(true));
  cout << is_foldrA << ' ' << is_foldrB << '\n';

  auto cons = [=](auto x, auto xs) {
    decltype(xs) ys{x};
    std::copy(begin(xs), end(xs), std::back_inserter(ys));
    return ys;
  };
  auto id = [=](auto xs){ return foldr(cons, decltype(xs){}, xs); };
  cout << "id:       " << id(mk(1,2,3,4))     << '\n';
  static_assert(std::is_same<
                  decltype(foldr(cons, std::list<int>{}, mk(1,2,3))),
                  std::list<int>
                >::value,"");

  auto append = [=](auto xs, auto ys){ return foldr(cons, ys, xs); };
  cout << "append:   " << append(mk(1,2,3),mk<std::list>(4,5,6)) << '\n';
  static_assert(std::is_same<
                  decltype(append(mk(1,2,3),mk<std::list>(4,5,6))),
                  std::list<int>
                >::value,"");

  auto length = [=](auto xs){
    return foldr([=](auto, auto n){ return 1+n; }, 0, xs);
  };
  cout << "length:   " << length(mk(1,2,3)) << '\n';

  auto reverse = [=](auto xs){
    return foldr([=](auto y, auto ys){ return append(ys,decltype(xs){y}); },
                 decltype(xs){},
                 xs);
  };
  cout << "reverse:  " << reverse(mk(1,2,3)) << '\n';

  auto head = [=](auto xs) { return *begin(xs); };
  static_assert(std::is_same<decltype(head(std::list<char>{})),char>::value,"");

  auto map = [=](auto f, auto xs){
    using r_t  = decltype(f(head(xs)));
    using rs_t = inject_t<decltype(xs),r_t>;
    return foldr(
      [=](auto y, auto ys){ return cons(f(y),ys); },
      rs_t{},
      xs
    );
  };
  cout << "map:      " << map([](auto x){ return x+1; }, mk(1,2,3)) << '\n';
  cout << "map:      " << map(length, mk(mk(1,2),mk(3))) << '\n';

  auto filter = [=](auto p, auto xs){
    return foldr(
      [=](auto y, auto ys){ return p(y) ? cons(y,ys) : ys; },
      decltype(xs){},
      xs
    );
  };
  cout << "filter:   " << filter([](auto x){ return x%2; },mk(1,2,3)) <<'\n';

  auto sumlength = [=](auto xs){
    return foldr(
      [=](auto n, auto p){ return std::make_pair(n + p.first, 1 + p.second); },
      std::make_pair(0,0),
      xs
    );
  };
  cout << "sumlength:" << sumlength(mk(1,2,3)) <<'\n';

  auto dropWhileB = [=](auto p, auto xs){
    return foldr(
      [=](auto x, auto a){
        return make_pair(p(x) ? a.first : cons(x,a.second), cons(x,a.second));
      },
      make_pair(decltype(xs){},decltype(xs){}),
      xs
    );
  };
  auto dropWhile = [=](auto p, auto xs){ return dropWhileB(p,xs).first; };
  cout << "dropWhile:" << dropWhile([](auto x){ return x<4; }, mk(1,2,3,4,5,6)) <<'\n';

  auto compose = [=](auto fs){
    using fn_t = typename decltype(fs)::value_type; // std::function<T(T)>
    return foldr(
      [=](auto f, auto g){ return [=](auto x){ return f(g(x)); }; },
      fn_t([=](auto x){ return x; }), // id
      fs
    );
  };
  auto incr = std::function<int(int)>([=](int x){ return x+1; });
  cout << "compose:  " << compose(mk(incr,incr,incr))(0) << '\n';

  auto foldl = [=](auto f, auto z, auto xs){
    using z_t  = decltype(z);
    using fn_t = std::function<z_t(z_t)>;
    return foldr(
      [=](auto x, auto g) { return [=](auto a){ return g(f(a,x)); }; },
      fn_t([=](auto x){ return x; }), // id
      xs
    )(z);
  };
  cout << "foldl:    " << foldl(std::minus<>{}, 0, mk(1,2,3)) << '\n';
  bool is_foldrC = foldl([](auto x, auto){ return x; }, false, mk(true));
  cout << is_foldrC << '\n';

  auto ack = [=](auto xs, auto ys){
    using ys_t = decltype(ys);
    using fn_t = std::function<ys_t(ys_t)>;
    return [=](auto zs){
      return foldr(
        [=](auto, auto g){
          return [=](auto ws){
            return foldr(
              [=](auto, auto as){ return g(as); },
              g(ys_t{1}),
              ws
            );
          };
        },
        fn_t([=](auto bs){ return cons(1,bs); }),
        zs
      );
    }(xs)(ys);
  };
  cout << "ack:      " << ack(mk<std::list>('a','a','a'),mk(7,7)) << '\n';

  return 0;
}

