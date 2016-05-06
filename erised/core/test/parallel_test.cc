#include "parallel.h"

#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <numeric>

TEST(Parallel, parallel_for) {
  using namespace erised;

  std::vector<int> v = {1, 2, 3, 4, 5, 6};

  Range<std::vector<int>::iterator> range(v.begin(), v.end());
  parallel_for(range, [&](Range<std::vector<int>::iterator>& r){
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
  int r = parallel_reduce(range, 0, [&](const Range<std::vector<int>::iterator>& r, int value)->int {
    return std::accumulate(r.begin(),r.end(),value);
  }, std::plus<int>());

  std::cout << "total: " << r << "\n";
}
