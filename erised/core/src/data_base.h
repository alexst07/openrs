#ifndef __ERISED_MATRIX_BASE__
#define __ERISED_MATRIX_BASE__

#include <initializer_list>
#include <valarray>

namespace erised {

template<size_t N>
class Pos {
};

template<>
class Pos<2> {
 public:
  Pos(size_t x, size_t y)
    : x_(x)
    , y_(x) {}

  size_t X() const {
    return x_;
  }

  size_t Y() const {
    return y_;
  }

 private:
  size_t x_, y_;
};

/**
 *
 *
 */
template<typename T, size_t N>
class MatBase {
 public:
  static constexpr size_t order = N;

  using value_type = T;
  using MapFn = T(&&)(T);
  using ReduceFn = T(&&)(T, T);

  MatBase() = default;
  virtual ~MatBase() = default;

  virtual void Map(MapFn fn) = 0;

  virtual void RowMap(size_t i, MapFn fn) = 0;

  virtual void ColMap(size_t i, MapFn fn) = 0;

  virtual T Reduce(ReduceFn fn) = 0;

  virtual T RowReduce(size_t i, ReduceFn fn) = 0;

  virtual T ColReduce(size_t i, ReduceFn fn) = 0;
};

template<typename T>
using DataBase = MatBase<T, 2>;

}
#endif //__ERISED_MATRIX_BASE__
