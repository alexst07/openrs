#include <gtest/gtest.h>
#include <iostream>

#include "traits.h"

struct X {
  bool continuos;
};

struct W {
  bool continusos;
};

TEST(Sample, Sampleunit) {
  std::cout << erised::is_continuos<char[]>::value << std::endl;
//   std::cout << erised::is_continuos<char>::value << std::endl;

  std::cout << erised::is_continuos<X>::value << std::endl;
  std::cout << erised::is_continuos<W>::value << std::endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
