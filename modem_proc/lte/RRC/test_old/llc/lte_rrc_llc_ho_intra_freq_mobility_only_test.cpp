/*!
  @file lte_rrc_llc_ho_intra_freq_mobility_only_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of HO related
  configuration when a UE receives a Reconfig message containing intra-freq mobility 
  information alone. To start receives a Connection Setup followed by Connection 
  Reconfiguration message that sets up a UM Bi-directional DRB, before handover.

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
lte_rrc_llc_ho_intra_freq_mobility_only_test.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/08   mm     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_sib.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_llc_test_common.h"
#include <tf_stub.h>  

extern "C"
{
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(uint8, lte_rrc_cap_get_fgi_bit, uint8);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
#ifdef FEATURE_LTE_CA
  TF_PROTO_STUB(boolean, lte_rrc_cap_is_band_combo_valid, sys_sband_lte_e_type, uint8 , 
    const sys_sband_lte_e_type *, const lte_bandwidth_e *, const uint8 *, boolean);
#endif
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s*);
}

extern "C"
{
extern lte_rrc_llc_s lte_rrc_llc_data;
}



/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*===========================================================================

                                UNIT TEST

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_fgi_bit_disable_31

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_fgi_bit_disable_31
(
  uint8 bit_pos
)
{
   if (bit_pos == 31) 
   {
      return 0;
   }

} /* lte_rrc_llc_test_get_fgi_bit_disable_31() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_cell_sib2_for_invalid_ul_freq

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns SIB2

    @return
    Pointer to SIB2

 */
/*=========================================================================*/
void *lte_rrc_llc_test_get_cell_sib2_for_invalid_ul_freq
(
  lte_rrc_sib_bitmask_t sib, /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/
  lte_rrc_sib_db_key_t key /*!< sib db index */
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  int decode_status = 0;
  /* Initializing(using) the parameters to handle compiler warnings */
  key = 0;  

    if (sib == LTE_RRC_SIB2_BITMASK) 
  { 
    /* If SIB2 is already decoded, free function first */
    if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr == NULL) 
    {
      encoded_pdu.length = sizeof(llc_invalid_sib2_2);
      encoded_pdu.value = llc_invalid_sib2_2;
      
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      TF_ASSERT(decode_status == 0);
      TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr != NULL);
    }

    /* return sib 2 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib2;
  }
  else if (sib == LTE_RRC_SIB1_BITMASK) 
  {
    if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr == NULL) 
    {    
      
      encoded_pdu.length = sizeof(llc_sib1_tdd);
      encoded_pdu.value = llc_sib1_tdd;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      TF_ASSERT(decode_status == 0);
      TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr != NULL);
    }

    /* Set TDD DL/UL config to Configuration 5 */
    lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1->tdd_Config.subframeAssignment 
      = lte_rrc_osys_TDD_Config_subframeAssignment_sa5;

    /* return sib 1 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1; 
  }
  else if (sib == LTE_RRC_MIB_BITMASK) 
  {
    lte_rrc_llc_test_common_data.llc_basic_mib.num_tx_antennas = 1;
    return &lte_rrc_llc_test_common_data.llc_basic_mib;
  }
  else
  { 
    TF_ASSERT(0);
  }

} /* lte_rrc_llc_test_get_cell_sib2_for_invalid_ul_freq() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_cell_invalid_sib5

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns SIB5

    @return
    Pointer to SIB5

 */
/*=========================================================================*/
void *lte_rrc_llc_test_get_cell_invalid_sib5
(
  lte_rrc_sib_bitmask_t sib, /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/
  lte_rrc_sib_db_key_t key /*!< sib db index */
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  int decode_status = 0;
  /* Initializing(using) the parameters to handle compiler warnings */
  key = 0;  

    if (sib == LTE_RRC_SIB2_BITMASK) 
  { 
    /* If SIB2 is already decoded, free function first */
    if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr == NULL) 
    {
      encoded_pdu.length = sizeof(llc_invalid_sib2_2);
      encoded_pdu.value = llc_invalid_sib2_2;
      
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      TF_ASSERT(decode_status == 0);
      TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr != NULL);
    }

    /* return sib 2 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib2;
  }
  else if (sib == LTE_RRC_SIB1_BITMASK) 
  {
    if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr == NULL) 
    {    
      
      encoded_pdu.length = sizeof(llc_sib1_tdd);
      encoded_pdu.value = llc_sib1_tdd;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      TF_ASSERT(decode_status == 0);
      TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr != NULL);
    }

    /* Set TDD DL/UL config to Configuration 5 */
    lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1->tdd_Config.subframeAssignment 
      = lte_rrc_osys_TDD_Config_subframeAssignment_sa5;

    /* return sib 1 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1; 
  }
  else if (sib == LTE_RRC_MIB_BITMASK) 
  {
    lte_rrc_llc_test_common_data.llc_basic_mib.num_tx_antennas = 1;
    return &lte_rrc_llc_test_common_data.llc_basic_mib;
  }
  else if (sib == LTE_RRC_SIB5_BITMASK) 
  {
    /* If SIB2 is already decoded, free function first */
    if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib5_ptr == NULL) 
    {
      encoded_pdu.length = sizeof(llc_invalid_sib5);
      encoded_pdu.value = llc_invalid_sib5;
      
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib5_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      TF_ASSERT(decode_status == 0);
      TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib5_ptr != NULL);
    }

    /* return sib 5 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib5_ptr->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib5;
  }
  else
  { 
    return NULL;
  }

} /* lte_rrc_llc_test_get_cell_invalid_sib5() */




TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcIntraFreqMobilityOnlyTest);

void LteRrcLlcIntraFreqMobilityOnlyTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcIntraFreqMobilityOnlyTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_intra_freq_mobility_info_only_msg);
  encoded_pdu.value = llc_reconfig_intra_freq_mobility_info_only_msg;
  lte_rrc_llc_test_intra_freq_mobility_only(encoded_pdu, TRUE);
  lte_rrc_utf_reset(); 
}

void LteRrcLlcIntraFreqMobilityOnlyTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcIntraFreqMobilityWithInvalidSib5Test);

void LteRrcLlcIntraFreqMobilityWithInvalidSib5Test::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcIntraFreqMobilityWithInvalidSib5Test::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
   TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_invalid_sib5;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_get_serving_cell_params;
  encoded_pdu.length = sizeof(llc_reconfig_intra_freq_mobility_info_only_msg);
  encoded_pdu.value = llc_reconfig_intra_freq_mobility_info_only_msg;
  lte_rrc_llc_test_intra_freq_mobility_only(encoded_pdu, TRUE);
  lte_rrc_utf_reset(); 
}

void LteRrcLlcIntraFreqMobilityWithInvalidSib5Test::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcIntraFreqMobilityOnlyWithSccTest);

void LteRrcLlcIntraFreqMobilityOnlyWithSccTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

#ifdef FEATURE_LTE_CA
  TF_STUB(lte_rrc_cap_is_band_combo_valid) = lte_rrc_llc_test_get_band_combo_bitmask_FF;
#endif

}

void LteRrcLlcIntraFreqMobilityOnlyWithSccTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

   encoded_pdu.length = sizeof(llc_rcfg_with_scc_ap_cqi);
  encoded_pdu.value = llc_rcfg_with_scc_ap_cqi;
  // Verify Reconfig 2
  lte_rrc_llc_test_scc_cqi_reporting(encoded_pdu);
#ifdef FEATURE_LTE_CA
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->scell_config.scell_cfg_list[0].scell_dedicated_cfg.ul_cfg.cqi_reporting_cfg.enable == TRUE);
#endif

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_intra_freq_mobility_info_only_msg);
  encoded_pdu.value = llc_reconfig_intra_freq_mobility_info_only_msg;
  lte_rrc_llc_test_mobility_with_scc_only(encoded_pdu, TRUE);
  
#ifdef FEATURE_LTE_CA
  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->scell_config.scell_cfg_list[0].scell_dedicated_cfg.ul_cfg.cqi_reporting_cfg.enable == FALSE);
#endif

  lte_rrc_utf_reset(); 
}

void LteRrcLlcIntraFreqMobilityOnlyWithSccTest::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcHoUlFreqNotMatchingWithSib2UlFreqTest);

void LteRrcLlcHoUlFreqNotMatchingWithSib2UlFreqTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcHoUlFreqNotMatchingWithSib2UlFreqTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update();
  lte_rrc_llc_test_rrc_connection_setup_default();
  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_intra_freq_mobility_info_only_msg);
  encoded_pdu.value = llc_reconfig_intra_freq_mobility_info_only_msg;
  lte_rrc_llc_test_intra_freq_mobility_only(encoded_pdu, TRUE);
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib2_for_invalid_ul_freq;
  TF_STUB(lte_rrc_controller_get_state) = lte_rrc_llc_test_controller_get_connected_state;
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_llc_test_get_fgi_bit_disable_31;
  lte_rrc_llc_test_basic_sib2_update_for_invalid_ul_freq();
  lte_rrc_utf_reset(); 
}

void LteRrcLlcHoUlFreqNotMatchingWithSib2UlFreqTest::Teardown()
{
  lte_rrc_utf_teardown();
}








