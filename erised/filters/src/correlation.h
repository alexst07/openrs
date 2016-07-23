#pragma once

#define ERISED_CORRELATION_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>

#include "exception.h"
#include "data_base.h"
#include "basic_statistic.h"
#include "parallel.h"

namespace erised {

template<class Data, class Sim>
class Correlation {
 public:
  virtual void Fit(Data &data) = 0;
  virtual const Sim& Similarity() const noexcept = 0;
  virtual Sim& Similarity() noexcept = 0;

//   virtual typename Sim<T, Alloc>::Slice Predict(size_t i) = 0;

 protected:
  Correlation(Axis axis):axis_(axis) {}

  Axis axis_;
};

template<class Data, class Sim>
class AdjustedCosine: public Correlation<Data, Sim> {
  using Base = Correlation<Data, Sim>;
  using value_type = typename Data::value_type;

 public:
  AdjustedCosine(Axis axis): Base(axis) {}

  void Fit(Data &data) override {
    auto vec_rows = erised::Avarage(data, this->axis_);
    size_t data_size = this->axis_ == Axis::ROW?
        data.SizeRows(): data.SizeRows();

    // Calculates the avarages for all lines in the axis
    auto avgs = erised::Avarage(data, this->axis_);

    Sim sim(data_size);
    Range<size_t> range_sim(0, data_size);

    // To calculates all similarities with have a serie:]
    // (n-1) + (n-2) + (n-3) + ...
    // parallel_for is used for main loop, but each item
    // calculates its similarity serially
    parallel_for(range_sim, [&](const Range<size_t>& r){
      // Scan each line and search for specific column
      for(auto i = r.begin(); i != r.end(); ++i) {
        for (size_t j = i+1; j < data_size; j++) {
          auto arr = SimTerms(data, i, j, avgs[i], avgs[j]);
          sim(i, j) = arr[0]/(sqrt(arr[1])*sqrt(arr[2]));
        }
      }
    });

    sim_ = std::move(sim_);
  }

  const Sim& Similarity() const noexcept override {
    return sim_;
  }

  Sim& Similarity() noexcept override {
    return sim_;
  }

//   typename Sim<T, Alloc>::Slice Predict(size_t i) override {
//
//   }

  std::array<value_type,3> SimTerms(const Data &data, size_t i, size_t j,
                                    value_type ia, value_type ja) {
    auto l = [ia, ja](value_type v1, value_type v2, std::array<value_type,3> ret) {
      std::array<float,3> r;
      r[0] = (v1 - ia)*(v2 - ja);

      value_type term = v1 - ia;
      r[1] = term*term;

      term = v2 - ja;
      r[2] = term*term;

      r[0] += ret[0];
      r[1] += ret[1];
      r[2] += ret[2];

      return r;
    };

    std::array<value_type,3> arr = data.template Reduce<3>(this->axis_, i, j, l);

    return arr;
  }

 private:
  Sim sim_;
};

// template<
//   class T,
//   class Alloc,
//   template<typename, typename> class Data,
//   template <typename, typename> class Sim>
// class Pearson: public Correlation<T, Alloc, Data, Sim> {
//  public:
//   virtual void Fit(Data<T, Alloc> &data) {
//
//   }
//
//   virtual const Sim<T, Alloc>& Similarity() const noexcept {
//
//   }
//
//   virtual Sim<T, Alloc>& Similarity() noexcept {
//
//   }
//
// //   virtual typename Sim<T, Alloc>::Slice Predict(size_t i) {
// //
// //   }
// };

}
