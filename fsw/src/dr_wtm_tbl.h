#ifndef DR_WTM_TBL_H
#define DR_WTM_TBL_H

#include "cfe.h"
#include "stdbool.h"

#include "dr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Table to define the LC Watchpoint to DR Test mapping
//

#define DR_MAX_CLEAR_CONDS  8

/// Define an enumerated type to specify the available types of tests
typedef enum
{
  DR_VALUE_TEST,
  DR_STALENESS_TEST
} dr_test_type_type;

/// Define an struct to hold the test latching info
typedef struct 
{
  uint32  latch_clear_count;
  uint32  latch_clear_indices[DR_MAX_CLEAR_CONDS];
} dr_latch_clear_type;
  
/// Define an entry in the watchpoint to test mapping table
typedef struct
{
  /// The index of the test result we are calculating
  uint32 test_index;
  /// The index of the limit checker watchpoint to use
  /// in calculating the test result
  uint32 watchpoint_index;
  /// Indicator if the test is active or disabled; if disabled
  /// the test result will be "unknown"
  bool   test_active;
  /// If there is a separate watchpoint to indicate if this test
  /// is currently valid, this will be the index of it, if not
  /// this index should be set to UINT32_MAX and the watchpoint will
  /// always be valid. If the index is not UINT32_MAX, DR checks
  /// the WatchResult of this index. If it has a WatchResult !=0,
  /// this test is valid.
  uint32 test_valid_watchpoint_index;
  /// Indicates if this test is a latching type, which keeps
  /// the test result value even if the watchpoint returns
  /// to indicating "unknown"
  bool   test_latching;
  /// Defines the type of test, value test or staleness test. 
  dr_test_type_type test_type;
  /// Defines the latching clear information.
  dr_latch_clear_type latch_clear;
  
} dr_wtm_entry_type;

#ifdef __cplusplus
}
#endif

#endif // DR_WTM_TBL_H
