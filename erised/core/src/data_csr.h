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

  using MapFn = T(&&)(T);
  using ReduceFn = T(&&)(T, T);

  DataCsr();
  DataCsr(size_type rows, size_type cols);
  DataCsr(std::initializer_list<std::initializer_list<T>> set);
  DataCsr(const DataCsr<T>& m);
  DataCsr(DataCsr<T>&& m);

  void Map(MapFn fn) override;

  void RowMap(size_t i, MapFn fn) override;

  void ColMap(size_t i, MapFn fn) override;

  T Reduce(ReduceFn fn) override;

  T RowReduce(size_t i, ReduceFn fn) override;

  T ColReduce(size_t i, ReduceFn fn) override;

  void AddRow(const std::vector<T>& row);
  void AddRow(const T* row, size_type size);

  void AddCol(const std::vector<T>& col);
  void AddCol(const T* col, size_type size);

  template<typename U>
  friend std::ostream& operator<<(std::ostream& stream, const DataCsr<U>& mat);

 private:
  std::vector<size_type> rows_offset_;
  std::vector<size_type> cols_index_;
  std::vector<T> elems_;

  size_type size_rows_;
  size_type size_cols_;
};

}

#include <data_csr-inl.h>
