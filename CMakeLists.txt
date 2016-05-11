cmake_minimum_required (VERSION 2.8)
project(erised C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(cmake/ErisedUtils.cmake)
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

ERSD_OPTION(WITH_TBB            "Include Intel TBB support"                   ON)
ERSD_OPTION(WITH_CUDA           "Include NVidia Cuda Runtime support"         OFF)
ERSD_OPTION(BUILD_TESTS         "Build all tests"                             ON)

if (WITH_TBB)
  find_package(TBB REQUIRED)
  set (HAVE_TBB ON)
endif ()

if (WITH_CUDA)
  find_package(CUDA REQUIRED)
  set (HAVE_CUDA ON)
  set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -Xcompiler -fPIC; -O3; -gencode arch=compute_32,code=sm_32; -ccbin /usr/bin/g++ -std=c++11)
endif ()

if (BUILD_TESTS)
  enable_testing()
  find_package(GTest REQUIRED)
  include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})
endif ()

configure_file(${CMAKE_SOURCE_DIR}/cmake/templates/config.h.in
               ${PROJECT_BINARY_DIR}/include/config.h)

include_directories(${PROJECT_BINARY_DIR}/include/)

add_subdirectory(erised)
