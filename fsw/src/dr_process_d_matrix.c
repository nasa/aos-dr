
#include "dr_process_d_matrix.h"

////////////////////////////////////////////////////////////////
// Private function prototypes
////////////////////////////////////////////////////////////////

/// Finds the failure modes affected by a test and marks them as
/// good, suspect, or unknown according to the test result and
/// the current estimated failure modes.
/// @param [in] test_index The index of the test to process
/// @param [in] test_result The test result corresponding to the given test_index
/// @param [in] d_matrix_tbl Pointer to the d-matrix
/// @param [inout] failure_modes Array of failure modes, modified in the course of processing this test
static void process_single_test (uint32_t const test_index,
                                 dr_test_result_type const test_result,
                                 dr_d_matrix_tbl_type const * const
                                 d_matrix_tbl,
                                 dr_failure_mode_type failure_modes[]);

/// Sets a single failure mode depending on the test value and the
/// current failure mode value.
/// @param [in] test_result The test result associated with the failure mode
/// @param [inout] failure_mode A single failure mode associated with the test_result, whose resulting value will be set based on the test_result and the failure_mode's current value.
static void set_failure_mode (dr_test_result_type const test_result,
                              dr_failure_mode_type * const failure_mode);

/// Checks to see if a given suspect failure mode may be marked as bad. This
/// should only be called for suspect failure modes, and
/// must be done after the initial pass to find good and suspects.
/// @WARNING! Also be sure that the index passed in corresponds only
/// to a suspect failure mode.
/// @param [in] suspect_failure_mode_index The index of the suspect failure mode to check
/// @param [in] test_results The full array of test results
/// @param [in] d_matrix_tbl Pointer to the d-matrix
/// @param [inout] failure_modes The array of failure modes determined from the first pass; will be modified if any failure modes may be marked as bad.
static void check_suspect_for_bad (uint32_t const suspect_failure_mode_index,
                                   dr_test_result_type const test_results[],
                                   dr_d_matrix_tbl_type const * const d_matrix_tbl,
                                   dr_failure_mode_type failure_modes[]);

/////////////////////////////////////////////////////////////////
// Public function definitions
/////////////////////////////////////////////////////////////////

dr_error_type dr_process_d_matrix(dr_d_matrix_tbl_type const * const d_matrix_tbl,
                                  uint32_t const num_tests,
                                  dr_test_result_type const test_results[num_tests],
                                  uint32_t const num_failure_modes,
                                  dr_failure_mode_type failure_modes[num_failure_modes])
{

  dr_error_type error = DR_ERROR_NO_ERROR;

  // Check the number of tests given
  if (num_tests != d_matrix_tbl->num_tests)
  {
    error = DR_ERROR_WRONG_NUM_TESTS;
  }
  // Check the number of failure modes given
  else if (num_failure_modes != d_matrix_tbl->num_failure_modes)
  {
    error = DR_ERROR_WRONG_NUM_FAILURE_MODES;
  }
  else
  {
    // Check that all of the given test results have valid values
    for (uint32_t i = 0; i < num_tests; ++i)
    {
      switch (test_results[i])
      {
        case DR_TEST_RESULT_PASS:
        case DR_TEST_RESULT_FAIL:
        case DR_TEST_RESULT_UNKNOWN:
          break;

        case DR_TEST_RESULT_COUNT:
        default:
          // If the test results are not valid, set the error to indicate such
          error = DR_ERROR_INVALID_TEST_RESULT;
          break;
      }

      if (DR_ERROR_NO_ERROR != error)
      {
        break;
      }
    }

    if (DR_ERROR_NO_ERROR == error)
    {
      // Finally get to doing the work of the function.

      // Initialize the array of failure modes
      for (uint32_t i = 0; i < num_failure_modes; ++i)
      {
        failure_modes[i] = DR_FAILURE_MODE_UNKNOWN;
      }

      // Step through the set of tests to process them all, will find the
      // good, suspect, and unknown failure modes
      for (uint32_t i = 0; i < num_tests; ++i)
      {
        process_single_test(i, test_results[i], d_matrix_tbl,
                            failure_modes);
      }

      // Check if any of the suspect failure modes is actually bad, if so then
      // mark it as such in the failure modes array.
      for (uint32_t i = 0; i < num_failure_modes; ++i)
      {
        if (DR_FAILURE_MODE_SUSPECT == failure_modes[i])
        {
          check_suspect_for_bad(i, test_results,
                                d_matrix_tbl, failure_modes);
        }

      }
    }
  }

  return error;

}

////////////////////////////////////////////////////////////////
// Private function definitions
////////////////////////////////////////////////////////////////

void process_single_test(uint32_t const test_index,
                         dr_test_result_type const test_result,
                         dr_d_matrix_tbl_type const * const d_matrix_tbl,
                         dr_failure_mode_type failure_modes[])
{

  for (uint32_t i = 0; i < d_matrix_tbl->num_failure_modes; ++i)
  {
    bool entry = d_matrix_tbl->d_matrix[i][test_index];

    if (entry)
    {
      set_failure_mode(test_result, &(failure_modes[i]));
    }
  }

}

void set_failure_mode(dr_test_result_type const test_result,
                      dr_failure_mode_type * const failure_mode)
{
  switch (test_result)
  {
    case DR_TEST_RESULT_PASS:
      // A pass test result will always set the failure mode to good,
      // regardless of current mode. This may exonerate some failure modes
      // that were previously marked bad or suspect.
      *failure_mode = DR_FAILURE_MODE_GOOD;
      break;

    case DR_TEST_RESULT_FAIL:

      // A fail result will set the failure mode to suspect
      // if there was no other test that previously exonerated it.
      if (*failure_mode != DR_FAILURE_MODE_GOOD)
      {
        *failure_mode = DR_FAILURE_MODE_SUSPECT;
      }
      break;

    case DR_TEST_RESULT_UNKNOWN:
    case DR_TEST_RESULT_COUNT:
    default:
      // Any of these remaining cases will not change the mode of the
      // failure mode, since the failure modes were initialized to unknown.
      break;
  }
}

void check_suspect_for_bad(uint32_t const suspect_failure_mode_index,
                           dr_test_result_type const test_results[],
                           dr_d_matrix_tbl_type const * const d_matrix_tbl,
                           dr_failure_mode_type failure_modes[])
{

  // For a suspect failure mode to be marked as bad, we look for the first test
  // that would implicate only it. That is, the test must have failed,
  // and it must implicate no other failure modes that remained suspect.
  // Perhaps that failed test implicates no other failure modes, or the
  // only other failure modes it implicated were exonerated by other
  // passing tests (in which case those failure modes are "good" at this
  // point).
  for (uint32_t j = 0; j < d_matrix_tbl->num_tests; ++j)
  {
    if ( (d_matrix_tbl->d_matrix[suspect_failure_mode_index][j]) &&
         (DR_TEST_RESULT_FAIL == test_results[j]) )
    {
      // we have now found a test which implicates this failure mode
      // and has failed. Find how many failure modes were implicated by
      // this test AND remain suspect or have been marked bad.
      uint32_t num_implicated_suspects_and_bads = 0;

      for (uint32_t k = 0; k < d_matrix_tbl->num_failure_modes; ++k)
      {
        if ( (d_matrix_tbl->d_matrix[k][j]) &&
             ( (DR_FAILURE_MODE_SUSPECT == failure_modes[k]) ||
               (DR_FAILURE_MODE_BAD == failure_modes[k]) ) )
        {
          num_implicated_suspects_and_bads++;
        }
      }

      // If the number of implicated failure modes that are still suspect
      // or bad is equal to 1, then only this (suspect) failure mode was
      // counted. This failure mode may now be marked as bad.
      // This is enough to mark it as bad, so break at this point,
      // don't continue to check all of the other tests.
      if (1 == num_implicated_suspects_and_bads)
      {
        failure_modes[suspect_failure_mode_index] = DR_FAILURE_MODE_BAD;
        break;
      }

    }
  }
}
