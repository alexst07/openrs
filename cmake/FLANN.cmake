include(ExternalProject)

set(URL_FLANN "https://github.com/alexst07/flann.git"
    CACHE STRING "Full path or URL of gmock source")

find_package(HDF5 REQUIRED)

ExternalProject_Add(gtest_ext
        GIT_REPOSITORY "${URL_FLANN}"
        GIT_TAG "fix_build"
        BINARY_DIR "${CMAKE_BINARY_DIR}/third-party/flann-build"
        SOURCE_DIR "${CMAKE_BINARY_DIR}/third-party/flann-src"
        INSTALL_COMMAND ""
        CMAKE_ARGS "${gtest_cmake_args}"
          "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DFLANN_LIB_INSTALL_DIR=/usr/local/lib"
)

set(FLANN_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/third-party/flann-src/src/cpp/")
set(FLANN_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/third-party/flann-build/lib/")
link_directories(${FLANN_LIBRARY_DIRS})

include_directories(${HDF5_INCLUDE_DIRS})

function(dep_flann name)
    target_link_libraries(${name} flann flann_cpp ${HDF5_LIBRARIES})
    set_property(TARGET ${name} APPEND PROPERTY INCLUDE_DIRECTORIES
                 ${FLANN_INCLUDE_DIRS})
    add_dependencies(${name} flann_cpp)
endfunction()
