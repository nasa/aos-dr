#ifndef DR_SAVE_RESULTS_H
#define DR_SAVE_RESULTS_H

#include "dr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////

/// Open the csv files 
dr_error_type dr_open_results_files(
  char const * const test_results_filename,
  char const * const failure_modes_filename);
  
// Close the csv files
dr_error_type dr_close_results_files(void);
  
  
///
// Saves the diagnosis results to csv files. Will use the OSAL
// file management functions.
//
dr_error_type dr_save_results(
  int const iteration,
  dr_error_type const error,
  int const num_tests,
  dr_test_result_type const test_results[num_tests],
  int const num_failure_modes,
  dr_failure_mode_type const failure_modes[num_failure_modes]
  );
  
#ifdef __cplusplus
} // extern "C" {
#endif

#endif // DR_SAVE_RESULTS_H
