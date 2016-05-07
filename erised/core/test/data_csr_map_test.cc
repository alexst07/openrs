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

  data.Map([](float v) -> float { return 2*v; });

  data.RowMap(3, [](float v) -> float { return 8*v; });

  data.ColMap(4, [](float v) -> float { return 10*v; });
  std::cout << data;
}
