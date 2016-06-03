#ifndef ERISED_CUDA_CSR_MATRIX_H_
#error "This should only be included by data_csr.h"
#endif

namespace erised { namespace cuda {

  // define const variables
  template<typename T>
  const int GpuCsr<T>::INVALID_LINE = -1;

  template<typename T>
  GpuCsr<T>::GpuCsr()
  : size_rows_(0)
  , size_cols_(0) {
  }

  template<typename T>
  GpuCsr<T>::GpuCsr(std::initializer_list<std::initializer_list<T>> set) {
    std::vector<int> rows_offset;
    std::vector<size_type> cols_index;
    std::vector<T> elems;

    size_type i = 0;
    size_type max_col = 0;
    size_type num_rows = 0;

    // Scans each line
    for (const auto& row: set) {
      size_type icol = 0;
      size_type i_prev = i;

      // Inserts the index for the next valid element
      rows_offset.push_back(i);

      // Scans each element on line
      for (const auto& v: row) {
        // Inserts only valid element
        if (v != 0) {
          elems.push_back(v);

          // Inserts index of the column for the valid element
          cols_index.push_back(icol);

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
        rows_offset.back() = INVALID_LINE;

      // Count each line
      num_rows++;
    }

    // Assigns the number of rows
    size_rows_ = rows_offset.size();

    // Assigns the number of elements
    num_elems_ = elems.size();

    // Allocates memory on device
    alloc(size_rows_, num_elems_);

    for (const auto v: rows_offset)
      std::cout << v << ", ";

    memcpy(cols_index_, cols_index.data(), cols_index.size()*sizeof(size_type));
    memcpy(elems_, elems.data(), elems.size()*sizeof(T));
    memcpy(rows_offset_, rows_offset.data(), size_rows_*sizeof(int));

    for (int i = 0; i < size_rows_; i++) {
      std::cout << rows_offset_[i] << " ";
    }

    // Assigns the max_col as the numbers of column for the matrix
    size_cols_ = max_col;
  }

  template<typename T>
  GpuCsr<T>::GpuCsr(size_type rows, size_type nelems)
  : size_rows_(rows)
  , num_elems_(nelems) {
    alloc(size_rows_, num_elems_);
  }

  template<typename T>
  GpuCsr<T>::GpuCsr(const GpuCsr<T>& m)
  : size_rows_(m.size_rows_)
  , num_elems_(m.num_elems_)
  , size_cols_(m.size_cols_) {
    free();

    cudaMemcpy(rows_offset_, m.rows_offset_, size_rows_, cudaMemcpyDeviceToDevice);
    cudaMemcpy(cols_index_, m.cols_index_, size_rows_, cudaMemcpyDeviceToDevice);
    cudaMemcpy(elems_, m.elems_, size_rows_, cudaMemcpyDeviceToDevice);
  }

  template<typename T>
  GpuCsr<T>::GpuCsr(GpuCsr<T>&& m)
  : size_rows_(m.size_rows_)
  , num_elems_(m.num_elems_)
  , size_cols_(m.size_cols_) {
    m.size_rows_ = 0;
    m.num_elems_ = 0;
    m.size_cols_ = 0;

    rows_offset_ = m.rows_offset_;
    cols_index_ = m.cols_index_;
    elems_ = m.elems_;

    m.rows_offset_ = nullptr;
    m.cols_index_ = nullptr;
    m.elems_ = nullptr;
  }

  template<typename T>
  GpuCsr<T>& GpuCsr<T>::operator=(const GpuCsr<T>& m) {
    // self-assignment check
    if (this != &m) {
      rows_offset_ = m.rows_offset_;
      size_cols_ = m.size_cols_;
      num_elems_ = m.num_elems_;

      free();

      cudaMemcpy(rows_offset_, m.rows_offset_, size_rows_, cudaMemcpyDeviceToDevice);
      cudaMemcpy(cols_index_, m.cols_index_, size_rows_, cudaMemcpyDeviceToDevice);
      cudaMemcpy(elems_, m.elems_, size_rows_, cudaMemcpyDeviceToDevice);
    }

    return *this;
  }

  template<typename T>
  GpuCsr<T>& GpuCsr<T>::operator=(GpuCsr<T>&& m) {
    rows_offset_ = m.rows_offset_;
    size_cols_ = m.size_cols_;
    num_elems_ = m.num_elems_;

    m.size_rows_ = 0;
    m.num_elems_ = 0;
    m.size_cols_ = 0;

    rows_offset_ = m.rows_offset_;
    cols_index_ = m.cols_index_;
    elems_ = m.elems_;

    m.rows_offset_ = nullptr;
    m.cols_index_ = nullptr;
    m.elems_ = nullptr;

    return *this;
  }

  template<typename T>
  void GpuCsr<T>::alloc(size_type nrows, size_type nelems) {
    if (rows_offset_ == nullptr)
      cudaHostAlloc((void**)&rows_offset_, nrows*sizeof(int),
          cudaHostAllocWriteCombined | cudaHostAllocMapped);

    if (cols_index_ == nullptr)
      cudaHostAlloc((void**)&cols_index_, nelems*sizeof(size_type),
          cudaHostAllocWriteCombined | cudaHostAllocMapped);

    if (elems_ == nullptr)
      cudaHostAlloc((void**)&elems_, nelems*sizeof(T),
          cudaHostAllocWriteCombined | cudaHostAllocMapped);
  }

  template<typename T>
  void GpuCsr<T>::free() {
    if (rows_offset_ != NULL) {
      cudaFreeHost(rows_offset_);
      rows_offset_ = nullptr;
    }

    if (cols_index_ != NULL) {
      cudaFreeHost(cols_index_);
      cols_index_ = nullptr;
    }

    if (elems_ != NULL) {
      cudaFreeHost(elems_);
      elems_ = nullptr;
    }
  }

  template<typename U>
  std::ostream& operator<<(std::ostream& stream, const GpuCsr<U>& mat) {
    stream << "rows size: " << mat.size_rows_ << "\n";
    stream << "rows vector: ";
    for (int i = 0; i < mat.size_rows_; i++) {
      stream << reinterpret_cast<int*>(mat.rows_offset_)[i] << " ";
    }
    stream << "\n";

    stream << "cols index: ";
    for (int i = 0; i < mat.num_elems_; i++) {
      stream << reinterpret_cast<typename GpuCsr<U>::size_type*>(mat.cols_index_)[i] << " ";
    }
    stream << "\n";

    stream << "elems: ";
    for (int i = 0; i < mat.num_elems_; i++) {
      stream << reinterpret_cast<U*>(mat.elems_)[i] << " ";
    }
    stream << "\n";

    return stream;
  }

  template<typename T>
  void GpuCsr<T>::AddCol(const T* col, size_type size) {

  }

  template<typename T>
  void GpuCsr<T>::AddCol(const std::vector<T>& col) {

  }

  template<typename T>
  void GpuCsr<T>::AddRow(const T* row, size_type size) {

  }

  template<typename T>
  void GpuCsr<T>::AddRow(const std::vector<T>& row) {

  }

  template<typename T>
  template<typename MapFn>
  void GpuCsr<T>::ColMap(size_t i, MapFn&& fn) {

  }

  template<typename T>
  template<typename ReduceFn>
  T GpuCsr<T>::ColReduce(size_t i, ReduceFn&& fn) {

  }

  template<typename T> 
  template<typename MapFn>
  void GpuCsr<T>::Map(MapFn&& fn) {
  }

  template<typename T> 
  template<typename ReduceFn>
  T GpuCsr<T>::Reduce(ReduceFn&& fn) {

  }

  template<typename T> 
  template<typename MapFn>
  void GpuCsr<T>::RowMap(size_t i, MapFn&& fn) {

  }

  template<typename T>
  template<typename ReduceFn>
  T GpuCsr<T>::RowReduce(size_t i, ReduceFn&& fn) {

  }

}}
