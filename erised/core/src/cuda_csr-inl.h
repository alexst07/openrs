#ifndef ERISED_CSR_MATRIX_H_
#error "This should only be included by data_csr.h"
#endif

#include "parallel.h"

namespace erised {

  // define const variables
  template<typename T>
  const int DataCsr<T>::INVALID_LINE = -1;

  template<typename T>
  DataCsr<T>::DataCsr()
  : size_rows_(0)
  , size_cols_(0) {
  }

  template<typename T>
  DataCsr<T>::DataCsr(std::initializer_list<std::initializer_list<T>> set) {
    size_type i = 0;
    size_type max_col = 0;
    size_type num_rows = 0;

    // Scans each line
    for (const auto& row: set) {
      size_type icol = 0;
      size_type i_prev = i;

      // Inserts the index for the next valid element
      rows_offset_.push_back(i);

      // Scans each element on line
      for (const auto& v: row) {
        // Inserts only valid element
        if (v != 0) {
          elems_.push_back(v);

          // Inserts index of the column for the valid element
          cols_index_.push_back(icol);

          // Updates the index for element
          i++;
        }

        // keeps the synchronization of column in which the element is
        icol++;

        // Gets the largest row, it means:
        // {{4 5 6}, {4 1 0 3 7}, {4 3 5 7}}
        // in the end of the process max_col will have 5, the longest
        // row elements of matrix
        if (icol > max_col)
          max_col = icol;
      }

      // If there is no valid value on line, assigns -1 as offset of this line
      if (i == i_prev)
        rows_offset_.back() = INVALID_LINE;

      // Count each line
      num_rows++;
    }

    // Assigns the max_col as the numbers of column for the matrix
    size_cols_ = max_col;

    // Assigns the number of rows
    size_rows_ = num_rows;
  }

  template<typename T>
  DataCsr<T>::DataCsr(size_type rows, size_type cols)
  : rows_offset_(rows + 1)
  , size_rows_(rows)
  , size_cols_(cols) {
  }

  template<typename T>
  GpuCsr<T>::GpuCsr(const GpuCsr<T>& m)
  : rows_offset_(m.rows_offset_)
  , cols_index_(m.cols_index_)
  , elems_(m.elems_) {
  }

  template<typename T>
  DataCsr<T>::DataCsr(DataCsr<T>&& m)
  : rows_offset_(std::move(m.rows_offset_))
  , cols_index_(std::move(m.cols_index_))
  , elems_(std::move(m.elems_)) {
  }

  template<typename T>
  DataCsr<T>& DataCsr<T>::operator=(const DataCsr<T>& m) {
    // self-assignment check
    if (this != &m) {
      rows_offset_ = m.rows_offset_;
      cols_index_ = m.cols_index_;
      elems_ = m.elems_;
    }

    return *this;
  }

  template<typename T>
  DataCsr<T>& DataCsr<T>::operator=(DataCsr<T>&& m) {
    rows_offset_ = std::move(m.rows_offset_);
    cols_index_ = std::move(m.cols_index_);
    elems_ = std::move(m.elems_);

    return *this;
  }

  template<typename U>
  std::ostream& operator<<(std::ostream& stream, const DataCsr<U>& mat) {
    stream << "rows vector: ";
    for (const auto& r: mat.rows_offset_) {
      stream << r << " ";
    }
    stream << "\n";

    stream << "cols index: ";
    for (const auto& i: mat.cols_index_) {
      stream << i << " ";
    }
    stream << "\n";

    stream << "elems: ";
    for (const auto& e: mat.elems_) {
      stream << e << " ";
    }
    stream << "\n";
  }

  template<typename T>
  void DataCsr<T>::AddCol(const T* col, size_type size) {

  }

  template<typename T>
  void DataCsr<T>::AddCol(const std::vector<T>& col) {

  }

  template<typename T>
  void DataCsr<T>::AddRow(const T* row, size_type size) {

  }

  template<typename T>
  void DataCsr<T>::AddRow(const std::vector<T>& row) {

  }

  template<typename T>
  void DataCsr<T>::ColMap(size_t i, MapFn fn) {

  }

  template<typename T>
  T DataCsr<T>::ColReduce(size_t i, const ReduceFn& fn) {

  }

  template<typename T>
  void DataCsr<T>::Map(const MapFn& fn) {
    // Gets all elements
    Range<ElemIter> range(elems_.begin(), elems_.end());

    // Executes the function fn on all elments
    parallel_for(range, [&](Range<ElemIter>& r){
      for(auto i = r.begin(); i!=r.end(); ++i)
        *i = fn(*i);
    });
  }

  template<typename T>
  T DataCsr<T>::Reduce(const ReduceFn& fn) {

  }

  template<typename T>
  void DataCsr<T>::RowMap(size_t i, MapFn fn) {
    // Verifies if the line has some valid element
    if (rows_offset_[i] == INVALID_LINE)
      return;

    // Calculates the start of the line on elments
    auto start = elems_.begin() + rows_offset_[i];

    // Calculates the end of the line on elments
    auto end = elems_.begin() + rows_offset_[i + 1];

    Range<ElemIter> range(start , end);

    // Executes the function fn on elments from line i
    parallel_for(range, [&](Range<ElemIter>& r){
      for(auto i = r.begin(); i!=r.end(); ++i)
        *i = fn(*i);
    });
  }

  template<typename T>
  T DataCsr<T>::RowReduce(size_t i, const ReduceFn& fn) {

  }

}
