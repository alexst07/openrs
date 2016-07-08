#pragma once

#define ERISED_FILTER_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>
#include <flann/flann.hpp>

#include "exception.h"

namespace erised {

template<class TD,
  class TS,
  class Alloc,
  template <typename> class Norm,
  template <typename, typename> class Sim,
  template <typename, typename> class Data>
class Filter {
 public:
  Filter(bool normalize = true);
  void Fit(Data<TD, Alloc> &data);
  const Sim<TD, Alloc>& Similarity() const noexcept;
  Sim<TD, Alloc>& Similarity() noexcept;

  std::vector<TS> Predict(size_t i);

};

}
