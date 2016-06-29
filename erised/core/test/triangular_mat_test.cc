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
    mat_ = erised::TriangularMat<T>(4, {1, 2, 3, 4, 5, 6});
  }

  void TearDown() {

  }

};

TYPED_TEST_CASE_P(TriangularMatTest);

TYPED_TEST_P(TriangularMatTest, Access) {
  auto row = TestFixture::mat_.Row(1);

  std::cout << "row size: " << row.Size() << "\n";
  for (const auto& v : row)
    std::cout << v << ", ";

  std::cout << "\n\n";
}

REGISTER_TYPED_TEST_CASE_P(TriangularMatTest,
                           Access);

typedef ::testing::Types<int, float, double> Types;
INSTANTIATE_TYPED_TEST_CASE_P(My, TriangularMatTest, Types);

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
