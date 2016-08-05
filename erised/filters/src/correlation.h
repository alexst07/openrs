#pragma once

#define ERISED_CORRELATION_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>
#include <unordered_map>

#include "exception.h"
#include "data_base.h"
#include "basic_statistic.h"
#include "parallel.h"
#include "flann.h"

namespace erised {

template<class Sim>
class NeighborsBase {
 public:
  using value_type = typename Sim::value_type;

  template<class T>
  using Mat = typename std::vector<std::vector<T>>;

  NeighborsBase(Sim& sim, size_t n): sim_(sim), n_(n) {}

  virtual ~NeighborsBase() = default;

  Mat<value_type> Neighbors() const noexcept = 0;

  virtual const Mat<size_t>& Indexes() const noexcept = 0;

  virtual std::vector<value_type> Neighbors(size_t) const noexcept = 0;

  virtual std::vector<size_t> Indexes(size_t) const noexcept = 0;

 protected:
  Sim& sim_;
  size_t n_;
};

template<class Sim>
class Neighbors: public NeighborsBase<Sim> {
 public:
  using value_type = typename Sim::value_type;


  template<class T>
  using Mat = typename NeighborsBase<Sim>::Mat<T>;

  Neighbors(Sim& sim, size_t n, const IndexParams& iparams,
            const SearchParams& sparams)
      : NeighborsBase(sim, n) {

  }

  virtual ~Neighbors() = default;

  Mat<value_type> Neighbors() const noexcept override {

  }

  const Mat<value_type>& Distances() const noexcept {

  }

  const Mat<size_t>& Indexes() const noexcept override {

  }

  const Mat<value_type>& Distances(size_t i) const noexcept {

  }

  virtual std::vector<value_type> Neighbors(size_t i) const noexcept override {

  }

  virtual std::vector<size_t> Indexes(size_t i) const noexcept override {

  }
};

template<>
template<class T, class Alloc>
class Neighbors<flann::SimMat<T, Alloc>>
    : public NeighborsBase<flann::SimMat<T, Alloc>> {
 public:
  using value_type = typename flann::SimMat<T, Alloc>::value_type;
  using Sim = typename flann::SimMat<T, Alloc>;

  template<class T>
  using Mat = typename NeighborsBase<flann::SimMat<T, Alloc>>::Mat<T>;

  Neighbors(Sim& sim, size_t n, const IndexParams& iparams,
            const SearchParams& sparams)
      : NeighborsBase(sim, n)
      , indices_(this->sim_.Rows(), n)
      , dists_(this->sim_.Rows(), n) {
    size_t num_rows = sim.Rows();
    size_t num_cols = sim.Cols();

    for (size_t i = 0; i < num_rows; i++) {
      // Gets num_cols elements from sim[num_cols*i]
      flann::Mat<value_type, Alloc> row(sim.Data + num_cols*i, num_cols);

      flann::Index<L2<float>> index(this->sim_, iparams);
      index.BuildIndex();
      index.KnnSearch(row, const_cast<flann::Mat<size_t>>(indices_[i]),
                      const_cast<flann::Mat<T, Alloc>>(dists_[i]), n, sparams);
    }
  }

  virtual ~Neighbors() = default;

  Mat<value_type> Neighbors() const noexcept override {
    Mat<value_type> mat(indices_.size());

    // TODO: Change this for to parallel_for
    size_t row = 0;
    for (const auto& indices: indices_) {
      std::vector<value_type> vec(this->n_);

      for (const auto& indice: indices) {
        vec.insert(sim_(indice, row));
      }

      mat.insert(std::move(vec));

      row++;
    }
  }

  const Mat<value_type>& Distances() const noexcept {
    value_type* data = indices_.Data();
    std::vector<value_type> vec(data, indices_.size());
    return vec;
  }

  const Mat<size_t>& Indexes() const noexcept override {
    return indices_;
  }

  const Mat<value_type>& Distances(size_t i) const noexcept {
    value_type* data = dists_[i].Data();
    std::vector<value_type> vec(data, indices_.size());
    return vec;
  }

  virtual std::vector<value_type> Neighbors(size_t i) const noexcept override {
    size_t row = 0;
    auto indices = indices_[i];
    std::vector<value_type> vec(this->n_);

    for (const auto& indice: indices) {
      vec.insert(sim_(indice, i));
    }

    return vec;
  }

  virtual std::vector<size_t> Indexes(size_t i) const noexcept override {
    value_type* data = indices_[i].Data();
    std::vector<value_type> vec(data, indices_.size());
    return vec;
  }

 private:
  std::vector<flann::Mat<size_t>> indices_;
  std::vector<flann::Mat<T, Alloc>> dists_;
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

    // To calculates all similarities with have a serie:]
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
  Pearson(): Correlation(Axis::ROW) {}
};

template<class Data, class Sim>
class CossineAdjusted: public Correlation<Data, Sim> {
  CossineAdjusted(): Correlation(Axis::COL) {}
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
