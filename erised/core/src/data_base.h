#ifndef __ERISED_MATRIX_BASE__
#define __ERISED_MATRIX_BASE__

#include <initializer_list>
#include <valarray>

namespace erised {

/**
 *
 *
 */
template< typename T, size_t N>
class DataBase {
 public:
  static constexpr size_t order = N;
  using value_type = T;

  MatBase() = default;
  virtual ~MatBase() = default;

  template<class Fn = T(T)>
  virtual void Apply(Fn&& fn) = 0;

  template<class Fn = T(T)>
  virtual void RowApply(Fn&& fn) = 0;

  template<class Fn = T(T)>
  virtual void ColApply(Fn&& fn) = 0;

  template<class Fn = T(T)>
  virtual T Reduce(Fn&& fn) = 0;

  template<class Fn = T(T)>
  virtual T RowReduce(Fn&& fn) = 0;

  template<class Fn = T(T)>
  virtual T ColReduce(Fn&& fn) = 0;
};

}
#endif //__ERISED_MATRIX_BASE__
