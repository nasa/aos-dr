//////////////////////////////////////////////////////////////////////////
// File: dr_app.h
//
// Purpose:
//   This file is main header file for the Diagnostic Reasoner (DR)
//   cFS application.
//
//////////////////////////////////////////////////////////////////////////

/*******************************************************************
Notices:

Copyright © 2020 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.

Disclaimers:

No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."

Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
************************************************************************/

#ifndef DR_APP_H
#define DR_APP_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "dr_msg.h"
#include "dr_types.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/// The number of messages possible in the DR app's message pipe
#define DR_PIPE_DEPTH   32

/// The text name of the DR app
#define DR_APP_NAME    "DR_APP"

// The text name of the DR mode definition table
#define DR_MODE_DEF_NAME "DR_MODE_DEF"
  
/// The text name of the DR D-matrix table
#define DR_D_MATRIX_NAME "DR_D_MATRIX"

/// The text name of the DR watchpoint->test mapping table
#define DR_WTM_NAME  "DR_WTM"

/// The filename to use for attempting to load the mode definition table
#define DR_MODE_DEF_DEFAULT_FILENAME "/cf/dr_mode_def.tbl"

//////////////////////////////////////////////////////////////////////
// Type Definitions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Public function prototypes
//////////////////////////////////////////////////////////////////////

/// The main entry point of the DR app.  
void DR_AppMain(void);

  
//////////////////////////////////////////////////////////////////////
// Global Data
//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // dr_app_h
