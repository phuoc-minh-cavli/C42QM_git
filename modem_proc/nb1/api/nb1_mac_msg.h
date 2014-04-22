/*!
  @file
  nb1_mac_msg.h

  @brief
  This file contains all External message IDs interfaces exported by MAC layer.


*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_mac_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/21   ve      CR 2869397 : FR68338:Idle mode RACH improvement changes
04/25/17   ss      CR 2038228 : MSG1 deadlock timer expiry debug support for ML1

===========================================================================*/

#ifndef NB1_MAC_MSG_H
#define NB1_MAC_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_nb1.h>
#include <lte_app.h>
#include <nb1_ind_msg.h>
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*-----------------------------------------------------------------------*/
/*     NB1 MAC Supervisory                                               */
/*-----------------------------------------------------------------------*/
/*! @brief Supervisory messages the NB1 MAC module receives.  */

/*-----------------------------------------------------------------------*/
/*     NB1 MAC External Request messages                                 */
/*-----------------------------------------------------------------------*/
/*! @brief external requests that the NB1 MAC module receives.  */
enum
{
  NB1_MAC_FIRST_REQ = MSGR_UMID_BASE(MSGR_NB1_MAC, MSGR_TYPE_REQ),

  MSGR_DEFINE_UMID(NB1,MAC,REQ,ACCESS,0x00,
                   nb1_mac_access_req_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,REQ,CFG,0x01,
                   nb1_mac_cfg_req_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,REQ,ACCESS_ABORT,0x02,
                   nb1_mac_access_abort_req_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,REQ,CANCEL_CONN,0x03,
                   nb1_mac_cancel_conn_req_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,REQ,FC_UL,0x04,
                   none),
  MSGR_DEFINE_UMID(NB1,MAC,REQ,START,0x05,
                   nb1_mac_start_req_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,REQ,STOP,0x06,
                   nb1_mac_stop_req_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,REQ,STATS_UPDATE,0x07,
                   nb1_mac_stats_update_req_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,REQ,RACH_RPT,0x08,
                   nb1_mac_rach_rpt_req_msg_s),
  NB1_MAC_MAX_REQ,
  NB1_MAC_LAST_REQ       = NB1_MAC_MAX_REQ - 1
};

/*-----------------------------------------------------------------------*/
/*     NB1 MAC External Confirmation messages                            */
/*-----------------------------------------------------------------------*/
/*! @brief external Confirmations that the NB1 MAC module sends  */
enum
{
  NB1_MAC_FIRST_CNF = MSGR_UMID_BASE(MSGR_NB1_MAC, MSGR_TYPE_CNF),

  MSGR_DEFINE_UMID(NB1,MAC,CNF,ACCESS,0x00,
                   nb1_mac_access_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,CNF,CFG,0x01,
                   nb1_mac_cfg_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,CNF,ACCESS_ABORT,0x02,
                   nb1_mac_access_abort_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,CNF,START,0x03,
                   nb1_mac_start_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,CNF,STOP,0x04,
                   nb1_mac_stop_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,CNF,STATS_UPDATE,0x05,
                   nb1_mac_stats_update_cnf_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,CNF,RACH_RPT,0x06,
                   nb1_mac_rach_rpt_cnf_msg_s),

  NB1_MAC_MAX_CNF,
  NB1_MAC_LAST_CNF       = NB1_MAC_MAX_CNF - 1
};

/*-----------------------------------------------------------------------*/
/*     NB1 MAC External Indication messages                              */
/*-----------------------------------------------------------------------*/
/*! @brief external Indications that the NB1 MAC module sends  */
enum
{
  /* note that all the truely broastcast indications will be defined
  in nb1_ind_msg.h. The IDNUM range is from 0x00 to the one below - 1 */
  MSGR_DEFINE_UMID(NB1,MAC,IND,RANDOM_ACCESS_PROBLEM,0x02,
                   nb1_mac_random_access_problem_ind_msg_s),
  MSGR_DEFINE_UMID(NB1,MAC,IND,RLC_DL_DATA,0x3,
                   msgr_hdr_s),
  MSGR_DEFINE_UMID(NB1,MAC,IND,RRC_PCCH_DL_DATA,0x04,
                   nb1_mac_rrc_dl_data_ind_s),
  MSGR_DEFINE_UMID(NB1,MAC,IND,RRC_CCCH_DL_DATA,0x05,
                   nb1_mac_rrc_dl_data_ind_s),
  MSGR_DEFINE_UMID(NB1,MAC,IND,RRC_BCCH_DL_DATA,0x06,
                   nb1_mac_rrc_dl_data_ind_s),
  MSGR_DEFINE_UMID(NB1,MAC,IND,A2_DL_PHY_DATA,0x07,
                   nb1_mac_a2_dl_phy_data_ind_s),
  MSGR_DEFINE_UMID(NB1,MAC,IND,RELEASE_RESOURCES,0x08,
                   nb1_mac_release_resources_ind_s),
  MSGR_DEFINE_UMID(NB1,MAC,IND,BCAST_DATA,0x09,
                   nb1_mac_bcast_data_ind_s),
  MSGR_DEFINE_UMID(NB1,MAC,IND,DUAL_DATA,0xA,
                   nb1_mac_dual_data_ind_s),
  MSGR_DEFINE_UMID(NB1,MAC,IND,LCID_PRIORITY,0xB,
                   nb1_mac_lcid_priority_ind_s),
  MSGR_DEFINE_UMID(NB1,MAC,IND,MAX_HARQ_RETX,0xC,
                   nb1_mac_max_harq_retx_ind_s),
  MSGR_DEFINE_UMID(NB1,MAC,IND,MSG5_GRANT,0xD,
                   nb1_mac_rrc_msg5_grant_ind_s),    
};

/*! @brief Supervisory messages that MAC receives or throws
*/
enum
{
  MSGR_DEFINE_UMID(NB1, MAC, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(NB1, MAC, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(NB1, MAC, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY,
                   none),
  MSGR_DEFINE_UMID(NB1, MAC, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL,
                   none)
};

/* TODO Debug API at MAC to help ML1 analyse MSG1 deadlock timer expiry 
   which is called by RRC to before asserting 
*/
extern void nb1_mac_rach_request_deadock_debug_rrc_cb
(
  void
);

#endif /* NB1_MAC_MSG_H */
