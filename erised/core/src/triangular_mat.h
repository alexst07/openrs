#pragma once

#define ERISED_TRIANGULAR_MATRIX_H_

#include <initializer_list>
#include <functional>
#include <exception>

#include "data_base.h"

namespace erised {

template<typename T>
class TriangularMat;

template<typename T>
class TriangularMatSlice {
  friend class TriangularMat<T>;
 public:
  class iterator;
  class const_iterator;

  TriangularMatSlice(const TriangularMatSlice<T>& m)
    : ref_(m.ref_), axis_(m.axis_), axis_i_(m.axis_i_) {}

  TriangularMatSlice(TriangularMatSlice<T>&& m) = delete;

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

  iterator begin() {
    return iterator(ref_, 0);
  }

  const_iterator begin() const {
    return const_iterator(ref_, 0);
  }

  iterator end() {
    size_t size_elems = SizeAxis(ref_.Size(), axis_, axis_i_);
    return iterator(ref_, size_elems);
  }

  const_iterator end() const {
    size_t size_elems = SizeAxis(ref_.Size(), axis_, axis_i_);
    return const_iterator(ref_, size_elems);
  }

  class iterator: public std::iterator<std::input_iterator_tag, T> {
   public:
    typedef iterator self_type;
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;

    iterator(const iterator& it): ref_(it.ref_), pos_(it.pos_) {}

    iterator& operator++() {
      ++pos_;
      return *this;
    }

    iterator operator++(int) {
      iterator tmp(*this);
      operator++();
      return tmp;
    }

    bool operator==(const iterator& it) {
      return pos_==it.pos_;
    }

    bool operator!=(const iterator& it) {
      return pos_!=it.pos_;
    }

    T& operator*() {
      return ref_[pos_];
    }

    const T& operator*() const {
      return ref_[pos_];
    }

  private:
    iterator(TriangularMatSlice& ref): ref_(ref), pos_(0) {}
    iterator(TriangularMatSlice& ref, size_t pos): ref_(ref), pos_(pos) {}

    TriangularMatSlice& ref_;
    size_t pos_;
  };

  class const_iterator: public std::iterator<std::input_iterator_tag, T> {
   public:
    typedef const_iterator self_type;
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef int difference_type;
    typedef std::forward_iterator_tag iterator_category;

    const_iterator(const const_iterator& it): ref_(it.ref_), pos_(it.pos_) {}

    iterator& operator++() {
      ++pos_;
      return *this;
    }

    iterator operator++(int) {
      iterator tmp(*this);
      operator++();
      return tmp;
    }

    bool operator==(const iterator& it) {
      return pos_ == it.pos_;
    }

    bool operator!=(const iterator& it) {
      return pos_ != it.pos_;
    }

    T& operator*() {
      return ref_[pos_];
    }

    const T& operator*() const {
      return ref_[pos_];
    }

  private:
    const_iterator(TriangularMatSlice& ref): ref_(ref), pos_(0) {}
    const_iterator(TriangularMatSlice& ref, size_t pos)
      : ref_(ref), pos_(pos) {}

    TriangularMatSlice& ref_;
    size_t pos_;
  };

 private:
  TriangularMatSlice(TriangularMat<T>& ref, Axis axis, size_t axis_i)
    : ref_(ref), axis_(axis), axis_i_(axis_i) {}

  static size_t SizeAxis(size_t size, Axis axis, size_t axis_i) {
    if (axis == Axis::ROW) {
      size_t num = size - axis_i - 1;
      return num >= 0? num: 0;
    } else {
      return axis_i;
    }
  }

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
class TriangularMat {
 public:
  explicit TriangularMat(size_t size)
    try: elems_{TriangularMatElems(size)}, size_(size) {
  } catch (std::bad_alloc& e) {
    throw std::bad_alloc("Error on allocate elements");
  }

  TriangularMat(size_t size, const std::vector<T>& elems)
    try: elems_{TriangularMatElems(size) == elems.size() ? elems :
        throw std::invalid_argument("Vector has wrong size")}
    , size_(size) {
  } catch (std::bad_alloc& e) {
    throw std::bad_alloc("Error on allocate elements");
  }

  TriangularMat(size_t size, std::vector<T>&& elems)
    try: elems_{TriangularMatElems(size) == elems.size() ? std::move(elems) :
        throw std::invalid_argument("Vector has wrong size")}
    , size_(size) {
  } catch (std::bad_alloc& e) {
    throw std::bad_alloc("Error on allocate elements");
  }

  TriangularMat(const TriangularMat<T>& tm): elems_(tm.elems_), size_(tm.size_) {}

  TriangularMat(TriangularMat<T>&& tm)
    : elems_(std::move(tm.elems_)), size_(tm.size_) {
    size_ = 0;
  }

  TriangularMat<T>& operator=(const TriangularMat<T>& tm) {
    // self-assignment check
    if (this != &tm) {
      elems_ = tm.elems_;
      size_ = tm.size_;
    }

    return *this;
  }

  TriangularMat<T>& operator=(TriangularMat<T>&& tm) {
    // self-assignment check
    if (this != &tm) {
      elems_ = std::move(tm.elems_);
      size_ = tm.size_;
      size_ = 0;
    }

    return *this;
  }

  TriangularMatSlice<T> Row(size_t i) {
    TriangularMatSlice<T> slice(*this, Axis::ROW, i);
    return slice;
  }

  TriangularMatSlice<T>  Col(size_t i) {
    TriangularMatSlice<T> slice(*this, Axis::COL, i);
    return slice;
  }

  T& operator()(size_t x, size_t y) noexcept {
    size_t n = SizeValidElements();
    size_t i = (n*(n-1)/2) - (n-x)*((n-x)-1)/2 + y - x - 1;
    return elems_[i];
  }

  const T& operator()(size_t x, size_t y) const noexcept {
    size_t n = SizeValidElements();
    size_t i = (n*(n-1)/2) - (n-x)*((n-x)-1)/2 + y - x - 1;
    return elems_[i];
  }

  size_t Size() const noexcept{
    return size_;
  }

  size_t NumElements() const noexcept {
    return elems_.size();
  }

  T& Data() {
    return SizeValidElements();
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
};

}
