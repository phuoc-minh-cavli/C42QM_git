/*!
  @file lte_rrc_llc_phy_common_ie_enum_values_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of all PHY 
  common config IEs, including those that come as part of mobility control
  information, with enum values, for all possible values and other range value
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
07/13/09   mm     Initial Revision
           
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
	
							 INTERNAL VARIABLES
	
===========================================================================*/
	
extern "C"
{
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhrAndEphrTest);

void LteRrcLlcPhrAndEphrTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcPhrAndEphrTest::Test()
{

  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  ASN1BitStr32 *osys_bitstring_ptr = NULL ;
  lte_rrc_llc_s *llc_ptr;
  lte_rrc_cfg_db_type *cfg_db_ptr;
  uint16 i = 0;
  int decode_status = 0;
  lte_rrc_osys_RadioResourceConfigDedicated *cfg_ded_ptr = NULL;

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_PHR_msg);
  encoded_pdu.value = llc_reconfig_PHR_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.
                  c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  cfg_db_ptr->phr_cfg.enable = FALSE;
  cfg_ded_ptr->m.mac_MainConfigPresent = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->m.mac_MainConfig_v1020Present = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.m.extendedPHR_r10Present = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.extendedPHR_r10 = 0;
  TF_ASSERT(E_DATA_INVALID == lte_rrc_llc_process_rel10_mac_main_config
                                (cfg_db_ptr, cfg_ded_ptr->mac_MainConfig.u.explicitValue));

  /* ---------------------------------------------------------------------- */

  cfg_db_ptr->phr_cfg.enable = FALSE;
  cfg_ded_ptr->m.mac_MainConfigPresent = 0;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->m.mac_MainConfig_v1020Present = 0;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.m.extendedPHR_r10Present = 0;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.extendedPHR_r10 = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_rel10_mac_main_config
                                (cfg_db_ptr, cfg_ded_ptr->mac_MainConfig.u.explicitValue));

  cfg_db_ptr->phr_cfg.enable = FALSE;
  cfg_ded_ptr->m.mac_MainConfigPresent = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->m.mac_MainConfig_v1020Present = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.m.extendedPHR_r10Present = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.extendedPHR_r10 = 1;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_rel10_mac_main_config
                                (cfg_db_ptr, cfg_ded_ptr->mac_MainConfig.u.explicitValue));

  cfg_db_ptr->phr_cfg.enable = TRUE;
  cfg_ded_ptr->m.mac_MainConfigPresent = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->m.mac_MainConfig_v1020Present = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.m.extendedPHR_r10Present = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.extendedPHR_r10 = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_rel10_mac_main_config
                                (cfg_db_ptr, cfg_ded_ptr->mac_MainConfig.u.explicitValue));

  cfg_db_ptr->phr_cfg.enable = TRUE;
  cfg_ded_ptr->m.mac_MainConfigPresent = 0;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->m.mac_MainConfig_v1020Present = 0;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.m.extendedPHR_r10Present = 0;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.extendedPHR_r10 = 0;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_rel10_mac_main_config
                                (cfg_db_ptr, cfg_ded_ptr->mac_MainConfig.u.explicitValue));

  cfg_db_ptr->phr_cfg.enable = TRUE;
  cfg_ded_ptr->m.mac_MainConfigPresent = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->m.mac_MainConfig_v1020Present = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.m.extendedPHR_r10Present = 1;
  cfg_ded_ptr->mac_MainConfig.u.explicitValue->mac_MainConfig_v1020.extendedPHR_r10 = 1;
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_rel10_mac_main_config
                                (cfg_db_ptr, cfg_ded_ptr->mac_MainConfig.u.explicitValue));

  /*Send a reconfig msg with PHR set to release*/
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);
  LTE_ASSERT(llc_ptr != NULL);
  cfg_db_ptr = llc_ptr->dd_ptr->oc_ptr;
  
  encoded_pdu.length = sizeof(llc_reconfig_PHR_msg);
  encoded_pdu.value = llc_reconfig_PHR_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               DL_DCCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
                  rrcConnectionReconfiguration_r8->radioResourceConfigDedicated;

  cfg_ded_ptr->mac_MainConfig.u.explicitValue->phr_Config.t = T_lte_rrc_osys_MAC_MainConfig_phr_Config_release; 
  
  TF_ASSERT(E_SUCCESS == lte_rrc_llc_process_rel10_mac_main_config
                                (cfg_db_ptr, cfg_ded_ptr->mac_MainConfig.u.explicitValue));

  /* ---------------------------------------------------------------------- */
  
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_ptr, lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  
  lte_rrc_utf_reset(); 

}

void LteRrcLlcPhrAndEphrTest::Teardown()
{
  lte_rrc_utf_teardown();
}




