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
  using LineIter = typename VecMap::iterator;
  using ConstLineIter = typename VecMap::const_iterator;
  using ColIter = typename std::unordered_map<size_type, T>::iterator;
  using ConstColIter = typename std::unordered_map<size_type, T>::iterator;

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

  T Reduce(const ReduceFn& fn) const override;

  T RowReduce(size_t i, const ReduceFn& fn) const override;

  T ColReduce(size_t i, const ReduceFn& fn) const override;

  T operator()(const Pos<DataBase<T>::order>& pos) const override;

  T operator()(size_type x, size_type y) const;

  size_t NumElements() const override;

  size_t NumElementsLine(size_t i) const override;

  size_t NumElementsCol(size_t i) const override;

  std::vector<size_t> NumElementsLines() const;

  std::vector<size_t> NumElementsCols() const;

  T Min(size_t i, Axis axis);

  T Max(size_t i, Axis axis);

  T Min(Axis axis);

  T Max(Axis axis);

  template<class Func>
  std::vector<T> Reduce(Axis axis, Func&& fn);

  template<class Func>
  std::vector<T> Map(Axis axis, Func&& fn);

  template<typename U>
  friend std::ostream& operator<<(std::ostream& stream, const DataCsrMap<U>& mat);

 private:
  template<class Func>
  std::vector<T> ReduceCols(Func&& fn);

  template<class Func>
  std::vector<T> ReduceRows(Func&& fn);

  template<class Func>
  std::vector<T> MapCols(Func&& fn);

  template<class Func>
  std::vector<T> MapRows(Func&& fn);

  T MinElemRow(size_t i);

  T MinElemCol(size_t i);

  T MaxElemRow(size_t i);

  T MaxElemCol(size_t i);

  std::vector<T> MinElemsRows();

  std::vector<T> MinElemsCols();

  std::vector<T> MaxElemsRows();

  std::vector<T> MaxElemsCols();

  VecMap rows_;

  size_type size_rows_;
  size_type size_cols_;
};

}

#include "data_csr_map-inl.h"
