#pragma once

#ifndef __ERISED_MAP_REDUCE__
#define __ERISED_MAP_REDUCE__

#include "data_base.h"

#include <initializer_list>

namespace erised {

template<class Data>
class MapReduce {
 public:
  using VectorValue = typename Data::VectorValue;

  MapReduce(Data& data);

  MapReduce(const MapReduce&) = delete;
  MapReduce(MapReduce&&) = delete;

  MapReduce& operator=(const MapReduce&) = delete;
  MapReduce& operator=(MapReduce&&) = delete;

  template<class Func>
  ForEachRow(size_t i, Func&& fn);

  template<class Func>
  ForEachCol(size_t i, Func&& fn);

  template<class Func>
  ForEachRows(Func&& fn);

  template<class Func>
  ForEachCols(Func&& fn);

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
};

}
#endif //__ERISED_MAP_REDUCE__
