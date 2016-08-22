#include <gtest/gtest.h>
#include <iostream>

#include <vector>

#include "flann.h"
#include "correlation.h"

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

  std::vector<float> datatv = {10, 10};
  Mat<float> datat(datatv.data(), 1, 2);

  data.SetRow(2, datat);

  std::cout << "\n---------\n";
  std::cout << data << "\n";
}

TEST(Sample, SimMat) {
  using namespace erised::flann;

  std::vector<float> datav = {1, 2, 3, 4, 1, 3, 1, 2, 2, 3, 1, 5, 3, 1, 1, 2, 4, 2, 5, 2};
  SimMat<float> data(datav.data(), 5);

  std::cout << data << "\n";

  for (const auto e: data) {
     std::cout << e << " ";
  }

  std::cout << "\n---------\n";

  auto row = data.Row(2);

  for (auto const& e: row)
    std::cout << e << " ";
}

TEST(Sample, Neighbors) {
  using namespace erised::flann;

  std::vector<float> datat = {1};
  std::vector<float> datav = {1, 2, 3, 4, 1, 3, 1, 2, 2, 3, 1, 5, 3, 1, 1, 2, 4, 2, 5, 2};
  SimMat<float> data(datav.data(), 5);
  Mat<float> data_test(datat, 1, 1);
  Mat<size_t> indexes;
  Mat<float> neighbors;


  std::cout << "\n===\n" << data << "\n=====\n" << data_test;

  erised::Knn<SimMat<float>> knn(data, KDTreeIndexParams(4));
  std::tie(neighbors, indexes) = knn.Search(data_test, 2, SearchParams(2));

  std::cout << "\n--Indexes--\n" << indexes << "\n";

  std::cout << "\n--Neighbors--\n" << neighbors << "\n";
}

TEST(Sample, Sampleunit) {
  using namespace erised::flann;
  size_t nn = 3;

  std::vector<float> datav = {1, 2, 3, 1, 2, 3, 4, 3, 4, 5};
  Mat<float> data(datav.data(), 5, 2);

  std::vector<float> queryv = {4, 2, 2, 2};
  Mat<float> query(queryv.data(), 2, 2);

  Mat<size_t> indices;
  Mat<float> dists;

  Index<float, L2<float>> index(data, KDTreeIndexParams(4));

  std::tie(indices, dists) = index.KnnSearch(query, nn, SearchParams(4));

  std::cout << data ;

  std::cout << "\n--query--\n";

  std::cout << query;

  std::cout << "\n--indices--\n";

  std::cout << indices;

  std::cout << "\n--dists--\n";

  std::cout << dists;

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
