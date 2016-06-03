#include "cuda_csr.h"

namespace erised { namespace cuda {

template<typename T>
template<typename MapFn>
__global__
void GpuCsr<T>::CuMap(MapFn&& fn) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  if (idx < num_elems_) {
    elems_[idx] = fn(elems_[idx]);
  }
}

}}
