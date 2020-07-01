#ifndef DR_TEST_D_MATRIX_ARGS_H
#define DR_TEST_D_MATRIX_ARGS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////

// Give the wrong number of tests to the d-matrix processing
// function and check if it returns the expected error.
// Returns true if the test passed; false otherwise.
bool test_d_matrix_wrong_num_tests(void);

// Give the wrong number of failure modes to the d-matrix processing
// function and check if it returns the expected error.
// Returns true if the test passed; false otherwise.
bool test_d_matrix_wrong_num_failure_modes(void);

// Give invalid test results to the d-matrix processing
// function and check if it returns the expected error.
// Returns true if the test passed; false otherwise.
bool test_d_matrix_invalid_test_results(void);
    
#ifdef __cplusplus
}  // extern "C" {
#endif
  

#endif // DR_TEST_D_MATRIX_ARGS_H


