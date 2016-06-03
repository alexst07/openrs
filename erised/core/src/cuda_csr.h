#pragma once
#define ERISED_CUDA_CSR_MATRIX_H_

#include "data_base.h"

#include <initializer_list>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>

// namespace erised::cuda
namespace erised { namespace cuda {

template<typename T>
class GpuCsr {
 public:
  using size_type = std::size_t;
  using ElemIter = typename std::vector<T>::iterator;
  using ConstElemIter = typename std::vector<T>::const_iterator;

  static const int INVALID_LINE;

  GpuCsr();
  GpuCsr(size_type rows, size_type cols);
  GpuCsr(std::initializer_list<std::initializer_list<T>> set);
  GpuCsr(const GpuCsr<T>& m);
  GpuCsr(GpuCsr<T>&& m);

  GpuCsr<T>& operator=(const GpuCsr<T>& m);
  GpuCsr<T>& operator=(GpuCsr<T>&& m);

  template<typename MapFn>
  void Map(MapFn&& fn);

  template<typename MapFn>
  void RowMap(size_t i, MapFn&& fn);

  template<typename MapFn>
  void ColMap(size_t i, MapFn&& fn);

  template<typename ReduceFn>
  T Reduce(ReduceFn&& fn);

  template<typename ReduceFn>
  T RowReduce(size_t i, ReduceFn&& fn);

  template<typename ReduceFn>
  T ColReduce(size_t i, ReduceFn&& fn);

  void AddRow(const std::vector<T>& row);
  void AddRow(const T* row, size_type size);

  void AddCol(const std::vector<T>& col);
  void AddCol(const T* col, size_type size);

  template<typename U>
  friend std::ostream& operator<<(std::ostream& stream, const GpuCsr<U>& mat);

 private:
  void alloc(size_type nrows, size_type nelems);
  void free();

  template<typename MapFn>
  __global__  void CuMap(MapFn&& fn);

  template<typename MapFn>
  __global__  void CuRowMap(size_t i, MapFn&& fn);

  int* rows_offset_;
  size_type* cols_index_;
  T* elems_;

  __host__ __device__ size_type size_rows_;
  __host__ __device__ size_type size_cols_;
  __host__ __device__ size_type num_elems_;
};

}}

#include "cuda_csr-inl.h"
