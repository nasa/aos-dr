/************************************************************************
** Purpose:
**   Process the tests, reading the LC watchpoint results table and 
**   converting those to the dmatrix pass-fail values.    
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/

#include "dr_process_tests.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "lc_tbl.h"

#include "dr_events.h"
#include "dr_wtm_tbl.h"

/************************************************************************
** Local Definitions
*************************************************************************/
//#define DR_TRACE

/************************************************************************
** Local Function Prototypes
*************************************************************************/
#ifdef DR_TRACE
static void print_watch_result(uint32 const watchpoint_index,
			       uint8 const watch_result);
static void print_test_result(dr_test_result_type const test_result,
			      dr_test_type_type const test_type,
                              int const test_idx);
#endif

static dr_test_result_type evaluate_test(
				   dr_test_type_type const test_type,
				   uint8 const watch_result);
static dr_test_result_type evaluate_value_test(uint8 const watch_result);
static dr_test_result_type evaluate_staleness_test(
						 uint8 const watch_result);
static void clear_test_results(uint32_t const num_tests,
		       dr_test_result_type test_results[num_tests]);

static uint8 get_latched_test(dr_wtm_entry_type const * const dr_wtm_ptr,
			      uint32 test_idx, LC_WRTEntry_t *WRTPtr );



int32 dr_process_tests(CFE_TBL_Handle_t const DR_LC_WRTHandle,
		       dr_wtm_entry_type const * const dr_wtm_ptr,
		       uint32_t const num_tests,
		       dr_test_result_type test_results[num_tests])
{

  int32 status = CFE_SUCCESS;

  // Since this is a public function, check if the pointer arguments
  // are null; if one is, then return CFE_ES_ERR_BUFFER. Out of all the
  // cFE error codes in cfe_error.h, CFE_ES_ERR_BUFFER seems the best
  // because the description above it reads "Invalid pointer argument (NULL)".
  if( (NULL == dr_wtm_ptr) ||
      (NULL == test_results) )
  {
    status = CFE_ES_ERR_BUFFER;
  }
  else
  {
  
    static dr_test_result_type prev_test_results[DR_MAX_TESTS] =
      { DR_TEST_RESULT_UNKNOWN };
    
    LC_WRTEntry_t *WRTPtr = NULL;
    
    // Clear the test_results in preparation for this iteration
    clear_test_results(num_tests, test_results);
    
    // Get the address of the LC table
    int32 get_address_status =
      CFE_TBL_GetAddress((void *)&WRTPtr, DR_LC_WRTHandle);
    
    switch(get_address_status)
    {
    case CFE_SUCCESS:
    case CFE_TBL_INFO_UPDATED:
      // leave status as CFE_SUCCESS
      break;
    case CFE_TBL_ERR_BAD_APP_ID:
    case CFE_TBL_ERR_NO_ACCESS:
    case CFE_TBL_ERR_INVALID_HANDLE:
    case CFE_ES_ERR_APPNAME:
    case CFE_ES_ERR_BUFFER:
    case CFE_TBL_ERR_UNREGISTERED:
      // modify status to be the error code
      status = get_address_status;
      break;
    case CFE_TBL_ERR_NEVER_LOADED:
      // modify status to be the error code
      status = get_address_status;
      // also, according to the cFE documentation, only for this error case
      // we need to release the table handle as a valid one was returned.
      CFE_TBL_ReleaseAddress(DR_LC_WRTHandle);
      break;
    default:
      // We got an error code we didn't expect, so pass it on
      status = get_address_status;      
      break;
    }
    
    if (CFE_SUCCESS == status)
    {
      // Now do our stuff with the table. Be cognizant to not hold the
      // table address for too long, since that locks the table and LCX
      // won't be able to update it. But we will do all of our processing
      // with the table locked to avoid concurrency issues, since our
      // processing doesn't take too too long.
      for (uint32 i = 0; i < num_tests; ++i)
      {
	// Check that the test is active, if so proceed on to calculating
	// the test result
	if (dr_wtm_ptr[i].test_active)
	{
	  // Check that the test is currently valid. If
	  // test_valid_watchpoint_index != UINT32_MAX
	  // then there is a watchpoint to indicate validity. Get the
	  // value of that watchpoint. 
	  uint8 validity_watch_result = 0;

	  if( (dr_wtm_ptr[i].test_valid_watchpoint_index) != UINT32_MAX)
	  {
	    validity_watch_result =
	      WRTPtr[dr_wtm_ptr[i].test_valid_watchpoint_index].WatchResult;
	  }
	  
	  // If there is no watchpoint to indicate validity
	  // (test_valid_watchpoint_index == UINT32_MAX) then proceed to
	  // calculate the test result from the watchpoint_index. If there
	  // is a watchpoint to indicate validity (!= UINT32_MAX), then proceed
	// to calculate the test result if the validity watchpoint result != 0. 
	  if( ( (dr_wtm_ptr[i].test_valid_watchpoint_index) == UINT32_MAX) ||
	      ( ((dr_wtm_ptr[i].test_valid_watchpoint_index) != UINT32_MAX)  &&
		(validity_watch_result != 0) ) )
	  {
	    uint8 watch_result;
	    if(dr_wtm_ptr[i].test_latching == true )
	    {
	      if( prev_test_results[i] == DR_TEST_RESULT_FAIL ) 
	      {
		watch_result = get_latched_test(dr_wtm_ptr, i, WRTPtr );
	    }
	      else
	      {
		watch_result = 
		  WRTPtr[dr_wtm_ptr[i].watchpoint_index].WatchResult;
	      }
	    }
	    else
	    {
	      watch_result = 
		WRTPtr[dr_wtm_ptr[i].watchpoint_index].WatchResult;
	    }
	  
#ifdef DR_TRACE
	    print_watch_result(dr_wtm_ptr[i].watchpoint_index, watch_result);
#endif
	    // @TODO: Double-check that the watchpoint is used? Would require
	    // reading the watchpoint definition table as well.
	    test_results[dr_wtm_ptr[i].test_index] =
	      evaluate_test(dr_wtm_ptr[i].test_type, watch_result);
	    
#ifdef DR_TRACE
	    print_test_result(test_results[i], dr_wtm_ptr[i].test_type, i);
#endif
	  }
	}
      }
      
      // Now release the address of the table
      status = CFE_TBL_ReleaseAddress(DR_LC_WRTHandle);
      if (CFE_SUCCESS != status)
      {
	CFE_EVS_SendEvent(DR_REL_TBL_ADDRESS_ERR_EID, CFE_EVS_ERROR,
			  "dr: Could not release address for wrt table, "
			  "RetCode: 0x%08X", status);
      }
      
      // Save this cycle's test_results to check for latched tests next time
      memcpy( (void *)prev_test_results, (void *)test_results,
	      sizeof(dr_test_result_type) * num_tests);
      
    } 
  }
  
  return status;
}

uint8 get_latched_test(dr_wtm_entry_type const * const dr_wtm_ptr,
		       uint32 test_idx, LC_WRTEntry_t *WRTPtr )
{
  uint32   i;
  uint32   clr_idx;
  uint8    clear_pass_cnt = 0;
  uint8    watch_result;
  dr_test_result_type proc_test;

  for( i = 0; i <
          dr_wtm_ptr[test_idx].latch_clear.latch_clear_count; i++ )
  {
    clr_idx = dr_wtm_ptr[test_idx].latch_clear.latch_clear_indices[i];
    proc_test = evaluate_value_test( WRTPtr[clr_idx].WatchResult );
    if( proc_test == DR_TEST_RESULT_PASS )
    {
      clear_pass_cnt++;
    }
  }
  if( clear_pass_cnt >=
              dr_wtm_ptr[test_idx].latch_clear.latch_clear_count )
  {
    clr_idx = dr_wtm_ptr[test_idx].watchpoint_index;
    watch_result = WRTPtr[clr_idx].WatchResult;
  }
  else
  {
    watch_result = LC_WATCH_TRUE;
  }
  return( watch_result );
}

#ifdef DR_TRACE
void print_watch_result(uint32 const watchpoint_index,
                        uint8 const watch_result)
{
    static const int str_size = 15;
    char wrt_value_string[str_size];
    switch(watch_result)
    {
      case LC_WATCH_FALSE:
        snprintf(wrt_value_string, str_size, "FALSE");
        break;
      case LC_WATCH_TRUE:
        snprintf(wrt_value_string, str_size, "TRUE");
        break;
      case LC_WATCH_ERROR:
        snprintf(wrt_value_string, str_size, "ERROR");
        break;
      case LC_WATCH_STALE:
        snprintf(wrt_value_string, str_size, "STALE");
        break;
      default:
        snprintf(wrt_value_string, str_size, "WHAT?!?");
        break;
    }

    OS_printf("dr: watch result of watchpoint %d is %s.\n",
		watchpoint_index, wrt_value_string);
}
#endif

#ifdef DR_TRACE
void print_test_result(dr_test_result_type const test_result,
		       dr_test_type_type const test_type,
		       int const test_idx)
{
  static const int str_size = 45;
  char test_value_string[str_size];
  char test_type_string[str_size];  

  OS_printf("DR: test index = %d, test result = %d, test_type = %d\n",
	    test_idx, test_result, test_type);
  
  switch(test_result)
  {
  case DR_TEST_RESULT_PASS:
    snprintf(test_value_string, str_size, "DR_TEST_RESULT_PASS");
    break;
  case DR_TEST_RESULT_FAIL:
    snprintf(test_value_string, str_size, "DR_TEST_RESULT_FAIL");
    break;
  case DR_TEST_RESULT_UNKNOWN:
    snprintf(test_value_string, str_size, "DR_TEST_RESULT_UNKNOWN");
    break;  
  case DR_TEST_RESULT_COUNT:
  default:
    snprintf(test_value_string, str_size, "WHAT?!?");
    break;
  }
  
  switch(test_type)
  {
    case DR_VALUE_TEST:
      snprintf(test_type_string, str_size, "DR_VALUE_TEST");
      break;
    case DR_STALENESS_TEST:
      snprintf(test_type_string, str_size, "SR_STALENESS_TEST");
      break;
    default:
      snprintf(test_type_string, str_size, "WHAT?!?");
      break;
  }

  OS_printf("dr: test result of test index %d is %s. (%s)\n",
	    test_idx, test_value_string, test_type_string);
}
#endif

dr_test_result_type evaluate_test(
				   dr_test_type_type const test_type,
				   uint8 const watch_result)
{
  dr_test_result_type test_result = DR_TEST_RESULT_UNKNOWN;

  switch(test_type)
  {
    case DR_VALUE_TEST:
      test_result = evaluate_value_test(watch_result);
      break;
    case DR_STALENESS_TEST:
      test_result = evaluate_staleness_test(watch_result);
      break;
    default:
      // leave test_result as unknown
      break;
  }

  return test_result;

}

dr_test_result_type evaluate_value_test(uint8 const watch_result)
{
  dr_test_result_type test_result = DR_TEST_RESULT_UNKNOWN;
  
  switch (watch_result)
  {
  case LC_WATCH_FALSE:
    test_result = DR_TEST_RESULT_PASS;
    break;
  case LC_WATCH_TRUE:
    test_result = DR_TEST_RESULT_FAIL;
    break;
  case LC_WATCH_ERROR:
  case LC_WATCH_STALE:
  default:
    test_result = DR_TEST_RESULT_UNKNOWN;
    break;
  }
  
  return test_result;
}

dr_test_result_type evaluate_staleness_test(uint8 const watch_result)
{
  dr_test_result_type test_result = DR_TEST_RESULT_UNKNOWN;

  switch (watch_result)
  {
    case LC_WATCH_FALSE:
    case LC_WATCH_TRUE:
      test_result = DR_TEST_RESULT_PASS;
      break;
    case LC_WATCH_STALE:
      test_result = DR_TEST_RESULT_FAIL;
      break;
    case LC_WATCH_ERROR:
    default:
      test_result = DR_TEST_RESULT_UNKNOWN;
      break;
  }

  return test_result;
}

void clear_test_results(uint32_t const num_tests,
			dr_test_result_type test_results[num_tests])
{

  // Here, "clearing" means setting each element to unknown.
  for(int i = 0; i < num_tests; ++i)
  {
    test_results[i] = DR_TEST_RESULT_UNKNOWN;
  }
    
}

