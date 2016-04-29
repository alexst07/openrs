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

  MatBase() = default;
  virtual ~MatBase() = default;

  template<class Fn = void(T)>
  virtual void Map(Fn&& fn) = 0;

  template<class Fn = void(T)>
  virtual void RowMap(size_t i, Fn&& fn) = 0;

  template<class Fn = void(T)>
  virtual void ColMap(size_t i, Fn&& fn) = 0;

  template<class Fn = T(T,T)>
  virtual T Reduce(Fn&& fn) = 0;

  template<class Fn = T(T,T)>
  virtual T RowReduce(size_t i, Fn&& fn) = 0;

  template<class Fn = T(T,T)>
  virtual T ColReduce(size_t i, Fn&& fn) = 0;
};

template<typename T>
typedef MatBase<T, 2> DataBase;

}
#endif //__ERISED_MATRIX_BASE__
