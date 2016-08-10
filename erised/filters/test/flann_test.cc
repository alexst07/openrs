#include <gtest/gtest.h>
#include <iostream>

#include <vector>

#include "flann.h"

TEST(Sample, MoveSim) {
  erised::flann::SimMat<float> sim(2);
  sim(1.2, 0, 0);
  sim(1.2, 1, 0);
  sim(1.2, 0, 1);
  sim(1.2, 1, 1);

  erised::flann::SimMat<float> sim2;

  std::cout << sim;

  sim2 = std::move(sim);

  std::cout << sim2;
}

TEST(Sample, LoopMat) {
  using namespace erised::flann;

  std::vector<float> datav = {1, 2, 3, 1, 2, 3, 4, 3, 4, 5};
  Mat<float> data(datav.data(), 5, 2);

  std::cout << data << "\n";

  for (const auto e: data) {
     std::cout << e << " ";
  }

  std::cout << "\n---------\n";

  auto row = data.Row(2);

  for (auto const& e: row)
    std::cout << e << " ";
}

TEST(Sample, Sampleunit) {
//   using namespace erised::flann;
//   size_t nn = 3;
//
//   std::vector<float> datav = {1, 2, 3, 1, 2, 3, 4, 3, 4, 5};
//   SimMat<float> data(datav.data(), 5, 2);
//
//   std::vector<float> queryv = {4, 2, 2, 2};
//   SimMat<float> query(queryv.data(), 2, 2);
//
//   SimMat<size_t> indices(query.Rows(), nn);
//   SimMat<float> dists(query.Rows(), nn);
//
//   Index<L2<float>> index(data, KDTreeIndexParams(4));
//   index.BuildIndex();
//   index.KnnSearch(query, indices, dists, nn, SearchParams(4));
//
//   std::cout << data ;
//
//   std::cout << "\n-----\n";
//
//   std::cout << query;
//
//   std::cout << "\n-----\n";
//
//   std::cout << indices;
//
//   std::cout << "\n-----\n";
//
//   std::cout << dists;

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
