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
class DataCsr: public DataBase {
 public:
  using size_type = std::size_t;

  CsrMat();
  CsrMat(size_type rows, size_type cols);
  CsrMat(std::initializer_list<std::initializer_list<T>> set);
  CsrMat(const CsrMat<T>& m);
  CsrMat(CsrMat<T>&& m);

  CsrMat();
  CsrMat(size_type rows, size_type cols);

  template<class Fn = void(T)>
  virtual void Map(Fn&& fn) = 0;

  template<class Fn = void(T)>
  virtual void RowMap(size_t i, Fn&& fn) = 0;

  template<class Fn = void(T)>
  virtual void ColMap(size_t i, Fn&& fn) = 0;

  template<class Fn = T(T,T)>
  virtual T Reduce(Fn&& fn) = 0;

  template<class Fn = T(T,T)>
  virtual T RowReduce(size_t i, Fn&& fn) = 0;

  template<class Fn = T(T,T)>
  virtual T ColReduce(size_t i, Fn&& fn) = 0;

  void AddRow(const std::vector<T>& row);
  void AddRow(const T* row, size_type size);

  void AddCol(const std::vector<T>& col);
  void AddCol(const T* col, size_type size);

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
