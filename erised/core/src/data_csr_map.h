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
  using VecMap = typename std::vector<std::unordered_map<size_type, T>, Alloc>;
  using LineIter = typename VecMap::iterator;
  using ConstLineIter = typename VecMap::const_iterator;
  using ColIter = typename std::unordered_map<size_type, T>::iterator;
  using ConstColIter = typename std::unordered_map<size_type, T>::iterator;
  using VectorValue = typename std::vector<T, Alloc>;
  using VectorSize = std::vector<size_t>;
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

  size_t SizeCols() const noexcept;

  size_t SizeRows() const noexcept;

  template<class Func>
  void Map(Func&& fn);

  template<class Func>
  void RowMap(size_t i, Func&& fn);

  template<class Func>
  void ColMap(size_t i, Func&& fn);

  template<class Func>
  T Reduce(Func&& fn) const;

  template<class Func>
  T RowReduce(size_t i, Func&& fn) const;

  template<class Func>
  T ColReduce(size_t i, Func&& fn) const;

  T& operator()(size_type x, size_type y) override;

  const T& operator()(size_type x, size_type y) const override;

  size_t NumElements() const override;

  size_t NumElementsLine(size_t i) const override;

  size_t NumElementsCol(size_t i) const override;

  VectorSize NumElements(Axis axis) const;

  VectorSize NumElementsLines() const;

  VectorSize NumElementsCols() const;

  T Min(size_t i, Axis axis) const;

  T Max(size_t i, Axis axis) const;

  VectorValue Min(Axis axis) const;

  VectorValue Max(Axis axis) const;

  VectorValue MinElemsRows() const;

  VectorValue MinElemsCols() const;

  VectorValue MaxElemsRows() const;

  VectorValue MaxElemsCols() const;

  template<class Func>
  VectorValue Reduce(Axis axis, Func&& fn) const;

  template<class Func>
  void Map(Axis axis, Func&& fn);

  template<class Func>
  void MapCols(Func&& fn);

  template<class Func>
  void MapRows(Func&& fn);

  template<class Func>
  VectorValue ReduceCols(Func&& fn) const;

  template<class Func>
  VectorValue ReduceRows(Func&& fn) const;

  template<size_t N, class Func>
  std::array<T,N> Reduce(Axis axis, size_t i1, size_t i2, Func&& fn) const;

  template<size_t N, class Func>
  std::array<T,N> ReduceRows(size_t i1, size_t i2, Func&& fn) const;

  template<size_t N, class Func>
  std::array<T,N> ReduceCols(size_t i1, size_t i2, Func&& fn) const;

  template<typename U, typename _Alloc>
  friend std::ostream& operator<<(std::ostream& stream,
                                  const DataCsrMap<U, _Alloc>& mat);

 private:
  T MinElemRow(size_t i) const;

  T MinElemCol(size_t i) const;

  T MaxElemRow(size_t i) const;

  T MaxElemCol(size_t i) const;

  VecMap rows_;

  size_type size_rows_;
  size_type size_cols_;

  value_type zero_;
};

}

#include "data_csr_map-inl.h"
