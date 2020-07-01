
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "dr_test_d_matrix_swaps.h"
#include "dr_test_d_matrix_examples.h"
#include "dr_test_d_matrix_args.h"

// This would be somewhat easier and more flexible using
// a unit test framework. However cFS doesn't seem to come with
// one and I don't want to introduce extra dependencies.
int main(int argc, char * argv[])
{
  int pass_test_count = 0;
  int fail_test_count = 0;

  // Seed the random number generator
  srand(time(NULL));

  // Perform the test for wrong number of tests
  {
    bool test_passed = test_d_matrix_wrong_num_tests();
    printf("test_d_matrix_wrong_num_tests(): %s\n",
	   (test_passed) ? "pass": "fail");
    (test_passed) ? ++pass_test_count: ++fail_test_count;
  }

  // Perform the test for wrong number of failure modes
  {
    bool test_passed = test_d_matrix_wrong_num_failure_modes();
    printf("test_d_matrix_wrong_num_failure_modes(): %s\n",
	   (test_passed) ? "pass": "fail");
    (test_passed) ? ++pass_test_count: ++fail_test_count;
  }

  // Perform the test for wrong number of failure modes
  {
    bool test_passed = test_d_matrix_invalid_test_results();
    printf("test_d_matrix_invalid_test_results(): %s\n",
	   (test_passed) ? "pass": "fail");
    (test_passed) ? ++pass_test_count: ++fail_test_count;
  }

  // Perform the swap column order test (test results)
  {
    bool test_passed = test_d_matrix_swap_column_order();
    printf("test_d_matrix_swap_column_order(): %s\n",
	   (test_passed) ? "pass": "fail");
    (test_passed) ? ++pass_test_count: ++fail_test_count;
  }
  
  // Perform the swap row order test (failure modes)
  {
    bool test_passed = test_d_matrix_swap_row_order();
    printf("test_d_matrix_swap_row_order(): %s\n",
	   (test_passed) ? "pass": "fail");
    (test_passed) ? ++pass_test_count: ++fail_test_count;
  }

  // Perform the test for example 1 
  {
    bool test_passed = test_d_matrix_alg_example_1();
    printf("test_d_matrix_alg_example_1(): %s\n",
	   (test_passed) ? "pass": "fail");
    (test_passed) ? ++pass_test_count: ++fail_test_count;
  }

  // Perform the test for example 2
  {
    bool test_passed = test_d_matrix_alg_example_2();
    printf("test_d_matrix_alg_example_2(): %s\n",
	   (test_passed) ? "pass": "fail");
    (test_passed) ? ++pass_test_count: ++fail_test_count;
  }

  // Perform the test for bug 001
  {
    bool test_passed = test_d_matrix_bug_001();
    printf("test_d_matrix_bug_001(): %s\n",
	   (test_passed) ? "pass": "fail");
    (test_passed) ? ++pass_test_count: ++fail_test_count;
  }

printf("\n\nDR unit tests: %d passed, %d failed...\n\n", pass_test_count,
	 fail_test_count);
  
  return 0;
}

