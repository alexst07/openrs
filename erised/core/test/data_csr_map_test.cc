#include <gtest/gtest.h>
#include <iostream>

#include "data_csr_map.h"

template <class Data>
class DataRatingTest : public ::testing::Test {
 public:
  Data mat_;

  void SetUp() {
      mat_ = Data{{0.44, 0.8, 0,   0,   0,   0,   0,   0,   0  },
                  {0,    0.8, 1.4, 0,   0,   0,   1.7, 0,   0  },
                  {0.22, 0,   0,   0.7, 0.4, 0,   0,   0,   0  },
                  {0.22, 0,   0,   0,   0,   0.7, 0.7, 0.7, 0  },
                  {0,    0,   0,   0.2, 0.4, 0,   0,   0,   0.7},
                  {0.22, 0,   0,   0,   0.4, 0,   0.3, 0.1, 0  }};

  }

  void TearDown() {

  }

};

TYPED_TEST_CASE_P(DataRatingTest);

TYPED_TEST_P(DataRatingTest, Access) {
  auto v = this->mat_({2,3});
  ASSERT_FLOAT_EQ(0.7, v);

  v = this->mat_({3,4});
  ASSERT_FLOAT_EQ(0, v);

  ASSERT_THROW(this->mat_({43,4}), erised::Exception);
  ASSERT_THROW(this->mat_({4,40}), erised::Exception);
}

TYPED_TEST_P(DataRatingTest, MinMaxSingle) {
  // Rows
  auto v = this->mat_.Min(0, erised::Axis::ROW);
  ASSERT_FLOAT_EQ(0.44, v);

  v = this->mat_.Min(1, erised::Axis::ROW);
  ASSERT_FLOAT_EQ(0.8, v);

  v = this->mat_.Min(2, erised::Axis::ROW);
  ASSERT_FLOAT_EQ(0.22, v);

  // Cols
  v = this->mat_.Min(0, erised::Axis::COL);
  ASSERT_FLOAT_EQ(0.22, v);

  v = this->mat_.Min(1, erised::Axis::COL);
  ASSERT_FLOAT_EQ(0.8, v);

  v = this->mat_.Min(3, erised::Axis::COL);
  ASSERT_FLOAT_EQ(0.2, v);

  // Exceptions
  ASSERT_THROW(this->mat_.Min(20, erised::Axis::ROW), erised::Exception);
  ASSERT_THROW(this->mat_.Min(20, erised::Axis::COL), erised::Exception);
}

TYPED_TEST_P(DataRatingTest, MinMaxVec) {
  // Rows
  auto&& v = this->mat_.MinElemsRows();
//   std::vector<float> vtest = {0.22, 0.8, 1.4, 0.7, 0.4, 0.7, 0.7, 0.7, 0.7};

  std::vector<float> vtest = {0.44, 0.8, 0.22, 0.22, 0.2, 0.1};

  int i = 0;
  for (auto const& e: v) {
    ASSERT_FLOAT_EQ(e, vtest[i]);
    i++;
  }
}

REGISTER_TYPED_TEST_CASE_P(DataRatingTest,
                           Access,
                           MinMaxSingle,
                           MinMaxVec
                          );

typedef ::testing::Types<erised::DataCsrMap<float>> Types;
INSTANTIATE_TYPED_TEST_CASE_P(My, DataRatingTest, Types);

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
