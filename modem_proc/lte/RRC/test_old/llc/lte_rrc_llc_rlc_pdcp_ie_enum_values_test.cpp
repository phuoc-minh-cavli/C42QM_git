/*!
  @file lte_rrc_llc_mac_ie_enum_values_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of all RLC and 
  PDCP config IEs with enum values, for all possible values and other range value
  checks.

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

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/09   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "__lte_rrc_llc_int.h"
#include "lte_rrc_llci.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  

/*===========================================================================

                                INTERNAL DEFINITIONS

===========================================================================*/
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcRLCPDCPIEEnumValuesTest);

void LteRrcLlcRLCPDCPIEEnumValuesTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, NULL,0);
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcRLCPDCPIEEnumValuesTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_RLC_Config *rlc_cfg_ptr = NULL;
  lte_rrc_osys_PDCP_Config *pdcp_cfg_ptr = NULL;
  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0, rb_cfg_idx = 3; /* for DRB3 */
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: PDCP and RLC Config IE Enum values test begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* Get the LLC internal pointer */
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_drb_am_msg);
  encoded_pdu.value = llc_reconfig_drb_am_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* ---------------------------------------------------------------------- */
  /* Validating RLC Config IE values */
  /* ---------------------------------------------------------------------- */

  rlc_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->rlc_Config;

  /* Validating T_Reordering IE values*/
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_T_REORDERING; i++)
  {
    rlc_cfg_ptr->u.am->dl_AM_RLC.t_Reordering = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_t_reordering(rlc_cfg_ptr->u.am->dl_AM_RLC.t_Reordering,
                                                        &cfg_db_ptr->rlcdl_rb_info[rb_cfg_idx].
                                                        cfg.am_cfg.t_reordering));
  }
  // Check for Spare value
  rlc_cfg_ptr->u.am->dl_AM_RLC.t_Reordering = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_t_reordering(rlc_cfg_ptr->u.am->dl_AM_RLC.t_Reordering,
                                                           &cfg_db_ptr->rlcdl_rb_info[rb_cfg_idx].
                                                           cfg.am_cfg.t_reordering));

  /* ---------------------------------------------------------------------- */
  /* Validating T_Status_Prohibit IE values*/
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_T_STATUS_PROHIBIT; i++)
  {
    rlc_cfg_ptr->u.am->dl_AM_RLC.t_StatusProhibit = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_t_status_prohibit(rlc_cfg_ptr->u.am->dl_AM_RLC.t_StatusProhibit,
                                                             &cfg_db_ptr->rlcdl_rb_info[rb_cfg_idx].
                                                             cfg.am_cfg.t_status_prohibit));
  }
  // Check for Spare value
  rlc_cfg_ptr->u.am->dl_AM_RLC.t_StatusProhibit = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_t_status_prohibit(rlc_cfg_ptr->u.am->dl_AM_RLC.t_StatusProhibit,
                                                                &cfg_db_ptr->rlcdl_rb_info[rb_cfg_idx].
                                                                cfg.am_cfg.t_status_prohibit));

  /* ---------------------------------------------------------------------- */
  /* Validating T_Poll_Retransmit IE values*/
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_T_POLL_RETRANSMIT; i++)
  {
    rlc_cfg_ptr->u.am->ul_AM_RLC.t_PollRetransmit = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_t_poll_retransmit(rlc_cfg_ptr->u.am->ul_AM_RLC.t_PollRetransmit,
                                                             &cfg_db_ptr->rlcul_rb_info[rb_cfg_idx].
                                                             cfg.am_cfg.t_poll_retransmit));
  }
  // Check for Spare value
  rlc_cfg_ptr->u.am->ul_AM_RLC.t_PollRetransmit = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_t_poll_retransmit(rlc_cfg_ptr->u.am->ul_AM_RLC.t_PollRetransmit,
                                                                &cfg_db_ptr->rlcul_rb_info[rb_cfg_idx].
                                                                cfg.am_cfg.t_poll_retransmit));

  /* ---------------------------------------------------------------------- */
  /* Validating T_Poll_PDU IE values*/
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_T_POLL_PDU; i++)
  {
    rlc_cfg_ptr->u.am->ul_AM_RLC.pollPDU = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_t_poll_pdu(rlc_cfg_ptr->u.am->ul_AM_RLC.pollPDU,
                                                      &cfg_db_ptr->rlcul_rb_info[rb_cfg_idx].
                                                      cfg.am_cfg.poll_pdu));
  }

  /* Check for invalid pollPDU value */
  rlc_cfg_ptr->u.am->ul_AM_RLC.pollPDU = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_t_poll_pdu(rlc_cfg_ptr->u.am->ul_AM_RLC.pollPDU,
                                                         &cfg_db_ptr->rlcul_rb_info[rb_cfg_idx].
                                                         cfg.am_cfg.poll_pdu));

  /* ---------------------------------------------------------------------- */
  /* Validating T_Poll_Byte IE values*/
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_T_POLL_BYTE; i++)
  {
    rlc_cfg_ptr->u.am->ul_AM_RLC.pollByte = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_t_poll_byte(rlc_cfg_ptr->u.am->ul_AM_RLC.pollByte,
                                                       &cfg_db_ptr->rlcul_rb_info[rb_cfg_idx].
                                                       cfg.am_cfg.poll_byte));
  }
  // Check for Spare value
  rlc_cfg_ptr->u.am->ul_AM_RLC.pollByte = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_t_poll_byte(rlc_cfg_ptr->u.am->ul_AM_RLC.pollByte,
                                                          &cfg_db_ptr->rlcul_rb_info[rb_cfg_idx].
                                                          cfg.am_cfg.poll_byte));

  /* ---------------------------------------------------------------------- */
  /* Validating T_Retransmission_threshold IE values*/
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_RETX_THRESHOLD; i++)
  {
    rlc_cfg_ptr->u.am->ul_AM_RLC.maxRetxThreshold = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_max_re_tx_threshold(&rlc_cfg_ptr->u.am->ul_AM_RLC,
                                                               &cfg_db_ptr->rlcul_rb_info[rb_cfg_idx].
                                                               cfg.am_cfg.max_retx_threshold));
  }

  /* Check for invalid maxRetxThreshold value */
  rlc_cfg_ptr->u.am->ul_AM_RLC.maxRetxThreshold = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_max_re_tx_threshold(&rlc_cfg_ptr->u.am->ul_AM_RLC,
                                                                  &cfg_db_ptr->rlcul_rb_info[rb_cfg_idx].
                                                                  cfg.am_cfg.max_retx_threshold));

  /* ---------------------------------------------------------------------- */
  /* Validating RLC Config IE values */
  /* ---------------------------------------------------------------------- */

  pdcp_cfg_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
    criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->pdcp_Config;

  /* Validating Discard timer IE values*/
  for (i = 0;i <= LTE_RRC_LLC_TEST_MAX_DISCARD_TIMER; i++)
  {
    pdcp_cfg_ptr->discardTimer = i;
    TF_ASSERT(E_SUCCESS == lte_rrc_llc_get_discard_timer(pdcp_cfg_ptr,
                                                         &cfg_db_ptr->pdcpul_info[rb_cfg_idx].
                                                         discard_timer));
  }

  /* Check for invalid maxRetxThreshold value */
  pdcp_cfg_ptr->discardTimer = i;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_get_discard_timer(pdcp_cfg_ptr,
                                                            &cfg_db_ptr->pdcpul_info[rb_cfg_idx].
                                                            discard_timer));

  /* ---------------------------------------------------------------------- */

  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, 
                             lte_rrc_osys_DL_CCCH_Message_PDU);
  
  TF_MSG("\nLLC: PDCP and RLC Config IE Enum values test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcRLCPDCPIEEnumValuesTest::Teardown()
{
  lte_rrc_utf_teardown();
}











