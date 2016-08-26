#pragma once
#define ERISED_CSR_MATRIX_MAP_H_

#include <initializer_list>
#include <vector>
#include <atomic>
#include <mutex>
#include <memory>
#include <functional>
#include <iostream>
#include <map>

#include "data_base.h"
#include "exception.h"
#include "parallel.h"

namespace erised {

namespace {
  template<class T>
  using iterator = typename std::vector<std::map<size_t, T>>::iterator;

  template<class T>
  using const_iterator =
      typename std::vector<std::map<size_t, T>>::const_iterator;
}

template<class T, class Alloc = std::allocator<T>>
class MatCsr: public MatRating<T, iterator<T>, const_iterator<T>> {
 public:
  using value_type = T;
  using size_type = size_t;
  using allocator_type = Alloc;
  using VecMap = typename std::vector<std::map<size_type, T>, Alloc>;
  using LineIter = typename VecMap::iterator;
  using ConstLineIter = typename VecMap::const_iterator;
  using ColIter = typename std::map<size_type, T>::iterator;
  using ConstColIter = typename std::map<size_type, T>::iterator;
  using VectorValue = typename std::vector<T, Alloc>;
  using VectorSize = std::vector<size_t>;
  constexpr static Axis main_iter = Axis::ROW;

  static const int INVALID_LINE;

  MatCsr(const allocator_type& a = allocator_type())
      : size_rows_(0)
      , size_cols_(0)
      , zero_(static_cast<value_type>(0)) {}

  MatCsr(size_type rows, size_type cols,
         const allocator_type& a = allocator_type())
      : rows_(rows)
      , size_rows_(rows)
      , size_cols_(cols)
      , zero_(static_cast<value_type>(0)) {}

  MatCsr(std::initializer_list<std::initializer_list<T>> set,
         const allocator_type& a = allocator_type()) {
    size_type i = 0;
    size_type max_col = 0;
    size_type num_rows = 0;

    // Scans each line
    for (const auto& row: set) {
      // Map with column index and elements
      std::map<size_type, T> map;
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

  MatCsr(const MatCsr<T, Alloc>& m)
      : rows_(m.rows_)
      , size_rows_(m.size_rows_)
      , size_cols_(m.size_cols_)
      , zero_(m.zero_) {}

  MatCsr(MatCsr<T, Alloc>&& m)
      : rows_(std::move(m.rows_))
      , size_rows_(m.size_rows_)
      , size_cols_(m.size_cols_)
      , zero_(m.zero_) {
    m.size_rows_ = 0;
    m.size_cols_ = 0;
  }

  MatCsr<T, Alloc>& operator=(const MatCsr<T, Alloc>& m) {
    // self-assignment check
    if (this != &m) {
      rows_ = m.rows_;
      size_cols_ = m.size_cols_;
      size_rows_ = m.size_rows_;
      zero_ = m.zero_;
    }

    return *this;
  }

  MatCsr<T, Alloc>& operator=(MatCsr<T, Alloc>&& m) {
    if (this != &m) {
      rows_ = std::move(m.rows_);
      size_cols_ = m.size_cols_;
      size_rows_ = m.size_rows_;
      zero_ = m.zero_;
    }

    return *this;
  }

  size_t SizeCols() const noexcept {
    return size_cols_;
  }

  size_t SizeRows() const noexcept {
    return size_rows_;
  }

  T& operator()(size_type x, size_type y) override {
    // Check if is a valid coordenate
    if (x > size_rows_)
      ERISED_Error(Error::OUT_OF_RANGE, "value of x is greater or equal than %d",
                  size_rows_ - 1);

    if (y > size_cols_)
      ERISED_Error(Error::OUT_OF_RANGE, "value of y is greater or equal than %d",
                  size_cols_ - 1);

    // Gets the line of vector
    auto map_row = rows_.at(x);

    // Find the col on the map and return it, if it
    // doesn't exist, return invalid value
    auto it = map_row.find(y);
    if (it != map_row.end())
      return it->second;

    return zero_;
  }

  const T& operator()(size_type x, size_type y) const noexcept override {
    return this->operator()(x, y);
  }

  void operator()(value_type v, size_type x, size_type y) override;

  iterator<value_type> begin() noexcept override {
    return rows_.begin();
  }

  const_iterator<value_type> begin() const noexcept override {
    return rows_.begin();
  }

  iterator<value_type> end() noexcept override {
    return rows_.end();
  }

  const_iterator<value_type> end() const noexcept override {
    return rows_.end();
  }

  size_t NumElements() const override {
    size_t num_elems = 0;

    // Sum the number of elements on map for each row
    for (const auto& r: rows_) {
      num_elems += r.size();
    }

    return num_elems;
  }

  size_t NumElementsLine(size_t i) const override {
    return rows_.at(i).size();
  }

  size_t NumElementsCol(size_t i) const override {
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

  VectorSize NumElements(Axis axis) const {
    if (axis == Axis::ROW)
      return std::move(NumElementsLines());

    return std::move(NumElementsCols());
  }

  VectorSize NumElementsLines() const {
    VectorSize rets(size_rows_);

    // Gets all lines
    Range<ConstLineIter> range(rows_.begin(), rows_.end());

    parallel_for(range, [&](const Range<ConstLineIter>& r) {
      for(auto i = r.begin(); i!=r.end(); ++i) {
        auto dist = std::distance(rows_.begin(), i);
        rets[dist] = i->size();
      }
    });

    return std::move(rets);
  }

  VectorSize NumElementsCols() const {
    VectorSize num_elems(size_cols_);
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

    return std::move(num_elems);
  }

  template<typename U, typename _Alloc>
  friend std::ostream& operator<<(std::ostream& stream,
                                  const MatCsr<U, _Alloc>& mat) {

  }

 private:

  VecMap rows_;

  size_type size_rows_;
  size_type size_cols_;

  value_type zero_;
};

template<typename U, typename _Alloc>
std::ostream& operator<<(std::ostream& stream,
                         const MatCsr<U, _Alloc>& mat) {
  stream << "rows vector: \n";
  for (const auto& r: mat.rows_) {
    for (const auto& m: r) {
      stream << m.first << " : " << m.second << '\n';
    }
    stream << "\n";
  }

  return stream;
}

}
