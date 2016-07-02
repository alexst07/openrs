#ifndef ERISED_SIM_MAT_H_
#error "This should only be included by sim_mat.h"
#endif

#include "sim_mat.h"

namespace erised {

template<class T>
SimMat<T>::SimMat(size_t size, const std::vector<T>& elems)
  try: mat_(size, elems) {
} catch (std::exception& e) {
  ERISED_Error(Error::BAD_ALLOC, e.what());
}

template<class T>
SimMat<T>::SimMat(size_t size, std::vector<T>&& elems)
  try: mat_(size, std::move<elems>) {
} catch (std::exception& e) {
  ERISED_Error(Error::BAD_ALLOC, e.what());
}

template<class T>
T& SimMat<T>::Element(size_t x, size_t y) {
  return mat_.Element(x, y);
}

template<class T>
const T& SimMat<T>::Element(size_t x, size_t y) const {
  return mat_.Element(x, y);
}

template<class T>
template<class Fn, class Fnm, size_t N>
std::array<T,N> SimMat<T>::Reduce(Fn&& fn, Fn&& fnm,
                                  std::vector<size_t>&& indexes,
                                  size_t axis) {
  // Rows and Cols are the same on similarity matrix
  auto sim_row = mat_.Row(axis);

  // Only rating items index are needed
  Range<VecIter> range(indexes.begin(), indexes.end());

  // Executes reduce for more the one value, it is useful
  // for calcuates weighted sum
  return parallel_reduce(range, static_cast<T>(0),
      [&](const Range<VecIter>& r, std::array<T,N> values) -> std::array<T,N> {
        std::array<T,N> rets = values;

        // Scan each line
        for(auto n = r.begin(); n != r.end(); ++n) {
          rets = fn(*n, sim_row[*n], rets);
        }

        return rets;
  }, [&fnm](std::array<T,N> a, std::array<T,N> b) -> T {
    return fnm(a, b);
  });
}

}
