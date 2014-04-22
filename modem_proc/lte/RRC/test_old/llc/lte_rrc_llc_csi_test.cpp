/*!
  @file lte_rrc_llc_csi_test.cpp

  @brief
  Unit test code for checking CSI RS OTA fields

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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/llc/
lte_rrc_llc_csi_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/14   md     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_rrc_llci.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"

#include <tf_stub.h>  
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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcCsiRsZeroPowerBitsTest);

void LteRrcLlcCsiRsZeroPowerBitsTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcCsiRsZeroPowerBitsTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_errno_e status;
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_ded_cfg_ptr = NULL;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_llc_s *llc_ptr;
  int decode_status = 0;
  int i = 0;

  llc_ptr = (lte_rrc_llc_s *)stm_get_user_data(LTE_RRC_LLC_SM);

  // Stub out UL prefix length to Normal CP and mode to FDD
  llc_ptr->dd_ptr->oc_ptr->ul_cyclic_prefix_length = LTE_CPHY_UL_CYCLIC_PREFIX_LENGTH_1;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;

  encoded_pdu.length = sizeof(llc_csi_rs_msg);
  encoded_pdu.value = llc_csi_rs_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                   (encoded_pdu.value,encoded_pdu.length,
                                                    DL_DCCH_Message, &decode_status);
	
#ifdef FEATURE_LTE_CA												
  lte_rrc_osys_SCellToAddModList_r10 *scell_add_mod_list = NULL;
  lte_rrc_osys_RadioResourceConfigDedicatedSCell_r10 *cfg_ded_ptr = NULL;
  scell_add_mod_list = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10;
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10.elem->radioResourceConfigDedicatedSCell_r10;
  
  status = lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr);
  TF_ASSERT(status == E_SUCCESS);

  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.
    zeroTxPowerCSI_RS_r10.u.setup->zeroTxPowerResourceConfigList_r10.data[0] = 0;
   cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.
    zeroTxPowerCSI_RS_r10.u.setup->zeroTxPowerResourceConfigList_r10.data[1] = 0;
  status = lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr);
  TF_ASSERT(status == E_SUCCESS);

  // Stub out UL prefix length to  ECP and mode to FDD
  llc_ptr->dd_ptr->oc_ptr->ul_cyclic_prefix_length = LTE_CPHY_UL_CYCLIC_PREFIX_LENGTH_2;

  encoded_pdu.length = sizeof(llc_csi_rs_msg);
  encoded_pdu.value = llc_csi_rs_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                   (encoded_pdu.value,encoded_pdu.length,
                                                    DL_DCCH_Message, &decode_status);
  
  scell_add_mod_list = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10;
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10.elem->radioResourceConfigDedicatedSCell_r10;
  status = lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr);
  TF_ASSERT(status == E_SUCCESS);

  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.
    zeroTxPowerCSI_RS_r10.u.setup->zeroTxPowerResourceConfigList_r10.data[0] = 0;
   cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.
    zeroTxPowerCSI_RS_r10.u.setup->zeroTxPowerResourceConfigList_r10.data[1] = 0;
  status = lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr);
  TF_ASSERT(status == E_SUCCESS);

  // Stub out UL prefix length to Normal CP and mode to FDD
  llc_ptr->dd_ptr->oc_ptr->ul_cyclic_prefix_length = LTE_CPHY_UL_CYCLIC_PREFIX_LENGTH_1;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;

  encoded_pdu.length = sizeof(llc_csi_rs_msg);
  encoded_pdu.value = llc_csi_rs_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                   (encoded_pdu.value,encoded_pdu.length,
                                                    DL_DCCH_Message, &decode_status);
  scell_add_mod_list = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10;
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10.elem->radioResourceConfigDedicatedSCell_r10;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.csi_RS_r10.u.setup->antennaPortsCount_r10 = 2;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.csi_RS_r10.u.setup->resourceConfig_r10 = 20;
  status = lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr);
  TF_ASSERT(status == E_SUCCESS);

  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.
    zeroTxPowerCSI_RS_r10.u.setup->zeroTxPowerResourceConfigList_r10.data[0] = 0;
   cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.
    zeroTxPowerCSI_RS_r10.u.setup->zeroTxPowerResourceConfigList_r10.data[1] = 0;
  status = lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr);
  TF_ASSERT(status == E_SUCCESS);
#endif

  // Stub out UL prefix length to  ECP and mode to FDD
  llc_ptr->dd_ptr->oc_ptr->ul_cyclic_prefix_length = LTE_CPHY_UL_CYCLIC_PREFIX_LENGTH_2;

  encoded_pdu.length = sizeof(llc_csi_rs_msg);
  encoded_pdu.value = llc_csi_rs_msg;
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu
                                                   (encoded_pdu.value,encoded_pdu.length,
                                                    DL_DCCH_Message, &decode_status);
  
#ifdef FEATURE_LTE_CA
  scell_add_mod_list = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10;
  cfg_ded_ptr = &dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
  criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->nonCriticalExtension.
  nonCriticalExtension.nonCriticalExtension.sCellToAddModList_r10.elem->radioResourceConfigDedicatedSCell_r10;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.csi_RS_r10.u.setup->antennaPortsCount_r10 = 2;
  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.csi_RS_r10.u.setup->resourceConfig_r10 = 20;
  status = lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr);
  TF_ASSERT(status == E_SUCCESS);

  cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.
    zeroTxPowerCSI_RS_r10.u.setup->zeroTxPowerResourceConfigList_r10.data[0] = 0;
   cfg_ded_ptr->physicalConfigDedicatedSCell_r10.nonUL_Configuration_r10.csi_RS_Config_r10.
    zeroTxPowerCSI_RS_r10.u.setup->zeroTxPowerResourceConfigList_r10.data[1] = 0;
  status = lte_rrc_llc_process_radio_resource_scell_cfg_ded(llc_ptr, scell_add_mod_list->elem[0].sCellIndex_r10, cfg_ded_ptr);
  TF_ASSERT(status == E_SUCCESS);
#endif
  
  lte_rrc_utf_reset(); 
  
}

void LteRrcLlcCsiRsZeroPowerBitsTest::Teardown()
{
  lte_rrc_utf_teardown();
}


