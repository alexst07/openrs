#ifndef __ERISED_MATRIX_BASE__
#define __ERISED_MATRIX_BASE__

#include <initializer_list>
#include <valarray>

namespace erised {

// MatInit describes the structure of a nested initializer_list
template<typename T, size_t N>
struct MatInit {
  using type = std::initializer_list<typename MatInit<T,N-1>::type>;
};

// The N==1 is special. That is where we get to the (most deeply nested)
// initializer_list<T>
template<typename T>
struct MatInit<T, 1> {
  using type = std::initializer_list<T>;
};

// undefined on purpose
template<typename T>
struct MatInit<T, 0>;


/**
 * Generalized Slice not implemented, because on this
 * first step it isn't useful
 */
template<size_t N>
struct MatSlice;

/**
 * Specialization for two dimensions matrix
 * on this case, for rating matrix only 2D matrix is used
 */
template<>
struct MatSlice<2> {
  MatSlice() :start(-1), length(-1), stride(1) { }

  explicit MatSlice(size_t s) :start(s), length(-1), stride(1) { }

  MatSlice(size_t s, size_t l, size_t n = 1) :start(s), length(l), stride(n) { }

  size_t operator()(size_t i) const { return start+i*stride; }

  static MatSlice all;
  size_t start;
  size_t length;
  size_t stride;
};

/**
 * Specialization for 1D matrix
 */
template<>
struct MatSlice<1> {
  MatSlice() = default;
  size_t operator()(size_t i) const { return i; }
};

/**
 *
 *
 */
template< typename T, size_t N>
class MatBase {
public:
  static constexpr size_t order = N;
  using value_type = T;

  MatBase() = default;
  virtual ~MatBase() = default;

  virtual T operator+(const ) = 0;
};


/**
 *
 *
 */
template< typename T, size_t N>
class MatRefBase: public MatBase<T, N> {
public:

};

/**
 *
 *
 */
template< typename T, size_t N>
class MatRefBase: public MatBase<T, N> {
public:

};

}
#endif //__ERISED_MATRIX_BASE__
