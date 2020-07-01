#include "dr_save_results.h"

#include <stdbool.h>

#include "osapi.h"


static const int MAX_FORMAT_BUFFER_SIZE = 32;

static int32 test_results_filedesc = OS_ERROR;
static int32 failure_modes_filedesc = OS_ERROR;

static bool save_int_csv(int32 filedesc, int const value);
static bool save_eol(int32 filedes);

static bool save_test_results(
  int const iteration,
  dr_error_type const error,
  int const num_tests,
  dr_test_result_type const test_results[num_tests]);

static bool save_failure_modes(
  int const iteration,
  dr_error_type const error,
  int const num_failure_modes,
  dr_failure_mode_type const failure_modes[num_failure_modes]);

dr_error_type dr_open_results_files(
  char const * const test_results_filename,
  char const * const failure_modes_filename)
{
  dr_error_type result = DR_ERROR_NO_ERROR;

  // Looking at OSAL osfileapi.c OS_open(), it looks as if errors are
  // negative numbers, and file descriptors may be from 0 to
  // OS_MAX_NUM_OPEN_FILES. So, test for failure of this call by checking
  // that the resulting file descriptor is less than 0.
  test_results_filedesc = OS_creat(test_results_filename, OS_WRITE_ONLY);
  
  if(test_results_filedesc < 0)
  {
    result = DR_ERROR_FILE_ERROR;
  }

  // Open the failure modes file 
  if(DR_ERROR_NO_ERROR == result)
  {
    failure_modes_filedesc = OS_creat(failure_modes_filename, OS_WRITE_ONLY);
    
    if(failure_modes_filedesc < 0)
    {
      result = DR_ERROR_FILE_ERROR;
      // Here we have created the test results file but not the
      // failure modes file. Attempt to close the file that was opened,
      // but since we are returning an error already, don't do anything with
      // the return result of this close.
      OS_close(test_results_filedesc);
    }
  }

  return result;
}

dr_error_type dr_close_results_files(void)
{
  dr_error_type result = DR_ERROR_NO_ERROR;

  // Attempt to close both of the files
  int32 test_results_close_result = OS_close(test_results_filedesc);
  int32 failure_modes_close_result = OS_close(failure_modes_filedesc);

  // If either call failed, report an error
  if( (OS_FS_SUCCESS != test_results_close_result) ||
      (OS_FS_SUCCESS != failure_modes_close_result) )
  {
    result = DR_ERROR_FILE_ERROR;
  }

  return result;
}


dr_error_type dr_save_results(
  int const iteration,
  dr_error_type const error,
  int const num_tests,
  dr_test_result_type const test_results[num_tests],
  int const num_failure_modes,
  dr_failure_mode_type const failure_modes[num_failure_modes])
{

  dr_error_type result = DR_ERROR_NO_ERROR;

  // Save the test results, and if there was an error set
  // the result accordingly
  bool save_tr_success =
    save_test_results(iteration, error, num_tests, test_results);

  if(!save_tr_success)
  {
    result = DR_ERROR_SAVE_ERROR;
  }

  // OS_printf("DR: dr_save_results(): save_tr_success = %d\n", save_tr_success);

  // Save the failure modes, and if there was an error set
  // the result accordingly
  if(DR_ERROR_NO_ERROR == result)
  {
    bool save_fm_success =
      save_failure_modes(iteration, error, num_failure_modes, failure_modes);
    
    if(!save_fm_success)
    {
      result = DR_ERROR_SAVE_ERROR;
    }
  }
  
  return result;
  
}

bool save_test_results(
  int const iteration,
  dr_error_type const error,
  int const num_tests,
  dr_test_result_type const test_results[num_tests])
{

  // OS_printf("DR in save test results: num test results = %d\n", num_tests);
  
  
  // First write the iteration and AOS error code
  bool success = save_int_csv(test_results_filedesc, iteration);

  if(success)
  {
    success = save_int_csv(test_results_filedesc, (int const)error);
  }
  else
  {
    // OS_printf("DR: Only saved iteration...\n");
  }

  // Then if we are still successful, AND the AOS error code was good,
  // save the test results
  if(success && (DR_ERROR_NO_ERROR == error) )
  {
    for(int i = 0; i < num_tests; ++i)
    {
      success = save_int_csv(test_results_filedesc,
				  (int const)test_results[i]);
      
      if(!success)
      {
	break;
      }
    }
  }

  // OS_printf("DR: save_test_results(): after saving test results, success = %d\n", success);

  // Finally add the EOL 
  if(success)
  {
    success = save_eol(test_results_filedesc);
  }

  // OS_printf("DR: save_test_results(): after save_eol, success = %d\n", success);

  return success;
}

static bool save_failure_modes(
  int const iteration,
  dr_error_type const error,
  int const num_failure_modes,
  dr_failure_mode_type const failure_modes[num_failure_modes])
{
  // First write the iteration and AOS error code
  bool success = save_int_csv(failure_modes_filedesc, iteration);

  if(success)
  {
    success = save_int_csv(failure_modes_filedesc, (int const)error);
  }

  // OS_printf("DR: save_failure_modes(): success = %d\n", success);

  // Then if we are still successful, AND the AOS error code was good,
  // save the failure modes
  if(success && (DR_ERROR_NO_ERROR == error) )
  {
    for(int i = 0; i < num_failure_modes; ++i)
    {
      success = save_int_csv(failure_modes_filedesc,
				  (int const)failure_modes[i]);
      
      if(!success)
      {
	break;
      }
    }
  }

  // Finally add the EOL 
  if(success)
  {
    success = save_eol(failure_modes_filedesc);
  }

  return success;
}

static bool save_int_csv(int32 filedes, int const value)
{
  bool success = true;
  
  char buffer[MAX_FORMAT_BUFFER_SIZE];

  // Format the int into the buffer, and indicate error if a
  // formatting error occurred or the buffer wasn't big enough
  int chars_needed = snprintf(buffer, MAX_FORMAT_BUFFER_SIZE,
			      "%d, ", value);

  if( (chars_needed < 0) ||
      (chars_needed >= MAX_FORMAT_BUFFER_SIZE) )
  {
    success = false;
  }

  // Write the formatted value to the file
  if(success)
  {
    int32 os_code = OS_write(filedes, buffer, chars_needed);
    
    if(os_code < 0) // OS errors are all < 0. 
    {
      success = false;
    }
  }

  return success;
}

static bool save_eol(int32 filedes)
{
  bool success = true;

  int32 os_code = OS_write(filedes, "\n", 1);
  
  if(os_code < 0) // OS errors are all < 0. 
  {
    success = false;
  }
  
  return success;
}
