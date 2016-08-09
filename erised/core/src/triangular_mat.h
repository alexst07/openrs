#pragma once

#define ERISED_TRIANGULAR_MATRIX_H_

#include <initializer_list>
#include <functional>
#include <exception>
#include <string>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/utility/enable_if.hpp>

#include "data_base.h"
#include "exception.h"

namespace erised {

template<typename T>
class TriangularMat;

template<typename T>
class TriangularMatAxisRef:
    public VecDiscontinuous<T, TriangularMatAxisRef<T>> {

  friend class TriangularMat<T>;

 public:
  TriangularMatAxisRef(const TriangularMatAxisRef<T>& m)
    : ref_(m.ref_), axis_(m.axis_), axis_i_(m.axis_i_) {}

  T& operator[](size_t i) {
    if (axis_ == Axis::ROW)
      return ref_(axis_i_, i);
    else
      return ref_(i, axis_i_);
  }

  const T& operator[](size_t i) const {
    if (axis_ == Axis::ROW)
      return ref_(axis_i_, i);
    else
      return ref_(i, axis_i_);
  }

  size_t Size() const noexcept {
    return ref_.Size();
  }

 private:
  TriangularMatAxisRef(TriangularMat<T>& ref, Axis axis, size_t axis_i)
    : ref_(ref), axis_(axis), axis_i_(axis_i) {}

  TriangularMat<T>& ref_;
  Axis axis_;
  size_t axis_i_;
};

constexpr size_t TriangularMatElems(size_t size) {
  return size*size/2 - size/2;
}

/**
 *
 *
 */
template<typename T>
class TriangularMat: public MatIter<T, TriangularMat<T>,
                                    TriangularMatAxisRef<T>> {

 public:
  using Base = MatIter<T, TriangularMat<T>, TriangularMatAxisRef<T>>;

  TriangularMat(): elems_(0), size_(0), unit_(static_cast<T>(1)) {}

  explicit TriangularMat(size_t size)
    try: elems_{TriangularMatElems(size)}
       , size_(size)
       , unit_(static_cast<T>(1)) {
  } catch (std::bad_alloc& e) {
    throw std::bad_alloc();
  }

  TriangularMat(size_t size, const std::vector<T>& elems)
    try: elems_{TriangularMatElems(size) == elems.size() ? elems :
        throw std::invalid_argument("Vector has wrong size")}
    , size_(size)
    , unit_(static_cast<T>(1)) {
  } catch (std::bad_alloc& e) {
    throw std::bad_alloc();
  }

  TriangularMat(size_t size, std::vector<T>&& elems)
    try: elems_{TriangularMatElems(size) == elems.size() ? std::move(elems) :
        throw std::invalid_argument("Vector has wrong size")}
    , size_(size)
    , unit_(static_cast<T>(1)) {
  } catch (std::bad_alloc& e) {
    throw std::bad_alloc();
  }

  TriangularMat(const TriangularMat<T>& tm)
    : elems_(tm.elems_)
    , size_(tm.size_)
    , unit_(tm.unit_)
    {}

  TriangularMat(TriangularMat<T>&& tm)
    : elems_(std::move(tm.elems_))
    , size_(tm.size_)
    , unit_(tm.unit_)
  {
    tm.size_ = 0;
  }

  TriangularMat<T>& operator=(const TriangularMat<T>& tm) {
    // self-assignment check
    if (this != &tm) {
      elems_ = tm.elems_;
      size_ = tm.size_;
      unit_ = tm.unit_;
    }

    return *this;
  }

  TriangularMat<T>& operator=(TriangularMat<T>&& tm) {
    // self-assignment check
    if (this != &tm) {
      elems_ = std::move(tm.elems_);
      size_ = tm.size_;
      unit_ = tm.unit_;
      tm.size_ = 0;
    }

    return *this;
  }

  TriangularMatAxisRef<T> Row(size_t i) {
    TriangularMatAxisRef<T> slice(*this, Axis::ROW, i);
    return slice;
  }

  TriangularMatAxisRef<T> Col(size_t i) {
    TriangularMatAxisRef<T> slice(*this, Axis::COL, i);
    return slice;
  }

  const T& Element(size_t x, size_t y) const noexcept {
    // If the client gives x > 0, the element is always 0
    // so, this operation is like a mirror operation on matrix
    // [[1 1 2 3]
    //  [1 1 4 5]
    //  [2 4 1 6]
    //  [3 5 6 1]]
    // And if x == y, so the element is from diagonal, what is 1
    if (x == y)
      return unit_;
    else if (x > y) {
      size_t tmp = x;
      x = y;
      y = tmp;
    }

    size_t n = size_;

    // This equations gives the index on the array for the coordinate
    size_t i = (n*(n-1)/2) - (n-x)*((n-x)-1)/2 + y - x - 1;
    return elems_[i];
  }

  T& operator()(size_t x, size_t y) override {
    return const_cast<T&>(Element(x, y));
  }

  const T& operator()(size_t x, size_t y) const override {
    return Element(x, y);
  }

  TriangularMatAxisRef<T> operator[](size_t i) {
    return Row(i);
  }

  size_t Size() const noexcept override {
    return size_;
  }

  size_t NumElements() const noexcept {
    return elems_.size();
  }

  T& Data() {
    return elems_.data();
  }

  const T& Data() const {
    return elems_.data();
  }

 private:
  size_t SizeValidElements() {
    return TriangularMatElems(size_);
  }

  std::vector<T> elems_;
  size_t size_;
  T unit_;
};

}
