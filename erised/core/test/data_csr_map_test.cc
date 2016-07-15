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

TYPED_TEST_P(DataRatingTest, MinVec) {
  // Rows
  auto&& v = this->mat_.MinElemsRows();

  std::vector<float> vtest = {0.44, 0.8, 0.22, 0.22, 0.2, 0.1};

  int i = 0;
  for (auto const& e: v) {
    ASSERT_FLOAT_EQ(e, vtest[i]);
    i++;
  }

  // Cols
  auto&& vc = this->mat_.MinElemsCols();
  std::vector<float> vtestc = {0.22, 0.8, 1.4, 0.2, 0.4, 0.7, 0.3, 0.1, 0.7};

  i = 0;
  for (auto const& e: vc) {
    ASSERT_FLOAT_EQ(e, vtestc[i]);
    i++;
  }
}

TYPED_TEST_P(DataRatingTest, MaxVec) {
  // Rows
  auto&& v = this->mat_.MaxElemsRows();

  std::vector<float> vtest = {0.8, 1.7, 0.7, 0.7, 0.7, 0.4};

  int i = 0;
  for (auto const& e: v) {
    ASSERT_FLOAT_EQ(e, vtest[i]);
    i++;
  }

  // Cols
  auto&& vc = this->mat_.MaxElemsCols();
  std::vector<float> vtestc = {0.44, 0.8, 1.4, 0.7, 0.4, 0.7, 1.7, 0.7, 0.7};

  i = 0;
  for (auto const& e: vc) {
    ASSERT_FLOAT_EQ(e, vtestc[i]);
    i++;
  }
}

TYPED_TEST_P(DataRatingTest, NumElements) {
  // Rows
  auto&& v = this->mat_.NumElementsLines();

  std::vector<size_t> vtest = {2, 3, 3, 4, 3, 4};

  int i = 0;
  for (auto const& e: v) {
    ASSERT_EQ(e, vtest[i]);
    i++;
  }

  // Cols
  auto&& vc = this->mat_.NumElementsCols();
  std::vector<size_t> vtestc = {4, 2, 1, 2, 3, 1, 3, 2, 1};

  i = 0;
  for (auto const& e: vc) {
    ASSERT_EQ(e, vtestc[i]);
    i++;
  }
}

TYPED_TEST_P(DataRatingTest, AssignMove) {
  typedef decltype(this->mat_) TYPE;

  TYPE mat1 = {{0.44, 0.8, 0,   0,   0,   0,   0,   0,   0  },
              {0,    0.8, 1.4, 0,   0,   0,   1.7, 0,   0  },
              {0.22, 0,   0,   0.7, 0.4, 0,   0,   0,   0  },
              {0.22, 0,   0,   0,   0,   0.7, 0.7, 0.7, 0  },
              {0,    0,   0,   0.2, 0.4, 0,   0,   0,   0.7},
              {0.22, 0,   0,   0,   0.4, 0,   0.3, 0.1, 0  }};
  TYPE mat2;
  TYPE mat3;

  mat2 = std::move(mat1);
  mat3 = mat2;

  auto v = mat2({2,3});
  ASSERT_FLOAT_EQ(0.7, v);

  v = mat2({3,4});
  ASSERT_FLOAT_EQ(0, v);

  v = mat3({2,3});
  ASSERT_FLOAT_EQ(0.7, v);
}

TYPED_TEST_P(DataRatingTest, MapRows) {
  typedef decltype(this->mat_) TYPE;

  TYPE mat1 = this->mat_;

  mat1.MapRows([](int i, float v) -> float {
    return i*v;
  });

  auto v = mat1({1,1});
  ASSERT_FLOAT_EQ(0.8, v);

  v = mat1({2,3});
  ASSERT_FLOAT_EQ(1.4, v);

  v = mat1({5,6});
  ASSERT_FLOAT_EQ(1.5, v);
}

TYPED_TEST_P(DataRatingTest, MapCols) {
  typedef decltype(this->mat_) TYPE;

  TYPE mat1 = this->mat_;

  mat1.MapCols([](int i, float v) -> float {
    return i*v;
  });

  auto v = mat1({1,1});
  ASSERT_FLOAT_EQ(0.8, v);

  v = mat1({2,3});
  ASSERT_FLOAT_EQ(2.1, v);

  v = mat1({5,6});
  ASSERT_FLOAT_EQ(1.8, v);
}

TYPED_TEST_P(DataRatingTest, ReduceRows) {
  typedef decltype(this->mat_) TYPE;

  TYPE mat1 = this->mat_;
  std::vector<float> vtest = {1.24, 3.9, 1.32, 2.32, 1.3, 1.02};

  auto vec = mat1.ReduceRows([](int i, float v1, float v2) -> float {
    return v1 + v2;
  });

  for (int i = 0; i < vtest.size(); i++)
    ASSERT_FLOAT_EQ(vec[i], vtest[i]);
}

TYPED_TEST_P(DataRatingTest, ReduceCols) {
  typedef decltype(this->mat_) TYPE;

//   TYPE mat1 = this->mat_;
//   std::vector<float> vtest = {1.24, 3.9, 1.32, 2.32, 1.3, 1.02};
//
//   auto vec = mat1.ReduceCols([](int i, float v1, float v2) -> float {
//     return v1 + v2;
//   });
//
//   for (int i = 0; i < vec.size(); i++)
//     std::cout << vec[i] << " ";
}

REGISTER_TYPED_TEST_CASE_P(DataRatingTest,
                           Access,
                           MinMaxSingle,
                           MinVec,
                           MaxVec,
                           NumElements,
                           AssignMove,
                           MapRows,
                           MapCols,
                           ReduceRows,
                           ReduceCols
                          );

typedef ::testing::Types<erised::DataCsrMap<float>> Types;
INSTANTIATE_TYPED_TEST_CASE_P(My, DataRatingTest, Types);

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
