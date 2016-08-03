#pragma once

#define ERISED_PREDICT_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>
#include <tuple>
#include <flann/flann.hpp>

#include "exception.h"
#include "correlation.h"
#include "data_base.h"
#include "model.h"

namespace erised {

template<class Data, class Sim, class Model>
class PredictData {
  friend Model;

 public:
  using value_type = typename Sim::value_type;

  PredictData(CollaborativeModel<Data, Sim>& model)
    : axis_(model.Axis())
    , model_(model) {}

  value_type Pred(size_t i) {

  }

 private:
  template<size_t N, class Func>
  std::array<value_type,N> Terms(const Data &data, const Sim& sim, size_t ri,
                                 const std::vector<size_t>& indexes,
                                 Func&& fn) {
    using const_iter = std::vector<size_t>::const_iterator;
    std::array<value_type,N> zarray{};

    // Use the index to get only elements that interest
    Range<const_iter> range(indexes.begin(), indexes.end());

    // Executes the function fn on all elments
    auto ret_arr = parallel_reduce(range, zarray,
        [&](const Range<const_iter>& r, std::array<value_type,N> value) {
          typename std::array<value_type,N> rets(std::move(value));

          for(auto it = r.begin(); it != r.end(); ++it) {
            // row gets the rating of an user and cols gets the ratings
            // for a specific item
            typename Data::value_type data_elem = static_cast<value_type>(0);
            if (axis_ == Axis::ROW) {
              data_elem = data(ri, *it);
            } else {
              data_elem = data(*it, ri);
            }

            // Similarity is a simetric matrix, so, (it, ri) == (ri, it)
            value_type sim_elem = sim(*it, ri);

            rets = fn(*it, data_elem, sim_elem, rets);
          }

          return rets;
        }, [](std::array<value_type,N> a, std::array<value_type,N> b) {
          std::array<value_type,N> acc;
          for (int i = 0; i < N; i++) {
            acc[i] = a[i] + b[i];
          }
          return acc;
      });

    return ret_arr;
  }

  template<size_t N, class Func>
  value_type Predict(const std::array<value_type,N>& arr, Func&& fn) {
    return fn(arr);
  }

  Axis axis_;
  CollaborativeModel<Data, Sim>& model_;
};

template<class Model>
class PredicVec {
  friend Model;

 public:
  PredicVec() = default;

 private:
  /**
   * To predict the rating for an user on an item, only its vector is needed
   */
  template<size_t N, class C, class Func>
  auto Terms(const C& data, C& sim, size_t ri, Func&& fn) ->
      std::array<typename C::value_type,N> {
    using value_type = typename C::value_type;

    if (data.size() != sim.size())
      ERISED_Error(Error::INVALID_ARGUMENT, "data vector and sim vector have "
                                            "differents sizes");

    using const_iter = std::vector<size_t>::const_iterator;
    std::array<value_type,N> zarray{};

    // Use the index to get only elements that interest
    Range<const_iter> range(0, data.size());

    // Executes the function fn on all elments
    auto ret_arr = parallel_reduce(range, zarray,
        [&](const Range<const_iter>& r, std::array<value_type,N> value) {
          typename std::array<value_type,N> rets(std::move(value));

          for(auto it = r.begin(); it != r.end(); ++it) {
            value_type data_elem = data[*it];
            value_type sim_elem = sim[*it];

            rets = fn(*it, data_elem, sim_elem, rets);
          }

          return rets;
        }, [](std::array<value_type,N> a, std::array<value_type,N> b) {
          std::array<value_type,N> acc;
          for (int i = 0; i < N; i++) {
            acc[i] = a[i] + b[i];
          }
          return acc;
      });

    return ret_arr;
  }

};

}
