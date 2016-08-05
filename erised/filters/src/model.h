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

template<class Data, class Sim, class Model>
class PredictData;

template<class Model>
class PredictVec;

template<class Data, class Sim, class Derived>
class CollaborativeModel {
  using Pred = PredictData<Derived>;
 public:
  using value_type = typename Sim::value_type;
  using Data = Data;
  using Sim = Sim;

  CollaborativeModel(Data& data, Axis axis)
    : data_(data)
    , correlation_(axis)
    , calculated_(false)
    , axis_(axis) {}

  const Sim& Similarity() const noexcept {
    return correlation_.Similarity();
  }

  Sim& Similarity() noexcept {
    return correlation_.Similarity();
  }

  Sim& NeighborsSimilarity(size_t n) noexcept {
    return correlation_.Similarity();
  }

  Sim& NeighborsSimilarity(size_t i, size_t n) noexcept {
    return correlation_.Similarity();
  }

  void Fit() {
    correlation_.Fit(data_);
    calculated_ = true;
  }

  Axis GetAxis() const noexcept {
    return axis_;
  }

 protected:
  template<size_t N, class Fn>
  std::array<value_type, N> PredTerms(
      const PredictData<Data, Sim, CollaborativeModel>& pred, size_t i, size_t n,
      Fn&& fn) {
    //TODO: Calculates the N indexes of neighbors
    std::vector<size_t> indexes;
    auto res = pred.template Terms<N>(data_, Similarity(), i, indexes, fn);
    return res;
  }

  virtual value_type Predict(
      const PredictData<Data, Sim, CollaborativeModel>& pred, size_t i) = 0;

  Data& data_;
  Correlation<Data, Sim> correlation_;
  bool calculated_;
  std::vector<size_t> indexes_;
  Axis axis_;
};

template<class Data, class Sim>
class UserFilter: public CollaborativeModel<Data, Sim> {
  using Pred = PredictData<Data, Sim, CollaborativeModel<Data, Sim>>;

  friend Pred;
 public:
  using value_type = typename CollaborativeModel<Data, Sim>::value_type;
  using Base = CollaborativeModelVec<C, ItemFilterVec<C>>;

  UserFilter(Data& data)
    : CollaborativeModel<Data, Sim>(data, Axis::ROW)
    , avgs_(erised::Avarage(data, Axis::ROW)) {}

 private:
  value_type Predict(const Pred& pred, size_t i) override {
    auto arr = this->template PredTerms<2>(pred, i,
      [this](size_t i, float v1, float v2, std::array<float, 2> arr){
        std::array<float,2> terms;
        terms[0] = v1*(v2 - avgs_[i]);
        terms[1] = v1;

        terms[0] += arr[0];
        terms[1] += arr[1];

        return terms;
      });

    if (arr[1] == 0)
      ERISED_Error(Error::DIVIDE_BY_ZERO, "Divide by zero exception");

    return avgs_[i] + arr[0]/arr[1];
  }

 private:
  std::vector<value_type> avgs_;
};

template <class C, class Derived>
class CollaborativeModelVec {
  using Pred = PredictVec<Derived>;
  friend Pred;
 public:
  using value_type = typename C::value_type;
  using container = C;

  CollaborativeModelVec(const C& ratings, const C& sim)
    : ratings_(ratings)
    , sim_(sim) {}

 protected:
  template<size_t N, class Func>
  std::array<value_type, N> PredTerms(const Pred& pred, Func&& fn) {
    auto res = pred.template Terms<N, Func>(ratings_, sim_, std::move(fn));
    return res;
  }

  virtual value_type Predict(const PredictVec<Derived>& pred) = 0;

  const C& ratings_;
  const C& sim_;
  value_type avg_;
};

template <class C>
class ItemFilterVec: public CollaborativeModelVec<C, ItemFilterVec<C>> {
  // Every class derived from model must declare predict as friend
  using Pred = PredictVec<ItemFilterVec>;
  friend Pred;

 public:
  using value_type = typename C::value_type;
  using Base = CollaborativeModelVec<C, ItemFilterVec<C>>;

  ItemFilterVec(const C& ratings, const C& sim)
    :CollaborativeModelVec<C, ItemFilterVec<C>>(ratings, sim) {}

 private:
  /**
   * This method is used by Predict class to predict a value based
   * on the model
   */
  value_type Predict(const Pred& pred) override {
    auto arr = this->template PredTerms<2>(pred,
      [this](size_t /*i*/, float v1, float v2, std::array<float, 2> arr){
        std::array<float,2> terms;
        terms[0] = v1*v2;
        terms[1] = v2;

        terms[0] += arr[0];
        terms[1] += arr[1];

        return terms;
      });

    if (arr[1] == 0)
      ERISED_Error(Error::DIVIDE_BY_ZERO, "Divide by zero exception");

    return arr[0]/arr[1];
  }

};
}
