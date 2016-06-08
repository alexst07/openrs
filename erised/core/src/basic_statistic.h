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
T Variance(const Data<T>& data, size_t i, Axis axis, size_t n) {
  T u = internal::Avarage(data, i, axis, n);
  T x2;

  if (axis == Axis::ROW) {
    x2 = data.RowReduce([](float a, float b) {
      return a*a + b;
    });
  } else {
    x2 = data.ColReduce([](float a, float b) {
      return a*a + b;
    });
  }

  T res = x2/n - u*u;
  return res;
}

template<class T, template<typename> class Data>
T StandardDeviation(const Data<T>& data, size_t i, Axis axis) {
  T variance = Variance(data, i, axis);
  return std::sqrt(variance);
}

template<class T, template<typename> class Data>
T StandardDeviation(const Data<T>& data, size_t i, Axis axis, size_t n) {
  T variance = Variance(data, i, axis, n);
  return std::sqrt(variance);
}

template<class T, template<typename> class Data>
void Standardization(Data<T>* data, size_t i, Axis axis, size_t n) {
  T u = Avarage(*data, i, axis, n);
  T a = StandardDeviation(*data, i, axis, n);

  if (axis == Axis::ROW) {
    data->RowMap(i, [u, a](T x) -> T { return (x - u)/a; });
  } else {
    data->ColMap(i, [u, a](T x) -> T { return (x - u)/a; });
  }
}

template<class T, template<typename> class Data>
void Standardization(Data<T>* data, size_t i, Axis axis) {
  size_t n;

  if (axis == Axis::ROW) {
    // Count the number of elements on row i
    n = data->NumElementsLine(i);
  } else {
    // Count the number of elements on col i
    n = data->NumElementsCol(i);
  }

  Standardization(data, i, axis, n);
}

template<class T, template<typename> class Data>
void Rescaling(Data<T>* data, size_t i, Axis axis, size_t n) {
  T min = data->Min(i, axis);
  T max = data->Max(i, axis);

  T den = max - min;
  if (den == 0)
    throw std::overflow_error("Divide by zero exception");

  if (axis == Axis::ROW) {
    data->RowMap(i, [den, min, max](T x) -> T { return (x - min)/den; });
  } else {
    data->ColMap(i, [den, min, max](T x) -> T { return (x - min)/den; });
  }
}

}
