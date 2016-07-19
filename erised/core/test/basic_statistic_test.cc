#include <gtest/gtest.h>
#include <iostream>

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

REGISTER_TYPED_TEST_CASE_P(BasicStatisticTest,
                           Avarage,
                           Variance
                          );

typedef ::testing::Types<erised::DataCsrMap<float>> Types;
INSTANTIATE_TYPED_TEST_CASE_P(My, BasicStatisticTest, Types);

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
