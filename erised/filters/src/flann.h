#pragma once

#define ERISED_FLANN_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>
#include <flann/flann.hpp>

#include "exception.h"

namespace erised { namespace flann {

template<class T, class Alloc = std::allocator<T>>
class Mat;

template<class T, class Alloc = std::allocator<T>>
class SimMatRef {
 friend class Mat<T, Alloc>;
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

template<class T, class Alloc>
class Mat: protected ::flann::Matrix<T> {
 public:
  static constexpr bool continuous = true;

  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;

  inline Mat(): row_size_(0), col_size_(0), delete_(false) {}

  inline Mat(size_t rows, size_t cols)
    : row_size_(rows)
    , col_size_(cols)
    , delete_(true)
    , ::flann::Matrix<T>(new T[rows*cols], row_size_, col_size_) {}

  inline Mat(T* data, size_t rows, size_t cols)
  : row_size_(rows)
  , col_size_(cols)
  , delete_(false)
  , ::flann::Matrix<T>(data, rows, cols) {}

  inline Mat(const std::vector<T>& v, size_t rows, size_t cols)
    : row_size_(rows)
    , col_size_(cols)
    , delete_(false)
    , ::flann::Matrix<T>(reinterpret_cast<T*>(v.data()), row_size_, col_size_)
    {}

  Mat(Mat&& mat) {
    this->data = mat.data;
    mat.data = nullptr;

    this->rows = mat.rows;
    this->stride = mat.stride;
    this->cols = mat.cols;
    row_size_ = mat.row_size_;
    col_size_ = mat.col_size_;
    delete_ = mat.delete_;

    mat.rows = 0;
    mat.stride = 0;
    mat.cols = 0;
    mat.row_size_ = 0;
    mat.col_size_ = 0;
    mat.delete_ = false;
  }

  Mat<T, Alloc>& operator=(const Mat<T, Alloc>& mat) {
    // self-assignment check
    if (this != &mat) {
      row_size_ = mat.row_size_;
      col_size_ = mat.row_size_;
      delete_ = mat.delete_;
    }

    return *this;
  }

  Mat<T, Alloc>& operator=(Mat<T, Alloc>&& mat) {
    this->data = mat.data;
    mat.data = nullptr;

    this->rows = mat.rows;
    this->stride = mat.stride;
    this->cols = mat.cols;
    row_size_ = mat.row_size_;
    col_size_ = mat.row_size_;
    delete_ = mat.delete_;

    mat.rows = 0;
    mat.stride = 0;
    mat.cols = 0;
    mat.row_size_ = 0;
    mat.col_size_ = 0;
    mat.delete_ = false;

    return *this;
  }

  virtual ~Mat() {
    if (delete_)
      delete this->data;
  }

  virtual void operator()(T value, size_t x, size_t y) {
    reinterpret_cast<T*>(this->data)[col_size_*x + y] = value;
  }

  inline T& operator()(size_t x, size_t y) {
    return reinterpret_cast<T*>(this->data)[col_size_*x + y];
  }

  inline const T& operator()(size_t x, size_t y) const {
    return reinterpret_cast<T*>(this->data)[col_size_*x + y];
  }

  inline const T* Data() const noexcept {
    return this->data;
  }

  inline T* Data() noexcept {
    return this->data;
  }

  inline size_t Capacity() const noexcept {
    return row_size_*col_size_;
  }

  inline ::flann::Matrix<T>& FlannMat() noexcept {
    return ::flann::Matrix<T>(this->data, row_size_, col_size_);
  }

  template<typename U, typename UAlloc>
  friend std::ostream& operator<<(std::ostream& stream, const Mat<U, UAlloc>& mat);

 protected:
  size_t row_size_;
  size_t col_size_;

 private:
  bool delete_;
};

template<typename U, typename UAlloc>
std::ostream& operator<<(std::ostream& stream, const Mat<U, UAlloc>& mat) {
  for (int i = 0; i < mat.row_size_; i++) {
    for (int j = 0; j < mat.col_size_; j++) {
      stream << mat(i, j) << " ";
    }
    stream << "\n";
  }

  return stream;
}

template<class T, class Alloc = std::allocator<T>>
class SimMat: public Mat<T, Alloc> {
 public:
  inline SimMat(): Mat<T, Alloc>() {}

  inline SimMat(size_t size)
    : Mat<T, Alloc>(size, size) {}

  inline SimMat(T* data, size_t size)
  : Mat<T, Alloc>(data, size, size) {}

  inline SimMat(const std::vector<T>& v, size_t size)
    : Mat<T, Alloc>(v, size, size) {}

  SimMat(SimMat&& sim): Mat<T, Alloc>(std::move(sim)) {}

  SimMat& operator=(const SimMat& sim) {
    Mat<T, Alloc>::operator=(sim);
    return *this;
  }

  SimMat& operator=(SimMat&& sim) {
    Mat<T, Alloc>::operator=(std::move(sim));
    return *this;
  }

  void operator()(T value, size_t x, size_t y) override {
    reinterpret_cast<T*>(this->data)[this->col_size_*x + y] = value;
    reinterpret_cast<T*>(this->data)[this->row_size_*y + x] = value;
  }
};


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
