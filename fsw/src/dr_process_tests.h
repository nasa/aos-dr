
#ifndef DR_PROCESS_TESTS_H
#define DR_PROCESS_TESTS_H

#include "cfe.h"

#include "dr_types.h"
#include "dr_wtm_tbl.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////

/// Main public function, to determine the test results. As described
/// elsewhere, this will read the Limit Checker (LC) watchpoint results
/// table and translate those into the test results. 
int32 dr_process_tests(CFE_TBL_Handle_t const DR_LC_WRTHandle,
		       dr_wtm_entry_type const * const dr_wtm_ptr,
		       uint32_t const num_tests,
		       dr_test_result_type test_results[num_tests]);
  
#ifdef __cplusplus
} // extern "C" {
#endif

  
#endif // DR_PROCESS_TESTS_H 
