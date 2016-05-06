#pragma once
#define ERISED_CSR_MATRIX_MAP_H_

#include "data_base.h"

#include <initializer_list>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <unordered_map>

namespace erised {

template<typename T>
class DataCsrMap: public DataBase<T> {
 public:
  using size_type = std::size_t;
  using MapFn = std::function<T(T)>;
  using ReduceFn = std::function<T(T,T)>;
  using VecMap = typename std::vector<std::unordered_map<size_type, T>>;
  using ElemIter = typename VecMap::iterator;
  using ConstElemIter = typename VecMap::const_iterator;

  static const int INVALID_LINE;

  DataCsrMap();
  DataCsrMap(size_type rows, size_type cols);
  DataCsrMap(std::initializer_list<std::initializer_list<T>> set);
  DataCsrMap(const DataCsrMap<T>& m);
  DataCsrMap(DataCsrMap<T>&& m);

  DataCsrMap<T>& operator=(const DataCsrMap<T>& m);
  DataCsrMap<T>& operator=(DataCsrMap<T>&& m);

  void Map(const MapFn& fn) override;

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
  friend std::ostream& operator<<(std::ostream& stream, const DataCsrMap<U>& mat);

 private:
  VecMap rows_;

  size_type size_rows_;
  size_type size_cols_;
};

}

#include "data_csr_map-inl.h"
