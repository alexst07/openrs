#ifndef __ERISED_CSR_MATRIX__
#define __ERISED_CSR_MATRIX__

#include "mat_base.h"

#include <initializer_list>
#include <vector>
#include <memory>
#include <functional>

namespace erised {

class CsrRow;

template<typename T>
class CsrMat: public MatBase<T, 2> {
 public:
  using size_type = std::size_t;

  CsrMat() {}
  CsrMat(size_type rows, size_type cols): rows_{rows}, cols_{cols} {}
  CsrMat(const CsrMat<T>& m): rows_{m.rows_}, cols_{m.cols_},
      elems_{m.elems_}, size_cols_{m.size_cols_} {}

  CsrMat(CsrMat<T>&& m): rows_{std::move(m.rows_)}, cols_{std::move(m.cols_)},
      elems_{std::move(m.elems_)} {}

  void Apply(std::function<T(T)>);
  void RowApply(std::function<T(T)>);
  void ColApply(std::function<T(T)>);

  T Reduce(std::function<T(T)>);
  T RowReduce(std::function<T(T)>);
  T ColReduce(std::function<T(T)>);

  CsrRow& Row(size_type index) const;

  const CsrRow& operator[](size_type index) const;
  CsrRow& operator[](size_type index) const;

 private:
  std::vector<size_type> rows_;
  std::vector<size_type> cols_;
  std::vector<T> elems_;

  size_type size_rows_;
  size_type size_cols_;
  size_type num_elems_;
};

template<typename T>
class CsrRow {
 public:
  using size_type = std::size_t;

  CsrRow(size_type* pcols, T* pelems, size_type row_len): pcols_{pcols},
      pelems_{pelems}, row_len_{row_len} {}

  const T& operator[](size_type index) const;
  T& operator[](size_type index) const;

private:
  size_type* pcols_;
  T* pelems_;
  size_type row_len_;
};

template<typename T>
class CsrCol {
public:
  using size_type = std::size_t;

  CsrRow(size_type* pcols, T* pelems, size_type row_len): pcols_{pcols},
  pelems_{pelems}, row_len_{row_len} {}

  const T& operator[](size_type index) const;
  T& operator[](size_type index) const;

private:
  size_type* pcols_;
  T* pelems_;
  size_type row_len_;
};

}
#endif //__ERISED_CSR_MATRIX__
