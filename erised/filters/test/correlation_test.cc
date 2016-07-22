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
  erised::AdjustedCosine<
      erised::DataCsrMap<float>,
      erised::flann::SimMat<float>> adcos(erised::Axis::COL);

  erised::DataCsrMap<float> mat1 = this->mat_;

  auto arr = adcos.SimTerms(mat1, 0, 4, 3.2, 3.25);

  float res = arr[0]/(sqrt(arr[1])*sqrt(arr[2]));
  std::cout << res;
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
