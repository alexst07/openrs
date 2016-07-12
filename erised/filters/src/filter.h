#pragma once

#define ERISED_FILTER_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>
#include <tuple>
#include <flann/flann.hpp>

#include "exception.h"
#include "correlation.h"

namespace erised {

template<class TD,
  class TS,
  class Alloc,
  template <typename> class Norm,
  template <typename, typename> class Sim,
  template <typename, typename> class Data>
class Filter {
 public:
  enum CorrelationType {
    Pearson,
    AdjustedCosine
  };

  Filter(CorrelationType type, size_t nn, bool normalize = true);
  Filter(Sim<TD, Alloc>&& mat);
  Filter(Sim<TD, Alloc>&& mat, Sim<size_t, Alloc>&& indices);

  void Fit(Data<TD, Alloc> &data);
  const Sim<TD, Alloc>& Similarity() const noexcept;
  Sim<TD, Alloc>& Similarity() noexcept;
  SimMat<size_t, Alloc>&& Neighbors();
  void Neighbors(SimMat<size_t, Alloc>&& dists);

  std::vector<TS> Predict(size_t i);
 protected:
  std::unique_ptr<Correlation> correlation_;
  bool sim_set_;
};

}
