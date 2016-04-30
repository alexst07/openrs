#include "data_csr.h"

namespace erised {

DataCsr::DataCsr()
  : size_rows_(0)
  , size_cols_(0) {
}

DataCsr::DataCsr(std::initializer_list<std::initializer_list< T >> set) {
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

DataCsr::DataCsr(size_type rows, size_type cols)
  : rows_offset_(rows + 1)
  , size_rows_(rows)
  , size_cols_(cols) {
}

DataCsr::DataCsr(const CsrMat<T>& m) {

}

std::ostream& DataCsr::operator<<(std::ostream& stream, const DataCsr& m) {
  stream << "rows vector:" << rows_offset_
         << "\ncols index: " << cols_index_
         << "\nelements: " << elems_ << "\n";

  return stream;
}

}
