/*******************************************************************************
** File: dr_app.c
**
** Purpose:
**   This file contains the source code for the Diagnostic Reasoner.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "dr_app.h"
#include "dr_perfids.h"
#include "dr_msgids.h"

#include "lc_platform_cfg.h" // for LC_APP_NAME
#include "lc_app.h" // for LC_WRT_TABLENAME

#include "cfe_tbl_msg.h"

#include "dr_msg.h"
#include "dr_events.h"
#include "dr_version.h" 
#include "dr_mode_def.h"
#include "dr_d_matrix_tbl.h"
#include "dr_wtm_tbl.h"

#include "dr_process_tests.h"
#include "dr_process_d_matrix.h"
#include "dr_print_results.h"
#include "dr_save_results.h"

////////////////////////////////////////////////////////
// Stuff to help debugging/testing

// Uncomment to enable trace messages
//#define DR_TRACE
// Hack, here are some other trace messages
//#define DR_TRACE_SUSPECTS_BADS

// Uncomment to print out timing info to the screen
//#define DR_TIMING

#ifdef DR_TIMING
// I would usually put headers farther up but we only need
// this one for doing the timing...
// Also, the man page for gettimeofday stated it was obsolete under
// POSIX 2008, and recommended clock_gettime() instead, so I'm
// using that. I should test for _POSIX_C_SOURCE >= 199309L to check
// if it exists, but maybe will do that later...
#include <time.h>
struct timespec time_interval(struct timespec start, struct timespec end);
#endif

/*********************************
** global data, what isn't shared with other modules is declared static
*/
static dr_hk_tlm_type         DR_HkTelemetryPkt;
static dr_diagnosis_msg_type  DR_Diagnosis_Msg;
static CFE_SB_PipeId_t        DR_CommandPipe;
static CFE_SB_MsgPtr_t        DR_MsgPtr;


static CFE_EVS_BinFilter_t  DR_EventFilters[] =
       {  /* Event ID    mask */
          {DR_STARTUP_INF_EID,       0x0000},
          {DR_COMMAND_ERR_EID,       0x0000},
          {DR_COMMANDNOP_INF_EID,    0x0000},
          {DR_COMMANDRST_INF_EID,    0x0000},
       };

///////////////////////////////////////////
// Handles to the various tables and table pointers we use
static CFE_TBL_Handle_t dr_mode_def_handle;
static dr_mode_def_entry_type * dr_mode_def_ptr;

static CFE_TBL_Handle_t dr_d_matrix_handle;
static dr_d_matrix_tbl_type * dr_d_matrix_ptr;

static CFE_TBL_Handle_t dr_wtm_handle;
static dr_wtm_entry_type * dr_wtm_ptr;

static CFE_TBL_Handle_t DR_LC_WDTHandle;

static CFE_TBL_Handle_t DR_LC_WRTHandle;


///////////////////////////////////////////////////
// Constants
// Make the DR startup sync timeout the same as the SCH sync timeout. 
static uint32 const DR_STARTUP_SYNC_TIMEOUT_MILLIS = 50000;
static uint32 const DR_TABLE_WAIT_TIMEOUT_MILLIS = 10000;
static uint32 const DR_TABLE_POLL_INTERVAL_MILLIS = 250;

static char const * const DR_TEST_RESULTS_FILENAME = "/cf/dr_test_results.csv";
static char const * const DR_FAILURE_MODES_FILENAME = "/cf/dr_failure_modes.csv";

//////////////////////////////////////////////////
// Private function prototypes
static int32 DR_AppInit(void);
static int32 DR_RegisterLcTables(void);
static int32 DR_UnregisterLcTables(void);
static int32 DR_InitTables(void);
static int32 DR_InitSwBus(void);
static int32 DR_ManageTables(void);
static int32 DR_ChangeMode(int32 new_mode);
static int32 DR_LoadLcWatchDefTable();
static int32 DR_WaitForTable(CFE_TBL_Handle_t table_handle, uint32 timeout_millis);

static int32   DR_AppPipe(CFE_SB_MsgPtr_t MessagePtr);
static void    DR_ProcessGroundCommand(void);
static void    DR_ReportHousekeeping(void);
static void    DR_ResetCounters(void);
static void    DR_ChangeModeCommand(void);
static int32   DR_Wakeup(void);
static int32   DR_Diagnose(void);
static int32   DR_Shutdown(void);

// TODO: Define table validation functions!! See cfe_tbl.h.
// Follows form of int32 CallbackFunc(void *TblPtr);

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* DR_AppMain() -- Application entry point and main process loop          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/

//  cppcheck-suppress unusedFunction This is the app entry point, but cppcheck doesn't know that. (yet?)
void DR_AppMain( void )
{
  int32  status;  
  uint32 RunStatus = CFE_ES_APP_RUN;

  CFE_ES_PerfLogEntry(DR_PERF_ID);
  
  status = DR_AppInit();
  if (CFE_SUCCESS != status)
  {
    RunStatus = CFE_ES_APP_ERROR;
  }

  /*
  ** DR Runloop
  */

#ifdef MODE_CHANGE_TRIGGER
  // Hack in a counter to change mode after a while
    int i = 0;
#endif
    
  while (CFE_ES_RunLoop(&RunStatus))
  {
    CFE_ES_PerfLogExit(DR_PERF_ID);
    
    /* Pend on receipt of command packet -- timeout set to 500 millisecs */
    status = CFE_SB_RcvMsg(&DR_MsgPtr, DR_CommandPipe, 500);
    
    CFE_ES_PerfLogEntry(DR_PERF_ID);
    
    if (CFE_SUCCESS == status)
    {
      status = DR_AppPipe(DR_MsgPtr);
    }
    if (CFE_SUCCESS != status)
    {
      switch(status)
      {
      case CFE_SUCCESS:
      case CFE_SB_TIME_OUT:
      case CFE_SB_NO_MESSAGE:
	status = CFE_SUCCESS;
	break;
      case CFE_SB_BAD_ARGUMENT:
      case CFE_SB_PIPE_RD_ERR:
      default: // we got an unknown status
	RunStatus = CFE_ES_APP_ERROR;
	break;
      }
    }
    
  }
  
  // Check for "fatal" process error...
  if (CFE_SUCCESS != status)
  {
    
    // Send an event describing the reason for the termination
    CFE_EVS_SendEvent(DR_TASK_EXIT_EID, CFE_EVS_CRITICAL,
		      "Task terminating, err = 0x%08X", status);
    
    // In case cFE Event Services is not working
    CFE_ES_WriteToSysLog("DR task terminating, err = 0x%08X\n", status);
    
  }
  
  DR_Shutdown();
  
  
  CFE_ES_ExitApp(RunStatus);
  
} /* End of DR_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* DR_AppInit() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 DR_AppInit(void) 
{
  ////////////////////////////////////////////////////////////////////////
  // We will read the LC(X)-owned watchpoint result table. To ensure that
  // we register for that table's handle after the LC(X) app has created it,
  // we will call CFE_ES_WaitForStartupSync(). Note that if we have
  // resources like tables of our own, we must create them before
  // that call in case other apps are similarly waiting for our
  // resources to be initialized.

  //////////////////////////////////////////////////////////////////////
  // Initialize "our stuff"

  /*
  ** Register the app with Executive services
  */
  int32 status = CFE_ES_RegisterApp();
  
  /*
  ** Register the events
  */
  if(CFE_SUCCESS == status)
  {
    
    status = CFE_EVS_Register(DR_EventFilters,
			  sizeof(DR_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			      CFE_EVS_BINARY_FILTER);
    
  }
  
  ///////////////////////////////////////////////////
  // Initialize our software bus items
  if(CFE_SUCCESS == status)
  {
    status = DR_InitSwBus();
  }
  
  ///////////////////////////////////////////////////
  // Initialize our tables
  if(CFE_SUCCESS == status)
  {
    status = DR_InitTables();
  }
  
  if(CFE_SUCCESS == status)
  {
    DR_ResetCounters();
  }

  //////////////////////////////////////////////////
  // Initialize our data files
  if(CFE_SUCCESS == status)
  {
    dr_error_type result = dr_open_results_files(
      DR_TEST_RESULTS_FILENAME, DR_FAILURE_MODES_FILENAME);
    
    if(DR_ERROR_NO_ERROR != result)
    {
      status = DR_FILE_OPEN_ERROR;
    }
  }
  
  OS_printf("DR: After opening DR files, status = %ld\n", (long)status);
  
  //////////////////////////////////////////////////////////
  // Now wait for other apps to initialize. No return value
  // so we don't know if all apps initialized or if we timed out.
  if (CFE_SUCCESS == status)
  {
    OS_printf("DR: Waiting for all apps to start\n");
    CFE_ES_WaitForStartupSync(DR_STARTUP_SYNC_TIMEOUT_MILLIS);
  }
  
  /////////////////////////////////////////////////////////
  // Now register for "other apps' stuff"

  // Register for the LC(X) watchpoint definition table and
  // watchpoint results table
  if (CFE_SUCCESS == status)
  {
    status = DR_RegisterLcTables();
  }
  OS_printf("DR: After DR_RegisterLcTables(), status = %ld\n", (long)status);

  ///////////////////////////////////////////
  // Now that all tables are registered, change to the default mode, 0 for now
  if(CFE_SUCCESS == status)
  {
    status = DR_ChangeMode(0);
  }

  OS_printf("DR: After DR_ChangeMode, status = %ld\n", (long)status);
  
  if(CFE_SUCCESS == status)
  {
    CFE_SB_InitMsg(&DR_HkTelemetryPkt,
                   DR_HK_TLM_MID,
                   DR_HK_TLM_LNGTH, TRUE);
    
    CFE_EVS_SendEvent (DR_STARTUP_INF_EID, CFE_EVS_INFORMATION,
		       "DR Initialized. Version %d.%d.%d.%d",
		       DR_MAJOR_VERSION,
		       DR_MINOR_VERSION, 
		       DR_REVISION, 
		       DR_MISSION_REV);
  }
  else
  {
    // send event that startup failed
    CFE_EVS_SendEvent (DR_STARTUP_ERR_EID, CFE_EVS_ERROR,
		       "DR: Error initializing app");
  }

  return status;
  
} /* End of DR_AppInit() */

int32 DR_Shutdown(void)
{  
  // @TODO: Do we need to unregister the table(s), or is that handled
  // by CFE_ES_ExitApp?

  int32 status = DR_UnregisterLcTables();

  // Close the files. Ignore any error since we are shutting down anyway.
  dr_close_results_files();
  
  return status;
}

int32 DR_InitSwBus(void)
{
  int32 status = CFE_SUCCESS;
  
  /*
  ** Create the Software Bus command pipe, subscribe to 
  **  messages, and initialize the diagnosis message we send
  */
  if(CFE_SUCCESS == status)
  {
    status = CFE_SB_CreatePipe(&DR_CommandPipe, DR_PIPE_DEPTH,"DR_CMD_PIPE");
  }
  if(CFE_SUCCESS == status)
  {
    status = CFE_SB_Subscribe(DR_CMD_MID, DR_CommandPipe);
  }
  if(CFE_SUCCESS == status)
  {
    status = CFE_SB_Subscribe(DR_SEND_HK_MID, DR_CommandPipe);
  }
  if(CFE_SUCCESS == status)
  {
    status = CFE_SB_Subscribe(DR_WAKEUP_MID, DR_CommandPipe);
  }

  if(CFE_SUCCESS == status)
  {
    CFE_SB_InitMsg(&DR_Diagnosis_Msg, DR_DIAGNOSIS_MID,
		   sizeof(dr_diagnosis_msg_type), TRUE);
  }

  return status;

}

int32 DR_InitTables(void)
{
  ///////////////////////////////////////////////////
  // Start with the mode definition table as it contains
  // the data we need for all of the others.
  // Register, load, and get a pointer to the mode def table
  uint32 option_flags = CFE_TBL_OPT_DEFAULT;
  int32 status = CFE_TBL_Register(&dr_mode_def_handle, DR_MODE_DEF_NAME,
		    (sizeof(dr_mode_def_entry_type)*DR_MAX_NUM_MODES),
			    option_flags,
			    NULL);

  if(CFE_SUCCESS == status)
  {
    status = CFE_TBL_Load(dr_mode_def_handle,
			  CFE_TBL_SRC_FILE, DR_MODE_DEF_DEFAULT_FILENAME);
  }
  
  if(CFE_SUCCESS == status)
  {
    status = CFE_TBL_GetAddress((void *)&dr_mode_def_ptr, dr_mode_def_handle);
    
    if(CFE_TBL_INFO_UPDATED == status) 
    {
      status = CFE_SUCCESS;
    }
  }

  ///////////////////////////////////////////////
  // Register the other DR tables. They will be loaded and
  // the addresses gotten at a later initilization step, by
  // calling the mode change function. 
  if(CFE_SUCCESS == status)
  {
    status = CFE_TBL_Register(&dr_d_matrix_handle, DR_D_MATRIX_NAME,
			    sizeof(dr_d_matrix_tbl_type), option_flags,
			    NULL);
  }

  if(CFE_SUCCESS == status)
  {
    status = CFE_TBL_Register(&dr_wtm_handle, DR_WTM_NAME,
			      sizeof(dr_wtm_entry_type) * DR_MAX_TESTS,
			      option_flags, NULL);
  }
  
  return status;

}

int32 DR_RegisterLcTables(void)
{

  // Create the full name of the LCX Watchpoint Definition Table (WDT), which
  // is APP_NAME.TABLE_NAME
  char lcWdtTableName[CFE_TBL_MAX_FULL_NAME_LEN];
  snprintf(lcWdtTableName, CFE_TBL_MAX_FULL_NAME_LEN, "%s.%s",
	   LC_APP_NAME, LC_WDT_TABLENAME);

  // Register for the table created by the other application
  int32 status = CFE_TBL_Share( &DR_LC_WDTHandle, lcWdtTableName);
  if(CFE_SUCCESS != status)
  {
      CFE_EVS_SendEvent(DR_TBL_SUB_ERR_EID, CFE_EVS_ERROR,
			"Error initializing watchpoint definition table, "
			"TblName=%s, RetCode=0x%08X", lcWdtTableName, status);
  }

  if(CFE_SUCCESS == status)
  {
    // Create the full name of the LCX Watchpoint Results Table (WRT), which is
    // APP_NAME.TABLE_NAME
    char lcWrtTableName[CFE_TBL_MAX_FULL_NAME_LEN];
    snprintf(lcWrtTableName, CFE_TBL_MAX_FULL_NAME_LEN, "%s.%s",
	     LC_APP_NAME, LC_WRT_TABLENAME);
    
    // Register for the table created by the other application
    status = CFE_TBL_Share( &DR_LC_WRTHandle, lcWrtTableName);
    if(CFE_SUCCESS != status)
    {
      CFE_EVS_SendEvent(DR_TBL_SUB_ERR_EID, CFE_EVS_ERROR,
			"Error initializing watchpoint results table, "
			"TblName=%s, RetCode=0x%08X", lcWrtTableName, status);
    }
  }

  // @TODO: Setup this to be notified by a message when the LC.WRT is changed,
  // instead of polling (I think - ask Pat?):
  // CFE_TBL_NotifyByMessage in cfe_tbl.h


  return status;

}

int32 DR_UnregisterLcTables(void)
{
  int32 status = CFE_TBL_Unregister(DR_LC_WDTHandle);

  if(CFE_SUCCESS == status)
  {
    status = CFE_TBL_Unregister(DR_LC_WRTHandle);
  }
  
  return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process a command pipe message                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 DR_AppPipe(CFE_SB_MsgPtr_t MessagePtr)
{
  int32           status      = CFE_SUCCESS;
  CFE_SB_MsgId_t  MsgId;
  
  MsgId = CFE_SB_GetMsgId(DR_MsgPtr);
  
  switch (MsgId)
  {
  case DR_CMD_MID:
    DR_ProcessGroundCommand();
    break;
    
  case DR_SEND_HK_MID:
    DR_ReportHousekeeping();
    break;
  case DR_WAKEUP_MID:
    status = DR_Wakeup();
    break;    
  default:
    DR_HkTelemetryPkt.dr_command_error_count++;
    CFE_EVS_SendEvent(DR_COMMAND_ERR_EID,CFE_EVS_ERROR,
		      "DR: invalid command packet,MID = 0x%x", MsgId);
    break;
  }
  
  return status;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* DR_ProcessGroundCommand() -- DR ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

void DR_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(DR_MsgPtr);

    /* Process "known" DR ground commands */
    switch (CommandCode)
    {
        case DR_NOOP_CC:
            DR_HkTelemetryPkt.dr_command_count++;
            CFE_EVS_SendEvent(DR_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"DR: NOOP command");
            break;

        case DR_RESET_COUNTERS_CC:
            DR_ResetCounters();
            break;

        case DR_CHANGE_MODE_CC:
            DR_ChangeModeCommand();
            break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of DR_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  DR_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void DR_ResetCounters(void)
{
    /* Status of commands processed by the DR */
    DR_HkTelemetryPkt.dr_command_count       = 0;
    DR_HkTelemetryPkt.dr_command_error_count = 0;

    CFE_EVS_SendEvent(DR_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"DR: RESET command");

    return;

} /* End of DR_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  DR_ReportHousekeeping                                              */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the DR       */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void DR_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &DR_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &DR_HkTelemetryPkt);
    return;

} /* End of DR_ReportHousekeeping() */

void    DR_ChangeModeCommand()
{
    int success = 1;
    size_t ExpectLength = sizeof (dr_change_mode_cmd_type);
    size_t ActualLength = CFE_SB_GetTotalMsgLength(DR_MsgPtr);
    if (ActualLength != ExpectLength) {
        CFE_EVS_SendEvent(DR_LEN_ERR_EID, CFE_EVS_ERROR,
                          "DR Change Mode command with bad length (Expected %lu, Observed %lu)",
                          (unsigned long)ExpectLength,
                          (unsigned long)ActualLength);
        success = 0;
    }

    dr_change_mode_cmd_type const *ptr =
        (dr_change_mode_cmd_type *)DR_MsgPtr;

    if (success) {
        unsigned long new_mode = ptr->NewMode;
        int32 status = DR_ChangeMode(new_mode);
        if (CFE_SUCCESS != status) {
            CFE_EVS_SendEvent(DR_COMMAND_ERR_EID, CFE_EVS_ERROR,
                              "Unable to change to mode %lu, error code is 0x%08lX (%ld)",
                              (unsigned long)new_mode,
                              0xFFFFFFFF & (unsigned long)status, (long)status);
            success = 0;
        } else {
            CFE_EVS_SendEvent(DR_MODE_CHANGED_INFO_EID, CFE_EVS_INFORMATION,
                              "Switched to mode %lu",
                              (unsigned long)new_mode);
        }
    }

    if (success) {
        DR_HkTelemetryPkt.dr_command_count++;
    } else {
        DR_HkTelemetryPkt.dr_command_error_count++;
    }

    return;
} /* End of DR_ChangeModeCommand() */


int32 DR_Wakeup(void)
{

  // First manage our tables on every wakeup.
  int32 status = DR_ManageTables();
  
  // Now do a diagnosis
  if(CFE_SUCCESS == status)
  {
    status = DR_Diagnose();
  }
  
  return status;

}

int32 DR_Diagnose(void)
{
#ifdef DR_TIMING
  struct timespec before_process_tests;
  clock_gettime(/*CLOCK_REALTIME*/ CLOCK_THREAD_CPUTIME_ID, &before_process_tests); 
#endif

  ///////////////////////////////////////
  // Process the test results
  dr_test_result_type test_results[DR_MAX_TESTS];
  uint32_t num_tests = dr_d_matrix_ptr->num_tests;
  
  int32 status = dr_process_tests(DR_LC_WRTHandle, dr_wtm_ptr,
			    num_tests,
			    test_results);

#ifdef DR_TIMING
  struct timespec before_process_d_matrix;
  clock_gettime(/*CLOCK_REALTIME*/ CLOCK_THREAD_CPUTIME_ID, &before_process_d_matrix); 
#endif

  ///////////////////////////////////////
  // Perform the diagnosis
  DR_Diagnosis_Msg.num_failure_modes = dr_d_matrix_ptr->num_failure_modes;
  
  DR_Diagnosis_Msg.error = dr_process_d_matrix(
    dr_d_matrix_ptr, num_tests, test_results,
    DR_Diagnosis_Msg.num_failure_modes,
    DR_Diagnosis_Msg.failure_modes);
  
  if(DR_Diagnosis_Msg.error != DR_ERROR_NO_ERROR)
  {
    status = DR_DIAGNOSIS_ERROR;
  }

#ifdef DR_TRACE_DR_TRACE_SUSPECTS_BADS
  OS_printf("dr: suspect and bad failure modes:\n");
  for(size_t i = 0; i < DR_Diagnosis_Msg.num_failure_modes; ++i)
  {
    if( (DR_FAILURE_MODE_SUSPECT == DR_Diagnosis_Msg.failure_modes[i]) ||
	(DR_FAILURE_MODE_BAD == DR_Diagnosis_Msg.failure_modes[i]) )
      {
	OS_printf("   index %u = %u\n", i, DR_Diagnosis_Msg.failure_modes[i]);
      }

  }
#endif
  
#ifdef DR_TIMING

  struct timespec after_process_d_matrix;
  clock_gettime(/*CLOCK_REALTIME*/CLOCK_THREAD_CPUTIME_ID, &after_process_d_matrix);

  struct timespec test_results_time = time_interval(before_process_tests,
						    before_process_d_matrix);
  
  struct timespec d_matrix_time = time_interval(before_process_d_matrix,
						after_process_d_matrix);
  
  struct timespec total_diag_time = time_interval(before_process_tests,
						  after_process_d_matrix);

  OS_printf("DR: test_results_time: %11lld.%.9ld \n",
	    (long long)test_results_time.tv_sec, test_results_time.tv_nsec);
  OS_printf("DR: d_matrix_time: %11lld.%.9ld \n",
	    (long long)d_matrix_time.tv_sec, d_matrix_time.tv_nsec);
  OS_printf("DR: total_diag_time: %11lld.%.9ld \n",
	    (long long)total_diag_time.tv_sec, total_diag_time.tv_nsec);
    
#endif

  // Send the results to the software bus. This is a best-effort send
  // like all sw bus messages and if it fails it would be logged
  // by cFE. 
  CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &DR_Diagnosis_Msg);
  CFE_SB_SendMsg((CFE_SB_Msg_t *) &DR_Diagnosis_Msg);
  
  
  // Write the results to the files
  static int iteration = 0;
  dr_error_type save_error = dr_save_results(
    iteration,
    DR_Diagnosis_Msg.error,
    num_tests,
    test_results,
    DR_Diagnosis_Msg.num_failure_modes,
    DR_Diagnosis_Msg.failure_modes);
  iteration++; 
  
  if(save_error != DR_ERROR_NO_ERROR)
  {
    status = DR_RESULTS_SAVE_ERROR;
  }

#ifdef DR_TRACE
  dr_print_results(DR_Diagnosis_Msg.error, dr_d_matrix_ptr,
		   num_tests, test_results,
		   DR_Diagnosis_Msg.num_failure_modes,
		   DR_Diagnosis_Msg.failure_modes);
#endif // DR_TRACE
  
  return status;
}

int32 DR_ManageTables(void)
{
    
  // Must release loadable table pointers before allowing updates
#ifdef DR_TRACE
  OS_printf("DR: DR_ManageTables(): Releasing table addresses\n");
#endif
  CFE_TBL_ReleaseAddress(dr_mode_def_handle);
  CFE_TBL_ReleaseAddress(dr_d_matrix_handle);
  CFE_TBL_ReleaseAddress(dr_wtm_handle);
  
  // Manage the tables (I need to find out what is involved here)
#ifdef DR_TRACE
  OS_printf("DR: DR_ManageTables(): Managing the tables\n");
#endif
  CFE_TBL_Manage(dr_mode_def_handle);
  CFE_TBL_Manage(dr_d_matrix_handle);
  CFE_TBL_Manage(dr_wtm_handle);
  
  // Re-acquire the pointers 
#ifdef DR_TRACE
  OS_printf("DR: DR_ManageTables(): Re-acquiring the addresses\n");
#endif
  int32 status = CFE_TBL_GetAddress((void *)&dr_mode_def_ptr,
			      dr_mode_def_handle);
  if(CFE_TBL_INFO_UPDATED == status)
  {
    status = CFE_SUCCESS;
  }
  if(CFE_SUCCESS != status)
  {
    OS_printf("DR: DR_ManageTables(): Error getting address for mode def table:"
		"status = 0x%08X\n", status);
  }
  
  if (CFE_SUCCESS == status)
  {
    status = CFE_TBL_GetAddress((void *)&dr_d_matrix_ptr,
				dr_d_matrix_handle);
    if(CFE_TBL_INFO_UPDATED == status)
    {
      status = CFE_SUCCESS;
    }
    if(CFE_SUCCESS != status)
    {
      OS_printf("DR: DR_ManageTables(): Error getting address for d matrix table: "
		"status = 0x%08X\n", status);
    }
  }
  
  if (CFE_SUCCESS == status)
  {
    status = CFE_TBL_GetAddress((void *)&dr_wtm_ptr,
				dr_wtm_handle);
    if(CFE_TBL_INFO_UPDATED == status)
    {
      status = CFE_SUCCESS;
    }
    if(CFE_SUCCESS != status)
    {
      OS_printf("DR: DR_ManageTables(): Error getting address for wtm table:"
		"status = 0x%08X\n", status);
    }
  }
  
  return status;
  
}

int32 DR_ChangeMode(int32 new_mode)
{
  // For DR, changing mode basically means loading new tables to use
  // for diagnosis. This functionality was developed but is not in
  // use yet. The intent is that DR will change modes based on receiving
  // an external command. It would also be possible for DR to change
  // modes based on sensor values or even additional LC watchpoint results.
  // Exactly which to implement is a topic for future development. 
  
  int32 status = CFE_SUCCESS;

  char * d_matrix_table_file = "";
  char * wtm_table_file = "";
  char * lc_wdt_table_file = "";
  
  if(CFE_SUCCESS == status)
  {
    // Read the mode definition table to find what files contain
    // the tables we will load, and load them
    int i;
    for(i = 0; i < DR_MAX_NUM_MODES; ++i)
      if(dr_mode_def_ptr[i].mode_index == new_mode)
	break;

    if (i < DR_MAX_NUM_MODES)
    {
      d_matrix_table_file = dr_mode_def_ptr[i].d_matrix_tbl_filename;
      wtm_table_file = dr_mode_def_ptr[i].wtm_tbl_filename;  
      lc_wdt_table_file = dr_mode_def_ptr[i].lc_wdt_tbl_filename;
      if ((!d_matrix_table_file) || (!*d_matrix_table_file) ||
          (!wtm_table_file) || (!*wtm_table_file) ||
          (!lc_wdt_table_file) || (!*lc_wdt_table_file)) {
        OS_printf("DR: mode %ld (table entry %d) is not valid.\n", (long)new_mode, i);
        status = CFE_SEVERITY_ERROR;
      }
    } else {
      OS_printf("DR: mode %ld is not valid (not in the table).\n", (long)new_mode);
      status = CFE_SEVERITY_ERROR;
    }
  }

  // If the request was invalid, avoid doing anything
  // to change state.
  if(CFE_SUCCESS == status)
  {
    // Must release loadable table pointers before making updates
    OS_printf("DR: DR_ChangeMode(): Releasing table addresses\n");
    CFE_TBL_ReleaseAddress(dr_d_matrix_handle);
    CFE_TBL_ReleaseAddress(dr_wtm_handle);
    CFE_TBL_ReleaseAddress(DR_LC_WDTHandle);
    OS_printf("DR: DR_ChangeMode(): Before loading the tables, status = 0x%08X\n", status);
  }

  // Load the two tables that DR manages directly
  if(CFE_SUCCESS == status)
  {
    status = CFE_TBL_Load(dr_d_matrix_handle,
			  CFE_TBL_SRC_FILE, d_matrix_table_file);
  }
  
  if(CFE_SUCCESS == status)
  {
    status = CFE_TBL_Load(dr_wtm_handle,
			  CFE_TBL_SRC_FILE, wtm_table_file);
  }

  // Load the LC watchpoint definition table. Note this one takes
  // quite a while, several seconds. (other tasks can run though.)
  if(CFE_SUCCESS == status)
  {
    status = DR_LoadLcWatchDefTable(lc_wdt_table_file);
  }

  // If the tables loaded successfully, then manage them. I'm not
  // sure what is involved with "managing" them on the cFS side but
  // it looks necessary.
  if(CFE_SUCCESS == status)
  {
    OS_printf("DR: DR_ChangeMode(): About to manage the tables.\n");
    status = DR_ManageTables();
  }
  
  return status;
  
}

int32 DR_LoadLcWatchDefTable(char const * const lc_wdt_table_file)
{

  // Send SB commands to TBL task, to load a new LC table. There are 3
  // commands to send to load that table:
  //  - load the table into an inactive buffer
  //  - verify the table
  //  - activate the table
  // Historical note: When I called the CFE_TBL_Load() directly, I got an error
  // because LC still had the table locked (i.e., still held the address).
  // This way had no errors, but there are complications. As I understand
  // it, the TBL task signals the LC task to do the validation and activiation,
  // so these functions are basically asynchronous. However, cFS gives
  // an error if we attempt to activate the table before LC has finished
  // the validation. So I implemented a function which polls to see when
  // the operations have completed. This typically takes 2 sec or so. But,
  // DR will not be performing diagnosis while this is in progress.
  // Final way, I also found the TBL command handler functions and tried
  // calling them directly in the hopes they would be synchronous that
  // way. (CFE_TBL_LoadCmd(), CFE_TBL_ValidateCmd(), CFE_TBL_ActivateCmd() )
  // It didn't work, it still waited for the LC task to do the
  // table validation and activation. I left the software bus messages
  // in place as the mechanism because that seemed better than calling
  // the TBL task functions directly.
  
  int32 status = CFE_SUCCESS;
  char * lc_wdt_table_name = LC_APP_NAME"."LC_WDT_TABLENAME;

  if(CFE_SUCCESS == status)
  {
    // Send a software bus command to table services to load the new LC table  
    CFE_TBL_LoadCmd_t table_load_cmd;
    
    CFE_SB_InitMsg(&table_load_cmd, CFE_TBL_CMD_MID,
                   sizeof(CFE_TBL_LoadCmd_t), FALSE);
    
    CFE_SB_SetCmdCode( (CFE_SB_Msg_t *)(&table_load_cmd), CFE_TBL_LOAD_CC);
    strncpy(table_load_cmd.Payload.LoadFilename, lc_wdt_table_file,
	    OS_MAX_PATH_LEN);
    
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &table_load_cmd);

    status = DR_WaitForTable(DR_LC_WDTHandle, DR_TABLE_WAIT_TIMEOUT_MILLIS);
    
  }
    
  if(CFE_SUCCESS == status)
  {
    // Send a software bus command to table services to validate the new LC table  
    CFE_TBL_ValidateCmd_t  table_validate_cmd;
    
    CFE_SB_InitMsg(&table_validate_cmd, CFE_TBL_CMD_MID,
                   sizeof(CFE_TBL_ValidateCmd_t), FALSE);
    
    CFE_SB_SetCmdCode( (CFE_SB_Msg_t *)(&table_validate_cmd), CFE_TBL_VALIDATE_CC);
    table_validate_cmd.Payload.ActiveTblFlag = CFE_TBL_INACTIVE_BUFFER;
    strncpy(table_validate_cmd.Payload.TableName, lc_wdt_table_name, CFE_TBL_MAX_FULL_NAME_LEN);
    
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &table_validate_cmd);
    
    OS_printf("DR: DR_ChangeMode(): Waiting on LC table validation...\n");
    status = DR_WaitForTable(DR_LC_WDTHandle, DR_TABLE_WAIT_TIMEOUT_MILLIS);
    
  }
  
  if(CFE_SUCCESS == status)
  {
    // Send a software bus command to table services to activate the LC table
    CFE_TBL_ActivateCmd_t  table_activate_cmd;
    
    CFE_SB_InitMsg(&table_activate_cmd, CFE_TBL_CMD_MID,
                   sizeof(CFE_TBL_ActivateCmd_t), FALSE);
    
    CFE_SB_SetCmdCode( (CFE_SB_Msg_t *)(&table_activate_cmd), CFE_TBL_ACTIVATE_CC);
    strncpy(table_activate_cmd.Payload.TableName, lc_wdt_table_name, CFE_TBL_MAX_FULL_NAME_LEN);
    
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &table_activate_cmd);
    
    OS_printf("DR: DR_ChangeMode(): Waiting on LC table activation...\n");
    status = DR_WaitForTable(DR_LC_WDTHandle, DR_TABLE_WAIT_TIMEOUT_MILLIS);

  }
  
  // Check that it actually was the table that we wanted.
  // It would be nice to have feedback before this point but I'm not
  // sure how to do that. We will compare the filename to see if it
  // matches what we tried to load. 
  if(CFE_SUCCESS == status)
  {
    CFE_TBL_Info_t lc_wdt_info;
    status = CFE_TBL_GetInfo(&lc_wdt_info, lc_wdt_table_name);
    
  
    if(CFE_SUCCESS == status)
    {
      int val = strncmp(lc_wdt_table_file, lc_wdt_info.LastFileLoaded, OS_MAX_PATH_LEN);
      if(0 == val)
      {
	OS_printf("DR: DR_ChangeMode(): LC WDT table ready to go!\n");
      }
      else
      {
	status = DR_WATCHPOINT_TABLE_LOAD_ERROR;
      }
    }
  }

  return status;

}


int32 DR_WaitForTable(CFE_TBL_Handle_t table_handle, uint32 timeout_millis)
{
  // This is not a precise timing, but it doesn't need to be. These delays
  // are on the order of a couple of seconds. 
  int32 loop_time_millis = 0;
  int32 table_status = CFE_SUCCESS;

  // Poll for the table status to be CFE_SUCCESS, indicating the
  // action with the table is complete. Use do-while so that we
  // will delay first, before checking the table status, to give
  // the table task time to respond. Otherwise we were returning
  // immediately before the table action was even started.
  do
  {
    OS_TaskDelay(DR_TABLE_POLL_INTERVAL_MILLIS);
    loop_time_millis += DR_TABLE_POLL_INTERVAL_MILLIS;
    
    table_status = CFE_TBL_GetStatus(table_handle);
    
    if(CFE_SUCCESS == table_status)
    {
      break;
    }    
    
  }
  while(loop_time_millis <= timeout_millis);

  return table_status;
  
}

//Function copied from internet at https://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
// but I changed the name a bit
#ifdef DR_TIMING
struct timespec time_interval(struct timespec start, struct timespec end)
{
  struct timespec temp;
  if (end.tv_nsec < start.tv_nsec)
  {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  }
  else
  {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return temp;
}
#endif
