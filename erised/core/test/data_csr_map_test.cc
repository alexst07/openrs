#include "data_csr_map.h"

#include <gtest/gtest.h>
#include <iostream>

TEST(Sample, Sampleunit) {
  using namespace erised;

  DataCsrMap<float> data{{0.44, 0.8, 0,   0,   0,   0,   0,   0,   0  },
                         {0,    0.8, 1.4, 0,   0,   0,   0,   0,   0  },
                         {0.22, 0,   0,   0.7, 0.4, 0,   0,   0,   0  },
                         {0.22, 0,   0,   0,   0,   0.7, 0.7, 0.7, 0  },
                         {0,    0,   0,   0,   0.4, 0,   0,   0,   0.7},
                         {0.22, 0,   0,   0,   0.4, 0,   0,   0,   0  }};

  float row_red = data.RowReduce(5, [](float a, float b) {
    return a + b;
  });

  std::cout << "row 5 reduce: " << row_red << "\n";

  float col_red = data.ColReduce(5, [](float a, float b) -> float {
    float c = a + b;
    return c;
  });

  std::cout << "col 5 reduce: " << col_red << "\n";

  data.Map([](float v) -> float { return 2*v; });

  data.RowMap(3, [](float v) -> float { return 8*v; });

  data.ColMap(4, [](float v) -> float { return 10*v; });
  std::cout << data;

  float r = data.Reduce([](float a, float b) {
    return a + b;
  });

  std::cout << "\nReduce: " << r << "\n";
}
