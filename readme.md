Files to accompany the ACCU Overload #130 article:

**A Tutorial on the Universality and Expressiveness of std::accumulate**\
Paul Keir

  https://accu.org/index.php/journals/2182

To compile:

```
g++ -std=c++14 accu.cpp
```

Plus, a C++26 version using Ranges' `std::fold_right` and a touch of constexpr
std::deque.

To compile:

```
g++ -std=c++26 accu2.cpp
```
