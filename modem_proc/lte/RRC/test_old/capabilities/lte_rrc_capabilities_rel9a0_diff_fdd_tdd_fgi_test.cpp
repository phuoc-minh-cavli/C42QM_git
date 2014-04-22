/*!
  @file lte_rrc_capabilities_rel9a0_diff_fdd_tdd_fgi_test.cpp

  @brief
  Inject Capability Enquiry DLM and get response with diff FGI for FDD and TDD controlled via EFS

*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/01/12   ap     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_osys_asn1util.h>
#include <__lte_rrc_capabilities.h>
#include <lte_rrc_utils.h>
#include <lte_rrc_capabilities.h>
#include <lte_rrc_capabilitiesi.h>
#include <fs_public.h>
}

#include <tf_stub.h>
#include <TestFramework.h>
#include <lte_rrc_utf.h>
#include "lte_rrc_capabilities_test_common.h"
#include <qtf_efs.h>
#include <fs_lib.h>

extern "C"
{
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
  extern STATIC lte_rrc_cap_s lte_rrc_cap_data;
}

//message structures
static lte_rrc_mh_dlm_s cap_enq_dlm;
static lte_rrc_ul_msg_cnfi_s cap_ulm_cnfi;

//MU ID to be returned from UL message send
#define LTE_RRC_CAPABILITIES_TEST_MU_ID 7

/*===========================================================================

                                UNIT TEST

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd

===========================================================================*/
/*!
    @brief
    This function simulates the function that returns the last camped mode
    
    @return
    lte_rrc_div_duplex_e, LTE_RRC_FREQ_DIV_DUPLEX means last camped mode is FDD

*/
/*=========================================================================*/
lte_rrc_div_duplex_e lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd
(
  void
)
{
  return LTE_RRC_FREQ_DIV_DUPLEX;
} /* lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd */

/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd

===========================================================================*/
/*!
    @brief
    This function simulates the function that returns the last camped mode
    
    @return
    lte_rrc_div_duplex_e, LTE_RRC_TIME_DIV_DUPLEX means last camped mode is TDD

*/
/*=========================================================================*/
lte_rrc_div_duplex_e lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd
(
  void
)
{
  return LTE_RRC_TIME_DIV_DUPLEX;
} /* lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd */

/*===========================================================================

  FUNCTION:  lte_rrc_capabilities_test_efs_diffxddfgi_init

===========================================================================*/
/*!
    @brief
    This function initializes the diff_fdd_tdd_fgi_enable efs with the value given
    
    @return
    None

*/
/*=========================================================================*/
void lte_rrc_capabilities_test_efs_diffxddfgi_init
(
  uint8 diff_fdd_tdd_fgi_enable
)
{
  int oflag, mode, efs_ret;
  uint8 test;
  
  /* Init EFS */
  qtf_efs_initialize("diff_fdd_tdd_fgi_enable");
	
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/cap/diff_fdd_tdd_fgi_enable", 
					&diff_fdd_tdd_fgi_enable, sizeof(diff_fdd_tdd_fgi_enable), oflag, mode) >= 0);

  /* Read the first byte to get the size of sib 1*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/cap/diff_fdd_tdd_fgi_enable", O_RDONLY)) != -1);
  TF_ASSERT(efs_read(efs_ret, &test, sizeof(test))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
}

//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD enabled via EFS (FDD only supported) */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesDiffFddTddFgiEnableWithFddBand);

void LteRrcCapabilitiesDiffFddTddFgiEnableWithFddBand::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);
  
  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = 
  lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
}

void LteRrcCapabilitiesDiffFddTddFgiEnableWithFddBand::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  
  lte_irat_capabilities_s irat_capabilities;
  uint32 ref_fgi = 0;
  uint32 rcv_fgi = 0;
  uint32 fgi_mask = 0;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.\
  		    m.nonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    m.lateNonCriticalExtensionPresent);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);
	TF_ASSERT((LTE_TX_ANTENNA_SELECTION_SUPPORTED_TDD && LTE_TX_ANTENNA_SELECTION_SUPPORTED_FDD) ==
		decoded_eutra_cap_ptr->phyLayerParameters.ue_TxAntennaSelectionSupported);
	TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.
          tdd_Add_UE_EUTRA_Capabilities_r9.m.phyLayerParameters_r9Present);
	TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.
          fdd_Add_UE_EUTRA_Capabilities_r9.m.phyLayerParameters_r9Present);
	
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_ASSERT(0 == rcv_fgi);	

  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  ref_fgi = lte_rrc_cap_get_feature_group_indicators_word();
  fgi_mask |= (1 << (LTE_RRC_CAP_FGI_BIT_32 - LTE_RRC_CAP_FGI_BIT_30));
  fgi_mask |= (1 << (LTE_RRC_CAP_FGI_BIT_32 - LTE_RRC_CAP_FGI_BIT_27));
  fgi_mask |= (1 << (LTE_RRC_CAP_FGI_BIT_32 - LTE_RRC_CAP_FGI_BIT_1));
  ref_fgi &= (~(fgi_mask));
  ref_fgi &= lte_rrc_cap_get_irat_mask(&(lte_rrc_cap_data.sd_ptr->irat_capabilities));
  TF_ASSERT(ref_fgi == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesDiffFddTddFgiEnableWithFddBand::Teardown()
{
  lte_rrc_utf_teardown();
}

//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD enabled via EFS (TDD only supported) */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesDiffFddTddFgiEnableWithTddBand);

void LteRrcCapabilitiesDiffFddTddFgiEnableWithTddBand::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = 
  lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_tdd;
}

void LteRrcCapabilitiesDiffFddTddFgiEnableWithTddBand::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  
  lte_irat_capabilities_s irat_capabilities;
  uint32 ref_fgi = 0;
  uint32 rcv_fgi = 0;
  uint32 fgi_mask = 0;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

    //specify bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);
	
  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.\
  		    m.nonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    m.lateNonCriticalExtensionPresent);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);
  /* Last camped mode is TDD */
	TF_ASSERT((LTE_TX_ANTENNA_SELECTION_SUPPORTED_TDD) ==
		decoded_eutra_cap_ptr->phyLayerParameters.ue_TxAntennaSelectionSupported);
	TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.
          tdd_Add_UE_EUTRA_Capabilities_r9.m.phyLayerParameters_r9Present);	
	TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.
          fdd_Add_UE_EUTRA_Capabilities_r9.m.phyLayerParameters_r9Present);

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_ASSERT(0 == rcv_fgi);

  /* Verify TDD FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  ref_fgi = lte_rrc_cap_get_feature_group_indicators_word();
  fgi_mask |= (1 << (LTE_RRC_CAP_FGI_BIT_32 - LTE_RRC_CAP_FGI_BIT_30));
  fgi_mask |= (1 << (LTE_RRC_CAP_FGI_BIT_32 - LTE_RRC_CAP_FGI_BIT_27));
  fgi_mask |= (1 << (LTE_RRC_CAP_FGI_BIT_32 - LTE_RRC_CAP_FGI_BIT_1));
  ref_fgi &= (~(fgi_mask));
  ref_fgi &= lte_rrc_cap_get_irat_mask(&(lte_rrc_cap_data.sd_ptr->irat_capabilities));
  ref_fgi &= lte_rrc_cap_get_tdd_mask(TRUE);

  TF_ASSERT(ref_fgi == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesDiffFddTddFgiEnableWithTddBand::Teardown()
{
  lte_rrc_utf_teardown();
}

//-----------------------------------------------------------------------------
/* Test to verify the UE capability with diff FGI for FDD and TDD enabled via EFS (FDD and TDD supported) */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesDiffFddTddFgiEnableWithFddTddBand);

void LteRrcCapabilitiesDiffFddTddFgiEnableWithFddTddBand::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(1);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = 
  lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
}

void LteRrcCapabilitiesDiffFddTddFgiEnableWithFddTddBand::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  
  lte_irat_capabilities_s irat_capabilities;
  uint32 ref_fgi = 0;
  uint32 rcv_fgi = 0;
  uint32 fgi_mask = 0;

  lte_rrc_cell_id_s cell_id;
  lte_rb_id_t rb_id;
  lte_sfn_s sfn;
  int pdu_num;
  uint16 encoded_msg_len;
  unsigned char *encoded_msg;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  //specify two bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);
	

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.\
  		    m.nonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    m.lateNonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndicators_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9.numbits);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndicators_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9.numbits);
	TF_ASSERT((LTE_TX_ANTENNA_SELECTION_SUPPORTED_TDD && LTE_TX_ANTENNA_SELECTION_SUPPORTED_FDD) ==
		decoded_eutra_cap_ptr->phyLayerParameters.ue_TxAntennaSelectionSupported);
	TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.
          tdd_Add_UE_EUTRA_Capabilities_r9.m.phyLayerParameters_r9Present);
	TF_ASSERT(LTE_TX_ANTENNA_SELECTION_SUPPORTED_TDD == decoded_eutra_cap_ptr->nonCriticalExtension.
					nonCriticalExtension.lateNonCriticalExtension.
          tdd_Add_UE_EUTRA_Capabilities_r9.phyLayerParameters_r9.
          ue_TxAntennaSelectionSupported);
	TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.
          fdd_Add_UE_EUTRA_Capabilities_r9.m.phyLayerParameters_r9Present);
	TF_ASSERT(LTE_TX_ANTENNA_SELECTION_SUPPORTED_FDD == decoded_eutra_cap_ptr->nonCriticalExtension.
					nonCriticalExtension.lateNonCriticalExtension.
          fdd_Add_UE_EUTRA_Capabilities_r9.phyLayerParameters_r9.
          ue_TxAntennaSelectionSupported);

  /* Verify FDD FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9);  
  ref_fgi = lte_rrc_cap_get_feature_group_indicators_word();
  fgi_mask |= (1 << (LTE_RRC_CAP_FGI_BIT_32 - LTE_RRC_CAP_FGI_BIT_27));
  fgi_mask |= (1 << (LTE_RRC_CAP_FGI_BIT_32 - LTE_RRC_CAP_FGI_BIT_1));
  ref_fgi &= (~(fgi_mask));
  ref_fgi &= lte_rrc_cap_get_irat_mask(&(lte_rrc_cap_data.sd_ptr->irat_capabilities));
  TF_ASSERT(ref_fgi == rcv_fgi);

  /* Verify TDD FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndicators_r9);
  ref_fgi &= lte_rrc_cap_get_tdd_mask(TRUE);
  TF_ASSERT(ref_fgi == rcv_fgi);

  /* Verify Common FGI */
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  TF_ASSERT(ref_fgi == rcv_fgi);

  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_ASSERT(0 == rcv_fgi);
  
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9.numbits);
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.fdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9);
  TF_ASSERT(0 == rcv_fgi);
  
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9.numbits);
  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.tdd_Add_UE_EUTRA_Capabilities_r9.featureGroupIndRel9Add_r9);
  TF_ASSERT(0 == rcv_fgi);
  
  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesDiffFddTddFgiEnableWithFddTddBand::Teardown()
{
  lte_rrc_utf_teardown();
}

//-----------------------------------------------------------------------------
/* Test to verify the UE capability that diff FGI for FDD and TDD are not included when disabled via EFS (FDD and TDD supported) */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcCapabilitiesDiffFddTddFgiDisableWithFddTddBand);

void LteRrcCapabilitiesDiffFddTddFgiDisableWithFddTddBand::Setup()
{
  lte_rrc_capabilities_test_efs_diffxddfgi_init(0);

  lte_rrc_utf_setup(LTE_RRC_CAPABILITIES_SM, NULL, 0);
  lte_rrc_capabilities_test_stub_init();
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = 
  lte_rrc_capabilities_test_stub_csp_get_last_camped_mode_fdd;
}

void LteRrcCapabilitiesDiffFddTddFgiDisableWithFddTddBand::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  sys_lte_band_mask_e_type lte_band_preference;
  lte_rrc_send_ul_msg_reqi_s *ul_msg_ptr;
  lte_rrc_osys_UECapabilityInformation *cap_info_ptr = NULL ;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_UECapabilityInformation_r8_IEs *r8_ies_ptr = NULL;
  lte_rrc_osys_UE_CapabilityRAT_Container *rat_container_elem_ptr = NULL;
  const OSOCTET *rat_container_ptr = NULL;
  lte_rrc_osys_UE_EUTRA_Capability *decoded_eutra_cap_ptr = NULL;
  int decode_status = 0;
  lte_irat_capabilities_s irat_capabilities;
  uint32 ref_fgi = 0;
  uint32 rcv_fgi = 0;
  uint32 fgi_mask = 0;

  TF_MSG("Start of test");

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  //specify two bands
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_preference);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND1);
	SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_preference, SYS_SBAND_LTE_EUTRAN_BAND33);

  lte_rrc_capabilities_test_stub_csp_set_lte_band_preference(
    lte_band_preference);

  //set MUID for Capability Enquiry
  lte_rrc_utf_set_sync_ul_msg_params(LTE_RRC_CAPABILITIES_TEST_MU_ID,0);

  TF_MSG("Sending Capability Enquiry DLM");
  lte_rrc_capabilities_test_construct_cap_enq_msg(&cap_enq_dlm);
  lte_rrc_utf_send_msg((byte*)&cap_enq_dlm, sizeof(cap_enq_dlm));

  TF_MSG("Checking for LTE_RRC_SEND_UL_MSG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ul_msg_ptr = (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_SEND_UL_MSG_REQI == ul_msg_ptr->msg_hdr.id);

  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);
  cap_info_ptr = (ul_dcch_msg_ptr->message).u.c1->u.ueCapabilityInformation;
  
  r8_ies_ptr = cap_info_ptr->criticalExtensions.u.c1->u.ueCapabilityInformation_r8;

  rat_container_elem_ptr = r8_ies_ptr->ue_CapabilityRAT_ContainerList.elem;

  TF_ASSERT(lte_rrc_osys_RAT_Type_eutra == rat_container_elem_ptr->rat_Type);

  TF_ASSERT(0 != rat_container_elem_ptr->ueCapabilityRAT_Container.numocts); 

  rat_container_ptr = rat_container_elem_ptr->ueCapabilityRAT_Container.data;

  TF_ASSERT(NULL != rat_container_ptr);

  decoded_eutra_cap_ptr = (lte_rrc_osys_UE_EUTRA_Capability *)
  lte_rrc_osys_asn1_decode_pdu((void *)rat_container_ptr, 
                               rat_container_elem_ptr->ueCapabilityRAT_Container.numocts, UE_EUTRA_Capability,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(NULL != decoded_eutra_cap_ptr);

  TF_ASSERT(32 == decoded_eutra_cap_ptr->featureGroupIndicators.numbits); 
  TF_ASSERT(1 == decoded_eutra_cap_ptr->m.nonCriticalExtensionPresent);			
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.\
  		    m.nonCriticalExtensionPresent);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    m.lateNonCriticalExtensionPresent);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.fdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.tdd_Add_UE_EUTRA_Capabilities_r9Present);
  TF_ASSERT(1 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.m.featureGroupIndRel9Add_r9Present);
  TF_ASSERT(32 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9.numbits);
	TF_ASSERT((LTE_TX_ANTENNA_SELECTION_SUPPORTED_TDD && LTE_TX_ANTENNA_SELECTION_SUPPORTED_FDD) ==
		decoded_eutra_cap_ptr->phyLayerParameters.ue_TxAntennaSelectionSupported);
	TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.
          tdd_Add_UE_EUTRA_Capabilities_r9.m.phyLayerParameters_r9Present);
	TF_ASSERT(0 == decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.lateNonCriticalExtension.
          fdd_Add_UE_EUTRA_Capabilities_r9.m.phyLayerParameters_r9Present);

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->nonCriticalExtension.nonCriticalExtension.\
  		    lateNonCriticalExtension.featureGroupIndRel9Add_r9);
  TF_ASSERT(0 == rcv_fgi);

  rcv_fgi = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32),&decoded_eutra_cap_ptr->featureGroupIndicators);
  ref_fgi = lte_rrc_cap_get_feature_group_indicators_word();
  fgi_mask |= (1 << (LTE_RRC_CAP_FGI_BIT_32 - LTE_RRC_CAP_FGI_BIT_27));
  fgi_mask |= (1 << (LTE_RRC_CAP_FGI_BIT_32 - LTE_RRC_CAP_FGI_BIT_1));
  ref_fgi &= (~(fgi_mask));
  ref_fgi &= lte_rrc_cap_get_irat_mask(&(lte_rrc_cap_data.sd_ptr->irat_capabilities));
  ref_fgi &= lte_rrc_cap_get_tdd_mask(FALSE);

  TF_ASSERT(ref_fgi == rcv_fgi);

  //free the UL msg
  lte_rrc_osys_asn1_free_pdu(ul_msg_ptr->ul_msg_ptr,
                             lte_rrc_osys_UL_DCCH_Message_PDU);

  TF_MSG("Sending LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI");
  lte_rrc_init_default_hdr(&cap_ulm_cnfi.msg_hdr,
                           LTE_RRC_UE_CAPABILITY_INFORMATION_CNFI);
  cap_ulm_cnfi.mu_id = LTE_RRC_CAPABILITIES_TEST_MU_ID;
  cap_ulm_cnfi.status = LTE_RRC_UL_MSG_ACK;
  lte_rrc_utf_send_msg((byte*)&cap_ulm_cnfi, sizeof(cap_ulm_cnfi));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INITIAL state");
  TF_ASSERT(stm_get_state(LTE_RRC_CAPABILITIES_SM) == 
            LTE_RRC_CAPABILITIES_SM__INITIAL);

  TF_MSG("End of test");
}

void LteRrcCapabilitiesDiffFddTddFgiDisableWithFddTddBand::Teardown()
{
  lte_rrc_utf_teardown();
}
