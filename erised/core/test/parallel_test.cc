#include "parallel.h"

#include <gtest/gtest.h>
#include <iostream>
#include <vector>

TEST(Parallel, parallel_for) {
  using namespace erised;

  std::vector<int> v = {1, 2, 3, 4, 5, 6};

  Range<std::vector<int>::iterator> range(v.begin(), v.end());
  parallel_for(range, [&](Range<std::vector<int>::iterator>& r){
    for(std::vector<int>::iterator i=r.begin(); i!=r.end(); ++i )
      *i = 2;
  });
}
