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

template<class T, class Alloc = std::allocator<T>>
class SimMat: protected ::flann::Matrix<T> {
 public:
  static constexpr bool continuous = true;

  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;

  inline SimMat(): size_(0), delete_(false) {}

  inline SimMat(size_t size)
    : size_(size)
    , delete_(true)
    , ::flann::Matrix<T>(new T[size*size], size, size) {}

  inline SimMat(T* data, size_t size)
  : size_(size), delete_(false), ::flann::Matrix<T>(data, size, size) {}

  inline SimMat(const std::vector<T>& v, size_t size)
    : size_(size)
    , delete_(false)
    , ::flann::Matrix<T>(reinterpret_cast<T*>(v.data()), size, size) {}

  SimMat(SimMat&& sim) {
    this->data = sim.data;
    sim.data = nullptr;

    this->rows = sim.rows;
    this->stride = sim.stride;
    this->cols = sim.cols;
    size_ = sim.size_;
    delete_ = sim.delete_;

    sim.rows = 0;
    sim.stride = 0;
    sim.cols = 0;
    sim.size_ = 0;
    sim.delete_ = false;
  }

  SimMat<T, Alloc>& operator=(const SimMat<T, Alloc>& sim) {
    // self-assignment check
    if (this != &sim) {
      size_ = sim.size_;
      delete_ = sim.delete_;
    }

    return *this;
  }

  SimMat<T, Alloc>& operator=(SimMat<T, Alloc>&& sim) {
    this->data = sim.data;
    sim.data = nullptr;

    this->rows = sim.rows;
    this->stride = sim.stride;
    this->cols = sim.cols;
    size_ = sim.size_;
    delete_ = sim.delete_;

    sim.rows = 0;
    sim.stride = 0;
    sim.cols = 0;
    sim.size_ = 0;
    sim.delete_ = false;

    return *this;
  }

  inline ~SimMat() {
    if (delete_)
      delete this->data;
  }

  inline void operator()(T value, size_t x, size_t y) {
    reinterpret_cast<T*>(this->data)[size_*x + y] = value;
    reinterpret_cast<T*>(this->data)[size_*y + x] = value;
  }

  inline T& operator()(size_t x, size_t y) {
    return reinterpret_cast<T*>(this->data)[size_*x + y];
  }

  inline const T& operator()(size_t x, size_t y) const {
    return reinterpret_cast<T*>(this->data)[size_*x + y];
  }

  inline const T* Data() const noexcept {
    return this->data;
  }

  inline T* Data() noexcept {
    return this->data;
  }

  inline size_t Capacity() const noexcept {
    return size_;
  }

  inline ::flann::Matrix<T>& FlannMat() noexcept {
    return ::flann::Matrix<T>(this->data, size_, size_);
  }

  template<typename U, typename UAlloc>
  friend std::ostream& operator<<(std::ostream& stream, const SimMat<U, UAlloc>& mat);

 private:
  size_t size_;
  bool delete_;
};

template<typename U, typename UAlloc>
std::ostream& operator<<(std::ostream& stream, const SimMat<U, UAlloc>& mat) {
  for (int i = 0; i < mat.size_; i++) {
    for (int j = 0; j < mat.size_; j++) {
      stream << mat(i, j) << " ";
    }
    stream << "\n";
  }

  return stream;
}

/******************************************************************************
 * Distance templates
 *****************************************************************************/
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

/******************************************************************************
 * Index params for knn search
 *****************************************************************************/
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

/******************************************************************************
 * FLANN paramters as check, eps, max_neighbors, etc.
 *****************************************************************************/
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
    if (indices.Capacity() != query.Rows()*nn)
      ERISED_Error(Error::BAD_ALLOC, "Indices matrix has no enough size");

    if (dists.Capacity() != query.Rows()*nn)
      ERISED_Error(Error::BAD_ALLOC, "Indices matrix has no enough size");

    ::flann::Matrix<Value> fquery(const_cast<Value*>(query.Data()),
                                  query.Rows(), query.Cols());

    index_.knnSearch(fquery, indices.FlannMat(), dists.FlannMat(), nn, params);
  }

 private:
  ::flann::Index<Distance> index_;
};

}}
