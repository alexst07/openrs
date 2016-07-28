#pragma once

#define ERISED_MODEL_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>
#include <tuple>
#include <flann/flann.hpp>

#include "exception.h"
#include "correlation.h"
#include "data_base.h"
#include "predict.h"

namespace erised {

template<class Data, class Sim>
class CollaborativeModel {
 public:
  using value_type = typename Sim::value_type;

  CollaborativeModel(Data& data, Axis axis)
    : data_(data)
    , correlation_(axis)
    , calculated_(false) {}

  const Sim& Similarity() const noexcept {
    return correlation_.Similarity();
  }

  Sim& Similarity() noexcept {
    return correlation_.Similarity();
  }

  void Fit() {
    correlation_.Fit(data_);
    calculated_ = true;
  }

 protected:
  template<size_t N, class Fn>
  std::array<value_type, N> PredTerms(
      const Predict<Data, Sim, CollaborativeModel>& pred,
      size_t i,
      Fn&& fn) {
    auto res = pred.template Terms<N>(data_, Similarity(), i, indexes, fn);
    return res;
  }

  virtual value_type Predict(const Predict<Data, Sim>& pred, size_t i) = 0;

  Data& data_;
  Correlation correlation_;
  bool calculated_;
  std::vector<size_t> indexes_;
};

template<class Data, class Sim>
class UserFilter: public CollaborativeModel {
  friend Predict<Data, Sim, CollaborativeModel>;
 public:
  UserFilter(Data& data): CollaborativeModel(data, Axis::ROW) {}

 private:
  value_type Predict(const Predict<Data, Sim>& pred, size_t i) override {
    auto arr = this->template PredTerms<3>(pred, i, [](){

    });
  }
};

}
