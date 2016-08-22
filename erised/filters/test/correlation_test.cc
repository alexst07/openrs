#include <gtest/gtest.h>
#include <iostream>
#include <cmath>

#include "correlation.h"
#include "flann.h"
#include "data_csr_map.h"

template <class Data>
class CorrelationTest : public ::testing::Test {
 public:
  Data mat_;

  void SetUp() {
    mat_ = Data{{5, 3, 4, 4, 0},
                {3, 1, 2, 3, 3},
                {4, 3, 4, 3, 5},
                {3, 3, 1, 5, 4},
                {1, 5, 5, 2, 1}};
  }

  void TearDown() {

  }

};

TYPED_TEST_CASE_P(CorrelationTest);

TYPED_TEST_P(CorrelationTest, AdjustedCosine) {
  erised::Correlation<
      erised::DataCsrMap<float>,
      erised::flann::SimMat<float>,
      erised::PearsonCossine<float, erised::DataCsrMap<float>>>
  adcos(this->mat_, erised::Axis::ROW);

//   erised::DataCsrMap<float> mat1 = this->mat_;

  auto sims = adcos.Similarities();

//   float res = arr[0]/(sqrt(arr[1])*sqrt(arr[2]));
//   ASSERT_NEAR(res, 0.83918, 0.01);
//
//   adcos.Fit(mat1);
//   auto& sims = adcos.Similarity();

  std::cout << sims;
}


REGISTER_TYPED_TEST_CASE_P(CorrelationTest,
                           AdjustedCosine
                          );

typedef ::testing::Types<erised::DataCsrMap<float>> Types;
INSTANTIATE_TYPED_TEST_CASE_P(My, CorrelationTest, Types);

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
