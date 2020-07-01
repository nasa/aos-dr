#ifndef DR_MODE_DEF_H
#define DR_MODE_DEF_H

#include "cfe.h"
#include "dr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Definitions for the maximum number of modes and the maximum filename
/// length. Used as array sizes in the code.
#define DR_MAX_NUM_MODES 10
#define DR_MAX_MODE_TBL_FILENAME_LENGTH 60

/// Defines an entry in the mode definition table
typedef struct
{
  /// The index which identifies this mode.
  int32 mode_index;
  
  /// The filename for this mode's d-matrix table.
  char d_matrix_tbl_filename[DR_MAX_MODE_TBL_FILENAME_LENGTH];

  /// The filename for this mode's watchpoint to test mapping table.
  char wtm_tbl_filename[DR_MAX_MODE_TBL_FILENAME_LENGTH];

  /// The filename for this mode's limit checker watchpoint definition table.
  char lc_wdt_tbl_filename[DR_MAX_MODE_TBL_FILENAME_LENGTH];
  
} dr_mode_def_entry_type;

#ifdef __cplusplus
}
#endif

#endif // DR_MODE_DEF_H
