#include <vector>
#include <deque>
#include <ranges>
#include <algorithm>
#include <functional>
#include <cassert>
#include "inject.hpp"

int main()
{
  using std::vector; using std::deque;
  using std::plus; using std::multiplies;
  using std::logical_and; using std::logical_or;
  using std::ranges::fold_right; using std::ranges::fold_left;

  auto sum     = [](auto xs) { return fold_right(xs, 0, plus{}); };
  auto product = [](auto xs) { return fold_right(xs, 1, multiplies{}); };
  auto and_    = [](auto xs) { return fold_right(xs, true, logical_and{}); };
  auto or_     = [](auto xs) { return fold_right(xs, false, logical_or{}); };

  static_assert(10 == sum(vector{1,2,3,4}));
  static_assert(24 == product(vector{1,2,3,4}));
  static_assert(false == and_(deque{true,false})); // deque; we're generic
  static_assert(true == or_(vector{true,false}));

  auto is_foldr = [](auto f) {
    return f(vector{true}, false, [](bool x, bool){ return x; });
  };

  static_assert(true  == is_foldr(fold_right));
  static_assert(false == is_foldr(fold_left));

  auto cons = [](auto x, auto xs) {
    xs.insert(xs.begin(), x);
    return xs;
  };
  auto id = [=](auto xs) { return fold_right(xs, decltype(xs){}, cons); };
  static_assert(deque{1,2,3} == id(deque{1,2,3}));

  auto append = [=](auto xs, auto ys) { return fold_right(xs, ys, cons); };
  static_assert(vector{1,2,3,4,5,6} == append(vector{1,2,3}, vector{4,5,6}));

  auto length = [](auto xs) {
    return fold_right(xs, 0, [=](auto, auto n){ return 1 + n; });
  };
  static_assert(3 == length(vector{1,2,3}));

  auto reverse = [=](auto xs) {
    using c = decltype(xs);
    return fold_right(xs, c{}, [=](auto y, auto ys){ return append(ys,c{y}); });
  };
  static_assert(vector{3,2,1} == reverse(vector{1,2,3}));

  auto head = [](auto xs) { return *begin(xs); };
  static_assert(1 == head(vector{1,2,3}));

  auto map = [=](auto f, auto xs) {
    using t  = decltype(f(head(xs)));
    using c2 = inject_t<decltype(xs), t>;
    return fold_right(xs, c2{}, [=](auto y, auto ys){ return cons(f(y), ys); });
  };
  static_assert(vector{2,1} == map(length, vector{vector{1,2}, vector{3}}));

  auto filter = [=](auto p, auto xs) {
    return fold_right(
      xs,
      decltype(xs){},
      [=](auto y, auto ys){ return p(y) ? cons(y,ys) : ys; }
    );
  };
  static_assert(vector{1,3} == filter([](auto x){ return x%2; },vector{1,2,3}));

  auto sum_length = [=](auto xs) {
    return fold_right(
      xs,
      std::pair(0,0),
      [=](auto n, auto p){ return std::pair(n + p.first, 1 + p.second); }
    );
  };
  static_assert(std::pair(6,3) == sum_length(vector{1,2,3}));

  auto dropWhileB = [=](auto p, auto xs) {
    return fold_right(
      xs,
      std::pair(decltype(xs){},decltype(xs){}),
      [=](auto x, auto a){
        return std::pair(p(x) ? a.first : cons(x,a.second), cons(x,a.second));
      }
    );
  };
  auto dropWhile = [=](auto p, auto xs) { return dropWhileB(p,xs).first; };
  static_assert(
    vector{4,5,6} == dropWhile([](auto x){ return x<4; }, vector{1,2,3,4,5,6})
  );

  auto compose = [=](auto fs) {
    using fn_t = typename decltype(fs)::value_type; // std::function<T(T)>
    return fold_right(
      fs,
      std::identity{},
      [=](auto f, auto g) { return fn_t{ [=](auto x){ return f(g(x)); } }; }
    );
  };
  auto incr = std::function<int(int)>([=](int x){ return x+1; });
  //static_assert(3 == compose(vector{incr,incr,incr})(0)); // ach no constexpr!
  assert(3 == compose(vector{incr,incr,incr})(0));

  auto foldl = [=](auto xs, auto z, auto f) {
    using z_t  = decltype(z);
    using fn_t = std::function<z_t(z_t)>;
    return fold_right(
      xs,
      std::identity{},
      [=](auto x, auto g) { return fn_t{[=](auto a){ return g(f(a,x)); }}; }
    )(z);
  };
  // static_assert(-6 == foldl(vector{1,2,3}, 0, std::minus{})); // ach
  // static_assert(false == is_foldr(foldl));                    // no constexpr
  assert(-6 == foldl(vector{1,2,3}, 0, std::minus{}));
  assert(false == is_foldr(foldl));

  auto ack = [=](auto xs, auto ys) {
    using ys_t = decltype(ys);
    using fn_t = std::function<ys_t(ys_t)>;
    return [=](auto zs) {
      return fold_right(
        zs,
        fn_t{ [=](auto bs) { return cons(1,bs); } },
        [=](auto, auto g) {
          return fn_t {
            [=](auto ws) {
              return fold_right(
                ws,
                g(ys_t{1}),
                [=](auto, auto as) { return g(as); }
              );
            }
          };
        }
      );
    }(xs)(ys);
  };
  // static_assert(vector(29, 1) == ack(deque{'a','a','a'}, vector{7,7}));
  assert(vector(29, 1) == ack(deque{'a','a','a'}, vector{7,7}));

  return 0;
}
