#pragma once

#include <stdexcept>

#include "data_base.h"

namespace erised {

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
  T sum;

  if (axis == Axis::ROW) {
    // Count the number of elements on row i
    num_elems = data.NumElementsLine(i);

    // Sum all valid elements from row i
    sum = data.RowReduce(i, [](float a, float b) {
      return a + b;
    });
  } else {
    // Count the number of elements on col i
    num_elems = data.NumElementsCol(i);

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
