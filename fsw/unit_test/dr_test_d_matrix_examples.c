#include "dr_test_d_matrix_examples.h"

#include <stdio.h>
#include <stddef.h>

#include "dr_process_d_matrix.h"
#include "dr_print_results.h"

///////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////

#define BUG_001_NUM_TESTS 2
#define BUG_001_NUM_FAILURE_MODES 2

///////////////////////////////////////////////////////
// Private function declarations
//////////////////////////////////////////////////////
static bool are_failure_mode_arrays_equal(size_t const num_failure_modes,
        dr_failure_mode_type const failure_modes_1[num_failure_modes],
	dr_failure_mode_type const failure_modes_2[num_failure_modes]);


///////////////////////////////////////////////////////
// Public function definitions
//////////////////////////////////////////////////////
bool test_d_matrix_alg_example_1(void)
{
  bool test_passed = true;

  dr_d_matrix_tbl_type d_matrix_tbl;
  initialize_example_d_matrix(&d_matrix_tbl);

  // Example 1 had these test results
  dr_test_result_type const test_results[DR_TEST_EXAMPLE_NUM_TESTS] =
    {
      DR_TEST_RESULT_PASS,
      DR_TEST_RESULT_PASS,
      DR_TEST_RESULT_FAIL,
      DR_TEST_RESULT_PASS
    };
  
  // And by hand we got these expected failure modes
  dr_failure_mode_type expected_failure_modes[DR_TEST_EXAMPLE_NUM_FAILURE_MODES] =
    {
      DR_FAILURE_MODE_GOOD,
      DR_FAILURE_MODE_GOOD,
      DR_FAILURE_MODE_GOOD,
      DR_FAILURE_MODE_GOOD,
      DR_FAILURE_MODE_BAD
    };

  // Process this d-matrix and see what we actually get
  dr_failure_mode_type actual_failure_modes[DR_TEST_EXAMPLE_NUM_FAILURE_MODES];
  dr_error_type error =
    dr_process_d_matrix(&d_matrix_tbl,
			d_matrix_tbl.num_tests, test_results,
			d_matrix_tbl.num_failure_modes, actual_failure_modes);

  if(DR_ERROR_NO_ERROR != error)
  {
    return false;
  }

  test_passed = are_failure_mode_arrays_equal(DR_TEST_EXAMPLE_NUM_FAILURE_MODES,
					      expected_failure_modes,
					      actual_failure_modes);
  
  return test_passed;
}
  
bool test_d_matrix_alg_example_2(void)
{
  bool test_passed = true;
  
  dr_d_matrix_tbl_type d_matrix_tbl;
  initialize_example_d_matrix(&d_matrix_tbl); 

  // Example 2 had these test results
  dr_test_result_type const test_results[DR_TEST_EXAMPLE_NUM_TESTS] =
    {
      DR_TEST_RESULT_UNKNOWN,
      DR_TEST_RESULT_FAIL,
      DR_TEST_RESULT_FAIL,
      DR_TEST_RESULT_PASS
    };
  
  // And by hand we got these expected failure modes
  dr_failure_mode_type expected_failure_modes[DR_TEST_EXAMPLE_NUM_FAILURE_MODES] =
    {
      DR_FAILURE_MODE_GOOD,
      DR_FAILURE_MODE_SUSPECT,
      DR_FAILURE_MODE_SUSPECT,
      DR_FAILURE_MODE_GOOD,
      DR_FAILURE_MODE_SUSPECT
    };

  // Process this d-matrix and see what we actually get
  dr_failure_mode_type actual_failure_modes[DR_TEST_EXAMPLE_NUM_FAILURE_MODES];
  dr_error_type error =
    dr_process_d_matrix(&d_matrix_tbl,
			d_matrix_tbl.num_tests, test_results,
			d_matrix_tbl.num_failure_modes, actual_failure_modes);

  if(DR_ERROR_NO_ERROR != error)
  {
    return false;
  }

  test_passed = are_failure_mode_arrays_equal(DR_TEST_EXAMPLE_NUM_FAILURE_MODES,
					      expected_failure_modes,
					      actual_failure_modes);
  
  return test_passed;

}

// Bug 001, as I'm calling it, was found by the random row swap test failing.
// This was a quite simple test case that exhibited the problem. It turned
// out to be in the part of the d-matrix solving that checked for BAD.
// Originally, the code was finding all of the failure modes related to a 
// failing test, but only counting them if the related tests were SUSPECT. It
// turned out that they needed to be counted if they were either SUSPECT or
// already BAD. 
bool test_d_matrix_bug_001(void)
{

  bool test_passed = true;
  
  dr_d_matrix_tbl_type const d_matrix_tbl = 
    {
      .num_tests = BUG_001_NUM_TESTS,
      .num_failure_modes = BUG_001_NUM_FAILURE_MODES,
      .d_matrix = {
	{ 1, 1 },
	{ 0, 1 } }
    };

  
  // Bug 001 had these test results
  dr_test_result_type const test_results[BUG_001_NUM_TESTS] =
    {
      DR_TEST_RESULT_FAIL,
      DR_TEST_RESULT_FAIL,
    };
  
  // And by hand we got these expected failure modes
  dr_failure_mode_type expected_failure_modes[BUG_001_NUM_FAILURE_MODES] =
    {
      DR_FAILURE_MODE_BAD,
      DR_FAILURE_MODE_SUSPECT,
    };

  // Process this d-matrix and see what we actually get
  dr_failure_mode_type actual_failure_modes[BUG_001_NUM_FAILURE_MODES];
  dr_error_type error =
    dr_process_d_matrix(&d_matrix_tbl,
			d_matrix_tbl.num_tests, test_results,
			d_matrix_tbl.num_failure_modes, actual_failure_modes);

  if(DR_ERROR_NO_ERROR != error)
  {
    return false;
  }

  test_passed = are_failure_mode_arrays_equal(BUG_001_NUM_FAILURE_MODES,
					      expected_failure_modes,
					      actual_failure_modes);
  
  return test_passed;

}

///////////////////////////////////////////////////////
// Private function definitions
//////////////////////////////////////////////////////

bool are_failure_mode_arrays_equal(size_t const num_failure_modes,
        dr_failure_mode_type const failure_modes_1[num_failure_modes],
        dr_failure_mode_type const failure_modes_2[num_failure_modes])
{
  bool failure_modes_are_equal = true;
  
  // Check that the two failure mode arrays are equal, by comparing
  // each element. If not equal, indicate break out of this loop
  for(size_t i = 0; i < num_failure_modes; ++i)
  {
    if(failure_modes_1[i] != failure_modes_2[i])
    {
      failure_modes_are_equal = false;
      break;
    }
  }

  return failure_modes_are_equal;
  
}

void initialize_example_d_matrix(dr_d_matrix_tbl_type * const d_matrix_tbl)
{
  // For these small examples it is clearer to use static initializers,
  // so use that on a temp variable then set the given array equal to it.
  dr_d_matrix_tbl_type foo =
    {
      .num_tests = DR_TEST_EXAMPLE_NUM_TESTS,
      .num_failure_modes = DR_TEST_EXAMPLE_NUM_FAILURE_MODES,
      .d_matrix = {
	{ 1, 1, 1, 1 },
	{ 0, 1, 1, 0 },
	{ 0, 1, 1, 0 },
	{ 0, 0, 0, 1 },
	{ 0, 0, 1, 0 } },
    };
  
  d_matrix_tbl->num_tests = foo.num_tests;
  d_matrix_tbl->num_failure_modes = foo.num_failure_modes;
  
  for(size_t i = 0; i < foo.num_tests; ++i)
  {
    for(size_t j = 0; j < foo.num_failure_modes; ++j)
    {
      d_matrix_tbl->d_matrix[j][i] = foo.d_matrix[j][i];
    }
  }

}
