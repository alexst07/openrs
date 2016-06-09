#include "parallel.h"

#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <atomic>
#include <numeric>

TEST(Parallel, parallel_for) {
  using namespace erised;

  std::vector<int> v = {1, 2, 3, 4, 5, 6};

  Range<std::vector<int>::iterator> range(v.begin(), v.end());
  parallel_for(range, [&](const Range<std::vector<int>::iterator>& r) {
    for(std::vector<int>::iterator i=r.begin(); i!=r.end(); ++i )
      *i = *i*2;
  });

  for (const auto& e: v)
    std::cout << e << " ";

  std::cout << "\n";
}

TEST(Parallel, parallel_reduce) {
  using namespace erised;

  std::vector<int> v = {1, 2, 3, 4, 5, 6};

  Range<std::vector<int>::iterator> range(v.begin(), v.end());
  int r = parallel_reduce(range, 0,
      [&](const Range<std::vector<int>::iterator>& r, int value)->int {
        return std::accumulate(r.begin(),r.end(),value);
      }, std::plus<int>());

  std::cout << "total: " << r << "\n";
}

TEST(Parallel, parallel_for_sum_int) {
  using namespace erised;

  std::vector<int> v = {1, 2, 3, 4, 5, 6};
  std::atomic<int> sum(0);

  Range<int> range(0, 6);
  parallel_for(range, [&](const Range<int>& r) {
    for(int i=r.begin(); i!=r.end(); ++i )
      sum += v[i];
  });

  std::cout << "total--: " << sum << "\n";
}

TEST(Parallel, parallel_reduce_int) {
  using namespace erised;

  std::vector<int> v = {1, 2, 3, 4, 5, 6};
  std::atomic<int> sum(0);

  Range<int> range(0, 6);
  int r = parallel_reduce(range, static_cast<int>(0),
      [&](const Range<int>& r, int value)->int {
        int ret = value;
        for(auto i = r.begin(); i!=r.end(); ++i) {
          ret =  v[i] + ret;
        }

        return ret;
      }, [](int a, int b) -> int {
        return a + b;
      });

  std::cout << "total--: " << r << "\n";
}
