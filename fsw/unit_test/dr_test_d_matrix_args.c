#include "dr_test_d_matrix_args.h"

#include "dr_process_d_matrix.h"
#include "dr_test_d_matrix_examples.h"

////////////////////////////////////////////
// Constants
///////////////////////////////////////////

// Define a constant for the wrong number of test results
#define DR_TEST_WRONG_NUM_TESTS (DR_TEST_EXAMPLE_NUM_TESTS + 2)
#define DR_TEST_WRONG_NUM_FAILURE_MODES (DR_TEST_EXAMPLE_NUM_FAILURE_MODES + 3)

bool test_d_matrix_wrong_num_tests(void)
{
  bool test_passed = true;

  // the example uses DR_TEST_EXAMPLE_NUM_TESTS. We will create a
  // different number of tests 
  dr_d_matrix_tbl_type d_matrix_tbl;
  initialize_example_d_matrix(&d_matrix_tbl);

  // We hopefully wouldn't use these test results but need to fill the array
  dr_test_result_type const test_results[DR_TEST_WRONG_NUM_TESTS] =
    {
      DR_TEST_RESULT_PASS,
      DR_TEST_RESULT_PASS,
      DR_TEST_RESULT_FAIL,
      DR_TEST_RESULT_PASS,
      DR_TEST_RESULT_FAIL,
      DR_TEST_RESULT_PASS
    };

  // Process this d-matrix and see if we get the error we're expecting,
  // set the overall test to passed if we correctly got the error of
  // an incorrect number of tests
  dr_failure_mode_type failure_modes[DR_TEST_EXAMPLE_NUM_FAILURE_MODES];
  dr_error_type error =
    dr_process_d_matrix(&d_matrix_tbl,
			DR_TEST_WRONG_NUM_TESTS, test_results,
			DR_TEST_EXAMPLE_NUM_FAILURE_MODES, failure_modes);
  

  test_passed = (DR_ERROR_WRONG_NUM_TESTS == error);
    
  return test_passed;
}

bool test_d_matrix_wrong_num_failure_modes(void)
{
  bool test_passed = true;

  // the example uses DR_TEST_EXAMPLE_NUM_FAILURE_MODES. We will create a
  // different number of failure modes and see if the error is caught correctly.
  dr_d_matrix_tbl_type d_matrix_tbl;
  initialize_example_d_matrix(&d_matrix_tbl);

  // We hopefully wouldn't use these test results but need to fill the array
  dr_test_result_type const test_results[DR_TEST_EXAMPLE_NUM_TESTS] =
    {
      DR_TEST_RESULT_PASS,
      DR_TEST_RESULT_PASS,
      DR_TEST_RESULT_FAIL,
      DR_TEST_RESULT_PASS
    };

  // Process this d-matrix and see if we get the error we're expecting,
  // set the overall test to passed if we correctly got the error of
  // an incorrect number of tests
  dr_failure_mode_type failure_modes[DR_TEST_WRONG_NUM_FAILURE_MODES];
  dr_error_type error =
    dr_process_d_matrix(&d_matrix_tbl,
			DR_TEST_EXAMPLE_NUM_TESTS, test_results,
			DR_TEST_WRONG_NUM_FAILURE_MODES, failure_modes);
  

  test_passed = (DR_ERROR_WRONG_NUM_FAILURE_MODES == error);
    
  return test_passed;

}

bool test_d_matrix_invalid_test_results(void)
{
  bool test_passed = true;

  dr_d_matrix_tbl_type d_matrix_tbl;
  initialize_example_d_matrix(&d_matrix_tbl);

  // We will initialize a couple of test results to invalid values
  dr_test_result_type const test_results[DR_TEST_EXAMPLE_NUM_TESTS] =
    {
      DR_TEST_RESULT_PASS,
      255,
      DR_TEST_RESULT_FAIL,
      131
    };

  // Process this d-matrix and see if we get the error we're expecting,
  // set the overall test to passed if we correctly got the error of
  // an invalid test result
  dr_failure_mode_type failure_modes[DR_TEST_EXAMPLE_NUM_FAILURE_MODES];
  dr_error_type error =
    dr_process_d_matrix(&d_matrix_tbl,
			DR_TEST_EXAMPLE_NUM_TESTS, test_results,
			DR_TEST_EXAMPLE_NUM_FAILURE_MODES, failure_modes);
  

  test_passed = (DR_ERROR_INVALID_TEST_RESULT == error);
    
  return test_passed;

}
