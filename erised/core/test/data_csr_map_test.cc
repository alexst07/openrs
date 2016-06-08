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

  std::cout << "\nMin elements col 0: " << data.Min(0, Axis::COL) << "\n";
  std::cout << "\nMin elements row 2: " << data.Min(2, Axis::ROW) << "\n";

  float row_red = data.RowReduce(5, [](float a, float b) {
    return 2*a + b;
  });

  std::cout << "row 5 reduce: " << row_red << "\n";

  float col_red = data.ColReduce(5, [](float a, float b) -> float {
    float c = 3*a + b;
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

  std::cout << "\n(2,3): " << data({2,3}) << "\n";
  std::cout << "\n(1,1): " << data(1,1) << "\n";
  std::cout << "\n(2,0): " << data(2,2) << "\n";

  std::cout << "\nNum elements: " << data.NumElements() << "\n";
  std::cout << "\nNum elements line 3: " << data.NumElementsLine(3) << "\n";
  std::cout << "\nNum elements col 4: " << data.NumElementsCol(4) << "\n";
}
