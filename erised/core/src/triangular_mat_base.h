#pragma once

#include <initializer_list>
#include <functional>

namespace erised {

/**
 *
 *
 */
template<typename T>
class TriangularMatBase {
 public:
  virtual TriangularMatSliceBase Row(size_t i) = 0;
  virtual TriangularMatSliceBase Col(size_t i) = 0;
  virtual T operator()(size_t x, size_t y) = 0;
  virtual T& Data() = 0;
  virtual const T& Data() const = 0;
};

}
