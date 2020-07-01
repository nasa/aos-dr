/*******************************************************************************
** File:
**   dr_msg.h 
**
** Purpose: 
**  Define DR Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef dr_msg_h
#define dr_msg_h

#include "dr_types.h"

#ifdef __cplusplus
extern "C" {
#endif


// DR command codes

#define DR_NOOP_CC                 0
#define DR_RESET_COUNTERS_CC       1
#define DR_CHANGE_MODE_CC          2

// Generic "no arguments" command
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} dr_no_args_cmd_type;

// Generic "no arguments" command
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint32   NewMode;
} dr_change_mode_cmd_type; 

// DR housekeeping typedef
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              dr_command_error_count;
    uint8              dr_command_count;
} dr_hk_tlm_type;
  
#define DR_HK_TLM_LNGTH   sizeof ( dr_hk_tlm_type )

// DR diagnosis struct
typedef struct
{
  /** The cFS message header */
  uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
  /** Whether an error occurred in the diagnosis */
  dr_error_type  error;
  /** The number of failure modes in the diagnosis */
  uint32_t num_failure_modes;
  /** The good/bad state of each failure mode */
  dr_failure_mode_type failure_modes[DR_MAX_FAILURE_MODES];
} dr_diagnosis_msg_type;  


#ifdef __cplusplus
} // extern "C" {
#endif

#endif // dr_msg_h 

/************************/
/*  End of File Comment */
/************************/
