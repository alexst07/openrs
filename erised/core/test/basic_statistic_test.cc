#include "data_base.h"
#include "data_csr_map.h"
#include "basic_statistic.h"

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

  float res = Avarage(data);
  float res_l = Avarage(data, 2, Axis::ROW);
  float res_c = Avarage(data, 0, Axis::COL);

  std::cout << "Avarage: " << res << "\n"
            << "Avarage row 2: " << res_l << "\n"
            << "Avarage col 0: " << res_c << "\n";
}
