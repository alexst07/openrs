#pragma once

#define ERISED_TRIANGULAR_MATRIX_H_

#include <initializer_list>
#include <functional>

#include "data_base.h"
#include "triangular_mat_base.h"

namespace erised {

class TriangularMat;
class TriangularMatSliceIterator;

template<typename T>
class TriangularMatSlice {
  friend class TriangularMat;
 public:
  using iterator = typename TriangularMatSliceIterator<T>;
  typedef TriangularMatSliceIterator iterator;

  TriangularMatSlice(const TriangularMatSlice<T>& m);

  TriangularMatSlice(TriangularMatSlice<T>&& m) = delete;

  T& operator[](size_t i);
  const T& operator[](size_t i) const;

  TriangularMatSliceIterator Begin();

  TriangularMatSliceIterator End();

 private:
  TriangularMatSlice(TriangularMat& ref, Axis axis, size_t axis_i);

  TriangularMat& ref_;
  Axis axis_;
  size_t axis_i_;
};

template<typename T>
class TriangularMatSliceIterator : public std::iterator<std::input_iterator_tag, T> {
  friend class TriangularMatSlice;
 public:

  TriangularMatSliceIterator(const TriangularMatSliceIterator& it)
    : ref_(it.ref_)
    , pos_(it.pos_) {}

  TriangularMatSliceIterator& operator++() {
    ++pos_;
    return *this;
  }

  TriangularMatSliceIterator operator++(int) {
    TriangularMatSliceIterator tmp(*this);
    operator++();
    return tmp;
  }

  bool operator==(const TriangularMatSliceIterator& it) {
    return pos_==it.pos_;
  }

  bool operator!=(const TriangularMatSliceIterator& it) {
    return pos_!=it.pos_;
  }

  T& operator*() {
    return ref_[pos_];
  }

  const T& operator*() const {
    return ref_[pos_];
  }

 private:
  TriangularMatSliceIterator(TriangularMatSlice& ref)
    : ref_(ref)
    , pos_(0) {}

  TriangularMatSlice& ref_;
  size_t pos_;
};

constexpr size_t TriangularMatElems(size_t size) {
  size_t total = size*size;
  return total/2 - size/2;
}

/**
 *
 *
 */
template<typename T>
class TriangularMat {
 public:
  TriangularMat(size_t size);
  TriangularMat(size_t size, std::vector<T> elems);

  TriangularMatSlice Row(size_t i);
  TriangularMatSlice Col(size_t i);
  T& operator()(size_t x, size_t y);
  const T& operator()(size_t x, size_t y) const;
  T& Data();
  const T& Data() const;

 private:
  size_t SizeValidElements();

  std::vector<T> elems_;
  size_t size_;
};

#include "triangular_mat-inl.h"

}
