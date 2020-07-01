
#include <stdbool.h>
#include <stdint.h>

#include "dr_process_d_matrix.h"

#include "mex.h"
#include "matrix.h"

// Enable this definition to turn on trace output
//#define AOS_DR_MEX_TRACE

// Helper functions to do data conversion

// Translate a Matlab 2-d logical array into a d-matrix.
bool matlab_to_d_matrix(mxArray const * mex_d_matrix,
			aos_dr_d_matrix_tbl_type * const d_matrix_tbl);

// Translate a Matlab uint8, single-row vector into an array of
// test results. The test result array given to this function must
// be at least of size AOS_DR_MAX_TESTS. 
bool matlab_to_test_results(mxArray const * mex_test_results,
			    uint32_t * const num_tests,
			    aos_dr_test_result_type test_results[]);

// Translate a failure mode array to a Matlab type. The returned array
// will be a single-column vector of size num_failure_modes. 
bool failure_modes_to_matlab(uint32_t const num_failure_modes,
			     aos_dr_failure_mode_type const failure_modes[],
			     mxArray * * mex_failure_modes);
			     
			     
// The required function entry point from Matlab
void
mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{

  bool success = true;

  // Do general checks on the number of arguments passed in
  if( (nlhs != 1) || (nrhs != 2) )
  {
    mexErrMsgTxt("dr error: dr_process_d_matrix requires 1 left-hand "
		 "argument and 2 right-hand arguments: [failure_modes] = "
		 "dr_process_d_matrix(d_matrix, test_results)" );
    success = false;
  }

  // Delcare the variables used in later code, declared at this scope since
  // we will use them in several blocks below. 
  aos_dr_d_matrix_tbl_type d_matrix_tbl;
  uint32_t num_tests;
  aos_dr_test_result_type test_results[AOS_DR_MAX_TESTS];
  uint32_t num_failure_modes;
  aos_dr_failure_mode_type  failure_modes[AOS_DR_MAX_FAILURE_MODES];

  mxArray * mex_failure_modes = 0;

  if(success)
  {
    // Attempt to convert the d-matrix table. If errors occurred
    // they will be logged inside this function. 
    success = matlab_to_d_matrix(prhs[0], &d_matrix_tbl);

#ifdef AOS_DR_MEX_TRACE
    mexPrintf("d-matrix in C :num_tests = %d, num_failure_modes = %d \n\t",
	      d_matrix_tbl.num_tests , d_matrix_tbl.num_failure_modes);
    for(int i = 0; i < d_matrix_tbl.num_failure_modes; ++i)
    {
      for(int j = 0; j < d_matrix_tbl.num_tests; ++j)
      {
	mexPrintf("%d   ", d_matrix_tbl.d_matrix[i][j]);
      }
       
      mexPrintf("\n\t");
    }
#endif

  }

  if(success)
  {
    // Attempt to convert the test results. If error occurred
    // they will be logged inside this function.
    success = matlab_to_test_results(prhs[1], &num_tests, test_results);
  }

  if(success)
  {
    // Finally call the DR d-matrix processing function
    num_failure_modes = d_matrix_tbl.num_failure_modes;
    
    aos_dr_error_type error =
      aos_dr_process_d_matrix(&d_matrix_tbl, num_tests, test_results,
			      num_failure_modes, failure_modes);

    // Report if there was an error
    if(AOS_DR_ERROR_NO_ERROR != error)
    {
      char err_msg_text[150];
      snprintf(err_msg_text, 150, "dr error: C function dr_process_d_matrix "
	       "returned error code %d", error);
      mexErrMsgTxt(err_msg_text);
      success = false;
    }
  }


  if(success)
  {
    // Attempt to convert the failure modes. If error occurred
    // it will be logged inside this function.
    success = failure_modes_to_matlab(num_failure_modes,
				      failure_modes,
				      &mex_failure_modes);
  }

  if(success)
  {
    plhs[0] = mex_failure_modes;

  }
  
}

bool matlab_to_d_matrix(const mxArray * mex_d_matrix,
			aos_dr_d_matrix_tbl_type * const d_matrix_tbl)
{

  bool success = true;
  
  // Check that the input d-matrix is what we expect: a logical,
  // 2-d array
  mwSize mex_d_matrix_dims = mxGetNumberOfDimensions(mex_d_matrix);
  
  if(!mxIsLogical(mex_d_matrix) || (2 != mex_d_matrix_dims) )
  {
    mexErrMsgTxt("dr error: D-matrix must be 2-d logical array.");
    success = false;
  }

  // Variable used later, so this call is placed in a higher scope.
  // Shouldn't hurt to call this even if an error occurred before.
  mwSize const * const p_sizes = mxGetDimensions(mex_d_matrix);

  if(success)
  {
    // Check that the number of elements in each dimension doesn't
    // exceed the max values in the C matrix. Note, at this point we
    // know we have 2 dimensions in the array.
    if( (p_sizes[0] > AOS_DR_MAX_FAILURE_MODES)
	|| (p_sizes[1] > AOS_DR_MAX_TESTS) )
    {
      mexErrMsgTxt("dr error: Exceeded D-matrix max size in "
		   "at least one dimension.");
      success = false;
    }
  }

  if(success)
  {
    // Finally get to converting the matlab matrix and filling the d-matrix.
    // Note, the D-matrix is defined with failure modes as the first
    // array index and tests as the second. I assume that in Matlab, the
    // same will be true. 
    // 
    mxLogical * mex_logical_matrix = mxGetLogicals(mex_d_matrix);
    
    d_matrix_tbl->num_tests = p_sizes[1];
    d_matrix_tbl->num_failure_modes = p_sizes[0];
    
    for(int i = 0; i < d_matrix_tbl->num_failure_modes; ++i)
    {
      for(int j = 0; j < d_matrix_tbl->num_tests; ++j)
      {
	// Looks like the data structure we get is a 1-D array which
	// represents the 2-D array. We'll need to convert by accessing
	// the elements like this:
	d_matrix_tbl->d_matrix[i][j] = 
	  mex_logical_matrix[ i + (j * d_matrix_tbl->num_failure_modes) ];
      }
    }
  }

  return success;
}


bool matlab_to_test_results(mxArray const * mex_test_results,
			    uint32_t * const num_tests,
			    aos_dr_test_result_type test_results[])
{

  bool success = true;
  
  // Check that the input test_results is what we expect: a 2-d uint8 array.
  mwSize mex_test_results_dims = mxGetNumberOfDimensions(mex_test_results);
  
  if(!mxIsUint8(mex_test_results) || (2 != mex_test_results_dims) )
  {
    mexErrMsgTxt("dr error: test results must be 2-d uint8 array.");
    success = false;
  }

  // Variable used later, so this call is placed in a higher scope.
  // Shouldn't hurt to call this even if an error occurred before.
  mwSize const * const p_sizes = mxGetDimensions(mex_test_results);
  
  if(success)
  {
    // Check that the input test_results is actually a row array;
    // the number of rows is 1. 
    if(p_sizes[0] != 1) 
    {
      mexErrMsgTxt("dr error: test results must be a row array.");
      success = false;
    }
  }
  
  if(success)
  {
    // Check that the number of elements doesn't exceed the max values
    // in the C array. 
    if(p_sizes[1] > AOS_DR_MAX_TESTS)
    {
      mexErrMsgTxt("dr error: Exceeded test results max array size.");
      success = false;
    }
  }
  
  if(success)
  {
    // Finally convert the values. Check that the value is
    // in the range of the enumerated type as we go.
    *num_tests = p_sizes[1];
    
    uint8_t * mex_data = (uint8_t *)mxGetData(mex_test_results);
    
    for(int i = 0; i < p_sizes[1]; ++i)
    {
      // As described before we have a 2-d matrix represented as a 
      // 1-d array. But, since this should be a row vector, it 
      // shouldn't matter here.
      uint8_t value = mex_data[i];

      // since we are unsigned we won't be less than 0, only check
      // that value is less than the max.
      if(value >= AOS_DR_TEST_RESULT_COUNT)
      {
	mexErrMsgTxt("dr error: test_result not in range of enumerated type.");
	success = false;
	break;
      }
      else
      {
	test_results[i] = value;
      }
    }
  }
   
  return success;
}

bool failure_modes_to_matlab(uint32_t const num_failure_modes,
			     aos_dr_failure_mode_type const failure_modes[],
			     mxArray * * mex_failure_modes)
{
  
  bool success = true;

  // Check that the number of failure modes doens't exceed the max number. 
  if(num_failure_modes > AOS_DR_MAX_FAILURE_MODES)
  {
    mexErrMsgTxt("dr error: Exceeded max failure modes array size.");
    success = false;
  }

  if(success)
  {
    *mex_failure_modes = mxCreateNumericMatrix(num_failure_modes, 1, 
					      mxUINT8_CLASS, mxREAL);

    uint8_t * mex_data = (uint8_t *)mxGetData(*mex_failure_modes);

    for(int i = 0; i < num_failure_modes; ++i)
    {
      // As described before we have a 2-d matrix represented as a 
      // 1-d array. But, since this should be a row vector, it 
      // shouldn't matter here.
      mex_data[i] = failure_modes[i];
    }
  }

  return success; 

}
