#ifndef DR_TEST_D_MATRIX_SWAPS_H
#define DR_TEST_D_MATRIX_SWAPS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////

// For many randomly-generated d-matrices and test
// results, swaps 2 columns of a d-matrix and the
// associated test results and checks that the swapped
// versions give the same failure modes as the original.
// Returns true if the test passed; false otherwise.
bool test_d_matrix_swap_column_order(void);

  
// For many randomly-generated d-matrices and test
// results, swaps 2 rows of a d-matrix and checks that
// the swapped versions give the same corresponding failure
// modes as the original.
// Returns true if the test passed; false otherwise.
bool test_d_matrix_swap_row_order(void);

#ifdef __cplusplus
}  // extern "C" {
#endif
  

#endif // DR_TEST_D_MATRIX_SWAPS_H
