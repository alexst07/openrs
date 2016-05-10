#pragma once
#define ERISED_CSR_MATRIX_H_

#include "data_base.h"
#include "cuda_ptr.h"

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
  void Map(MapFn&& fn) override;

  template<typename MapFn>
  void RowMap(size_t i, MapFn&& fn) override;

  template<typename MapFn>
  void ColMap(size_t i, MapFn&& fn) override;

  template<typename ReduceFn>
  T Reduce(ReduceFn&& fn) override;

  template<typename ReduceFn>
  T RowReduce(size_t i, ReduceFn&& fn) override;

  template<typename ReduceFn>
  T ColReduce(size_t i, ReduceFn&& fn) override;

  void AddRow(const std::vector<T>& row);
  void AddRow(const T* row, size_type size);

  void AddCol(const std::vector<T>& col);
  void AddCol(const T* col, size_type size);

  template<typename U>
  friend std::ostream& operator<<(std::ostream& stream, const GpuCsr<U>& mat);

 private:
  cuda::cu_unique_ptr<int> rows_offset_;
  cuda::cu_unique_ptr<size_type> cols_index_;
  cuda::cu_unique_ptr<T> elems_;

  size_type size_rows_;
  size_type size_cols_;
};

}}

#include <data_csr-inl.h>
