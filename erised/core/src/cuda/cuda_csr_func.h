#pragma once

// namespace erised::cuda
namespace erised { namespace cuda {

template<class T, class MapFn>
__global__
void CuMap(T* elems, size_t len, MapFn fn);

template<class T, class MapFn>
__global__
void CuRowMap(T* elems, size_t len, size_t i, MapFn fn);

}}
