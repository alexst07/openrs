cmake_minimum_required (VERSION 2.8)
project(erised C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(cmake/ErisedUtils.cmake)
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#
# FLANN support to calculates fast knn
#
include(cmake/FLANN.cmake)
include_directories(${FLANN_INCLUDE_DIRS})

#
# Boost support
#
find_package(Boost 1.58 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})


if (WITH_TBB)
  find_package(TBB REQUIRED)
  set (HAVE_TBB ON)
endif ()

if (WITH_CUDA)
  find_package(CUDA REQUIRED)
  set (HAVE_CUDA ON)
  # TODO: improve cuda support
  set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -Xcompiler -fPIC; -O3; -ccbin /usr/bin/g++ -std=c++11)
endif ()

if (BUILD_TESTS)
  enable_testing()
  include(cmake/GTest.cmake)
  include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})
endif ()

configure_file(${CMAKE_SOURCE_DIR}/cmake/templates/config.h.in
               ${PROJECT_BINARY_DIR}/include/config.h)

include_directories(${PROJECT_BINARY_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/erised/core/src/)

add_subdirectory(erised)

ERSD_OPTION(WITH_TBB            "Include Intel TBB support"                   ON)
ERSD_OPTION(WITH_CUDA           "Include NVidia Cuda Runtime support"         OFF)
ERSD_OPTION(BUILD_TESTS         "Build all tests"                             ON)

