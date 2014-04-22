/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state_manual_search.c_v   1.4   07 Jun 2002 14:23:56   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_state_manual_search.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/20/02   kwa     Added output_name function.
05/29/02   kwa     Changed mode when call routine to prioritize PLMN list.
06/06/02   kwa     Added logic to handle sending multiple mmr_plmn_search_req
                   messages when operating in dual mode.
06/27/02   kwa     Modified function that sends an mmr_plmn_search_req due to
                   interface change.
09/19/02   kwa     Added function to process the cm_act_req message.
11/04/02   kwa     Eliminated call to start plmn selection after receiving
                   the available PLMN list from MM.
12/30/02   kwa     Modified F3 message to fit within 40 characters.
05/21/03   kwa     Restructured code to eliminate lint warnings.
05/22/03   kwa     Modified process_mmr_plmn_search_cnf to pass a const
                   pointer to eliminate a lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added support for band preferences.
11/16/03   kwa     Modified process_mmr_plmn_search_cnf so the available PLMN
                   list stored in reg_mode.c is used during manual PLMN
                   selection. 
11/19/03   kwa     Removed obsolete function for retrieving the state name.
04/28/04   kwa     Added reg_state_manual_search_mode_pref so the mode
                   preference received in the cm_network_list_req is used to
                   prioritize the available PLMN list.
05/17/04   kwa     Added function to process the mmr_camped_ind message.
11/12/04   kwa     Added support for FEATURE_GTOW_BACKGROUND_PLMN_SEARCH in
                   process_mmr_plmn_search_cnf.
02/22/05   up      Updated for Cingular ENS feature.
                   Updated state table to add new function pointer for FCN IND.
                   Updated function process_mmr_plmn_search_cnf.
09/19/05   up      Updated state table for new primitive CM_HPLMN_SEARCH_REQ.
10/24/05   up      Ignore Auto PLMN Search while waiting for MANUAL Search CNF.
11/07/05   ajt     Added handling of MMR_EMERGENCY_NUM_LIST_IND in Manual Search state.
12/08/05   up      Send status of Manual Search in cm_network_list_cnf.
01/05/06   sn      Modified structure reg_state_manual_search. Added new
                   function process_hplmn_search_timer_expiration.
01/18/06   up      Updated process_mmr_plmn_search_cnf to ignore
                   PLMN_SEARCH_CNF of OLD request (old TI).
02/20/06   sn      EHPLMN Feature
04/18/06   sn      Added function process_mmr_service_ind to add PLMN to 
                   forbidden list when received respective reject cause.
10/12/06   sn      WCDMA_1X mode preference implementation.
11/23/06   sn      Added feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
11/23/06   sn      FEATURE_PRESERVE_MANUAL_PLMN_LIST `	
09/20/11   sla     Store the PLMn id of the cell where GSTK event is sent 
09/20/11   sla    Store last gstk service status sent.send IDLE event to GSK 
                  if current service status and last gstk sent service status
                  are different 
3/26/13   c_mg    Ported CR-447611
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

