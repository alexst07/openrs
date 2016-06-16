#pragma once

#include <initializer_list>
#include <functional>

namespace erised {

template<typename T>
class TriangularMatSliceBase {
 public:
  virtual T& operator[](size_t i) = 0;
  virtual const T& operator[](size_t i) const = 0;
};

/**
 *
 *
 */
template<typename T>
class TriangularMatBase {
 public:
  virtual TriangularMatRef Row(size_t i) = 0;
  virtual TriangularMatRef Col(size_t i) = 0;
  virtual T& operator()(size_t x, size_t y) = 0;
  virtual const T& operator()(size_t x, size_t y) const = 0;
  virtual T& Data() = 0;
  virtual const T& Data() const = 0;
};

}
