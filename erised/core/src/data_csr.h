#pragma once
#define ERISED_CSR_MATRIX_H_

#include "data_base.h"

#include <initializer_list>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>

namespace erised {

template<typename T>
class DataCsr: public DataBase<T> {
 public:
  using size_type = std::size_t;
  using MapFn = std::function<T(T)>;
  using ReduceFn = std::function<T(T,T)>;
  using ElemIter = typename std::vector<T>::iterator;
  using ConstElemIter = typename std::vector<T>::const_iterator;

  static const int INVALID_LINE;

  DataCsr();
  DataCsr(size_type rows, size_type cols);
  DataCsr(std::initializer_list<std::initializer_list<T>> set);
  DataCsr(const DataCsr<T>& m);
  DataCsr(DataCsr<T>&& m);

  DataCsr<T>& operator=(const DataCsr<T>& m);
  DataCsr<T>& operator=(DataCsr<T>&& m);

  void Map(const MapFn& fn);

  void RowMap(size_t i, MapFn fn);

  void ColMap(size_t i, MapFn fn);

  T Reduce(const ReduceFn& fn) const;

  T RowReduce(size_t i, const ReduceFn& fn) const;

  T ColReduce(size_t i, const ReduceFn& fn) const;

  T& operator()(size_type x, size_type y) override {}

  const T& operator()(size_type x, size_type y) const override {}

  size_t NumElements() const override {};

  size_t NumElementsLine(size_t i) const override {};

  size_t NumElementsCol(size_t i) const override {};

  void AddRow(const std::vector<T>& row);
  void AddRow(const T* row, size_type size);

  void AddCol(const std::vector<T>& col);
  void AddCol(const T* col, size_type size);

  template<typename U>
  friend std::ostream& operator<<(std::ostream& stream, const DataCsr<U>& mat);

 private:
  std::vector<int> rows_offset_;
  std::vector<size_type> cols_index_;
  std::vector<T> elems_;

  size_type size_rows_;
  size_type size_cols_;
};

}

#include <data_csr-inl.h>
