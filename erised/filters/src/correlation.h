#pragma once

#define ERISED_CORRELATION_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include <tuple>

#include "exception.h"
#include "data_base.h"
#include "basic_statistic.h"
#include "parallel.h"
#include "flann.h"

namespace erised {

template<class MatIn, template<typename, template<typename> class> class Mat>
class NeighborsBase {
 public:
  using value_type = typename MatIn::value_type;
  using MatNeighbors = Mat<value_type, MatIn::template alloc>;
  using MatIndexes = Mat<size_t, MatIn::template alloc>;

  NeighborsBase(MatIn& mat): mat_(mat) {}

  virtual ~NeighborsBase() = default;

  virtual const MatNeighbors& Neighbors() const noexcept = 0;

  virtual const MatIndexes& Indexes() const noexcept = 0;

  virtual MatNeighbors& Neighbors() noexcept = 0;

  virtual MatIndexes& Indexes() noexcept = 0;

  virtual typename MatNeighbors::RowType Neighbors(size_t) noexcept = 0;

  virtual typename MatIndexes::RowType Indexes(size_t) noexcept = 0;

 protected:
  MatIn& mat_;
};

template<class Sim>
class Knn: public NeighborsBase<Sim, flann::Mat> {
 public:
  using value_type = typename Sim::value_type;
  using Base = NeighborsBase<Sim, flann::Mat>;
  using Mat = flann::Mat<typename Sim::value_type, Sim::template alloc>;
  using MatSizet = flann::Mat<size_t, Sim::template alloc>;

  Knn(Sim& sim, size_t n): Base(sim, n) {

  }

  virtual ~Knn() = default;

  Mat& Neighbors() noexcept override {

  }

  const Mat& Neighbors() const noexcept override {

  }

  const MatSizet& Indexes() const noexcept override {

  }

  MatSizet& Indexes() noexcept override {

  }

  typename Mat::Row Neighbors(size_t) noexcept override {

  }

  typename MatSizet::Row Indexes(size_t) noexcept override {

  }
};


template<>
template<class T, template<typename> class Alloc>
class Knn<flann::Mat<T, Alloc>>
    : public NeighborsBase<flann::Mat<T, Alloc>, flann::Mat> {

 public:
  using value_type = typename flann::Mat<T, Alloc>::value_type;
  using Base = NeighborsBase<flann::Mat<T, Alloc>, flann::Mat>;
  using Sim = flann::Mat<T, Alloc>;
  using MatIn = flann::Mat<T, Alloc>;
  using MatNeighbors = flann::Mat<typename Sim::value_type, Sim::template alloc>;
  using MatIndexes = flann::Mat<size_t, Sim::template alloc>;

  Knn(Sim& sim, const flann::IndexParams& iparams)
    : Base(sim)
    , iparams_(iparams) {}

  virtual ~Knn() = default;

  void Search(MatIn data_test, size_t n, const flann::SearchParams& sparams) {
    size_t num_rows = this->mat_.Rows();
    size_t num_cols = this->mat_.Cols();

    neighbors_ =
        std::move(flann::Mat<value_type, Alloc>(this->mat_.Rows(), n));

    indices_ = std::move(flann::Mat<size_t, Alloc>(this->mat_.Rows(), n));

    for (size_t i = 0; i < num_rows; i++) {

      // Gets num_cols elements from sim[num_cols*i]
      flann::Mat<value_type, Alloc> row(&this->mat_.Data()[num_cols*i], num_cols, 1);

      flann::Mat<size_t, Alloc> indices;

      flann::Index<value_type, flann::L2<value_type>> index(row, iparams_);

      // Writes in each row from matrix indices_ and dists_
      std::tie(indices, std::ignore) = index.KnnSearch(data_test, n, sparams);

      indices_.SetRow(i, indices);
    }

    CalcNeighbors();
  }

  virtual const MatNeighbors& Neighbors() const noexcept {
    return neighbors_;
  }

  virtual MatNeighbors& Neighbors() noexcept {
    return neighbors_;
  }

  virtual const MatIndexes& Indexes() const noexcept {
    return indices_;
  }

  virtual MatIndexes& Indexes() noexcept {
    return indices_;
  }

  virtual typename MatNeighbors::RowType Neighbors(size_t i) noexcept {
    return neighbors_.Row(i);
  }

  virtual typename MatIndexes::RowType Indexes(size_t i) noexcept {
    return indices_.Row(i);
  }

 private:
  void CalcNeighbors() {
    // TODO: Change this for to parallel_for
    size_t row = 0;
    for (size_t i = 0; i < indices_.Rows(); i++) {
      auto indices_row = indices_.Row(i);

      for (size_t j = 0; j < indices_row.Size(); j++) {
        size_t indice = indices_row[j];
        neighbors_(this->mat_(i, (indice > i? --indice:indice)), i, j);
      }
    }
  }

  MatNeighbors neighbors_;
  flann::Mat<size_t, Alloc> indices_;
  flann::IndexParams iparams_;
};

template<>
template<class T, template<typename> class Alloc>
class Knn<flann::SimMat<T, Alloc>>: public Knn<flann::Mat<T, Alloc>> {
 public:
  using value_type = typename flann::Mat<T, Alloc>::value_type;
  using Base = Knn<flann::Mat<T, Alloc>>;
  using Sim = flann::Mat<T, Alloc>;

  Knn(Sim& sim, const flann::IndexParams& iparams): Base(sim, iparams) {}

  virtual ~Knn() = default;
};

template<class Data, class Sim>
class Correlation {
  using value_type = typename Data::value_type;

 public:
  Correlation(Axis axis): axis_(axis) {}

  virtual ~Correlation() = default;

  virtual void Fit(const Data& data) {
    auto vec_rows = erised::Avarage(data, this->axis_);
    size_t data_size = this->axis_ == Axis::ROW?
        data.SizeRows(): data.SizeRows();

    // Calculates the avarages for all lines in the axis
    auto avgs = erised::Avarage(data, this->axis_);

    Sim sim(data_size);
    Range<size_t> range_sim(0, data_size);

    // To calculates all similarities, note that dynamic programming
    // can be used, for all item or user n, all n's before has already
    // been calculed, as:
    // (n-1) + (n-2) + (n-3) + ...
    // parallel_for is used for main loop, but each item
    // calculates its similarity serially
    parallel_for(range_sim, [&](const Range<size_t>& r){
      // Scan each line and search for specific column
      for(auto i = r.begin(); i != r.end(); ++i) {
        for (size_t j = i+1; j < data_size; j++) {
          auto arr = SimTerms(data, i, j, avgs[i], avgs[j]);
          sim(arr[0]/(sqrt(arr[1])*sqrt(arr[2])), i, j);
        }
      }
    });

    sim_ = std::move(sim);
  }

  const Sim& Similarity() const noexcept {
    return sim_;
  }

  Sim& Similarity() noexcept {
    return sim_;
  }

//   std::vector<std::unordered_map<size_t, std::vector<value_type>>>
//   Neighbors(size_t n) {
//
//   }
//
//   NeighborsElement(size_t i, size_t n) {
//
//   }
//
//   NeighborsIndexes(size_t n) {
//
//   }
//
//   NeighborsElementIndexes(size_t i, size_t n) {
//
//   }

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
  Axis axis_;
};

template<class Data, class Sim>
class Pearson: public Correlation<Data, Sim> {
  Pearson(): Correlation<Data, Sim>(Axis::ROW) {}
};

template<class Data, class Sim>
class CossineAdjusted: public Correlation<Data, Sim> {
  CossineAdjusted(): Correlation<Data, Sim>(Axis::COL) {}
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
