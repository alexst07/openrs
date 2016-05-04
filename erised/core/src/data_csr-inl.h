#ifndef ERISED_CSR_MATRIX_H_
#error "This should only be included by AtomicHashArray.h"
#endif

namespace erised {

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

  for (const auto& row: set) {
    size_type icol = 0;
    rows_offset_.push_back(i);
    for (const auto& v: row) {
      // insert only valid element
      if (v != 0) {
        elems_.push_back(v);
        cols_index_.push_back(icol);

        // Update the index for element
        i++;
      }
      icol++;

      // Gets the largest row, it means:
      // {{4 5 6}, {4 1 0 3 7}, {4 3 5 7}}
      // in the end of the process max_col will have 5, the longest
      // row elements of matrix
      if (icol > max_col)
        max_col = icol;
    }
    num_rows++;
  }

  size_cols_ = max_col;
  size_rows_ = num_rows;
}

template<typename T>
DataCsr<T>::DataCsr(size_type rows, size_type cols)
  : rows_offset_(rows + 1)
  , size_rows_(rows)
  , size_cols_(cols) {
}

template<typename T>
DataCsr<T>::DataCsr(const DataCsr<T>& m) {

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

//   stream << "rows vector:" << rows_offset_
//          << "\ncols index: " << cols_index_
//          << "\nelements: " << elems_ << "\n";
//
//   return stream;
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
DataCsr<T>::DataCsr(DataCsr<T>&& m) {

}

template<typename T>
void DataCsr<T>::ColMap(size_t i, MapFn fn) {

}

template<typename T>
T DataCsr<T>::ColReduce(size_t i, ReduceFn fn) {

}

template<typename T>
void DataCsr<T>::Map(MapFn fn) {

}

template<typename T>
T DataCsr<T>::Reduce(ReduceFn fn) {

}

template<typename T>
void DataCsr<T>::RowMap(size_t i, MapFn fn) {

}

template<typename T>
T DataCsr<T>::RowReduce(size_t i, ReduceFn fn) {

}

}
