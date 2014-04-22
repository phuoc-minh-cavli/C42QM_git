/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state_registering.c_v   1.2   20 May 2002 11:48:12   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_state_registering.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/20/02   kwa     Added output_name function.
09/19/02   kwa     Added function to process the cm_act_req message.
05/21/03   kwa     Restructured code to eliminate lint warnings.
06/04/03   kwa     Removed one remaining lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added support for band preferences.
11/19/03   kwa     Removed obsolete function for retrieving the state name.
02/04/04   kwa     Added support for PS follow on request.
05/17/04   kwa     Added function to process the mmr_camped_ind message.
07/19/04   kwa     Removed service_state parameter from call to
                   reg_send_cm_camped_ind.
07/26/04   kwa     Added active_rat and active_band parameters to call to
                   reg_send_cm_camped_ind.
02/22/05   up      Updated for Cingular ENS feature.
                   Updated state table to add new function pointer for FCN IND.
09/19/05   up      Updated state table for new primitive CM_HPLMN_SEARCH_REQ.
11/07/05   ajt     Added handling of MMR_EMERGENCY_NUM_LIST_IND in registering state.
04/05/06   sn      Added function process_hplmn_search_timer_expiration to
                   restart 10 sec timer.
07/03/06   sn      Updated process_mmr_camped_ind to update active rat 
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

