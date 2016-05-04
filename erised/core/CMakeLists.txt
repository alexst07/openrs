include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

if (ERISED_BUILD_TESTS)
  file(GLOB SOURCES_TEST ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc)
  foreach(local_file ${SOURCES_TEST})
    get_filename_component(local_filename ${local_file} NAME_WE)
    message("Compiling: ${local_file} ${SOURCES}")
    add_executable(${local_filename} ${local_file} ${SOURCES})
    target_link_libraries(${local_filename} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
  endforeach()
endif ()
