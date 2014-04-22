/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state_deregistering.c_v   1.3   06 Jun 2002 10:50:36   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_state_deregistering.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/20/02   kwa     Added output_name function.
06/05/02   kwa     Filled roaming_ind upon receiving an mmr_service_ind.
09/19/02   kwa     Added function to process the cm_act_req message.
05/21/03   kwa     Restructured code to eliminate lint warnings.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
11/11/03   kwa     Added process_mmr_stop_mode_cnf.
11/19/03   kwa     Removed obsolete function for retrieving the state name.
05/17/04   kwa     Added function to process the mmr_camped_ind message.
02/22/05   up      Updated for Cingular ENS feature.
                   Updated state table to add new function pointer for FCN IND.
09/19/05   up      Updated state table for new primitive CM_HPLMN_SEARCH_REQ.
11/07/05   ajt     Added handling of MMR_EMERGENCY_NUM_LIST_IND in deregistering state.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

