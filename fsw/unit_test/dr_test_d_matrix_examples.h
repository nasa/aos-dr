#ifndef DR_TEST_D_MATRIX_EXAMPLES_H
#define DR_TEST_D_MATRIX_EXAMPLES_H

#include <stdbool.h>

#include "dr_d_matrix_tbl.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////
// Constants
//////////////////////////////////////////////
#define DR_TEST_EXAMPLE_NUM_TESTS 4
#define DR_TEST_EXAMPLE_NUM_FAILURE_MODES 5

//////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////

// Runs the first example given in the d-matrix
// algorithm solving description, at
// https://babelfish.arc.nasa.gov/confluence/display/AOS/DR+D-matrix+Solving+Algorithm+Description
// Returns true if the test passed; false otherwise.
bool test_d_matrix_alg_example_1(void);

// Runs the second example given in the d-matrix
// algorithm solving description, at
// https://babelfish.arc.nasa.gov/confluence/display/AOS/DR+D-matrix+Solving+Algorithm+Description
// Returns true if the test passed; false otherwise.
bool test_d_matrix_alg_example_2(void);

// Runs a test case that we found which we think shows a
// bug in DR
bool test_d_matrix_bug_001(void);

// Function which initializes a d-matrix, which I realized would
// be useful for at least a couple of other tests
void initialize_example_d_matrix(dr_d_matrix_tbl_type * const d_matrix_tbl);

  
#ifdef __cplusplus
}  // extern "C" {
#endif
  

#endif // DR_TEST_D_MATRIX_EXAMPLES_H
