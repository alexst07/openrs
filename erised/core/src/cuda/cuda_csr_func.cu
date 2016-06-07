#include "cuda_csr_func.h"

namespace erised { namespace cuda {

template<class MapFn, class MapFn>
__global__
void GpuCsr<T>::CuMap(T* elems, size_t len, MapFn fn) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  if (idx < len) {
    elems[idx] = fn(elems[idx]);
  }
}

}}
