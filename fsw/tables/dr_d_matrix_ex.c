//
// File defining an example D-matrix table. Follows the example
// D-matrix in the DR user's manual.
//

#include "cfe_tbl_filedef.h"
#include "dr_d_matrix_tbl.h"
#include "dr_app.h"

/*************************************************************************
** Exported Data
*************************************************************************/
/*
** Table file header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
  "dr_d_matrix_ex", DR_APP_NAME "." DR_D_MATRIX_NAME,
    "DR d-matrix ex", "dr_d_matrix_ex.tbl",
    (sizeof(dr_d_matrix_tbl_type))
};

dr_d_matrix_tbl_type dr_d_matrix_ex =
{ 4,
  5,
  { { 1, 1, 1, 1 },
    { 0, 1, 1, 0 },
    { 0, 1, 1, 0 },
    { 0, 0, 0, 1 },
    { 0, 0, 1, 0 } }
};

