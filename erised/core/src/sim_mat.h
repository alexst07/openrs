#pragma once

#include <initializer_list>

#include "triangular_mat.h"

namespace erised {

template<typename T>
class SimMat {
 public:
  SimMat() = default;

  SimMat(size_t n): mat_{n} {}

  SimMat(size_t size, const std::vector<T>& elem);

  SimMat(size_t size, std::vector<T>&& elem);

  T& Element(size_t x, size_t y);
  const T& Element(size_t x, size_t y) const;



 private:
  TriangularMat<T> mat_;
};

}
