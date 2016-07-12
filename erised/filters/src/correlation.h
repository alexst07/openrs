#pragma once

#define ERISED_CORRELATION_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>

#include "exception.h"

namespace erised {

template<
  class TD,
  class TS,
  class Alloc,
  template<typename, typename> class Data,
  template <typename, typename> class Sim>
class Correlation {
 public:
  virtual void Fit(Data<TD, Alloc> &data) = 0;
  virtual const Sim<TD, Alloc>& Similarity() const noexcept = 0;
  virtual Sim<TS, Alloc>& Similarity() noexcept = 0;

  virtual typename Sim<TS, Alloc>::Slice Predict(size_t i) = 0;
};

template<
  class TD,
  class TS,
  class Alloc,
  template<typename, typename> class Data,
  template <typename, typename> class Sim>
class AdjustedCosine: public Correlation<TD, TS, Alloc, Data, Sim> {
 public:
  virtual void Fit(Data<TD, Alloc> &data) {

  }

  virtual const Sim<TD, Alloc>& Similarity() const noexcept {

  }

  virtual Sim<TS, Alloc>& Similarity() noexcept {

  }

  virtual typename Sim<TS, Alloc>::Slice Predict(size_t i) {

  }
};

template<
  class TD,
  class TS,
  class Alloc,
  template<typename, typename> class Data,
  template <typename, typename> class Sim>
class Pearson: public Correlation<TD, TS, Alloc, Data, Sim> {
 public:
  virtual void Fit(Data<TD, Alloc> &data) {

  }

  virtual const Sim<TD, Alloc>& Similarity() const noexcept {

  }

  virtual Sim<TS, Alloc>& Similarity() noexcept {

  }

  virtual typename Sim<TS, Alloc>::Slice Predict(size_t i) {

  }
};

}
