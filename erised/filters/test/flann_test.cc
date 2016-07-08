#include <gtest/gtest.h>
#include <iostream>

#include <vector>

#include "flann.h"

TEST(Sample, Sampleunit) {
  using namespace erised::flann;
  size_t nn = 3;

  std::vector<float> datav = {1, 2, 3, 1, 2, 3, 4, 3, 4, 5};
  SimMat<float> data(datav.data(), 5, 2);

  std::vector<float> queryv = {4, 2};
  SimMat<float> query(std::move(queryv), 1, 2);

  SimMat<size_t> indices(query.Rows(), nn);
  SimMat<float> dists(query.Rows(), nn);

  Index<L2<float>> index(data, KDTreeIndexParams(4));
  index.BuildIndex();
  index.KnnSearch(query, indices, dists, nn, SearchParams(4));

  std::cout << data ;

  std::cout << "\n-----\n";

  std::cout << query;

  std::cout << "\n-----\n";

  std::cout << indices;

  std::cout << "\n-----\n";

  std::cout << dists;

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
