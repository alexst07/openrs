#include <gtest/gtest.h>
#include <iostream>

#include "data_csr_map.h"

TEST(Sample, Sampleunit) {
  std::cout << "teste\n";
}


// template <typename T>
// class TriangularMatTest : public ::testing::Test {
//  public:
//   static T shared_;
//   std::vector<T> elems;
//   erised::TriangularMat<T> mat_;
//
//   void SetUp() {
//     mat_ = erised::TriangularMat<T>(4, {1, 2, 3, 4, 5, 6});
//   }
//
//   void TearDown() {
//
//   }
//
// };
//
// TYPED_TEST_CASE_P(TriangularMatTest);
//
// TYPED_TEST_P(TriangularMatTest, Access) {
//   auto row = TestFixture::mat_.Row(3);
//
//   std::cout << "row size: " << row.Size() << "\n";
//   for (const auto& v : row)
//     std::cout << v << ", ";
//
//   std::cout << "\n\n";
//
//   auto col = TestFixture::mat_.Col(2);
//
//   std::cout << "col size: " << col.Size() << "\n";
//   for (const auto& v : col)
//     std::cout << v << ", ";
//
//   std::cout << "\n\n";
// }
//
// REGISTER_TYPED_TEST_CASE_P(TriangularMatTest,
//                            Access);
//
// typedef ::testing::Types<int, float, double> Types;
// INSTANTIATE_TYPED_TEST_CASE_P(My, TriangularMatTest, Types);

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

// #include "data_csr_map.h"
//
// #include <gtest/gtest.h>
// #include <iostream>
//
// TEST(Sample, Sampleunit) {
//   using namespace erised;
//
//   DataCsrMap<float> data{{0.44, 0.8, 0,   0,   0,   0,   0,   0,   0  },
//                          {0,    0.8, 1.4, 0,   0,   0,   0,   0,   0  },
//                          {0.22, 0,   0,   0.7, 0.4, 0,   0,   0,   0  },
//                          {0.22, 0,   0,   0,   0,   0.7, 0.7, 0.7, 0  },
//                          {0,    0,   0,   0,   0.4, 0,   0,   0,   0.7},
//                          {0.22, 0,   0,   0,   0.4, 0,   0,   0,   0  }};
//
//   std::cout << "\nMin elements col 0: " << data.Min(0, Axis::COL) << "\n";
//   std::cout << "\nMin elements row 2: " << data.Min(2, Axis::ROW) << "\n";
//   std::cout << "\nMax elements col 0: " << data.Max(0, Axis::COL) << "\n";
//   std::cout << "\nMax elements row 2: " << data.Max(2, Axis::ROW) << "\n";
//
//   float row_red = data.RowReduce(5, [](float a, float b) {
//     return 2*a + b;
//   });
//
//   std::cout << "row 5 reduce: " << row_red << "\n";
//
//   float col_red = data.ColReduce(5, [](float a, float b) -> float {
//     float c = 3*a + b;
//     return c;
//   });
//
//   std::cout << "col 5 reduce: " << col_red << "\n";
//
//   data.Map([](float v) -> float { return 2*v; });
//
//   data.RowMap(3, [](float v) -> float { return 8*v; });
//
//   data.ColMap(4, [](float v) -> float { return 10*v; });
//   std::cout << data;
//
//   float r = data.Reduce([](float a, float b) {
//     return a + b;
//   });
//
//   std::cout << "\nReduce: " << r << "\n";
//
//   std::cout << "\n(2,3): " << data({2,3}) << "\n";
//   std::cout << "\n(1,1): " << data(1,1) << "\n";
//   std::cout << "\n(2,0): " << data(2,2) << "\n";
//
//   std::cout << "\nNum elements: " << data.NumElements() << "\n";
//   std::cout << "\nNum elements line 3: " << data.NumElementsLine(3) << "\n";
//   std::cout << "\nNum elements col 4: " << data.NumElementsCol(4) << "\n";
// }
//
// int main(int argc, char **argv) {
//   ::testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
