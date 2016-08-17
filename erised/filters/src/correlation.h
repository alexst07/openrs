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

  std::tuple<MatNeighbors, MatIndexes>
  Search(MatIn data_test, size_t n, const flann::SearchParams& sparams) {
    size_t num_rows = this->mat_.Rows();
    size_t num_cols = this->mat_.Cols();

    MatIndexes indices(this->mat_.Rows(), n);

    for (size_t i = 0; i < num_rows; i++) {

      // Gets num_cols elements from sim[num_cols*i]
      flann::Mat<value_type, Alloc> row(&this->mat_.Data()[num_cols*i], num_cols, 1);

      flann::Mat<size_t, Alloc> indices_row;

      flann::Index<value_type, flann::L2<value_type>> index(row, iparams_);

      // Writes in each row from matrix indices_ and dists_
      std::tie(indices_row, std::ignore) = index.KnnSearch(data_test, n, sparams);

      indices.SetRow(i, indices_row);
    }

    MatNeighbors neighbors = CalcNeighbors(n, indices);

    return std::tuple<MatNeighbors, MatIndexes>(std::move(neighbors),
                                                std::move(indices));
  }

 private:
   MatNeighbors CalcNeighbors(size_t n, const MatIndexes indices) {
     MatNeighbors neighbors(this->mat_.Rows(), n);

    // TODO: Change this for to parallel_for
    size_t row = 0;
    for (size_t i = 0; i < indices.Rows(); i++) {
      auto indices_row = indices.Row(i);

      for (size_t j = 0; j < indices_row.Size(); j++) {
        size_t indice = indices_row[j];
        neighbors(this->mat_(i, (indice > i? --indice:indice)), i, j);
      }
    }

    return neighbors;
  }

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

  virtual const Sim& Similarity() const noexcept {
    return sim_;
  }

  virtual Sim& Similarity() noexcept {
    return sim_;
  }

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

template<class T, template<typename = T> class Alloc = std::allocator>
class SimNeighbors {
 public:
  using Sim = flann::SimMat<T, Alloc>;
  using value_type = typename Sim::value_type;
  using Mat = flann::Mat<value_type, Alloc>;
  using MatSize = flann::Mat<size_t, Alloc>;

  template<class Data>
  SimNeighbors(const Correlation<Data, Sim>& correlations)
    : sim_(correlations.Similarity()) {}

  SimNeighbors(const Sim& sim): sim_(sim) {}

  std::tuple<Mat, MatSize> Search(size_t n, const flann::IndexParams& iparams,
                                  const flann::SearchParams& sparams) {
    Knn<Sim> knn(sim_, iparams);

    std::vector<value_type> datav = {1};
    Mat data_test(datav, 1, 1);

    return knn.Search(data_test, n, sparams);
  }

 private:
  const Sim& sim_;
};

}
