/*!
  @file
  nb1_tlb_msg.h

  @brief
  This file contains all External message IDs interfaces exported by Test
  Loopback module


*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_tlb_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/26/17   ss      FR 39221 : CR 2038743 Rel.13 LTE Cat.NB1 loopback Modes G

===========================================================================*/

#ifndef NB1_TLB_MSG_H
#define NB1_TLB_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_nb1.h>
#include <lte_app.h>
#include <msgcfg.h>
#include <msg.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @define diag F3 macros for MAC TLB task */
#define NB1_TLB_MSG_SPRINTF_0(xx_ss_mask, xx_fmt)   \
  MSG(MSG_SSID_LTE, xx_ss_mask, xx_fmt)

#define   NB1_TLB_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1)

#define   NB1_TLB_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

#define   NB1_TLB_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define   NB1_TLB_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define   NB1_TLB_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define   NB1_TLB_MSG_SPRINTF_6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_LTE, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

/*-----------------------------------------------------------------------*/
/*     NB1 TLB External Indication messages                              */
/*-----------------------------------------------------------------------*/
/*! @brief external Indications that the NB1 TLB module sends  */
enum
{
  NB1_TLB_FIRST_IND = MSGR_UMID_BASE(MSGR_NB1_TLB, MSGR_TYPE_IND),

  /* Indication send to DS to infor TLB is activated */
  MSGR_DEFINE_UMID( NB1,TLB,IND,LB_ACT,0x00,none),
  /* Indication send to DS to infor TLB is closed */
  MSGR_DEFINE_UMID( NB1,TLB,IND,LB_CLOSE_TL,0x01,none),
  /* Indication send to DS to infor TLB is deactivated */
  MSGR_DEFINE_UMID( NB1,TLB,IND,LB_DEACT,0x02,none),
  /* Indication from NAS which contains message for TLB. This should be
  moved to NAS, but for now, we can leave it here */
  MSGR_DEFINE_UMID( NB1,TLB,IND,DL_INFO,0x03,nb1_tlb_dl_info_ind_s),
  /* Indication send to DS to inform TLB open test loop*/
  MSGR_DEFINE_UMID( NB1,TLB,IND,LB_OPEN,0x04,nb1_tlb_lb_open_ind_s),
  /* Indication from DS to inform TLB that DS is ready for incoming packets*/
  MSGR_DEFINE_UMID( NB1,TLB,IND,DS_CTL_READY,0x05,nb1_tlb_ds_close_tl_ready_ind_s),
  /* Indication from DS to inform TLB that DS is ready for incoming packets*/
  MSGR_DEFINE_UMID( NB1,TLB,IND,SND_DL_PKT,0x06,nb1_ds_dl_enqueue_ind_s),
    /* Indication from DS to inform TLB that DS is ready for incoming packets*/
  MSGR_DEFINE_UMID( NB1,TLB,IND,TIMER_EXP,0x07,none),
  NB1_TLB_MAX_IND,
  NB1_TLB_LAST_IND       = NB1_TLB_MAX_IND - 1
};

/*-----------------------------------------------------------------------*/
/*     NB1 TLB External Request messages                              */
/*-----------------------------------------------------------------------*/
/*! @brief external Request that the NB1 TLB module sends  */
enum
{
  NB1_TLB_FIRST_REQ = MSGR_UMID_BASE(MSGR_NB1_TLB, MSGR_TYPE_REQ),

  MSGR_DEFINE_UMID( NB1,TLB,REQ,UL_INFO,0x00,nb1_tlb_ul_info_req_s),
  MSGR_DEFINE_UMID( NB1,TLB,REQ,RRC_CFG,0x01,nb1_tlb_rrc_cfg_req_s),

  NB1_TLB_MAX_REQ,
  NB1_TLB_LAST_REQ       = NB1_TLB_MAX_REQ - 1
};

/*! @brief external confirmations that the NB1 TLB module receives  */
enum
{
  NB1_TLB_FIRST_CNF = MSGR_UMID_BASE(MSGR_NB1_TLB, MSGR_TYPE_CNF),

  MSGR_DEFINE_UMID( NB1,TLB,CNF,RRC_CFG,0x00,nb1_tlb_rrc_cfg_cnf_s),

  NB1_TLB_MAX_CNF,
  NB1_TLB_LAST_CNF       = NB1_TLB_MAX_CNF - 1
};

/*! @brief Supervisory messages that TLB receives or throws
*/
enum
{
  MSGR_DEFINE_UMID( NB1, TLB, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID( NB1, TLB, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID( NB1, TLB, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY,
                   none),
  MSGR_DEFINE_UMID( NB1, TLB, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL,
                   none)
};

#endif /* NB1_TLB_MSG_H */
