#pragma once

#define ERISED_FLANN_H_

#include <initializer_list>
#include <vector>
#include <array>
#include <memory>
#include <flann/flann.hpp>

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

  SimMat() = default;
  SimMat(const std::vector<T>& v, size_t rows, size_t cols);
  SimMat(std::vector<T>&& v, size_t rows, size_t cols);

  T& operator()(size_t x, size_t y);
  const T& operator()(size_t x, size_t y) const;

  T* Data() {
    return mat_.data();
  }

 private:
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

template<class Distance>
class Index {
 public:
  template<class T>
  inline Index(const SimMat<T>& data, const IndexParams& params)
    : index_(::flann::Matrix<T>(data.Data()), params){}

  ~Index() {}

  inline void BuildIndex() {
    index_.buildIndex();
  }

  // TODO: Throws an exception if doesn't have enough memory allocated for
  // indices and dists
  template<class T>
  inline void KnnSearch(const SimMat<T>& query, SimMat<size_t>& indices,
                 SimMat<T>& dists, size_t nn,
                 const SearchParams& params) {
    ::flann::Matrix<T> fquery(query.Data());

    // TODO: Uses allocator insted of new operator
    ::flann::Matrix<int> findices(new int[fquery.rows*nn], fquery.rows, nn);
    ::flann::Matrix<T> fdists(new T[fquery.rows*nn], fquery.rows, nn);

    index_.knnSearch(fquery, findices, fdists, nn, params);

    dists.Data() = fdists.ptr();
    indices.Data() = findices.ptr();
  }

 private:
  Index<Distance> index_;
};

}}
