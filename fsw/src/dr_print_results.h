#ifndef DR_PRINT_RESULTS_H
#define DR_PRINT_RESULTS_H

#include "dr_types.h"
#include "dr_d_matrix_tbl.h"

#ifdef __cplusplus
extern "C" {
#endif

void dr_print_results(dr_error_type const error,
  dr_d_matrix_tbl_type const * const d_matrix_tbl,
  int const num_tests,
  dr_test_result_type const test_results[num_tests],
  int const num_failure_modes,
  dr_failure_mode_type const failure_modes[num_failure_modes]);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // DR_PRINT_RESULTS_H
