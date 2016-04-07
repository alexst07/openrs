#ifndef __ERISED_CSR_MATRIX__
#define __ERISED_CSR_MATRIX__

#include "data_base.h"

#include <initializer_list>
#include <vector>
#include <memory>
#include <functional>

namespace erised {

class CsrRow;

template<typename T>
class DataCsr: public DataBase<T, 2> {
 public:
  using size_type = std::size_t;

//   CsrMat() {}
//   CsrMat(size_type rows, size_type cols): rows_{rows}, cols_{cols} {}
//   CsrMat(const CsrMat<T>& m): rows_{m.rows_}, cols_{m.cols_},
//       elems_{m.elems_}, size_cols_{m.size_cols_} {}
//
//   CsrMat(CsrMat<T>&& m): rows_{std::move(m.rows_)}, cols_{std::move(m.cols_)},
//       elems_{std::move(m.elems_)} {}

  CsrMat();
  CsrMat(size_type rows, size_type cols);

  template<class Fn = T(T)>
  void Apply(Fn&& fn) override;

  template<class Fn = T(T)>
  void RowApply(Fn&& fn) override;

  template<class Fn = T(T)>
  void ColApply(Fn&& fn) override;

  template<class Fn = T(T)>
  T Reduce(Fn&& fn) override;

  template<class Fn = T(T)>
  T RowReduce(Fn&& fn) override;

  template<class Fn = T(T)>
  T ColReduce(Fn&& fn) override;

 private:
  std::vector<size_type> rows_;
  std::vector<size_type> cols_;
  std::vector<T> elems_;

  size_type size_rows_;
  size_type size_cols_;
  size_type num_elems_;
};

}
#endif //__ERISED_CSR_MATRIX__
