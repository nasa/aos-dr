/////////////////////////////////////////////////////////////////////
// File:
//    dr_events.h 
//
// Purpose: 
//  Define DR App Events IDs
//
// Notes:
//
/////////////////////////////////////////////////////////////////////

#ifndef DR_EVENTS_H
#define DR_EVENTS_H

#ifdef __cplusplus
extern "C" {
#endif

/// The definitions for the events that DR may emit. Some are informational
/// and some are errors.
#define DR_RESERVED_EID              0
#define DR_STARTUP_INF_EID           1
#define DR_STARTUP_ERR_EID           2
#define DR_COMMAND_ERR_EID           3
#define DR_COMMANDNOP_INF_EID        4 
#define DR_COMMANDRST_INF_EID        5
#define DR_INVALID_MSGID_ERR_EID     6 
#define DR_LEN_ERR_EID               7 
#define DR_TBL_SUB_ERR_EID           8
#define DR_GET_TBL_ADDRESS_ERR_EID   9
#define DR_REL_TBL_ADDRESS_ERR_EID  10
#define DR_TASK_EXIT_EID            11
#define DR_MODE_CHANGED_INFO_EID    12
  
#ifdef __cplusplus
} // extern "C" {
#endif

  
#endif // DR_EVENTS_H
