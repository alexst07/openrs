#ifndef ERISED_CSR_MATRIX_MAP_H_
#error "This should only be included by data_csr_map.h"
#endif

#include <atomic>
#include <limits>

#include "parallel.h"

namespace erised {

// define const variables
template<typename T>
const int DataCsrMap<T>::INVALID_LINE = -1;

template<typename T>
DataCsrMap<T>::DataCsrMap()
  : size_rows_(0)
  , size_cols_(0) {
}

template<typename T>
DataCsrMap<T>::DataCsrMap(std::initializer_list<std::initializer_list<T>> set) {
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

template<typename T>
DataCsrMap<T>::DataCsrMap(size_type rows, size_type cols)
  : size_rows_(rows)
  , size_cols_(cols) {
}

template<typename T>
DataCsrMap<T>::DataCsrMap(const DataCsrMap<T>& m)
  : rows_(m.rows_) {
}

template<typename T>
DataCsrMap<T>::DataCsrMap(DataCsrMap<T>&& m)
  : rows_(std::move(m.rows_)) {
}

template<typename T>
DataCsrMap<T>& DataCsrMap<T>::operator=(const DataCsrMap<T>& m) {
  // self-assignment check
  if (this != &m) {
    rows_ = m.rows_;
  }

  return *this;
}

template<typename T>
DataCsrMap<T>& DataCsrMap<T>::operator=(DataCsrMap<T>&& m) {
  rows_ = std::move(m.rows_);

  return *this;
}

template<typename U>
std::ostream& operator<<(std::ostream& stream, const DataCsrMap<U>& mat) {
  stream << "rows vector: ";
  for (const auto& r: mat.rows_) {
    for (const auto& m: r) {
      stream << m.first << " : " << m.second << '\n';
    }
    stream << "\n";
  }

  return stream;
}

template<typename T>
T DataCsrMap<T>::operator()(const Pos<DataBase<T>::order>& pos) const {
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

template<typename T>
T DataCsrMap<T>::operator()(size_type x, size_type y) const {
  return this->operator()({x, y});
}

template<typename T>
size_t DataCsrMap<T>::NumElements() const {
  size_t num_elems = 0;

  // Sum the number of elements on map for each row
  for (const auto& r: rows_) {
    num_elems += r.size();
  }

  return num_elems;
}

template<typename T>
size_t DataCsrMap<T>::NumElementsLine(size_t i) const {
  return rows_.at(i).size();
}

template<typename T>
size_t DataCsrMap<T>::NumElementsCol(size_t i) const {
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

template<typename T>
void DataCsrMap<T>::ColMap(size_t i, MapFn fn) {
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

template<typename T>
T DataCsrMap<T>::ColReduce(size_t i, const ReduceFn& fn) const {
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

template<typename T>
void DataCsrMap<T>::Map(const MapFn& fn) {
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

template<typename T>
T DataCsrMap<T>::Reduce(const ReduceFn& fn) const {
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

template<typename T>
void DataCsrMap<T>::RowMap(size_t i, MapFn fn) {
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

template<typename T>
T DataCsrMap<T>::RowReduce(size_t i, const ReduceFn& fn) const {
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

template<typename T>
T DataCsrMap<T>::Min(size_t i, Axis axis) {
  if (axis == Axis::ROW) {
    return MinElemRow(i);
  } else {
    return MinElemCol(i);
  }
}

template<typename T>
T DataCsrMap<T>::MinElemRow(size_t i) {
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

template<typename T>
T DataCsrMap<T>::MinElemCol(size_t i) {
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

}
