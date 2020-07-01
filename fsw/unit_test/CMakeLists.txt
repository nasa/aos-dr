
cmake_minimum_required(VERSION 2.8)

project(dr_unit_test)

set(DR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_definitions(-DDR_UNIT_TEST)

include_directories(
  ${DR_SOURCE_DIR}
)
  
set(SOURCES
  dr_unit_test_main.c
  dr_test_d_matrix_swaps.c
  dr_test_d_matrix_examples.c
  dr_test_d_matrix_args.c
  ${DR_SOURCE_DIR}/dr_process_d_matrix.c
  ${DR_SOURCE_DIR}/dr_print_results.c
)

#
# Test if we are using gcc, and if so, add the appropriate flags.
# Note, there are also strings defined to indicate MS, Intel and Clang
# compilers, they just aren't relevant to our project.
#
if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  # We will use C99 - c'mon people, it's been 17 years
  # Also set a couple of other options
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wswitch-enum")
endif()


add_executable(dr_unit_test ${SOURCES} )

if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries(dr_unit_test m)
endif()  