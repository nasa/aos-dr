#include "dr_print_results.h"

///
// Here is something that you just have to know. If we are compiling
// for the DR unit tests, which don't depend on cFE, then make OS_printf
// define to plain old printf. Otherwise we'll use the cFE OS_printf.
// This hootenanny lets us use the same printing function for the cFS app
// and the unit tests.
#ifdef DR_UNIT_TEST
  #include <stdio.h>
  #define OS_printf printf
#else
  #include "cfe.h"
#endif

static char test_result_to_char(dr_test_result_type const test_result);
static char failure_mode_to_char(dr_failure_mode_type const failure_mode);

// Function to print the results of a call to DR's d-matrix processing.
// Exact printout depends on the size and entries in the d-matrix. But,
// hopefully the printing will result in something looking like this
// small example (if there is no error):
//
//      P   F   U
//    -------------
//  G | 1 | 0 | 1 |
//  B | 0 | 1 | 0 |
//  S | 0 | 1 | 1 |
//  U | 0 | 0 | 1 |
//    -------------

void dr_print_results(dr_error_type const error,
		     dr_d_matrix_tbl_type const * const d_matrix_tbl,
		     int const num_tests,
		     dr_test_result_type const test_results[num_tests],
		     int const num_failure_modes,
		     dr_failure_mode_type const failure_modes[num_failure_modes])
{
  OS_printf("DR: Error = %d\n", error);

  if(DR_ERROR_NO_ERROR != error)
  {
    OS_printf("DR: not printing results due to error...\n");
  }
  else
  {
    OS_printf("DR: Test results are across, calculated failure modes are down:\n");
    
    // Print the first line
    OS_printf("     "); // start with 5 spaces
    for(int i = 0; i < num_tests; ++i)
    {
      OS_printf("%3c  ", test_result_to_char(test_results[i]) );
    }
    OS_printf("\n");
    
    // Print the matrix top edge
    OS_printf("     "); // start with 5 spaces
    for(int i = 0; i < num_tests; ++i)
    {
      OS_printf("-----");
    }
    OS_printf("\n");
    
    // Print the rows of the matrix
    for(int i = 0; i < num_failure_modes; ++i)
    {
      OS_printf("%3c |", failure_mode_to_char(failure_modes[i]) );
      for(int j = 0; j < num_tests; ++j)
      {
	OS_printf("%3d |", d_matrix_tbl->d_matrix[i][j]);
      }
      OS_printf("\n");
    }
    
    // Print the matrix bottom edge
    OS_printf("     "); // start with 5 spaces
    for(int i = 0; i < num_tests; ++i)
    {
      OS_printf("-----");
    }
    OS_printf("\n");
  }
  
  OS_printf("\n\n");
}

char test_result_to_char(dr_test_result_type const test_result)
{
  char c;
  
  switch(test_result)
  {
  case DR_TEST_RESULT_PASS:
    c = 'P';
    break;
    
  case DR_TEST_RESULT_FAIL:
    c = 'F';
    break;
    
  case DR_TEST_RESULT_UNKNOWN:
    c = 'U';
    break;
    
  case DR_TEST_RESULT_COUNT:
  default:
    c = '?';
    break;

  }
  
  return c;
}

char failure_mode_to_char(dr_failure_mode_type const failure_mode)
{
  char c;

  switch(failure_mode)
  {
  case DR_FAILURE_MODE_GOOD:
    c = 'G';
    break;
  case DR_FAILURE_MODE_SUSPECT:
    c = 'S';
    break;
  case DR_FAILURE_MODE_BAD:
    c = 'B';
    break;
  case DR_FAILURE_MODE_UNKNOWN:
    c = 'U';
    break;
  case DR_FAILURE_MODE_COUNT:
  default:
    c = '?';
    break;


  }

  return c;

}
