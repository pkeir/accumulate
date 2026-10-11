#include <vector>
#include <ranges>
#include <algorithm>
#include <functional>

int main()
{
  using std::vector;
  using std::plus; using std::multiplies;
  using std::logical_and; using std::logical_or;
  using std::ranges::fold_right; using std::ranges::fold_left;

  auto sum     = [](auto xs){ return fold_right(xs, 0, plus<>{}); };
  auto product = [](auto xs){ return fold_right(xs, 1, multiplies<>{}); };
  auto and_    = [](auto xs){ return fold_right(xs, true, logical_and<>{}); };
  auto or_     = [](auto xs){ return fold_right(xs, false, logical_or<>{}); };

  static_assert(10 == sum(vector{1,2,3,4}));
  static_assert(24 == product(vector{1,2,3,4}));
  static_assert(false == and_(vector{true,false}));
  static_assert(true == or_(vector{true,false}));

  auto is_foldr = [](auto f) {
    return f(vector{true}, false, [](bool x, bool){ return x; });
  };

  static_assert(true  == is_foldr(fold_right));
  static_assert(false == is_foldr(fold_left));

  return 0;
}
