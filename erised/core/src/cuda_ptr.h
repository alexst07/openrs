#pragma once
#define ERISED_CSR_MATRIX_H_

#include "data_base.h"

#include <initializer_list>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>

// namespace erised::cuda
namespace erised { namespace cuda {

template<class T>
using cu_unique_ptr = std::unique_ptr<T>;

template<class T>
std::unique_ptr<T> mk_host_unique(std::size_t size) {
  T *ptr;
  cudaHostAlloc((void**)&ptr, size*sizeof(T),
      cudaHostAllocWriteCombined | cudaHostAllocMapped);
  auto data = std::unique_ptr<T, decltype(cudaFreeHost)*>{ptr, cudaFreeHost };
  return data;
}

}}

#include <data_csr-inl.h>
