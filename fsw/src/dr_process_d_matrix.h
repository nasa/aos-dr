#ifndef DR_PROCESS_D_MATRIX_H
#define DR_PROCESS_D_MATRIX_H

#include "dr_types.h"
#include "dr_d_matrix_tbl.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Main public function, to process the tests according to the given
/// d-matrix and test_results, and fill the given array with the
/// calculated failure modes (the last argument).
/// @param [in] d_matrix_tbl The d-matrix to solve
/// @param [in] num_tests  The number of tests in the test results array
/// @param [in] test_results The given test results
/// @param [in] num_failure_modes The number of failure modes in the failure_modes array
/// @param [out] failure_modes The returned failure modes from processing the d-matrix
dr_error_type dr_process_d_matrix(dr_d_matrix_tbl_type const * const d_matrix_tbl,
                                  uint32_t const num_tests,
                                  dr_test_result_type const test_results[num_tests],
                                  uint32_t const num_failure_modes,
                                  dr_failure_mode_type failure_modes[num_failure_modes]);

#ifdef __cplusplus
} // extern "C" {
#endif


#endif // DR_PROCESS_D_MATRIX_H
