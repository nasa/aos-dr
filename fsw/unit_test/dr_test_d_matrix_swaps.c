#include "dr_test_d_matrix_swaps.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "dr_process_d_matrix.h"
#include "dr_print_results.h"

///////////////////////////////////////////////////////
// Private function declarations
//////////////////////////////////////////////////////

static void swap_d_matrix_columns(dr_d_matrix_tbl_type * const d_matrix_tbl,
				  dr_test_result_type test_results[],
				  unsigned int swap_index_1,
				  unsigned int swap_index_2);
static void swap_d_matrix_rows(dr_d_matrix_tbl_type * const d_matrix_tbl,
			       unsigned int swap_index_1,
			       unsigned int swap_index_2);
static void swap_failure_modes(dr_failure_mode_type failure_modes[],
			       unsigned int swap_index_1,
			       unsigned int swap_index_2);

static int random_one_to_n(int max_value);
static int random_zero_to_n_minus_one(int max_value);

///////////////////////////////////////////////////////
// Public function definitions
//////////////////////////////////////////////////////
bool test_d_matrix_swap_column_order(void)
{
  bool test_passed = true;

  // For a large number of times, generate a random d-matrix and random
  // test results. Process the d-matrix to get the solved failure mode
  // vector. Then, randomly select two of the columns of the d-matrix, and
  // swap them, and swap the same indices in the test results. Process the
  // d-matrix again, and check that the solved failure mode vector is
  // the same as before.
  for(int i = 0; i < 10000; ++i)
  {
    // Set up the d-matrix and tests from random values
    dr_d_matrix_tbl_type d_matrix_tbl;

    d_matrix_tbl.num_tests = random_one_to_n(DR_MAX_TESTS);
    d_matrix_tbl.num_failure_modes = random_one_to_n(DR_MAX_FAILURE_MODES);
    
    for(int i = 0; i < d_matrix_tbl.num_tests; ++i)
    {
      for(int j = 0; j < d_matrix_tbl.num_failure_modes; ++j)
      {
	d_matrix_tbl.d_matrix[j][i] = random_zero_to_n_minus_one(2);
      }
    }

    dr_test_result_type test_results[DR_MAX_TESTS];
    for(int i = 0; i < d_matrix_tbl.num_tests; ++i)
    {
      test_results[i] = random_zero_to_n_minus_one(2);
    }
    dr_failure_mode_type failure_modes[DR_MAX_FAILURE_MODES];
    
    // Get the first failure modes
    dr_error_type error =
      dr_process_d_matrix(&d_matrix_tbl, d_matrix_tbl.num_tests,
			      test_results,
			      d_matrix_tbl.num_failure_modes, failure_modes);
    if(DR_ERROR_NO_ERROR != error)
    {
      return false;
    }
    // Set up swapped d_matrix and tests
    int swap_index_1 = random_zero_to_n_minus_one(d_matrix_tbl.num_tests);
    int swap_index_2 = random_zero_to_n_minus_one(d_matrix_tbl.num_tests);
    
    swap_d_matrix_columns(&d_matrix_tbl, test_results,
			  swap_index_1, swap_index_2);
    
    // Process the d_matrix again
    dr_failure_mode_type failure_modes_swap[DR_MAX_FAILURE_MODES]
      = { 0 };
    
    error =
      dr_process_d_matrix(&d_matrix_tbl, d_matrix_tbl.num_tests,
			      test_results, d_matrix_tbl.num_failure_modes,
			      failure_modes_swap);
    if(DR_ERROR_NO_ERROR != error)
    {
      return false;
    }
    
    // Check that the two failure mode arrays are equal, if not
    // indicate that the test failed and break out of this loop
    for(int i = 0; i < d_matrix_tbl.num_failure_modes; ++i)
    {
      if(failure_modes[i] != failure_modes_swap[i])
      {
	test_passed = false;
	break;
      }
    }

    // If any of the large numbers of swaps failed, then break out
    // of the overall loop and report the test failed.
    if(!test_passed)
    {
      break;
    }

  }
  
  return test_passed;
}

bool test_d_matrix_swap_row_order(void)
{
  bool test_passed = true;

  // For a large number of times, generate a random d-matrix and random
  // test results. Process the d-matrix to get the solved failure mode
  // vector. Then, randomly select two of the columns of the d-matrix, and
  // swap them, and swap the same indices in the test results. Process the
  // d-matrix again, and check that the solved failure mode vector is
  // the same as before.
  for(int i = 0; i < 10000; ++i)
  {
    // Set up the d-matrix and tests from random values
    dr_d_matrix_tbl_type d_matrix_tbl;

    d_matrix_tbl.num_tests = random_one_to_n(DR_MAX_TESTS);
    d_matrix_tbl.num_failure_modes = random_one_to_n(DR_MAX_FAILURE_MODES);
    
    for(int i = 0; i < d_matrix_tbl.num_tests; ++i)
    {
      for(int j = 0; j < d_matrix_tbl.num_failure_modes; ++j)
      {
	d_matrix_tbl.d_matrix[j][i] = random_zero_to_n_minus_one(2);
      }
    }

    dr_test_result_type test_results[DR_MAX_TESTS];
    for(int i = 0; i < d_matrix_tbl.num_tests; ++i)
    {
      test_results[i] = random_zero_to_n_minus_one(2);
    }
    dr_failure_mode_type failure_modes[DR_MAX_FAILURE_MODES];
    
    // Get the first failure modes
    dr_error_type error =
      dr_process_d_matrix(&d_matrix_tbl, d_matrix_tbl.num_tests,
			      test_results,
			      d_matrix_tbl.num_failure_modes, failure_modes);
    if(DR_ERROR_NO_ERROR != error)
    {
      return false;
    }
    
    // Set up swapped d_matrix
    int swap_index_1 =
      random_zero_to_n_minus_one(d_matrix_tbl.num_failure_modes);
    int swap_index_2 =
      random_zero_to_n_minus_one(d_matrix_tbl.num_failure_modes);
    
    //printf("Results before swapping rows %d and %d:\n",
//	   swap_index_1, swap_index_2);
//    dr_print_results(printf, error, &d_matrix_tbl, d_matrix_tbl.num_tests,
//		     test_results, d_matrix_tbl.num_failure_modes,
//			  failure_modes);

    swap_d_matrix_rows(&d_matrix_tbl, swap_index_1, swap_index_2);
    
    // Process the d_matrix again
    dr_failure_mode_type failure_modes_swap[DR_MAX_FAILURE_MODES]
      = { 0 };
    
    error =
      dr_process_d_matrix(&d_matrix_tbl, d_matrix_tbl.num_tests,
			      test_results, d_matrix_tbl.num_failure_modes,
			      failure_modes_swap);
    if(DR_ERROR_NO_ERROR != error)
    {
      return false;
    }

//    printf("Results after swapping rows %d and %d:\n",
//	   swap_index_1, swap_index_2);
//    dr_print_results(printf, error, &d_matrix_tbl, d_matrix_tbl.num_tests,
//		     test_results, d_matrix_tbl.num_failure_modes,
//			  failure_modes_swap);
    
    // Swap back the failure modes so that the failure modes correspond
    // to the original indices again.
    swap_failure_modes(failure_modes_swap, swap_index_1, swap_index_2);
    
    // Check that the two failure mode arrays are equal, if not
    // indicate that the test failed and break out of this loop
    for(int i = 0; i < d_matrix_tbl.num_failure_modes; ++i)
    {
      if(failure_modes[i] != failure_modes_swap[i])
      {
	test_passed = false;
	break;
      }
    }

    // If any of the large numbers of swaps failed, then break out
    // of the overall loop and report the test failed.
    if(!test_passed)
    {
      break;
    }

  }
  
  return test_passed;


}

///////////////////////////////////////////////////////
// Private function definitions
//////////////////////////////////////////////////////

void swap_d_matrix_columns(dr_d_matrix_tbl_type * const d_matrix_tbl,
			   dr_test_result_type test_results[],
			   unsigned int swap_index_1,
			   unsigned int swap_index_2)
{
  // Swap the d_matrix column entries
  for(int i = 0; i < d_matrix_tbl->num_failure_modes; ++i)
  {
    bool tmp = d_matrix_tbl->d_matrix[i][swap_index_1];
    d_matrix_tbl->d_matrix[i][swap_index_1] =
      d_matrix_tbl->d_matrix[i][swap_index_2];
    d_matrix_tbl->d_matrix[i][swap_index_2] = tmp;
  }

  // Swap the test results so that the same tests still correspond
  // to the correct columns. 
  dr_test_result_type tmp2 = test_results[swap_index_1];
  test_results[swap_index_1] = test_results[swap_index_2];
  test_results[swap_index_2] = tmp2;
}

void swap_d_matrix_rows(dr_d_matrix_tbl_type * const d_matrix_tbl,
			   unsigned int swap_index_1,
			   unsigned int swap_index_2)
{
  // Swap the d_matrix row entries
  for(int i = 0; i < d_matrix_tbl->num_tests; ++i)
  {
    bool tmp = d_matrix_tbl->d_matrix[swap_index_1][i];
    d_matrix_tbl->d_matrix[swap_index_1][i] =
      d_matrix_tbl->d_matrix[swap_index_2][i];
    d_matrix_tbl->d_matrix[swap_index_2][i] = tmp;
  }

}

// This helper function is supposed to be called with the same
// swap indices as that used to swap the d-matrix rows. For the
// columns we could have one function since the test results must
// be swapped also before processing the d-matrix. For the
// failure modes, we need separate functions because we'll swap the
// rows, then process the d-matrix, then swap the failure modes
void swap_failure_modes(dr_failure_mode_type failure_modes[],
			   unsigned int swap_index_1,
			   unsigned int swap_index_2)
{
  // Swap the failure modes 
  dr_failure_mode_type tmp = failure_modes[swap_index_1];
  failure_modes[swap_index_1] = failure_modes[swap_index_2];
  failure_modes[swap_index_2] = tmp;
}

int random_one_to_n(int max_value)
{

  int rand_num = rand();
  
  if(0 == rand_num)
  {
    rand_num++;
  }
  
  int num = ceil( (double)rand_num / (double)RAND_MAX * (double)max_value );

  return num;
}

int random_zero_to_n_minus_one(int max_value)
{
  int rand_num = rand();
  
  if(RAND_MAX == rand_num)
  {
    rand_num--;
  }
  
  int num = floor( (double)rand() / (double)RAND_MAX * (double)max_value );

  return num;
}

