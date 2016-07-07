#pragma once

#define ERISED_FLANN_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>
#include <flann/flann.hpp>

#include "exception.h"

namespace erised { namespace flann {



class Mat;

template<class T>
class SimMatRef {
 friend class Mat;
 public:
  SimMatRef(const SimMatRef& mr);
  SimMatRef(SimMatRef&& mr);

  SimMatRef<T>& operator=(const SimMatRef<T>& mr);
  SimMatRef<T>& operator=(SimMatRef<T>&& mr);

  T& operator[](size_t);
  const T& operator[](size_t) const;

 private:
  SimMatRef(T* ptr, size_t step);
  T* ptr_;
  size_t step_;
};

template<class T>
class SimMat {
 public:
  static constexpr bool continuous = true;

  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;

  inline SimMat(): rows_(0), cols_(0) {}

  inline SimMat(size_t rows, size_t cols)
    : rows_(rows), cols_(cols), mat_(rows*cols) {}

  inline SimMat(const std::vector<T>& v, size_t rows, size_t cols)
    : rows_(0), cols_(0), mat_(v) {}

  inline SimMat(std::vector<T>&& v, size_t rows, size_t cols)
    : rows_(0), cols_(0), mat_(std::move(v)) {}

  inline T& operator()(size_t x, size_t y) {
    return mat_[cols_*x + y];
  }

  inline const T& operator()(size_t x, size_t y) const {
    return mat_[cols_*x + y];
  }

  inline const T* Data() const noexcept {
    return mat_.data();
  }

  inline T* Data() noexcept {
    return mat_.data();
  }

  inline size_t Size() const noexcept {
    return  mat_.size();
  }

  inline size_t Capacity() const noexcept {
    return  mat_.capacity();
  }

  inline size_t Rows() const noexcept {
    return rows_;
  }

  inline size_t Cols() const noexcept {
    return cols_;
  }

 private:
  size_t rows_;
  size_t cols_;
  std::vector<T> mat_;
};


template<class T>
using L2_Simple = struct ::flann::L2_Simple<T>;

template<class T>
using L2_3D = struct ::flann::L2_3D<T>;

template<class T>
using L2 = struct ::flann::L2<T>;

template<class T>
using L1 = struct ::flann::L1<T>;

template<class T>
using MinkowskiDistance = struct ::flann::MinkowskiDistance<T>;

template<class T>
using MaxDistance = struct ::flann::MaxDistance<T>;

typedef struct ::flann::HammingLUT HammingLUT;

template<class T>
using HammingPopcnt = struct ::flann::HammingPopcnt<T>;

template<class T>
using Hamming = struct ::flann::Hamming<T>;

template<class T>
using HistIntersectionDistance = struct ::flann::HistIntersectionDistance<T>;

template<class T>
using HellingerDistance = struct ::flann::HellingerDistance<T>;

template<class T>
using ChiSquareDistance = struct ::flann::ChiSquareDistance<T>;

template<class T>
using KL_Divergence = struct ::flann::KL_Divergence<T>;


typedef ::flann::IndexParams IndexParams;

typedef ::flann::LinearIndexParams LinearIndexParams;

typedef ::flann::SavedIndexParams SavedIndexParams;

typedef ::flann::CompositeIndexParams CompositeIndexParams;

typedef ::flann::AutotunedIndexParams AutotunedIndexParams;

#ifdef HAVE_CUDA
typedef ::flann::KDTreeCuda3dIndexParams KDTreeCuda3dIndexParams;
#endif

typedef ::flann::LshIndexParams LshIndexParams;

typedef ::flann::HierarchicalClusteringIndexParams HierarchicalClusteringIndexParams;

typedef ::flann::KDTreeSingleIndexParams KDTreeSingleIndexParams;

typedef ::flann::KDTreeIndexParams KDTreeIndexParams;

typedef ::flann::KMeansIndexParams KMeansIndexParams;


typedef struct ::flann::SearchParams SearchParams;

template<typename Distance>
class Index {
 public:
  using Value = typename ::flann::Index<Distance>::ElementType;

  inline Index(const SimMat<Value>& data, const IndexParams& params)
    : index_(::flann::Matrix<Value>(const_cast<Value*>(data.Data()),
                                    data.Rows(),
                                    data.Cols()), params){}

  ~Index() {}

  inline void BuildIndex() {
    index_.buildIndex();
  }

  void KnnSearch(const SimMat<Value>& query, SimMat<size_t>& indices,
                 SimMat<Value>& dists, size_t nn,
                 const SearchParams& params) {
    if (indices.Size() != query.Rows()*nn)
      ERISED_Error(Error::BAD_ALLOC, "Indices matrix has no enough size");

    if (dists.Size() != query.Rows()*nn)
      ERISED_Error(Error::BAD_ALLOC, "Indices matrix has no enough size");

    ::flann::Matrix<Value> fquery(const_cast<Value*>(query.Data()),
                                  query.Rows(), query.Cols());

    // TODO: Uses allocator insted of new operator
    ::flann::Matrix<size_t> findices(new size_t[fquery.rows*nn], fquery.rows, nn);
    ::flann::Matrix<Value> fdists(new Value[fquery.rows*nn], fquery.rows, nn);

    index_.knnSearch(fquery, findices, fdists, nn, params);

//     dists.Data() = const_cast<Value*>(fdists.ptr());
//     indices.Data() = const_cast<size_t*>(findices.ptr());
  }

 private:
  ::flann::Index<Distance> index_;
};

}}
