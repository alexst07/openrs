#include <gtest/gtest.h>
#include <iostream>
#include <cmath>

#include "basic_statistic.h"
#include "data_csr_map.h"

template <class Data>
class BasicStatisticTest : public ::testing::Test {
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

TYPED_TEST_CASE_P(BasicStatisticTest);

TYPED_TEST_P(BasicStatisticTest, Avarage) {
  auto vec_rows = erised::Avarage(this->mat_, erised::Axis::ROW);
  std::vector<float> vtest_rows = {0.62, 1.3, 0.44, 0.58, 0.43333337, 0.255};

  for (int i = 0; i < vec_rows.size(); i++)
    ASSERT_FLOAT_EQ(vec_rows[i], vtest_rows[i]);

  auto vec_cols = erised::Avarage(this->mat_, erised::Axis::COL);
  std::vector<float> vtest_cols = {0.275, 0.8, 1.4, 0.45, 0.4, 0.7, 0.9, 0.4, 0.7};

  for (int i = 0; i < vec_rows.size(); i++)
    ASSERT_FLOAT_EQ(vec_cols[i], vtest_cols[i]);
}

TYPED_TEST_P(BasicStatisticTest, Variance) {
  auto num_elems_rows = this->mat_.NumElementsLines();
  auto vec_rows = erised::Variance(this->mat_, erised::Axis::ROW, num_elems_rows);
  std::vector<float> vtest_rows = {0.0324, 0.14, 0.0392, 0.0432, 0.0422222, 0.012075};

  for (int i = 0; i < vec_rows.size(); i++)
    ASSERT_FLOAT_EQ(vec_rows[i], vtest_rows[i]);

  auto num_elems_cols = this->mat_.NumElementsCols();
  auto vec_cols = erised::Variance(this->mat_, erised::Axis::COL, num_elems_cols);
  std::vector<float> vtest_cols = {0.009075, 0, 0, 0.0625, 0, 0, 0.34666669, 0.09, 0};

  for (int i = 0; i < vec_cols.size(); i++)
    ASSERT_FLOAT_EQ(vec_cols[i], vtest_cols[i]);
}

TYPED_TEST_P(BasicStatisticTest, StandardDeviation) {
  auto num_elems_rows = this->mat_.NumElementsLines();
  auto vec_rows = erised::StandardDeviation(this->mat_, erised::Axis::ROW, num_elems_rows);
  std::vector<float> vtest_rows = {0.18, 0.37416574, 0.19799, 0.207846, 0.20548, 0.109886};

  for (int i = 0; i < vec_rows.size(); i++)
    ASSERT_NEAR(vec_rows[i], vtest_rows[i], 0.01);

  auto num_elems_cols = this->mat_.NumElementsCols();
  auto vec_cols = erised::StandardDeviation(this->mat_, erised::Axis::COL, num_elems_cols);
  std::vector<float> vtest_cols = {0.0952628, 0, 0, 0.25, 0, 0, 0.588784, 0.3, 0};;

  for (int i = 0; i < vec_cols.size(); i++)
    ASSERT_NEAR(vec_cols[i], vtest_cols[i], 0.01);
}

TYPED_TEST_P(BasicStatisticTest, StandardizationRows) {
  auto mat2 = this->mat_;

  auto num_elems_rows = this->mat_.NumElementsLines();
  erised::Standardization(&mat2, erised::Axis::ROW, num_elems_rows);

  auto v = mat2(0,0);
  ASSERT_FLOAT_EQ(-1, v);

  v = mat2(0,1);
  ASSERT_FLOAT_EQ(1, v);

  v = mat2(2,0);
  ASSERT_NEAR(-1.111167, v, 0.01);

  v = mat2(5,4);
  ASSERT_NEAR(1.31954935115, v, 0.01);
}

TYPED_TEST_P(BasicStatisticTest, StandardizationCols) {
  auto mat2 = this->mat_;

  auto num_elems_cols = this->mat_.NumElementsCols();
  erised::Standardization(&mat2, erised::Axis::COL, num_elems_cols);

  auto v = mat2(0,0);
  ASSERT_NEAR(1.73205070605, v, 0.01);

  v = mat2(1,6);
  ASSERT_NEAR(1.35873257425, v, 0.01);
}

TYPED_TEST_P(BasicStatisticTest, RescalingRows) {
  auto mat2 = this->mat_;

  auto num_elems_rows = this->mat_.NumElementsLines();
  erised::Rescaling(&mat2, erised::Axis::ROW, num_elems_rows);

  auto v = mat2(0,0);
  ASSERT_FLOAT_EQ(0, v);

  v = mat2(0,1);
  ASSERT_FLOAT_EQ(1, v);

  v = mat2(2,4);
  ASSERT_NEAR(0.375, v, 0.01);

  v = mat2(5,6);
  ASSERT_NEAR(0.666666, v, 0.01);
}

TYPED_TEST_P(BasicStatisticTest, RescalingCols) {
  typedef decltype(this->mat_) TYPE;

  TYPE mat = {{0.44, 0.8, 0,   0,   0,   0,   0,   0,   0  },
              {0,    0.8, 1.4, 0,   0,   0.2, 1.7, 0,   0  },
              {0.22, 0,   0,   0.7, 0.4, 0,   0,   0,   0.4},
              {0.22, 0.2, 1.2, 0,   0,   0.7, 0.7, 0.7, 0  },
              {0,    0,   0,   0.2, 0.3, 0,   0,   0,   0.7},
              {0.22, 0,   0,   0,   0.4, 0,   0.3, 0.1, 0  }};

  auto num_elems_cols = mat.NumElementsCols();
  erised::Rescaling(&mat, erised::Axis::COL, num_elems_cols);

  auto v = mat(0,0);
  ASSERT_FLOAT_EQ(1, v);

  v = mat(2,3);
  ASSERT_FLOAT_EQ(1, v);

  v = mat(1,6);
  ASSERT_NEAR(1, v, 0.01);

  v = mat(3,6);
  ASSERT_NEAR(0.285, v, 0.01);
}

REGISTER_TYPED_TEST_CASE_P(BasicStatisticTest,
                           Avarage,
                           Variance,
                           StandardDeviation,
                           StandardizationRows,
                           StandardizationCols,
                           RescalingRows,
                           RescalingCols
                          );

typedef ::testing::Types<erised::DataCsrMap<float>> Types;
INSTANTIATE_TYPED_TEST_CASE_P(My, BasicStatisticTest, Types);

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
