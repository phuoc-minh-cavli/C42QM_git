/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state_inactive.c_v   1.2   20 May 2002 11:47:42   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_state_inactive.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/20/02   kwa     Added output_name function.
08/06/02   kwa     Updated SIM state when cm_sim_available_req is received.
09/19/02   kwa     Added function to process the cm_act_req message.
05/21/03   kwa     Restructured code to eliminate lint warnings.
                   Modified reg_state_e_type process_cm_act_req to pass a
                   const pointer to eliminate a lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added support for band preferences.
11/19/03   kwa     Removed obsolete function for retrieving the state name.
03/17/04   kwa     Added call to reg_mode_initialize_equivalent_plmn_list in
                   process_cm_sim_available_req.
05/17/04   kwa     Added function to process the mmr_camped_ind message.
08/17/04   kwa     Removed call to reg_state_update_sim_state from
                   process_cm_sim_available_req.
02/22/05   up      Updated for Cingular ENS feature.
                   Updated state table to add new function pointer for FCN IND.
                   Updated function process_cm_sim_available_req.
09/19/05   up      Updated state table for new primitive CM_HPLMN_SEARCH_REQ.
10/27/05   up      Send CM_SIM_AVAILABLE_CNF even when SIM access failed.
11/07/05   ajt     Added handling of MMR_EMERGENCY_NUM_LIST_IND in inactive state.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "msg.h"
#include "reg_mode.h"
#include "reg_send.h"
#include "reg_sim_v.h"
#include "reg_state.h"
#include "reg_timers.h"
#include "mm_v.h"

#include "reg_nv.h"
#include "ULogFront.h"


/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/


