#pragma once

#include <stdexcept>
#include <vector>
#include <cmath>

#include "data_base.h"
#include "parallel.h"
#include "exception.h"

namespace erised {

template<class T, class Alloc, template<typename, typename> class Data>
T Avarage(const Data<T, Alloc>& data, size_t i, Axis axis, size_t num_elems) {
  T sum;

  if (axis == Axis::ROW) {
    // Sum all valid elements from row i
    sum = data.RowReduce(i, [](size_t /*i*/, float a, float b) {
      return a + b;
    });
  } else {
    // Sum all valid elements from col i
    sum = data.ColReduce(i, [](size_t /*i*/, float a, float b) {
      return a + b;
    });
  }

  if (num_elems == 0)
    ERISED_Error(Error::DIVIDE_BY_ZERO, "Divide by zero exception");

  // Make the cast from size_t to T, T MUST be a typename
  // where this kind of conversion is possible
  return sum/ static_cast<T>(num_elems);
}

template<class T, class Alloc, template<typename, typename> class Data>
std::vector<T, Alloc> Avarage(const Data<T, Alloc>& data, Axis axis,
                              const std::vector<size_t>& num_elems) {
  using vec_it = typename Data<T, Alloc>::VectorValue::iterator;

  std::vector<T, Alloc> avgs(num_elems.size(), 0);
  auto sums = data.Reduce(axis, [](size_t /*i*/, T a, T b) -> T {
    return a + b;
  });

  size_t sums_size = sums.size();
  size_t num_elems_size = num_elems.size();

  if (sums_size != num_elems_size)
    ERISED_Error(Error::INVALID_ARGUMENT, "size of number of elements vector "
                 "is not equal axis number");

  Range<vec_it> range(avgs.begin(), avgs.end());

  parallel_for(range, [&](const Range<vec_it>& r) {
    // Scan each line
    for(auto i = r.begin(); i!=r.end(); ++i) {
      auto dist = std::distance(avgs.begin(), i);
      if (num_elems[dist] != 0)
        *i = sums[dist]/ static_cast<T>(num_elems[dist]);
    }
  });

  return std::move(avgs);
}

template<class T, class Alloc, template<typename, typename> class Data>
std::vector<T, Alloc> Avarage(const Data<T, Alloc>& data, Axis axis) {
  std::vector<size_t> num_elems;

  if (axis == Axis::ROW) {
    // Count the number of elements on row i
    num_elems = data.NumElementsLines();
  } else {
    // Count the number of elements on col i
    num_elems = data.NumElementsCols();
  }

  typename Data<T, Alloc>::VectorValue avgs = Avarage(data, axis, num_elems);

  return std::move(avgs);
}

template<class T, class Alloc, template<typename, typename> class Data>
T Avarage(const Data<T, Alloc>& data, size_t i, Axis axis) {
  size_t num_elems = 0;

  if (axis == Axis::ROW) {
    // Count the number of elements on row i
    num_elems = data.NumElementsLine(i);
  } else {
    // Count the number of elements on col i
    num_elems = data.NumElementsCol(i);
  }

  return Avarage(data, i, axis, num_elems);
}

template<class T, class Alloc, template<typename, typename> class Data>
T Variance(const Data<T, Alloc>& data, size_t i, Axis axis) {
  size_t n = 0;

  if (axis == Axis::ROW) {
    // Count the number of elements on row i
    n = data.NumElementsLine(i);
  } else {
    // Count the number of elements on col i
    n = data.NumElementsCol(i);
  }

  T u = Avarage(data, i, axis, n);

  T x2 = data.Reduce([](float a, float b) {
    return a*a + b;
  });

  T res = x2/n - u*u;
  return res;
}

template<class T, class Alloc, template<typename, typename> class Data>
T Variance(const Data<T, Alloc>& data, size_t i, Axis axis, size_t n) {
  T u = Avarage(data, i, axis, n);
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

template<class T, class Alloc, template<typename, typename> class Data>
std::vector<T> Variance(const Data<T, Alloc>& data, Axis axis,
                        const std::vector<size_t>& n) {
  using vec_it = typename std::vector<T>::iterator;
  auto u = Avarage(data, axis, n);
  std::vector<T, Alloc> res(n.size(), 0);

  auto x2 = data.Reduce(axis, [&u](size_t i, T a, T b) -> T {
    T term = a - u[i];
    return term*term + b;
  });

  auto num_elems = data.NumElements(axis);

  Range<vec_it> range(res.begin(), res.end());
  parallel_for(range, [&](const Range<vec_it>& r) {
    // Scan each line
    for(auto i = r.begin(); i!=r.end(); ++i) {
      auto dist = std::distance(res.begin(), i);
      T den = num_elems[dist];

      if (den == 0)
        ERISED_Error(Error::DIVIDE_BY_ZERO, "Divide by zero exception");

      *i = x2[dist]/ den;
    }
  });

  return std::move(res);
}

template<class T, class Alloc, template<typename, typename> class Data>
T StandardDeviation(const Data<T, Alloc>& data, size_t i, Axis axis) {
  T variance = Variance(data, i, axis);
  return std::sqrt(variance);
}

template<class T, class Alloc, template<typename, typename> class Data>
T StandardDeviation(const Data<T, Alloc>& data, size_t i, Axis axis, size_t n) {
  T variance = Variance(data, i, axis, n);
  return std::sqrt(variance);
}

template<class T, class Alloc, template<typename, typename> class Data>
std::vector<T> StandardDeviation(const Data<T, Alloc>& data, Axis axis,
                                 const std::vector<size_t>& n) {
  using vec_it = typename std::vector<T>::iterator;

  std::vector<T> variance = Variance(data, axis, n);
  std::vector<T> res(n.size());

  Range<vec_it> range(res.begin(), res.end());
  parallel_for(range, [&](const Range<vec_it>& r) {
    // Scan each line
    for(auto i = r.begin(); i!=r.end(); ++i) {
      auto dist = std::distance(res.begin(), i);
      *i = std::sqrt(variance[dist]);
    }
  });

  return std::move(res);
}

template<class T, class Alloc, template<typename, typename> class Data>
void Standardization(Data<T, Alloc>* data, size_t i, Axis axis, size_t n) {
  T u = Avarage(*data, i, axis, n);
  T a = StandardDeviation(*data, i, axis, n);

  if (axis == Axis::ROW) {
    data->RowMap(i, [u, a](T x) -> T { return (x - u)/a; });
  } else {
    data->ColMap(i, [u, a](T x) -> T { return (x - u)/a; });
  }
}

template<class T, class Alloc, template<typename, typename> class Data>
void Standardization(Data<T, Alloc>* data, Axis axis, const std::vector<size_t>& n) {
  std::vector<T> u = Avarage(*data, axis, n);
  std::vector<T> a = StandardDeviation(*data, axis, n);

  data->Map(axis, [&u, &a](size_t i, T x) -> T { return (x - u[i])/a[a]; });
}

template<class T, class Alloc, template<typename, typename> class Data>
void Standardization(Data<T, Alloc>* data, size_t i, Axis axis) {
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
    ERISED_Error(Error::DIVIDE_BY_ZERO, "Divide by zero exception");

  if (axis == Axis::ROW) {
    data->RowMap(i, [den, min, max](T x) -> T { return (x - min)/den; });
  } else {
    data->ColMap(i, [den, min, max](T x) -> T { return (x - min)/den; });
  }
}

template<class T, template<typename> class Data>
void Rescaling(Data<T>* data, Axis axis, const std::vector<size_t>& n) {
  using vec_it = typename std::vector<T>::iterator;

  std::vector<T> mins = data->Min(axis);
  std::vector<T> maxs = data->Max(axis);
  std::vector<T> dens(n.size());

  Range<vec_it> range(dens.begin(), dens.end());
  parallel_for(range, [&](const Range<vec_it>& r) {
    // Scan each line
    for(auto i = r.begin(); i!=r.end(); ++i) {
      auto dist = std::distance(dens.begin(), i);
      *i = maxs[dist] - mins[dist];
      if (*i == 0)
        ERISED_Error(Error::DIVIDE_BY_ZERO, "Divide by zero exception");
    }
  });

  data->Map(axis, [&dens, &mins, &maxs](size_t i, T x) -> T {
    return (x - mins[i])/dens[i];
  });
}

}
