#pragma once

#include "config.h"

#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>

namespace erised {

template<typename Iter>
class Range {
 public:
   Range(Iter begin, Iter end) : begin_(begin), end_(end) {}

  Iter begin() const {
    return begin_;
  }

  Iter end() const {
    return end_;
  }

 private:
  Iter begin_;
  Iter end_;
};

#ifdef HAVE_TBB
template<typename Iter, typename Func>
inline void parallel_for(const Range<Iter>& range, Func&& f) {
  tbb::parallel_for(tbb::blocked_range<Iter>(range.begin(), range.end()),
      [&](const tbb::blocked_range<Iter>& r) {
        Range<Iter> rg(r.begin(), r.end());
        f(rg);
      });
}

template<typename Range,
         typename T,
         typename Iter,
         typename Func,
         typename Reduct>
T parallel_reduce(const Range& range, const T& init, Func&& f, Reduct&& fr) {
  auto r = tbb::blocked_range<Iter>(range.begin(), range.end());
  return tbb::parallel_reduce(r, init, f, fr);
}
#else

#endif

}
