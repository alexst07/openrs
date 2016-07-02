#include <gtest/gtest.h>
#include <iostream>

#include "sim_mat.h"

TEST(Sample, Sampleunit) {
  std::cout << "teste\n";
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
