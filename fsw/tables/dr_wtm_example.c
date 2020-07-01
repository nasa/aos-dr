//
// An example watchpoint-to-test mapping table. Matches the example
// D-matrix table, with 4 tests. Each entry in this table maps an
// LC watchpoint into a DR test. Note that the LC watchpoints must
// be defined in the LC watchpoint definition table.
//

#include "cfe_tbl_filedef.h"
#include "dr_wtm_tbl.h"
#include "dr_app.h"

/*************************************************************************
** Exported Data
*************************************************************************/
/*
** Table file header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
  "dr_wtm_example", DR_APP_NAME "." DR_WTM_NAME,
    "DR wtm table ex", "dr_wtm_example.tbl",
    (sizeof(dr_wtm_entry_type)*DR_MAX_TESTS)
};


dr_wtm_entry_type dr_wtm_example[DR_MAX_TESTS] = 
{ 
  // Example data test index 0
  {
    .test_index = 0U,
    .watchpoint_index = 2U,
    .test_active = true,
    .test_latching = false,
    .test_type = DR_VALUE_TEST,
    .latch_clear.latch_clear_count = 0
  },
  // Example data test index 1
  {
    .test_index = 1U,
    .watchpoint_index = 0U,
    .test_active = true,
    .test_latching = false,
    .test_type = DR_VALUE_TEST,
    .latch_clear.latch_clear_count = 0
  },
  // Example data test index 2
  {
    .test_index = 2U,
    .watchpoint_index = 3U,
    .test_active = true,
    .test_latching = false,
    .test_type = DR_VALUE_TEST,
    .latch_clear.latch_clear_count = 0
  },
  // Example data test index 3
  {
    .test_index = 3U,
    .watchpoint_index = 1U,
    .test_active = true,
    .test_latching = false,
    .test_type = DR_VALUE_TEST,
    .latch_clear.latch_clear_count = 0
  },
};
