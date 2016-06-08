#pragma once

#include <stdexcept>
#include <cmath>

#include "data_base.h"

namespace erised {
namespace internal {

template<class T, template<typename> class Data>
T Avarage(const Data<T>& data, size_t i, Axis axis, size_t num_elems) {
  T sum;

  if (axis == Axis::ROW) {
    // Sum all valid elements from row i
    sum = data.RowReduce(i, [](float a, float b) {
      return a + b;
    });
  } else {
    // Sum all valid elements from col i
    sum = data.ColReduce(i, [](float a, float b) {
      return a + b;
    });
  }

  if (num_elems == 0)
    throw std::overflow_error("Divide by zero exception");

  // Make the cast from size_t to T, T MUST be a typename
  // where this kind of conversion is possible
  return sum/ static_cast<T>(num_elems);
}

}

template<class T, template<typename> class Data>
T Avarage(const Data<T>& data) {
  size_t num_elems = data.NumElements();
  T sum = data.Reduce([](float a, float b) {
    return a + b;
  });

  if (num_elems == 0)
    throw std::overflow_error("Divide by zero exception");

  // Make the cast from size_t to T, T MUST be a typename
  // where this kind of conversion is possible
  return sum/ static_cast<T>(num_elems);
}

template<class T, template<typename> class Data>
T Avarage(const Data<T>& data, size_t i, Axis axis) {
  size_t num_elems = 0;

  if (axis == Axis::ROW) {
    // Count the number of elements on row i
    num_elems = data.NumElementsLine(i);
  } else {
    // Count the number of elements on col i
    num_elems = data.NumElementsCol(i);
  }

  return internal::Avarage(data, i, axis, num_elems);
}

template<class T, template<typename> class Data>
T Variance(const Data<T>& data, size_t i, Axis axis) {
  size_t n = 0;

  if (axis == Axis::ROW) {
    // Count the number of elements on row i
    n = data.NumElementsLine(i);
  } else {
    // Count the number of elements on col i
    n = data.NumElementsCol(i);
  }

  T u = internal::Avarage(data, i, axis, n);

  T x2 = data.Reduce([](float a, float b) {
    return a*a + b;
  });

  T res = x2/n - u*u;
  return res;
}

template<class T, template<typename> class Data>
T StandardDeviation(const Data<T>& data, size_t i, Axis axis) {
  T variance = Variance(data, i, axis);
  return std::sqrt(variance);
}

}
