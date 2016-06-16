#ifndef ERISED_TRIANGULAR_MATRIX_H_
#error "This should only be included by triangular_mat.h"
#endif

namespace erised {

template<typename T>
size_t TriangularMatBase::SizeValidElements(){
  size_t total = size_*size_;
  return total/2 - size_/2;
}

template<typename T>
T& TriangularMatBase::operator()(size_t x, size_t y) {
  size_t n = SizeValidElements();
  size_t i = (n*(n-1)/2) - (n-x)*((n-x)-1)/2 + y - x - 1;
  return data_[i];
}

template<typename T>
const T& TriangularMatBase::operator()(size_t x, size_t y) const {
  size_t n = SizeValidElements();
  size_t i = (n*(n-1)/2) - (n-x)*((n-x)-1)/2 + y - x - 1;
  return data_[i];
}

}
