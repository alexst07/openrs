#include "data_csr.h"

#include <gtest/gtest.h>
#include <iostream>

TEST(Sample, Sampleunit) {
  using namespace erised;

  DataCsr<float> data{{0.44, 0.8, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0.8, 1.4, 0, 0, 0, 0, 0, 0},
                      {0.22, 0, 0, 0.7, 0.4, 0, 0, 0, 0},
                      {0.22, 0, 0, 0, 0, 0.7, 0.7, 0.7, 0},
                      {0, 0, 0, 0, 0.4, 0, 0, 0, 0.7},
                      {0.22, 0, 0, 0, 0.4, 0, 0, 0, 0}};

  std::cout << data;
}
