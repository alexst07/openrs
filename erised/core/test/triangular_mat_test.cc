#include <gtest/gtest.h>
#include <iostream>

#include "triangular_mat.h"

template <typename T>
class TriangularMatTest : public ::testing::Test {
 public:
  static T shared_;
  std::vector<T> elems;
  erised::TriangularMat<T> mat_;

  void SetUp() {
    elems = {1, 2, 3, 4, 5, 6};
    mat_ = erised::TriangularMat<T>(4, std::move(elems));
  }

  void TearDown() {

  }

};

TYPED_TEST_CASE_P(TriangularMatTest);

TYPED_TEST_P(TriangularMatTest, Access) {



}

REGISTER_TYPED_TEST_CASE_P(TriangularMatTest,
                           Access);

typedef ::testing::Types<int, float, double> Types;
INSTANTIATE_TYPED_TEST_CASE_P(My, TriangularMatTest, Types);

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
