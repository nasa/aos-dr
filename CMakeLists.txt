cmake_minimum_required(VERSION 2.6.4)
project(CFE_DR C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${MISSION_SOURCE_DIR}/apps/lc/fsw/platform_inc )
include_directories(${MISSION_SOURCE_DIR}/apps/lc/fsw/src )

#aux_source_directory(fsw/src APP_SRC_FILES)
set(APP_SRC_FILES
  fsw/src/dr_app.c
  fsw/src/dr_process_tests.c
  fsw/src/dr_process_d_matrix.c
  fsw/src/dr_print_results.c
  fsw/src/dr_save_results.c
)

# Create the app module
add_cfe_app(dr ${APP_SRC_FILES})

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

# Add the "CFS Table" source files.
#
# Table files frequently want to use headers that are
# private to this app, kept in fsw/src. Add that directory
# to the header search path.
include_directories(fsw/src)

add_cfe_tables(dr
  fsw/tables/dr_d_matrix_ex.c
  fsw/tables/dr_wtm_example.c
  fsw/tables/dr_mode_def.c
  fsw/tables/lc_def_wdt_ex.c
  )
