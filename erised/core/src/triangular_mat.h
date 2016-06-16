#pragma once

#define ERISED_TRIANGULAR_MATRIX_H_

#include <initializer_list>
#include <functional>

#include "triangular_mat_base.h"

namespace erised {

template<typename T>
class TriangularMatSlice {
 public:
  template<typename Func>
  TriangularMatSlice(Func&& f);
  TriangularMatSlice(const TriangularMatSlice<T>& m);
  TriangularMatSlice(TriangularMatSlice<T>&& m);

  T& operator[](size_t i);
  const T& operator[](size_t i) const;

 private:
  std::function fn_;
};

template<typename T>
class TriangularMatSliceIterator : public std::iterator<std::input_iterator_tag, T> {
  friend class TriangularMatSlice;
 public:
  MyIterator(int* x) :p(x) {}
  MyIterator(const MyIterator& mit) : p(mit.p) {}
  MyIterator& operator++() {++p;return *this;}
  MyIterator operator++(int) {MyIterator tmp(*this); operator++(); return tmp;}
  bool operator==(const MyIterator& rhs) {return p==rhs.p;}
  bool operator!=(const MyIterator& rhs) {return p!=rhs.p;}
  T& operator*() {return *p;}
 private:
  TriangularMatSliceIterator()
  TriangularMatSlice* p_;
};


/**
 *
 *
 */
template<typename T>
class TriangularMatBase {
 public:
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
