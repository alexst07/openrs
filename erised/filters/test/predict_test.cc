#include <gtest/gtest.h>
#include <iostream>
#include <vector>

#include "data_csr_map.h"
#include "predict.h"
#include "flann.h"

class PredictTest : public ::testing::Test {
 protected:
  using Data = erised::DataCsrMap<float>;
  using Sim = erised::flann::SimMat<float>;

  Data mat_;
  Sim sim_;

  PredictTest() {
  }

  virtual ~PredictTest() {
  }

  virtual void SetUp() {
    mat_ = Data{{5, 3, 4, 4, 0},
                {3, 1, 2, 3, 3},
                {4, 3, 4, 3, 5},
                {3, 3, 1, 5, 4},
                {1, 5, 5, 2, 1}};

    std::vector<float> vec({1, 0.839181, 0.606339, 0, -0.768095,
                            0.839181, 1, 0.467707, 0.489956, -0.900149,
                            0.606339, 0.467707, 1, -0.161165, -0.46657,
                            0, 0.489956, -0.161165, 1, -0.641503,
                            -0.768095, -0.900149, -0.46657, -0.641503, 1});

    Sim sim(vec, 5);
    sim_ = sim;
  }

  virtual void TearDown() {

  }
};

TEST_F(PredictTest, PredictVec) {
  using namespace erised;
  std::vector<float> ratings = {1, 2, 4, 3, 2};
  std::vector<float> sims = {0.8, 0.85, 0.88, 0.9, 0.92};

  ItemFilterVec<std::vector<float>> item_model(ratings, sims);
  PredictVec<ItemFilterVec<std::vector<float>>> predict(item_model);
  float v = predict.Predict();

  std::cout << "Value: " << v << "\n";
}

TEST_F(PredictTest, Predict) {
  erised::UserFilter<erised::DataCsrMap<float>, erised::flann::SimMat<float>> filter();

  erised::PredictData<
      erised::UserFilter<erised::DataCsrMap<float>, erised::flann::SimMat<float>>>
  pred(erised::Axis::ROW);
  std::vector<size_t> indexes = {1, 2};

  auto l = [](size_t i, float v1, float v2, std::array<float, 3> arr) {
    std::array<float,3> r;
    r[0] = v1;
    std::cout << "v1: " << v1 << ", v2: " << v2 << "\n";

    r[1] = v2;

    r[2] = v1 + v2;

    r[0] += arr[0];
    r[1] += arr[1];
    r[2] += arr[2];

    return r;
  };

  auto res = pred.template Terms<3>(this->mat_, this->sim_, 0, indexes, l);
  std::cout << this->sim_;

  ASSERT_NEAR(res[0], 7, 0.01);
  ASSERT_NEAR(res[1], 0.839181 + 0.606339, 0.01);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
