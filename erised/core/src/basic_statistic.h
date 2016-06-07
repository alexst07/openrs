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

  return T/ static_cast<T>(num_elems);
}

template<class T, template<typename> class Data>
T Avarage(const Data<T>& data, size_t i, Axis axis) {
  size_t num_elems = 0;
  T sum;

  if (axis == Axis.ROW) {
    num_elems = data.NumElementsLine(i);

    sum = data.RowReduce([](float a, float b) {
      return a + b;
    });
  } else {
    num_elems = data.NumElementsCol(i);

    sum = data.ColReduce([](float a, float b) {
      return a + b;
    });
  }

  if (num_elems == 0)
    throw std::overflow_error("Divide by zero exception");

  return T/ static_cast<T>(num_elems);
}

}
