#pragma once

#define ERISED_FLANN_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>
#include <flann/flann.hpp>

#include "exception.h"
#include "data_base.h"

namespace erised { namespace flann {

template<class T, template<typename = T> class Alloc = std::allocator>
class Mat;

template<class T, template<typename = T> class Alloc = std::allocator>
class MatRef: public VecContinuous<T> {

  friend class Mat<T, Alloc>;

 public:
  using value_type = T;

  template<class A>
  using alloc = Alloc<A>;

  MatRef(const MatRef& mr)
    : ptr_(mr.ptr_), size_(mr.size_), step_(mr.step_) {}

  MatRef(MatRef&& mr)
    : ptr_(mr.ptr_), size_(mr.size_), step_(mr.step_) {
    mr.ptr_ = nullptr;
    mr.size_ = 0;
  }

  MatRef& operator=(const MatRef& mr) {
    ptr_ = mr.ptr_;
    size_ = mr.size_;
    step_ = mr.step_;
  }

  MatRef& operator=(MatRef&& mr) {
    ptr_ = mr.ptr_;
    size_ = mr.size_;
    step_ = mr.step_;

    mr.ptr_ = nullptr;
    mr.size_ = 0;
  }

  value_type& operator[](size_t i) override {
    return ptr_[i*step_];
  }

  const value_type& operator[](size_t i) const override {
    return ptr_[i*step_];
  }

  size_t Size() const noexcept override {
    return size_;
  }

  value_type* Data() noexcept override {
    return ptr_;
  }

  const value_type* Data() const noexcept override {
    return ptr_;
  }

 private:
  MatRef(T* ptr, size_t size, size_t step = 1)
    : ptr_(ptr)
    , size_(size)
    , step_(step) {}

  T* ptr_;
  size_t size_;
  size_t step_;
};

template<class T, template<typename> class Alloc>
class Mat: protected ::flann::Matrix<T>,
    public MatIter<T, Mat<T, Alloc>>{
 public:
  static constexpr bool continuous = true;

  using value_type = T;

  template<class A>
  using alloc = Alloc<A>;

  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;
  using IterType = MatRef<T, Alloc>;

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
    , ::flann::Matrix<T>(const_cast<T*>(v.data()),
                         row_size_, col_size_) {}

  inline Mat(const Mat& mat) {
    // self-assignment check
    if (this != &mat) {
      row_size_ = mat.row_size_;
      col_size_ = mat.row_size_;
      delete_ = mat.delete_;
      this->rows = mat.rows;
      this->stride = mat.stride;
      this->cols = mat.cols;

      if (!delete_)
        this->data = reinterpret_cast<decltype(this->data)>(
            new T[row_size_*col_size_]);

      memcpy(this->data, mat.data, col_size_*col_size_*sizeof(T));
    }
  }


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

  Mat& operator=(const Mat& mat) {
    // self-assignment check
    if (this != &mat) {
      row_size_ = mat.row_size_;
      col_size_ = mat.row_size_;
      delete_ = mat.delete_;
      this->rows = mat.rows;
      this->stride = mat.stride;
      this->cols = mat.cols;

      if (!delete_)
        this->data = reinterpret_cast<decltype(this->data)>(
            new T[row_size_*col_size_]);

      memcpy(this->data, mat.data, col_size_*col_size_*sizeof(T));
    }

    return *this;
  }

  Mat& operator=(Mat&& mat) {
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

  // set value to position (x,y)
  virtual void operator()(T value, size_t x, size_t y) {
    reinterpret_cast<T*>(this->data)[col_size_*x + y] = value;
  }

  virtual const value_type& operator()(size_t x, size_t y) const {
    return reinterpret_cast<T*>(this->data)[col_size_*x + y];
  }

  virtual value_type& operator()(size_t x, size_t y) {
    return reinterpret_cast<T*>(this->data)[col_size_*x + y];
  }

  inline const value_type* Data() const noexcept {
    return this->data;
  }

  inline size_t Size() const noexcept {
    return this->row_size_*this->col_size_;
  }

  inline size_t size() const noexcept {
    return Size();
  }

  virtual value_type& operator[](size_t i) {
    return Data()[i];
  }

  virtual size_t SizeIter() const noexcept {
    return Rows()*Cols();
  }

  inline size_t Rows() const noexcept {
    return this->row_size_;
  }

  inline size_t Cols() const noexcept {
    return this->col_size_;
  }

  MatRef<value_type> Row(size_t i) {
    return MatRef<value_type, Alloc>(Data() + i*Cols(), Cols());
  }

  inline T* Data() noexcept {
    return reinterpret_cast<T*>(this->data);
  }

  inline size_t Capacity() const noexcept {
    return row_size_*col_size_;
  }

  inline ::flann::Matrix<T>& FlannMat() noexcept {
    return ::flann::Matrix<T>(this->data, row_size_, col_size_);
  }

  template<class U, template<typename> class UAlloc>
  friend std::ostream& operator<<(std::ostream& stream,
                                  const Mat<U, UAlloc>& mat);

 protected:
  size_t row_size_;
  size_t col_size_;

 private:
  bool delete_;
};

template<class U, template<typename> class UAlloc>
std::ostream& operator<<(std::ostream& stream, const Mat<U, UAlloc>& mat) {
  for (int i = 0; i < mat.row_size_; i++) {
    for (int j = 0; j < mat.col_size_; j++) {
      stream << mat(i, j) << " ";
    }
    stream << "\n";
  }

  return stream;
}

template<class T, template<typename = T> class Alloc = std::allocator>
class SimMat: public Mat<T, Alloc> {
 public:
  using value_type = T;

  template<class A>
  using alloc = Alloc<A>;

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

  inline T& operator()(size_t x, size_t y) {
    return Mat<T, Alloc>::operator()(x, y);
  }

  inline const T& operator()(size_t x, size_t y) const {
    return Mat<T, Alloc>::operator()(x, y);
  }

  inline const T* Data() const noexcept {
    return this->data;
  }

  inline size_t Size() const noexcept {
    return this->row_size_;
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

  inline Index(const Mat<Value>& data, const IndexParams& params)
    : index_(::flann::Matrix<Value>(const_cast<Value*>(data.Data()),
                                    data.Rows(),
                                    data.Cols()), params){}

  ~Index() {}

  inline void BuildIndex() {
    index_.buildIndex();
  }

  void KnnSearch(const Mat<Value>& query, Mat<size_t>& indices,
                 Mat<Value>& dists, size_t nn,
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
