#ifndef __ERISED_EIGEN_MATRIX__
#define __ERISED_EIGEN_MATRIX__

#include "data_base.h"

#include <initializer_list>
#include <eigen3/Eigen/Sparse>

namespace erised { namespace eigen {

template<typename T>
class Mat: public DataBase<T, 2> {
 public:
  Mat(): mat_() {}
  Mat(size_t rows, size_t cols): mat_(rows, cols) {}

 private:
   typedef Eigen::SparseMatrix<T> SpMat;
   SpMat mat_;
};

}}
#endif //__ERISED_MATRIX_BASE__
