#include <gtest/gtest.h>

#include <iostream>
#include "cuda_csr.h"

TEST(Sample, Sampleunit) {
  using namespace erised;

  cuda::GpuCsr<float> data{{0.44, 0.8, 0,   0,   0,   0,   0,   0,   0  },
        	           {0,    0.8, 1.4, 0,   0,   0,   0,   0,   0  },
                	   {0.22, 0,   0,   0.7, 0.4, 0,   0,   0,   0  },
	                   {0.22, 0,   0,   0,   0,   0.7, 0.7, 0.7, 0  },
        	           {0,    0,   0,   0,   0.4, 0,   0,   0,   0.7},
                	   {0.22, 0,   0,   0,   0.4, 0,   0,   0,   0  }};

  std::cout << data;
  // TODO: ON the future this feature will be supported by CUDA8
  //data.Map([] __device__ __host__(float v) -> float { return 2*v; });

  std::cout << data;
}

