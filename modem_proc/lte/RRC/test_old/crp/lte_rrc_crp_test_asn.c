/*!
  @file lte_rrc_crp_test_asn.c

  @brief
  File with ASN operations required due to as1.h not being able to be 
  included in cpp files.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/crp/lte_rrc_crp_test_asn.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/2008 sureshs Removed conn rel msg free function
10/09/2008 ask     Change to set pdu_num when sending mh_dlm_s
09/15/2008 sureshs Refactored connection release message construction routine
09/15/2008 sureshs Function for including redirect information
09/13/2008 sureshs Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte_rrc_int_msg.h>
#include <lte_static_rrc_mem_util.h>
#include <lte_rrc_utils.h>
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_rrc_crp_test_construct_conn_rel_msg

===========================================================================*/
/*!
    @brief
    Constructs a Connection Release DLM by dynamic allocation 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_crp_test_construct_conn_rel_msg
(
  lte_rrc_mh_dlm_s *msg_ptr, /*!< Pointer to constructed message */
  boolean tau_is_reqd, /*!< Is load balancing TAU required */
  boolean redirect_info_is_present, /*!< Is redirect info present */
  boolean mobility_info_is_present /*!< Is mobility info present */
)
{
  lte_rrc_osys_DL_DCCH_Message *dl_msg_ptr;
  lte_rrc_osys_RRCConnectionRelease_r8_IEs *conn_rel_r8_ptr;

  lte_rrc_init_default_hdr(&msg_ptr->msg_hdr,
                           LTE_RRC_RRC_CONNECTION_RELEASE_DLM);

  msg_ptr->sfn.sys_fn = 100;
  msg_ptr->pdu_num = lte_rrc_osys_DL_DCCH_Message_PDU;
  memset(&msg_ptr->cell_id,0,sizeof(msg_ptr->cell_id));

  msg_ptr->dl_msg_ptr = lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DL_DCCH_Message));
  dl_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *) msg_ptr->dl_msg_ptr;
  dl_msg_ptr->message.t = T_lte_rrc_osys_DL_DCCH_MessageType_c1;

  dl_msg_ptr->message.u.c1 = (lte_rrc_osys_DL_DCCH_MessageType_c1 *)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DL_DCCH_MessageType_c1));
  dl_msg_ptr->message.u.c1->t = T_lte_rrc_osys_DL_DCCH_MessageType_c1_rrcConnectionRelease;

  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease = (lte_rrc_osys_RRCConnectionRelease *)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_RRCConnectionRelease));
  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->criticalExtensions.t =
    T_lte_rrc_osys_RRCConnectionRelease_criticalExtensions_c1;
  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->rrc_TransactionIdentifier = 1;

  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->
                     criticalExtensions.u.c1 = 
    (lte_rrc_osys_RRCConnectionRelease_criticalExtensions_c1 *)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_RRCConnectionRelease_criticalExtensions_c1));

  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->criticalExtensions.u.c1->t = 
    T_lte_rrc_osys_RRCConnectionRelease_criticalExtensions_c1_rrcConnectionRelease_r8;

  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->
                     criticalExtensions.u.c1->u.rrcConnectionRelease_r8 = 
    (lte_rrc_osys_RRCConnectionRelease_r8_IEs *)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_RRCConnectionRelease_r8_IEs));  
  conn_rel_r8_ptr = dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->
                     criticalExtensions.u.c1->u.rrcConnectionRelease_r8;

  if (tau_is_reqd)
  {
    conn_rel_r8_ptr->releaseCause = lte_rrc_osys_ReleaseCause_loadBalancingTAUrequired;
  }
  else
  {
    conn_rel_r8_ptr->releaseCause = lte_rrc_osys_ReleaseCause_other;
  }

  conn_rel_r8_ptr->m.idleModeMobilityControlInfoPresent = 0;
  conn_rel_r8_ptr->m.redirectedCarrierInfoPresent = 0;
  conn_rel_r8_ptr->m.nonCriticalExtensionPresent = 0;

  if (redirect_info_is_present)
  {
    conn_rel_r8_ptr->m.redirectedCarrierInfoPresent = 1;
    conn_rel_r8_ptr->redirectedCarrierInfo.t = 
      T_lte_rrc_osys_RedirectedCarrierInfo_cdma2000_HRPD;
    conn_rel_r8_ptr->redirectedCarrierInfo.u.cdma2000_HRPD = 
      (lte_rrc_osys_CarrierFreqCDMA2000 *)
        lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CarrierFreqCDMA2000));
    conn_rel_r8_ptr->redirectedCarrierInfo.u.cdma2000_HRPD->arfcn = 2000;
    conn_rel_r8_ptr->redirectedCarrierInfo.u.cdma2000_HRPD->bandClass = 
      lte_rrc_osys_BandclassCDMA2000_bc0;
  }

  if (mobility_info_is_present)
  {
    conn_rel_r8_ptr->m.idleModeMobilityControlInfoPresent = 1;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.m.t320Present = 1;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.t320 = lte_rrc_osys_IdleModeMobilityControlInfo_t320_min5;
  }
} /* lte_rrc_crp_test_construct_conn_rel_msg */

#if 0
/*===========================================================================

  FUNCTION:  lte_rrc_crp_test_construct_conn_rel_msg_with_g_freqPriorityList

===========================================================================*/
/*!
    @brief
    Constructs a Connection Release DLM by dynamic allocation 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_crp_test_construct_conn_rel_msg_with_g_freqPriorityList
(
  lte_rrc_mh_dlm_s *msg_ptr, /*!< Pointer to constructed message */
  uint8 list_type
)
{
  lte_rrc_osys_DL_DCCH_Message *dl_msg_ptr;
  lte_rrc_osys_RRCConnectionRelease_r8_IEs *conn_rel_r8_ptr;
  lte_rrc_osys_ARFCN_ValueGERAN *arfcn_valueGERAN_ptr;
  lte_rrc_osys_ARFCN_ValueGERAN *arfcn_valueGERAN_ptr_2;
  char value[] = {0x08, 0x40, 0x36};
  char value_2[] = {0xF0};

  lte_rrc_init_default_hdr(&msg_ptr->msg_hdr,
                           LTE_RRC_RRC_CONNECTION_RELEASE_DLM);

  msg_ptr->sfn.sys_fn = 100;
  msg_ptr->pdu_num = lte_rrc_osys_DL_DCCH_Message_PDU;
  memset(&msg_ptr->cell_id,0,sizeof(msg_ptr->cell_id));

  msg_ptr->dl_msg_ptr = lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DL_DCCH_Message));
  dl_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *) msg_ptr->dl_msg_ptr;
  dl_msg_ptr->message.t = T_lte_rrc_osys_DL_DCCH_MessageType_c1;

  dl_msg_ptr->message.u.c1 = (lte_rrc_osys_DL_DCCH_MessageType_c1 *)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DL_DCCH_MessageType_c1));
  dl_msg_ptr->message.u.c1->t = T_lte_rrc_osys_DL_DCCH_MessageType_c1_rrcConnectionRelease;

  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease = (lte_rrc_osys_RRCConnectionRelease *)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_RRCConnectionRelease));
  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->criticalExtensions.t =
    T_lte_rrc_osys_RRCConnectionRelease_criticalExtensions_c1;
  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->rrc_TransactionIdentifier = 1;

  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->
                     criticalExtensions.u.c1 = 
    (lte_rrc_osys_RRCConnectionRelease_criticalExtensions_c1 *)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_RRCConnectionRelease_criticalExtensions_c1));

  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->criticalExtensions.u.c1->t = 
    T_lte_rrc_osys_RRCConnectionRelease_criticalExtensions_c1_rrcConnectionRelease_r8;

  dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->
                     criticalExtensions.u.c1->u.rrcConnectionRelease_r8 = 
    (lte_rrc_osys_RRCConnectionRelease_r8_IEs *)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_RRCConnectionRelease_r8_IEs));  
  conn_rel_r8_ptr = dl_msg_ptr->message.u.c1->u.rrcConnectionRelease->
                     criticalExtensions.u.c1->u.rrcConnectionRelease_r8;

  conn_rel_r8_ptr->releaseCause = lte_rrc_osys_ReleaseCause_other;

  conn_rel_r8_ptr->m.idleModeMobilityControlInfoPresent = 0;
  conn_rel_r8_ptr->m.redirectedCarrierInfoPresent = 0;
  conn_rel_r8_ptr->m.nonCriticalExtensionPresent = 0;

  conn_rel_r8_ptr->m.idleModeMobilityControlInfoPresent = 1;
  conn_rel_r8_ptr->idleModeMobilityControlInfo.m.t320Present = 1;
  conn_rel_r8_ptr->idleModeMobilityControlInfo.t320 = lte_rrc_osys_IdleModeMobilityControlInfo_t320_min5;
  conn_rel_r8_ptr->idleModeMobilityControlInfo.m.freqPriorityListGERANPresent = 1;
  if (list_type == 1) {
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.n = 2;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem = 
      (lte_rrc_osys_FreqsPriorityGERAN *)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_FreqsPriorityGERAN) * conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.n);
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.startingARFCN = 512;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.bandIndicator = 
         lte_rrc_osys_BandIndicatorGERAN_dcs1800;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].cellReselectionPriority = 5;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.t = 
         T_lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_explicitListOfARFCNs;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.explicitListOfARFCNs = 
    (struct lte_rrc_osys_ExplicitListOfARFCNs*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(struct lte_rrc_osys_ExplicitListOfARFCNs));
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.explicitListOfARFCNs->n = 2;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.explicitListOfARFCNs->elem = 
          (lte_rrc_osys_ARFCN_ValueGERAN *) lte_rrc_asn1_osys_IE_malloc(
        conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.explicitListOfARFCNs->n *
        sizeof(lte_rrc_osys_ARFCN_ValueGERAN));
    arfcn_valueGERAN_ptr = 
        conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.explicitListOfARFCNs->elem;
    *arfcn_valueGERAN_ptr++ = 520;
    *arfcn_valueGERAN_ptr++ = 525;
        conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.startingARFCN = 536;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.bandIndicator = 
         lte_rrc_osys_BandIndicatorGERAN_dcs1800;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].cellReselectionPriority = 6;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.t = 
         T_lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_explicitListOfARFCNs;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.explicitListOfARFCNs = 
    (struct lte_rrc_osys_ExplicitListOfARFCNs*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(struct lte_rrc_osys_ExplicitListOfARFCNs));
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.explicitListOfARFCNs->n = 2;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.explicitListOfARFCNs->elem = 
          (lte_rrc_osys_ARFCN_ValueGERAN *) lte_rrc_asn1_osys_IE_malloc(
        conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.explicitListOfARFCNs->n *
        sizeof(lte_rrc_osys_ARFCN_ValueGERAN));
    arfcn_valueGERAN_ptr_2 = 
        conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.explicitListOfARFCNs->elem;
    *arfcn_valueGERAN_ptr_2++ = 541;
    *arfcn_valueGERAN_ptr_2   = 542;
  }
  else if (list_type == 2)
  {
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.n = 2;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem = 
      (lte_rrc_osys_FreqsPriorityGERAN *)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_FreqsPriorityGERAN) * conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.n);
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.startingARFCN = 512;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.bandIndicator = 
         lte_rrc_osys_BandIndicatorGERAN_dcs1800;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].cellReselectionPriority = 5;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.t = 
         T_lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_equallySpacedARFCNs;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.equallySpacedARFCNs = 
    (struct lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_equallySpacedARFCNs*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(struct lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_equallySpacedARFCNs));
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.equallySpacedARFCNs->numberOfFollowingARFCNs = 2;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.equallySpacedARFCNs->arfcn_Spacing = 5;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.startingARFCN = 526;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.bandIndicator = 
         lte_rrc_osys_BandIndicatorGERAN_pcs1900;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].cellReselectionPriority = 2;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.t = 
         T_lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_equallySpacedARFCNs;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.equallySpacedARFCNs = 
    (struct lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_equallySpacedARFCNs*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(struct lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_equallySpacedARFCNs));
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.equallySpacedARFCNs->numberOfFollowingARFCNs = 2;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.equallySpacedARFCNs->arfcn_Spacing = 2;
  }
  else if (list_type == 3)
  {
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.n = 2;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem = 
      (lte_rrc_osys_FreqsPriorityGERAN *)
      lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_FreqsPriorityGERAN) * conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.n);
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.startingARFCN = 512;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.bandIndicator = 
         lte_rrc_osys_BandIndicatorGERAN_pcs1900;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].cellReselectionPriority = 5;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.t = 
         T_lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs = 
    (struct lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(struct lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs));
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->numocts = 3;
    
    memcpy(conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->data,
         value,
         conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[0].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->numocts);
        conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.startingARFCN = 537;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.bandIndicator = 
         lte_rrc_osys_BandIndicatorGERAN_pcs1900;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].cellReselectionPriority = 7;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.t = 
         T_lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs;
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs = 
    (struct lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs*)
      lte_rrc_asn1_osys_IE_malloc(sizeof(struct lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs));
    conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->numocts = 1;
    
    memcpy(conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->data,
         value_2,
         conn_rel_r8_ptr->idleModeMobilityControlInfo.freqPriorityListGERAN.elem[1].carrierFreqs.followingARFCNs.u.variableBitMapOfARFCNs->numocts);
  }
} /* lte_rrc_crp_test_construct_conn_rel_msg_with_g_freqPriorityList */
#endif

