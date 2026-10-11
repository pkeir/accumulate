#include <vector>
#include <flat_set>
#include <ranges>
#include <algorithm>
#include <functional>
#include "inject.hpp"

int main()
{
  using std::vector; using std::flat_set;
  using std::plus; using std::multiplies;
  using std::logical_and; using std::logical_or;
  using std::ranges::fold_right; using std::ranges::fold_left;

  auto sum     = [](auto xs) { return fold_right(xs, 0, plus<>{}); };
  auto product = [](auto xs) { return fold_right(xs, 1, multiplies<>{}); };
  auto and_    = [](auto xs) { return fold_right(xs, true, logical_and<>{}); };
  auto or_     = [](auto xs) { return fold_right(xs, false, logical_or<>{}); };

  static_assert(10 == sum(vector{1,2,3,4}));
  static_assert(24 == product(vector{1,2,3,4}));
  static_assert(false == and_(flat_set{true,false})); // flat_set; we're generic
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
  static_assert(flat_set{1,2,3} == id(flat_set{1,2,3}));

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

  auto map = [=](auto f, auto xs){
    using t  = decltype(f(head(xs)));
    using c2 = inject_t<decltype(xs), t>;
    return fold_right(xs, c2{}, [=](auto y, auto ys){ return cons(f(y), ys); });
  };
  static_assert(vector{2,1} == map(length, vector{vector{1,2}, vector{3}}));

  return 0;
}
