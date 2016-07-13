#ifndef ERISED_CSR_MATRIX_MAP_H_
#error "This should only be included by data_csr_map.h"
#endif

#include <atomic>
#include <limits>
#include <mutex>

#include "data_csr_map.h"
#include "parallel.h"

namespace erised {

// define const variables
template<typename T, typename Alloc>
const int DataCsrMap<T, Alloc>::INVALID_LINE = -1;

template<typename T, typename Alloc>
DataCsrMap<T, Alloc>::DataCsrMap(const allocator_type& a)
  : size_rows_(0)
  , size_cols_(0) {
}

template<typename T, typename Alloc>
DataCsrMap<T, Alloc>::DataCsrMap(
    std::initializer_list<std::initializer_list<T>> set,
    const allocator_type& a) {
  size_type i = 0;
  size_type max_col = 0;
  size_type num_rows = 0;

  // Scans each line
  for (const auto& row: set) {
    // Map with column index and elements
    std::unordered_map<size_type, T> map;
    size_type icol = 0;
    size_type i_prev = i;
    size_type row_size = row.size();

    // Assigns the larger line as the max column numbers elements
    if (max_col < row_size)
      max_col = row_size;

    // Scans each element on line
    for (const auto& v: row) {
      // Inserts only valid elements
      if (v != 0) {
        map.insert(std::pair<size_type, T>(icol, v));
      }

      icol++;
    }

    // Insert the map with column index and elments
    rows_.push_back(std::move(map));

    // Count each line
    num_rows++;
  }

  // Assigns the max_col as the numbers of column for the matrix
  size_cols_ = max_col;

  // Assigns the number of rows
  size_rows_ = set.size();
}

template<typename T, typename Alloc>
DataCsrMap<T, Alloc>::DataCsrMap(size_type rows, size_type cols,
                                 const allocator_type& a)
  : size_rows_(rows)
  , size_cols_(cols) {
}

template<typename T, typename Alloc>
DataCsrMap<T, Alloc>::DataCsrMap(const DataCsrMap<T, Alloc>& m)
  : rows_(m.rows_) {
}

template<typename T, typename Alloc>
DataCsrMap<T, Alloc>::DataCsrMap(DataCsrMap<T, Alloc>&& m)
  : rows_(std::move(m.rows_)) {
}

template<typename T, typename Alloc>
DataCsrMap<T, Alloc>& DataCsrMap<T, Alloc>::operator=(
    const DataCsrMap<T, Alloc>& m) {
  // self-assignment check
  if (this != &m) {
    rows_ = m.rows_;
  }

  return *this;
}

template<typename T, typename Alloc>
DataCsrMap<T, Alloc>& DataCsrMap<T, Alloc>::operator=(
    DataCsrMap<T, Alloc>&& m) {
  rows_ = std::move(m.rows_);

  return *this;
}

template<typename U, typename _Alloc>
std::ostream& operator<<(std::ostream& stream,
                         const DataCsrMap<U, _Alloc>& mat) {
  stream << "rows vector: ";
  for (const auto& r: mat.rows_) {
    for (const auto& m: r) {
      stream << m.first << " : " << m.second << '\n';
    }
    stream << "\n";
  }

  return stream;
}

template<typename T, typename Alloc>
T DataCsrMap<T, Alloc>::operator()(const Pos<DataBase<T>::order>& pos) const {
  size_t x = pos.X();
  size_t y = pos.Y();

  std::cout << "x: " << x << "y: " << y << "\n";

  // Check if is a valid coordenate
  if (x > size_rows_)
    throw std::out_of_range("row x is greater or equal than its x size");

  if (y > size_cols_)
    throw std::out_of_range("col y is greater or equal than its y size");

  // Gets the line of vector
  auto map_row = rows_.at(x);

  // Find the col on the map and return it, if it
  // doesn't exist, return invalid value
  auto it = map_row.find(y);
  if (it != map_row.end())
    return it->second;

  return 0;
}

template<typename T, typename Alloc>
T DataCsrMap<T, Alloc>::operator()(size_type x, size_type y) const {
  return this->operator()({x, y});
}

template<typename T, typename Alloc>
size_t DataCsrMap<T, Alloc>::NumElements() const {
  size_t num_elems = 0;

  // Sum the number of elements on map for each row
  for (const auto& r: rows_) {
    num_elems += r.size();
  }

  return num_elems;
}

template<typename T, typename Alloc>
size_t DataCsrMap<T, Alloc>::NumElementsLine(size_t i) const {
  return rows_.at(i).size();
}

template<typename T, typename Alloc>
std::vector<size_t>  DataCsrMap<T, Alloc>::NumElementsLines() const {
  std::vector<size_t> rets(size_rows_);
  // Gets all lines
  Range<LineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments
  return parallel_for(range, [&]( const Range<LineIter>& r) {
    // Scan only one line, because unordered_map::iterator doesn't
    // have any to use compare operator as > or < so, TBB parallel
    // doesn't work correct for unordered_map, and this work arount
    // is used to guarantee the correct compilation
    for(auto i = r.begin(); i!=r.end(); ++i) {
      auto dist = std::distance(rows_.begin(), i);
      rets[dist] = i->size();
    }
  });
  return rets;
}

template<typename T, typename Alloc>
size_t DataCsrMap<T, Alloc>::NumElementsCol(size_t i) const {
  std::atomic<size_t> num_elems(0);

  // Gets all lines
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Iterates over a column, and count the elements
  parallel_for(range, [&](const Range<ConstLineIter>& r){
    // Scan each line and search for specific column
    for(auto row = r.begin(); row != r.end(); ++row) {
      auto e = row->find(i);

      // Verify if column exists
      if (e != row->end())
        num_elems++;
    }
  });

  return num_elems;
}

template<typename T, typename Alloc>
std::vector<size_t>  DataCsrMap<T, Alloc>::NumElementsCols() const {
  std::vector<size_t> num_elems(size_cols_);
  std::vector<std::mutex> mtxv(size_cols_);

  // Gets all lines
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Iterates over a column, and count the elements
  parallel_for(range, [&](const Range<ConstLineIter>& r){
    // Scan each line and search for specific column
    for(auto row = r.begin(); row != r.end(); ++row) {
      for (size_t i = 0; i < size_cols_; i++) {
        auto e = row->find(i);

        // Verify if column exists
        if (e != row->end()) {
          mtxv[i].lock();
          num_elems[i]++;
          mtxv[i].unlock();
        }
      }
    }
  });

  return num_elems;
}

template<typename T, typename Alloc>
void DataCsrMap<T, Alloc>::ColMap(size_t i, MapFn fn) {
  // Gets all lines
  Range<LineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments from specific column
  parallel_for(range, [&](const Range<LineIter>& r){
    // Scan each line and search for specific column
    for(auto row = r.begin(); row != r.end(); ++row) {
      auto got = row->find(i);

      // Verify if column exists
      if (got != row->end())
        got->second = fn(got->second);
    }
  });
}

template<typename T, typename Alloc>
T DataCsrMap<T, Alloc>::ColReduce(size_t i, const ReduceFn& fn) const {
  // Gets all lines
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments
  return parallel_reduce(range, static_cast<T>(0),
      [&](const Range<ConstLineIter>& r, T value) -> T {
        T ret = value;

        // Scan each line
        for(auto row = r.begin(); row != r.end(); ++row) {
          auto got = row->find(i);

          // Verify if column exists
          if (got != row->end()) {
            ret = fn(got->second, ret);
          }
        }

        return ret;
  }, [&fn](T a, T b) -> T {
    return fn(a, b);
  });
}

template<typename T, typename Alloc>
void DataCsrMap<T, Alloc>::Map(const MapFn& fn) {
  // Gets all elements
  Range<LineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments
  parallel_for(range, [&](const Range<LineIter>& r) {
    // Scan each line
    for(auto i = r.begin(); i!=r.end(); ++i)
      // Scan element by element from the line
      for(const auto& e: *i)
        i->operator[](e.first) = fn(e.second);
  });
}

template<typename T, typename Alloc>
T DataCsrMap<T, Alloc>::Reduce(const ReduceFn& fn) const {
  // Gets all elements
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments
  return parallel_reduce(range, static_cast<T>(0),
      [&](const Range<ConstLineIter>& r, T value) -> T {
        T ret = value;
        // Scan each line
        for(auto i = r.begin(); i!=r.end(); ++i)
          // Scan element by element from the line
          for(const auto& e: *i)
            ret = fn(e.second, ret);

        return ret;
  }, [&fn](T a, T b) -> T {
    return fn(a, b);
  });
}

template<typename T, typename Alloc>
void DataCsrMap<T, Alloc>::RowMap(size_t i, MapFn fn) {
  auto row_ref = rows_.begin() + i;
  // Gets all elements from line row_ref
  Range<LineIter> range(row_ref, row_ref + 1);

  // Executes the function fn on all elments
  return parallel_for(range, [&]( const Range<LineIter>& r) {
    // Scan only one line, because unordered_map::iterator doesn't
    // have any to use compare operator as > or < so, TBB parallel
    // doesn't work correct for unordered_map, and this work arount
    // is used to guarantee the correct compilation
    for(auto i = r.begin(); i!=r.end(); ++i)
      // Scan element by element from the line
      for(const auto& e: *i)
        i->operator[](e.first) = fn(e.second);
  });
}

template<typename T, typename Alloc>
T DataCsrMap<T, Alloc>::RowReduce(size_t i, const ReduceFn& fn) const {
  auto row_ref = rows_.begin() + i;
  // Gets all elements from line row_ref
  Range<ConstLineIter> range(row_ref, row_ref + 1);

  // Executes the function fn on all elments
  parallel_reduce(range, static_cast<T>(0),
      [&](const Range<ConstLineIter>& r, T value) -> T {
        T ret = value;
        // Scan each line
        for(auto i = r.begin(); i!=r.end(); ++i)
          // Scan element by element from the line
          for(const auto& e: *i)
            ret = fn(e.second, ret);

          return ret;
  }, [&fn](T a, T b) -> T {
    return fn(a, b);
  });
}

template<typename T, typename Alloc>
T DataCsrMap<T, Alloc>::Min(size_t i, Axis axis) {
  if (axis == Axis::ROW) {
    return MinElemRow(i);
  } else {
    return MinElemCol(i);
  }
}

template<typename T, typename Alloc>
T DataCsrMap<T, Alloc>::MinElemRow(size_t i) {
  T min = std::numeric_limits<T>::max();

  // Gets the map pointed by the row_ref
  auto row = rows_.at(i);

  // Iterate over the map
  for (const auto& e: row) {
    // Compare to find the lowest
    // element on the line
    if (e.second < min) {
      min = e.second;
    }
  }

  return min;
}

template<typename T, typename Alloc>
T DataCsrMap<T, Alloc>::MinElemCol(size_t i) {
  std::atomic<T> min(std::numeric_limits<T>::max());

  // Gets all lines
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Iterates over a column, and count the elements
  parallel_for(range, [&](const Range<ConstLineIter>& r){
    // Scan each line and search for specific column
    for(const auto &row : r) {
      auto e = row.find(i);

      // Verify if column exists
      if (e != row.end()) {
        // Compare to find the lowest
        // element on the col
        if (e->second < min) {
          min = e->second;
        }
      }
    }
  });

  return min;
}

template<typename T, typename Alloc>
T DataCsrMap<T, Alloc>::Max(size_t i, Axis axis) {
  if (axis == Axis::ROW) {
    return MaxElemRow(i);
  } else {
    return MaxElemCol(i);
  }
}

template<typename T, typename Alloc>
T DataCsrMap<T, Alloc>::MaxElemRow(size_t i) {
  T max = std::numeric_limits<T>::min();

  // Gets the map pointed by the row_ref
  auto row = rows_.at(i);

  // Iterate over the map
  for (const auto& e: row) {
    // Compare to find the lowest
    // element on the line
    if (e.second > max) {
      max = e.second;
    }
  }

  return max;
}

template<typename T, typename Alloc>
T DataCsrMap<T, Alloc>::MaxElemCol(size_t i) {
  std::atomic<T> max(std::numeric_limits<T>::min());

  // Gets all lines
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Iterates over a column, and count the elements
  parallel_for(range, [&](const Range<ConstLineIter>& r){
    // Scan each line and search for specific column
    for(const auto &row : r) {
      auto e = row.find(i);

      // Verify if column exists
      if (e != row.end()) {
        // Compare to find the lowest
        // element on the col
        if (e->second > max) {
          max = e->second;
        }
      }
    }
  });

  return max;
}

template<typename T, typename Alloc>
std::vector<T> DataCsrMap<T, Alloc>::MinElemsRows() {
  std::vector<T> min_elems(size_rows_);

  // Gets all elements
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments
  parallel_for(range, [&](const Range<ConstLineIter>& r) {
    // Scan each line
    for(auto i = r.begin(); i!=r.end(); ++i) {
        auto dist = std::distance(rows_.begin(), i);
        min_elems[dist] = MinElemRow(dist);
    }
  });

  return std::move(min_elems);
}

template<typename T, typename Alloc>
std::vector<T> DataCsrMap<T, Alloc>::MinElemsCols() {
  std::vector<T> min_elems(size_cols_, std::numeric_limits<T>::max());
  std::vector<std::mutex> mtxv(size_cols_);

  // Gets all lines
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Iterates over a column, and count the elements
  parallel_for(range, [&](const Range<ConstLineIter>& r){
    // Scan each line and search for specific column
    for (const auto &row : r) {
      for (size_t i = 0; i < size_cols_; i++) {
        auto e = row.find(i);

        // Verify if column exists
        if (e != row.end()) {
          auto dist = std::distance(rows_.begin(), row);
          mtxv[e->first].lock();
          if (e->second < min_elems[e->first]) {
            min_elems[e->first] = e->second;
          }
          mtxv[e->first].unlock();
        }
      }
    }
  });

  return std::move(min_elems);
}

template<typename T, typename Alloc>
std::vector<T> DataCsrMap<T, Alloc>::MaxElemsRows() {
  std::vector<T> min_elems(size_rows_);

  // Gets all elements
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments
  parallel_for(range, [&](const Range<ConstLineIter>& r) {
    // Scan each line
    for(auto i = r.begin(); i!=r.end(); ++i) {
        auto dist = std::distance(rows_.begin(), i);
        min_elems[dist] = MaxElemRow(dist);
    }
  });

  return std::move(min_elems);
}

template<typename T, typename Alloc>
std::vector<T> DataCsrMap<T, Alloc>::MaxElemsCols() {
  std::vector<T> max_elems(size_cols_, std::numeric_limits<T>::min());
  std::vector<std::mutex> mtxv(size_cols_);

  // Gets all lines
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Iterates over a column, and count the elements
  parallel_for(range, [&](const Range<ConstLineIter>& r){
    // Scan each line and search for specific column
    for (const auto &row : r) {
      for (size_t i = 0; i < size_cols_; i++) {
        auto e = row.find(i);

        // Verify if column exists
        if (e != row.end()) {
          auto dist = std::distance(rows_.begin(), row);
          mtxv[e->first].lock();
          if (e->second > max_elems[e->first]) {
            max_elems[e->first] = e->second;
          }
          mtxv[e->first].unlock();
        }
      }
    }
  });

  return std::move(max_elems);
}

template<typename T, typename Alloc>
template<class Func>
std::vector<T> DataCsrMap<T, Alloc>::ReduceCols(Func&& fn) {
  std::vector<T> rets(size_cols_);
  std::vector<std::mutex> mtxv(size_cols_);

  // Gets all elements
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments
  parallel_for(range, [&](const Range<ConstLineIter>& r) {
    // Scan each line
    for(auto i = r.begin(); i!=r.end(); ++i)
      // Scan element by element from the line
      for(const auto& e: *i) {
        mtxv[e.first].lock();
        rets[e.first] = fn(e.first, e.second, rets[i]);
        mtxv[e.first].unlock();
      }
  });

  return std::move(rets);
}

template<typename T, typename Alloc>
template<class Func>
std::vector<T> DataCsrMap<T, Alloc>::ReduceRows(Func&& fn) {
  std::vector<T> rets(size_rows_);

  // Gets all elements
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments
  parallel_for(range, [&](const Range<ConstLineIter>& r) {
    // Scan each line
    for(auto i = r.begin(); i!=r.end(); ++i)
      // Scan element by element from the line
      for(const auto& e: *i) {
        auto dist = std::distance(rows_.begin(), r);
        rets[i] = fn(dist, e.second, rets[i]);
      }
  });

  return std::move(rets);
}

template<typename T, typename Alloc>
template<class Func>
std::vector<T> DataCsrMap<T, Alloc>::Reduce(Axis axis, Func&& fn) {
  if (axis == Axis::ROW) {
    return std::move(ReduceRows(fn));
  } else {
    return std::move(ReduceCols(fn));
  }
}

template<typename T, typename Alloc>
template<class Func>
std::vector<T> DataCsrMap<T, Alloc>::MapCols(Func&& fn) {
  // Gets all lines
  Range<LineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments from specific column
  parallel_for(range, [&](const Range<LineIter>& r){
    // Scan each line and search for specific column
    for (auto row = r.begin(); row != r.end(); ++row) {
      for (size_t i = 0; i < size_cols_; i++) {
        auto got = row->find(i);

        // Verify if column exists
        if (got != row->end())
          got->second = fn(i, got->second);
      }
    }
  });
}

template<typename T, typename Alloc>
template<class Func>
std::vector<T> DataCsrMap<T, Alloc>::MapRows(Func&& fn) {
  // Gets all lines
  Range<LineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments
  return parallel_for(range, [&]( const Range<LineIter>& r) {
    // Scan only one line, because unordered_map::iterator doesn't
    // have any to use compare operator as > or < so, TBB parallel
    // doesn't work correct for unordered_map, and this work arount
    // is used to guarantee the correct compilation
    for(auto i = r.begin(); i!=r.end(); ++i)
      // Scan element by element from the line
      for(const auto& e: *i)
        i->operator[](e.first) = fn(std::distance(rows_.begin(), r), e.second);
  });
}

template<typename T, typename Alloc>
template<class Func>
std::vector<T> DataCsrMap<T, Alloc>::Map(Axis axis, Func&& fn) {
  if (axis == Axis::ROW) {
    MapRows(fn);
  } else {
    MapCols(fn);
  }
}

template<typename T, typename Alloc>
template<class Func, size_t N>
std::array<T,N> DataCsrMap<T, Alloc>::ReduceRows(size_t i1, size_t i2,
                                                 Func&& fn) {
  Range<size_t> range(0, size_cols_);

  auto row1 = rows_.at(i1);
  auto row2 = rows_.at(i2);

  // Executes the function fn on all elments
  parallel_reduce(range, static_cast<T>(0),
      [&](const Range<size_t>& r, std::array<T,N> value) -> T {
        std::array<T,N> rets = value;
        // Scan each line
        for(auto i = r.begin(); i!=r.end(); ++i) {
          auto it1 = row1->find(i);
          auto it2 = row2->find(i);

          // Verify if column exists
          if ((it1 != it1->end()) && (it2 != it2->end()))
            rets = fn(it1->second, it2->second, rets);
        }

        return rets;
  }, [](std::array<T,N> a, std::array<T,N> b) -> T {
    std::array<T,N> acc;
    for (int i = 0; i < N; i++) {
      acc[i] = a[i] + b[i];
    }
    return acc;
  });
}

template<typename T, typename Alloc>
template<class Func, size_t N>
std::array<T,N> DataCsrMap<T, Alloc>::ReduceCols(size_t i1, size_t i2,
                                                 Func&& fn) {
  // Gets all elements from line row_ref
  Range<ConstLineIter> range(rows_.begin(), rows_.end());

  // Executes the function fn on all elments
  parallel_reduce(range, static_cast<T>(0),
      [&](const Range<ConstLineIter>& r, std::array<T,N> value) -> T {
        std::array<T,N> rets = value;
        // Scan each line
        for(auto i = r.begin(); i!=r.end(); ++i) {
          auto e1 = r->find(i1);
          auto e2 = r->find(i2);

           // Verify if column exists
          if ((e1 != e1->end()) && (e2 != e2->end()))
            rets = fn(e1->second, e2->second, rets);
        }

          return rets;
  }, [](std::array<T,N> a, std::array<T,N> b) -> T {
    std::array<T,N> acc;
    for (int i = 0; i < N; i++) {
      acc[i] = a[i] + b[i];
    }
    return acc;
  });
}

template<typename T, typename Alloc>
template<class Func, size_t N>
std::array<T,N> DataCsrMap<T, Alloc>::Reduce(Axis axis, size_t i1, size_t i2,
                                             Func&& fn) {
  if (axis == Axis::ROW) {
    ReduceRows(i1, i2, fn);
  } else {
    ReduceCols(i1, i2, fn);
  }
}

}
