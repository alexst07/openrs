#pragma once

#define ERISED_SIM_MAT_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>

#include "parallel.h"
#include "exception.h"
#include "triangular_mat.h"

namespace erised {

template<class T>
class SimMat {
 public:
  using VecIter = std::vector<size_t>::const_iterator;

  SimMat() = default;

  SimMat(size_t n): mat_{n} {}

  SimMat(size_t size, const std::vector<T>& elems);

  SimMat(size_t size, std::vector<T>&& elems);

  T& Element(size_t x, size_t y);
  const T& Element(size_t x, size_t y) const;

  template<class Fn, class Fnm, size_t N>
  std::array<T,N> Reduce(Fn&& fn, Fn&& fnm, std::vector<size_t>&& indexes,
                         size_t axis);

 private:
  TriangularMat<T> mat_;
};

}

#include "sim_mat-inl.h"
