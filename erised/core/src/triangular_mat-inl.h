#ifndef ERISED_TRIANGULAR_MATRIX_H_
#error "This should only be included by triangular_mat.h"
#endif

#include "triangular_mat.h"

namespace erised {

TriangularMat::TriangularMat(size_t size)
  : elems_(TriangularMatElems(size))
  , size_(size) {
}

TriangularMat(size_t size, std::vector<T> elems)
  : elems_(TriangularMatElems(size) == elems.size() ? std::move(elems) :
      throw std::invalid_argument("Vector has wrong size"))
  , size_(size) {
}

template<typename T>
size_t TriangularMat::SizeValidElements(){
  return TriangularMatElems(size_);
}

template<typename T>
T& TriangularMat::operator()(size_t x, size_t y) {
  size_t n = SizeValidElements();
  size_t i = (n*(n-1)/2) - (n-x)*((n-x)-1)/2 + y - x - 1;
  return elems_[i];
}

template<typename T>
const T& TriangularMat::operator()(size_t x, size_t y) const {
  size_t n = SizeValidElements();
  size_t i = (n*(n-1)/2) - (n-x)*((n-x)-1)/2 + y - x - 1;
  return elems_[i];
}

TriangularMatSlice TriangularMat::Row(size_t i) {
  TriangularMatSlice slice(*this, Axis::ROW, i);
  return slice;
}

TriangularMatSlice TriangularMat::Col(size_t i) {
  TriangularMatSlice slice(*this, Axis::COL, i);
  return slice;
}

TriangularMatSlice(TriangularMat& ref, Axis axis, size_t axis_i)
  : ref_(ref)
  , axis_(axis)
  , axis_i_(axis_i) {
}

TriangularMatSlice(const TriangularMatSlice<T>& m)
  : ref_(m.ref_)
  , axis_(m.axis_)
  , axis_i_(m.axis_i_) {
}

T& TriangularMatSlice::operator[](size_t i) {
  if (axis_ == Axis.ROW)
    return ref_(axis_i_, i);
  else
    return ref_(i, axis_i_);
}

const T& TriangularMatSlice::operator[](size_t i) const {
  if (axis_ == Axis.ROW)
    return ref_(axis_i_, i);
  else
    return ref_(i, axis_i_);
}


}
