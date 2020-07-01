
#include "cfe_tbl_filedef.h"
#include "dr_mode_def.h"
#include "dr_app.h"

/*
** Table file header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
  "dr_mode_def", DR_APP_NAME "." DR_MODE_DEF_NAME,
    "DR mode def table", "dr_mode_def.tbl",
  (sizeof(dr_mode_def_entry_type)*DR_MAX_NUM_MODES)
};

dr_mode_def_entry_type dr_mode_def[DR_MAX_NUM_MODES] =
{
  {
    .mode_index = 0,
    .d_matrix_tbl_filename = "/cf/dr_d_matrix_ex.tbl",
    .wtm_tbl_filename = "/cf/dr_wtm_example.tbl",
    .lc_wdt_tbl_filename  = "/cf/lc_def_wdt_ex.tbl"
  }, 
  {
    .mode_index = 1,
    .d_matrix_tbl_filename = "",
    .wtm_tbl_filename = "",
    .lc_wdt_tbl_filename  = ""
  },
  {
    .mode_index = 2,
    .d_matrix_tbl_filename = "",
    .wtm_tbl_filename = "",
    .lc_wdt_tbl_filename  = ""
  },
  {
    .mode_index = 3,
    .d_matrix_tbl_filename = "",
    .wtm_tbl_filename = "",
    .lc_wdt_tbl_filename  = ""
  },
  {
    .mode_index = 4,
    .d_matrix_tbl_filename = "",
    .wtm_tbl_filename = "",
    .lc_wdt_tbl_filename  = ""
  },
  {
    .mode_index = 5,
    .d_matrix_tbl_filename = "",
    .wtm_tbl_filename = "",
    .lc_wdt_tbl_filename  = ""
  },
  {
    .mode_index = 6,
    .d_matrix_tbl_filename = "",
    .wtm_tbl_filename = "",
    .lc_wdt_tbl_filename  = ""
  },
  {
    .mode_index = 7,
    .d_matrix_tbl_filename = "",
    .wtm_tbl_filename = "",
    .lc_wdt_tbl_filename  = ""
  },
  {
    .mode_index = 8,
    .d_matrix_tbl_filename = "",
    .wtm_tbl_filename = "",
    .lc_wdt_tbl_filename  = ""
  },
  {
    .mode_index = 9,
    .d_matrix_tbl_filename = "",
    .wtm_tbl_filename = "",
    .lc_wdt_tbl_filename  = ""
  },

  
};
