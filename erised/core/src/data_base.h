#ifndef __ERISED_MATRIX_BASE__
#define __ERISED_MATRIX_BASE__

#include <initializer_list>
#include <functional>

namespace erised {

template<size_t N>
class Pos {
};

template<>
class Pos<2> {
 public:
  Pos(size_t x, size_t y)
    : x_(x)
    , y_(y) {}

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
  using MapFn = std::function<T(T)>;
  using ReduceFn = std::function<T(T,T)>;

  MatBase() = default;
  virtual ~MatBase() = default;

  virtual void Map(const MapFn& fn) = 0;

  virtual void RowMap(size_t i, MapFn fn) = 0;

  virtual void ColMap(size_t i, MapFn fn) = 0;

  virtual T Reduce(const ReduceFn& fn) const = 0;

  virtual T RowReduce(size_t i, const ReduceFn& fn) const = 0;

  virtual T ColReduce(size_t i, const ReduceFn& fn) const = 0;

  virtual T operator()(const Pos<order>& pos) const = 0;

  virtual size_t NumElements() const = 0;

  virtual size_t NumElementsLine(size_t i) const = 0;

  virtual size_t NumElementsCol(size_t i) const = 0;
};

template<typename T>
using DataBase = MatBase<T, 2>;

enum class Axis{
  ROW,
  COL
};

}
#endif //__ERISED_MATRIX_BASE__
