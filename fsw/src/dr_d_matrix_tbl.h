#ifndef DR_D_MATRIX_TBL_H
#define DR_D_MATRIX_TBL_H

#include <stdbool.h>
#include <stdint.h>

#include "dr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// A type to hold information for a dependency matrix (D-matrix). 
typedef struct
{
  /// The actual number tests used by this D-matrix.
  uint32_t num_tests;

  /// The actual number of failure modes used by this D-matrix.
  uint32_t num_failure_modes;

  /// The D-matrix itself. A 2-D array of boolean values that maps
  /// the test outcomes into failures. The format is shown below.
  //                 test1   test2
  //                ---------------
  // failure_mode1  |  1   |  0   |
  // failure_mode2  |  1   |  1   |
  //                ---------------
  // However, because the C language is column-major order, the first
  // subscript will correspond to failure modes and the second to tests.
  bool d_matrix[DR_MAX_FAILURE_MODES][DR_MAX_TESTS];

} dr_d_matrix_tbl_type;

#ifdef __cplusplus
} // extern "C" {
#endif


#endif // DR_D_MATRIX_TBL_H
