#pragma once

#define ERISED_FLANN_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>
#include <tuple>
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

  using IterType = MatRef<T, Alloc>;
  using RowType = MatRef<value_type, Alloc>;

  inline Mat(): ::flann::Matrix<T>(), delete_(false) {}

  inline Mat(size_t rows, size_t cols)
    : delete_(true)
    , ::flann::Matrix<T>(new T[rows*cols], rows, cols) {}

  inline Mat(T* data, size_t rows, size_t cols)
  : delete_(false)
  , ::flann::Matrix<T>(data, rows, cols) {}

  inline Mat(const std::vector<T>& v, size_t rows, size_t cols)
    : delete_(false)
    , ::flann::Matrix<T>(const_cast<T*>(v.data()),
                         rows, cols) {}

  inline Mat(const Mat& mat) {
    // self-assignment check
    if (this != &mat) {
      delete_ = mat.delete_;
      this->rows = mat.rows;
      this->stride = mat.stride;
      this->cols = mat.cols;

      if (!delete_)
        this->data = reinterpret_cast<decltype(this->data)>(
            new T[this->rows*this->cols]);

      memcpy(this->data, mat.data, this->rows*this->cols*sizeof(T));
    }
  }


  Mat(Mat&& mat) {
    this->data = mat.data;
    mat.data = nullptr;

    this->rows = mat.rows;
    this->stride = mat.stride;
    this->cols = mat.cols;
    delete_ = mat.delete_;

    mat.rows = 0;
    mat.stride = 0;
    mat.cols = 0;
    mat.delete_ = false;
  }

  Mat& operator=(const Mat& mat) {
    // self-assignment check
    if (this != &mat) {
      delete_ = mat.delete_;
      this->rows = mat.rows;
      this->stride = mat.stride;
      this->cols = mat.cols;

      if (!delete_)
        this->data = reinterpret_cast<decltype(this->data)>(
            new T[this->rows*this->cols]);

      memcpy(this->data, mat.data, this->rows*this->cols*sizeof(T));
    }

    return *this;
  }

  Mat& operator=(Mat&& mat) {
    this->data = mat.data;
    mat.data = nullptr;

    this->rows = mat.rows;
    this->stride = mat.stride;
    this->cols = mat.cols;
    delete_ = mat.delete_;

    mat.rows = 0;
    mat.stride = 0;
    mat.cols = 0;
    mat.delete_ = false;

    return *this;
  }

  virtual ~Mat() {
    if (delete_)
      delete this->data;
  }

  // set value to position (x,y)
  virtual void operator()(T value, size_t x, size_t y) {
    reinterpret_cast<T*>(this->data)[this->cols*x + y] = value;
  }

  virtual const value_type& operator()(size_t x, size_t y) const {
    return reinterpret_cast<T*>(this->data)[this->cols*x + y];
  }

  virtual value_type& operator()(size_t x, size_t y) {
    return reinterpret_cast<T*>(this->data)[this->cols*x + y];
  }

  inline const value_type* Data() const noexcept {
    return reinterpret_cast<value_type*>(this->data);
  }

  inline size_t Size() const noexcept {
    return this->rows*this->cols;
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
    return this->rows;
  }

  inline size_t Cols() const noexcept {
    return this->cols;
  }

  RowType Row(size_t i) {
    return MatRef<value_type, Alloc>(Data() + i*Cols(), Cols());
  }

  inline T* Data() noexcept {
    return const_cast<T*>(reinterpret_cast<T*>(this->data));
  }

  inline size_t Capacity() const noexcept {
    return this->rows*this->cols;
  }

  template<class U, template<typename> class UAlloc>
  friend std::ostream& operator<<(std::ostream& stream,
                                  const Mat<U, UAlloc>& mat);

 private:
  bool delete_;
};

template<class U, template<typename> class UAlloc>
std::ostream& operator<<(std::ostream& stream, const Mat<U, UAlloc>& mat) {
  for (int i = 0; i < mat.Rows(); i++) {
    for (int j = 0; j < mat.Cols(); j++) {
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

  using Base = Mat<T, Alloc>;
  using RowType = typename Base::RowType;

  inline SimMat(): Mat<T, Alloc>() {}

  /**
   * the size of sim matrix is 4
   *
   * 1 2 3 4
   * 2 1 5 6
   * 3 5 1 2
   * 4 6 2 1
   *
   * if the diagonal is taken off
   *
   * 2 3 4
   * 2 5 6
   * 3 5 2
   * 4 6 2
   *
   * the matrix has now a col with size 3,
   * so the size of col must 1 minus the
   * size of rows
   */
  inline SimMat(size_t size)
    : Mat<T, Alloc>(size, size - 1) {}

  inline SimMat(T* data, size_t size)
    : Mat<T, Alloc>(data, size, size - 1) {}

  inline SimMat(const std::vector<T>& v, size_t size)
    : Mat<T, Alloc>(size, size - 1) {}

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
    // Diagonal elements must no be set, because it is always 1
    // on similarities matrix
    if (x == y)
      return;

    size_t xt = x;

    // If the position is is higher than the position of the
    // diagonal element must subtract its position, since
    // in this case the diagonal does not exist, because
    // it is always formed by 1's
    if (xt > y)
      xt--;

    reinterpret_cast<T*>(this->data)[this->Cols()*y + xt] = value;

    // as the matrix of similarities is symmetrical with its diagonal
    // with unitary elements, in which case you need to change the
    // value in two places to achieve this behavior
    if (y > x)
      y--;

    reinterpret_cast<T*>(this->data)[this->Rows()*x + y] = value;
  }

  inline T& operator()(size_t x, size_t y) {
    // If the position is is higher than the position of the
    // diagonal elementin the row, it must subtract its position,
    // since in this case the diagonal does not exist, because
    // it is always formed by 1's
    if (x > y)
      x--;
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

template<class T, class Distance, template<typename = T> class Alloc =
    std::allocator>
class Index {
 public:
  using type_value = typename ::flann::Index<Distance>::ElementType;
  using FMat = Mat<T, Alloc>;
  using FMatSize = Mat<size_t, Alloc>;

  inline Index(const FMat& data, const IndexParams& params)
    : index_(::flann::Matrix<type_value>(const_cast<type_value*>(data.Data()),
                                    data.Rows(),
                                    data.Cols()), params){
    BuildIndex();
  }

  ~Index() {}

  std::tuple<FMatSize, FMat> KnnSearch(const FMat& query, size_t nn,
                                     const SearchParams& params) {
    FMatSize indices(query.Rows(), nn);
    FMat dists(query.Rows(), nn);

    ::flann::Matrix<type_value> fquery(const_cast<type_value*>(query.Data()),
                                  query.Rows(), query.Cols());

    ::flann::Matrix<size_t> findices(const_cast<size_t*>(indices.Data()),
                                     indices.Rows(), indices.Cols());

    ::flann::Matrix<type_value> fdists(const_cast<type_value*>(dists.Data()),
                                  dists.Rows(), dists.Cols());

    index_.knnSearch(fquery, findices, fdists, nn, params);

    return std::tuple<FMatSize, FMat>(std::move(indices), std::move(dists));
  }

 private:
  inline void BuildIndex() {
    index_.buildIndex();
  }

  ::flann::Index<Distance> index_;
};

}}
