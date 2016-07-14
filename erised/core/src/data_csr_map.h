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

template<typename T, typename Alloc = std::allocator<T>>
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
  using VectorValue = typename std::vector<T, Alloc>;
  using VectorSize = typename std::vector<size_t, Alloc>;
  typedef Alloc allocator_type;
  typedef T value_type;

  static const int INVALID_LINE;

  DataCsrMap(const allocator_type& a = allocator_type());

  DataCsrMap(size_type rows, size_type cols,
             const allocator_type& a = allocator_type());

  DataCsrMap(std::initializer_list<std::initializer_list<T>> set,
             const allocator_type& a = allocator_type());

  DataCsrMap(const DataCsrMap<T, Alloc>& m);
  DataCsrMap(DataCsrMap<T, Alloc>&& m);

  DataCsrMap<T, Alloc>& operator=(const DataCsrMap<T, Alloc>& m);
  DataCsrMap<T, Alloc>& operator=(DataCsrMap<T, Alloc>&& m);

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

  VectorSize NumElementsLines() const;

  VectorSize NumElementsCols() const;

  T Min(size_t i, Axis axis);

  T Max(size_t i, Axis axis);

  T Min(Axis axis);

  T Max(Axis axis);

  VectorValue MinElemsRows();

  VectorValue MinElemsCols();

  VectorValue MaxElemsRows();

  VectorValue MaxElemsCols();

  template<class Func>
  VectorValue Reduce(Axis axis, Func&& fn);

  template<class Func>
  VectorValue Map(Axis axis, Func&& fn);

  // This is the reduction function used for calculates similarities
  // because on similarities is able to calculates two terms in only
  // one interation, but to improve perfomance is better calculates
  // the reduction for all lines or collumns in each iteration
  template<class Func, size_t N>
  std::vector<std::array<T,N>> Reduce(Axis axis, Func&& fn, Func&& fnm);

  template<class Func, size_t N>
  std::array<T,N> Reduce(Axis axis, size_t i1, size_t i2, Func&& fn);

  template<typename U, typename _Alloc>
  friend std::ostream& operator<<(std::ostream& stream,
                                  const DataCsrMap<U, _Alloc>& mat);

 private:
  template<class Func>
  VectorValue ReduceCols(Func&& fn);

  template<class Func>
  VectorValue ReduceRows(Func&& fn);

  template<class Func>
  VectorValue MapCols(Func&& fn);

  template<class Func>
  VectorValue MapRows(Func&& fn);

  T MinElemRow(size_t i);

  T MinElemCol(size_t i);

  T MaxElemRow(size_t i);

  T MaxElemCol(size_t i);

  template<class Func, size_t N>
  std::array<T,N> ReduceRows(size_t i1, size_t i2, Func&& fn);

  template<class Func, size_t N>
  std::array<T,N> ReduceCols(size_t i1, size_t i2, Func&& fn);

  VecMap rows_;

  size_type size_rows_;
  size_type size_cols_;
};

}

#include "data_csr_map-inl.h"
