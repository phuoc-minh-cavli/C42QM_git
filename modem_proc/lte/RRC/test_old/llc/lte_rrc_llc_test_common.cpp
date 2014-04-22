/*!
  @file lte_rrc_llc_test_common.cpp

  @brief
  Common utilities for llc test

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/llc/lte_rrc_llc_test_common.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------   
03/16/10   fhu     Replaced lte_l1_tdd_subframe_assign_e by 
                     lte_l1_tdd_ul_dl_cfg_index_e
04/27/09   da      Added PHR processing.  Removed usage of rel_mask and using
                   enable\disable flags.
10/09/08   da      Initial Revision 
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_rrc_csp.h"
#include "lte.h"
#include "lte_rrc_llc.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_controller.h"
#include "lte_mac_ind.h"
#include "lte_rrc_emp.h"
#include "lte_rrc_ext_api.h"
#include "lte_rrc_capabilities.h"
}

#include <math.h>
#include <tf_stub.h>
#include <TestFramework.h>
#include "lte_rrc_llc_test_common.h"
#include "lte_rrc_utf.h"

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s*);
  TF_PROTO_STUB( lte_errno_e, lte_rrc_csp_get_camped_cell_bw, lte_bandwidth_e *);
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_sec_dcch_prepare_config, lte_rrc_osys_DL_DCCH_Message *,
                lte_pdcp_security_cfg_s *,lte_phy_cell_id_t, lte_earfcn_t);
  //TF_PROTO_STUB(lte_errno_e, lte_rrc_sec_process_irat_to_lte_ho_config, lte_rrc_osys_DL_DCCH_Message *,
               // lte_pdcp_security_cfg_s *,lte_phy_cell_id_t, lte_earfcn_t);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_sec_param_prepare_config, uint8, boolean, 
                lte_pdcp_security_cfg_s *);
  TF_PROTO_STUB(uint8, lte_rrc_csp_get_num_tx_antennas, void);
  TF_PROTO_STUB(boolean, lte_rrc_cap_inter_freq_is_supported, void);
  TF_PROTO_STUB(boolean, lte_rrc_cap_get_r11_cap_bit, uint8);
  TF_PROTO_STUB( lte_errno_e, lte_rrc_meas_get_eutra_meas_obj_param, lte_earfcn_t, lte_cphy_meas_obj_eutra_s *);
  TF_PROTO_STUB(boolean, lte_rrc_sib_b12_to_b17_conversion_is_needed, void);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
  //TF_PROTO_STUB(boolean, lte_rrc_get_tdd_prune_required, void);
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
  TF_PROTO_STUB(uint8, lte_rrc_cap_get_fgi_bit, uint8);
//  TF_PROTO_STUB(boolean, lte_rrc_rohc_is_supported, void);
 // TF_PROTO_STUB(boolean, lte_rrc_config_ho_to_eutra_in_progress, void);
  #ifdef FEATURE_LTE_EMBMS
  TF_PROTO_STUB(lte_rrc_emp_warn_reason_e, lte_rrc_esmgr_get_oos_warn_reason, void);
  TF_PROTO_STUB(lte_rrc_osys_MCCH_Message *, lte_rrc_emp_get_mbsfnareaconfig, lte_mbsfn_area_id_t);
  #endif
  TF_PROTO_STUB(boolean, lte_rrc_csp_lte_band_is_supported, sys_sband_lte_e_type);
  //TF_PROTO_STUB(void, lte_rrc_controller_get_cphy_rf_status_info, lte_rrc_rf_status_info_s *);
  TF_PROTO_STUB(boolean, lte_rrc_cell_is_tdd, lte_earfcn_t);

#ifdef FEATURE_LTE_CA
	TF_PROTO_STUB(boolean, lte_rrc_cap_is_band_combo_valid, sys_sband_lte_e_type, uint8 , 
    const sys_sband_lte_e_type *, const lte_bandwidth_e *, const uint8 *, boolean);
#endif

  TF_PROTO_STUB(boolean, lte_rrc_cap_transmit_antenna_selection_is_supported, boolean);
#ifdef FEATURE_LTE_TDD
  TF_PROTO_STUB(boolean, lte_ml1_validate_tdd_cfg_mbsfn_sf_alloc, 
    lte_cphy_tdd_cfg_param_s*, lte_cphy_common_cfg_req_param_s*);
#endif
   //TF_PROTO_STUB(boolean, lte_rrc_controller_tune_aways_enabled, void);
  TF_PROTO_STUB(boolean, lte_rrc_is_rel_10_ca_enabled, void);	 
}

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
enum
{
  LTE_RRC_LLC_TEST_COMMON_CFG_NULL,
  LTE_RRC_LLC_TEST_COMMON_CFG_NEW,
  LTE_RRC_LLC_TEST_COMMON_CFG_OLD
};

extern "C"
{
extern lte_rrc_llc_s lte_rrc_llc_data;
}
/*! @brief Variables internal to module lte_rrc_llc_test_common.cpp
*/
STATIC lte_rrc_llc_test_common_s lte_rrc_llc_test_common_data;

uint32 lte_rrc_llc_test_umid_list[] = LTE_RRC_LLC_TEST_UMID_LIST;

lte_cphy_common_cfg_mask_t cphy_common_cfg_verif_mask = 
  (
   LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
   LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
   LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
   LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
   LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
   LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
   LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
   LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
   LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
   LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG |
   LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM |
   LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG |
   LTE_CPHY_COMMON_CFG_MASK_BIT_PRACH_CFG
  );

uint8 lte_rrc_llc_test_common_cfg_sts = LTE_RRC_LLC_TEST_COMMON_CFG_NULL;

boolean lte_rrc_llc_test_embms_tdd_collision_detected = FALSE;

/*===========================================================================

                                FUNCTION: lte_rrc_llc_test_stub__ml1_validate_tdd_cfg_mbsfn_sf_alloc

===========================================================================*/

boolean lte_rrc_llc_test_stub__ml1_validate_tdd_cfg_mbsfn_sf_alloc
(
  lte_cphy_tdd_cfg_param_s *tdd_cfg, 
  lte_cphy_common_cfg_req_param_s *cfg_req
)
{
  return (!lte_rrc_llc_test_embms_tdd_collision_detected);
}

/*===========================================================================

                                FUNCTION: lte_rrc_llc_test_stub__cap_ca_combo_stub

===========================================================================*/

uint64 lte_rrc_llc_test_stub__cap_ca_combo_stub
(
  sys_sband_lte_e_type band
)
{
  return 0x01;
}

/*===========================================================================

                                FUNCTION: lte_rrc_llc_test_stub__cap_ca_bw_valid_stub

===========================================================================*/
#ifdef FEATURE_LTE_CA
boolean lte_rrc_llc_test_stub__cap_ca_bw_valid_stub
(
  sys_sband_lte_e_type pcell_band, 
  sys_sband_lte_e_type scell_band,
  lte_bandwidth_e pcell_dl_bandwidth, /*Pcell bandwidth */
  lte_bandwidth_e scell_dl_bandwidth, /*Scell bandwidth */
  lte_rrc_osys_CA_BandwidthClass_r10_Root bw_calss
)
{
  return TRUE;
}
#endif



/*===========================================================================

   FUNCTION:  lte_rrc_llc_test_send_handover_completed_indi

===========================================================================*/
/*!
    @brief
    This function builds and sends LTE_RRC_HANDOVER_COMPLETED_IND 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_llc_test_send_handover_completed_indi
(
  void
)
{

  lte_rrc_handover_completed_ind_s indi;

  /* Send HO Completed indication to CSP */
  lte_rrc_init_default_hdr(&indi.msg_hdr,LTE_RRC_HANDOVER_COMPLETED_IND);
  lte_rrc_utf_send_msg((byte*)&indi, sizeof(indi));
  lte_rrc_utf_wait_for_done();

}/* lte_rrc_llc_test_send_handover_completed_indi */


/*===========================================================================

   FUNCTION:  lte_rrc_llc_test_send_stopped_indi

===========================================================================*/
/*!
    @brief
    This function builds and sends LTE_RRC_STOPPED_INDI 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_llc_test_send_stopped_indi
(
  void
)
{

  lte_rrc_stopped_indi_s indi;

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&indi.msg_hdr,LTE_RRC_STOPPED_INDI);

  lte_rrc_utf_send_msg((byte*)&indi, sizeof(indi));
  lte_rrc_utf_wait_for_done();

}/* lte_rrc_llc_test_send_stopped_indi */

/*===========================================================================

   FUNCTION:  lte_rrc_llc_test_cphy_common_cfg_cnf

===========================================================================*/
/*!
    @brief
    This function build cphy_common_cfg_cnf 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_llc_test_cphy_common_cfg_cnf
(
   lte_cphy_common_cfg_cnf_s *cnf_ptr, /*!< pointer to common cfg cnf */ 
   boolean success, /*!< boolean to indicate whether to send success or failure */ 
   boolean invalid /*!< boolean to indicate whether to send failure with cause invalid cfg */ 
)
{
  LTE_ASSERT(cnf_ptr != NULL);

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cnf_ptr->msgr_hdr,LTE_CPHY_COMMON_CFG_CNF);
  if (success) 
  { 
    cnf_ptr->status = E_SUCCESS;

    /* If eMBMS bearers are active, include MBSFN areas config in the CNF bitmask */
    if (lte_rrc_llc_test_common_data.cc_mrb_info.num_active_mrb > 0)
    {
      cnf_ptr->common_cfg_validity = (LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_AREAS_MCCH_SCHDL_CFG | 
                                      LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_AREA_CFG);
    }
  }
  else
  {
    cnf_ptr->status = E_FAILURE;
    if (invalid) 
    {
      /* if we want the failure to be a validity failure, set it to 1 */
      cnf_ptr->common_cfg_validity = 1;
    }
  }
}/*lte_rrc_llc_test_cphy_common_cfg_cnf*/
/*===========================================================================

   FUNCTION:  lte_rrc_llc_test_cphy_dedicated_cfg_cnf

===========================================================================*/
/*!
    @brief
    This function build cphy_dedicated_cfg_cnf 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_llc_test_cphy_dedicated_cfg_cnf
(
   lte_cphy_dedicated_cfg_cnf_s *cnf_ptr, /*!< pointer to dedicated cfg cnf */ 
   boolean success, /*!< boolean to indicate whether to send success or failure */ 
   boolean invalid /*!< boolean to indicate whether to send failure with cause invalid cfg */ 
)
{
  LTE_ASSERT(cnf_ptr != NULL);

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cnf_ptr->msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  if (success) 
  { 
    cnf_ptr->status = E_SUCCESS;
  }
  else
  {
    cnf_ptr->status = E_FAILURE;
    if (invalid) 
    {
      /* if we want the failure to be a validity failure, set it to 1 */
      cnf_ptr->dedicated_cfg_validity = 1;
    }
  }
}/*lte_rrc_llc_test_cphy_dedicated_cfg_cnf*/
/*===========================================================================

   FUNCTION:  lte_rrc_llc_test_mac_cfg_cnf

===========================================================================*/
/*!
    @brief
    This function build lte_mac_cfg_cnf 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_llc_test_mac_cfg_cnf
( 
  lte_mac_cfg_cnf_msg_s *cnf_ptr, /*!< pointer to mac cfg cnf */ 
  boolean success /*!< boolean to indicate whether to send success or failure */ 
)
{
  LTE_ASSERT(cnf_ptr != NULL);

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cnf_ptr->hdr,LTE_MAC_CFG_CNF);
  if (success) 
  {
    cnf_ptr->status = E_SUCCESS;
  }
  else
  { 
    cnf_ptr->status = E_FAILURE;
  }
}/*lte_rrc_llc_test_mac_cfg_cnf*/


/*===========================================================================

   FUNCTION:  lte_rrc_llc_test_construct_and_send_common_cfg_reqi

===========================================================================*/
/*!
    @brief
    This function builds LTE_RRC_CFG_REQI for COMMON CFG 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_llc_test_construct_common_cfg_reqi
( 
  lte_rrc_cfg_reqi_s *cfg_reqi_ptr, /*!< pointer to cfg reqi */
  boolean sib1_update,               /*!< boolean to indicate SIB1 update */
  boolean sib2_update,               /*!< boolean to indicate SIB2 update */
  boolean default_cfg,               /*!< boolean to indicate default cfg needed */
  uint32 *trans_id
)
{
  LTE_ASSERT(cfg_reqi_ptr != NULL);
  TF_MSG("\nLLC: Send LTE_RRC_CFG_REQI for SIB2 Update \n");
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_reqi_ptr->msg_hdr,LTE_RRC_CFG_REQI);

  /* construct cfg_reqi for basic sib2 update */
  cfg_reqi_ptr->num_cfg = 1;
  cfg_reqi_ptr->proc = LTE_RRC_PROC_CSP;
  cfg_reqi_ptr->cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi_ptr->trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi_ptr->num_cfg = 1;
  cfg_reqi_ptr->cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi_ptr->cfg_info[0].cfg_u.common_cfg.sib2_update = sib2_update;
  cfg_reqi_ptr->cfg_info[0].cfg_u.common_cfg.sib1_update = sib1_update;
  cfg_reqi_ptr->cfg_info[0].cfg_u.common_cfg.default_cfg = default_cfg;
}/*lte_rrc_llc_test_cfg_reqi_common*/

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_common_csp_get_serving_cell_params

===========================================================================*/
/*!
    @brief
    This function returns current cell ID and frequency

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
lte_errno_e lte_rrc_llc_test_common_csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Pointer to cell info */
)
{
  /* Just hard code to something for now*/
  cell_info_ptr->cell_id.freq = 500;
  cell_info_ptr->cell_id.phy_cell_id = 503;
  cell_info_ptr->sib_db_key = 2;
  lte_rrc_llc_test_common_data.cell_info.cell_id.freq = 500;
  lte_rrc_llc_test_common_data.cell_info.cell_id.phy_cell_id = 503;
  lte_rrc_llc_test_common_data.cell_info.sib_db_key = 2;
  return E_SUCCESS;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params

===========================================================================*/
/*!
    @brief
    This function returns current cell ID and frequency for a TDD band

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
lte_errno_e lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Pointer to cell info */
)
{
  /* Just hard code to something for now*/
  cell_info_ptr->cell_id.freq = 36000;
  cell_info_ptr->cell_id.phy_cell_id = 502;
  cell_info_ptr->sib_db_key = 2;
  lte_rrc_llc_test_common_data.cell_info.cell_id.freq = 36000;
  lte_rrc_llc_test_common_data.cell_info.cell_id.phy_cell_id = 502;
  lte_rrc_llc_test_common_data.cell_info.sib_db_key = 2;
  return E_SUCCESS;
}


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_basic_get_cell_sib

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns the serving cell SIBs.

    @return
    Pointer to SIB2

 */
/*=========================================================================*/
void *lte_rrc_llc_test_basic_get_cell_sib
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
      encoded_pdu.length = sizeof(llc_basic_sib2_2);
      encoded_pdu.value = llc_basic_sib2_2;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)
      lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      
      TF_MSG("RRC sib2 decode 0x%x", lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr );

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
      
      encoded_pdu.length = sizeof(llc_basic_sib1_1);
      encoded_pdu.value = llc_basic_sib1_1;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);

      TF_MSG("RRC sib1 decode 0x%x", lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr );
      TF_ASSERT(decode_status == 0);
      TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr != NULL);

    }
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
  else if (sib == LTE_RRC_SIB13_BITMASK) 
  {
    /* If SIB13 is not decoded yet, decode it */
    if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr == NULL) 
    {
      encoded_pdu.length = sizeof(llc_sib13_msg);
      encoded_pdu.value = llc_sib13_msg;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)
      lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      
      TF_MSG("RRC sib13 decode 0x%x", lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr );

      TF_ASSERT(decode_status == 0);
      TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr != NULL);
    }

#ifdef FEATURE_LTE_EMBMS
    /* If MRBs are active, then modify some parameters in SIB13 to test reconfiguration */
    if (lte_rrc_llc_test_common_data.cc_mrb_info.num_active_mrb > 0)
    {
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.systemInformation->criticalExtensions.
        u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920->mbsfn_AreaInfoList_r9.elem[0].non_MBSFNregionLength 
        = lte_rrc_osys_MBSFN_AreaInfo_r9_non_MBSFNregionLength_s1;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.systemInformation->criticalExtensions.
        u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920->mbsfn_AreaInfoList_r9.elem[0].mcch_Config_r9.
        signallingMCS_r9 = lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_signallingMCS_r9_n13;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.systemInformation->criticalExtensions.
        u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920->notificationConfig_r9.notificationOffset_r9 = 10;
    }
#endif
    /* return sib 13 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920;
  }
  else
  { 
    TF_ASSERT(0);
  }

} /* lte_rrc_llc_test_basic_get_cell_sib() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_tdd_get_cell_sib

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns the serving cell SIBs for
    a TDD cell.

    @return
    Pointer to SIB2

 */
/*=========================================================================*/
void *lte_rrc_llc_test_tdd_get_cell_sib
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
      encoded_pdu.length = sizeof(llc_basic_sib2_2);
      encoded_pdu.value = llc_basic_sib2_2;
      
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
    TF_ASSERT(0);
  }

} /* lte_rrc_llc_test_tdd_get_cell_sib() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_srs_release_get_cell_sib

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns the serving cell SIBs for
    that release SRS common config.

    @return
    Pointer to SIB2

 */
/*=========================================================================*/
void *lte_rrc_llc_test_srs_release_get_cell_sib
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
      encoded_pdu.length = sizeof(llc_basic_sib2_2);
      encoded_pdu.value = llc_basic_sib2_2;
      
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      TF_ASSERT(decode_status == 0);
      TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr != NULL);
    }

    lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.
      sib2->radioResourceConfigCommon.soundingRS_UL_ConfigCommon.t 
      = T_lte_rrc_osys_SoundingRS_UL_ConfigCommon_release;

    /* return sib 2 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib2;
  }
  else if (sib == LTE_RRC_SIB1_BITMASK) 
  {
    if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr == NULL) 
    {    
      
      encoded_pdu.length = sizeof(llc_basic_sib1_1);
      encoded_pdu.value = llc_basic_sib1_1;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      TF_ASSERT(decode_status == 0);
    }
    /* return sib 1 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1; 
  }
  else if (sib == LTE_RRC_MIB_BITMASK) 
  {
    lte_rrc_llc_test_common_data.llc_basic_mib.num_tx_antennas = 1;
    return &lte_rrc_llc_test_common_data.llc_basic_mib;
  }
  else if (sib == LTE_RRC_SIB13_BITMASK) 
  {
    /* If SIB13 is not decoded yet, decode it */
    if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr == NULL) 
    {
      encoded_pdu.length = sizeof(llc_sib13_msg);
      encoded_pdu.value = llc_sib13_msg;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)
      lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      
      TF_MSG("RRC sib13 decode 0x%x", lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr );

      TF_ASSERT(decode_status == 0);
      TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr != NULL);
    }

#ifdef FEATURE_LTE_EMBMS
    /* If MRBs are active, then modify some parameters in SIB13 to test reconfiguration */
    if (lte_rrc_llc_test_common_data.cc_mrb_info.num_active_mrb > 0)
    {
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.systemInformation->criticalExtensions.
        u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920->mbsfn_AreaInfoList_r9.elem[0].non_MBSFNregionLength 
        = lte_rrc_osys_MBSFN_AreaInfo_r9_non_MBSFNregionLength_s1;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.systemInformation->criticalExtensions.
        u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920->mbsfn_AreaInfoList_r9.elem[0].mcch_Config_r9.
        signallingMCS_r9 = lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_signallingMCS_r9_n13;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.systemInformation->criticalExtensions.
        u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920->notificationConfig_r9.notificationOffset_r9 = 10;
    }
#endif

    /* return sib 13 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920;
  }
  else
  { 
    TF_ASSERT(0);
  }

} /* lte_rrc_llc_test_srs_release_get_cell_sib() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_common_mh_dlm_processing_is_active

===========================================================================*/
/*!
    @brief
    This function simulates MH API that returns if DLM processing is active.

    @return
    boolean

 */
/*=========================================================================*/
boolean lte_rrc_llc_test_common_mh_dlm_processing_is_active
(
  void
)
{
  return TRUE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_common_mh_dlm_processing_is_not_active

===========================================================================*/
/*!
    @brief
    This function simulates MH API that returns if DLM processing is active.

    @return
    boolean

 */
/*=========================================================================*/
boolean lte_rrc_llc_test_common_mh_dlm_processing_is_not_active
(
  void
)
{
  return FALSE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_srs_get_serv_cell_sib

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns the serving cell SIB2 which
    includes SRS configuration.

    @return
    Pointer to SIB2

 */
/*=========================================================================*/

void *lte_rrc_llc_test_srs_get_serv_cell_sib
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
    encoded_pdu.length = sizeof(llc_sib2_with_mbsfn);
    encoded_pdu.value = llc_sib2_with_mbsfn;
   
    lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                 encoded_pdu.length,
                                 BCCH_DL_SCH_Message,
                                 &decode_status);
    TF_ASSERT(decode_status == 0);
    TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr != NULL);
    /* return sib 2 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib2;
  }
  else if (sib == LTE_RRC_SIB1_BITMASK) 
  {
    if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr == NULL) 
    {    
      
      encoded_pdu.length = sizeof(llc_basic_sib1_1);
      encoded_pdu.value = llc_basic_sib1_1;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      TF_ASSERT(decode_status == 0);
      TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr != NULL);
    }
    /* return sib 1 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1; 
  }
  else if (sib == LTE_RRC_MIB_BITMASK) 
  {
    lte_rrc_llc_test_common_data.llc_basic_mib.num_tx_antennas = 1;
    return &lte_rrc_llc_test_common_data.llc_basic_mib;
  }
  else if (sib == LTE_RRC_SIB13_BITMASK) 
  {
    /* If SIB13 is not decoded yet, decode it */
    if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr == NULL) 
    {
      encoded_pdu.length = sizeof(llc_sib13_msg);
      encoded_pdu.value = llc_sib13_msg;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)
      lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                   encoded_pdu.length,
                                   BCCH_DL_SCH_Message,
                                   &decode_status);
      
      TF_MSG("RRC sib13 decode 0x%x", lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr );

      TF_ASSERT(decode_status == 0);
      TF_ASSERT(lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr != NULL);
    }

#ifdef FEATURE_LTE_EMBMS
    /* If MRBs are active, then modify some parameters in SIB13 to test reconfiguration */
    if (lte_rrc_llc_test_common_data.cc_mrb_info.num_active_mrb > 0)
    {
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.systemInformation->criticalExtensions.
        u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920->mbsfn_AreaInfoList_r9.elem[0].non_MBSFNregionLength 
        = lte_rrc_osys_MBSFN_AreaInfo_r9_non_MBSFNregionLength_s1;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.systemInformation->criticalExtensions.
        u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920->mbsfn_AreaInfoList_r9.elem[0].mcch_Config_r9.
        signallingMCS_r9 = lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_signallingMCS_r9_n13;
      lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.systemInformation->criticalExtensions.
        u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920->notificationConfig_r9.notificationOffset_r9 = 10;
    }
#endif

    /* return sib 13 ptr */
    return lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920;
  }
  else
  { 
    TF_ASSERT(0);
  }

} /* lte_rrc_llc_test_srs_get_serv_cell_sib() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_cap_inter_freq_is_supported

===========================================================================*/
/*!
  @brief
  Simulates the function in Capabilities

  @return
  boolean
*/
/*=========================================================================*/
boolean lte_rrc_llc_test_cap_inter_freq_is_supported
(
  void
)
{
  return TRUE;

} /* lte_rrc_llc_test_cap_inter_freq_is_supported */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_cap_inter_freq_is_not_supported

===========================================================================*/
/*!
  @brief
  Simulates the function in Capabilities

  @return
  boolean
*/
/*=========================================================================*/
boolean lte_rrc_llc_test_cap_inter_freq_is_not_supported
(
  void
)
{
  return FALSE;

} /* lte_rrc_llc_test_cap_inter_freq_is_not_supported */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_sib_b12_to_b17_conversion_is_needed

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_llc_test_sib_b12_to_b17_conversion_is_needed
(
  void
)
{
  return TRUE;

} /* lte_rrc_llc_test_sib_b12_to_b17_conversion_is_needed() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_sib_b12_to_b17_conversion_is_not_needed

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_llc_test_sib_b12_to_b17_conversion_is_not_needed
(
  void
)
{
  return FALSE;

} /* lte_rrc_llc_test_sib_b12_to_b17_conversion_is_not_needed() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_3gpp_release_version_for_rel8

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API.

    @return
    boolean
*/
/*=========================================================================*/
lte_3gpp_release_ver_e lte_rrc_llc_test_get_3gpp_release_version_for_rel8
(
  void
)
{
  return LTE_3GPP_REL8_MAR09;

} /* lte_rrc_llc_test_get_3gpp_release_version_for_rel8 */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_3gpp_release_version_for_rel8

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API.

    @return
    boolean
*/
/*=========================================================================*/
lte_errno_e lte_rrc_llc_test_get_eutra_meas_obj_param
(
  lte_earfcn_t ref_earfcn,
  lte_cphy_meas_obj_eutra_s *eutra_meas_obj_ptr
)
{
  return E_FAILURE;

} /* lte_rrc_llc_test_get_eutra_meas_obj_param */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_3gpp_release_version_for_rel9

===========================================================================*/
/*!
    @brief
    This function simulates the Utils API.

    @return
    boolean
*/
/*=========================================================================*/
lte_3gpp_release_ver_e lte_rrc_llc_test_get_3gpp_release_version_for_rel9
(
  void
)
{
  return LTE_3GPP_REL9_JUN10;

} /* lte_rrc_llc_test_get_3gpp_release_version_for_rel9 */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_3gpp_release_version_for_rel10

===========================================================================*/
/*!
    @brief
    This function simulates the Utils API.

    @return
    boolean
*/
/*=========================================================================*/
lte_3gpp_release_ver_e lte_rrc_llc_test_get_3gpp_release_version_for_rel10
(
  void
)
{
  return LTE_3GPP_REL10;

} /* lte_rrc_llc_test_get_3gpp_release_version_for_rel10 */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_3gpp_release_version_for_rel11

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API.

    @return
    boolean
*/
/*=========================================================================*/
lte_3gpp_release_ver_e lte_rrc_llc_test_get_3gpp_release_version_for_rel11
(
  void
)
{
  return LTE_3GPP_REL11;

} /* lte_rrc_llc_test_get_3gpp_release_version_for_rel11 */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_rohc_enabled

===========================================================================*/
/*!
    @brief
    This function simulates the CAP API

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_llc_test_get_rohc_enabled
(
  uint8 cap_bit
)
{
  if (cap_bit == LTE_RRC_CAP_R11_BIT_2)
  {
    return TRUE;
  }
  return FALSE;

} /* lte_rrc_llc_test_get_rohc_enabled */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_ho_in_progress

===========================================================================*/
/*!
    @brief
    This function simulates the CFG HO API

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_llc_test_get_ho_in_progress
(
  void
)
{
  return TRUE;

} /* lte_rrc_llc_test_get_ho_in_progress */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_r11_cap_bit_ssf9_enabled

===========================================================================*/
/*!
    @brief
    This function simulates the CAP API

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_llc_test_get_r11_cap_bit_ssf9_enabled
(
  uint8 cap_bit
)
{
  if (cap_bit == LTE_RRC_CAP_R11_BIT_9)
  {
    return TRUE;
  }
  return FALSE;

} /* lte_rrc_llc_test_get_r11_cap_bit_ssf9_enabled */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_band_combo_bitmask_FF

===========================================================================*/
/*!
    @brief
    This function simulates the CAP API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_llc_test_get_band_combo_bitmask_FF
(
  sys_sband_lte_e_type camp_band,
  uint8 num_cc, /* NUmber of Configured cells */
  const sys_sband_lte_e_type *cell_band, /* Configured cell bands */
  const lte_bandwidth_e *cell_bw, /* Configured cell bandwith */
  const uint8 *cell_bw_class, /* Configured cell bandwith class*/
  boolean disable_bw_validation /* Disable all BW related validations */
)
{
  return TRUE;

} /* lte_rrc_llc_test_get_band_combo_bitmask_FF */

#ifdef FEATURE_LTE_CA
/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_cap_is_ca_bandwidth_combo_valid_true

===========================================================================*/
/*!
    @brief
    This function simulates the CAP API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_llc_test_cap_is_ca_bandwidth_combo_valid_true
(
   sys_sband_lte_e_type pcell_band, 
   sys_sband_lte_e_type scell_band,
   lte_bandwidth_e pcell_dl_bandwidth, /*Pcell bandwidth */
   lte_bandwidth_e scell_dl_bandwidth, /*Scell bandwidth */
   lte_rrc_osys_CA_BandwidthClass_r10_Root bw_calss
)
{
    return TRUE;
 
} /* lte_rrc_llc_test_cap_is_ca_bandwidth_combo_valid_true */
#endif

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_tdd_prune_required_false

===========================================================================*/
/*!
  @brief
  Simulates the function that returns if TDD pruning is required

  @return
  boolean
*/
/*=========================================================================*/
boolean lte_rrc_llc_test_get_tdd_prune_required_false
(
  void
)
{

  return FALSE;

} /* lte_rrc_llc_test_get_tdd_prune_required_false */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_last_camped_mode_fdd

===========================================================================*/
/*!
    @brief
    Simulates the CSP fn that returns the camped cell duplex mode

    @return
    lte_rrc_div_duplex_e

*/
/*=========================================================================*/
lte_rrc_div_duplex_e lte_rrc_llc_test_get_last_camped_mode_fdd
(
  void
)
{
  return LTE_RRC_FREQ_DIV_DUPLEX;

} /* lte_rrc_llc_test_get_last_camped_mode_fdd */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_fgi_bit_1

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_fgi_bit_1
(
  uint8 bit_pos
)
{
  return 1;

} /* lte_rrc_llc_test_get_fgi_bit_1() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_fgi_bit_disable_105

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_fgi_bit_disable_105
(
  uint8 bit_pos
)
{
   if (bit_pos == 105) 
   {
      return 0;

   }
   else
   {
      return 1;
   }

} /* lte_rrc_llc_test_get_fgi_bit_disable_105() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_fgi_bit_disable_110

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_fgi_bit_disable_110
(
  uint8 bit_pos
)
{
   if (bit_pos == 110) 
   {
      return 0;

   }
   else
   {
      return 1;
   }

} /* lte_rrc_llc_test_get_fgi_bit_disable_110() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_fgi_bit_disable_109

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_fgi_bit_disable_109
(
  uint8 bit_pos
)
{
   if (bit_pos == 109) 
   {
      return 0;

   }
   else
   {
      return 1;
   }

} /* lte_rrc_llc_test_get_fgi_bit_disable_109() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_fgi_bit_disable_106

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_fgi_bit_disable_106
(
  uint8 bit_pos
)
{
   if (bit_pos == 106) 
   {
      return 0;

   }
   else
   {
      return 1;
   }

} /* lte_rrc_llc_test_get_fgi_bit_disable_106() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_fgi_bit_disable_107

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_fgi_bit_disable_107
(
  uint8 bit_pos
)
{
   if (bit_pos == 107) 
   {
      return 0;

   }
   else
   {
      return 1;
   }

} /* lte_rrc_llc_test_get_fgi_bit_disable_107() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_fgi_bit_disable_108

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_fgi_bit_disable_108
(
  uint8 bit_pos
)
{
   if (bit_pos == 108) 
   {
      return 0;

   }
   else
   {
      return 1;
   }

} /* lte_rrc_llc_test_get_fgi_bit_disable_108() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_fgi_bit_0

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_fgi_bit_0
(
  uint8 bit_pos
)
{
  return 0;

} /* lte_rrc_llc_test_get_fgi_bit_0() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rohc_is_supported

===========================================================================*/
/*!
    @brief
    This function simulates the function that returns if ROHC is supported

    @return
    boolean, TRUE means support is present.

*/
/*=========================================================================*/
boolean lte_rrc_llc_test_rohc_is_supported
(
  void
)
{
  return TRUE;

} /* lte_rrc_llc_test_rohc_is_supported */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_to_eutra_in_progress_false

===========================================================================*/
/*!
    @brief
    This function simulates the function that returns if HO to EUTRA in progress

    @return
    boolean, TRUE means HO to EUTRA is in progress.

*/
/*=========================================================================*/
boolean lte_rrc_llc_test_ho_to_eutra_in_progress_false
(
  void
)
{
  return FALSE;

} /* lte_rrc_llc_test_ho_to_eutra_in_progress_false */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_to_eutra_in_progress_true

===========================================================================*/
/*!
    @brief
    This function simulates the function that returns if HO to EUTRA in progress

    @return
    boolean, TRUE means HO to EUTRA is in progress.

*/
/*=========================================================================*/
boolean lte_rrc_llc_test_ho_to_eutra_in_progress_true
(
  void
)
{
  return TRUE;

} /* lte_rrc_llc_test_ho_to_eutra_in_progress_true */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_esmgr_get_oos_warn_reason_none

===========================================================================*/
/*!
    @brief
    The function returns the OOS warn reason

    @return
    lte_rrc_emp_warn_reason_e, the OOS warn reason

*/
/*=========================================================================*/
lte_rrc_emp_warn_reason_e lte_rrc_llc_test_esmgr_get_oos_warn_reason_none
(
  void
)
{
  return LTE_RRC_EMP_WARN_REASON_NONE;

} /* lte_rrc_llc_test_esmgr_get_oos_warn_reason_none */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_esmgr_get_oos_warn_reason_ucast

===========================================================================*/
/*!
    @brief
    The function returns the OOS warn reason

    @return
    lte_rrc_emp_warn_reason_e, the OOS warn reason

*/
/*=========================================================================*/
lte_rrc_emp_warn_reason_e lte_rrc_llc_test_esmgr_get_oos_warn_reason_ucast
(
  void
)
{
  return LTE_RRC_EMP_WARN_REASON_OOS_UNICAST;

} /* lte_rrc_llc_test_esmgr_get_oos_warn_reason_ucast */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_last_camped_mode_tdd

===========================================================================*/
/*!
    @brief
    Simulates the CSP fn that returns the camped cell duplex mode

    @return
    lte_rrc_div_duplex_e

*/
/*=========================================================================*/
lte_rrc_div_duplex_e lte_rrc_llc_test_get_last_camped_mode_tdd
(
  void
)
{
  return LTE_RRC_TIME_DIV_DUPLEX;

} /* lte_rrc_llc_test_get_last_camped_mode_tdd */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_free_sib_ptr

===========================================================================*/
/*!
    @brief
    This function frees SIB2 ptr

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_free_sib_ptr
(
  void
)
{
  if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr != NULL) 
  {
    TF_MSG("RRC sib1 mem free 0x%x ",lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr);

    lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr, 
                               lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
    lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr = NULL; 
  }

  if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr != NULL) 
  {
    TF_MSG("RRC sib2 mem free 0x%x ",lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr);

    lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr, 
                               lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
    lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr = NULL; 
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_free_dl_ded_msg_ptr

===========================================================================*/
/*!
    @brief
    This function frees DL dedicated messages on CCCH and DCCH.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_free_dl_ded_msg_ptr
(
  void
)
{
  if (lte_rrc_llc_test_common_data.dl_ccch_msg_ptr != NULL) 
  {
    lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_test_common_data.dl_ccch_msg_ptr, 
                               lte_rrc_osys_DL_CCCH_Message_PDU);
    lte_rrc_llc_test_common_data.dl_ccch_msg_ptr = NULL; 
  }

  if (lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL) 
  {
    lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr, 
                               lte_rrc_osys_DL_DCCH_Message_PDU);
    lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = NULL; 
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_controller_get_state

===========================================================================*/
/*!
    @brief
    Returns a simulated the current state of RRC Controller

    @return
    stm_state_t

*/
/*=========================================================================*/

stm_state_t lte_rrc_llc_test_controller_get_state(void)
{
  if ((lte_rrc_llc_data.dd_ptr->cmd_db.cfg_info.cfg_type == LTE_RRC_CFG_DED) ||
      (lte_rrc_llc_data.dd_ptr->cmd_db.cfg_info.cfg_type == LTE_RRC_CFG_RLF) ||
      (lte_rrc_llc_data.dd_ptr->cmd_db.proc == LTE_RRC_PROC_MAX))
  {
    return (LTE_RRC_CONTROLLER_SM__CONNECTED);
  }
  else if (lte_rrc_llc_data.dd_ptr->cmd_db.cfg_info.cfg_type == LTE_RRC_CFG_CONN_REL)
  {
    return (LTE_RRC_CONTROLLER_SM__CLOSING);
  }
  else
  {
    return (LTE_RRC_CONTROLLER_SM__IDLE_CAMPED);
  }
} /* lte_rrc_llc_test_controller_get_state */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_controller_get_connected_state

===========================================================================*/
/*!
    @brief
    Returns a simulated the current state of RRC Controller

    @return
    stm_state_t

*/
/*=========================================================================*/

stm_state_t lte_rrc_llc_test_controller_get_connected_state(void)
{
  return (LTE_RRC_CONTROLLER_SM__CONNECTED);
} /* lte_rrc_llc_test_controller_get_connected_state */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_csp_lte_band_is_supported

===========================================================================*/
/*!
    @brief
    Returns a simulated TRUE for lte band support query

    @return
    boolean

*/
/*=========================================================================*/

boolean lte_rrc_llc_test_csp_lte_band_is_supported(sys_sband_lte_e_type band)
{
  return TRUE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_cphy_rf_status_available

===========================================================================*/
/*!
    @brief
    Returns a simulated TRUE for CPHY RF Status

    @return
    void

*/
/*=========================================================================*/

void lte_rrc_llc_test_cphy_rf_status_available(lte_rrc_rf_status_info_s *info_ptr)
{
  info_ptr->rf_status = LTE_RRC_RF_AVAILABLE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_no_dsds_mode

===========================================================================*/
/*!
    @brief
    Returns a simulated FALSE for DSDS mode

    @return
    FALSE

*/
/*=========================================================================*/

boolean lte_rrc_llc_test_no_dsds_mode(void)
{
  return FALSE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_dsds_mode

===========================================================================*/
/*!
    @brief
    Returns a simulated TRUE for DSDS mode

    @return
    TRUE

*/
/*=========================================================================*/

boolean lte_rrc_llc_test_dsds_mode(void)
{
  return TRUE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_transmit_antenna_selection_is_supported_true

===========================================================================*/
/*!
    @brief
    Returns a simulated TRUE 
    lte_rrc_llc_test_transmit_antenna_selection_is_supported_true

    @return
    TRUE

*/
/*=========================================================================*/

boolean lte_rrc_llc_test_transmit_antenna_selection_is_supported_true(boolean tdd_cell)
{
  return TRUE;
}


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_cphy_rf_status_un_available

===========================================================================*/
/*!
    @brief
    Returns a simulated FALSE for CPHY RF Status

    @return
    void

*/
/*=========================================================================*/

void lte_rrc_llc_test_cphy_rf_status_un_available(lte_rrc_rf_status_info_s *info_ptr)
{
  info_ptr->rf_status = LTE_RRC_RF_UNAVAILABLE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_cfg_is_for_tdd_cell_true

===========================================================================*/
/*!
    @brief
    Returns a simulated TRUE for tdd cell

    @return
    boolean

*/
/*=========================================================================*/

boolean lte_rrc_llc_test_cfg_is_for_tdd_cell_true( lte_earfcn_t freq )
{
  return TRUE;
}

boolean lte_rrc_llc_test_cfg_is_for_tdd_cell_false( lte_earfcn_t freq )
{
  return FALSE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi

===========================================================================*/
/*!
    @brief
    This function constructs and sends dedicated LTE_RRC_CFG_REQI to LLC for 
    unit testing purpose.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_construct_and_send_ded_cfg_reqi
(
  lte_rrc_pdu_buf_s *encoded_pdu_ptr, /*! Ptr to PDU to be decoded and sent to LLC */
  lte_rrc_proc_e source_proc,  /*!< source RRC proc */ 
  boolean pdu_decoded   /*!< if the PDU has been decoded */
)
{
  lte_rrc_cfg_reqi_s cfg_req;
  int decode_status = 0;

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_req.msg_hdr,LTE_RRC_CFG_REQI);
  /* Send the decoded Connection Setup message to LLC module */
  cfg_req.cnfi_required = TRUE;
  cfg_req.num_cfg = 1;
  cfg_req.proc = source_proc;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_req.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_req.cfg_info[0].cfg_type = LTE_RRC_CFG_DED;

  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  if ((source_proc == LTE_RRC_PROC_CEP) || (source_proc == LTE_RRC_PROC_CRE))
  {
    if (pdu_decoded == FALSE)
    {
      lte_rrc_llc_test_common_data.dl_ccch_msg_ptr = (lte_rrc_osys_DL_CCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu_ptr->value, encoded_pdu_ptr->length, 
                                   DL_CCCH_Message, &decode_status);
      TF_ASSERT(decode_status == 0);

    }
    cfg_req.cfg_info[0].cfg_u.ded_cfg.msg_ptr = lte_rrc_llc_test_common_data.dl_ccch_msg_ptr;
  }
  else if (source_proc == LTE_RRC_PROC_CONFIG)
  {
    if (pdu_decoded == FALSE)
    {
      lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu_ptr->value, encoded_pdu_ptr->length, 
                                   DL_DCCH_Message, &decode_status);
      TF_ASSERT(decode_status == 0);
    }
    cfg_req.cfg_info[0].cfg_u.ded_cfg.msg_ptr = lte_rrc_llc_test_common_data.dl_dcch_msg_ptr;

    if (lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
        criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->m.mobilityControlInfoPresent)
    {
      lte_rrc_llc_test_common_data.ho_in_progress = TRUE;
    }
  }
  else if (source_proc == LTE_RRC_PROC_CRP)
  {
    cfg_req.cfg_info[0].cfg_type = LTE_RRC_CFG_CONN_REL;
    cfg_req.cfg_info[0].cfg_u.rel_cfg.ml1_status = LTE_RRC_ML1_STATUS_NORMAL;
  }
  else
  {
    TF_ASSERT(FALSE);
  }

  /* On receiving a CFG_REQI, LLC should be in the INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  lte_rrc_utf_send_msg((byte*)&cfg_req, sizeof(cfg_req));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf

===========================================================================*/
/*!
    @brief
    This function constructs and sends LTE_RRC_CFG_REQI to LLC for RLF.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf
(
  lte_rrc_ho_failure_cause_e ho_failure_cause
)
{
  lte_rrc_cfg_reqi_s cfg_req;
   /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_req.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_req.cnfi_required = TRUE;
  cfg_req.proc = LTE_RRC_PROC_CRE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_req.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_req.num_cfg = 1;
  cfg_req.cfg_info[0].cfg_type = LTE_RRC_CFG_RLF;
  if ((ho_failure_cause == LTE_RRC_HO_FAILURE_CAUSE_RACH) ||
      (ho_failure_cause == LTE_RRC_HO_FAILURE_CAUSE_RACH_MEAS))
  {
    cfg_req.cfg_info[0].cfg_u.susp_cfg.revert_l2_config = TRUE;
  }
  else
  {
    cfg_req.cfg_info[0].cfg_u.susp_cfg.revert_l2_config = FALSE;
  }

  lte_rrc_utf_send_msg((byte*)&cfg_req, sizeof(cfg_req));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup

===========================================================================*/
/*!
    @brief
    This function constructs and sends LTE_RRC_CFG_REQI to LLC for eMBMS.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup
(
  void
)
{
  lte_rrc_cfg_reqi_s cfg_req;
  
  
#ifdef FEATURE_LTE_EMBMS
   /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_req.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_req.cnfi_required = TRUE;
  cfg_req.proc = LTE_RRC_PROC_EMP;
  
  cfg_req.num_cfg = 1;
  cfg_req.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_req.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_req.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mcch = 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 101;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mtch = 0;

  /* Make a copy of the LLC CFG_REQI */
  LTE_RRC_MEMCPY(&lte_rrc_llc_test_common_data.llc_cfg_req, sizeof(lte_rrc_cfg_reqi_s), 
                 &cfg_req, sizeof(cfg_req));

  lte_rrc_utf_send_msg((byte*)&cfg_req, sizeof(cfg_req));
 #endif
 
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup

===========================================================================*/
/*!
    @brief
    This function constructs and sends LTE_RRC_CFG_REQI to LLC for eMBMS.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup
(
  void
)
{
  lte_rrc_cfg_reqi_s cfg_req;
  
#ifdef FEATURE_LTE_EMBMS
   /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_req.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_req.cnfi_required = TRUE;
  cfg_req.proc = LTE_RRC_PROC_EMP;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_req.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_req.num_cfg = 1;
  cfg_req.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mcch = 0;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mtch = 4;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_id = LTE_MIN_MTCH_RB_ID;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.lc_id = 28;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x21;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x63;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x54;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xFF;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x0F;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_id = LTE_MIN_MTCH_RB_ID + 1;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.lc_id = 0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.session_id_present = TRUE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.session_id = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x21;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x63;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x54;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0x0F;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0x0F;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x0F;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_id = LTE_MIN_MTCH_RB_ID + 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.pmch_id = 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.lc_id = 0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x54;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x36;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x21;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0xF0;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_id = LTE_MIN_MTCH_RB_ID + 3;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.pmch_id = 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.lc_id = 10;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x90;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x41;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x40;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xFF;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xFF;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0xFF;

  /* Make a copy of the LLC CFG_REQI */
  LTE_RRC_MEMCPY(&lte_rrc_llc_test_common_data.llc_cfg_req, sizeof(lte_rrc_cfg_reqi_s),
                 &cfg_req, sizeof(cfg_req));

  lte_rrc_utf_send_msg((byte*)&cfg_req, sizeof(cfg_req));
  #endif
  
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_same_pmch

===========================================================================*/
/*!
    @brief
    This function constructs and sends LTE_RRC_CFG_REQI to LLC for eMBMS.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_same_pmch
(
  void
)
{
  lte_rrc_cfg_reqi_s cfg_req;
  
#ifdef FEATURE_LTE_EMBMS
   /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_req.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_req.cnfi_required = TRUE;
  cfg_req.proc = LTE_RRC_PROC_EMP;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_req.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_req.num_cfg = 1;
  cfg_req.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mcch = 0;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mtch = 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_id = LTE_MIN_MTCH_RB_ID;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.pmch_id = 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.lc_id = 0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x54;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x36;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x21;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0xF0;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_id = LTE_MIN_MTCH_RB_ID + 1;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.pmch_id = 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.lc_id = 10;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x90;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x41;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x40;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xFF;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xFF;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0xFF;

  /* Make a copy of the LLC CFG_REQI */
  LTE_RRC_MEMCPY(&lte_rrc_llc_test_common_data.llc_cfg_req, sizeof(lte_rrc_cfg_reqi_s),
                 &cfg_req, sizeof(cfg_req));

  lte_rrc_utf_send_msg((byte*)&cfg_req, sizeof(cfg_req));
  #endif
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_feicic_crs_info

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing SCC/PCC CQI R10 cfg

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_scc_cqi_reporting
(
  lte_rrc_pdu_buf_s encoded_pdu
)
{   
  int decode_status = 0;

  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  uint32 buf_len;
  lte_mac_cfg_req_msg_s *mac_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);
  TF_MSG("\nLLC: Rcvd CPHY_DEDICATED_CFG_REQ \n");

  /* After CPHY Config req is sent, assert LLC is in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  TF_MSG("\nCPHY_DEDICATED_CFG_REQ rcvd and LLC Module in WAIT_FOR_CPHY_CFG_CNF state\n");

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  /* Setting the fields in the LTE_CPHY_DEDICATED_CFG_CNF sent by CPHY, The bits
    set in dedicated_cfg_validity field reflect the channel config that were set
    in the llc_conn_setup_msg in the lte_rrc_llc_ota_msg.h */
  cphy_dedicated_cfg_cnf.dedicated_cfg_validity = cphy_dedicated_cfg_req_ptr->dedicated_cfg_param.cfg_mask;
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  lte_rrc_utf_get_next_ext_msg((byte **) &mac_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  /* LLC should be in WAIT_FOR_MAC_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);
  TF_ASSERT(mac_req_ptr != NULL);

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf.hdr,LTE_MAC_CFG_CNF);
  mac_cfg_cnf.status = E_SUCCESS;
  
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nConnection Reconfiguration Test Ends \n");

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
} /* lte_rrc_llc_test_scc_cqi_reporting() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_invalid_scc_cqi_reporting

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing SCC/PCC CQI R10 cfg

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_invalid_scc_cqi_reporting
(
  lte_rrc_pdu_buf_s encoded_pdu
)
{   
  int decode_status = 0;

  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  uint32 buf_len;
  lte_mac_cfg_req_msg_s *mac_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
} /* lte_rrc_llc_test_invalid_scc_cqi_reporting() */



/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_mtch_setup

===========================================================================*/
/*!
    @brief
    This function constructs and sends LTE_RRC_CFG_REQI to LLC for eMBMS.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_mtch_setup
(
  void
)
{
  lte_rrc_cfg_reqi_s cfg_req;
  
#ifdef FEATURE_LTE_EMBMS
   /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_req.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_req.cnfi_required = TRUE;
  cfg_req.proc = LTE_RRC_PROC_EMP;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_req.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_req.num_cfg = 1;
  cfg_req.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mcch = 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 101;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mtch = 4;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_id = LTE_MIN_MTCH_RB_ID;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.lc_id = 28;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x21;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x63;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x54;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xFF;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x0F;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_id = LTE_MIN_MTCH_RB_ID + 1;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.lc_id = 0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.session_id_present = TRUE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.session_id = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x21;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x63;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x54;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0x0F;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0x0F;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x0F;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_id = LTE_MIN_MTCH_RB_ID + 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.pmch_id = 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.lc_id = 0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x54;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x36;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x21;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0xF0;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_id = LTE_MIN_MTCH_RB_ID + 3;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.pmch_id = 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.lc_id = 10;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x90;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x41;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x40;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xFF;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xFF;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0xFF;

  /* Make a copy of the LLC CFG_REQI */
  LTE_RRC_MEMCPY(&lte_rrc_llc_test_common_data.llc_cfg_req, sizeof(lte_rrc_cfg_reqi_s),
                 &cfg_req, sizeof(cfg_req));

  lte_rrc_utf_send_msg((byte*)&cfg_req, sizeof(cfg_req));
  #endif
  
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mrb_reconfig

===========================================================================*/
/*!
    @brief
    This function constructs and sends LTE_RRC_CFG_REQI to LLC for eMBMS.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mrb_reconfig
(
  void
)
{
  lte_rrc_cfg_reqi_s cfg_req;
  
#ifdef FEATURE_LTE_EMBMS
  lte_rrc_osys_SystemInformationBlockType13_r9 *sib13_ptr 
    = lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920;

   /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_req.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_req.cnfi_required = TRUE;
  cfg_req.proc = LTE_RRC_PROC_EMP;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_req.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_req.num_cfg = 1;
  cfg_req.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mcch = 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 101;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_RELEASE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[1].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mtch = 4;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_id = LTE_MIN_MTCH_RB_ID;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_RELEASE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.lc_id = 28;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x21;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x63;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x54;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xFF;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x0F;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_id = LTE_MIN_MTCH_RB_ID + 1;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  // LC modification
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.lc_id = 5;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.session_id_present = TRUE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.session_id = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x21;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x63;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x54;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0x0F;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0x0F;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x0F;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_id = LTE_MIN_MTCH_RB_ID + 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  // PMCH modification
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.lc_id = 10;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x54;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x36;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x21;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xF0;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0xF0;

  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_id = LTE_MIN_MTCH_RB_ID + 3;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_RELEASE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.pmch_id = 2;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.lc_id = 10;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x90;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x41;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x40;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xFF;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xFF;
  cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[3].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0xFF;

  /* Make a copy of the LLC CFG_REQI */
  LTE_RRC_MEMCPY(&lte_rrc_llc_test_common_data.llc_cfg_req, sizeof(lte_rrc_cfg_reqi_s),
                 &cfg_req, sizeof(cfg_req));

  lte_rrc_utf_send_msg((byte*)&cfg_req, sizeof(cfg_req));
  #endif
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mrb_rel

===========================================================================*/
/*!
    @brief
    This function constructs and sends LTE_RRC_CFG_REQI to LLC for eMBMS.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mrb_rel
(
  boolean release_all_mrb
)
{
  lte_rrc_cfg_reqi_s cfg_req;
  
#ifdef FEATURE_LTE_EMBMS
   /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_req.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_req.cnfi_required = TRUE;
  cfg_req.proc = LTE_RRC_PROC_EMP;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_req.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_req.num_cfg = 1;
  cfg_req.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;

  if (release_all_mrb)
  {
    cfg_req.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = TRUE;
  }
  else
  {
    cfg_req.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mcch = 1;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 101;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_RELEASE;

    cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mtch = 3;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_id = LTE_MIN_MTCH_RB_ID;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_RELEASE;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.pmch_id = 1;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.lc_id = 28;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x21;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x63;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x54;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xFF;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xF0;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x0F;

    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_id = LTE_MIN_MTCH_RB_ID + 1;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_RELEASE;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.pmch_id = 1;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.lc_id = 0;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.session_id_present = TRUE;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.session_id = 0xF0;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x21;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x63;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x54;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0x0F;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0x0F;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[1].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x0F;

    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_id = LTE_MIN_MTCH_RB_ID + 2;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_RELEASE;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.pmch_id = 2;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.lc_id = 0;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.session_id_present = FALSE;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x54;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x36;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x21;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0xF0;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0xF0;
    cfg_req.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[2].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0xF0;
  }

  /* Make a copy of the LLC CFG_REQI */
  LTE_RRC_MEMCPY(&lte_rrc_llc_test_common_data.llc_cfg_req, sizeof(lte_rrc_cfg_reqi_s),
                 &cfg_req, sizeof(cfg_req));

  lte_rrc_utf_send_msg((byte*)&cfg_req, sizeof(cfg_req));
  #endif
  
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_lte_stop

===========================================================================*/
/*!
    @brief
    This function constructs and sends LTE_RRC_CFG_REQI to LLC for eMBMS.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_lte_stop
(
  void
)
{
  lte_rrc_cfg_reqi_s cfg_req;
   /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_req.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_req.cnfi_required = TRUE;
  cfg_req.proc = LTE_RRC_PROC_CONTROLLER;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_req.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_req.num_cfg = 1;
  cfg_req.cfg_info[0].cfg_type = LTE_RRC_CFG_DEACTIVATE_FS;

  /* Make a copy of the LLC CFG_REQI */
  LTE_RRC_MEMCPY(&lte_rrc_llc_test_common_data.llc_cfg_req, sizeof(lte_rrc_cfg_reqi_s),
                 &cfg_req, sizeof(cfg_req));

  lte_rrc_utf_send_msg((byte*)&cfg_req, sizeof(cfg_req));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pdsch_cfg_common

===========================================================================*/
/*!
    @brief
    This function tests the PDSCH common config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_pdsch_cfg_common
(
  lte_rrc_osys_PDSCH_ConfigCommon *ota_pdsch_cfg_common_ptr,
  lte_cphy_pdsch_cfg_common_param_s *pdsch_cfg_common_ptr
)
{
  TF_ASSERT(pdsch_cfg_common_ptr->pb == ota_pdsch_cfg_common_ptr->p_b);
  TF_ASSERT(pdsch_cfg_common_ptr->ref_signal_pwr == ota_pdsch_cfg_common_ptr->referenceSignalPower);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pusch_cfg_common

===========================================================================*/
/*!
    @brief
    This function tests the PUSCH common config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_pusch_cfg_common
(
  lte_rrc_osys_PUSCH_ConfigCommon *ota_pusch_cfg_common_ptr,
  lte_cphy_pusch_cfg_common_param_s *pusch_cfg_common_ptr
)
{
  TF_ASSERT(pusch_cfg_common_ptr->n_sb 
            == ota_pusch_cfg_common_ptr->pusch_ConfigBasic.n_SB);
  TF_ASSERT(pusch_cfg_common_ptr->hopping_mode 
            == ota_pusch_cfg_common_ptr->pusch_ConfigBasic.hoppingMode);
  TF_ASSERT(pusch_cfg_common_ptr->hopping_offset 
            == ota_pusch_cfg_common_ptr->pusch_ConfigBasic.pusch_HoppingOffset);
  TF_ASSERT(pusch_cfg_common_ptr->enable_64_qam 
            == ota_pusch_cfg_common_ptr->pusch_ConfigBasic.enable64QAM);

  /* UL Reference signals PUSCH*/
  TF_ASSERT(pusch_cfg_common_ptr->group_hopping_enabled 
            == ota_pusch_cfg_common_ptr->ul_ReferenceSignalsPUSCH.groupHoppingEnabled);
  TF_ASSERT(pusch_cfg_common_ptr->group_assignment_pusch 
            == ota_pusch_cfg_common_ptr->ul_ReferenceSignalsPUSCH.groupAssignmentPUSCH);
  TF_ASSERT(pusch_cfg_common_ptr->seq_hopping_enabled 
            == ota_pusch_cfg_common_ptr->ul_ReferenceSignalsPUSCH.sequenceHoppingEnabled);
  TF_ASSERT(pusch_cfg_common_ptr->cyclic_shift 
            == ota_pusch_cfg_common_ptr->ul_ReferenceSignalsPUSCH.cyclicShift);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pucch_cfg_common

===========================================================================*/
/*!
    @brief
    This function tests the PUCCH common config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_pucch_cfg_common
(
  lte_rrc_osys_PUCCH_ConfigCommon *ota_pucch_cfg_common_ptr,
  lte_cphy_pucch_cfg_common_param_s *pucch_cfg_common_ptr
)
{
  /* L1 enum for delta shift starts at 1 whereas the SIB2 enum starts at 0, so increment 1*/
  TF_ASSERT(pucch_cfg_common_ptr->delta_shift 
            == ota_pucch_cfg_common_ptr->deltaPUCCH_Shift + 1);
  TF_ASSERT(pucch_cfg_common_ptr->n_rb_cqi == ota_pucch_cfg_common_ptr->nRB_CQI);
  TF_ASSERT(pucch_cfg_common_ptr->n_cs_an == ota_pucch_cfg_common_ptr->nCS_AN);
  TF_ASSERT(pucch_cfg_common_ptr->n1_pucch_an == ota_pucch_cfg_common_ptr->n1PUCCH_AN);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_srs_cfg_common

===========================================================================*/
/*!
    @brief
    This function tests the SRS common config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_srs_cfg_common
(
  lte_rrc_osys_SoundingRS_UL_ConfigCommon *ota_srs_cfg_common_ptr,
  lte_cphy_srs_cfg_common_param_s *srs_cfg_common_ptr,
  boolean tdd_cfg
)
{
  if (ota_srs_cfg_common_ptr->t == T_lte_rrc_osys_SoundingRS_UL_ConfigCommon_release)
  {
    TF_ASSERT(srs_cfg_common_ptr->enable == FALSE);
  }
  else 
  {
    TF_ASSERT(srs_cfg_common_ptr->enable == TRUE);
    TF_ASSERT(srs_cfg_common_ptr->srs_bw_cfg 
              == ota_srs_cfg_common_ptr->u.setup->srs_BandwidthConfig);
    TF_ASSERT(srs_cfg_common_ptr->srs_subframe_config 
              == ota_srs_cfg_common_ptr->u.setup->srs_SubframeConfig);
    TF_ASSERT(srs_cfg_common_ptr->sim_ack_nak_srs 
              == ota_srs_cfg_common_ptr->u.setup->ackNackSRS_SimultaneousTransmission);
    if ((tdd_cfg) && (ota_srs_cfg_common_ptr->u.setup->m.srs_MaxUpPtsPresent != 0))
    {
      TF_ASSERT(srs_cfg_common_ptr->srx_max_up_pts == TRUE);
      lte_rrc_llc_test_common_data.tdd_param.srx_max_up_pts_configured = TRUE;
    }
    else
    {
      TF_ASSERT(srs_cfg_common_ptr->srx_max_up_pts == FALSE);
    }
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ul_pwr_ctrl_cfg_common

===========================================================================*/
/*!
    @brief
    This function tests the UL Power Ctrl common config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_ul_pwr_ctrl_cfg_common
(
  lte_rrc_osys_UplinkPowerControlCommon *ota_ul_pwr_ctrl_cfg_ptr,
  lte_cphy_ul_power_ctrl_common_param_s *ul_pwr_ctrl_cfg_ptr
)
{
    int pucch_format_val = 0;

    TF_ASSERT(ul_pwr_ctrl_cfg_ptr->p0_nominal_pusch == ota_ul_pwr_ctrl_cfg_ptr->p0_NominalPUSCH);
    TF_ASSERT(ul_pwr_ctrl_cfg_ptr->alpha == ota_ul_pwr_ctrl_cfg_ptr->alpha);
    TF_ASSERT(ul_pwr_ctrl_cfg_ptr->p0_nominal_pucch == ota_ul_pwr_ctrl_cfg_ptr->p0_NominalPUCCH);

    /* Check format1 */
    pucch_format_val = ota_ul_pwr_ctrl_cfg_ptr->deltaFList_PUCCH.deltaF_PUCCH_Format1;
    /* format1 enum values from SIB2 are {0,1,2} whereas ML1 enum values are {-2,0,2},
      so do the appropriate conversion before checking */
    if (pucch_format_val == 0)
    {
    TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.format1 == (pucch_format_val - 2));
    }
    else if (pucch_format_val == 1)
    {
      TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.format1 == (pucch_format_val - 1));
    }
    else
    {
      TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.format1 == pucch_format_val);
    }

    /* Check format1b */
    pucch_format_val = (int) ota_ul_pwr_ctrl_cfg_ptr->deltaFList_PUCCH.deltaF_PUCCH_Format1b;
    /* format1b enum values from SIB2 are {0,1,2} whereas ML1 enum values are {1,3,5},
      so do the appropriate conversion before checking */
    TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.
              format1b == pucch_format_val + (pucch_format_val + 1));

    /* Check format2 */
    pucch_format_val = (int) ota_ul_pwr_ctrl_cfg_ptr->deltaFList_PUCCH.deltaF_PUCCH_Format2;
    /* format2 enum values from SIB2 are {0,1,2,3} whereas ML1 enum values are {-2,0,1,2},
      so do the appropriate conversion before checking */
    if (pucch_format_val == 0)
    {
      TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.
                format2 == (ota_ul_pwr_ctrl_cfg_ptr->deltaFList_PUCCH.deltaF_PUCCH_Format2 - 2));
    }
    else
    {
      TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.
                format2 == (ota_ul_pwr_ctrl_cfg_ptr->deltaFList_PUCCH.deltaF_PUCCH_Format2 - 1));
    }

    /* Check format2a */
    pucch_format_val = (int) ota_ul_pwr_ctrl_cfg_ptr->deltaFList_PUCCH.deltaF_PUCCH_Format2a;
    /* format2a enum values from SIB2 are {0,1,2} whereas ML1 enum values are {-2,0,2},
      so do the appropriate conversion before checking */
    if (pucch_format_val == 0)
    {
      TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.format2a == (pucch_format_val - 2));
    }
    else if (pucch_format_val == 1)
    {
      TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.format2a == (pucch_format_val - 1));
    }
    else
    {
      TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.format2a == pucch_format_val);
    }

    /* Check format2b */
    pucch_format_val = (int) ota_ul_pwr_ctrl_cfg_ptr->deltaFList_PUCCH.deltaF_PUCCH_Format2b;
    /* format2b enum values from SIB2 are {0,1,2} whereas ML1 enum values are {-2,0,2},
      so do the appropriate conversion before checking */
    if (pucch_format_val == 0)
    {
      TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.format2b == (pucch_format_val - 2));
    }
    else if (pucch_format_val == 1)
    {
      TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.format2b == (pucch_format_val - 1));
    }
    else
    {
      TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_tf_list_pucch.format2b == pucch_format_val);
    }

    /* Check delta_preamble_msg3 */
    TF_ASSERT(ul_pwr_ctrl_cfg_ptr->delta_preamble_msg3 == ota_ul_pwr_ctrl_cfg_ptr->deltaPreambleMsg3 * 2);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ul_freq_info

===========================================================================*/
/*!
    @brief
    This function tests the UL freq IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_ul_freq_info
(
  lte_rrc_osys_SystemInformationBlockType2* sib2_ptr,
  lte_cphy_ul_freq_cfg_s *ul_freq_ptr
)
{
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr = NULL;
  lte_bandwidth_e ul_bandwidth = LTE_BW_MIN;

  if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr)
  {
    sib1_ptr = lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1;
  }

  /* DO NOT change the order of the if-else-if statements below */
  if ((sib1_ptr) && (sib1_ptr->m.tdd_ConfigPresent != 0))
  {
    /* For TDD config, UL freq is same as DL freq */
    TF_ASSERT(ul_freq_ptr->ul_cell_frequency == 36000);
  }
  else if (sib2_ptr->freqInfo.m.ul_CarrierFreqPresent != 0)
  {
    TF_ASSERT(ul_freq_ptr->ul_cell_frequency == sib2_ptr->freqInfo.ul_CarrierFreq);
  }
  else
  { 
    /* Same as the default DL frequency set in the stub function 
      lte_rrc_llc_test_common_csp_get_serving_cell_params */
    TF_ASSERT(ul_freq_ptr->ul_cell_frequency == 18500);
  }

  if (sib2_ptr->freqInfo.m.ul_BandwidthPresent != 0)
  {
    /* Get UL Bandwidth Information from SIB2 */
    switch(sib2_ptr->freqInfo.ul_Bandwidth)
    {
      case lte_rrc_osys_SystemInformationBlockType2_freqInfo_ul_Bandwidth_n6:
        ul_bandwidth = LTE_BW_NRB_6;
        break;
      case lte_rrc_osys_SystemInformationBlockType2_freqInfo_ul_Bandwidth_n15:
        ul_bandwidth = LTE_BW_NRB_15;
        break;
      case lte_rrc_osys_SystemInformationBlockType2_freqInfo_ul_Bandwidth_n25:
        ul_bandwidth = LTE_BW_NRB_25;
        break;
      case lte_rrc_osys_SystemInformationBlockType2_freqInfo_ul_Bandwidth_n50:
        ul_bandwidth = LTE_BW_NRB_50;
        break;
      case lte_rrc_osys_SystemInformationBlockType2_freqInfo_ul_Bandwidth_n75:
        ul_bandwidth = LTE_BW_NRB_75;
        break;
      case lte_rrc_osys_SystemInformationBlockType2_freqInfo_ul_Bandwidth_n100:
        ul_bandwidth = LTE_BW_NRB_100;
        break;
      default:
        ul_bandwidth = LTE_BW_NRB_15;
    }
    TF_ASSERT(ul_freq_ptr->ul_bandwidth == ul_bandwidth);
  }
  else
  {
    /* Same as DL bandwidth set in the stub function lte_rrc_llc_test_csp_get_camped_cell_bw */
    TF_ASSERT(ul_freq_ptr->ul_bandwidth == LTE_BW_NRB_100);
  }

  TF_ASSERT(ul_freq_ptr->add_spectrum_emission_present == TRUE);
  TF_ASSERT(ul_freq_ptr->add_spectrum_emission == sib2_ptr->freqInfo.additionalSpectrumEmission);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mbsfn_cfg

===========================================================================*/
/*!
    @brief
    This function tests the MBSFN config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_mbsfn_cfg
(
  lte_rrc_osys_SystemInformationBlockType2* sib2_ptr,
  lte_cphy_mbsfn_cfg_s *mbsfn_cfg_ptr
)
{
  uint8 i = 0;
  lte_rrc_osys_MBSFN_SubframeConfigList ota_mbsfg_cfg_list;

  /* Check the value of number of MBMS allocations */
  TF_ASSERT(mbsfn_cfg_ptr->num_mbms_allocations == sib2_ptr->mbsfn_SubframeConfigList.n);
  ota_mbsfg_cfg_list = sib2_ptr->mbsfn_SubframeConfigList;

  /* Verify each of the MBMS allocation entries */
  for (i = 0; i < mbsfn_cfg_ptr->num_mbms_allocations; i++)
  {
    TF_ASSERT(mbsfn_cfg_ptr->mbsfn_alloacation[i].allocation_period 
              == ota_mbsfg_cfg_list.elem[i].radioframeAllocationPeriod);
    TF_ASSERT(mbsfn_cfg_ptr->mbsfn_alloacation[i].allocation_offset 
              == ota_mbsfg_cfg_list.elem[i].radioframeAllocationOffset);
    if (ota_mbsfg_cfg_list.elem[i].subframeAllocation.
        t == T_lte_rrc_osys_MBSFN_SubframeConfig_subframeAllocation_oneFrame)
    {
      TF_ASSERT(mbsfn_cfg_ptr->mbsfn_alloacation[i].sfn_allocation == LTE_L1_MBSFN_ALLOCATION_ONE);
      TF_ASSERT(mbsfn_cfg_ptr->mbsfn_alloacation[i].sfn_allocation_mask 
                == (uint32) lte_rrc_osys_asn1_bitstring_to_int(sizeof(mbsfn_cfg_ptr->mbsfn_alloacation[i].
                                                                 sfn_allocation_mask), 
                                                               (ota_mbsfg_cfg_list.elem[i].subframeAllocation.
                                                                 u.oneFrame)));
    }
    else
    {
      TF_ASSERT(mbsfn_cfg_ptr->mbsfn_alloacation[i].sfn_allocation == LTE_L1_MBSFN_ALLOCATION_FOUR);
      TF_ASSERT(mbsfn_cfg_ptr->mbsfn_alloacation[i].sfn_allocation_mask 
                == (uint32) lte_rrc_osys_asn1_bitstring_to_int(sizeof(mbsfn_cfg_ptr->mbsfn_alloacation[i].
                                                                 sfn_allocation_mask), 
                                                               (ota_mbsfg_cfg_list.elem[i].subframeAllocation.
                                                                 u.fourFrames)));
    }
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_cphy_prach_cfg

===========================================================================*/
/*!
    @brief
    This function tests the CPHY PRACH config processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_cphy_prach_cfg
(
  lte_rrc_osys_SystemInformationBlockType2* sib2_ptr,
  lte_cphy_prach_cfg_common_param_s *prach_cfg_ptr
)
{
  /* Check the value of hihg speed flag */
  TF_ASSERT(prach_cfg_ptr->high_speed_flag 
            == sib2_ptr->radioResourceConfigCommon.prach_Config.prach_ConfigInfo.highSpeedFlag);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_cphy_prach_cfg_mobility

===========================================================================*/
/*!
    @brief
    This function tests the CPHY PRACH config during HO processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_cphy_prach_cfg_mobility
(
  lte_rrc_osys_PRACH_Config* ota_prach_cfg_ptr,
  lte_cphy_prach_cfg_common_param_s *prach_cfg_ptr
)
{
  /* Check the value of high speed flag */
  TF_ASSERT(prach_cfg_ptr->high_speed_flag 
            == ota_prach_cfg_ptr->prach_ConfigInfo.highSpeedFlag);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_tmr_and_const_param

===========================================================================*/
/*!
    @brief
    This function tests the UE Timers and Constants IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_tmr_and_const_param
(
  lte_rrc_osys_UE_TimersAndConstants *ota_tmr_and_const_param_ptr,
  lte_cphy_tmr_and_const_param_s *tmr_and_const_param_ptr
)
{
  uint32 n310, n311;

  TF_ASSERT(tmr_and_const_param_ptr->t310 == ota_tmr_and_const_param_ptr->t310);

  // Get the n310 from SIB2
  switch(ota_tmr_and_const_param_ptr->n310)
  {
    case lte_rrc_osys_UE_TimersAndConstants_n310_n1:
      n310 = LTE_CPHY_N310_1;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n310_n2:
      n310 = LTE_CPHY_N310_2;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n310_n3:
      n310 = LTE_CPHY_N310_3;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n310_n4:
      n310 = LTE_CPHY_N310_4;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n310_n6:
      n310 = LTE_CPHY_N310_6;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n310_n8:
      n310 = LTE_CPHY_N310_8;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n310_n10:
      n310 = LTE_CPHY_N310_10;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n310_n20:
      n310 = LTE_CPHY_N310_20;
      break;
    default:
      TF_ASSERT(0);
  }

  TF_ASSERT(tmr_and_const_param_ptr->n310 == n310);

  // Get the n310 from SIB2
  switch(ota_tmr_and_const_param_ptr->n311)
  {
    case lte_rrc_osys_UE_TimersAndConstants_n311_n1:
      n311 = LTE_CPHY_N311_1;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n311_n2:
      n311 = LTE_CPHY_N311_2;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n311_n3:
      n311 = LTE_CPHY_N311_3;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n311_n4:
      n311 = LTE_CPHY_N311_4;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n311_n5:
      n311 = LTE_CPHY_N311_5;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n311_n6:
      n311 = LTE_CPHY_N311_6;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n311_n8:
      n311 = LTE_CPHY_N311_8;
      break;
    case lte_rrc_osys_UE_TimersAndConstants_n311_n10:
      n311 = LTE_CPHY_N311_10;
      break;
    default:
      TF_ASSERT(0);
  }

  TF_ASSERT(tmr_and_const_param_ptr->n311 == n311);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_tdd_cfg

===========================================================================*/
/*!
    @brief
    This function tests the TDD Config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_tdd_cfg
(
  lte_rrc_osys_TDD_Config *ota_tdd_cfg_ptr,
  lte_cphy_tdd_cfg_param_s *tdd_cfg_ptr,
  lte_rrc_osys_TDD_Config_v1130 *ota_tdd_config_v1130
)
{
  TF_ASSERT(tdd_cfg_ptr->subframe_assign == ota_tdd_cfg_ptr->subframeAssignment);
  if (ota_tdd_config_v1130 != NULL) 
  {
     TF_ASSERT(tdd_cfg_ptr->special_sub_fn_patterns == 
               LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_9);
     TF_ASSERT(ota_tdd_config_v1130->specialSubframePatterns_v1130 == 
               lte_rrc_osys_TDD_Config_v1130_specialSubframePatterns_v1130_ssp9);
  }
  else
  {
     TF_ASSERT(tdd_cfg_ptr->special_sub_fn_patterns == 
               ota_tdd_cfg_ptr->specialSubframePatterns);
  }

  // Store the TDD UL\DL config for verifying if this is passed to MAC correctly
  lte_rrc_llc_test_common_data.tdd_param.tdd_ul_dl_cfg = tdd_cfg_ptr->subframe_assign;
} /*lte_rrc_llc_test_tdd_cfg*/

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_cell_select_cfg

===========================================================================*/
/*!
    @brief
    This function tests the Cell Selection IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_cell_select_cfg
(
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr,
  lte_cphy_cell_select_cfg_s *cell_select_cfg_ptr
)
{
  TF_ASSERT(cell_select_cfg_ptr->q_rxlevmin == (2 * sib1_ptr->cellSelectionInfo.q_RxLevMin));
  TF_ASSERT(cell_select_cfg_ptr->q_rxlevmin_offset == LTE_RRC_LLC_DEFAULT_Q_RXLEVMIN_OFFSET);

  if (sib1_ptr->m.p_MaxPresent != 0)
  {
    TF_ASSERT(cell_select_cfg_ptr->p_max == sib1_ptr->p_Max);
  }
  else
  {
    TF_ASSERT(cell_select_cfg_ptr->p_max == LTE_DEFAULT_PMAX);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_phy_common_cfg

===========================================================================*/
/*!
    @brief
    This function tests the PHY common config IEs processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_phy_common_cfg
(
  lte_rrc_osys_SystemInformationBlockType2 *sib2_ptr,
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr
)
{
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr = NULL; 
  lte_cphy_common_cfg_mask_t cphy_common_cfg_req_bitmask = 0;

  if (lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr)
  {
    sib1_ptr = lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.systemInformationBlockType1;
  }

  /* This will be true for */
  if (sib2_ptr == NULL)
  {
    sib2_ptr = lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib2;
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG)
  {
    /****** PDSCH common cfg *******/
    TF_ASSERT((cphy_common_cfg_req_ptr->common_cfg_param.
               cfg_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG) != 0);
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG;
    lte_rrc_llc_test_pdsch_cfg_common(&sib2_ptr->radioResourceConfigCommon.pdsch_ConfigCommon,
                                      &cphy_common_cfg_req_ptr->common_cfg_param.pdsch_cfg_common);
    TF_MSG("\n PDSCH config common verified successfully \n");
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG)
  {
    /****** PUSCH common cfg *******/
    TF_ASSERT((cphy_common_cfg_req_ptr->common_cfg_param.
               cfg_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG) != 0);
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG;
    lte_rrc_llc_test_pusch_cfg_common(&sib2_ptr->radioResourceConfigCommon.pusch_ConfigCommon,
                                      &cphy_common_cfg_req_ptr->common_cfg_param.pusch_cfg_common);
    TF_MSG("\n PUSCH config common verified successfully \n");
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG)
  {
    /****** PUCCH common cfg *******/
    TF_ASSERT((cphy_common_cfg_req_ptr->common_cfg_param.
               cfg_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG) != 0);
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG;
    lte_rrc_llc_test_pucch_cfg_common(&sib2_ptr->radioResourceConfigCommon.pucch_ConfigCommon,
                                      &cphy_common_cfg_req_ptr->common_cfg_param.pucch_cfg_common);
    TF_MSG("\n PUCCH config common verified successfully \n");
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG)
  {
    /****** SRS common cfg *******/
    TF_ASSERT((cphy_common_cfg_req_ptr->common_cfg_param.
               cfg_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG) != 0);
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG;
    lte_rrc_llc_test_srs_cfg_common(&sib2_ptr->radioResourceConfigCommon.soundingRS_UL_ConfigCommon,
                                    &cphy_common_cfg_req_ptr->common_cfg_param.srs_cfg_common,
                                    lte_rrc_llc_test_common_data.tdd_param.tdd_cfg);
    TF_MSG("\n SRS config common verified successfully \n");
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL)
  {
    /****** UL Power Control *******/
    TF_ASSERT((cphy_common_cfg_req_ptr->common_cfg_param.
               cfg_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL) != 0);
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL;
    lte_rrc_llc_test_ul_pwr_ctrl_cfg_common(&sib2_ptr->radioResourceConfigCommon.uplinkPowerControlCommon,
                                            &cphy_common_cfg_req_ptr->common_cfg_param.ul_power_ctrl_common);
    TF_MSG("\n UL Power control config common verified successfully \n");
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH)
  {
    /****** UL Cyclic Prefix Length *******/
    TF_ASSERT((cphy_common_cfg_req_ptr->common_cfg_param.
               cfg_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH) != 0);
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH;
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.ul_cyclic_prefix_length 
              == (lte_cphy_ul_cyclic_prefix_length_e) sib2_ptr->radioResourceConfigCommon.
                  ul_CyclicPrefixLength);
    TF_MSG("\n UL Cyclic prefix length verified successfully \n");
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG)
  {
    /****** UL freq Information *******/
    TF_ASSERT((cphy_common_cfg_req_ptr->common_cfg_param.
               cfg_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG) != 0);
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG;
    lte_rrc_llc_test_ul_freq_info(sib2_ptr, 
                                  &cphy_common_cfg_req_ptr->common_cfg_param.ul_freq_cfg);
    TF_MSG("\n UL freq info verified successfully \n");
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG)
  {
    /****** MBSFN Config ******/
    TF_ASSERT((cphy_common_cfg_req_ptr->common_cfg_param.
               cfg_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG) != 0);
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG;
    lte_rrc_llc_test_mbsfn_cfg(sib2_ptr, &cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_cfg);
    TF_MSG("\n MBSFN config verified successfully \n");
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM)
  {
    /****** UE Timers and Constants *******/
    TF_ASSERT((cphy_common_cfg_req_ptr->common_cfg_param.
               cfg_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM) != 0);
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM;
    lte_rrc_llc_test_tmr_and_const_param(&sib2_ptr->ue_TimersAndConstants, 
                                         &cphy_common_cfg_req_ptr->common_cfg_param.
                                         tmr_and_const_param);
    TF_MSG("\n Timers and constants verified successfully \n");
  }

  if (sib1_ptr)
  {
    if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG)
    {
      TF_ASSERT((cphy_common_cfg_req_ptr->common_cfg_param.
                 cfg_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG) != 0);
      lte_rrc_llc_test_cell_select_cfg(sib1_ptr, 
                                       &cphy_common_cfg_req_ptr->common_cfg_param.cell_selection_cfg);
      TF_MSG("\n Cell select verified successfully \n");
    }
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG)
  {
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG;
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG)
  {
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG;
  }

  if (cphy_common_cfg_verif_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_PRACH_CFG)
  {
    /****** MBSFN Config ******/
    TF_ASSERT((cphy_common_cfg_req_ptr->common_cfg_param.
               cfg_mask & LTE_CPHY_COMMON_CFG_MASK_BIT_PRACH_CFG) != 0);
    cphy_common_cfg_req_bitmask |= LTE_CPHY_COMMON_CFG_MASK_BIT_PRACH_CFG;
    lte_rrc_llc_test_cphy_prach_cfg(sib2_ptr, &cphy_common_cfg_req_ptr->common_cfg_param.prach_cfg_common);
    TF_MSG("\n CPHY PRACH config verified successfully \n");
  }

  /* Validate bitmasks of all fields that are part of the common phy config */
  TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.cfg_mask == cphy_common_cfg_req_bitmask);
  TF_MSG("\n Phy common config verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_phich_cfg_common

===========================================================================*/
/*!
    @brief
    This function tests the PHICH Config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_phich_cfg_common
(
  lte_rrc_osys_PHICH_Config *ota_phich_cfg_ptr,
  lte_l1_phich_cfg_param_s *phich_cfg_ptr
)
{
  TF_ASSERT(phich_cfg_ptr->phich_duration == ota_phich_cfg_ptr->phich_Duration);
  TF_ASSERT(phich_cfg_ptr->phich_resource == ota_phich_cfg_ptr->phich_Resource);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_antenna_info_common

===========================================================================*/
/*!
    @brief
    This function tests the Antenna info common IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_antenna_info_common
(
  lte_rrc_osys_AntennaInfoCommon *ota_antenna_info_ptr,
  lte_cphy_antenna_cfg_common_param_s *antenna_info_ptr
)
{
  lte_l1_antenna_info_ports_count_e antenna_port_cnt;
  switch(ota_antenna_info_ptr->antennaPortsCount)
  {
    case lte_rrc_osys_AntennaInfoCommon_antennaPortsCount_an1:
      antenna_port_cnt = LTE_L1_ANTENNA_INFO_PORTS_COUNT_1;
      break;
    case lte_rrc_osys_AntennaInfoCommon_antennaPortsCount_an2:
      antenna_port_cnt = LTE_L1_ANTENNA_INFO_PORTS_COUNT_2;
      break;
    case lte_rrc_osys_AntennaInfoCommon_antennaPortsCount_an4:
      antenna_port_cnt = LTE_L1_ANTENNA_INFO_PORTS_COUNT_4;
      break;
    default:
      TF_ASSERT(FALSE);
  }
  TF_ASSERT(antenna_info_ptr->antenna_ports_count == antenna_port_cnt);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_phy_common_mobility_cfg

===========================================================================*/
/*!
    @brief
    This function tests the PHY common mobility config IEs processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_phy_common_mobility_cfg
(
  lte_rrc_osys_MobilityControlInfo *ota_mobility_info_ptr,
  lte_cphy_handover_req_s *cphy_handover_cfg_ptr
)
{
  lte_rrc_osys_RadioResourceConfigCommon *ota_radio_resrc_common_cfg_ptr = 
    &ota_mobility_info_ptr->radioResourceConfigCommon;
  lte_cphy_common_mobility_cfg_mask_t cphy_common_mobility_cfg_bitmask = 0;
  lte_cphy_common_mobility_req_param_s *cphy_common_mobility_cfg_param_ptr = 
    &cphy_handover_cfg_ptr->common_mobility_cfg_param;

  if (ota_radio_resrc_common_cfg_ptr->m.pdsch_ConfigCommonPresent)
  {
    /****** PDSCH common cfg *******/
    TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
               LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PDSCH_CFG) != 0);
    cphy_common_mobility_cfg_bitmask |= LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PDSCH_CFG;
    lte_rrc_llc_test_pdsch_cfg_common(&ota_radio_resrc_common_cfg_ptr->pdsch_ConfigCommon,
                                      &cphy_common_mobility_cfg_param_ptr->pdsch_cfg_common);
  }

  /****** PUSCH common cfg *******/
  TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
             LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PUSCH_CFG) != 0);
  cphy_common_mobility_cfg_bitmask |= LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PUSCH_CFG;
  lte_rrc_llc_test_pusch_cfg_common(&ota_radio_resrc_common_cfg_ptr->pusch_ConfigCommon,
                                    &cphy_common_mobility_cfg_param_ptr->pusch_cfg_common);

  /****** PHICH common cfg *******/
  if (ota_radio_resrc_common_cfg_ptr->m.phich_ConfigPresent)
  {
    TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
               LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PHICH_CFG) != 0);
    cphy_common_mobility_cfg_bitmask |= LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PHICH_CFG;
    lte_rrc_llc_test_phich_cfg_common(&ota_radio_resrc_common_cfg_ptr->phich_Config,
                                      &cphy_common_mobility_cfg_param_ptr->phich_cfg);
  }

  /****** PUCCH common cfg *******/
  if (ota_radio_resrc_common_cfg_ptr->m.pucch_ConfigCommonPresent)
  {
    TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
               LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PUCCH_CFG) != 0);
    cphy_common_mobility_cfg_bitmask |= LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PUCCH_CFG;
    lte_rrc_llc_test_pucch_cfg_common(&ota_radio_resrc_common_cfg_ptr->pucch_ConfigCommon,
                                      &cphy_common_mobility_cfg_param_ptr->pucch_cfg_common);
  }

  /****** SRS common cfg *******/
  if (ota_radio_resrc_common_cfg_ptr->m.soundingRS_UL_ConfigCommonPresent)
  {
    TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
               LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_SRS_CFG) != 0);
    cphy_common_mobility_cfg_bitmask |= LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_SRS_CFG;
    lte_rrc_llc_test_srs_cfg_common(&ota_radio_resrc_common_cfg_ptr->soundingRS_UL_ConfigCommon,
                                    &cphy_common_mobility_cfg_param_ptr->srs_cfg_common,
                                    lte_rrc_llc_test_common_data.tdd_param.tdd_cfg);
  }
  /* For TDD to FDD HO, if srx_max_up_pts was configured earlier, then it will be disabled */
  else if ((lte_rrc_llc_test_common_data.tdd_param.tdd_cfg == FALSE) && 
           (lte_rrc_llc_test_common_data.tdd_param.srx_max_up_pts_configured))
  {
    TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
               LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_SRS_CFG) != 0);
    cphy_common_mobility_cfg_bitmask |= LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_SRS_CFG;
    TF_ASSERT(cphy_common_mobility_cfg_param_ptr->srs_cfg_common.srx_max_up_pts == FALSE);
    lte_rrc_llc_test_common_data.tdd_param.srx_max_up_pts_configured = FALSE;
  }

  /****** UL Power Control *******/
  if (ota_radio_resrc_common_cfg_ptr->m.uplinkPowerControlCommonPresent)
  {
    TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
               LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_POWER_CTRL) != 0);
    cphy_common_mobility_cfg_bitmask |= LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_POWER_CTRL;
    lte_rrc_llc_test_ul_pwr_ctrl_cfg_common(&ota_radio_resrc_common_cfg_ptr->uplinkPowerControlCommon,
                                            &cphy_common_mobility_cfg_param_ptr->ul_power_ctrl_common);
  }

  /****** Antenna common cfg *******/
  if (ota_radio_resrc_common_cfg_ptr->m.antennaInfoCommonPresent)
  {
    TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
               LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_ANTENNA_CFG) != 0);
    cphy_common_mobility_cfg_bitmask |= LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_ANTENNA_CFG;
    lte_rrc_llc_test_antenna_info_common(&ota_radio_resrc_common_cfg_ptr->antennaInfoCommon,
                                         &cphy_common_mobility_cfg_param_ptr->antenna_cfg_common);
  }

  /****** UL Cyclic Prefix Length *******/
  TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
             LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH) != 0);
  cphy_common_mobility_cfg_bitmask |= LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH;
  TF_ASSERT(cphy_common_mobility_cfg_param_ptr->ul_cyclic_prefix_length 
            == (lte_cphy_ul_cyclic_prefix_length_e) ota_radio_resrc_common_cfg_ptr->ul_CyclicPrefixLength);

  /****** TDD Config *******/
  if ((ota_radio_resrc_common_cfg_ptr->m.tdd_ConfigPresent) &&
      (lte_rrc_llc_test_common_data.tdd_param.tdd_cfg))
  {
    TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
               LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_TDD_CFG) != 0);
    cphy_common_mobility_cfg_bitmask |= LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_TDD_CFG;
    if (ota_radio_resrc_common_cfg_ptr->m.tdd_Config_v1130Present) 
    {
       lte_rrc_llc_test_tdd_cfg(&ota_radio_resrc_common_cfg_ptr->tdd_Config, 
                             &cphy_common_mobility_cfg_param_ptr->tdd_cfg,
                             &ota_radio_resrc_common_cfg_ptr->tdd_Config_v1130);
    }
    else
    {
    lte_rrc_llc_test_tdd_cfg(&ota_radio_resrc_common_cfg_ptr->tdd_Config, 
                                &cphy_common_mobility_cfg_param_ptr->tdd_cfg, NULL);
    }
  }

  /****** P_max config *******/
  if (ota_radio_resrc_common_cfg_ptr->m.p_MaxPresent)
  {
    TF_ASSERT(cphy_common_mobility_cfg_param_ptr->p_max == ota_radio_resrc_common_cfg_ptr->p_Max);
  }
  else
  {
    TF_ASSERT(cphy_common_mobility_cfg_param_ptr->p_max == LTE_DEFAULT_PMAX);
  }

  if (ota_radio_resrc_common_cfg_ptr->prach_Config.m.prach_ConfigInfoPresent)
  {
    /****** Prach Config ******/
    TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
               LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PRACH_CFG) != 0);
    cphy_common_mobility_cfg_bitmask |= LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PRACH_CFG;
    lte_rrc_llc_test_cphy_prach_cfg_mobility(&ota_radio_resrc_common_cfg_ptr->prach_Config, 
                                             &cphy_common_mobility_cfg_param_ptr->prach_cfg_common);
    TF_MSG("\n CPHY PRACH config verified successfully \n");
  }

  /* UL Freq and p_max should always be present */
  TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
             LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_FREQ_CFG) != 0);
  TF_ASSERT((cphy_common_mobility_cfg_param_ptr->cfg_mask & 
             LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_P_MAX) != 0);

  cphy_common_mobility_cfg_bitmask |= (LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_FREQ_CFG |
                                       LTE_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_P_MAX);
  
  /* Validate bitmasks of all fields that are part of the common mobility phy config */
  TF_ASSERT(cphy_common_mobility_cfg_param_ptr->cfg_mask == cphy_common_mobility_cfg_bitmask);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_phy_mobility_cfg

===========================================================================*/
/*!
    @brief
    This function tests the PHY mobility config IEs processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_phy_mobility_cfg
(
  lte_cphy_handover_req_s *cphy_handover_cfg_ptr
)
{
  lte_rrc_osys_MobilityControlInfo *ota_mobility_info_ptr = &lte_rrc_llc_test_common_data.
    dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
    rrcConnectionReconfiguration_r8->mobilityControlInfo;
  lte_bandwidth_e bandwidth = LTE_BW_MIN;
  lte_earfcn_t ul_freq;

  // Determine if target cell is TDD or FDD
  if ((ota_mobility_info_ptr->m.carrierFreqPresent) && 
      (ota_mobility_info_ptr->carrierFreq.dl_CarrierFreq >= 36000) &&
      (ota_mobility_info_ptr->carrierFreq.dl_CarrierFreq <= 39649))
  {
    lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;
  }
  else if ((ota_mobility_info_ptr->m.carrierFreqPresent) && 
           (ota_mobility_info_ptr->carrierFreq.dl_CarrierFreq < 36000))
  {
    lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = FALSE;
  }

  /***** validate source and target cell information *****/
  if((((((*(((*((((*(lte_rrc_llc_test_common_data.
  	dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration)).criticalExtensions).u)
  	.c1)).u).rrcConnectionReconfiguration_r8)).radioResourceConfigDedicated).physicalConfigDedicated).m).antennaInfoPresent) &&
  	(((((*(((*((((*(lte_rrc_llc_test_common_data.           
  	dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration)).criticalExtensions).u)     
  	.c1)).u).rrcConnectionReconfiguration_r8)).radioResourceConfigDedicated).physicalConfigDedicated).antennaInfo).t==1)&&
  	((*((((((*(((*((((*(lte_rrc_llc_test_common_data.      
  	dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration)).criticalExtensions).u)
  	.c1)).u).rrcConnectionReconfiguration_r8)).radioResourceConfigDedicated).physicalConfigDedicated).
  	antennaInfo).u).explicitValue)).transmissionMode == 
  	lte_rrc_osys_AntennaInfoDedicated_r10_transmissionMode_r10_tm8_v920))
  {
	TF_ASSERT(cphy_handover_cfg_ptr->source_cell_info.phy_cell_id == 503);
	TF_ASSERT(cphy_handover_cfg_ptr->source_cell_info.dl_cell_frequency == 500);

  }
  else if (lte_rrc_llc_test_common_data.tdd_param.tdd_cfg)
  {
    TF_ASSERT(cphy_handover_cfg_ptr->source_cell_info.phy_cell_id == 502);
    TF_ASSERT(cphy_handover_cfg_ptr->source_cell_info.dl_cell_frequency == 36000);
  }
  else
  {
    TF_ASSERT(cphy_handover_cfg_ptr->source_cell_info.phy_cell_id == 503);
    TF_ASSERT(cphy_handover_cfg_ptr->source_cell_info.dl_cell_frequency == 500);
  }

  TF_ASSERT(cphy_handover_cfg_ptr->ho_target_cell_info.
            target_cell_id == ota_mobility_info_ptr->targetPhysCellId);

  if (ota_mobility_info_ptr->m.carrierFreqPresent)
  {
    TF_ASSERT(cphy_handover_cfg_ptr->ho_target_cell_info.
              dl_cell_frequency == ota_mobility_info_ptr->carrierFreq.dl_CarrierFreq);
    if (ota_mobility_info_ptr->carrierFreq.m.ul_CarrierFreqPresent)
    {
      TF_ASSERT(cphy_handover_cfg_ptr->common_mobility_cfg_param.ul_freq_cfg.
                ul_cell_frequency == ota_mobility_info_ptr->carrierFreq.ul_CarrierFreq);
    }
    else
    {
      /* UL freq is same as the source cell */
      TF_ASSERT(E_SUCCESS == lte_rrc_llc_update_ul_earfcn(cphy_handover_cfg_ptr->ho_target_cell_info.
                                                          dl_cell_frequency,
                                                          &ul_freq));
      TF_ASSERT(cphy_handover_cfg_ptr->common_mobility_cfg_param.ul_freq_cfg.
                ul_cell_frequency == ul_freq);
    }
  }
  else
  {
    if (lte_rrc_llc_test_common_data.tdd_param.tdd_cfg)
    {
      /* Same as the default DL frequency set in the stub function 
        lte_rrc_llc_test_common_csp_get_serving_cell_params */
      TF_ASSERT(cphy_handover_cfg_ptr->ho_target_cell_info.dl_cell_frequency == 36000);
      /* UL freq is same as DL freq */
      TF_ASSERT(cphy_handover_cfg_ptr->common_mobility_cfg_param.ul_freq_cfg.ul_cell_frequency == 36000);
    }
    else
    {
      /* Same as the default DL frequency set in the stub function 
        lte_rrc_llc_test_common_csp_get_serving_cell_params */
      TF_ASSERT(cphy_handover_cfg_ptr->ho_target_cell_info.dl_cell_frequency == 500);
      /* UL freq of target cell is same as UL freq of the source cell */
      TF_ASSERT(cphy_handover_cfg_ptr->common_mobility_cfg_param.ul_freq_cfg.ul_cell_frequency == 18500);
    }
  }

  if (ota_mobility_info_ptr->m.carrierBandwidthPresent)
  {
    switch(ota_mobility_info_ptr->carrierBandwidth.dl_Bandwidth)
    {
      case lte_rrc_osys_CarrierBandwidthEUTRA_dl_Bandwidth_n6:
        bandwidth = LTE_BW_NRB_6;
        break;
      case lte_rrc_osys_CarrierBandwidthEUTRA_dl_Bandwidth_n15:
        bandwidth = LTE_BW_NRB_15;
        break;
      case lte_rrc_osys_CarrierBandwidthEUTRA_dl_Bandwidth_n25:
        bandwidth = LTE_BW_NRB_25;
        break;
      case lte_rrc_osys_CarrierBandwidthEUTRA_dl_Bandwidth_n50:
        bandwidth = LTE_BW_NRB_50;
        break;
      case lte_rrc_osys_CarrierBandwidthEUTRA_dl_Bandwidth_n75:
        bandwidth = LTE_BW_NRB_75;
        break;
      case lte_rrc_osys_CarrierBandwidthEUTRA_dl_Bandwidth_n100:
        bandwidth = LTE_BW_NRB_100;
        break;
      default:
        /* Same as DL bandwidth set in the stub function lte_rrc_llc_test_csp_get_camped_cell_bw */
        TF_ASSERT(cphy_handover_cfg_ptr->ho_target_cell_info.dl_bandwidth == LTE_BW_NRB_15);
    }
    TF_ASSERT(cphy_handover_cfg_ptr->ho_target_cell_info.dl_bandwidth == bandwidth);

    if (ota_mobility_info_ptr->carrierBandwidth.m.ul_BandwidthPresent)
    {
      switch(ota_mobility_info_ptr->carrierBandwidth.ul_Bandwidth)
      {
        case lte_rrc_osys_CarrierBandwidthEUTRA_ul_Bandwidth_n6:
          bandwidth = LTE_BW_NRB_6;
          break;
        case lte_rrc_osys_CarrierBandwidthEUTRA_ul_Bandwidth_n15:
          bandwidth = LTE_BW_NRB_15;
          break;
        case lte_rrc_osys_CarrierBandwidthEUTRA_ul_Bandwidth_n25:
          bandwidth = LTE_BW_NRB_25;
          break;
        case lte_rrc_osys_CarrierBandwidthEUTRA_ul_Bandwidth_n50:
          bandwidth = LTE_BW_NRB_50;
          break;
        case lte_rrc_osys_CarrierBandwidthEUTRA_ul_Bandwidth_n75:
          bandwidth = LTE_BW_NRB_75;
          break;
        case lte_rrc_osys_CarrierBandwidthEUTRA_ul_Bandwidth_n100:
          bandwidth = LTE_BW_NRB_100;
          break;
        default:
          /* Same as DL bandwidth set in the stub function lte_rrc_llc_test_csp_get_camped_cell_bw */
          TF_ASSERT(cphy_handover_cfg_ptr->common_mobility_cfg_param.ul_freq_cfg.ul_bandwidth == LTE_BW_NRB_15);
      }
      TF_ASSERT(cphy_handover_cfg_ptr->common_mobility_cfg_param.ul_freq_cfg.ul_bandwidth == bandwidth);
    }
    else
    {
      /* Same as DL bandwidth set in the stub function lte_rrc_llc_test_csp_get_camped_cell_bw */
      TF_ASSERT(cphy_handover_cfg_ptr->common_mobility_cfg_param.
                ul_freq_cfg.ul_bandwidth == cphy_handover_cfg_ptr->ho_target_cell_info.dl_bandwidth);
    }
  }
  else
  {
    /* Same as DL bandwidth set in the stub function lte_rrc_llc_test_csp_get_camped_cell_bw */
    TF_ASSERT(cphy_handover_cfg_ptr->ho_target_cell_info.dl_bandwidth == LTE_BW_NRB_100);
    TF_ASSERT(cphy_handover_cfg_ptr->common_mobility_cfg_param.ul_freq_cfg.ul_bandwidth == LTE_BW_NRB_100);
  }


  TF_ASSERT(cphy_handover_cfg_ptr->common_mobility_cfg_param.ul_freq_cfg.
            add_spectrum_emission_present == TRUE);

  if (ota_mobility_info_ptr->m.additionalSpectrumEmissionPresent)
  {
    TF_ASSERT(cphy_handover_cfg_ptr->common_mobility_cfg_param.ul_freq_cfg.
              add_spectrum_emission == ota_mobility_info_ptr->additionalSpectrumEmission);
  }

  /* Verify RadioResourceConfigCommon parameters */
  lte_rrc_llc_test_phy_common_mobility_cfg(ota_mobility_info_ptr,
                                           cphy_handover_cfg_ptr);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pdsch_cfg_dedicated

===========================================================================*/
/*!
    @brief
    This function tests the PDSCH dedicated config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_pdsch_cfg_dedicated
(
  lte_rrc_osys_PDSCH_ConfigDedicated* ota_pdsch_cfg_ded_ptr,
  lte_cphy_pdsch_cfg_dedicated_param_s *pdsch_cfg_ded_ptr
)
{
  TF_ASSERT(pdsch_cfg_ded_ptr->pa == ota_pdsch_cfg_ded_ptr->p_a); 
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pucch_cfg_dedicated

===========================================================================*/
/*!
    @brief
    This function tests the PUCCH dedicated config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_pucch_cfg_dedicated
(
  lte_rrc_osys_PUCCH_ConfigDedicated* ota_pucch_cfg_ded_ptr,
  lte_cphy_pucch_cfg_dedicated_param_s *pucch_cfg_ded_ptr
)
{
  TF_ASSERT(pucch_cfg_ded_ptr->enable == TRUE);
  if (ota_pucch_cfg_ded_ptr->ackNackRepetition.
      t == T_lte_rrc_osys_PUCCH_ConfigDedicated_ackNackRepetition_release)
  {
    TF_ASSERT(pucch_cfg_ded_ptr->ack_nak_repetition_enable == FALSE);
  }
  else
  {
    TF_ASSERT(pucch_cfg_ded_ptr->ack_nak_repetition_enable == TRUE);
    TF_ASSERT(pucch_cfg_ded_ptr->ack_nak_repetition_factor 
              == ota_pucch_cfg_ded_ptr->ackNackRepetition.u.setup->repetitionFactor);
    TF_ASSERT(pucch_cfg_ded_ptr->n1_pucch_an_rep == ota_pucch_cfg_ded_ptr->ackNackRepetition.
                                                     u.setup->n1PUCCH_AN_Rep);
  }

  if (lte_rrc_llc_test_common_data.tdd_param.tdd_cfg)
  {
    if (pucch_cfg_ded_ptr->tdd_ack_nak_feedback_mode_enable == TRUE)
    {
      if (ota_pucch_cfg_ded_ptr->tdd_AckNackFeedbackMode 
          == lte_rrc_osys_PUCCH_ConfigDedicated_tdd_AckNackFeedbackMode_bundling)
      {
        TF_ASSERT(pucch_cfg_ded_ptr->tdd_ack_nak_feedback_mode 
                  == LTE_L1_TDD_ACK_NAK_FEEDBACK_MODE_BUNDLING);
      }
      else
      {
        TF_ASSERT(pucch_cfg_ded_ptr->tdd_ack_nak_feedback_mode 
                  == LTE_L1_TDD_ACK_NAK_FEEDBACK_MODE_MULTIPLEXING);
      }
      lte_rrc_llc_test_common_data.tdd_param.tdd_ack_nak_feedback_mode_enabled = TRUE;
    }
  }
  else
  {
    TF_ASSERT(pucch_cfg_ded_ptr->tdd_ack_nak_feedback_mode_enable == FALSE);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pusch_cfg_dedicated

===========================================================================*/
/*!
    @brief
    This function tests the PUSCH dedicated config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_pusch_cfg_dedicated
(
  lte_rrc_osys_PUSCH_ConfigDedicated* ota_pusch_cfg_ded_ptr,
  lte_cphy_pusch_cfg_dedicated_param_s *pusch_cfg_ded_ptr
)
{
  TF_ASSERT(pusch_cfg_ded_ptr->delta_offset_ack_index == ota_pusch_cfg_ded_ptr->betaOffset_ACK_Index);
  TF_ASSERT(pusch_cfg_ded_ptr->delta_offset_cqi_index == ota_pusch_cfg_ded_ptr->betaOffset_CQI_Index);
  TF_ASSERT(pusch_cfg_ded_ptr->delta_offset_ri_index == ota_pusch_cfg_ded_ptr->betaOffset_RI_Index);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_srs_cfg_dedicated

===========================================================================*/
/*!
    @brief
    This function tests the SRS dedicated config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_srs_cfg_dedicated
(
  lte_rrc_osys_SoundingRS_UL_ConfigDedicated* ota_srs_cfg_ded_ptr,
  lte_cphy_srs_cfg_dedicated_param_s *srs_cfg_ded_ptr
)
{
  if (ota_srs_cfg_ded_ptr->t == T_lte_rrc_osys_SoundingRS_UL_ConfigDedicated_release)
  {
    TF_ASSERT(srs_cfg_ded_ptr->srs_periodic_cfg_ded.enable == FALSE);
  }
  else
  {
    TF_ASSERT(srs_cfg_ded_ptr->srs_periodic_cfg_ded.enable == TRUE);
    TF_ASSERT(srs_cfg_ded_ptr->srs_periodic_cfg_ded.cyclic_shift == ota_srs_cfg_ded_ptr->u.setup->cyclicShift);
    TF_ASSERT(srs_cfg_ded_ptr->srs_periodic_cfg_ded.duration == ota_srs_cfg_ded_ptr->u.setup->duration);
    TF_ASSERT(srs_cfg_ded_ptr->srs_periodic_cfg_ded.freq_domain_pos == ota_srs_cfg_ded_ptr->u.setup->freqDomainPosition);
    TF_ASSERT(srs_cfg_ded_ptr->srs_periodic_cfg_ded.srs_cfg_index == ota_srs_cfg_ded_ptr->u.setup->srs_ConfigIndex);
    TF_ASSERT(srs_cfg_ded_ptr->srs_periodic_cfg_ded.srs_bw == ota_srs_cfg_ded_ptr->u.setup->srs_Bandwidth);  
    TF_ASSERT(srs_cfg_ded_ptr->srs_periodic_cfg_ded.srs_hbw == ota_srs_cfg_ded_ptr->u.setup->srs_HoppingBandwidth);
    TF_ASSERT(srs_cfg_ded_ptr->srs_periodic_cfg_ded.tx_comb == ota_srs_cfg_ded_ptr->u.setup->transmissionComb);
  }
}


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ul_pwr_ctrl_cfg_dedicated

===========================================================================*/
/*!
    @brief
    This function tests the UL Power Ctrl dedicated config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_ul_pwr_ctrl_cfg_dedicated
(
  lte_rrc_osys_UplinkPowerControlDedicated* ota_ul_pwr_ctrl_cfg_ded_ptr,
  lte_cphy_ul_power_ctrl_dedicated_param_s *ul_pwr_ctrl_cfg_ded_ptr
)
{
  TF_ASSERT(ul_pwr_ctrl_cfg_ded_ptr->accum_enabled == ota_ul_pwr_ctrl_cfg_ded_ptr->accumulationEnabled);
  if (ota_ul_pwr_ctrl_cfg_ded_ptr->deltaMCS_Enabled 
      == lte_rrc_osys_UplinkPowerControlDedicated_deltaMCS_Enabled_en0)
  {
    TF_ASSERT(ul_pwr_ctrl_cfg_ded_ptr->delta_mcs_enabled == LTE_L1_UL_POWER_CTRL_DELTA_MCS_ENABLE_0);
  }
  else
  {
    TF_ASSERT(ul_pwr_ctrl_cfg_ded_ptr->delta_mcs_enabled == LTE_L1_UL_POWER_CTRL_DELTA_MCS_ENABLE_1);
  }
  TF_ASSERT(ul_pwr_ctrl_cfg_ded_ptr->p0_ue_pucch == ota_ul_pwr_ctrl_cfg_ded_ptr->p0_UE_PUCCH);
  TF_ASSERT(ul_pwr_ctrl_cfg_ded_ptr->p0_ue_pusch == ota_ul_pwr_ctrl_cfg_ded_ptr->p0_UE_PUSCH);
  TF_ASSERT(ul_pwr_ctrl_cfg_ded_ptr->p_srs_offset == ota_ul_pwr_ctrl_cfg_ded_ptr->pSRS_Offset);
  TF_ASSERT(ul_pwr_ctrl_cfg_ded_ptr->filter_coefficient == ota_ul_pwr_ctrl_cfg_ded_ptr->filterCoefficient);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_tpc_pdcch_cfg

===========================================================================*/
/*!
    @brief
    This function tests the TPC PDCCH config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_tpc_pdcch_cfg
(
  lte_rrc_osys_TPC_PDCCH_Config* ota_tpc_pdcch_cfg_ptr,
  lte_cphy_tpc_pdcch_cfg_s *tpc_pdcch_cfg_ptr
)
{
  if (ota_tpc_pdcch_cfg_ptr->t == T_lte_rrc_osys_TPC_PDCCH_Config_setup)
  {
    TF_ASSERT(tpc_pdcch_cfg_ptr->enable == TRUE);
    TF_ASSERT(tpc_pdcch_cfg_ptr->tpc_rnti == (uint16) lte_rrc_osys_asn1_bitstring_to_int(
                                                        sizeof(tpc_pdcch_cfg_ptr->tpc_rnti),
                                                        &ota_tpc_pdcch_cfg_ptr->u.setup->tpc_RNTI));

    if (ota_tpc_pdcch_cfg_ptr->u.setup->tpc_Index.t == T_lte_rrc_osys_TPC_Index_indexOfFormat3)
    {
      TF_ASSERT(tpc_pdcch_cfg_ptr->tpc_index.dci_format == LTE_L1_TPC_DCI_FORMAT3);
      TF_ASSERT(tpc_pdcch_cfg_ptr->tpc_index.
                index_value == ota_tpc_pdcch_cfg_ptr->u.setup->tpc_Index.u.indexOfFormat3);
    }
    else
    {
      TF_ASSERT(tpc_pdcch_cfg_ptr->tpc_index.dci_format == LTE_L1_TPC_DCI_FORMAT3A);
      TF_ASSERT(tpc_pdcch_cfg_ptr->tpc_index.
                index_value == ota_tpc_pdcch_cfg_ptr->u.setup->tpc_Index.u.indexOfFormat3A);
    }
  }
  else
  {
    TF_ASSERT(tpc_pdcch_cfg_ptr->enable == FALSE);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rel9_cqi_cfg

===========================================================================*/
/*!
    @brief
    This function tests the CQI config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_rel9_cqi_cfg
(
  lte_rrc_osys_PhysicalConfigDedicated *phy_cfg_ded_ptr,
  lte_cphy_cqi_reporting_param_s *cqi_cfg_ptr
)
{
  lte_rrc_osys_CQI_ReportConfig_v920 *ota_rel9_cqi_cfg_ptr = &phy_cfg_ded_ptr->cqi_ReportConfig_v920;
  /* If CQI rel9 parameters were updated as part of the current configuration */
  boolean cqi_rel9_param_updated = FALSE;

  /* Verify CQI mask */
  /* If CQI mask is present and is set to 'setup', CQI mask should be enabled */
  if ((ota_rel9_cqi_cfg_ptr->m.cqi_Mask_r9Present) &&
      (ota_rel9_cqi_cfg_ptr->cqi_Mask_r9 == lte_rrc_osys_CQI_ReportConfig_v920_cqi_Mask_r9_setup))
  {
    TF_ASSERT(cqi_cfg_ptr->cqi_rel9_param.cqi_mask_enable == TRUE);
    // Store the CQI mask enabled flag
    lte_rrc_llc_test_common_data.rel9_param.cqi_mask_enabled = TRUE;
    cqi_rel9_param_updated = TRUE;
  }
  /* If CQI mask is previously enabled, CQI periodic reporting is set to 'setup' and CQI
    mask is not present in the current message, CQI mask should be released */
  else if ((lte_rrc_llc_test_common_data.rel9_param.cqi_mask_enabled == TRUE) &&
           (phy_cfg_ded_ptr->cqi_ReportConfig.cqi_ReportPeriodic.t 
            == T_lte_rrc_osys_CQI_ReportPeriodic_setup) &&
           (ota_rel9_cqi_cfg_ptr->m.cqi_Mask_r9Present == 0))
  {
    TF_ASSERT(cqi_cfg_ptr->cqi_rel9_param.cqi_mask_enable == FALSE);
    // Store the CQI mask enabled flag
    lte_rrc_llc_test_common_data.rel9_param.cqi_mask_enabled = FALSE;
    cqi_rel9_param_updated = TRUE;
  }
  /* If CQI mask is previously enabled and CQI periodic reporting is set to 'release',
    CQI mask should be released */
  else if ((lte_rrc_llc_test_common_data.rel9_param.cqi_mask_enabled == TRUE) &&
           (phy_cfg_ded_ptr->cqi_ReportConfig.cqi_ReportPeriodic.t 
            == T_lte_rrc_osys_CQI_ReportPeriodic_release))
  {
    TF_ASSERT(cqi_cfg_ptr->cqi_rel9_param.cqi_mask_enable == FALSE);
    // Store the CQI mask enabled flag
    lte_rrc_llc_test_common_data.rel9_param.cqi_mask_enabled = FALSE;
    cqi_rel9_param_updated = TRUE;
  }

  /* Verify PMI RI reporting */
  /* If PMI/RI mask is present and is set to 'setup', PMI/RI reporting should be enabled */
  if ((ota_rel9_cqi_cfg_ptr->m.pmi_RI_Report_r9Present) &&
      (ota_rel9_cqi_cfg_ptr->pmi_RI_Report_r9 == lte_rrc_osys_CQI_ReportConfig_v920_pmi_RI_Report_r9_setup))
  {
    TF_ASSERT(cqi_cfg_ptr->cqi_rel9_param.pmi_ri_report_configured == TRUE);
    // Store the PMI/RI reporting flag
    lte_rrc_llc_test_common_data.rel9_param.pmi_ri_reporting_configured = TRUE;
    cqi_rel9_param_updated = TRUE;
  }
  /* If CQI periodic reporting is set to 'setup' or CQI aperiodic reporting is present,
    while PMI/RI mask is not present, PMI/RI reporting should be disabled */
  else if ((lte_rrc_llc_test_common_data.rel9_param.pmi_ri_reporting_configured) &&
           ((phy_cfg_ded_ptr->cqi_ReportConfig.cqi_ReportPeriodic.t 
             == T_lte_rrc_osys_CQI_ReportPeriodic_setup) ||
            (phy_cfg_ded_ptr->cqi_ReportConfig.m.cqi_ReportModeAperiodicPresent)) &&
           (ota_rel9_cqi_cfg_ptr->m.pmi_RI_Report_r9Present == 0))
  {
    TF_ASSERT(cqi_cfg_ptr->cqi_rel9_param.pmi_ri_report_configured == FALSE);
    // Store the PMI/RI reporting flag
    lte_rrc_llc_test_common_data.rel9_param.pmi_ri_reporting_configured = FALSE;
    cqi_rel9_param_updated = TRUE;
  }
  /* If CQI periodic reporting is set to 'release' or CQI aperiodic reporting is absent,
    while PMI/RI reporting is configured previously, PMI/RI reporting should be disabled */
  else if ((lte_rrc_llc_test_common_data.rel9_param.pmi_ri_reporting_configured) &&
           ((phy_cfg_ded_ptr->cqi_ReportConfig.cqi_ReportPeriodic.t 
             == T_lte_rrc_osys_CQI_ReportPeriodic_release) ||
           (phy_cfg_ded_ptr->cqi_ReportConfig.m.cqi_ReportModeAperiodicPresent == 0)))
  {
    TF_ASSERT(cqi_cfg_ptr->cqi_rel9_param.pmi_ri_report_configured == FALSE);
    // Store the PMI/RI reporting flag
    lte_rrc_llc_test_common_data.rel9_param.pmi_ri_reporting_configured = FALSE;
    cqi_rel9_param_updated = TRUE;
  }

  if (cqi_rel9_param_updated == TRUE)
  {
    TF_ASSERT(cqi_cfg_ptr->cqi_rel9_param_present == TRUE);
  }
  else
  {
    TF_ASSERT(cqi_cfg_ptr->cqi_rel9_param_present == FALSE);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_cqi_cfg

===========================================================================*/
/*!
    @brief
    This function tests the CQI config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_cqi_cfg
(
  lte_rrc_osys_PhysicalConfigDedicated *phy_cfg_ded_ptr,
  lte_cphy_cqi_reporting_param_s *cqi_cfg_ptr
)
{
  lte_rrc_osys_CQI_ReportConfig* ota_cqi_cfg_ptr = &phy_cfg_ded_ptr->cqi_ReportConfig;

  if ((phy_cfg_ded_ptr->m.cqi_ReportConfig_v920Present) ||
      (lte_rrc_llc_test_common_data.cqi_srs_sr_info.cqi_reporting_configured == TRUE))
  {
    lte_rrc_llc_test_rel9_cqi_cfg(phy_cfg_ded_ptr, cqi_cfg_ptr);
  }

  TF_ASSERT(cqi_cfg_ptr->enable == TRUE);
  if (ota_cqi_cfg_ptr->m.cqi_ReportModeAperiodicPresent)
  {
    TF_ASSERT(cqi_cfg_ptr->cqi_reporting_mode_aperiodic_enable == TRUE);
    TF_ASSERT(cqi_cfg_ptr->cqi_reporting_mode_aperiodic == ota_cqi_cfg_ptr->cqi_ReportModeAperiodic);
  }
  else
  {
    TF_ASSERT(cqi_cfg_ptr->cqi_reporting_mode_aperiodic_enable == FALSE);
  }

  TF_ASSERT(cqi_cfg_ptr->nom_pdsch_rs_epre_offset == ota_cqi_cfg_ptr->nomPDSCH_RS_EPRE_Offset * 2);

  if (ota_cqi_cfg_ptr->m.cqi_ReportPeriodicPresent)
  {
    TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic_present == TRUE);
    if (ota_cqi_cfg_ptr->cqi_ReportPeriodic.t == T_lte_rrc_osys_CQI_ReportPeriodic_release)
    {
      TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.enable == FALSE);
    }
    else
    {
      TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.enable == TRUE);
      TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.
                cqi_pmi_cfg_index == ota_cqi_cfg_ptr->cqi_ReportPeriodic.u.setup->cqi_pmi_ConfigIndex);
      TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.
                pucch_resource_index == ota_cqi_cfg_ptr->cqi_ReportPeriodic.u.setup->cqi_PUCCH_ResourceIndex);
      if (ota_cqi_cfg_ptr->cqi_ReportPeriodic.u.setup->cqi_FormatIndicatorPeriodic.
          t == T_lte_rrc_osys_CQI_ReportPeriodic_setup_cqi_FormatIndicatorPeriodic_widebandCQI)
      {
        TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.
                  format_indicator == LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_WIDEBAND);
      }
      else
      {
        TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.
                  format_indicator == LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND);
        TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.subband_cqi_k 
                  == ota_cqi_cfg_ptr->cqi_ReportPeriodic.u.setup->cqi_FormatIndicatorPeriodic.u.subbandCQI->k);
      }

      if ((phy_cfg_ded_ptr->m.antennaInfoPresent) &&
          ((phy_cfg_ded_ptr->antennaInfo.t 
            == T_lte_rrc_osys_PhysicalConfigDedicated_antennaInfo_defaultValue) ||
           ((phy_cfg_ded_ptr->antennaInfo.u.explicitValue->transmissionMode 
             != lte_rrc_osys_AntennaInfoDedicated_transmissionMode_tm3) &&
            (phy_cfg_ded_ptr->antennaInfo.u.explicitValue->transmissionMode 
             != lte_rrc_osys_AntennaInfoDedicated_transmissionMode_tm4)&&
            (phy_cfg_ded_ptr->antennaInfo.u.explicitValue->transmissionMode 
             != lte_rrc_osys_AntennaInfoDedicated_transmissionMode_tm8_v920))))
      {
        TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.ri_cfg_index_enable == FALSE);
      }	  
      else if (ota_cqi_cfg_ptr->cqi_ReportPeriodic.u.setup->m.ri_ConfigIndexPresent)
      {
		if((phy_cfg_ded_ptr->antennaInfo.u.explicitValue->transmissionMode 
             == lte_rrc_osys_AntennaInfoDedicated_transmissionMode_tm8_v920) &&
			 !(((*phy_cfg_ded_ptr).cqi_ReportConfig_v920).m).pmi_RI_Report_r9Present)
		{
			TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.ri_cfg_index_enable == FALSE);
		}
		else
		{
        TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.ri_cfg_index_enable == TRUE);
        TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.
                  ri_cfg_index == ota_cqi_cfg_ptr->cqi_ReportPeriodic.u.setup->ri_ConfigIndex);
		}
      }
      else
      {
        TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.ri_cfg_index_enable == FALSE);
      }
      TF_ASSERT(cqi_cfg_ptr->cqi_reporting_periodic.
                sim_ack_nak_cqi == ota_cqi_cfg_ptr->cqi_ReportPeriodic.u.setup->simultaneousAckNackAndCQI);
    }
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_antenna_cfg_dedicated

===========================================================================*/
/*!
    @brief
    This function tests the antenna dedicated config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_antenna_cfg_dedicated
(
  lte_rrc_osys_PhysicalConfigDedicated *phy_cfg_ded_ptr,
  lte_cphy_antenna_cfg_dedicated_param_s *antenna_cfg_ded_ptr
)
{
  lte_rrc_osys_AntennaInfoDedicated* ota_antenna_cfg_ded_ptr = phy_cfg_ded_ptr->antennaInfo.u.explicitValue;
  lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs* ota_recfg_msg_ptr = NULL;

  if (phy_cfg_ded_ptr->antennaInfo.t 
      == T_lte_rrc_osys_PhysicalConfigDedicated_antennaInfo_explicitValue)
  {
    if (ota_antenna_cfg_ded_ptr->ue_TransmitAntennaSelection.t 
        == T_lte_rrc_osys_AntennaInfoDedicated_ue_TransmitAntennaSelection_release)
    {
      TF_ASSERT(antenna_cfg_ded_ptr->tx_antenna_selection_enabled == FALSE);
    }
    else
    {
      TF_ASSERT(antenna_cfg_ded_ptr->tx_antenna_selection_enabled == TRUE);

      /* L1 enum for tm starts at 1 whereas the OTA enum starts at 0, so increment 1*/
      TF_ASSERT(antenna_cfg_ded_ptr->transmission_mode 
                == (ota_antenna_cfg_ded_ptr->transmissionMode + 1));

      /* Get codebooksubsetrestriction info if only TM mode is TM3, TM4, TM5 or TM6.  For
       the rest, ignore this optional field and act as if NW didn't signal. */
      if ((ota_antenna_cfg_ded_ptr->m.codebookSubsetRestrictionPresent) &&
          ((antenna_cfg_ded_ptr->transmission_mode == LTE_L1_DL_TM3) ||
           (antenna_cfg_ded_ptr->transmission_mode == LTE_L1_DL_TM4) ||
           (antenna_cfg_ded_ptr->transmission_mode == LTE_L1_DL_TM5) ||
           (antenna_cfg_ded_ptr->transmission_mode == LTE_L1_DL_TM6) ))
      {
        switch (ota_antenna_cfg_ded_ptr->codebookSubsetRestriction.t) 
        {
          case T_lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n2TxAntenna_tm3:
            TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction 
                      == lte_rrc_osys_asn1_bitstring_to_int(sizeof(antenna_cfg_ded_ptr->codebook_subset_restriction), 
                                                            ota_antenna_cfg_ded_ptr->codebookSubsetRestriction.
                                                            u.n2TxAntenna_tm3));
            break;    
          case T_lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n4TxAntenna_tm3:
            TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction 
                      == lte_rrc_osys_asn1_bitstring_to_int(sizeof(antenna_cfg_ded_ptr->codebook_subset_restriction), 
                                                            ota_antenna_cfg_ded_ptr->codebookSubsetRestriction.
                                                            u.n4TxAntenna_tm3));
            break;
          case T_lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n2TxAntenna_tm4:
            TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction 
                      == lte_rrc_osys_asn1_bitstring_to_int(sizeof(antenna_cfg_ded_ptr->codebook_subset_restriction), 
                                                            ota_antenna_cfg_ded_ptr->codebookSubsetRestriction.
                                                            u.n2TxAntenna_tm4));
            break;
          case T_lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n4TxAntenna_tm4:
            TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction 
                      == lte_rrc_osys_asn1_bitstring_to_int(sizeof(antenna_cfg_ded_ptr->codebook_subset_restriction), 
                                                            (ASN1BitStr32 *) ota_antenna_cfg_ded_ptr->
                                                            codebookSubsetRestriction.u.n4TxAntenna_tm4));
            break;
          case T_lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n2TxAntenna_tm5:
            TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction 
                      == lte_rrc_osys_asn1_bitstring_to_int(sizeof(antenna_cfg_ded_ptr->codebook_subset_restriction), 
                                                            ota_antenna_cfg_ded_ptr->codebookSubsetRestriction.
                                                            u.n2TxAntenna_tm5));
            break;

          case T_lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n4TxAntenna_tm5:
            TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction 
                      == lte_rrc_osys_asn1_bitstring_to_int(sizeof(antenna_cfg_ded_ptr->codebook_subset_restriction), 
                                                            ota_antenna_cfg_ded_ptr->codebookSubsetRestriction.
                                                            u.n4TxAntenna_tm5));
            break;
          case T_lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n2TxAntenna_tm6:
            TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction 
                      == lte_rrc_osys_asn1_bitstring_to_int(sizeof(antenna_cfg_ded_ptr->codebook_subset_restriction), 
                                                            ota_antenna_cfg_ded_ptr->codebookSubsetRestriction.
                                                            u.n2TxAntenna_tm6));
            break;

          case T_lte_rrc_osys_AntennaInfoDedicated_codebookSubsetRestriction_n4TxAntenna_tm6:
            TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction 
                      == lte_rrc_osys_asn1_bitstring_to_int(sizeof(antenna_cfg_ded_ptr->codebook_subset_restriction), 
                                                            ota_antenna_cfg_ded_ptr->codebookSubsetRestriction.
                                                            u.n4TxAntenna_tm6));
            break;
          default:
            TF_ASSERT(FALSE);
        }
      }
      else if (antenna_cfg_ded_ptr->transmission_mode == LTE_L1_DL_TM8_REL9) 
      {
        if (phy_cfg_ded_ptr->m.antennaInfo_v920Present)
        {
          if (phy_cfg_ded_ptr->antennaInfo_v920.m.codebookSubsetRestriction_v920Present)
          {
            switch (phy_cfg_ded_ptr->antennaInfo_v920.codebookSubsetRestriction_v920.t) 
            {
              case T_lte_rrc_osys_AntennaInfoDedicated_v920_codebookSubsetRestriction_v920_n2TxAntenna_tm8_r9:
                TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction 
                          == lte_rrc_osys_asn1_bitstring_to_int(sizeof(antenna_cfg_ded_ptr->codebook_subset_restriction), 
                                                                phy_cfg_ded_ptr->antennaInfo_v920.codebookSubsetRestriction_v920.
                                                                u.n2TxAntenna_tm8_r9));
                break;    
              case T_lte_rrc_osys_AntennaInfoDedicated_v920_codebookSubsetRestriction_v920_n4TxAntenna_tm8_r9:
                TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction 
                          == lte_rrc_osys_asn1_bitstring_to_int(sizeof(antenna_cfg_ded_ptr->codebook_subset_restriction), 
                                                                phy_cfg_ded_ptr->antennaInfo_v920.codebookSubsetRestriction_v920.
                                                                u.n4TxAntenna_tm8_r9));
            }
          }
          else
          {
            /* Existing codebook subset restriction should be released */
            TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction == 0);
          }
        }
      }
      else
      {
        TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction == 0);
      }

      if (ota_antenna_cfg_ded_ptr->ue_TransmitAntennaSelection.u.setup 
          == lte_rrc_osys_AntennaInfoDedicated_ue_TransmitAntennaSelection_setup_closedLoop)
      {
        TF_ASSERT(antenna_cfg_ded_ptr->tx_antenna_selection_ctrl 
                  == LTE_L1_TX_ANTENNA_SELECTION_CLOSED_LOOP);
      }
      else
      {
        TF_ASSERT(antenna_cfg_ded_ptr->tx_antenna_selection_ctrl 
                  == LTE_L1_TX_ANTENNA_SELECTION_OPEN_LOOP);
      }
    }
  }
  else
  {
    if (lte_rrc_llc_test_common_data.ho_in_progress)
    {
      ota_recfg_msg_ptr = lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.
        rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8;

      if ((ota_recfg_msg_ptr->mobilityControlInfo.radioResourceConfigCommon.m.antennaInfoCommonPresent) &&
          (ota_recfg_msg_ptr->mobilityControlInfo.radioResourceConfigCommon.antennaInfoCommon.
          antennaPortsCount > lte_rrc_osys_AntennaInfoCommon_antennaPortsCount_an1))
      {
        TF_ASSERT(antenna_cfg_ded_ptr->transmission_mode == LTE_L1_DL_TM2);
      }
      else
      {
        if (lte_rrc_llc_test_common_data.llc_basic_mib.num_tx_antennas > 1)
        {
          TF_ASSERT(antenna_cfg_ded_ptr->transmission_mode == LTE_L1_DL_TM2);
        }
        else
        {
          TF_ASSERT(antenna_cfg_ded_ptr->transmission_mode == LTE_L1_DL_TM1);
        }
      }
    }
    else
    {
      if (lte_rrc_llc_test_common_data.llc_basic_mib.num_tx_antennas > 1)
      {
        TF_ASSERT(antenna_cfg_ded_ptr->transmission_mode == LTE_L1_DL_TM2);
      }
      else
      {
        TF_ASSERT(antenna_cfg_ded_ptr->transmission_mode == LTE_L1_DL_TM1);
      }
    }

    TF_ASSERT(antenna_cfg_ded_ptr->tx_antenna_selection_enabled == FALSE);
    TF_ASSERT(antenna_cfg_ded_ptr->codebook_subset_restriction == 0);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_sr_cfg

===========================================================================*/
/*!
    @brief
    This function tests the SR config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_sr_cfg
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_cphy_scheduling_req_cfg_type *sr_cfg_ptr
)
{
  lte_rrc_osys_SchedulingRequestConfig *ota_sr_cfg_ptr 
    = &radio_resrc_cfg_ptr->physicalConfigDedicated.schedulingRequestConfig;

  if (radio_resrc_cfg_ptr->physicalConfigDedicated.m.schedulingRequestConfigPresent)
  {
    if (ota_sr_cfg_ptr->t == T_lte_rrc_osys_SchedulingRequestConfig_release)
    {
      TF_ASSERT(sr_cfg_ptr->enable == FALSE);
    }
    else
    {
      TF_ASSERT(sr_cfg_ptr->enable == TRUE);
      TF_ASSERT(sr_cfg_ptr->sr_configuration_index == ota_sr_cfg_ptr->u.setup->sr_ConfigIndex);
      TF_ASSERT(sr_cfg_ptr->sr_pucch_resource_index ==  ota_sr_cfg_ptr->u.setup->sr_PUCCH_ResourceIndex);
      /* ML1 enum has values {4,8,16,32,64} corresponding to OTA enum {0,1,2,3,4}, so 
        convert approriately */
      TF_ASSERT(sr_cfg_ptr->dsr_trans_max == (int) (pow((float) 2,
                                                         (int)(ota_sr_cfg_ptr->u.setup->
                                                               dsr_TransMax + 2))));
    }
  }

  /* Validate Release 9 fields */
  if ((radio_resrc_cfg_ptr->m.mac_MainConfigPresent) &&
      (radio_resrc_cfg_ptr->mac_MainConfig.t 
       == T_lte_rrc_osys_RadioResourceConfigDedicated_mac_MainConfig_explicitValue) &&
      (radio_resrc_cfg_ptr->mac_MainConfig.u.explicitValue->m.sr_ProhibitTimer_r9Present))
  {
    TF_ASSERT(sr_cfg_ptr->sr_prohibit_timer_present == TRUE);
    TF_ASSERT(sr_cfg_ptr->sr_prohibit_timer 
              == radio_resrc_cfg_ptr->mac_MainConfig.u.explicitValue->sr_ProhibitTimer_r9);
  }
  else
  {
    TF_ASSERT(sr_cfg_ptr->sr_prohibit_timer_present == FALSE);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ulsch_cfg

===========================================================================*/
/*!
    @brief
    This function tests the ULSCH config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_ulsch_cfg
(
  lte_rrc_osys_MAC_MainConfig* ota_trch_cfg_ptr,
  lte_cphy_ulsch_cfg_dedicated_param_s *ulsch_cfg_ptr
)
{

  switch(ota_trch_cfg_ptr->ul_SCH_Config.maxHARQ_Tx)
  {
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n1:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n1);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n2:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n2);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n3:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n3);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n4:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n4);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n5:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n5);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n6:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n6);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n7:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n7);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n8:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n8);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n10:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n10);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n12:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n12);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n16:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n16);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n20:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n20);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n24:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n24);
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_maxHARQ_Tx_n28:
      TF_ASSERT(ulsch_cfg_ptr->max_harq_tx == LTE_L1_n28);
      break;
    default:
      TF_ASSERT(FALSE);
  }

  TF_ASSERT(ulsch_cfg_ptr->tti_bundling == ota_trch_cfg_ptr->ul_SCH_Config.ttiBundling);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_connected_drx_cfg

===========================================================================*/
/*!
    @brief
    This function tests the connected DRX config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_connected_drx_cfg
(
  lte_rrc_osys_MAC_MainConfig* ota_trch_cfg_ptr,
  lte_cphy_drx_cfg_dedicated_param_s *connected_drx_cfg_ptr
)
{
  if (ota_trch_cfg_ptr->drx_Config.t == T_lte_rrc_osys_DRX_Config_release)
  {
    TF_ASSERT(connected_drx_cfg_ptr->drx_enable == FALSE);
  }
  else
  {
    TF_ASSERT(connected_drx_cfg_ptr->drx_enable == TRUE);

    switch (ota_trch_cfg_ptr->drx_Config.u.setup->onDurationTimer)
    {
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf1:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF1;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf2:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF2;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf3:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF3;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf4:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF4;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf5:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF5;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf6:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF6;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf8:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF8;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf10:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF10;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf20:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF20;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf30:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF30;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf40:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF40;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf50:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF50;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf60:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF60;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf80:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF80;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf100:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF100;
        break;
      case lte_rrc_osys_DRX_Config_setup_onDurationTimer_psf200:
        connected_drx_cfg_ptr->on_duration_timer = LTE_L1_ON_DURATION_TIMER_PSF200;
        break;
      default:
        TF_ASSERT(0);
    }

    switch (ota_trch_cfg_ptr->drx_Config.u.setup->drx_RetransmissionTimer)
    {
      case lte_rrc_osys_DRX_Config_setup_drx_RetransmissionTimer_psf1:
        connected_drx_cfg_ptr->retx_timer = LTE_L1_DRX_RETX_TIMER_SF1;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_RetransmissionTimer_psf2:
        connected_drx_cfg_ptr->retx_timer = LTE_L1_DRX_RETX_TIMER_SF2;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_RetransmissionTimer_psf4:
        connected_drx_cfg_ptr->retx_timer = LTE_L1_DRX_RETX_TIMER_SF4;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_RetransmissionTimer_psf6:
        connected_drx_cfg_ptr->retx_timer = LTE_L1_DRX_RETX_TIMER_SF6;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_RetransmissionTimer_psf8:
        connected_drx_cfg_ptr->retx_timer = LTE_L1_DRX_RETX_TIMER_SF8;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_RetransmissionTimer_psf16:
        connected_drx_cfg_ptr->retx_timer = LTE_L1_DRX_RETX_TIMER_SF16;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_RetransmissionTimer_psf24:
        connected_drx_cfg_ptr->retx_timer = LTE_L1_DRX_RETX_TIMER_SF24;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_RetransmissionTimer_psf33:
        connected_drx_cfg_ptr->retx_timer = LTE_L1_DRX_RETX_TIMER_SF33;
        break;
      default:
        LTE_ASSERT(0);
        break;
    }


    switch (ota_trch_cfg_ptr->drx_Config.u.setup->drx_InactivityTimer)
    {
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf1:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF1;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf2:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF2;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf3:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF3;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf4:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF4;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf5:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF5;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf6:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF6;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf8:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF8;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf10:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF10;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf20:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF20;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf30:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF30;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf40:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF40;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf50:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF50;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf60:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF60;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf80:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF80;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf100:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF100;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf200:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF200;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf300:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF300;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf500:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF500;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf750:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF750;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf1280:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF1280;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf1920:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF1920;
        break;
      case lte_rrc_osys_DRX_Config_setup_drx_InactivityTimer_psf2560:
        connected_drx_cfg_ptr->inactivity_timer = LTE_L1_DRX_INACTIVITY_TIMER_PSF2560;
        break;
      default:
        TF_ASSERT(0);
    }


    switch(ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.t)
    {
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf10:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF10);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf10);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf20:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF20);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf20);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf32:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF32);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf32);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf40:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF40);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf40);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf64:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF64);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf64);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf80:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF80);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf80);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf128:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF128);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf128);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf160:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF160);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf160);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf256:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF256);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf256);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf320:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF320);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf320);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf512:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF512);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf512);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf640:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF640);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf640);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf1024:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF1024);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf1024);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf1280:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF1280);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf1280);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf2048:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF2048);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf2048);
        break;
      case T_lte_rrc_osys_DRX_Config_setup_longDRX_CycleStartOffset_sf2560:
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  long_drx_cycle == LTE_L1_DRX_LONG_CYCLE_SF2560);
        TF_ASSERT(connected_drx_cfg_ptr->long_drx_cycle_start_offset.
                  offset == ota_trch_cfg_ptr->drx_Config.u.setup->longDRX_CycleStartOffset.u.sf2560);
        break;
      default:
        TF_ASSERT(FALSE);
    }

    if (ota_trch_cfg_ptr->drx_Config.u.setup->m.shortDRXPresent)
    {
      TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle_enable == TRUE);

      switch (ota_trch_cfg_ptr->drx_Config.u.setup->shortDRX.shortDRX_Cycle)
      {
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf2:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF2);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf5:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF5);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf8:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF8);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf10:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF10);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf16:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF16);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf20:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF20);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf32:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF32);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf40:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF40);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf64:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF64);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf80:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF80);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf128:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF128);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf160:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF160);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf256:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF256);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf320:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF320);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf512:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF512);
          break;
        case lte_rrc_osys_DRX_Config_setup_shortDRX_shortDRX_Cycle_sf640:
          TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle == LTE_L1_DRX_SHORT_CYCLE_SF640);
          break;
        default:
          TF_ASSERT(FALSE);
      }

      TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle_timer 
                == ota_trch_cfg_ptr->drx_Config.u.setup->shortDRX.drxShortCycleTimer);
    }
    else
    {
      TF_ASSERT(connected_drx_cfg_ptr->short_drx_cycle_enable == FALSE);
    }
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_phr_cfg

===========================================================================*/
/*!
    @brief
    This function tests the PHR config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_phr_cfg
(
  lte_rrc_osys_MAC_MainConfig* ota_trch_cfg_ptr,
  lte_cphy_phr_cfg_dedicated_param_s *phr_cfg_ptr
)
{
  if (ota_trch_cfg_ptr->phr_Config.t == T_lte_rrc_osys_MAC_MainConfig_phr_Config_release)
  {
    TF_ASSERT(phr_cfg_ptr->enable == FALSE);
    TF_ASSERT(phr_cfg_ptr->extended_phr_indicated == FALSE);
  }
  else
  {
    TF_ASSERT(phr_cfg_ptr->enable == TRUE);

    switch (ota_trch_cfg_ptr->phr_Config.u.setup->periodicPHR_Timer)
    {
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_periodicPHR_Timer_sf10:
        TF_ASSERT(phr_cfg_ptr->periodic_phr_timer == LTE_L1_PERIODIC_PHR_TIMER_SF10);
        break;
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_periodicPHR_Timer_sf20:
        TF_ASSERT(phr_cfg_ptr->periodic_phr_timer == LTE_L1_PERIODIC_PHR_TIMER_SF20);
        break;  
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_periodicPHR_Timer_sf50:
        TF_ASSERT(phr_cfg_ptr->periodic_phr_timer == LTE_L1_PERIODIC_PHR_TIMER_SF50);
        break;
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_periodicPHR_Timer_sf100:
        TF_ASSERT(phr_cfg_ptr->periodic_phr_timer == LTE_L1_PERIODIC_PHR_TIMER_SF100);
        break;  
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_periodicPHR_Timer_sf200:
        TF_ASSERT(phr_cfg_ptr->periodic_phr_timer == LTE_L1_PERIODIC_PHR_TIMER_SF200);
        break;
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_periodicPHR_Timer_sf500:
        TF_ASSERT(phr_cfg_ptr->periodic_phr_timer == LTE_L1_PERIODIC_PHR_TIMER_SF500);
        break;  
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_periodicPHR_Timer_sf1000:
        TF_ASSERT(phr_cfg_ptr->periodic_phr_timer == LTE_L1_PERIODIC_PHR_TIMER_SF1000);
        break;
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_periodicPHR_Timer_infinity:
        TF_ASSERT(phr_cfg_ptr->periodic_phr_timer == LTE_L1_PERIODIC_PHR_TIMER_INFINITE);
        break;
      default:
        TF_ASSERT(FALSE);
    }

    switch (ota_trch_cfg_ptr->phr_Config.u.setup->prohibitPHR_Timer)
    {
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_prohibitPHR_Timer_sf0:
        TF_ASSERT(phr_cfg_ptr->prohibit_phr_timer == LTE_L1_PROHOBIT_PHR_TIMER_SF0);
        break;
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_prohibitPHR_Timer_sf10:
        TF_ASSERT(phr_cfg_ptr->prohibit_phr_timer == LTE_L1_PROHOBIT_PHR_TIMER_SF10);
        break;  
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_prohibitPHR_Timer_sf20:
        TF_ASSERT(phr_cfg_ptr->prohibit_phr_timer == LTE_L1_PROHOBIT_PHR_TIMER_SF20);
        break;
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_prohibitPHR_Timer_sf50:
        TF_ASSERT(phr_cfg_ptr->prohibit_phr_timer == LTE_L1_PROHOBIT_PHR_TIMER_SF50);
        break;  
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_prohibitPHR_Timer_sf100:
        TF_ASSERT(phr_cfg_ptr->prohibit_phr_timer == LTE_L1_PROHOBIT_PHR_TIMER_SF100);
        break;
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_prohibitPHR_Timer_sf200:
        TF_ASSERT(phr_cfg_ptr->prohibit_phr_timer == LTE_L1_PROHOBIT_PHR_TIMER_SF200);
        break;  
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_prohibitPHR_Timer_sf500:
        TF_ASSERT(phr_cfg_ptr->prohibit_phr_timer == LTE_L1_PROHIBIT_PHR_TIMER_SF500);
        break;
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_prohibitPHR_Timer_sf1000:
        TF_ASSERT(phr_cfg_ptr->prohibit_phr_timer == LTE_L1_PROHOBIT_PHR_TIMER_SF1000);
        break;
      default:
        TF_ASSERT(FALSE);
    }

    switch (ota_trch_cfg_ptr->phr_Config.u.setup->dl_PathlossChange)
    {
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_dl_PathlossChange_dB1:
        TF_ASSERT(phr_cfg_ptr->dl_pathloss_change == LTE_L1_DL_PATHLOSS_DB_1);
        break;
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_dl_PathlossChange_dB3:
        TF_ASSERT(phr_cfg_ptr->dl_pathloss_change == LTE_L1_DL_PATHLOSS_DB_3);
        break;  
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_dl_PathlossChange_dB6:
        TF_ASSERT(phr_cfg_ptr->dl_pathloss_change == LTE_L1_DL_PATHLOSS_DB_6);
        break;
      case lte_rrc_osys_MAC_MainConfig_phr_Config_setup_dl_PathlossChange_infinity:
        TF_ASSERT(phr_cfg_ptr->dl_pathloss_change == LTE_L1_DL_PATHLOSS_DB_INFINITE);
        break;  
      default:
        TF_ASSERT(FALSE);
    }
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_sps_interval

===========================================================================*/
/*!
    @brief
    This function tests determines the SPS interval from OTA msg

    @return
    void

 */
/*=========================================================================*/

lte_l1_sps_interval_e lte_rrc_llc_test_get_sps_interval
(
  lte_rrc_osys_SPS_ConfigUL_setup_semiPersistSchedIntervalUL ota_sps_interval,
  boolean tdd_mode
)
{
  lte_l1_sps_interval_e sps_interval;

  switch (ota_sps_interval)
  {
    case lte_rrc_osys_SPS_ConfigUL_setup_semiPersistSchedIntervalUL_sf10:
      sps_interval = LTE_L1_SPS_INTERVAL_SF10;
      break;
    case lte_rrc_osys_SPS_ConfigUL_setup_semiPersistSchedIntervalUL_sf20:
      sps_interval = LTE_L1_SPS_INTERVAL_SF20;
      break;  
    case lte_rrc_osys_SPS_ConfigUL_setup_semiPersistSchedIntervalUL_sf32:
      if (tdd_mode)
      {
        sps_interval = LTE_L1_SPS_INTERVAL_SF30;
      }
      else
      {
        sps_interval = LTE_L1_SPS_INTERVAL_SF32;
      }
      break;
    case lte_rrc_osys_SPS_ConfigUL_setup_semiPersistSchedIntervalUL_sf40:
      sps_interval = LTE_L1_SPS_INTERVAL_SF40;
      break; 
    case lte_rrc_osys_SPS_ConfigUL_setup_semiPersistSchedIntervalUL_sf64:
      if (tdd_mode)
      {
        sps_interval = LTE_L1_SPS_INTERVAL_SF60;
      }
      else
      {
        sps_interval = LTE_L1_SPS_INTERVAL_SF64;
      }
      break;
    case lte_rrc_osys_SPS_ConfigUL_setup_semiPersistSchedIntervalUL_sf80:
      sps_interval = LTE_L1_SPS_INTERVAL_SF80;
      break;  
    case lte_rrc_osys_SPS_ConfigUL_setup_semiPersistSchedIntervalUL_sf128:
      if (tdd_mode)
      {
        sps_interval = LTE_L1_SPS_INTERVAL_SF120;
      }
      else
      {
        sps_interval = LTE_L1_SPS_INTERVAL_SF128;
      }
      break;
    case lte_rrc_osys_SPS_ConfigUL_setup_semiPersistSchedIntervalUL_sf160:
      sps_interval = LTE_L1_SPS_INTERVAL_SF160;
      break;  
    case lte_rrc_osys_SPS_ConfigUL_setup_semiPersistSchedIntervalUL_sf320:
      sps_interval = LTE_L1_SPS_INTERVAL_SF320;
      break;
    case lte_rrc_osys_SPS_ConfigUL_setup_semiPersistSchedIntervalUL_sf640:
      sps_interval = LTE_L1_SPS_INTERVAL_SF640;
      break;  
    default:
      TF_ASSERT(FALSE);
  }

  return sps_interval;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_sps_cfg

===========================================================================*/
/*!
    @brief
    This function tests the SPS config IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_sps_cfg
(
  lte_rrc_osys_SPS_Config* ota_sps_cfg_ptr,
  lte_cphy_sps_cfg_s *sps_cfg_ptr
)
{
  uint8 i = 0;
  lte_rrc_osys_N1PUCCH_AN_PersistentList ota_n1_pucch_persistent_list;
  lte_l1_sps_interval_e sps_interval;
  boolean tdd_mode = FALSE;

  //Check if the cell is TDD or not
  if (((LTE_RRC_TIME_DIV_DUPLEX == lte_rrc_csp_get_last_camped_mode()) &&
       (lte_rrc_llc_data.dd_ptr->oc_ptr->ho_config_in_progress == LTE_RRC_LLC_HANDOVER_NONE)) ||
      ((lte_rrc_llc_data.dd_ptr->oc_ptr->ho_config_in_progress != LTE_RRC_LLC_HANDOVER_NONE) &&
       (LTE_RRC_TIME_DIV_DUPLEX == lte_rrc_get_mode_for_earfcn(lte_rrc_llc_data.dd_ptr->oc_ptr->ho_target_cell_info.dl_cell_frequency))))
  {
    tdd_mode = TRUE;
  }

 /***** Validate SPS Config params *****/
  if (ota_sps_cfg_ptr->m.semiPersistSchedC_RNTIPresent)
  {
    TF_ASSERT(sps_cfg_ptr->use_sps_crnti == TRUE);
    TF_ASSERT(sps_cfg_ptr->sps_crnti 
              == (lte_rnti_val_t)lte_rrc_osys_asn1_bitstring_to_int(sizeof(sps_cfg_ptr->sps_crnti), 
                                                                    &(ota_sps_cfg_ptr->semiPersistSchedC_RNTI)));
  }
  else
  {
    TF_ASSERT(sps_cfg_ptr->use_sps_crnti == FALSE);
  }

  /***** Validate SPS DL Config params *****/
  if (ota_sps_cfg_ptr->m.sps_ConfigDLPresent)
  {
    TF_ASSERT(sps_cfg_ptr->sps_cfg_dl.valid == TRUE);

    if (ota_sps_cfg_ptr->sps_ConfigDL.t == T_lte_rrc_osys_SPS_ConfigDL_release)
    {
      TF_ASSERT(sps_cfg_ptr->sps_cfg_dl.enable == FALSE);
    }
    else
    {
      TF_ASSERT(sps_cfg_ptr->sps_cfg_dl.enable == TRUE);

      ota_n1_pucch_persistent_list = ota_sps_cfg_ptr->sps_ConfigDL.u.setup->n1PUCCH_AN_PersistentList;

      TF_ASSERT(sps_cfg_ptr->sps_cfg_dl.n1_pucch_an_list_size == ota_n1_pucch_persistent_list.n);
      ota_n1_pucch_persistent_list = ota_sps_cfg_ptr->sps_ConfigDL.u.setup->n1PUCCH_AN_PersistentList;
      
      for (i = 0; i < sps_cfg_ptr->sps_cfg_dl.n1_pucch_an_list_size; i++)
      {
        TF_ASSERT(sps_cfg_ptr->sps_cfg_dl.
                  n1_pucch_an_persistent[i] == (int) ota_n1_pucch_persistent_list.elem[i]);
      }
      TF_ASSERT(sps_cfg_ptr->sps_cfg_dl.
                num_conf_sps_processes == ota_sps_cfg_ptr->sps_ConfigDL.u.setup->numberOfConfSPS_Processes);
      sps_interval =  lte_rrc_llc_test_get_sps_interval(ota_sps_cfg_ptr->sps_ConfigDL.
                                                        u.setup->semiPersistSchedIntervalDL, tdd_mode);
      TF_ASSERT(sps_cfg_ptr->sps_cfg_dl.sps_interval == sps_interval);
    }
  }
  else
  {
    TF_ASSERT(sps_cfg_ptr->sps_cfg_dl.valid == FALSE);
  }

  /***** Validate SPS UL Config params *****/
  if (ota_sps_cfg_ptr->m.sps_ConfigULPresent)
  {
    TF_ASSERT(sps_cfg_ptr->sps_cfg_ul.valid == TRUE);
    if (ota_sps_cfg_ptr->sps_ConfigUL.t == T_lte_rrc_osys_SPS_ConfigUL_release)
    {
      TF_ASSERT(sps_cfg_ptr->sps_cfg_ul.enable == FALSE);
    }
    else
    {
      TF_ASSERT(sps_cfg_ptr->sps_cfg_ul.enable == TRUE);

      if (ota_sps_cfg_ptr->sps_ConfigUL.u.setup->m.p0_PersistentPresent)
      {
        TF_ASSERT(sps_cfg_ptr->sps_cfg_ul.p0_persistent_present == TRUE);
        TF_ASSERT(sps_cfg_ptr->sps_cfg_ul.p0_nominal_pusch_persistent 
                  == ota_sps_cfg_ptr->sps_ConfigUL.u.setup->p0_Persistent.p0_NominalPUSCH_Persistent);
        TF_ASSERT(sps_cfg_ptr->sps_cfg_ul.p0_ue_pusch_persistent == ota_sps_cfg_ptr->sps_ConfigUL.u.setup->
                                                                     p0_Persistent.p0_UE_PUSCH_Persistent);
      }
      else
      {
        TF_ASSERT(sps_cfg_ptr->sps_cfg_ul.p0_persistent_present == FALSE);
      }

      sps_interval =  lte_rrc_llc_test_get_sps_interval(ota_sps_cfg_ptr->sps_ConfigUL.
                                                        u.setup->semiPersistSchedIntervalUL, tdd_mode);
      TF_ASSERT(sps_cfg_ptr->sps_cfg_ul.sps_interval == sps_interval);

      if ((lte_rrc_llc_test_common_data.tdd_param.tdd_cfg) &&
          (ota_sps_cfg_ptr->sps_ConfigUL.u.setup->m.twoIntervalsConfigPresent) &&
          (ota_sps_cfg_ptr->sps_ConfigUL.u.setup->twoIntervalsConfig 
           == lte_rrc_osys_SPS_ConfigUL_setup_twoIntervalsConfig_true))
      {
        TF_ASSERT(sps_cfg_ptr->sps_cfg_ul.two_intervals_config == TRUE);
        lte_rrc_llc_test_common_data.tdd_param.two_intervals_cfg_enabled = TRUE;
      }
      else
      {
        TF_ASSERT(sps_cfg_ptr->sps_cfg_ul.two_intervals_config == FALSE);
      }
    }
  }
  else
  {
    TF_ASSERT(sps_cfg_ptr->sps_cfg_ul.valid == FALSE);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rel9_rlf_timers

===========================================================================*/
/*!
    @brief
    This function tests the Rel 9 RLF timers IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_rel9_rlf_timers
(
  lte_rrc_osys_RLF_TimersAndConstants_r9* ota_rlf_timers_ptr,
  lte_cphy_rlf_tmr_and_const_dedicated_param_s *rlf_timers_ptr
)
{
  if (ota_rlf_timers_ptr->t == T_lte_rrc_osys_RLF_TimersAndConstants_r9_setup)
  {
    TF_ASSERT(rlf_timers_ptr->enable == TRUE);
    TF_ASSERT(rlf_timers_ptr->tmr_and_const.t310 == ota_rlf_timers_ptr->u.setup->t310_r9);
    /* L1 enum for n310 starts at 1 whereas the OTA enum starts at 0, so increment 1*/
    TF_ASSERT(rlf_timers_ptr->tmr_and_const.n310 == (ota_rlf_timers_ptr->u.setup->n310_r9 + 1));
    /* L1 enum for n311 starts at 1 whereas the OTA enum starts at 0, so increment 1*/
    TF_ASSERT(rlf_timers_ptr->tmr_and_const.n311 == (ota_rlf_timers_ptr->u.setup->n311_r9+ 1));
  }
  else
  {
    TF_ASSERT(rlf_timers_ptr->enable == FALSE);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_meas_subframe_pattern_pcell

===========================================================================*/
/*!
    @brief
    This function tests the eICIC MeassubframePattern Pcell IE processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_meas_subframe_pattern_pcell
(
  lte_rrc_osys_MeasSubframePatternPCell_r10 *measSubframePatternPCell_ptr,
  lte_l1_subfm_pattern_cfg_param_s *meas_subfm_pattern_pcell_ptr
)
{

  if (measSubframePatternPCell_ptr->t == T_lte_rrc_osys_MeasSubframePatternPCell_r10_setup)
  {

    TF_ASSERT(meas_subfm_pattern_pcell_ptr->enable == TRUE);
    TF_ASSERT(meas_subfm_pattern_pcell_ptr->meas_subfm.type == LTE_L1_MEAS_SUBFM_PATTERN_FDD);
    TF_ASSERT(meas_subfm_pattern_pcell_ptr->meas_subfm.subfm_num_bits == 
              LTE_RRC_SUBFRAME_PATTERN_FDD_NUMBITS);
    TF_ASSERT(meas_subfm_pattern_pcell_ptr->meas_subfm.subfm_pattern[0] == 0x40414243);
    TF_ASSERT(meas_subfm_pattern_pcell_ptr->meas_subfm.subfm_pattern[1] == 0x44000000);

  }

  else
  {

    TF_ASSERT(meas_subfm_pattern_pcell_ptr->enable == FALSE);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr

===========================================================================*/
/*!
    @brief
    This function returns the radio resource config ptr corresponding to the
    source RRC procedure

    @return
    lte_rrc_RadioResourceConfigDedicated *

 */
/*=========================================================================*/
lte_rrc_osys_RadioResourceConfigDedicated *lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr
(
  lte_rrc_proc_e source_proc
)
{
  /* If verifying non-default dedicated phy Config */
  if (source_proc == LTE_RRC_PROC_CEP)
  {
    /* Conn Setup */
    return &lte_rrc_llc_test_common_data.dl_ccch_msg_ptr->message.u.c1->u.
      rrcConnectionSetup->criticalExtensions.u.c1->u.rrcConnectionSetup_r8->radioResourceConfigDedicated;
  }
  else if (source_proc == LTE_RRC_PROC_CONFIG)
  {
    /* Conn Reconfig */
    return &lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.
      rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
      radioResourceConfigDedicated;
  }
  else if (source_proc == LTE_RRC_PROC_CRE)
  {
    /* Conn Reestablishment */
    return &lte_rrc_llc_test_common_data.dl_ccch_msg_ptr->message.u.c1->u.
      rrcConnectionReestablishment->criticalExtensions.u.c1->u.rrcConnectionReestablishment_r8->
      radioResourceConfigDedicated;
  }
  else if ((source_proc == LTE_RRC_PROC_CRP) || (source_proc == LTE_RRC_PROC_CONTROLLER))
  {
    return NULL;
  }
  else
  {
    /* not expected */
    TF_ASSERT(FALSE);
  }
}/* lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr ()*/

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_phy_dedicated_cfg

===========================================================================*/
/*!
    @brief
    This function tests the PHY dedicated config IEs processing by LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_phy_dedicated_cfg
(
  lte_cphy_dedicated_cfg_req_param_s *cphy_cfg_req_param_ptr,
  boolean is_default_cfg,
  lte_rrc_proc_e source_proc
)
{
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr;
  lte_rrc_osys_PhysicalConfigDedicated *phy_cfg_ded_ptr = NULL;
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr = NULL;
  lte_cphy_dedicated_cfg_mask_t cphy_cfg_req_bitmask = 0;

  /* If verifying default dedicated phy config */
  if (is_default_cfg)
  {  
    /* Validate bitmasks of all fields that are part of the default phy config */
    TF_ASSERT(cphy_cfg_req_param_ptr->cfg_mask == (LTE_CPHY_DEDICATED_CFG_MASK_BIT_PDSCH_CFG      |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_PUCCH_CFG      |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_PUSCH_CFG      |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_SRS_CFG        |  
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_UL_POWER_CTRL  |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_CQI_CFG        |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_DRX_CFG        |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_ANTENNA_CFG    |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_TPC_PDCCH_CFG_PUSCH |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_TPC_PDCCH_CFG_PUCCH |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_SCHEDULING_REQ |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_ULSCH_CFG      |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_SPS_CFG        |
                                                   LTE_CPHY_DEDICATED_CFG_MASK_BIT_PHR_CFG));

      /* PDSCH default cfg */
      TF_ASSERT(cphy_cfg_req_param_ptr->pdsch_cfg_dedicated.pa == LTE_L1_PA_DB_0);
  
      /* PUCCH default cfg*/
      TF_ASSERT(cphy_cfg_req_param_ptr->pucch_cfg_dedicated.enable == TRUE);
      TF_ASSERT(cphy_cfg_req_param_ptr->pucch_cfg_dedicated.ack_nak_repetition_enable == FALSE);

    if (lte_rrc_llc_test_common_data.cell_info.cell_id.freq >= 36000)
    {
      TF_ASSERT(cphy_cfg_req_param_ptr->pucch_cfg_dedicated.tdd_ack_nak_feedback_mode_enable == TRUE);
      TF_ASSERT(cphy_cfg_req_param_ptr->pucch_cfg_dedicated.
                tdd_ack_nak_feedback_mode == LTE_L1_TDD_ACK_NAK_FEEDBACK_MODE_BUNDLING);
    }
    else
    {
      TF_ASSERT(cphy_cfg_req_param_ptr->pucch_cfg_dedicated.tdd_ack_nak_feedback_mode_enable == FALSE);
    }

    /* PUSCH default cfg*/
    TF_ASSERT(cphy_cfg_req_param_ptr->pusch_cfg_dedicated.delta_offset_ack_index == 10);
    TF_ASSERT(cphy_cfg_req_param_ptr->pusch_cfg_dedicated.delta_offset_ri_index == 12);
    TF_ASSERT(cphy_cfg_req_param_ptr->pusch_cfg_dedicated.delta_offset_cqi_index == 15);
  
    /* UL Pwr Ctrl default */
    TF_ASSERT(cphy_cfg_req_param_ptr->ul_power_ctrl_dedicated.p0_ue_pusch == 0)
    TF_ASSERT(cphy_cfg_req_param_ptr->ul_power_ctrl_dedicated.
              delta_mcs_enabled == LTE_L1_UL_POWER_CTRL_DELTA_MCS_ENABLE_0);
    TF_ASSERT(cphy_cfg_req_param_ptr->ul_power_ctrl_dedicated.accum_enabled == TRUE)
    TF_ASSERT(cphy_cfg_req_param_ptr->ul_power_ctrl_dedicated.p0_ue_pucch == 0);
    TF_ASSERT(cphy_cfg_req_param_ptr->ul_power_ctrl_dedicated.
              p_srs_offset == LTE_RRC_LLC_DEFAULT_UL_PWR_CTRL_P_SRS_OFFSET);
    TF_ASSERT(cphy_cfg_req_param_ptr->ul_power_ctrl_dedicated.
              filter_coefficient == LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_4);
  
    /* TPC PDCCH default - should be released */
    TF_ASSERT(cphy_cfg_req_param_ptr->tpc_pdcch_cfg_pucch.enable == FALSE);
    TF_ASSERT(cphy_cfg_req_param_ptr->tpc_pdcch_cfg_pusch.enable == FALSE);
  
    /* CQI config - CQIPeriodic config should be released */
    TF_ASSERT(cphy_cfg_req_param_ptr->cqi_cfg.cqi_reporting_periodic_present == TRUE);
    TF_ASSERT(cphy_cfg_req_param_ptr->cqi_cfg.cqi_reporting_periodic.enable == FALSE);
    lte_rrc_llc_test_common_data.cqi_srs_sr_info.cqi_reporting_configured = FALSE;
  
    /* SRS config default - should be released */
    TF_ASSERT(cphy_cfg_req_param_ptr->srs_cfg_dedicated.srs_periodic_cfg_ded.enable == FALSE);
    lte_rrc_llc_test_common_data.cqi_srs_sr_info.srs_configured = FALSE;
  
    /* Antenna cfg default params */
    TF_ASSERT(cphy_cfg_req_param_ptr->antenna_cfg_dedicated.transmission_mode == LTE_L1_DL_TM1);
    TF_ASSERT(cphy_cfg_req_param_ptr->antenna_cfg_dedicated.tx_antenna_selection_enabled == FALSE);
    TF_ASSERT(cphy_cfg_req_param_ptr->antenna_cfg_dedicated.codebook_subset_restriction == 0);
  
    /* SPS default - should be released*/
    TF_ASSERT(cphy_cfg_req_param_ptr->sps_cfg.sps_cfg_dl.valid == TRUE);
    TF_ASSERT(cphy_cfg_req_param_ptr->sps_cfg.sps_cfg_dl.enable == FALSE);
    TF_ASSERT(cphy_cfg_req_param_ptr->sps_cfg.sps_cfg_ul.valid == TRUE);
    TF_ASSERT(cphy_cfg_req_param_ptr->sps_cfg.sps_cfg_ul.enable == FALSE);
  
    /* DRX default - should be released*/
    TF_ASSERT(cphy_cfg_req_param_ptr->con_drx_cfg.drx_enable == FALSE);
  
    /* PHR default - should be released*/
    TF_ASSERT(cphy_cfg_req_param_ptr->phr_cfg.enable == FALSE);
  
    /* ULSCH default cfg*/
    if (source_proc == LTE_RRC_PROC_CSP)
    {
      TF_ASSERT(cphy_cfg_req_param_ptr->ulsch_cfg.max_harq_tx == 5);
      TF_ASSERT(cphy_cfg_req_param_ptr->ulsch_cfg.tti_bundling == FALSE);
    }

	TF_ASSERT(cphy_cfg_req_param_ptr->scheduling_req_cfg.enable == FALSE);
	lte_rrc_llc_test_common_data.cqi_srs_sr_info.sr_configured = FALSE;
	
    TF_MSG("\n PHY default dedicated config verified successfully \n");
    return;
  }

  /* If verifying non-default dedicated phy Config */
  /* Get the appropriate radio resource config */
  radio_resrc_cfg_ptr = lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr(source_proc);
  TF_ASSERT(radio_resrc_cfg_ptr != NULL);

  if (source_proc == LTE_RRC_PROC_CONFIG)
  {
    phy_cfg_ded_ptr = &radio_resrc_cfg_ptr->physicalConfigDedicated;

    /* If verifying handover, UE_RELATED_CFG should be present */
    if (lte_rrc_llc_test_common_data.ho_in_progress)
    {
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_UE_RELATED_CFG;
      if (lte_rrc_llc_test_common_data.cqi_srs_sr_info.cqi_reporting_configured)
      {
        cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_CQI_CFG;
        /* Reset cqi_reporting_configured flag in test DB */
        lte_rrc_llc_test_common_data.cqi_srs_sr_info.cqi_reporting_configured = FALSE;
        if ((phy_cfg_ded_ptr->m.cqi_ReportConfigPresent) == 0)
        {
          TF_ASSERT(cphy_cfg_req_param_ptr->cqi_cfg.enable == TRUE);
          TF_ASSERT(cphy_cfg_req_param_ptr->cqi_cfg.cqi_reporting_periodic.enable == FALSE);
        }
      }
      if (lte_rrc_llc_test_common_data.cqi_srs_sr_info.srs_configured)
      {
        cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_SRS_CFG;
        /* Reset srs_configured flag in test DB */
        lte_rrc_llc_test_common_data.cqi_srs_sr_info.srs_configured = FALSE;
        if ((phy_cfg_ded_ptr->m.soundingRS_UL_ConfigDedicatedPresent) == 0)
        {
          TF_ASSERT(cphy_cfg_req_param_ptr->srs_cfg_dedicated.srs_periodic_cfg_ded.enable == FALSE);
        }
      }
      if (lte_rrc_llc_test_common_data.cqi_srs_sr_info.sr_configured)
      {
        cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_SCHEDULING_REQ;
        if ((phy_cfg_ded_ptr->m.schedulingRequestConfigPresent) == 0)
        {
          /* Here the test DB variable lte_rrc_llc_test_common_data.cqi_srs_sr_info.sr_configured 
            is not set to FALSE, since MAC config is not verified yet. It will be set to FALSE
            while verifying MAC config parameters */
          TF_ASSERT(cphy_cfg_req_param_ptr->scheduling_req_cfg.enable == FALSE)
        }
      }

      if ((lte_rrc_llc_test_common_data.tdd_param.tdd_cfg == FALSE) &&
          (lte_rrc_llc_test_common_data.tdd_param.tdd_ack_nak_feedback_mode_enabled))
      {
        /* For TDD to FDD HO, tdd_ack_nak_feedback_mode_enable should be disabled */
        cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_PUCCH_CFG;
        TF_ASSERT(cphy_cfg_req_param_ptr->pucch_cfg_dedicated.
                  tdd_ack_nak_feedback_mode_enable == FALSE);
        lte_rrc_llc_test_common_data.tdd_param.tdd_ack_nak_feedback_mode_enabled = FALSE;
      }

      if ((lte_rrc_llc_test_common_data.tdd_param.tdd_cfg == FALSE) &&
          (lte_rrc_llc_test_common_data.tdd_param.two_intervals_cfg_enabled))
      {
        /* For TDD to FDD HO, if two_intervals_config was enabled, it should be disabled */
        cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_SPS_CFG;
        TF_ASSERT(cphy_cfg_req_param_ptr->sps_cfg.sps_cfg_ul.two_intervals_config == FALSE);
        lte_rrc_llc_test_common_data.tdd_param.two_intervals_cfg_enabled = FALSE;
      }
    }
  }

  phy_cfg_ded_ptr = &radio_resrc_cfg_ptr->physicalConfigDedicated;

  /***** validate pdsch params   *****/
  if (phy_cfg_ded_ptr->m.pdsch_ConfigDedicatedPresent)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_PDSCH_CFG) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_PDSCH_CFG;
      lte_rrc_llc_test_pdsch_cfg_dedicated(&phy_cfg_ded_ptr->pdsch_ConfigDedicated,
                                           &cphy_cfg_req_param_ptr->pdsch_cfg_dedicated);
      TF_MSG("\n PDSCH dedicated config verified successfully \n");
    }

  /***** validate pucch params   *****/
  if (phy_cfg_ded_ptr->m.pucch_ConfigDedicatedPresent)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_PUCCH_CFG) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_PUCCH_CFG;
      lte_rrc_llc_test_pucch_cfg_dedicated(&phy_cfg_ded_ptr->pucch_ConfigDedicated,
                                           &cphy_cfg_req_param_ptr->pucch_cfg_dedicated);
      TF_MSG("\n PUCCH dedicated config verified successfully \n");
  }

  if (phy_cfg_ded_ptr->m.pucch_ConfigDedicated_v1020Present)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_PUCCH_CFG) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_PUCCH_CFG;
      TF_MSG("\n PUCCH dedicated config verified successfully \n");
  }

  /***** validate pusch params   *****/
  if (phy_cfg_ded_ptr->m.pusch_ConfigDedicatedPresent)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_PUSCH_CFG) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_PUSCH_CFG;
      lte_rrc_llc_test_pusch_cfg_dedicated(&phy_cfg_ded_ptr->pusch_ConfigDedicated,
                                           &cphy_cfg_req_param_ptr->pusch_cfg_dedicated);
      TF_MSG("\n PUSCH dedicated config verified successfully \n");
  }

  /***** Validate soundingRsUl params *****/
  if (phy_cfg_ded_ptr->m.soundingRS_UL_ConfigDedicatedPresent)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_SRS_CFG) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_SRS_CFG;
      lte_rrc_llc_test_srs_cfg_dedicated(&phy_cfg_ded_ptr->soundingRS_UL_ConfigDedicated,
                                         &cphy_cfg_req_param_ptr->srs_cfg_dedicated);
      TF_MSG("\n SRS dedicated config verified successfully \n");
      if (phy_cfg_ded_ptr->soundingRS_UL_ConfigDedicated.
          t == T_lte_rrc_osys_SoundingRS_UL_ConfigDedicated_setup)
      {
        /* Set srs_configured flag in test DB */
        lte_rrc_llc_test_common_data.cqi_srs_sr_info.srs_configured = TRUE;
      }
    }

  /***** validate ul power control params   *****/
  if (phy_cfg_ded_ptr->m.uplinkPowerControlDedicatedPresent)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_UL_POWER_CTRL) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_UL_POWER_CTRL;
      lte_rrc_llc_test_ul_pwr_ctrl_cfg_dedicated(&phy_cfg_ded_ptr->uplinkPowerControlDedicated,
                                                 &cphy_cfg_req_param_ptr->ul_power_ctrl_dedicated);
      TF_MSG("\n UL power control dedicated config verified successfully \n");
    }

  /***** Validate tpc-PDCCH-ConfigPUCCH params   *****/
  if (phy_cfg_ded_ptr->m.tpc_PDCCH_ConfigPUCCHPresent)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_TPC_PDCCH_CFG_PUCCH) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_TPC_PDCCH_CFG_PUCCH;
      lte_rrc_llc_test_tpc_pdcch_cfg(&phy_cfg_ded_ptr->tpc_PDCCH_ConfigPUCCH,
                                     &cphy_cfg_req_param_ptr->tpc_pdcch_cfg_pucch);
      TF_MSG("\n TPC PDCCH for PUCCH dedicated config verified successfully \n");
    }

  /* Validate tpc-PDCCH-ConfigPUSCH params */
  if (phy_cfg_ded_ptr->m.tpc_PDCCH_ConfigPUSCHPresent)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_TPC_PDCCH_CFG_PUSCH) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_TPC_PDCCH_CFG_PUSCH;
      lte_rrc_llc_test_tpc_pdcch_cfg(&phy_cfg_ded_ptr->tpc_PDCCH_ConfigPUSCH,
                                     &cphy_cfg_req_param_ptr->tpc_pdcch_cfg_pusch);
      TF_MSG("\n TPC PDCCH for PUSCH dedicated config verified successfully \n");
    }

  /***** Validate cqi-Reporting params *****/
  if (phy_cfg_ded_ptr->m.cqi_ReportConfigPresent)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_CQI_CFG) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_CQI_CFG;
      lte_rrc_llc_test_cqi_cfg(phy_cfg_ded_ptr,
                               &cphy_cfg_req_param_ptr->cqi_cfg);
      TF_MSG("\n CQI config verified successfully \n");
      if (phy_cfg_ded_ptr->cqi_ReportConfig.m.cqi_ReportPeriodicPresent)
      {
        /* Set cqi_reporting_configured flag in test DB */
        lte_rrc_llc_test_common_data.cqi_srs_sr_info.cqi_reporting_configured = TRUE;
      }
    }

  /***** Validate antenna information params *****/
  if (phy_cfg_ded_ptr->m.antennaInfoPresent)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_ANTENNA_CFG) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_ANTENNA_CFG;
      lte_rrc_llc_test_antenna_cfg_dedicated(phy_cfg_ded_ptr,
                                             &cphy_cfg_req_param_ptr->antenna_cfg_dedicated);
      TF_MSG("\n Antenna dedicated config verified successfully \n");
    }

  /***** Validate schedulingRequestConfig params *****/
  if ((phy_cfg_ded_ptr->m.schedulingRequestConfigPresent) ||
      ((radio_resrc_cfg_ptr->m.mac_MainConfigPresent) &&
       (radio_resrc_cfg_ptr->mac_MainConfig.t 
        == T_lte_rrc_osys_RadioResourceConfigDedicated_mac_MainConfig_explicitValue) && 
       (radio_resrc_cfg_ptr->mac_MainConfig.u.explicitValue->m.sr_ProhibitTimer_r9Present)))
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_SCHEDULING_REQ) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_SCHEDULING_REQ;
      lte_rrc_llc_test_sr_cfg(radio_resrc_cfg_ptr,
                              &cphy_cfg_req_param_ptr->scheduling_req_cfg);
      TF_MSG("\n SR config verified successfully \n");
      if (phy_cfg_ded_ptr->schedulingRequestConfig.
          t == T_lte_rrc_osys_SchedulingRequestConfig_setup)
      {
        /* Set sr_configured flag in test DB */
        lte_rrc_llc_test_common_data.cqi_srs_sr_info.sr_configured = TRUE;
      }
    }

  /***** Validate ULSCH params *****/
  if (radio_resrc_cfg_ptr->mac_MainConfig.t 
      == T_lte_rrc_osys_RadioResourceConfigDedicated_mac_MainConfig_explicitValue)
  {    
    trch_cfg_ptr = radio_resrc_cfg_ptr->mac_MainConfig.u.explicitValue;
    if (trch_cfg_ptr->m.ul_SCH_ConfigPresent)
    {
        TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_ULSCH_CFG) != 0);
        cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_ULSCH_CFG;
        lte_rrc_llc_test_ulsch_cfg(trch_cfg_ptr,
                                   &cphy_cfg_req_param_ptr->ulsch_cfg);
        TF_MSG("\n ULSCH config verified successfully \n");
      }
    }
  else if (radio_resrc_cfg_ptr->mac_MainConfig.t 
           == T_lte_rrc_osys_RadioResourceConfigDedicated_mac_MainConfig_defaultValue)
  {
      /* ULSCH default cfg*/
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_ULSCH_CFG) != 0);
      TF_ASSERT(cphy_cfg_req_param_ptr->ulsch_cfg.max_harq_tx == 5);
      TF_ASSERT(cphy_cfg_req_param_ptr->ulsch_cfg.tti_bundling == FALSE);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_ULSCH_CFG;
      TF_MSG("\n ULSCH default config verified successfully \n");
    }

  /***** Validate DRX params *****/
  if (trch_cfg_ptr)
  {
    if (trch_cfg_ptr->m.drx_ConfigPresent)
    {
        TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_DRX_CFG) != 0);
        cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_DRX_CFG;
        lte_rrc_llc_test_connected_drx_cfg(trch_cfg_ptr,
                                           &cphy_cfg_req_param_ptr->con_drx_cfg);
        TF_MSG("\n Connected DRX config verified successfully \n");
      }

    /***** Validate PHR params *****/
    if (trch_cfg_ptr->m.phr_ConfigPresent)
    {
        TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_PHR_CFG) != 0);
        cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_PHR_CFG;
        lte_rrc_llc_test_phr_cfg(trch_cfg_ptr,
                                 &cphy_cfg_req_param_ptr->phr_cfg);
        TF_MSG("\n PHR config verified successfully \n");
      }
    }

  /***** Validate SPS params *****/
  if (radio_resrc_cfg_ptr->m.sps_ConfigPresent)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_SPS_CFG) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_SPS_CFG;
      lte_rrc_llc_test_sps_cfg(&radio_resrc_cfg_ptr->sps_Config,
                               &cphy_cfg_req_param_ptr->sps_cfg);
      TF_MSG("\n SPS config verified successfully \n");
    }

  /***** Validate Rel 9 RLF timers *****/
  if (radio_resrc_cfg_ptr->m.rlf_TimersAndConstants_r9Present)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_TMR_AND_CONST_PARAM) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_TMR_AND_CONST_PARAM;
      lte_rrc_llc_test_rel9_rlf_timers(&radio_resrc_cfg_ptr->rlf_TimersAndConstants_r9,
                                       &cphy_cfg_req_param_ptr->rlf_tmr_and_const_param);
      TF_MSG("\n Rel9 RLF timers verified successfully \n");
  }

  /***** Validate MeasSubframePattern Pcell *****/
  if (radio_resrc_cfg_ptr->m.measSubframePatternPCell_r10Present)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_SUBFR_PATTERN_PCELL) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_SUBFR_PATTERN_PCELL;

      lte_rrc_llc_test_meas_subframe_pattern_pcell(&radio_resrc_cfg_ptr->measSubframePatternPCell_r10,
                                       &cphy_cfg_req_param_ptr->meas_subfm_pattern_pcell);
      TF_MSG("\n eICIC MeasSubframePattern Pcell verified successfully \n");
    }

  #ifdef FEATURE_LTE_REL11

  /***** CRS assistance info *****/
  if (radio_resrc_cfg_ptr->m.neighCellsCRS_Info_r11Present)
  {
      TF_ASSERT((cphy_cfg_req_param_ptr->cfg_mask & LTE_CPHY_DEDICATED_CFG_MASK_BIT_NEIGH_CELL_CRS_CFG) != 0);
      cphy_cfg_req_bitmask |= LTE_CPHY_DEDICATED_CFG_MASK_BIT_NEIGH_CELL_CRS_CFG;
  }
  #endif

  if (lte_rrc_llc_test_common_data.full_config)
  {
    if (lte_rrc_llc_test_common_data.reconfig_after_cre)
    {
      /* Validate bitmasks that are part of the dedicated phy config during full config */
      TF_ASSERT(cphy_cfg_req_param_ptr->cfg_mask == 0xEFFF); 
    }
    else
    {
      /* Validate bitmasks that are part of the dedicated phy config during full config */
      TF_ASSERT(cphy_cfg_req_param_ptr->cfg_mask == 0xFFFF); 
    }
  }
  else
  {
    /* Validate bitmasks of all fields that are part of the dedicated phy config */
    TF_ASSERT(cphy_cfg_req_param_ptr->cfg_mask == cphy_cfg_req_bitmask); 
  }

  TF_MSG("\n Physical dedicated config verified successfully \n");

}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_shortest_mod_period

===========================================================================*/
/*!
    @brief
    This function tests MCCH schdl config that comes in SIB13

    @return
    uint8, the enum value of the shortest MCCH mod period in SIB13

 */
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_shortest_mod_period
(
  lte_rrc_osys_SystemInformationBlockType13_r9 *sib13_ptr
)
{
  uint8 i = 0, shortest_mod_period = 0;

#ifdef FEATURE_LTE_EMBMS
  shortest_mod_period = sib13_ptr->mbsfn_AreaInfoList_r9.elem[0].mcch_Config_r9.mcch_ModificationPeriod_r9;
  for (i = 1; i < sib13_ptr->mbsfn_AreaInfoList_r9.n; i++)
  {
    if (sib13_ptr->mbsfn_AreaInfoList_r9.elem[i].mcch_Config_r9.
        mcch_ModificationPeriod_r9 < shortest_mod_period )
    {
      shortest_mod_period = sib13_ptr->mbsfn_AreaInfoList_r9.elem[i].mcch_Config_r9.
        mcch_ModificationPeriod_r9;
    }
  }
#endif

  return shortest_mod_period;
}
#ifdef FEATURE_LTE_EMBMS
/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_phy_mcch_cfg

===========================================================================*/
/*!
    @brief
    This function tests MCCH schdl config that comes in SIB13

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_phy_mcch_cfg
(
  lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9 *ota_mcch_cfg_ptr,
  lte_l1_mbsfn_mcch_cfg_s *mcch_schdl_cfg_ptr
)
{
  uint8 i = 0;

  /* Verify MCCH modification period */
  switch (ota_mcch_cfg_ptr->mcch_ModificationPeriod_r9)
  {
    case lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_mcch_ModificationPeriod_r9_rf512:
      TF_ASSERT(mcch_schdl_cfg_ptr->mod_period == LTE_L1_MBSFN_MCCH_MOD_PERIOD_512RF);
      break;
    case lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_mcch_ModificationPeriod_r9_rf1024:
      TF_ASSERT(mcch_schdl_cfg_ptr->mod_period == LTE_L1_MBSFN_MCCH_MOD_PERIOD_1024RF);
      break;
    default:
      TF_ASSERT(FALSE);
  }

  /* Verify MCCH Offset */
  TF_ASSERT(mcch_schdl_cfg_ptr->offset == ota_mcch_cfg_ptr->mcch_Offset_r9);

  /* Verify MCCH repetition period */
  switch (ota_mcch_cfg_ptr->mcch_RepetitionPeriod_r9)
  {
    case lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_mcch_RepetitionPeriod_r9_rf32:
      TF_ASSERT(mcch_schdl_cfg_ptr->rep_period == LTE_L1_MBSFN_MCCH_REP_PERIOD_32RF);
      break;
    case lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_mcch_RepetitionPeriod_r9_rf64:
      TF_ASSERT(mcch_schdl_cfg_ptr->rep_period == LTE_L1_MBSFN_MCCH_REP_PERIOD_64RF);
      break;
    case lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_mcch_RepetitionPeriod_r9_rf128:
      TF_ASSERT(mcch_schdl_cfg_ptr->rep_period == LTE_L1_MBSFN_MCCH_REP_PERIOD_128RF);
      break;
    case lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_mcch_RepetitionPeriod_r9_rf256:
      TF_ASSERT(mcch_schdl_cfg_ptr->rep_period == LTE_L1_MBSFN_MCCH_REP_PERIOD_256RF);
      break;
    default:
      TF_ASSERT(FALSE);
  }

  /* Verify MCCH sf-allocation info bitmask */
  TF_ASSERT(mcch_schdl_cfg_ptr->sf_alloc_info_bitmask 
            == (uint8) lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint8), 
                                                          &ota_mcch_cfg_ptr->sf_AllocInfo_r9));

  /* Verify MCCH signaling MCS */
  switch (ota_mcch_cfg_ptr->signallingMCS_r9)
  {
    case lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_signallingMCS_r9_n2:
      TF_ASSERT(mcch_schdl_cfg_ptr->mcs == LTE_L1_MBSFN_SIGNALLING_MCS_N2);
      break;
    case lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_signallingMCS_r9_n7:
      TF_ASSERT(mcch_schdl_cfg_ptr->mcs == LTE_L1_MBSFN_SIGNALLING_MCS_N7);
      break;
    case lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_signallingMCS_r9_n13:
      TF_ASSERT(mcch_schdl_cfg_ptr->mcs == LTE_L1_MBSFN_SIGNALLING_MCS_N13);
      break;
    case lte_rrc_osys_MBSFN_AreaInfo_r9_mcch_Config_r9_signallingMCS_r9_n19:
      TF_ASSERT(mcch_schdl_cfg_ptr->mcs == LTE_L1_MBSFN_SIGNALLING_MCS_N19);
      break;
    default:
      TF_ASSERT(FALSE);
  }    

} /* lte_rrc_llc_test_phy_mcch_cfg */
#endif
/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_phy_mcch_notification_cfg

===========================================================================*/
/*!
    @brief
    This function tests MCCH notification config that comes in SIB13

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_phy_mcch_notification_cfg
(
  lte_rrc_osys_SystemInformationBlockType13_r9 *sib13_ptr,
  lte_cphy_mbsfn_notification_cfg_s *notif_cfg_ptr
)
{
#ifdef FEATURE_LTE_EMBMS  
  lte_rrc_osys_MBMS_NotificationConfig_r9 *ota_notif_cfg_ptr = &sib13_ptr->notificationConfig_r9;
  uint32 shortest_mod_period = 0, notification_period  = 0;

  /* Verify notification config */
  TF_ASSERT(notif_cfg_ptr->cfg_action == LTE_L1_CONFIG);

  /* Verify notification repetition coefficient */
  switch (ota_notif_cfg_ptr->notificationRepetitionCoeff_r9)
  {
    case lte_rrc_osys_MBMS_NotificationConfig_r9_notificationRepetitionCoeff_r9_n2:
      TF_ASSERT(notif_cfg_ptr->coeff == LTE_L1_MBSFN_NOTIFICATION_COEFF_N2);
      break;
    case lte_rrc_osys_MBMS_NotificationConfig_r9_notificationRepetitionCoeff_r9_n4:
      TF_ASSERT(notif_cfg_ptr->coeff == LTE_L1_MBSFN_NOTIFICATION_COEFF_N4);
      break;
    default:
      TF_ASSERT(FALSE);
  }    

  /* Verify notification offset */
  TF_ASSERT(notif_cfg_ptr->offset == ota_notif_cfg_ptr->notificationOffset_r9);

  /* Verify notification SF Index */
  TF_ASSERT(notif_cfg_ptr->subfn_idx == ota_notif_cfg_ptr->notificationSF_Index_r9);

  /* Get the shortest mod period from SIB13 */
  shortest_mod_period = lte_rrc_llc_test_get_shortest_mod_period(sib13_ptr);

  switch (shortest_mod_period)
  {
    case 0:
      notification_period = (512 / notif_cfg_ptr->coeff);
      break;
    case 1:
      notification_period = (1024 / notif_cfg_ptr->coeff);
      break;
    default:
      TF_ASSERT(FALSE);
  }    

  switch (notification_period)
  {
    case 128:
      TF_ASSERT(notif_cfg_ptr->period == LTE_L1_MBSFN_NOTIFICATION_PERIOD_128RF);
      break;
    case 256:
      TF_ASSERT(notif_cfg_ptr->period == LTE_L1_MBSFN_NOTIFICATION_PERIOD_256RF);
      break;
    case 512:
      TF_ASSERT(notif_cfg_ptr->period == LTE_L1_MBSFN_NOTIFICATION_PERIOD_512RF);
      break;
    default:
      TF_ASSERT(FALSE);
  }    
#endif
} /* lte_rrc_llc_test_phy_mcch_notification_cfg */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_phy_mcch_schdl_cfg

===========================================================================*/
/*!
    @brief
    This function tests MCCH schdl config that comes in SIB13

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_phy_mcch_schdl_cfg
(
  lte_rrc_osys_SystemInformationBlockType13_r9 *sib13_ptr,
  lte_cphy_mbsfn_areas_mcch_schdl_cfg_s *mcch_schdl_cfg_ptr
)
{
  uint8 req_loop_idx = 0, sib_loop_idx = 0, shortest_mod_period = 0, notification_period  = 0;
  boolean area_idx_in_sib = 0;

#ifdef FEATURE_LTE_EMBMS
  /* Verify MCCH config */
  TF_ASSERT(mcch_schdl_cfg_ptr->num_mbsfn_areas == sib13_ptr->mbsfn_AreaInfoList_r9.n);

  /* Verify MBSFN area schedule config */
  for (req_loop_idx = 0; req_loop_idx < lte_rrc_llc_test_common_data.llc_cfg_req.
        cfg_info[0].cfg_u.embms_cfg.num_mcch; req_loop_idx++)
  {

    for (sib_loop_idx = 0; sib_loop_idx < sib13_ptr->mbsfn_AreaInfoList_r9.n; sib_loop_idx++)
    {
      if (mcch_schdl_cfg_ptr->mbsfn_area[req_loop_idx].area_id 
          == sib13_ptr->mbsfn_AreaInfoList_r9.elem[sib_loop_idx].mbsfn_AreaId_r9)
      {
        area_idx_in_sib = sib_loop_idx;
      }
    }

    TF_ASSERT(mcch_schdl_cfg_ptr->mbsfn_area[req_loop_idx].
              area_id == sib13_ptr->mbsfn_AreaInfoList_r9.elem[area_idx_in_sib].mbsfn_AreaId_r9);

    if ((lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[req_loop_idx].
         mrb_cfg_u.mcch_cfg.mbsfn_area_id == mcch_schdl_cfg_ptr->mbsfn_area[req_loop_idx].area_id) &&
        (lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[req_loop_idx].
         mrb_cfg_u.mcch_cfg.mrb_action == LTE_RRC_EMBMS_RB_ACTION_CFG))
    {
      TF_ASSERT(mcch_schdl_cfg_ptr->mbsfn_area[req_loop_idx].cfg_action == LTE_L1_CONFIG);
      TF_ASSERT(mcch_schdl_cfg_ptr->mbsfn_area[req_loop_idx].
                monitor_pattern == LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS);
    }
    else if ((lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[req_loop_idx].
              mrb_cfg_u.mcch_cfg.mbsfn_area_id == mcch_schdl_cfg_ptr->mbsfn_area[req_loop_idx].area_id) &&
             (lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[req_loop_idx].
              mrb_cfg_u.mcch_cfg.mrb_action == LTE_RRC_EMBMS_RB_ACTION_RELEASE))
    {
      TF_ASSERT(mcch_schdl_cfg_ptr->mbsfn_area[req_loop_idx].cfg_action == LTE_L1_DECONFIG);
    }

    /* Verify non-MBSFN area region length */
    switch(sib13_ptr->mbsfn_AreaInfoList_r9.elem[area_idx_in_sib].non_MBSFNregionLength)
    {
      case lte_rrc_osys_MBSFN_AreaInfo_r9_non_MBSFNregionLength_s1:
        TF_ASSERT(mcch_schdl_cfg_ptr->mbsfn_area[req_loop_idx].non_mbsfn_symbols == LTE_L1_NON_MBSFN_SYMBOL_ONE);
        break;
      case lte_rrc_osys_MBSFN_AreaInfo_r9_non_MBSFNregionLength_s2:
        TF_ASSERT(mcch_schdl_cfg_ptr->mbsfn_area[req_loop_idx].non_mbsfn_symbols == LTE_L1_NON_MBSFN_SYMBOL_TWO);
        break;
      default:
        TF_ASSERT(FALSE);
    }

    /* Verify the notification ind bit */
    TF_ASSERT(mcch_schdl_cfg_ptr->mbsfn_area[req_loop_idx].notification_ind_bit 
              == sib13_ptr->mbsfn_AreaInfoList_r9.elem[area_idx_in_sib].notificationIndicator_r9);

    /* Verify MCCH schedule config from SIB13 */
    lte_rrc_llc_test_phy_mcch_cfg(&sib13_ptr->mbsfn_AreaInfoList_r9.elem[area_idx_in_sib].mcch_Config_r9, 
                                  &mcch_schdl_cfg_ptr->mbsfn_area[req_loop_idx].mcch_schdl_cfg);

  }

  /* Verify the notification config from SIB13 */
  lte_rrc_llc_test_phy_mcch_notification_cfg(sib13_ptr, &mcch_schdl_cfg_ptr->notification_cfg);
#endif

}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pmch_is_active

===========================================================================*/
/*!
    @brief
    This function checks if the passed PMCH id is active based on the CFG_REQI
    from EMP.

    @return
    boolean, TRUE if PMCH is active, FALSE otherwise

 */
/*=========================================================================*/
boolean lte_rrc_llc_test_pmch_is_active
(
  lte_mbsfn_area_id_t area_id,
  lte_pmch_id_t pmch_id
)
{
#ifdef FEATURE_LTE_EMBMS
  lte_rrc_llc_embms_cfg_s embms_cfg = lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_u.embms_cfg;
  uint8 i = 0;

  for (i = 0; i < embms_cfg.num_mtch; i++)
  {
    if ((embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.mbsfn_area_id == area_id) &&
        (embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.pmch_id == pmch_id) &&
        (embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.mrb_action == LTE_RRC_EMBMS_RB_ACTION_CFG))
    {
      return TRUE;
    }
  }
#endif

  return FALSE;

} /* lte_rrc_llc_test_pmch_is_active */

#ifdef FEATURE_LTE_EMBMS
/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mbsfn_sf_alloc_pattern

===========================================================================*/
/*!
    @brief
    This function tests SF alloc pattern parameters that are read from the OTA
    message

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mbsfn_sf_alloc_pattern
(
  lte_rrc_osys_CommonSF_AllocPatternList_r9 *ota_sf_alloc_pattern_list,
  lte_cphy_mbsfn_area_cfg_s *mbsfn_area_cfg_ptr
)
{
  uint8 i = 0;

  /* Verify number of SF allocation for eMBMS */
  TF_ASSERT(mbsfn_area_cfg_ptr->num_mbms_subfn_allocations 
            == ota_sf_alloc_pattern_list->n);

  for (i = 0; i < mbsfn_area_cfg_ptr->num_mbms_subfn_allocations; i++)
  {
    /* Verify common RF allocation period */
    switch(ota_sf_alloc_pattern_list->elem[i].radioframeAllocationPeriod)
    {
      case lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n1:
        TF_ASSERT(mbsfn_area_cfg_ptr->subfn_allocation[i].
                  allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_1);
        break;
      case lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n2:
        TF_ASSERT(mbsfn_area_cfg_ptr->subfn_allocation[i].
                  allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_2);
        break;
      case lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n4:
        TF_ASSERT(mbsfn_area_cfg_ptr->subfn_allocation[i].
                  allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_4);
        break;
      case lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n8:
        TF_ASSERT(mbsfn_area_cfg_ptr->subfn_allocation[i].
                  allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_8);
        break;
      case lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n16:
        TF_ASSERT(mbsfn_area_cfg_ptr->subfn_allocation[i].
                  allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_16);
        break;
      case lte_rrc_osys_MBSFN_SubframeConfig_radioframeAllocationPeriod_n32:
        TF_ASSERT(mbsfn_area_cfg_ptr->subfn_allocation[i].
                  allocation_period == LTE_L1_MBSFN_ALLOCATION_PERIOD_32);
        break;
      default:
        TF_ASSERT(FALSE);
    }

    /* Verify RF allocation offset */
    TF_ASSERT(mbsfn_area_cfg_ptr->subfn_allocation[i].allocation_offset 
              == ota_sf_alloc_pattern_list->elem[i].radioframeAllocationOffset);

    /* Verify SF allocation */
    switch(ota_sf_alloc_pattern_list->elem[i].subframeAllocation.t)
    {
      case T_lte_rrc_osys_MBSFN_SubframeConfig_subframeAllocation_oneFrame:
        TF_ASSERT(mbsfn_area_cfg_ptr->subfn_allocation[i].
                  sfn_allocation == LTE_L1_MBSFN_ALLOCATION_ONE);

        TF_ASSERT(mbsfn_area_cfg_ptr->subfn_allocation[i].sfn_allocation_mask 
                  == (uint32) lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32), 
                                                                 (ota_sf_alloc_pattern_list->elem[i].
                                                                  subframeAllocation.u.oneFrame)));
        break;
      case T_lte_rrc_osys_MBSFN_SubframeConfig_subframeAllocation_fourFrames:
        TF_ASSERT(mbsfn_area_cfg_ptr->subfn_allocation[i].
                  sfn_allocation == LTE_L1_MBSFN_ALLOCATION_FOUR);
        TF_ASSERT(mbsfn_area_cfg_ptr->subfn_allocation[i].sfn_allocation_mask 
                  == (uint32) lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint32), 
                                                                 (ota_sf_alloc_pattern_list->elem[i].
                                                                  subframeAllocation.u.fourFrames)));
        break;
      default:
        TF_ASSERT(FALSE);
    }
  }

} /* lte_rrc_llc_test_mbsfn_sf_alloc_pattern */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pmch_config

===========================================================================*/
/*!
    @brief
    This function tests PMCH config that are read from the OTA
    message

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_pmch_config
(
  lte_rrc_osys_PMCH_InfoList_r9 *ota_pmch_info_list,
  lte_cphy_mbsfn_area_cfg_s *mbsfn_area_cfg_ptr
)
{
#ifdef FEATURE_LTE_EMBMS  
  uint8 i = 0;

  /* Verify PMCH config */
  TF_ASSERT(mbsfn_area_cfg_ptr->num_pmch == ota_pmch_info_list->n);

  for (i = 0; i < ota_pmch_info_list->n; i++)
  {
    TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.pmch_id == (i + 1));
    TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg_action == LTE_L1_CONFIG);

    /* Check if the PMCH id is active */
    if (lte_rrc_llc_test_pmch_is_active(mbsfn_area_cfg_ptr->area_id, (i + 1)))
    {
      TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].monitor_pattern 
                == LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS);
    }
    else
    {
      TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].monitor_pattern 
                == LTE_CPHY_MBSFN_AREA_NO_MONITOR);
    }

    /* Verify PMCH scheduling period */
    switch(ota_pmch_info_list->elem[i].pmch_Config_r9.mch_SchedulingPeriod_r9)
    {
      case lte_rrc_osys_PMCH_Config_r9_mch_SchedulingPeriod_r9_rf8:
        TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.schdl_period == LTE_L1_MBSFN_MCH_SCHDL_PERIOD_8RF);
        break;
      case lte_rrc_osys_PMCH_Config_r9_mch_SchedulingPeriod_r9_rf16:
        TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.schdl_period == LTE_L1_MBSFN_MCH_SCHDL_PERIOD_16RF);
        break;
      case lte_rrc_osys_PMCH_Config_r9_mch_SchedulingPeriod_r9_rf32:
        TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.schdl_period == LTE_L1_MBSFN_MCH_SCHDL_PERIOD_32RF);
        break;
      case lte_rrc_osys_PMCH_Config_r9_mch_SchedulingPeriod_r9_rf64:
        TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.schdl_period == LTE_L1_MBSFN_MCH_SCHDL_PERIOD_64RF);
        break;
      case lte_rrc_osys_PMCH_Config_r9_mch_SchedulingPeriod_r9_rf128:
        TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.schdl_period == LTE_L1_MBSFN_MCH_SCHDL_PERIOD_128RF);
        break;
      case lte_rrc_osys_PMCH_Config_r9_mch_SchedulingPeriod_r9_rf256:
        TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.schdl_period == LTE_L1_MBSFN_MCH_SCHDL_PERIOD_256RF);
        break;
      case lte_rrc_osys_PMCH_Config_r9_mch_SchedulingPeriod_r9_rf512:
        TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.schdl_period == LTE_L1_MBSFN_MCH_SCHDL_PERIOD_512RF);
        break;
      case lte_rrc_osys_PMCH_Config_r9_mch_SchedulingPeriod_r9_rf1024:
        TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.schdl_period == LTE_L1_MBSFN_MCH_SCHDL_PERIOD_1024RF);
        break;
      default:
        TF_ASSERT(FALSE);
    }

    /* Verify data MCS */
    TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.mcs == ota_pmch_info_list->elem[i].pmch_Config_r9.dataMCS_r9);

    /* If verifying PMCH id 1*/
    if (i == 0)
    {
      TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.sf_alloc_start == 0);
    }
    else
    {
      /* Else, verify SF alloc start = SF alloc end of previous PMCH in the pmch_InfoList */
      TF_ASSERT(mbsfn_area_cfg_ptr->pmch[i].cfg.sf_alloc_start 
                == (ota_pmch_info_list->elem[i - 1].pmch_Config_r9.sf_AllocEnd_r9 + 1));
    }
  } 
#endif
}/* lte_rrc_llc_test_pmch_config */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mbsfn_areas_cfg

===========================================================================*/
/*!
    @brief
    This function tests MBSFN areas config that comes in MCCH msg

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mbsfn_areas_cfg
(
  lte_rrc_osys_MBSFNAreaConfiguration_r9 *ota_mbsfn_area_cfg_ptr,
  lte_cphy_mbsfn_areas_cfg_s *mcch_areas_cfg_ptr
)
{
  uint8 i = 0;
  TF_ASSERT(mcch_areas_cfg_ptr->num_mbsfn_areas == 1);
  TF_ASSERT(mcch_areas_cfg_ptr->mbsfn_area[0].area_id == 100);
  TF_ASSERT(mcch_areas_cfg_ptr->mbsfn_area[0].cfg_action == LTE_L1_CONFIG);

  /* Verify common SF allocation params */
  /* Verify SF allocation period for eMBMS */
  switch(ota_mbsfn_area_cfg_ptr->commonSF_AllocPeriod_r9)
  {
    case lte_rrc_osys_MBSFNAreaConfiguration_r9_commonSF_AllocPeriod_r9_rf4:
      TF_ASSERT(mcch_areas_cfg_ptr->mbsfn_area[0].
                subfn_allocation_period == LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_4RF);
      break;
    case lte_rrc_osys_MBSFNAreaConfiguration_r9_commonSF_AllocPeriod_r9_rf8:
      TF_ASSERT(mcch_areas_cfg_ptr->mbsfn_area[0].
                subfn_allocation_period == LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_8RF);
      break;
    case lte_rrc_osys_MBSFNAreaConfiguration_r9_commonSF_AllocPeriod_r9_rf16:
      TF_ASSERT(mcch_areas_cfg_ptr->mbsfn_area[0].
                subfn_allocation_period == LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_16RF);
      break;
    case lte_rrc_osys_MBSFNAreaConfiguration_r9_commonSF_AllocPeriod_r9_rf32:
      TF_ASSERT(mcch_areas_cfg_ptr->mbsfn_area[0].
                subfn_allocation_period == LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_32RF);
      break;
    case lte_rrc_osys_MBSFNAreaConfiguration_r9_commonSF_AllocPeriod_r9_rf64:
      TF_ASSERT(mcch_areas_cfg_ptr->mbsfn_area[0].
                subfn_allocation_period == LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_64RF);
      break;
    case lte_rrc_osys_MBSFNAreaConfiguration_r9_commonSF_AllocPeriod_r9_rf128:
      TF_ASSERT(mcch_areas_cfg_ptr->mbsfn_area[0].
                subfn_allocation_period == LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_128RF);
      break;
    case lte_rrc_osys_MBSFNAreaConfiguration_r9_commonSF_AllocPeriod_r9_rf256:
      TF_ASSERT(mcch_areas_cfg_ptr->mbsfn_area[0].
                subfn_allocation_period == LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_256RF);
      break;
    default:
      TF_ASSERT(FALSE);
  }

  /* Verify MBSFN SF alloc pattern */
  lte_rrc_llc_test_mbsfn_sf_alloc_pattern(&ota_mbsfn_area_cfg_ptr->commonSF_Alloc_r9, 
                                          &mcch_areas_cfg_ptr->mbsfn_area[0]);

  /* Verify PMCH config */
  lte_rrc_llc_test_pmch_config(&ota_mbsfn_area_cfg_ptr->pmch_InfoList_r9, 
                               &mcch_areas_cfg_ptr->mbsfn_area[0]);

} /* lte_rrc_llc_test_mbsfn_areas_cfg */
#endif

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_phy_common_cfg_req_and_send_cnf

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying PHY common config 
    parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_phy_common_cfg_req_and_send_cnf
(
  void
)
{
  lte_rrc_osys_SystemInformationBlockType2 *sib2_ptr = lte_rrc_llc_test_common_data.
    bcch_dl_sch_sib2_ptr->message.u.c1->u.systemInformation->criticalExtensions.u.systemInformation_r8->
    sib_TypeAndInfo.elem->u.sib2;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  uint32 buf_len;

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  TF_MSG("\nLLC: Rcvd CPHY_COMMON_CFG_REQ \n");

  /* Verify all PHY Common Config IEs */
  lte_rrc_llc_test_phy_common_cfg(sib2_ptr, cphy_common_cfg_req_ptr);

  /* send CPHY_COMMON_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_phy_common_cfg_req_and_send_cnf_embms

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying PHY common config 
    parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_phy_common_cfg_req_and_send_cnf_embms
(
  boolean cfg_for_release,
  boolean release_all_mrb
)
{
  lte_rrc_osys_SystemInformationBlockType13_r9 *sib13_ptr = NULL; 
#ifdef FEATURE_LTE_EMBMS  
  lte_rrc_osys_MBSFNAreaConfiguration_r9 *ota_mbsfn_area_cfg_ptr = NULL;
#endif
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  uint32 buf_len;
  uint16 cfg_mask = 0;

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  TF_MSG("\nLLC: Rcvd CPHY_COMMON_CFG_REQ \n");

  /* Validate bitmasks of all fields that are part of the common phy config */

#ifdef FEATURE_LTE_EMBMS
  if (lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mcch > 0)
  {
    cfg_mask |= LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_AREAS_MCCH_SCHDL_CFG;
  }

  if (lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mtch > 0)
  {
    cfg_mask |= LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_AREA_CFG;
  }
#endif

  TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.cfg_mask == cfg_mask);

  if (release_all_mrb)
  {
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.
              mbsfn_areas_mcch_schdl_cfg_ptr->mbsfn_area[0].cfg_action == LTE_L1_DECONFIG);
  }
  else if (cfg_for_release)
  {
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.
              mbsfn_areas_mcch_schdl_cfg_ptr->num_mbsfn_areas == 2);

    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.
              mbsfn_areas_mcch_schdl_cfg_ptr->mbsfn_area[0].area_id == 100);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.
              mbsfn_areas_mcch_schdl_cfg_ptr->mbsfn_area[0].cfg_action == LTE_L1_CONFIG);

    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.
              mbsfn_areas_mcch_schdl_cfg_ptr->mbsfn_area[1].area_id == 101);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.
              mbsfn_areas_mcch_schdl_cfg_ptr->mbsfn_area[1].cfg_action == LTE_L1_DECONFIG);
  }
  else
  {
    sib13_ptr = lte_rrc_llc_test_common_data.bcch_dl_sch_sib13_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib13_v920;

    /* Verify MCCH schdl config */
    lte_rrc_llc_test_phy_mcch_schdl_cfg(sib13_ptr,
                                        cphy_common_cfg_req_ptr->common_cfg_param.
                                        mbsfn_areas_mcch_schdl_cfg_ptr);
  }

#ifdef FEATURE_LTE_EMBMS
  /* For the non-release all case, PMCHs will be released individually */
  if (release_all_mrb)
  {
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->num_mbsfn_areas == 1);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              area_id == 100);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              cfg_action == LTE_L1_DECONFIG);
  }
  else if (cfg_for_release)
  {
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->num_mbsfn_areas == 1);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              area_id == 100);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              cfg_action == LTE_L1_CONFIG);

    /* Verify PMCH config */
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              num_pmch == 3);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              pmch[0].cfg.pmch_id == 1);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              pmch[0].cfg_action == LTE_L1_CONFIG);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              pmch[0].monitor_pattern == LTE_CPHY_MBSFN_AREA_NO_MONITOR);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              pmch[1].cfg.pmch_id == 2);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              pmch[1].cfg_action == LTE_L1_CONFIG);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              pmch[1].monitor_pattern == LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              pmch[2].cfg.pmch_id == 3);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              pmch[2].cfg_action == LTE_L1_CONFIG);
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr->mbsfn_area[0].
              pmch[2].monitor_pattern == LTE_CPHY_MBSFN_AREA_NO_MONITOR);
  }
  else if (lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mtch > 0)
  {
    ota_mbsfn_area_cfg_ptr = lte_rrc_llc_test_common_data.dl_mcch_msg_ptr->message.u.
      c1->u.mbsfnAreaConfiguration_r9;

    /* Verify MBSFN areas cfg from MCCH message */
    lte_rrc_llc_test_mbsfn_areas_cfg(ota_mbsfn_area_cfg_ptr,
                                     cphy_common_cfg_req_ptr->common_cfg_param.mbsfn_areas_cfg_ptr);
  }
  
  /* send CPHY_COMMON_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  #endif
  
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_phy_dedicated_cfg_req

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying PHY dedicated config 
    parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf
(
  lte_rrc_proc_e proc,
  boolean test_default_cfg
)
{
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  uint32 buf_len;

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);
  TF_MSG("\nLLC: Rcvd CPHY_DEDICATED_CFG_REQ \n");

  if ((test_default_cfg) && (proc != LTE_RRC_PROC_CEP) && (proc != LTE_RRC_PROC_CRE))
  {
    TF_ASSERT(cphy_dedicated_cfg_req_ptr->is_default_cfg == TRUE);
  }
  else
  {
    TF_ASSERT(cphy_dedicated_cfg_req_ptr->is_default_cfg == FALSE);
  }

  /* Verify default dedicated PHY Config params */
  lte_rrc_llc_test_phy_dedicated_cfg(&cphy_dedicated_cfg_req_ptr->dedicated_cfg_param,test_default_cfg,proc);

  /* After CPHY Config req is sent, assert LLC is in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  TF_MSG("\nCPHY_DEDICATED_CFG_REQ rcvd and LLC Module in WAIT_FOR_CPHY_CFG_CNF state\n");

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  /* Setting the fields in the LTE_CPHY_DEDICATED_CFG_CNF sent by CPHY, The bits
    set in dedicated_cfg_validity field reflect the channel config that were set
    in the llc_conn_setup_msg in the lte_rrc_llc_ota_msg.h */
  cphy_dedicated_cfg_cnf.dedicated_cfg_validity = cphy_dedicated_cfg_req_ptr->dedicated_cfg_param.cfg_mask;
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying PHY handover config 
    parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf
(
  boolean handover_success
)
{
  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;
  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;
  uint32 buf_len = 0;
  
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);

  /* Verify Mobility Control IE parameters */
  lte_rrc_llc_test_phy_mobility_cfg(cphy_cfg_req_ptr);
  /* Verify dedicated PHY Config params */
  lte_rrc_llc_test_phy_dedicated_cfg(&cphy_cfg_req_ptr->dedicated_cfg_param,FALSE,LTE_RRC_PROC_CONFIG);

  /* After CPHY Config req is sent, assert LLC is in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  TF_MSG("\nCPHY Config Request sent to CPHY and LLC Module in WAIT_FOR_CPHY_CFG_CNF state\n");
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);

  if (handover_success)
  {
    cphy_cfg_cnf_msg.status = E_SUCCESS;
    /* Setting the fields in the LTE_CPHY_HANDOVER_CNF sent by CPHY, The bits
      set in com_mob_parm_validity field reflect the channel config that were set
      in the llc_reconfig_msg in the lte_rrc_llc_ota_msg.h */
    cphy_cfg_cnf_msg.com_mob_parm_validity = cphy_cfg_req_ptr->common_mobility_cfg_param.cfg_mask;
    cphy_cfg_cnf_msg.dedicated_parm_validity = cphy_cfg_req_ptr->dedicated_cfg_param.cfg_mask;
  }
  else
  {
    cphy_cfg_cnf_msg.status = E_FAILURE;
  }
  
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  if (handover_success)
  {
    /* Catch the serving cell changed indication thrown by LLC */
    lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
    TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);

    /* The freq and cell id should be same as what was sent in LTE_CPHY_HANDOVER_REQ */
    TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
              freq == cphy_cfg_req_ptr->ho_target_cell_info.dl_cell_frequency);
    TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
              phy_cell_id == cphy_cfg_req_ptr->ho_target_cell_info.target_cell_id);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rach_cfg

===========================================================================*/
/*!
    @brief
    This function tests all the RACH config parameters.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_rach_cfg
(
  lte_rrc_osys_RACH_ConfigCommon *ota_rach_cfg_ptr,
  lte_mac_rach_cfg_s *rach_cfg_ptr
)
{
  uint16 message_size_qroup_a = 0, preamble_trans_max = 0;
  uint8 ra_rsp_win_size = 0;

  /* Verify preamble info */
  if (ota_rach_cfg_ptr->preambleInfo.m.preamblesGroupAConfigPresent)
  {
    /* sizeOfRA_PreamblesGroupA enum values range from (0,1,2,3,4,5...) while 
      MAC enum for ra_index range from (4,8,12,16,20...) */
    TF_ASSERT(rach_cfg_ptr->ra_index1 
              == (uint8) ((ota_rach_cfg_ptr->preambleInfo.preamblesGroupAConfig.sizeOfRA_PreamblesGroupA + 1) * 4));

    /* numberOfRA_Preambles enum values range from (0,1,2,3,4,5...) while 
      MAC enum for ra_index range from (4,8,12,16,20...) */
    TF_ASSERT(rach_cfg_ptr->ra_index2 
              == (uint8) ((ota_rach_cfg_ptr->preambleInfo.numberOfRA_Preambles + 1) * 4));

    /* Get the messageSizeGroupA based on the OTA message */
    switch (ota_rach_cfg_ptr->preambleInfo.preamblesGroupAConfig.messageSizeGroupA) 
    {
      case lte_rrc_osys_RACH_ConfigCommon_preambleInfo_preamblesGroupAConfig_messageSizeGroupA_b56:
        message_size_qroup_a = 56 >> 3;
        break;
      case lte_rrc_osys_RACH_ConfigCommon_preambleInfo_preamblesGroupAConfig_messageSizeGroupA_b144:
        message_size_qroup_a = 144 >> 3;
        break;
      case lte_rrc_osys_RACH_ConfigCommon_preambleInfo_preamblesGroupAConfig_messageSizeGroupA_b208:
        message_size_qroup_a = 208 >> 3;
        break;
      case lte_rrc_osys_RACH_ConfigCommon_preambleInfo_preamblesGroupAConfig_messageSizeGroupA_b256:
        message_size_qroup_a = 256 >> 3;
        break;
      default:
        TF_ASSERT(FALSE);
    }
    /* Check if the determined messageSizeGroupA value is what is passed to MAC */
    TF_ASSERT(rach_cfg_ptr->message_size_qroup_a == message_size_qroup_a);

    TF_ASSERT(rach_cfg_ptr->pwr_offset_group_b 
              == (lte_mac_msg_pwr_offset_group_b_e)rach_cfg_ptr->pwr_offset_group_b);
  }
  else
  {
    /* numberOfRA_Preambles enum values range from (0,1,2,3,4,5...) while 
      MAC enum for ra_index range from (4,8,12,16,20...) */
    TF_ASSERT(rach_cfg_ptr->ra_index1 
              == (uint8) ((ota_rach_cfg_ptr->preambleInfo.numberOfRA_Preambles + 1) * 4));
  }

  /* Verify power ramping parameters */
  /* power_ramping_step enum values range from (0,1,2,3) while 
    MAC enum for power_ramping_step range from (0,2,4,6) */
  TF_ASSERT(rach_cfg_ptr->power_ramping_step 
            == (uint8) (ota_rach_cfg_ptr->powerRampingParameters.powerRampingStep * 2));
  /* preamble_initial_power enum values range from (0,1,2,3,4,5...) while 
    MAC enum for preamble_initial_power range from (-120,-118,-116,-114,-112...) */
  TF_ASSERT(rach_cfg_ptr->preamble_initial_power 
            == ((ota_rach_cfg_ptr->powerRampingParameters.preambleInitialReceivedTargetPower*2) - 120));

  /* Verify RA Supervision info */
  /* Get the preambleTransMax based on the OTA message */
  switch (ota_rach_cfg_ptr->ra_SupervisionInfo.preambleTransMax) 
  {
    case lte_rrc_osys_PreambleTransMax_n3:
      preamble_trans_max = 3;
      break;
    case lte_rrc_osys_PreambleTransMax_n4:
      preamble_trans_max = 4;
      break;
    case lte_rrc_osys_PreambleTransMax_n5:
      preamble_trans_max = 5;
      break;
    case lte_rrc_osys_PreambleTransMax_n6:
      preamble_trans_max = 6;
      break;
    case lte_rrc_osys_PreambleTransMax_n7:
      preamble_trans_max = 7;
      break;
    case lte_rrc_osys_PreambleTransMax_n8:
      preamble_trans_max = 8;
      break;
    case lte_rrc_osys_PreambleTransMax_n10:
      preamble_trans_max = 10;
      break;
    case lte_rrc_osys_PreambleTransMax_n20:
      preamble_trans_max = 20;
      break;
    case lte_rrc_osys_PreambleTransMax_n50:
      preamble_trans_max = 50;
      break;
    case lte_rrc_osys_PreambleTransMax_n100:
      preamble_trans_max = 100;
      break;
    case lte_rrc_osys_PreambleTransMax_n200:
      preamble_trans_max = 200;
      break;
    default:
      TF_ASSERT(FALSE);
  }
  /* Check if the determined preambleTransMax value is what is passed to MAC */
  TF_ASSERT(rach_cfg_ptr->preamble_trans_max == preamble_trans_max);

  /* preamble_initial_power enum values range from (0,1,2,3,4,5...) while 
    MAC enum for ra_index range from (8,16,24,32,40...) */
  TF_ASSERT(rach_cfg_ptr->contention_resolution_timer 
            == ((ota_rach_cfg_ptr->ra_SupervisionInfo.mac_ContentionResolutionTimer + 1) * 8));

  /* ML1 PRACH info */
  /* Get the ra_ResponseWindowSize based on the OTA message */
  switch (ota_rach_cfg_ptr->ra_SupervisionInfo.ra_ResponseWindowSize) 
  {
    case lte_rrc_osys_RACH_ConfigCommon_ra_SupervisionInfo_ra_ResponseWindowSize_sf2:
      ra_rsp_win_size = 2;
      break;
    case lte_rrc_osys_RACH_ConfigCommon_ra_SupervisionInfo_ra_ResponseWindowSize_sf3:
      ra_rsp_win_size = 3;
      break;
    case lte_rrc_osys_RACH_ConfigCommon_ra_SupervisionInfo_ra_ResponseWindowSize_sf4:
      ra_rsp_win_size = 4;
      break;
    case lte_rrc_osys_RACH_ConfigCommon_ra_SupervisionInfo_ra_ResponseWindowSize_sf5:
      ra_rsp_win_size = 5;
      break;
    case lte_rrc_osys_RACH_ConfigCommon_ra_SupervisionInfo_ra_ResponseWindowSize_sf6:
      ra_rsp_win_size = 6;
      break;
    case lte_rrc_osys_RACH_ConfigCommon_ra_SupervisionInfo_ra_ResponseWindowSize_sf7:
      ra_rsp_win_size = 7;
      break;
    case lte_rrc_osys_RACH_ConfigCommon_ra_SupervisionInfo_ra_ResponseWindowSize_sf8:
      ra_rsp_win_size = 8;
      break;
    case lte_rrc_osys_RACH_ConfigCommon_ra_SupervisionInfo_ra_ResponseWindowSize_sf10:
      ra_rsp_win_size = 10;
      break;
    default:
      TF_ASSERT(FALSE);        
  }
  /* Check if the determined ra_ResponseWindowSize value is what is passed to MAC */
  TF_ASSERT(rach_cfg_ptr->l1_rach.ra_rsp_win_size == ra_rsp_win_size);
  TF_ASSERT(rach_cfg_ptr->l1_rach.max_retx_msg3 == ota_rach_cfg_ptr->maxHARQ_Msg3Tx);
  TF_MSG("\n RACH config verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_prach_cfg_info

===========================================================================*/
/*!
    @brief
    This function tests all the PRACH config info parameters.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_prach_cfg_info
(
  lte_rrc_osys_PRACH_ConfigInfo *ota_prach_cfg_info_ptr,
  lte_cphy_prach_setup_param_s *prach_cfg_info_ptr
)
{
  TF_ASSERT(prach_cfg_info_ptr->prach_cfg == ota_prach_cfg_info_ptr->prach_ConfigIndex);
  TF_ASSERT(prach_cfg_info_ptr->high_speed_flag == ota_prach_cfg_info_ptr->highSpeedFlag);
  TF_ASSERT(prach_cfg_info_ptr->cs_zone_length == ota_prach_cfg_info_ptr->zeroCorrelationZoneConfig);
  TF_ASSERT(prach_cfg_info_ptr->prach_frequency_offset == ota_prach_cfg_info_ptr->prach_FreqOffset);
  TF_MSG("\n PRACH config verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mac_common_cfg

===========================================================================*/
/*!
    @brief
    This function tests all the MAC common config parameters.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mac_common_cfg
(
  lte_rrc_osys_SystemInformationBlockType2 *sib2_ptr,
  lte_mac_cfg_req_msg_s *mac_req_ptr
)
{

  uint16 ta_timer = 0;

  TF_ASSERT(mac_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_RACH_CFG_BM | 
                                                  LTE_MAC_DL_CFG_BM |
                                                  LTE_MAC_UL_CFG_BM));

  lte_rrc_llc_test_rach_cfg(&sib2_ptr->radioResourceConfigCommon.rach_ConfigCommon,
                            mac_req_ptr->cfg_data.rach_cfg);

  /* Check delta preamble msg3 value in uplinkPowerControl */
  TF_ASSERT(mac_req_ptr->cfg_data.rach_cfg->delta_preamble_msg3 
            == sib2_ptr->radioResourceConfigCommon.uplinkPowerControlCommon.deltaPreambleMsg3 * 2);

  TF_ASSERT(mac_req_ptr->cfg_data.rach_cfg->l1_rach.
            log_root_seq_index == sib2_ptr->radioResourceConfigCommon.prach_Config.rootSequenceIndex);

  lte_rrc_llc_test_prach_cfg_info(&sib2_ptr->radioResourceConfigCommon.prach_Config.prach_ConfigInfo,
                                  &mac_req_ptr->cfg_data.rach_cfg->l1_rach);

  /* MAC DL configuration information */
  TF_ASSERT(mac_req_ptr->cfg_data.dl_cfg->ta_timer_cfg_included == TRUE);

  /* Get the timeAlignmentTimerCommon based on the OTA message */
  switch (sib2_ptr->timeAlignmentTimerCommon) 
  {
    case lte_rrc_osys_TimeAlignmentTimer_sf500:
      ta_timer = 500;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf750:
      ta_timer = 750;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf1280:
      ta_timer = 1280;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf1920:
      ta_timer = 1920;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf2560:
      ta_timer = 2560;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf5120:
      ta_timer = 5120;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf10240:
      ta_timer = 10240;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_infinity:
      ta_timer = LTE_OTA_IE_INFINITY_UINT16;
      break;
    default:
      TF_ASSERT(FALSE);
  }
  /* Check if the determined timeAlignmentTimerCommon value is what is passed to MAC */
  TF_ASSERT(mac_req_ptr->cfg_data.dl_cfg->ta_timer == ta_timer);

  /* MAC defult UL Configuration */
  TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->bsr_cfg_included == TRUE);
  TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->bsr_timer == LTE_OTA_IE_INFINITY_UINT16);
  TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->retxed_bsr_cfg_included == TRUE);
  TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->retxed_bsr_timer == LTE_RRC_LLC_DEFAULT_RETX_BSR_TIMER_VAL);
  TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->sps_release_cfg_included == TRUE);
  TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->sps_num_tx_release == 0);
  TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->sr_cfg_included == TRUE);
  TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->sr_resource_present == FALSE);

  /* If current cell is TDD, check if TDD UL\DL config is passed to MAC */
  if (lte_rrc_llc_test_common_data.tdd_param.tdd_cfg)
  {
    TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->tdd_ul_dl_cfg_included == TRUE);
    TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->tdd_ul_dl_cfg 
              == lte_rrc_llc_test_common_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
                  systemInformationBlockType1->tdd_Config.subframeAssignment);
  }
  else
  {
    TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->tdd_ul_dl_cfg_included == FALSE);
  }

  TF_MSG("\n MAC Common config verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mac_dedicated_ul_cfg

===========================================================================*/
/*!
    @brief
    This function tests all the MAC dedicated UL Config parameters.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mac_dedicated_ul_cfg
(
  lte_rrc_osys_MAC_MainConfig *ota_mac_cfg_ptr,
  lte_mac_ul_cfg_s*       ul_cfg_ptr
)
{
  uint16 bsr_timer = 0, retx_bsr_timer = 0;

  if (ota_mac_cfg_ptr->ul_SCH_Config.m.periodicBSR_TimerPresent)
  {
    /* Get the periodicBSR_Timer based on the OTA message */
    switch(ota_mac_cfg_ptr->ul_SCH_Config.periodicBSR_Timer)
    {
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf5:
        bsr_timer = 5;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf10:
        bsr_timer = 10;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf16:
        bsr_timer = 16;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf20:
        bsr_timer = 20;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf32:
        bsr_timer = 32;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf40:
        bsr_timer = 40;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf64:
        bsr_timer = 64;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf80:
        bsr_timer = 80;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf128:
        bsr_timer = 128;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf160:
        bsr_timer = 160;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf320:
        bsr_timer = 320;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf640:
        bsr_timer = 640;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf1280:
        bsr_timer = 1280;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_sf2560:
        bsr_timer = 2560;
        break;
      case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_periodicBSR_Timer_infinity:
        bsr_timer = LTE_OTA_IE_INFINITY_UINT16;
        break;
      default:  
        /* If the periodicBSR_Timer value is invalid */
        bsr_timer = 0;
    }

    if (bsr_timer != 0)
    {
      TF_ASSERT(ul_cfg_ptr->bsr_cfg_included == TRUE);
      /* Check if the determined periodicBSR_Timer value is what is passed to MAC */
      TF_ASSERT(ul_cfg_ptr->bsr_timer == bsr_timer);
    }
    else
    {
      /* If periodicBSR_Timer is invalid, it should not be passed to MAC */
      TF_ASSERT(ul_cfg_ptr->bsr_cfg_included == FALSE);
    }
  }
  else
  {
    TF_ASSERT(ul_cfg_ptr->bsr_cfg_included == FALSE);
  }

  TF_ASSERT(ul_cfg_ptr->retxed_bsr_cfg_included == TRUE);

  /* Get the retxBSR_Timer based on the OTA message */
  switch (ota_mac_cfg_ptr->ul_SCH_Config.retxBSR_Timer) 
  {
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_retxBSR_Timer_sf320:
      retx_bsr_timer = 320;
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_retxBSR_Timer_sf640:
      retx_bsr_timer = 640;
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_retxBSR_Timer_sf1280:
      retx_bsr_timer = 1280;
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_retxBSR_Timer_sf2560:
      retx_bsr_timer = 2560;
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_retxBSR_Timer_sf5120:
      retx_bsr_timer = 5120;
      break;
    case lte_rrc_osys_MAC_MainConfig_ul_SCH_Config_retxBSR_Timer_sf10240:
      retx_bsr_timer = 10240;
      break;
    default:  
      TF_ASSERT(FALSE);
  }

  /* Check if the determined retx_bsr_timer value is what is passed to MAC */
  TF_ASSERT(ul_cfg_ptr->retxed_bsr_timer == retx_bsr_timer);

}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_lc_cfg

===========================================================================*/
/*!
    @brief
    This function tests logical channel config parameters. 

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_lc_cfg
(
  lte_rrc_osys_LogicalChannelConfig *ota_log_chan_cfg_ptr,
  lte_mac_lcqos_cfg_s   *lc_cfg_ptr
)
{
  uint16 pbr = 0, bucket_size_duration = 0;
  if (ota_log_chan_cfg_ptr->ul_SpecificParameters.m.logicalChannelGroupPresent)
  {
    TF_ASSERT(lc_cfg_ptr->lcgroup_present == TRUE);
    TF_ASSERT(lc_cfg_ptr->lcgroup == ota_log_chan_cfg_ptr->ul_SpecificParameters.logicalChannelGroup);
  }
  else
  {
    TF_ASSERT(lc_cfg_ptr->lcgroup_present == FALSE);
  }

  /* Get the pbr based on the OTA message */
  switch (ota_log_chan_cfg_ptr->ul_SpecificParameters.prioritisedBitRate) 
  {
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_prioritisedBitRate_kBps0:
      pbr = 0;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_prioritisedBitRate_kBps8:
      pbr = 8;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_prioritisedBitRate_kBps16:
      pbr = 16;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_prioritisedBitRate_kBps32:
      pbr = 32;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_prioritisedBitRate_kBps64:
      pbr = 64;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_prioritisedBitRate_kBps128:
      pbr = 128;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_prioritisedBitRate_kBps256:
      pbr = 256;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_prioritisedBitRate_infinity:
      pbr = LTE_OTA_IE_INFINITY_UINT16;
      break;
    default:
      TF_ASSERT(FALSE);
  }

  /* Get the bucketSizeDuration based on the OTA message */
  switch (ota_log_chan_cfg_ptr->ul_SpecificParameters.bucketSizeDuration) 
  {
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_bucketSizeDuration_ms50:
      bucket_size_duration = 50;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_bucketSizeDuration_ms100:
      bucket_size_duration = 100;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_bucketSizeDuration_ms150:
      bucket_size_duration = 150;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_bucketSizeDuration_ms300:
      bucket_size_duration = 300;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_bucketSizeDuration_ms500:
      bucket_size_duration = 500;
      break;
    case lte_rrc_osys_LogicalChannelConfig_ul_SpecificParameters_bucketSizeDuration_ms1000:
      bucket_size_duration = 1000;
      break;
    default:
      TF_ASSERT(FALSE);
  }

  /* Check if the determined pbr value is what is passed to MAC */
  TF_ASSERT(lc_cfg_ptr->pbr == pbr);
  TF_ASSERT(lc_cfg_ptr->token_bucket_size == (uint32) (pbr * bucket_size_duration));
  TF_ASSERT(lc_cfg_ptr->priority == ota_log_chan_cfg_ptr->ul_SpecificParameters.priority);

  if ((ota_log_chan_cfg_ptr->m.logicalChannelSR_Mask_r9Present) &&
      (ota_log_chan_cfg_ptr->logicalChannelSR_Mask_r9 
       == lte_rrc_osys_LogicalChannelConfig_logicalChannelSR_Mask_r9_setup))
  {
    TF_ASSERT(lc_cfg_ptr->sr_mask_configured == TRUE);
  }
  else
  {
    TF_ASSERT(lc_cfg_ptr->sr_mask_configured == FALSE);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_stored_idx_for_rb

===========================================================================*/
/*!
    @brief
    This function returns the stored rb_idx of an established RB. The function iterates
    through the test DB to find the rb_idx for the RB.

    @return
    uint8

 */
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_stored_idx_for_rb
(
  lte_rb_cfg_idx_t rb_cfg_idx
)
{
  uint8 i = 0, idx = LTE_MAX_ACTIVE_RB;
  for (i = 0;i < LTE_MAX_ACTIVE_RB; i++)
  {
    if ((lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx == rb_cfg_idx) &&
        (lte_rrc_llc_test_common_data.rb_info[i].rb_direction != LTE_RRC_LLC_RB_NOT_ESTABLISHED))
    {
      // RB is found in the stored test DB, break here
      idx = i;
      break;
    }
  }
  return idx;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rb_id_is_active

===========================================================================*/
/*!
    @brief
    This function determines if the passed rb_id is active.

    @return
    boolean

 */
/*=========================================================================*/
boolean lte_rrc_llc_test_rb_id_is_active
(
  lte_rb_id_t rb_id,
  uint8 rb_direction
)
{
  uint8 i = 0;

  for (i = 0;i < lte_rrc_llc_test_common_data.num_active_rb; i++)
  {
    if ((lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx == rb_id) &&
        (lte_rrc_llc_test_common_data.rb_info[i].rb_direction & rb_direction))
    {
      // EPS id is found in the stored test DB
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_srb_modified_in_ota_msg

===========================================================================*/
/*!
    @brief
    This function checks if a particular SRB is being modified in the OTA msg

    @return
    boolean, TRUE if the RB is being modified in the OTA msg, FALSE otherwise.

 */
/*=========================================================================*/
boolean lte_rrc_llc_test_srb_modified_in_ota_msg
(
  lte_rrc_osys_SRB_ToAddModList srb_list,
  lte_rb_cfg_idx_t rb_cfg_idx
)
{
  lte_rb_id_t rb_id = rb_cfg_idx - LTE_MAX_RB;
  uint8 rb_cnt = 0;

  while (rb_cnt < srb_list.n)
  {
    if (srb_list.elem[rb_cnt].srb_Identity == rb_id)
    {
      return TRUE;
    }
    rb_cnt++;
  }

  return FALSE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_drb_modified_in_ota_msg

===========================================================================*/
/*!
    @brief
    This function checks if a particular DRB is being modified in the OTA msg

    @return
    boolean, TRUE if the RB is being modified in the OTA msg, FALSE otherwise.

 */
/*=========================================================================*/
boolean lte_rrc_llc_test_drb_modified_in_ota_msg
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_rb_cfg_idx_t rb_cfg_idx
)
{
  lte_rrc_osys_DRB_ToAddModList drb_list;
  uint8 cnt = 0;

  if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent == 0)
  {
    return FALSE;
  }

  drb_list = radio_resrc_cfg_ptr->drb_ToAddModList;
  while (cnt < drb_list.n)
  {
    if (drb_list.elem[cnt].drb_Identity == rb_cfg_idx)
    {
      return TRUE;
    }
    cnt++;
  }

  return FALSE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_drb_pdcp_cfg_modified_in_ota_msg

===========================================================================*/
/*!
    @brief
    This function checks if the PDCP config of a particular DRB is being 
    modified in the OTA msg

    @return
    boolean, TRUE if the PDCP config of the RB is being modified in the OTA msg, FALSE otherwise.

 */
/*=========================================================================*/
boolean lte_rrc_llc_test_drb_pdcp_cfg_modified_in_ota_msg
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_rb_cfg_idx_t rb_cfg_idx
)
{
  lte_rrc_osys_DRB_ToAddModList drb_list;
  uint8 cnt = 0;

  if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent == 0)
  {
    return FALSE;
  }

  drb_list = radio_resrc_cfg_ptr->drb_ToAddModList;
  while (cnt < drb_list.n)
  {
    if ((drb_list.elem[cnt].drb_Identity == rb_cfg_idx) &&
        (drb_list.elem[cnt].m.pdcp_ConfigPresent))
    {
      return TRUE;
    }
    cnt++;
  }

  return FALSE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_drb_released_in_ota_msg

===========================================================================*/
/*!
    @brief
    This function checks if a particular DRB is being released in the OTA msg

    @return
    boolean, TRUE if the RB is being released in the OTA msg, FALSE otherwise.

 */
/*=========================================================================*/
boolean lte_rrc_llc_test_drb_released_in_ota_msg
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_rb_cfg_idx_t rb_cfg_idx
)
{
  lte_rrc_osys_DRB_ToReleaseList drb_rel_list;
  lte_rrc_osys_DRB_ToAddModList drb_add_list;
  uint8 cnt = 0;
  boolean drb_released = FALSE;

  if (rb_cfg_idx > LTE_MAX_RB)
  {
    return drb_released;
  }

  if (radio_resrc_cfg_ptr->m.drb_ToReleaseListPresent == 0)
  {
    /* For full configuration, if the passed DRB id is not present in DRBToAddModifiedList
      then it will be released */
    if (lte_rrc_llc_test_common_data.full_config)
    {
      drb_add_list = radio_resrc_cfg_ptr->drb_ToAddModList;
      drb_released = TRUE;
      cnt = 0;
      while (cnt < drb_add_list.n)
      {
        if (drb_add_list.elem[cnt].drb_Identity == rb_cfg_idx)
        {
          drb_released = FALSE;
          break;
        }
        cnt++;
      }
    }
  }
  else
  {
    drb_rel_list = radio_resrc_cfg_ptr->drb_ToReleaseList;
    while (cnt < drb_rel_list.n)
    {
      if (drb_rel_list.elem[cnt] == rb_cfg_idx)
      {
        drb_released = TRUE;
        break;
      }
      cnt++;
    }
  }

  return drb_released;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mac_lc_info

===========================================================================*/
/*!
    @brief
    This function tests MAC LC info. If logical channel info is present, the
    function returns TRUE.

    @return
    boolean, TRUE if logical channel info present, else FALSE.

 */
/*=========================================================================*/
boolean lte_rrc_llc_test_mac_lc_info
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_mac_lc_info_cfg_s   *lc_info_ptr
)
{
  lte_rrc_osys_SRB_ToAddModList srb_list;
  lte_rrc_osys_DRB_ToAddModList drb_add_list;
  lte_rrc_osys_DRB_ToReleaseList drb_rel_list;
  uint8 srb_count = 0, ul_drb_count = 0, i = 0, lc_id = 0, loop_count = 0, del_ul_rb_count = 0, 
    srb_idx = 0, stored_rb_idx = 0;
  boolean ret_value = FALSE;

  /* Verify RRC Connection release scenario, when both SRBs and DRBs are released */
  if (radio_resrc_cfg_ptr == NULL)
  {
    /* For the RBs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if (lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION)
      {
        TF_ASSERT(lc_info_ptr->deleted_lc_list[del_ul_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].lc_id);
        del_ul_rb_count++;
      }
    }
    TF_ASSERT(lc_info_ptr->num_deleted_lc == del_ul_rb_count);
    TF_MSG("\n Connection release: MAC LC info verified successfully \n");

    /* All parameters for connection release have been tested, just return TRUE
      implying LC info changes are being passed to MAC */
    return TRUE;
  }

  /* Verify DRB release during full config scenario */
  if (lte_rrc_llc_test_common_data.full_config)
  {
    /* For the LCs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if ((lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION) &&
          (lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx < LTE_MAX_RB))
      {
        TF_ASSERT(lc_info_ptr->deleted_lc_list[del_ul_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].lc_id);
        del_ul_rb_count++;
      }
    }
    TF_ASSERT(lc_info_ptr->num_deleted_lc == del_ul_rb_count);
    TF_MSG("\n Verifying MAC LC info, full config scenario verified successfully \n");
  }
  /* If any DRB is being released, test the released lc list in the request sent to MAC */
  else if (radio_resrc_cfg_ptr->m.drb_ToReleaseListPresent)
  {
    /* Released LCs should correspond to the DRBToReleaseList IE in the OTA message */
    drb_rel_list = radio_resrc_cfg_ptr->drb_ToReleaseList;
    while (loop_count < drb_rel_list.n)
    {
      stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_rel_list.elem[loop_count]);
      /* If the DRB is not found, ignore it and move to the next RB in the list */
      if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
      {
        loop_count++;
        continue;
      }
      /* If the DRB being released is not a DL only RB, then it should be present
        in the released lc list sent to MAC */
      if (lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION)
      {
        TF_ASSERT(lc_info_ptr->deleted_lc_list[ul_drb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].lc_id);
        ul_drb_count++;
      }
      
      /* Iterate through the DRBToReleaseList */
      loop_count++;
    }
    /* If there was atleast one RB (non-DL-only RB), with logical channel parameters, being released */
    if (ul_drb_count > 0)
    {
      ret_value = TRUE;
    }
    TF_ASSERT(lc_info_ptr->num_deleted_lc == ul_drb_count);
    TF_MSG("\n Verifying MAC LC info, UL DRB release verified successfully \n");
  }

  loop_count = 0;
  /* If SRBtoAddModify list contains logical channel config for any SRBs, then
    verify the changed parameters being sent to MAC for that SRB */
  if (radio_resrc_cfg_ptr->m.srb_ToAddModListPresent)
  {
    srb_list = radio_resrc_cfg_ptr->srb_ToAddModList;
    /* Iterate through the SRBToAddModifiedList */
    while (loop_count < srb_list.n)
    {
      srb_idx = srb_list.elem[loop_count].srb_Identity + LTE_MAX_RB;
      /* If logical channel config info is present or if it is Rel 9 full config, 
        look at the SRB */
      if ((srb_list.elem[loop_count].m.logicalChannelConfigPresent) || 
          (lte_rrc_llc_test_common_data.full_config))
      {
        /* Verify if LC id is same as SRB id */
        lc_id = srb_list.elem[loop_count].srb_Identity;
        TF_ASSERT(lc_info_ptr->addmod_lc_list[srb_count] == lc_id);
        if (srb_list.elem[loop_count].logicalChannelConfig.t 
            == T_lte_rrc_osys_SRB_ToAddMod_logicalChannelConfig_explicitValue)
        {
          if (srb_list.elem[loop_count].logicalChannelConfig.u.explicitValue->m.ul_SpecificParametersPresent)
          {
            /* Verify the logical channel parameters for the specific LC */
            lte_rrc_llc_test_lc_cfg(srb_list.elem[loop_count].logicalChannelConfig.u.explicitValue,
                                    &lc_info_ptr->lc_list[lc_id]);
          }
        }
        else if ((srb_list.elem[loop_count].logicalChannelConfig.t 
                  == T_lte_rrc_osys_SRB_ToAddMod_logicalChannelConfig_defaultValue) ||
                 (lte_rrc_llc_test_common_data.full_config))
        {
          TF_ASSERT(lc_info_ptr->lc_list[lc_id].lcgroup_present == TRUE);
          TF_ASSERT(lc_info_ptr->lc_list[lc_id].lcgroup == 0);
          TF_ASSERT(lc_info_ptr->lc_list[lc_id].pbr == LTE_OTA_IE_INFINITY_UINT16);
          if (srb_list.elem[loop_count].srb_Identity == LTE_RRC_DCCH_SRB1)
          {
            TF_ASSERT(lc_info_ptr->lc_list[lc_id].priority == 1);
          }
          else if (srb_list.elem[loop_count].srb_Identity == LTE_RRC_DCCH_SRB2)
          {
            TF_ASSERT(lc_info_ptr->lc_list[lc_id].priority == 3);
          }
          else
          {
            TF_ASSERT(FALSE);
          }
        }
        else
        {
          TF_ASSERT(FALSE);
        }
        ret_value = TRUE;
        srb_count++;
      }
      loop_count++;
    }
    TF_ASSERT(srb_count <= 2);
    TF_MSG("\n Verifying MAC LC info, SRB list verified successfully \n");
  }

  /* ul_drb_count set to srb_count since lc info of DRBs starts at the index following the SRBs */
  ul_drb_count = srb_count;
  loop_count = 0;
  /* If DRBtoAddModify list contains logical channel config for any DRBs, then
    verify the changed parameters being sent to MAC for that DRB */
  if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent)
  {
    drb_add_list = radio_resrc_cfg_ptr->drb_ToAddModList;
    /* Iterate through the DRBToAddModifiedList */
    while (loop_count < drb_add_list.n)
    {
      /* Get the stored idx of the DRB from test DB, to verify DRB modify case */
      stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_add_list.elem[loop_count].drb_Identity);

      if ((drb_add_list.elem[loop_count].m.logicalChannelConfigPresent) &&
          (drb_add_list.elem[loop_count].rlc_Config.t != T_lte_rrc_osys_RLC_Config_um_Uni_Directional_DL))
      {
        /* If verifying DRB setup case */
        if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
        {
          /* Verify DRB Setup case */
          lc_id = drb_add_list.elem[loop_count].logicalChannelIdentity;
          TF_ASSERT(lc_info_ptr->addmod_lc_list[ul_drb_count] == lc_id);
        }
        else
        {
          /* If verifying DRB modify case */
          lc_id = lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].lc_id;
        }

        /* Verify the logical channel parameters for the specific LC */
        lte_rrc_llc_test_lc_cfg(&drb_add_list.elem[loop_count].logicalChannelConfig,
                                &lc_info_ptr->lc_list[lc_id]);
        ul_drb_count++;
        ret_value = TRUE;
      }
      loop_count++;
    }
    /* Verify if total number of LCs added/modified includes LCs corresponding to both SRBs and DRBs.
      At this point ul_drb_count includes SRBs */
    TF_ASSERT(lc_info_ptr->num_addmod_lc == ul_drb_count);
    TF_MSG("\n Verifying MAC LC info, DRB list verified successfully \n");
  }
  TF_MSG("\n MAC LC info verified successfully \n");
  return ret_value;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mac_mobility_cfg

===========================================================================*/
/*!
    @brief
    This function tests all the parameters in Mobility Control IE passed to MAC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mac_mobility_cfg
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr,
  uint8 *cfg_mask_ptr
)
{
  lte_rrc_osys_MobilityControlInfo *mobility_info_ptr = &lte_rrc_llc_test_common_data.
    dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo;

  /* During HO, CRNTI is always present and PRACH config is always signaled */
  *cfg_mask_ptr |= (LTE_MAC_C_RNTI_CFG_BM | LTE_MAC_RACH_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.crnti_cfg 
            == (uint16) lte_rrc_osys_asn1_bitstring_to_int(sizeof(lte_rrc_osys_C_RNTI),
                                                           &mobility_info_ptr->newUE_Identity));
  /* Verify PRACH config */
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.rach_cfg->l1_rach.log_root_seq_index 
            == mobility_info_ptr->radioResourceConfigCommon.prach_Config.rootSequenceIndex);

  if (mobility_info_ptr->radioResourceConfigCommon.prach_Config.m.prach_ConfigInfoPresent)
  {
    lte_rrc_llc_test_prach_cfg_info(&mobility_info_ptr->radioResourceConfigCommon.
                                    prach_Config.prach_ConfigInfo,
                                    &mac_cfg_req_ptr->cfg_data.rach_cfg->l1_rach);
  }

  /* If uplinkPowerControl is signaled, check delta preamble msg3 value passed to MAC */
  if (mobility_info_ptr->radioResourceConfigCommon.m.uplinkPowerControlCommonPresent)
  {
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.rach_cfg->delta_preamble_msg3 
              == mobility_info_ptr->radioResourceConfigCommon.uplinkPowerControlCommon.deltaPreambleMsg3 * 2);
  }

  /* Check p_max value passed to MAC */
  if (mobility_info_ptr->radioResourceConfigCommon.m.p_MaxPresent)
  {
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.rach_cfg->p_max 
              == mobility_info_ptr->radioResourceConfigCommon.p_Max);
  }
  else
  {
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.rach_cfg->p_max == LTE_DEFAULT_PMAX);
  }

  if (mobility_info_ptr->radioResourceConfigCommon.m.rach_ConfigCommonPresent)
  {
    lte_rrc_llc_test_rach_cfg(&mobility_info_ptr->radioResourceConfigCommon.rach_ConfigCommon,
                              mac_cfg_req_ptr->cfg_data.rach_cfg);
  }

  /* if SR is configured previously, check if it is released */
  if ((lte_rrc_llc_test_common_data.cqi_srs_sr_info.sr_configured) &&
      ((radio_resrc_cfg_ptr->physicalConfigDedicated.m.schedulingRequestConfigPresent) == 0))
  {
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_cfg_included == TRUE);
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_resource_present == FALSE);
    lte_rrc_llc_test_common_data.cqi_srs_sr_info.sr_configured = FALSE;
    *cfg_mask_ptr |= LTE_MAC_UL_CFG_BM;
  }
  TF_MSG("\n MAC mobility config verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mac_ta_timer_cfg

===========================================================================*/
/*!
    @brief
    This function tests TA timer value passed to MAC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mac_ta_timer_cfg
(
  lte_rrc_osys_MAC_MainConfig *ota_mac_cfg_ptr,
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr
)
{
  uint16 ta_timer = 0;

  /* Verify TA timer is always included */
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.dl_cfg->ta_timer_cfg_included == TRUE);

  /* Get the TA timer value based on the OTA message */
  switch (ota_mac_cfg_ptr->timeAlignmentTimerDedicated) 
  {
    case lte_rrc_osys_TimeAlignmentTimer_sf500:
      ta_timer = 500;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf750:
      ta_timer = 750;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf1280:
      ta_timer = 1280;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf1920:
      ta_timer = 1920;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf2560:
      ta_timer = 2560;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf5120:
      ta_timer = 5120;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_sf10240:
      ta_timer = 10240;
      break;
    case lte_rrc_osys_TimeAlignmentTimer_infinity:
      ta_timer = LTE_OTA_IE_INFINITY_UINT16;
      break;
    default:
      TF_ASSERT(FALSE);
  }
  /* Check if the determined TA timer value is what is passed to MAC */
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.dl_cfg->ta_timer == ta_timer);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mac_sr_cfg

===========================================================================*/
/*!
    @brief
    This function tests SR config passed to MAC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mac_sr_cfg
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr,
  uint8 *cfg_mask_ptr
)
{
  lte_rrc_osys_SchedulingRequestConfig ota_sr_config;

  /* If SR is being configured in the current OTA message then sr config should
    be included and SR resource should be set to present */
  if (radio_resrc_cfg_ptr->physicalConfigDedicated.m.schedulingRequestConfigPresent)
  {
    ota_sr_config = radio_resrc_cfg_ptr->physicalConfigDedicated.schedulingRequestConfig;
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_cfg_included == TRUE);
    if (radio_resrc_cfg_ptr->physicalConfigDedicated.schedulingRequestConfig.
        t == T_lte_rrc_osys_SchedulingRequestConfig_setup)
    {
      TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_resource_present == TRUE);

      // Verify SR periodicity
      if (ota_sr_config.u.setup->sr_ConfigIndex < 5)
      {
        TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_periodicity == 5);
      }
      else if (ota_sr_config.u.setup->sr_ConfigIndex < 15)
      {
        TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_periodicity == 10);
      }
      else if (ota_sr_config.u.setup->sr_ConfigIndex < 35)
      {
        TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_periodicity == 20);
      }
      else if (ota_sr_config.u.setup->sr_ConfigIndex < 75)
      {
        TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_periodicity == 40);
      }
      else if (ota_sr_config.u.setup->sr_ConfigIndex < 155)
      {
        TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_periodicity == 80);
      }
      else if (ota_sr_config.u.setup->sr_ConfigIndex < 157)
      {
        TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_periodicity == 2);
      }
      else if (ota_sr_config.u.setup->sr_ConfigIndex == 157)
      {
        TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_periodicity == 1);
      }
    }
    else
    {
      TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_resource_present == FALSE);
    }
    *cfg_mask_ptr |= LTE_MAC_UL_CFG_BM;
  }
  TF_MSG("\n MAC SR config verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mac_sps_cfg

===========================================================================*/
/*!
    @brief
    This function tests SPS config passed to MAC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mac_sps_cfg
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr,
  uint8 *cfg_mask_ptr
)
{
  uint16 sps_num_tx_release = 0;

  /* Verify the SPS parameters relevant to MAC */
  if ((radio_resrc_cfg_ptr->m.sps_ConfigPresent) &&
      (radio_resrc_cfg_ptr->sps_Config.m.sps_ConfigULPresent) &&
      (radio_resrc_cfg_ptr->sps_Config.sps_ConfigUL.t == T_lte_rrc_osys_SPS_ConfigUL_setup))
  {
    /* Get the implicitReleaseAfter value based on the OTA message */
    switch(radio_resrc_cfg_ptr->sps_Config.sps_ConfigUL.u.setup->implicitReleaseAfter)
    {
      case lte_rrc_osys_SPS_ConfigUL_setup_implicitReleaseAfter_e2:
        sps_num_tx_release = 2;
        break;
      case lte_rrc_osys_SPS_ConfigUL_setup_implicitReleaseAfter_e3:
        sps_num_tx_release = 3;
        break;
      case lte_rrc_osys_SPS_ConfigUL_setup_implicitReleaseAfter_e4:
        sps_num_tx_release = 4;
        break;
      case lte_rrc_osys_SPS_ConfigUL_setup_implicitReleaseAfter_e8:
        sps_num_tx_release = 8;
        break;
      default:
        TF_ASSERT(FALSE);                    
    }
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sps_release_cfg_included == TRUE);
    /* Check if the determined implicitReleaseAfter value is what is passed to MAC */
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sps_num_tx_release == sps_num_tx_release);
    *cfg_mask_ptr |= LTE_MAC_UL_CFG_BM;
    TF_MSG("\n MAC SPS config verified successfully \n");
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mac_main_cfg

===========================================================================*/
/*!
    @brief
    This function tests mac_MainConfig parameters passed to MAC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mac_main_cfg
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr,
  uint8 *cfg_mask_ptr
)
{
  lte_rrc_osys_MAC_MainConfig *ota_mac_cfg_ptr = NULL;

  if (radio_resrc_cfg_ptr->mac_MainConfig.t 
      == T_lte_rrc_osys_RadioResourceConfigDedicated_mac_MainConfig_defaultValue)
  {
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->bsr_cfg_included == TRUE);
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->bsr_timer == LTE_OTA_IE_INFINITY_UINT16);
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->retxed_bsr_cfg_included == TRUE);
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->retxed_bsr_timer == LTE_RRC_LLC_DEFAULT_RETX_BSR_TIMER_VAL);
    *cfg_mask_ptr |= LTE_MAC_UL_CFG_BM;
  }
  else if (radio_resrc_cfg_ptr->mac_MainConfig.t 
           == T_lte_rrc_osys_RadioResourceConfigDedicated_mac_MainConfig_explicitValue)
  {
    ota_mac_cfg_ptr = radio_resrc_cfg_ptr->mac_MainConfig.u.explicitValue;
    *cfg_mask_ptr |= LTE_MAC_DL_CFG_BM;

    /* Verify ta_timer config passed to MAC */
    lte_rrc_llc_test_mac_ta_timer_cfg(ota_mac_cfg_ptr, mac_cfg_req_ptr);

    if (ota_mac_cfg_ptr->m.ul_SCH_ConfigPresent)
    {
      lte_rrc_llc_test_mac_dedicated_ul_cfg(ota_mac_cfg_ptr, mac_cfg_req_ptr->cfg_data.ul_cfg);
      *cfg_mask_ptr |= LTE_MAC_UL_CFG_BM;
    }
  }
  else
  {
    TF_ASSERT(FALSE);
  }
  TF_MSG("\n MAC main config verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mac_dedicated_cfg

===========================================================================*/
/*!
    @brief
    This function tests all the MAC dedicated config parameters.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mac_dedicated_cfg
(
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr,
  lte_rrc_proc_e proc
)
{
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr = NULL;
  uint8 cfg_bitmask = 0;

  /* Get the appropriate radio resource config */
  radio_resrc_cfg_ptr = lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr(proc);

  /* Verify the relevant config based on the source proc */
  if (proc == LTE_RRC_PROC_CEP)
  {
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RECFG);
  }
  else if (proc == LTE_RRC_PROC_CONFIG)
  {
    if (lte_rrc_llc_test_common_data.ho_in_progress)
    {
      TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_HO);
      /* Test all mobility control IE parameters passed to MAC */
      lte_rrc_llc_test_mac_mobility_cfg(radio_resrc_cfg_ptr, mac_cfg_req_ptr, &cfg_bitmask);
    }
    else
    {
      TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RECFG);
    }
  }
  else if ((proc == LTE_RRC_PROC_CRP) || (proc == LTE_RRC_PROC_CONTROLLER))
  {
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RELEASE);
    if (lte_rrc_llc_test_mac_lc_info(NULL, &mac_cfg_req_ptr->cfg_data.lc_info_cfg))
    {
      cfg_bitmask |= LTE_MAC_LC_INFO_CFG_BM;
    }
    /* Verify the MAC Config request bitmask */
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == cfg_bitmask);

    return;
  }
  else if (proc == LTE_RRC_PROC_CRE)
  {
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RECFG);
  }
  else
  {
    TF_ASSERT(FALSE);
  }

  if (lte_rrc_llc_test_mac_lc_info(radio_resrc_cfg_ptr, &mac_cfg_req_ptr->cfg_data.lc_info_cfg))
  {
    cfg_bitmask |= LTE_MAC_LC_INFO_CFG_BM;
  }

  if (radio_resrc_cfg_ptr->m.mac_MainConfigPresent)
  {
    lte_rrc_llc_test_mac_main_cfg(radio_resrc_cfg_ptr, mac_cfg_req_ptr, &cfg_bitmask);
  }

  if (radio_resrc_cfg_ptr->m.physicalConfigDedicatedPresent)
  {
    /* Verify the SR parameters relevant to MAC */
    lte_rrc_llc_test_mac_sr_cfg(radio_resrc_cfg_ptr, mac_cfg_req_ptr, &cfg_bitmask);
  }
  else if ((proc == LTE_RRC_PROC_CEP) || (proc == LTE_RRC_PROC_CRE))
  {
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_cfg_included == TRUE);
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.ul_cfg->sr_resource_present == FALSE);
    cfg_bitmask |= LTE_MAC_UL_CFG_BM;
  }

  /* Verify the SPS parameters relevant to MAC */
  lte_rrc_llc_test_mac_sps_cfg(radio_resrc_cfg_ptr, mac_cfg_req_ptr, &cfg_bitmask);

  /* */
  if (lte_rrc_llc_test_common_data.tdd_param.tdd_cfg)
  //if (TRUE == mac_cfg_req_ptr->cfg_data.ul_cfg->tdd_ul_dl_cfg_included)
  {
    cfg_bitmask |= LTE_MAC_UL_CFG_BM;
  }

  /* Verify the MAC Config request bitmask */
  if (lte_rrc_llc_test_common_data.full_config)
  {
    if (lte_rrc_llc_test_common_data.reconfig_after_cre)
    {
      TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_DL_CFG_BM   |   
                                                          LTE_MAC_UL_CFG_BM   |   
                                                          LTE_MAC_LC_INFO_CFG_BM));
    }
    else
    {
      TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_DL_CFG_BM   |   
                                                          LTE_MAC_UL_CFG_BM   |   
                                                          LTE_MAC_RACH_CFG_BM |
                                                          LTE_MAC_C_RNTI_CFG_BM |
                                                          LTE_MAC_LC_INFO_CFG_BM));
    }
  }
  else
  {
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == cfg_bitmask);
  }
  
  TF_MSG("\n MAC dedicated config verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mac_embms_cfg

===========================================================================*/
/*!
    @brief
    This function tests all the MAC eMBMS config parameters.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mac_embms_cfg
(
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr
)
{
  lte_rrc_llc_mrb_info_s *mrb_info_ptr = &lte_rrc_llc_test_common_data.oc_mrb_info;
  uint8 i = 0;

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_EMBMS_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_EMBMS);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.embms_cfg.num_active_lc == mrb_info_ptr->num_active_mrb);
  for (i = 0; i < mrb_info_ptr->num_active_mrb; i++)
  {
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.embms_cfg.active_lc_list[i].
              area_id == mrb_info_ptr->mrb_info[i].area_id);
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.embms_cfg.active_lc_list[i].
              pmch_id == mrb_info_ptr->mrb_info[i].pmch_id);
    TF_ASSERT(mac_cfg_req_ptr->cfg_data.embms_cfg.active_lc_list[i].
              lc_id == mrb_info_ptr->mrb_info[i].lc_id);
  }

  TF_MSG("\n MAC eMBMS config verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying MAC config request
    parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf
(
  boolean common_config,
  lte_rrc_osys_SystemInformationBlockType2 *sib2_ptr,
  lte_rrc_proc_e proc
)
{
  lte_mac_cfg_req_msg_s *mac_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  uint32 buf_len = 0;

  lte_rrc_utf_get_next_ext_msg((byte **) &mac_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  TF_MSG("\nLLC: Rcvd MAC_CFG_REQ \n");

  /* LLC should be in WAIT_FOR_MAC_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);

  if (common_config)
  {
    if (sib2_ptr == NULL)
    {
      sib2_ptr = lte_rrc_llc_test_common_data.bcch_dl_sch_sib2_ptr->message.u.c1->u.
      systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib2;
    }
    lte_rrc_llc_test_mac_common_cfg(sib2_ptr, mac_req_ptr);
  }
  else
  {
    lte_rrc_llc_test_mac_dedicated_cfg(mac_req_ptr, proc);
  }

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send MAC_CFG_CNF with sucess \n");
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf_embms

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying MAC config request
    parameters for eMBMS and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf_embms
(
  boolean cfg_for_release,
  boolean release_all_mrb,
  boolean release_all_rb
)
{
  lte_mac_cfg_req_msg_s *mac_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  uint32 buf_len = 0;

  lte_rrc_utf_get_next_ext_msg((byte **) &mac_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  TF_MSG("\nLLC: Rcvd MAC_CFG_REQ \n");

  /* LLC should be in WAIT_FOR_MAC_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);

  TF_ASSERT(mac_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_EMBMS_CFG_BM);

  if (release_all_rb)
  {
    TF_ASSERT(mac_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RELEASE_ALL);
  }
  else if (release_all_mrb)
  {
    TF_ASSERT(mac_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RELEASE_ALL_EMBMBS);
    TF_ASSERT(mac_req_ptr->cfg_data.embms_cfg.num_active_lc == 0);
  }
  else if (cfg_for_release)
  {
    TF_ASSERT(mac_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_EMBMS);
    TF_ASSERT(mac_req_ptr->cfg_data.embms_cfg.num_active_lc == 1);
  }
  else
  {
    TF_ASSERT(mac_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_EMBMS);
    lte_rrc_llc_test_mac_embms_cfg(mac_req_ptr);
  }

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send MAC_CFG_CNF with sucess \n");
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf

===========================================================================*/
/*!
    @brief
    This function performs verifies the MAC access abort request request and
    sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf
(
  void
)
{
  lte_mac_access_abort_req_msg_s *mac_access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s mac_access_abort_cnf_msg;
  uint32 buf_len = 0;

  /* Catch the ACCESS_ABORT_REQ to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_abort_req_ptr,&buf_len);
  /* Make sure the ACCESS_ABORT_REQ is valid */
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);

  /* Send ACCESS_ABORT_CNF to Config */
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&mac_access_abort_cnf_msg.hdr,LTE_MAC_ACCESS_ABORT_CNF);
  mac_access_abort_cnf_msg.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&mac_access_abort_cnf_msg, sizeof(mac_access_abort_cnf_msg));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_l2_cfg_reason

===========================================================================*/
/*!
    @brief
    This function verifies the passed L2 config reason based on the source proc.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_l2_cfg_reason
(
  lte_rrc_proc_e proc, /*!< RRC proc */
  lte_l2_cfg_reason_e cfg_reason  /*!< L2 cfg_reason to be verified */
)
{
  /* Verify the config type based on the source proc */
  if ((proc == LTE_RRC_PROC_CEP) || (proc == LTE_RRC_PROC_CRE))
  {
    TF_ASSERT(cfg_reason == LTE_L2_CFG_RECFG);
  }
  else if (proc == LTE_RRC_PROC_CONFIG)
  {
    if (lte_rrc_llc_test_common_data.ho_in_progress)
    {
      TF_ASSERT(cfg_reason == LTE_L2_CFG_HO);
    }
    else
    {
      TF_ASSERT(cfg_reason == LTE_L2_CFG_RECFG);
    }
  }
  else if ((proc == LTE_RRC_PROC_CRP) || (proc == LTE_RRC_PROC_CONTROLLER))
  {
    TF_ASSERT(cfg_reason == LTE_L2_CFG_RELEASE);
  }
  else
  {
    TF_ASSERT(FALSE);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_rb_direction_for_released_drb

===========================================================================*/
/*!
    @brief
    This function loops through DRBToReleasedList in the OTA message and 
    determines their rb_direction from the stored DRBs.

    @return
    uint8, If the RBs in the list contain DL only RBs which are being released 
    then returns LTE_RRC_LLC_RB_DL_DIRECTION, similar for UL.If both DL and UL RBs or 
    bidirectional RBs are present, returns LTE_RRC_LLC_RB_BI_DIRECTION. If no DRBs
    are being released, then returns LTE_RRC_LLC_RB_NOT_ESTABLISHED.

 */
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_rb_direction_for_released_drb
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr
)
{
  lte_rrc_osys_DRB_ToReleaseList drb_rel_list = radio_resrc_cfg_ptr->drb_ToReleaseList;
  uint8 rb_direction = LTE_RRC_LLC_RB_NOT_ESTABLISHED, stored_rb_idx = 0, cnt = 0;

  /* If DRBToReleasedList is present, loop through it to determine the RB direction of the 
    DRBs being released */
  if (radio_resrc_cfg_ptr->m.drb_ToReleaseListPresent)
  {
    while (cnt < drb_rel_list.n)
    {
      stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_rel_list.elem[cnt]);
      /* If the DRB is not found, ignore it and move to the next RB in the list */
      if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
      {
        cnt++;
      }

      if (lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].
          rb_direction == LTE_RRC_LLC_RB_BI_DIRECTION)
      {
        return LTE_RRC_LLC_RB_BI_DIRECTION;
      }
      else if (lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].
               rb_direction == LTE_RRC_LLC_RB_DL_DIRECTION)
      {
        rb_direction |= LTE_RRC_LLC_RB_DL_DIRECTION;
      }
      else if (lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].
               rb_direction == LTE_RRC_LLC_RB_UL_DIRECTION)
      {
        rb_direction |= LTE_RRC_LLC_RB_UL_DIRECTION;
      }
      cnt++;
    }
  }
  return rb_direction;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_rlc_entity_to_modify_from_ota_msg

===========================================================================*/
/*!
    @brief
    This function loops through SRB-AddMod and DRB-AddMod list in the OTA message and 
    determines if the SRBs/DRBs in the list contain RLC Config changes for UL/DL 
    only RBs or both.

    @return
    uint8, the rb_direction of the RBs whose RLC config is changed by this OTA msg. 
    Eg. If the RBs in the list contain DL only RBs whose RLC config has changed 
    then returns LTE_RRC_LLC_RB_DL_DIRECTION, similar for UL.If both DL and UL RBs or 
    bidirectional RBs are present, returns LTE_RRC_LLC_RB_BI_DIRECTION. If RLC config 
    is not signaled for any of the RBs, then returns LTE_RRC_LLC_RB_NOT_ESTABLISHED.

 */
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_rlc_entity_to_modify_from_ota_msg
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr
)
{
  lte_rrc_osys_SRB_ToAddModList srb_list = radio_resrc_cfg_ptr->srb_ToAddModList;
  lte_rrc_osys_DRB_ToAddModList drb_add_list = radio_resrc_cfg_ptr->drb_ToAddModList;
  uint8 rb_direction = LTE_RRC_LLC_RB_NOT_ESTABLISHED;
  uint8 rb_count = 0;

  /* If RLC config of SRBs are present, then return LTE_RRC_LLC_RB_BI_DIRECTION */
  if (radio_resrc_cfg_ptr->m.srb_ToAddModListPresent)
  {
    while (rb_count < srb_list.n)
    {
      if (srb_list.elem[rb_count].m.rlc_ConfigPresent)
      {
        return LTE_RRC_LLC_RB_BI_DIRECTION;
      }
      rb_count++;
    }
  }

  /* If DRBToAddModifiedList is present, loop through it to see if any of the 
    DRBs contain changes in RLC config */
  if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent)
  {
    while (rb_count < drb_add_list.n)
    {
      if (drb_add_list.elem[rb_count].m.rlc_ConfigPresent)
      {
        if ((drb_add_list.elem[rb_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_am) ||
            (drb_add_list.elem[rb_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Bi_Directional))
        {
          return LTE_RRC_LLC_RB_BI_DIRECTION;
        }
        else if (drb_add_list.elem[rb_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Uni_Directional_DL)
        {
          rb_direction |= LTE_RRC_LLC_RB_DL_DIRECTION;
        }
        else if (drb_add_list.elem[rb_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Uni_Directional_UL)
        {
          rb_direction |= LTE_RRC_LLC_RB_UL_DIRECTION;
        }
      }
      if (rb_direction == LTE_RRC_LLC_RB_BI_DIRECTION)
      {
        return rb_direction;
      }
      rb_count++;
    }
  }

  /* If DRBToReleasedList is present, loop through it to determine the RB direction of the 
    DRBs being released */
  rb_direction |= lte_rrc_llc_test_get_rb_direction_for_released_drb(radio_resrc_cfg_ptr);

  return rb_direction;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_rb_direction_of_suspended_rbs

===========================================================================*/
/*!
    @brief
    This function determines the direction of suspended RBs. This function is
    used while verifying the first reconfig after reestablishment.

    @return
    uint8, the rb_direction of the RBs that are suspended. Eg. If the suspended RBs 
    contain DL only RBs, then LTE_RRC_LLC_RB_DL_DIRECTION is returned, similar for UL.
    If both DL and UL RBs or bidirectional RBs are present, LTE_RRC_LLC_RB_BI_DIRECTION 
    is returned. Else, LTE_RRC_LLC_RB_NOT_ESTABLISHED is returned.

 */
/*=========================================================================*/
uint8 lte_rrc_llc_test_get_rb_direction_of_suspended_rbs
(
  void
)
{
  uint8 rb_direction = LTE_RRC_LLC_RB_NOT_ESTABLISHED;
  uint8 i = 0;

  for (i = 0; i < lte_rrc_llc_test_common_data.num_active_rb; i++)
  {
    /* Skip SRB1 since it is not suspended */
    if (lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx != LTE_RRC_LLC_SRB1_RB_CFG_IDX)
    {
      rb_direction |= lte_rrc_llc_test_common_data.rb_info[i].rb_direction;
      if (rb_direction == LTE_RRC_LLC_RB_BI_DIRECTION)
      {
        return rb_direction;
      }
    }
  }
  return rb_direction;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_drx_opt_req

===========================================================================*/
/*!
    @brief
    This function determines if a DRX optimization request needs to be sent to
    ML1. If so, it verifies the req sent by LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_drx_opt_req
(
  void
)
{
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr = NULL;
  lte_rrc_osys_DRB_ToAddModList drb_list;
  lte_rrc_osys_DRB_ToReleaseList drb_rel_list;
  lte_cphy_drx_opt_req_s *drx_opt_req_ptr = NULL;
  uint32 buf_len = 0;
  uint8 i = 0, num_active_rb = 0, stored_rb_idx;
  boolean um_rb_in_cc = FALSE, um_rb_add_by_ota = FALSE, um_rb_after_ota_rel = FALSE;
  rb_info_s rb_info[LTE_MAX_ACTIVE_RB];

  /* Determine if a UM DRB is part of current configuration */
  for (i = 0; i < lte_rrc_llc_test_common_data.num_active_rb; i++)
  {
    if (lte_rrc_llc_test_common_data.rb_info[i].rb_mode == LTE_RB_RLC_MODE_UM)
    {
      um_rb_in_cc = TRUE;
      break;
    }
  }

  /* Get the appropriate radio resource config */
  radio_resrc_cfg_ptr = lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr(LTE_RRC_PROC_CONFIG);

  /* Determine if UM RB is being added by OTA msg */
  if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent)
  {
    drb_list = radio_resrc_cfg_ptr->drb_ToAddModList;
    i = 0;
    while (i < drb_list.n)
    {
      if (drb_list.elem[i].m.rlc_ConfigPresent) 
      {
        if ((drb_list.elem[i].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Bi_Directional) ||
            (drb_list.elem[i].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Uni_Directional_UL) ||
            (drb_list.elem[i].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Uni_Directional_DL))
        {
          um_rb_add_by_ota = TRUE;
          break;
        }
      }
      i++;
    }
  }

  /* Make a copy of the current configuration of RBs */
  num_active_rb = lte_rrc_llc_test_common_data.num_active_rb;
  for (i = 0; i < lte_rrc_llc_test_common_data.num_active_rb; i++)
  {
    LTE_RRC_MEMCPY(&rb_info[i], sizeof(rb_info_s),
                   &lte_rrc_llc_test_common_data.rb_info[i], sizeof(rb_info_s));
  }

  /* Remove the DRBs that are released by the OTA msg */
  if (radio_resrc_cfg_ptr->m.drb_ToReleaseListPresent)
  {
    /* For DRB release, remove the DRB from stored RBs list */
    drb_rel_list = radio_resrc_cfg_ptr->drb_ToReleaseList;
    i = 0;
    while (i < drb_rel_list.n)
    {
      stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_rel_list.elem[i]);
      /* If the DRB is not found, ignore it and move to the next RB in the list */
      if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
      {
        continue;
      }

      if (rb_info[stored_rb_idx].rb_direction != LTE_RRC_LLC_RB_NOT_ESTABLISHED)
      {
        rb_info[stored_rb_idx].rb_direction = LTE_RRC_LLC_RB_NOT_ESTABLISHED;
        rb_info[stored_rb_idx].rb_mode = LTE_RB_RLC_MODE_INVALID;
        num_active_rb--;
      }
      i++;
    }
  }

  /* Determine if all UM DRBs were released by OTA msg */
  for (i = 0; i < num_active_rb; i++)
  {
    if (rb_info[i].rb_mode == LTE_RB_RLC_MODE_UM)
    {
      um_rb_after_ota_rel = TRUE;
      break;
    }
  }

  /* If a UM DRB existed during CRE */
  if ((lte_rrc_llc_test_common_data.reconfig_after_cre) &&
      (um_rb_in_cc))
  {
    lte_rrc_utf_get_next_ext_msg((byte **) &drx_opt_req_ptr,&buf_len);
    TF_ASSERT(drx_opt_req_ptr->msgr_hdr.id == LTE_CPHY_DRX_OPT_REQ);
    TF_ASSERT(drx_opt_req_ptr->um_mode_enabled == um_rb_in_cc);
  }
  /* If a UM DRB was added by the OTA msg when no UM RB existed */
  else if ((um_rb_in_cc == FALSE) && (um_rb_add_by_ota == TRUE))
  {
    lte_rrc_utf_get_next_ext_msg((byte **) &drx_opt_req_ptr,&buf_len);
    TF_ASSERT(drx_opt_req_ptr->msgr_hdr.id == LTE_CPHY_DRX_OPT_REQ);
    TF_ASSERT(drx_opt_req_ptr->um_mode_enabled == TRUE);
  }
  /* If an existing UM DRB was released by OTA msg */
  else if ((um_rb_add_by_ota == FALSE) && 
           (um_rb_in_cc == TRUE) &&
           (um_rb_after_ota_rel == FALSE))
  {
    lte_rrc_utf_get_next_ext_msg((byte **) &drx_opt_req_ptr,&buf_len);
    TF_ASSERT(drx_opt_req_ptr->msgr_hdr.id == LTE_CPHY_DRX_OPT_REQ);
    TF_ASSERT(drx_opt_req_ptr->um_mode_enabled == FALSE);
  }

  return;

}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_update_stored_rb_info

===========================================================================*/
/*!
    @brief
    This function updates the stored RB info in the test DB based on the OTA
    msg that is being processed. This function is called at the end of the test
    case, after L2 requests have been verified.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_update_stored_rb_info
(
  lte_rrc_proc_e proc /*!< source RRC procedure */
)
{
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr = NULL;
  lte_rrc_osys_SRB_ToAddModList srb_list;
  lte_rrc_osys_DRB_ToAddModList drb_list;
  lte_rrc_osys_DRB_ToReleaseList drb_rel_list;
  uint8 i = 0, srb_idx = 0, stored_rb_idx = 0, loop_count = 0;

  if ((proc == LTE_RRC_PROC_CRP) || (proc == LTE_RRC_PROC_CONTROLLER))
  {
    /* For Connection release, remove all the stored RBs */
    loop_count = lte_rrc_llc_test_common_data.num_active_rb;
    for (i = 0; i < loop_count; i++)
    {
      lte_rrc_llc_test_common_data.rb_info[i].rb_direction = LTE_RRC_LLC_RB_NOT_ESTABLISHED;
      lte_rrc_llc_test_common_data.num_active_rb--;
    }
    return;
  }

  if (proc == LTE_RRC_PROC_CEP)
  {
    /* For Connection setup, add SRB1 to the stored RB */
    lte_rrc_llc_test_common_data.rb_info[0].rb_cfg_idx = LTE_RRC_LLC_SRB1_RB_CFG_IDX;
    lte_rrc_llc_test_common_data.rb_info[0].lc_id = LTE_RRC_DCCH_SRB1;
    lte_rrc_llc_test_common_data.rb_info[0].eps_id = 0;
    lte_rrc_llc_test_common_data.rb_info[0].rb_direction = LTE_RRC_LLC_RB_BI_DIRECTION;
    lte_rrc_llc_test_common_data.num_active_rb++;
  }
  else if (proc == LTE_RRC_PROC_CONFIG)
  {
    /* Get the appropriate radio resource config */
    radio_resrc_cfg_ptr = lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr(LTE_RRC_PROC_CONFIG);

    if (radio_resrc_cfg_ptr->m.drb_ToReleaseListPresent)
    {
      /* For DRB release, remove the DRB from stored RBs list */
      drb_rel_list = radio_resrc_cfg_ptr->drb_ToReleaseList;
      while (loop_count < drb_rel_list.n)
      {
        stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_rel_list.elem[loop_count]);
        /* If the DRB is not found, ignore it and move to the next RB in the list */
        if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
        {
          continue;
        }

        if (lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].rb_direction 
            != LTE_RRC_LLC_RB_NOT_ESTABLISHED)
        {
          lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].rb_direction = LTE_RRC_LLC_RB_NOT_ESTABLISHED;
          lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].rb_mode = LTE_RB_RLC_MODE_INVALID;
          lte_rrc_llc_test_common_data.num_active_rb--;
        }
        loop_count++;
      }
    }

    /* For full configuration, clear all stored DRBs */
    if (lte_rrc_llc_test_common_data.full_config)
    {
      loop_count = lte_rrc_llc_test_common_data.num_active_rb;
      for (i = 0; i < loop_count; i++)
      {
        if (lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx < LTE_MAX_RB)
        {
          lte_rrc_llc_test_common_data.rb_info[i].rb_direction = LTE_RRC_LLC_RB_NOT_ESTABLISHED;
          lte_rrc_llc_test_common_data.rb_info[i].rb_mode = LTE_RB_RLC_MODE_INVALID;
          lte_rrc_llc_test_common_data.num_active_rb--;
        }
      }
    }

    loop_count = 0;
    if (radio_resrc_cfg_ptr->m.srb_ToAddModListPresent)
    {
      /* For SRB2 addition, add it to the stored RBs list */
      srb_list = radio_resrc_cfg_ptr->srb_ToAddModList;
      while (loop_count < srb_list.n)
      {
        srb_idx = srb_list.elem[loop_count].srb_Identity + LTE_MAX_RB;
        stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(srb_idx);
        /* If the SRB is not found, ignore it and move to the next RB in the list */
        if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
        {
          lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
            rb_cfg_idx = srb_idx;
          lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
            lc_id = LTE_RRC_DCCH_SRB2;
          lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
            eps_id = 0;
          lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
            rb_direction = LTE_RRC_LLC_RB_BI_DIRECTION;
          lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
            rb_mode = LTE_RB_RLC_MODE_AM;
          lte_rrc_llc_test_common_data.num_active_rb++;  
        }
        loop_count++;
      }
    }

    loop_count = 0;
    if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent)
    {
      drb_list = radio_resrc_cfg_ptr->drb_ToAddModList;
      while (loop_count < drb_list.n)
      {
        stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_list.elem[loop_count].drb_Identity);

       /* Store the RB mapping of the DRB being added in the test DB for cross verification 
        at a future point in the test. Add only if the DRB has not been added previously. 
        If the same DRB id is getting released and added in a single msg, the first condition 
        will not be true when the released RB is a DL only RB. Thats the reason the second OR 
        clause is needed. */
        if ((stored_rb_idx == LTE_MAX_ACTIVE_RB) || 
            ((lte_rrc_llc_test_drb_released_in_ota_msg(radio_resrc_cfg_ptr,
                                                       lte_rrc_llc_test_common_data.
                                                       rb_info[stored_rb_idx].rb_cfg_idx)) && 
             (lte_rrc_llc_test_drb_modified_in_ota_msg(radio_resrc_cfg_ptr,
                                                       lte_rrc_llc_test_common_data.
                                                       rb_info[stored_rb_idx].rb_cfg_idx))))
        {
          lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
            rb_cfg_idx = drb_list.elem[loop_count].drb_Identity;
          lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
            eps_id = drb_list.elem[loop_count].eps_BearerIdentity;
          lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
            lc_id = drb_list.elem[loop_count].logicalChannelIdentity;
          if (drb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Uni_Directional_UL)
          {
            lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
              rb_direction = LTE_RRC_LLC_RB_UL_DIRECTION;
            lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
              rb_mode = LTE_RB_RLC_MODE_UM;
          }
          else if (drb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Uni_Directional_DL)
          {
            lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
              rb_direction = LTE_RRC_LLC_RB_DL_DIRECTION;
            lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
              rb_mode = LTE_RB_RLC_MODE_UM;
          }
          else if (drb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_am)
          {
            /* Store it in the test DB for future checking during the test run */
            lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
              pdcp_status_required = drb_list.elem[loop_count].pdcp_Config.rlc_AM.statusReportRequired;
            lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
              rb_direction = LTE_RRC_LLC_RB_BI_DIRECTION;
            lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
              rb_mode = LTE_RB_RLC_MODE_AM;
          }
          else
          {
            lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
              rb_direction = LTE_RRC_LLC_RB_BI_DIRECTION;
            lte_rrc_llc_test_common_data.rb_info[lte_rrc_llc_test_common_data.num_active_rb].
              rb_mode = LTE_RB_RLC_MODE_UM;
          }
          lte_rrc_llc_test_common_data.num_active_rb++;
        }
        loop_count++;
      }
    }
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_copy_test_db

===========================================================================*/
/*!
    @brief
    This function copies CC to OC / OC to CC in the test DB

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_copy_test_db
(
  lte_rrc_llc_mrb_info_s *dest_mrb_info_ptr,
  lte_rrc_llc_mrb_info_s *src_mrb_info_ptr
)
{
  LTE_RRC_MEMCPY(dest_mrb_info_ptr, 
                 sizeof(lte_rrc_llc_mrb_info_s),
                 src_mrb_info_ptr, 
                 sizeof(lte_rrc_llc_mrb_info_s));

} /* lte_rrc_llc_test_copy_test_db */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_swap_cfg_dbs

===========================================================================*/
/*!
    @brief
    This function will swap the configuration test DBs and makes CC the cfg
    in use.  

    @return
    None
*/
/*=========================================================================*/
void lte_rrc_llc_test_swap_cfg_dbs
(
  void
)
{
  lte_rrc_llc_mrb_info_s *temp_mrb_info_ptr = NULL, *cc_ptr = NULL, *oc_ptr = NULL; 

  cc_ptr = &lte_rrc_llc_test_common_data.cc_mrb_info;
  oc_ptr = &lte_rrc_llc_test_common_data.oc_mrb_info;

  temp_mrb_info_ptr = cc_ptr;
  cc_ptr = oc_ptr;
  oc_ptr = temp_mrb_info_ptr;

} /* lte_rrc_llc_test_swap_cfg_dbs() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mrb_is_active

===========================================================================*/
/*!
    @brief
    This function determines if the passed MRB is part of the active MRBs
    stored in CC in the test DB

    @return
    uint8, returns the index of the stored MRB in test DB, 0xFF otherwise

 */
/*=========================================================================*/
uint8 lte_rrc_llc_test_mrb_is_active
(
  lte_rrc_llc_mrb_info_s *mrb_info_ptr,
  lte_rb_id_t mrb_id
)
{
  uint8 i = 0, stored_db_idx = 0xFF;

  /* Loop through the list of stored MRBs to see if the MRB being configured 
    is already part of the test DB */
  for (i = 0; i < mrb_info_ptr->num_active_mrb; i++)
  {
    if (mrb_info_ptr->mrb_info[i].mrb_id == mrb_id)
    {
      stored_db_idx = i;
      break;
    }
  }

  return stored_db_idx;

} /* lte_rrc_llc_test_mrb_is_active */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_update_stored_mrb_info

===========================================================================*/
/*!
    @brief
    This function updates the stored <RB info in the test DB based on the CFG_REQI
    that LLC receives from EMP. This function is called at the end of the test
    case, after L2 requests have been verified.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_update_stored_mrb_info
(
  lte_rrc_llc_mrb_info_s *mrb_info_ptr  /*!< Ptr to MRB info in test DB */
)
{
#ifdef FEATURE_LTE_EMBMS
  lte_rrc_llc_embms_cfg_s embms_cfg = lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_u.embms_cfg;
  uint8 i, mrb_idx = 0, stored_db_idx = 0xFF;

  /* If this req is to release all MRBs */
  if ((lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_type == LTE_RRC_CFG_DEACTIVATE_FS) ||
      (embms_cfg.release_all_mrb))
  {
    mrb_info_ptr->num_active_mrb = 0;

    for (i = 0; i < LTE_MAX_ACTIVE_MRB; i++)
    {
      memset(&mrb_info_ptr->mrb_info[i], 0, sizeof(mrb_info_s));
    }
    return;
  }

  for (i = 0; i < embms_cfg.num_mtch; i++)
  {
     stored_db_idx = lte_rrc_llc_test_mrb_is_active(&lte_rrc_llc_test_common_data.oc_mrb_info, 
                                                    embms_cfg.mtch_cfg[i].mrb_id);

    /* If the MRB is being configured */
    if (embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.mrb_action == LTE_RRC_EMBMS_RB_ACTION_CFG)
    {
      if (stored_db_idx == 0xFF)
      {
        mrb_idx = mrb_info_ptr->num_active_mrb;
        mrb_info_ptr->num_active_mrb++;
      }
      else
      {
        mrb_idx = stored_db_idx;
      }
      /* If MRB is not already stored in the test DB, store it */
      mrb_info_ptr->mrb_info[mrb_idx].area_id = embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.mbsfn_area_id;
      mrb_info_ptr->mrb_info[mrb_idx].pmch_id = embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.pmch_id;
      mrb_info_ptr->mrb_info[mrb_idx].lc_id = embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.lc_id;
      mrb_info_ptr->mrb_info[mrb_idx].mrb_id = embms_cfg.mtch_cfg[i].mrb_id;
    }
    else if ((embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.mrb_action == LTE_RRC_EMBMS_RB_ACTION_RELEASE) &&
             (stored_db_idx != 0xFF))
    {
      memmove(&mrb_info_ptr->mrb_info[i], 
              &mrb_info_ptr->mrb_info[i+1], 
              (sizeof(mrb_info_ptr->mrb_info[i]) * 
               (LTE_MAX_ACTIVE_MRB - 1 - i)));

      mrb_info_ptr->num_active_mrb--;
    }
  }
   #endif
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rlcdl_explicit_am_cfg

===========================================================================*/
/*!
    @brief
    This function performs all the test explicit RLC DL am config parameters.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rlcdl_explicit_am_cfg
(
  lte_rrc_osys_DL_AM_RLC *ota_rlcdl_am_cfg_ptr,
  lte_rlcdl_am_cfg_s *rlcdl_am_cfg_ptr
)
{
  int t_reordering = 0, t_status_prohibit = 0;
  
  t_reordering = (int) ota_rlcdl_am_cfg_ptr->t_Reordering;
  /* Map the t_reordering enum value in the OTA msg to the actual value */
  if (t_reordering <= 20)
  {
    TF_ASSERT(rlcdl_am_cfg_ptr->t_reordering == (t_reordering * 5));
  }
  else if (t_reordering <= 30)
  {
    TF_ASSERT(rlcdl_am_cfg_ptr->t_reordering == ((t_reordering * 5) + ((t_reordering - 20) * 5)));
  }
  else
  {
    TF_ASSERT(FALSE);
  }

  t_status_prohibit = (int) ota_rlcdl_am_cfg_ptr->t_StatusProhibit;
  /* Map the t_status_prohibit enum value in the OTA msg to the actual value */
  if (t_status_prohibit <= 50)
  {
    TF_ASSERT(rlcdl_am_cfg_ptr->t_status_prohibit == (t_status_prohibit * 5));
  }
  else if (t_status_prohibit <= 55)
  {
    TF_ASSERT(rlcdl_am_cfg_ptr->t_status_prohibit == (250 + ((t_status_prohibit - 50) * 50)));
  }
  else
  {
    TF_ASSERT(FALSE);
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rlcdl_explicit_um_cfg

===========================================================================*/
/*!
    @brief
    This function performs all the test explicit RLC DL um config parameters.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rlcdl_explicit_um_cfg
(
  lte_rrc_osys_DL_UM_RLC *ota_rlcdl_um_cfg_ptr,
  lte_rlcdl_um_cfg_s *rlcdl_um_cfg_ptr
)
{
  int t_reordering = 0, sn_field_length = 0;
  
  t_reordering = (int) ota_rlcdl_um_cfg_ptr->t_Reordering;
  /* Map the t_reordering enum value in the OTA msg to the actual value */
  if (t_reordering <= 20)
  {
    TF_ASSERT(rlcdl_um_cfg_ptr->t_reordering == (t_reordering * 5));
  }
  else if (t_reordering <= 30)
  {
    TF_ASSERT(rlcdl_um_cfg_ptr->t_reordering == ((t_reordering * 5) + ((t_reordering - 20) * 5)));
  }
  else
  {
    TF_ASSERT(FALSE);
  }

  sn_field_length = (int) ota_rlcdl_um_cfg_ptr->sn_FieldLength;
  /* Map the t_status_prohibit enum value in the OTA msg to the actual value */
  TF_ASSERT(rlcdl_um_cfg_ptr->sn_length == ((sn_field_length * 5) + 5));
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rlcdl_rb_info

===========================================================================*/
/*!
    @brief
    This function performs all the test RB info in RLC DL config request.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rlcdl_rb_info
(
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr,
  lte_rrc_proc_e proc
)
{
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr = NULL;
  lte_rrc_osys_SRB_ToAddModList srb_list;
  lte_rrc_osys_DRB_ToAddModList drb_list;
  lte_rrc_osys_DRB_ToReleaseList drb_rel_list;
  uint8 mod_rb_count = 0, del_rb_count = 0, i = 0, srb_idx = 0, drb_idx = 0, 
   stored_rb_idx = 0, loop_count = 0;

  /* Get the appropriate radio resource config */
  radio_resrc_cfg_ptr = lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr(proc);

  /* Verify RRC Connection release scenario, when both SRBs and DRBs are released */
  if (radio_resrc_cfg_ptr == NULL)
  {
    /* For the RBs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if (lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_DL_DIRECTION)
      {
        TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
        del_rb_count++;
      }
    }
    TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == del_rb_count);
    TF_MSG("\n Verifying RLC DL RB info, connection release scenario verified successfully \n");
    /* All parameters for connection release have been tested, just return */
    return;
  }

  /* Verify DRB release during full config scenario */
  if (lte_rrc_llc_test_common_data.full_config)
  {
    /* For the RBs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if ((lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_DL_DIRECTION) &&
          (lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx < LTE_MAX_RB))
      {
        TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
        del_rb_count++;
      }
    }
    TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == del_rb_count);
    TF_MSG("\n Verifying RLC DL RB info, full config scenario verified successfully \n");
  }
  /* If DRBsToRelease list is present, verify the DRBs released at RLC */
  else if ((proc != LTE_RRC_PROC_CEP) && 
      (radio_resrc_cfg_ptr->m.drb_ToReleaseListPresent))
  {
    drb_rel_list = radio_resrc_cfg_ptr->drb_ToReleaseList;
    while (loop_count < drb_rel_list.n)
    {
      stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_rel_list.elem[loop_count]);
      /* If the DRB is not found, ignore it and move to the next RB in the list */
      if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
      {
        loop_count++;
        del_rb_count++;
        continue;
      }

      if (lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].rb_direction & LTE_RRC_LLC_RB_DL_DIRECTION)
      {
        TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] == drb_rel_list.elem[loop_count]);
        del_rb_count++;
      }
      loop_count++;
    }
    TF_MSG("\n Verifying RLC DL RB info, DRB release list verified successfully \n");
  }

  loop_count = 0;
  /* If RLC config of SRBs is present, verify the RLC Config parameters */
  if (radio_resrc_cfg_ptr->m.srb_ToAddModListPresent)
  {
    srb_list = radio_resrc_cfg_ptr->srb_ToAddModList;
    /* Iterate through the SRBToAddModifiedList */
    while(loop_count < srb_list.n)
    {
      /* If RLC config is present or if it is Rel 9 full config, look at the SRB */
      if ((srb_list.elem[loop_count].m.rlc_ConfigPresent) ||
          (lte_rrc_llc_test_common_data.full_config))
      {
        srb_idx = srb_list.elem[loop_count].srb_Identity + LTE_MAX_RB;
        TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[mod_rb_count].rb_cfg_idx == srb_idx);
        stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(srb_idx);

        /* If the SRB is not found, ignore it and move to the next RB in the list */
        if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
        {
          TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[mod_rb_count].action == LTE_RLC_RB_ACTION_ADD);
        }
        else
        {
          TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[mod_rb_count].action == LTE_RLC_RB_ACTION_MODIFY);
        }
        /* Verify the default values for DL RLC */
        TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].lc_id == srb_list.elem[loop_count].srb_Identity);
        TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_id == srb_list.elem[loop_count].srb_Identity);
        TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_cfg_idx == srb_idx);
        TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_mode == LTE_RB_RLC_MODE_AM);
        TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_type == LTE_RB_TYPE_SRB);

        if (srb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_SRB_ToAddMod_rlc_Config_explicitValue)
        {
          lte_rrc_llc_test_rlcdl_explicit_am_cfg(&srb_list.elem[loop_count].rlc_Config.u.explicitValue->u.am->dl_AM_RLC,
                                                 &rlcdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].cfg.am_cfg);
        }
        else if ((srb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_SRB_ToAddMod_rlc_Config_defaultValue) ||
            (lte_rrc_llc_test_common_data.full_config))
        {
          TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].cfg.am_cfg.t_reordering == 35);
          TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].cfg.am_cfg.t_status_prohibit == 0);
        }
        else
        {
          TF_ASSERT(FALSE);
        }
        mod_rb_count++;
      }
      loop_count++;
    }
    TF_MSG("\n Verifying RLC DL RB info, SRB list verified successfully \n");
  }

  loop_count = 0;
  /* If RLC config of DRBs is present, verify the RLC Config parameters */
  if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent)
  {
    drb_list = radio_resrc_cfg_ptr->drb_ToAddModList;
    /* Iterate through the DRBToAddModifiedList */
    while(loop_count < drb_list.n)
    {
      if ((drb_list.elem[loop_count].m.rlc_ConfigPresent) &&
          (drb_list.elem[loop_count].rlc_Config.t != T_lte_rrc_osys_RLC_Config_um_Uni_Directional_UL))
      {
        drb_idx = drb_list.elem[loop_count].drb_Identity;
        TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[mod_rb_count].rb_cfg_idx == drb_idx);
        stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_idx);
        
        /* If the RB status is not established in the stored DB, or if
          the same DRB being released and added together */
        if ((stored_rb_idx == LTE_MAX_ACTIVE_RB) ||
            ((lte_rrc_llc_test_drb_released_in_ota_msg(radio_resrc_cfg_ptr,
                                                       lte_rrc_llc_test_common_data.
                                                       rb_info[stored_rb_idx].rb_cfg_idx)) && 
             (lte_rrc_llc_test_drb_modified_in_ota_msg(radio_resrc_cfg_ptr,
                                                       lte_rrc_llc_test_common_data.
                                                       rb_info[stored_rb_idx].rb_cfg_idx))) ||
            (lte_rrc_llc_test_common_data.full_config))
        {
          TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[mod_rb_count].action == LTE_RLC_RB_ACTION_ADD);
          TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                    lc_id == drb_list.elem[loop_count].logicalChannelIdentity);
        }
        else
        {
          TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[mod_rb_count].action == LTE_RLC_RB_ACTION_MODIFY);
          TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                    lc_id == lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].lc_id);
        }
        TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_id == drb_idx);
        TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_cfg_idx == drb_idx);
        if (drb_list.elem[loop_count].m.eps_BearerIdentityPresent)
        {
          TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                    eps_id == drb_list.elem[loop_count].eps_BearerIdentity);
        }
        else
        {
          TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                    eps_id == lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].eps_id);
        }
        TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_type == LTE_RB_TYPE_DRB);

        if (drb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_am)
        {
          TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_mode == LTE_RB_RLC_MODE_AM);
          lte_rrc_llc_test_rlcdl_explicit_am_cfg(&drb_list.elem[loop_count].rlc_Config.u.am->dl_AM_RLC,
                                                 &rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].cfg.am_cfg);
        }
        else 
        {
          TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_mode == LTE_RB_RLC_MODE_UM);
          if (drb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Bi_Directional)
          {
            lte_rrc_llc_test_rlcdl_explicit_um_cfg(&drb_list.elem[loop_count].rlc_Config.u.um_Bi_Directional->dl_UM_RLC,
                                                   &rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].cfg.um_cfg);
          }
          else if (drb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Uni_Directional_DL)
          {
            lte_rrc_llc_test_rlcdl_explicit_um_cfg(&drb_list.elem[loop_count].rlc_Config.u.um_Uni_Directional_DL->dl_UM_RLC,
                                                   &rlcdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].cfg.um_cfg);
          }
        }
        mod_rb_count++;
      }
      loop_count++;
    }
    TF_MSG("\n Verifying RLC DL RB info, DRB list verified successfully \n");
  }

  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == mod_rb_count);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == del_rb_count);
  TF_MSG("\n RLC DL RB info verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rlcdl_mrb_info

===========================================================================*/
/*!
    @brief
    This function performs all the test MRB info in RLC DL config request.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rlcdl_mrb_info
(
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr
)
{
#ifdef FEATURE_LTE_EMBMS
  lte_rrc_llc_embms_cfg_s embms_cfg = lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_u.embms_cfg;
  lte_rrc_llc_mrb_info_s *oc_mrb_info_ptr = &lte_rrc_llc_test_common_data.oc_mrb_info;
  lte_rrc_llc_mrb_info_s *cc_mrb_info_ptr = &lte_rrc_llc_test_common_data.cc_mrb_info;
  uint8 i = 0, j = 0, num_released_mrb = 0, mrb_cfg_idx = 0;

  /* Find the num released MRBs from the request */
  for (i = 0; i < embms_cfg.num_mtch; i++)
  {
    if (embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.mrb_action == LTE_RRC_EMBMS_RB_ACTION_RELEASE)
    {
      num_released_mrb++;
    }
    else 
    {
      for (j = 0; j < cc_mrb_info_ptr->num_active_mrb; j++)
      {
        /* If either of Area id, PMCH id or LC id has changed, then MRB will be released in RLC */
        if ((embms_cfg.mtch_cfg[i].mrb_id == cc_mrb_info_ptr->mrb_info[j].mrb_id) &&
            ((embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.mbsfn_area_id != cc_mrb_info_ptr->mrb_info[j].area_id) ||
             (embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.pmch_id != cc_mrb_info_ptr->mrb_info[j].pmch_id) ||
             (embms_cfg.mtch_cfg[i].mrb_cfg_u.mtch_cfg.lc_id != cc_mrb_info_ptr->mrb_info[j].lc_id)))
        {
          num_released_mrb++;
        }
      }
    }
  }

  TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.num_added_mrb == oc_mrb_info_ptr->num_active_mrb);
  TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.num_released_mrb == num_released_mrb);

  for (i = 0; i < oc_mrb_info_ptr->num_active_mrb; i++)
  {
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.added_mrb_cfg_idx[i].rb_cfg_idx 
              == (oc_mrb_info_ptr->mrb_info[i].mrb_id - (LTE_EMBMS_MCCH_RB_ID + 1)));
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.added_mrb_cfg_idx[i].action == LTE_RLC_RB_ACTION_ADD);
  }

  for (i = 0; i < oc_mrb_info_ptr->num_active_mrb; i++)
  {
    mrb_cfg_idx = (oc_mrb_info_ptr->mrb_info[i].mrb_id - (LTE_EMBMS_MCCH_RB_ID + 1));
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.mrb_cfg_base_ptr[mrb_cfg_idx].
              area_id == oc_mrb_info_ptr->mrb_info[i].area_id);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.mrb_cfg_base_ptr[mrb_cfg_idx].
              pmch_id == oc_mrb_info_ptr->mrb_info[i].pmch_id);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.mrb_cfg_base_ptr[mrb_cfg_idx].
              lc_id == oc_mrb_info_ptr->mrb_info[i].lc_id);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.mrb_cfg_base_ptr[mrb_cfg_idx].mrb_id == oc_mrb_info_ptr->mrb_info[i].mrb_id);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.mrb_cfg_base_ptr[mrb_cfg_idx].mrb_cfg_idx 
              == (oc_mrb_info_ptr->mrb_info[i].mrb_id - (LTE_EMBMS_MCCH_RB_ID + 1)));
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.mrb_cfg_base_ptr[mrb_cfg_idx].rb_mode == LTE_RB_RLC_MODE_UM);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.mrb_cfg_base_ptr[mrb_cfg_idx].rb_type == LTE_RB_TYPE_MTCH);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.mrb_cfg_base_ptr[mrb_cfg_idx].cfg.
              sn_length == LTE_RRC_LLC_MRB_RLC_SN_LENGTH);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.mrb_cfg_base_ptr[mrb_cfg_idx].cfg.
              t_reordering == LTE_RRC_LLC_MRB_RLC_T_REORDERING);
  }
  
  TF_MSG("\n RLC DL MRB info verified successfully \n");
  #endif
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rlcul_poll_pdu

===========================================================================*/
/*!
    @brief
    This function determines the RLC UL poll PDU from the OTA msg.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rlcul_poll_pdu
(
  lte_rrc_osys_UL_AM_RLC *ota_rlcul_am_cfg_ptr,
  lte_rlcul_am_cfg_s *rlcul_am_cfg_ptr
)
{
  uint16 poll_pdu = 0;

  /* Get the pollPDU value based on the OTA message */
  switch (ota_rlcul_am_cfg_ptr->pollPDU) 
  {
    case lte_rrc_osys_PollPDU_p4:
      poll_pdu = 4;
      break;
    case lte_rrc_osys_PollPDU_p8:
      poll_pdu = 8;
      break;
    case lte_rrc_osys_PollPDU_p16:
      poll_pdu = 16;
      break;
    case lte_rrc_osys_PollPDU_p32:
      poll_pdu = 32;
      break;
    case lte_rrc_osys_PollPDU_p64:
      poll_pdu = 64;
      break;
    case lte_rrc_osys_PollPDU_p128:
      poll_pdu = 128;
      break;
    case lte_rrc_osys_PollPDU_p256:
      poll_pdu = 256;
      break;
    case lte_rrc_osys_PollPDU_pInfinity:
      poll_pdu = LTE_OTA_IE_INFINITY_UINT16;
      break;
    default:
      TF_ASSERT(FALSE);
  }
  /* Check if the determined pollPDU value is what is passed to RLC UL */
  TF_ASSERT(rlcul_am_cfg_ptr->poll_pdu == poll_pdu);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rlcul_poll_byte

===========================================================================*/
/*!
    @brief
    This function determines the RLC UL poll byte from the OTA msg.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rlcul_poll_byte
(
  lte_rrc_osys_UL_AM_RLC *ota_rlcul_am_cfg_ptr,
  lte_rlcul_am_cfg_s *rlcul_am_cfg_ptr
)
{
  uint32 poll_byte = 0;

  /* Get the pollByte value based on the OTA message */
  switch (ota_rlcul_am_cfg_ptr->pollByte) 
  {
    case lte_rrc_osys_PollByte_kB25:
      poll_byte = 25*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB50:
      poll_byte = 50*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB75:
      poll_byte = 75*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB100:
      poll_byte = 100*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB125:
      poll_byte = 125*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB250:
      poll_byte = 250*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB375:
      poll_byte = 375*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB500:
      poll_byte = 500*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB750:
      poll_byte = 750*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB1000:
      poll_byte = 1000*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB1250:
      poll_byte = 1250*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB1500:
      poll_byte = 1500*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB2000:
      poll_byte = 2000*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kB3000:
      poll_byte = 3000*LTE_RRC_LLC_KB_TO_BYTE_CONSTANT;
      break;
    case lte_rrc_osys_PollByte_kBinfinity:
      poll_byte = LTE_OTA_IE_INFINITY_UINT32;
      break;
    default:
      TF_ASSERT(FALSE);
  }
  /* Check if the determined poll_byte value is what is passed to RLC UL */
  TF_ASSERT(rlcul_am_cfg_ptr->poll_byte == poll_byte);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rlcul_max_retx_threshold

===========================================================================*/
/*!
    @brief
    This function determines the RLC UL max_retx_thresh from the OTA msg.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rlcul_max_retx_threshold
(
  lte_rrc_osys_UL_AM_RLC *ota_rlcul_am_cfg_ptr,
  lte_rlcul_am_cfg_s *rlcul_am_cfg_ptr
)
{
   uint8 max_retx_threshold = 0;

  /* Get the max_retx_thresh value based on the OTA message */
  switch (ota_rlcul_am_cfg_ptr->maxRetxThreshold) 
  {
    case lte_rrc_osys_UL_AM_RLC_maxRetxThreshold_t1:
      max_retx_threshold = 1;
      break;
    case lte_rrc_osys_UL_AM_RLC_maxRetxThreshold_t2:
      max_retx_threshold = 2;
      break;
    case lte_rrc_osys_UL_AM_RLC_maxRetxThreshold_t3:
      max_retx_threshold = 3;
      break;
    case lte_rrc_osys_UL_AM_RLC_maxRetxThreshold_t4:
      max_retx_threshold = 4;
      break;
    case lte_rrc_osys_UL_AM_RLC_maxRetxThreshold_t6:
      max_retx_threshold = 6;
      break;
    case lte_rrc_osys_UL_AM_RLC_maxRetxThreshold_t8:
      max_retx_threshold = 8;
      break;
    case lte_rrc_osys_UL_AM_RLC_maxRetxThreshold_t16:
      max_retx_threshold = 16;
      break;
    case lte_rrc_osys_UL_AM_RLC_maxRetxThreshold_t32:
      max_retx_threshold = 32;
      break;
    default:
      TF_ASSERT(FALSE);
  }
  /* Check if the determined max_retx_thresh value is what is passed to RLC UL */
  TF_ASSERT(rlcul_am_cfg_ptr->max_retx_threshold == max_retx_threshold);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pdcp_discard_timer

===========================================================================*/
/*!
    @brief
    This function determines the PDCP discard timer from the OTA msg.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_pdcp_discard_timer
(
  lte_rrc_osys_PDCP_Config *ota_pdcp_cfg_ptr,
  uint16 *pdcp_discard_timer
)
{
   uint16 discard_timer = 0;

  /* Get the discard timer value based on the OTA message */
  switch (ota_pdcp_cfg_ptr->discardTimer) 
  {
    case lte_rrc_osys_PDCP_Config_discardTimer_ms50:
      discard_timer = 50;
      break;
    case lte_rrc_osys_PDCP_Config_discardTimer_ms100:
      discard_timer = 100;
      break;
    case lte_rrc_osys_PDCP_Config_discardTimer_ms150:
      discard_timer = 150;
      break;
    case lte_rrc_osys_PDCP_Config_discardTimer_ms300:
      discard_timer = 300;
      break;
    case lte_rrc_osys_PDCP_Config_discardTimer_ms500:
      discard_timer = 500;
      break;
    case lte_rrc_osys_PDCP_Config_discardTimer_ms750:
      discard_timer = 750;
      break;
    case lte_rrc_osys_PDCP_Config_discardTimer_ms1500:
      discard_timer = 1500;
      break;
    case lte_rrc_osys_PDCP_Config_discardTimer_infinity:
      discard_timer = LTE_OTA_IE_INFINITY_UINT16;
      break;
    default:
      TF_ASSERT(FALSE);
  }
  /* Check if the determined discard_timer value is what is passed to RLC UL */
  TF_ASSERT(*pdcp_discard_timer == discard_timer);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rlcul_explicit_am_cfg

===========================================================================*/
/*!
    @brief
    This function performs all the test explicit RLC UL am config parameters.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rlcul_explicit_am_cfg
(
  lte_rrc_osys_UL_AM_RLC *ota_rlcul_am_cfg_ptr,
  lte_rlcul_am_cfg_s *rlcul_am_cfg_ptr
)
{
  int t_poll_retransmit = 0;

  /* Map the t_poll_retransmit enum value in the OTA msg to the actual value */
  t_poll_retransmit = (int) ota_rlcul_am_cfg_ptr->t_PollRetransmit;

  if (t_poll_retransmit < 50)
  {
    TF_ASSERT(rlcul_am_cfg_ptr->t_poll_retransmit == ((t_poll_retransmit + 1) * 5));
  }
  else if (t_poll_retransmit < 55)
  {
    TF_ASSERT(rlcul_am_cfg_ptr->t_poll_retransmit == (250 + (((t_poll_retransmit + 1) - 50) * 50)));
  }
  else
  {
    TF_ASSERT(FALSE);
  }
  
  lte_rrc_llc_test_rlcul_poll_pdu(ota_rlcul_am_cfg_ptr, rlcul_am_cfg_ptr);
  lte_rrc_llc_test_rlcul_poll_byte(ota_rlcul_am_cfg_ptr, rlcul_am_cfg_ptr);
  lte_rrc_llc_test_rlcul_max_retx_threshold(ota_rlcul_am_cfg_ptr, rlcul_am_cfg_ptr);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rlcul_rb_info

===========================================================================*/
/*!
    @brief
    This function performs all the test RB info in RLC UL config request.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rlcul_rb_info
(
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr,
  lte_rrc_proc_e proc
)
{
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr = NULL;
  lte_rrc_osys_SRB_ToAddModList srb_list;
  lte_rrc_osys_DRB_ToAddModList drb_list;
  lte_rrc_osys_DRB_ToReleaseList drb_rel_list;
  uint8 mod_rb_count = 0, del_rb_count = 0, i = 0, srb_idx = 0, drb_idx = 0, 
   stored_rb_idx = 0, loop_count = 0, sn_field_length = 0;

  /* Get the appropriate radio resource config */
  radio_resrc_cfg_ptr = lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr(proc);

  /* Verify RRC Connection release scenario, when both SRBs and DRBs are released */
  if (radio_resrc_cfg_ptr == NULL)
  {
    /* For the RBs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if (lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION)
      {
        TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
        del_rb_count++;
      }
    }
    TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == del_rb_count);
    TF_MSG("\n Verifying RLC UL RB info, connection release scenario verified successfully \n");
    /* All parameters for connection release have been tested, just return */
    return;
  }

  /* Verify DRB release during full config scenario */
  if (lte_rrc_llc_test_common_data.full_config)
  {
    /* For the RBs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if ((lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION) &&
          (lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx < LTE_MAX_RB))
      {
        TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
        del_rb_count++;
      }
    }
    TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == del_rb_count);
    TF_MSG("\n Verifying RLC UL RB info, full config scenario verified successfully \n");
  }
  /* If DRBsToRelease list is present, verify the DRBs released at RLC */
  else if ((proc != LTE_RRC_PROC_CEP) && 
      (radio_resrc_cfg_ptr->m.drb_ToReleaseListPresent))
  {
    drb_rel_list = radio_resrc_cfg_ptr->drb_ToReleaseList;
    while (loop_count < drb_rel_list.n)
    {
      stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_rel_list.elem[loop_count]);
      /* If the DRB is not found, ignore it and move to the next RB in the list */
      if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
      {
        loop_count++;
        continue;
      }

      if (lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION)
      {
        TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] == drb_rel_list.elem[loop_count]);
        del_rb_count++;
      }
      loop_count++;
    }
    TF_MSG("\n Verifying RLC UL RB info, DRB release verified successfully \n");
  }

  loop_count = 0;
  /* If RLC config of SRBs is present, verify the RLC Config parameters */
  if (radio_resrc_cfg_ptr->m.srb_ToAddModListPresent)
  {
    srb_list = radio_resrc_cfg_ptr->srb_ToAddModList;
    /* Iterate through the SRBToAddModifiedList */
    while(loop_count < srb_list.n)
    {
      /* If RLC config is present or if it is Rel 9 full config, look at the SRB */
      if ((srb_list.elem[loop_count].m.rlc_ConfigPresent) ||
          (lte_rrc_llc_test_common_data.full_config))
      {
        srb_idx = srb_list.elem[loop_count].srb_Identity + LTE_MAX_RB;
        TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[mod_rb_count].rb_cfg_idx == srb_idx);
        stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(srb_idx);
        /* If the SRB is not found, ignore it and move to the next RB in the list */
        if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
        {
          TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[mod_rb_count].action == LTE_RLC_RB_ACTION_ADD);
        }
        else
        {
          TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[mod_rb_count].action == LTE_RLC_RB_ACTION_MODIFY);
        }
        /* Verify the default values for UL RLC */
        TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].lc_id == srb_list.elem[loop_count].srb_Identity);
        TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_id == srb_list.elem[loop_count].srb_Identity);
        TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_cfg_idx == srb_idx);
        TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_mode == LTE_RB_RLC_MODE_AM);
        TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_type == LTE_RB_TYPE_SRB);
        TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].
                  pdcp_discard_timer == LTE_RRC_LLC_DISCARD_TIMER_NOT_PRESENT);
        TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].pdcp_status_required == FALSE);
        TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].pdcp_seq_len == LTE_RRC_LLC_DEFAULT_SRB_SEQ_LEN);

        if (srb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_SRB_ToAddMod_rlc_Config_explicitValue)
        {
          lte_rrc_llc_test_rlcul_explicit_am_cfg(&srb_list.elem[loop_count].rlc_Config.u.explicitValue->u.am->ul_AM_RLC,
                                                 &rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].cfg.am_cfg);
        }
        else if ((srb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_SRB_ToAddMod_rlc_Config_defaultValue) ||
            (lte_rrc_llc_test_common_data.full_config))
        {
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].cfg.am_cfg.max_retx_threshold == 4);
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].cfg.am_cfg.poll_byte == LTE_OTA_IE_INFINITY_UINT32);
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].cfg.am_cfg.poll_pdu == LTE_OTA_IE_INFINITY_UINT16);
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].cfg.am_cfg.t_poll_retransmit == 45); 
        }
        else
        {
          TF_ASSERT(FALSE);
        }
        mod_rb_count++;
      }
      loop_count++;
    }
    TF_MSG("\n Verifying RLC UL RB info, SRB list verified successfully \n");
  }

  loop_count = 0;
  /* If RLC config of DRBs is present, verify the RLC Config parameters */
  if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent)
  {
    drb_list = radio_resrc_cfg_ptr->drb_ToAddModList;
    /* Iterate through the DRBToAddModifiedList */
    while(loop_count < drb_list.n)
    {
      drb_idx = drb_list.elem[loop_count].drb_Identity;
      /* Get the RB idx of the RB being added from the CC test DB */
      stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_idx);
      
      if ((drb_list.elem[loop_count].m.rlc_ConfigPresent) &&
          (drb_list.elem[loop_count].rlc_Config.t != T_lte_rrc_osys_RLC_Config_um_Uni_Directional_DL))
      {
        TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[mod_rb_count].rb_cfg_idx == drb_idx);
        /* If the RB status is not established in the stored DB, or if
          the same DRB being released and added together */
        if ((stored_rb_idx == LTE_MAX_ACTIVE_RB) ||
            ((lte_rrc_llc_test_drb_released_in_ota_msg(radio_resrc_cfg_ptr,
                                                       lte_rrc_llc_test_common_data.
                                                       rb_info[stored_rb_idx].rb_cfg_idx)) && 
             (lte_rrc_llc_test_drb_modified_in_ota_msg(radio_resrc_cfg_ptr,
                                                       lte_rrc_llc_test_common_data.
                                                       rb_info[stored_rb_idx].rb_cfg_idx))) ||
            (lte_rrc_llc_test_common_data.full_config))
        {
          TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[mod_rb_count].action == LTE_RLC_RB_ACTION_ADD);
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                    lc_id == drb_list.elem[loop_count].logicalChannelIdentity);
          lte_rrc_llc_test_pdcp_discard_timer(&drb_list.elem[loop_count].pdcp_Config,
                                              &rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                                              pdcp_discard_timer);
          if (drb_list.elem[loop_count].pdcp_Config.m.rlc_AMPresent == 1)
          {
            TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                      pdcp_status_required == drb_list.elem[loop_count].pdcp_Config.rlc_AM.statusReportRequired);
          }

          /* Verify PDCP seq length for UM RBs*/
          if (drb_list.elem[loop_count].pdcp_Config.m.rlc_UMPresent == 1)
          {
            if (drb_list.elem[loop_count].pdcp_Config.rlc_UM.pdcp_SN_Size == lte_rrc_osys_PDCP_Config_rlc_UM_pdcp_SN_Size_len7bits)
            {
              TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                        pdcp_seq_len == LTE_RRC_LLC_SHORT_DRB_SEQ_LEN);
            }
            else
            {
              TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                        pdcp_seq_len == LTE_RRC_LLC_LONG_DRB_SEQ_LEN);
            }
          }
        }
        else
        {
          TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[mod_rb_count].action == LTE_RLC_RB_ACTION_MODIFY);
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                    lc_id == lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].lc_id);
        }
        TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_id == drb_idx);
        if (drb_list.elem[loop_count].m.eps_BearerIdentityPresent)
        {
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                    eps_id == drb_list.elem[loop_count].eps_BearerIdentity);
        }
        else
        {
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                    eps_id == lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].eps_id);
        }
        TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_cfg_idx == drb_idx);
        TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_type == LTE_RB_TYPE_DRB);
        
        if (drb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_am)
        {
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_mode == LTE_RB_RLC_MODE_AM);
          lte_rrc_llc_test_rlcul_explicit_am_cfg(&drb_list.elem[loop_count].rlc_Config.u.am->ul_AM_RLC,
                                                 &rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].cfg.am_cfg);
          /* Verify PDCP seq length for UM RBs*/
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].pdcp_seq_len == LTE_RRC_LLC_LONG_DRB_SEQ_LEN);
        }
        else 
        {
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_mode == LTE_RB_RLC_MODE_UM);
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].pdcp_status_required == FALSE);
          if (drb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Bi_Directional)
          {
            sn_field_length = (int) drb_list.elem[loop_count].rlc_Config.u.um_Bi_Directional->ul_UM_RLC.sn_FieldLength;
            /* Map the sn_field_length value in the OTA msg to the actual value */
            TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].cfg.um_cfg.
                      sn_length == ((sn_field_length * 5) + 5));
          }
          else if (drb_list.elem[loop_count].rlc_Config.t == T_lte_rrc_osys_RLC_Config_um_Uni_Directional_UL)
          {
            sn_field_length = (int) drb_list.elem[loop_count].rlc_Config.u.um_Uni_Directional_UL->ul_UM_RLC.sn_FieldLength;
            /* Map the sn_field_length value in the OTA msg to the actual value */
            TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].cfg.um_cfg.
                      sn_length == ((sn_field_length * 5) + 5));
          }
        }
        mod_rb_count++;
      }
      else
      {
        /* Verifying the RB modification when the RLC info present under PDCP config is changed */
        if ((drb_list.elem[loop_count].m.pdcp_ConfigPresent) &&
            (drb_list.elem[loop_count].pdcp_Config.m.rlc_AMPresent) &&
            (drb_list.elem[loop_count].pdcp_Config.rlc_AM.statusReportRequired 
             != lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].pdcp_status_required))
        {
          TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[mod_rb_count].rb_cfg_idx == drb_idx);
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].lc_id 
                    == lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].lc_id);
          TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[mod_rb_count].action == LTE_RLC_RB_ACTION_MODIFY);
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_mode == LTE_RB_RLC_MODE_AM);
          TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                    pdcp_status_required == drb_list.elem[loop_count].pdcp_Config.rlc_AM.statusReportRequired);
          mod_rb_count++;
        }
      }
      loop_count++;
    }
    TF_MSG("\n Verifying RLC UL RB info, DRB list verified successfully \n");
  }

  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == mod_rb_count);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == del_rb_count);
  TF_MSG("\n RLC UL RB info verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_rlcdl_cfg_req_and_send_cnf

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying RLC DL config request
    parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_rlcdl_cfg_req_and_send_cnf
(
  lte_rrc_proc_e proc
)
{
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  uint32 buf_len = 0;

  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);

  /* Verify the config type based on the source proc */
  lte_rrc_llc_test_verify_l2_cfg_reason(proc, rlcdl_cfg_req_ptr->cfg_reason);

  /* Verify all RB parameters in RLC DL */
  lte_rrc_llc_test_rlcdl_rb_info(rlcdl_cfg_req_ptr, proc);

  /* After RLCDL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);

  TF_MSG("\nCRLCDL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_rlcdl_cfg_req_and_send_cnf_embms

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying RLC DL config request
    parameters for eMBMS and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_rlcdl_cfg_req_and_send_cnf_embms
(
  boolean cfg_for_release,
  boolean release_all_mrb,
  boolean release_all_rb

)
{
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  lte_rrc_llc_mrb_info_s *mrb_info_ptr = &lte_rrc_llc_test_common_data.cc_mrb_info;
  uint32 buf_len = 0, i = 0;
  lte_rb_cfg_idx_t mrb_idx = 0;

  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);

  if ((release_all_rb) || (release_all_mrb))
  {
    if (release_all_rb)
    {
      TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RELEASE_ALL);
    }
    else
    {
      TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RELEASE_ALL_EMBMBS);
    }

    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.num_added_mrb == 0);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.num_released_mrb == mrb_info_ptr->num_active_mrb);
    for (i = 0; i < mrb_info_ptr->num_active_mrb; i++)
    {
      mrb_idx = (mrb_info_ptr->mrb_info[i].mrb_id - (LTE_EMBMS_MCCH_RB_ID + 1));
      TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.released_mrb_cfg_idx[i] == mrb_idx);
    }
  }
  else if (cfg_for_release)
  {
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.num_added_mrb == 0);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.num_released_mrb == 3);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.released_mrb_cfg_idx[0] == 0);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.released_mrb_cfg_idx[1] == 1);
    TF_ASSERT(rlcdl_cfg_req_ptr->embms_cfg.released_mrb_cfg_idx[2] == 2);
  }
  else 
  {
    TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_EMBMS);
    /* Verify all MRB parameters in RLC DL */
    lte_rrc_llc_test_rlcdl_mrb_info(rlcdl_cfg_req_ptr);
  }

  /* After RLCDL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);

  TF_MSG("\nCRLCDL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_rlcul_cfg_req_and_send_cnf

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying RLC UL config request
    parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_rlcul_cfg_req_and_send_cnf
(
  lte_rrc_proc_e proc
)
{
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  uint32 buf_len = 0;

  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);

  /* Verify the config type based on the source proc */
  lte_rrc_llc_test_verify_l2_cfg_reason(proc, rlcul_cfg_req_ptr->cfg_reason);

  /* Verify all RB parameters in RLC DL */
  lte_rrc_llc_test_rlcul_rb_info(rlcul_cfg_req_ptr, proc);

  /* After RLCDL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);

  TF_MSG("\nCRLCUL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying RLC DL/UL config 
    request parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf
(
  lte_rrc_proc_e proc
)
{
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr = NULL;
  uint8 rb_direction = 0;

  /* Get the appropriate radio resource config */
  radio_resrc_cfg_ptr = lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr(proc);

  if ((proc != LTE_RRC_PROC_CRP) && (proc != LTE_RRC_PROC_CONTROLLER))
  {
    /* Get the direction of RBs present in the in the OTA msg */
    rb_direction = lte_rrc_llc_test_get_rlc_entity_to_modify_from_ota_msg(radio_resrc_cfg_ptr);
  }

  if (((proc == LTE_RRC_PROC_CRP) || (proc == LTE_RRC_PROC_CONTROLLER)) || 
      ((proc == LTE_RRC_PROC_CONFIG) && 
      (lte_rrc_llc_test_common_data.ho_in_progress)) ||
      (rb_direction == LTE_RRC_LLC_RB_BI_DIRECTION))
  {
    lte_rrc_llc_test_verify_rlcdl_cfg_req_and_send_cnf(proc);
    lte_rrc_llc_test_verify_rlcul_cfg_req_and_send_cnf(proc);
  }
  else if (rb_direction == LTE_RRC_LLC_RB_DL_DIRECTION)
  {
    lte_rrc_llc_test_verify_rlcdl_cfg_req_and_send_cnf(proc);
  }
  else if (rb_direction == LTE_RRC_LLC_RB_UL_DIRECTION)
  {
    lte_rrc_llc_test_verify_rlcul_cfg_req_and_send_cnf(proc);
  }
  else
  {
    return;
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pdcpul_recfg_prep_rb_info

===========================================================================*/
/*!
    @brief
    This function performs all the test RB info in PDCP UL reconfig prep request.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_pdcpul_recfg_prep_rb_info
(
  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_recfg_prep_req_ptr,
  lte_rrc_proc_e proc
)
{
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr = NULL;
  lte_rrc_osys_DRB_ToReleaseList drb_rel_list;
  uint8 del_rb_count = 0, i = 0, stored_rb_idx = 0, loop_count = 0;

  /* Get the appropriate radio resource config */
  radio_resrc_cfg_ptr = lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr(proc);

  /* Verify RRC Connection release scenario, when both SRBs and DRBs are released */
  if (radio_resrc_cfg_ptr == NULL)
  {
    /* For the RBs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if (lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION)
      {
        TF_ASSERT(pdcpul_recfg_prep_req_ptr->released_rb_cfg_idx[del_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
        del_rb_count++;
      }
    }
    TF_ASSERT(pdcpul_recfg_prep_req_ptr->num_released_rb == del_rb_count);
    TF_MSG("\n Verifying PDCP UL Reconfig prep request, connection release "
           "scenario verified successfully \n");
    /* All parameters for connection release have been tested, just return */
    return;
  }

  /* Verify DRB release during full config scenario */
  if (lte_rrc_llc_test_common_data.full_config)
  {
    /* For the RBs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if ((lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION) &&
          (lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx < LTE_MAX_RB) &&
          (lte_rrc_llc_test_drb_modified_in_ota_msg(radio_resrc_cfg_ptr,
                                                    lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx) == FALSE))
      {
        TF_ASSERT(pdcpul_recfg_prep_req_ptr->released_rb_cfg_idx[del_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
        del_rb_count++;
      }
    }
    TF_ASSERT(pdcpul_recfg_prep_req_ptr->num_released_rb == del_rb_count);
    TF_MSG("\n Verifying PDCP UL Reconfig prep request, full config scenario "
           "verified successfully \n");
  }

  /* If DRBsToRelease list is present, verify the DRBs released at PDCP */
  if ((proc != LTE_RRC_PROC_CEP) && 
      (lte_rrc_llc_test_common_data.full_config == FALSE) && 
      (radio_resrc_cfg_ptr->m.drb_ToReleaseListPresent))
  {
    drb_rel_list = radio_resrc_cfg_ptr->drb_ToReleaseList;
    while (loop_count < drb_rel_list.n)
    {
      stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_rel_list.elem[loop_count]);
      /* If the DRB is not found, ignore it and move to the next RB in the list */
      if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
      {
        loop_count++;
        continue;
      }

      if (lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].
          rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION)
      {
        TF_ASSERT(pdcpul_recfg_prep_req_ptr->released_rb_cfg_idx[del_rb_count] 
                  == drb_rel_list.elem[loop_count]);
        del_rb_count++;
      }
      loop_count++;
    }
    TF_MSG("\n Verifying PDCP UL Reconfig prep request, DRB release verified successfully \n");
  }
  TF_ASSERT(pdcpul_recfg_prep_req_ptr->num_released_rb == del_rb_count);
  TF_MSG("\n PDCP UL Reconfig prep request verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying PDCP UL reconfig 
    prep request parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf
(
  lte_rrc_proc_e proc
)
{
  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_recfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_recfg_prep_cnf;
  uint32 buf_len = 0;

  /* Catch the PDCPUL reconfig prep request sent by LLC to PDCPUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_recfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCPUL reconfig prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);

  /* Verify the config type based on the source proc */
  lte_rrc_llc_test_verify_l2_cfg_reason(proc, pdcpul_recfg_prep_req_ptr->cfg_reason);

  /* Verify all RB parameters in PDCP UL */
  lte_rrc_llc_test_pdcpul_recfg_prep_rb_info(pdcpul_recfg_prep_req_ptr, proc);

  /* After PDCPUL reconfig prep req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);

  TF_MSG("\nPDCPUL Reconfig Prep Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcpul_recfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_recfg_prep_cnf.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCPUL */
  pdcpul_recfg_prep_cnf.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_recfg_prep_cnf, sizeof(pdcpul_recfg_prep_cnf));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pdcpdl_srb_info

===========================================================================*/
/*!
    @brief
    This function tests the SRB info in PDCP DL config request.

    @return
    uint8, modified RB count

 */
/*=========================================================================*/
uint8 lte_rrc_llc_test_pdcpdl_srb_info
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr,
  lte_rrc_proc_e proc
)
{
  lte_rrc_osys_SRB_ToAddModList srb_list = radio_resrc_cfg_ptr->srb_ToAddModList;
  uint8 mod_rb_count = 0, srb_idx = 0, stored_rb_idx = 0, loop_count = 0;

  /* Iterate through the SRBToAddModifiedList */
  while(loop_count < srb_list.n)
  {
    srb_idx = srb_list.elem[loop_count].srb_Identity + LTE_MAX_RB;
    /* While processing first reconfig after CRE, verifying SRB1 should be skipped */
    if ((lte_rrc_llc_test_common_data.reconfig_after_cre) && 
        (srb_idx == LTE_RRC_LLC_SRB1_RB_CFG_IDX))
    {
      loop_count++;
      continue;
    }
    stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(srb_idx);
    /* If the SRB is not found, ignore it and move to the next RB in the list */
    if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
    {
      TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].rb_cfg_idx == srb_idx);
      TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].cfg_act == LTE_PDCP_CFG_ACT_ADD);
      mod_rb_count++;
    }
    else if ((proc == LTE_RRC_PROC_CRE) || (lte_rrc_llc_test_common_data.reconfig_after_cre))
    {
      /* If resuming SRB1 during CRE, or SRB2 during reconfig after CRE, check if the action
        is MODIFIED_RESUME */
      TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].rb_cfg_idx == srb_idx);
      TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].
                cfg_act == LTE_PDCP_CFG_ACT_MODIFIED_RESUME);
      mod_rb_count++;
    }
    TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_id == srb_list.elem[loop_count].srb_Identity);
    TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_cfg_idx == srb_idx);
    TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_mode == LTE_RB_RLC_MODE_AM);
    TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_type == LTE_RB_TYPE_SRB);
    TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rohc_mask == 0);
    TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].status_report == FALSE);
    TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].seq_len == LTE_RRC_LLC_DEFAULT_SRB_SEQ_LEN);
    loop_count++;
  }
  TF_MSG("\n Verifying PDCP DL RB info, SRB list verified successfully \n");

  return mod_rb_count;

}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_rohc_profile_mask

===========================================================================*/
/*!
    @brief
    This function tests the DRB info in PDCP DL config request.

    @return
    uint16

 */
/*=========================================================================*/
uint16 lte_rrc_llc_test_get_rohc_profile_mask
(
  lte_rrc_osys_PDCP_Config_headerCompression_rohc_profiles *rohc_profile_ptr
)
{
  uint16 rohc_mask = LTE_PDCP_ROHC_PROFILE_ID_0X0000;

  if (rohc_profile_ptr->profile0x0001)
  {
    rohc_mask |= LTE_PDCP_ROHC_PROFILE_ID_0X0001;
  }
  if (rohc_profile_ptr->profile0x0002)
  {
    rohc_mask |= LTE_PDCP_ROHC_PROFILE_ID_0X0002;
  }
  if (rohc_profile_ptr->profile0x0003)
  {
    rohc_mask |= LTE_PDCP_ROHC_PROFILE_ID_0X0003;
  }
  if (rohc_profile_ptr->profile0x0004)
  {
    rohc_mask |= LTE_PDCP_ROHC_PROFILE_ID_0X0004;
  }
  if (rohc_profile_ptr->profile0x0006)
  {
    rohc_mask |= LTE_PDCP_ROHC_PROFILE_ID_0X0006;
  }
  if (rohc_profile_ptr->profile0x0101)
  {
    rohc_mask |= LTE_PDCP_ROHC_PROFILE_ID_0X0101;
  }
  if (rohc_profile_ptr->profile0x0102)
  {
    rohc_mask |= LTE_PDCP_ROHC_PROFILE_ID_0X0102;
  }
  if (rohc_profile_ptr->profile0x0103)
  {
    rohc_mask |= LTE_PDCP_ROHC_PROFILE_ID_0X0103;
  }
  if (rohc_profile_ptr->profile0x0104)
  {
    rohc_mask |= LTE_PDCP_ROHC_PROFILE_ID_0X0104;
  }

  return rohc_mask;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pdcpdl_drb_info

===========================================================================*/
/*!
    @brief
    This function tests the DRB info in PDCP DL config request.

    @return
    uint8, modified RB count

 */
/*=========================================================================*/
uint8 lte_rrc_llc_test_pdcpdl_drb_info
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr
)
{
  lte_rrc_osys_DRB_ToAddModList drb_list = radio_resrc_cfg_ptr->drb_ToAddModList;
  uint8 mod_rb_count = 0, drb_idx = 0, stored_rb_idx = 0, loop_count = 0;

  /* Iterate through the DRBToAddModifiedList */
  while(loop_count < drb_list.n)
  {
    drb_idx = drb_list.elem[loop_count].drb_Identity;
    /* Determine the rb_idx if the DRB is already stored in the test DB */
    stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_idx);

    /* If PDCP config is present for the DRB and either the DRB has a DL direction or
      its an AM RB then it will have a PDCP DL entity parameters to verify */
    if ((drb_list.elem[loop_count].m.pdcp_ConfigPresent) &&
        (((drb_list.elem[loop_count].m.rlc_ConfigPresent) &&
        (drb_list.elem[loop_count].rlc_Config.t != T_lte_rrc_osys_RLC_Config_um_Uni_Directional_UL)) ||
         (drb_list.elem[loop_count].pdcp_Config.m.rlc_AMPresent)))
    {
      /* If the RB status is not established in the stored DB, or if
        the same DRB being released and added together, or if is a rel 9 
        full config and the EPS bearer bearer is already active, all PDCP DL 
        parameters need to be verified */
      if ((stored_rb_idx == LTE_MAX_ACTIVE_RB) ||
          ((lte_rrc_llc_test_drb_released_in_ota_msg(radio_resrc_cfg_ptr,
                                                     lte_rrc_llc_test_common_data.
                                                     rb_info[stored_rb_idx].rb_cfg_idx)) && 
           (lte_rrc_llc_test_drb_pdcp_cfg_modified_in_ota_msg(radio_resrc_cfg_ptr,
                                                              lte_rrc_llc_test_common_data.
                                                              rb_info[stored_rb_idx].rb_cfg_idx))) ||
          (lte_rrc_llc_test_common_data.full_config))
      {
        if ((lte_rrc_llc_test_common_data.full_config) && 
            (lte_rrc_llc_test_rb_id_is_active(drb_list.elem[loop_count].drb_Identity, 
                                              LTE_RRC_LLC_RB_DL_DIRECTION)))
        {
          TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);
        }
        else
        {
          TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].cfg_act == LTE_PDCP_CFG_ACT_ADD);
        }
        TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                  eps_id == drb_list.elem[loop_count].eps_BearerIdentity);
      }
      else if (lte_rrc_llc_test_common_data.reconfig_after_cre)
      {
        TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].
                  cfg_act == LTE_PDCP_CFG_ACT_MODIFIED_RESUME);
      }
      else
      {
        TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].
                  cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
      }

      TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_id == drb_idx);
      TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_cfg_idx == drb_idx);
      TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_type == LTE_RB_TYPE_DRB);

      if (drb_list.elem[loop_count].pdcp_Config.headerCompression.
          t == T_lte_rrc_osys_PDCP_Config_headerCompression_rohc)
      {
        TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                  rohc_max_cid == drb_list.elem[loop_count].pdcp_Config.headerCompression.u.rohc->maxCID);
        TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                  rohc_mask == lte_rrc_llc_test_get_rohc_profile_mask(&drb_list.elem[loop_count].pdcp_Config.
                                                                      headerCompression.u.rohc->profiles));
      }
      else
      {
        TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rohc_mask == LTE_PDCP_ROHC_PROFILE_ID_0X0000);
      }

      if (drb_list.elem[loop_count].pdcp_Config.m.rlc_AMPresent)
      {
        TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_mode == LTE_RB_RLC_MODE_AM);
        TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                  status_report == drb_list.elem[loop_count].pdcp_Config.rlc_AM.statusReportRequired);
        TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                  seq_len == LTE_RRC_LLC_LONG_DRB_SEQ_LEN);
      }
      else if (drb_list.elem[loop_count].pdcp_Config.m.rlc_UMPresent)
      {
        TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_mode == LTE_RB_RLC_MODE_UM);
        TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                  seq_len == ((drb_list.elem[loop_count].pdcp_Config.rlc_UM.pdcp_SN_Size == 0) ? 7 : 12));
      }
      mod_rb_count++;
    }
    /* Check if it is a Rel 9 full configuration */

    loop_count++;
  }
  TF_MSG("\n Verifying PDCP DL RB info, DRB list verified successfully \n");

  return mod_rb_count;

}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pdcpdl_rb_info

===========================================================================*/
/*!
    @brief
    This function performs all the test RB info in PDCP DL config request.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_pdcpdl_rb_info
(
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr,
  lte_rrc_proc_e proc
)
{
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr = NULL;
  lte_rrc_osys_DRB_ToReleaseList drb_rel_list;
  uint8 mod_rb_count = 0, del_rb_count = 0, i = 0, srb_idx = 0, drb_idx = 0, 
    stored_rb_idx = 0, loop_count = 0;

  /* Get the appropriate radio resource config */
  radio_resrc_cfg_ptr = lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr(proc);

  /* Verify RRC Connection release scenario, when both SRBs and DRBs are released */
  if (radio_resrc_cfg_ptr == NULL)
  {
    /* For the RBs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if (lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_DL_DIRECTION)
      {
        TF_ASSERT(pdcpdl_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
        del_rb_count++;
      }
    }
    TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == del_rb_count);
    TF_MSG("\n Verifying PDCP DL RB info, connection release scenario verified successfully \n");
    /* All parameters for connection release have been tested, just return */
    return;
  }

  /* Verify DRB release during full config scenario */
  if (lte_rrc_llc_test_common_data.full_config)
  {
    /* For the RBs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if ((lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_DL_DIRECTION) &&
          (lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx < LTE_MAX_RB) &&
          (lte_rrc_llc_test_drb_modified_in_ota_msg(radio_resrc_cfg_ptr,
                                                    lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx) == FALSE))
      {
        TF_ASSERT(pdcpdl_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
        del_rb_count++;
      }
    }
    TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == del_rb_count);
    TF_MSG("\n Verifying PDCP DL RB info, full config scenario verified successfully \n");
  }
  /* If DRBsToRelease list is present and it is not a full configuration scenario, 
    verify the DRBs released at PDCP. First check is needed so that we ignore 
    Connection Setup including DRB release list */
  else if ((proc != LTE_RRC_PROC_CEP) && 
      (radio_resrc_cfg_ptr->m.drb_ToReleaseListPresent))
  {
    drb_rel_list = radio_resrc_cfg_ptr->drb_ToReleaseList;
    while (loop_count < drb_rel_list.n)
    {
      stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_rel_list.elem[loop_count]);
      /* If the DRB is not found, ignore it and move to the next RB in the list */
      if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
      {
        loop_count++;
        continue;
      }

      if (lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].rb_direction & LTE_RRC_LLC_RB_DL_DIRECTION)
      {
        TF_ASSERT(pdcpdl_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] == drb_rel_list.elem[loop_count]);
        del_rb_count++;
      }
      loop_count++;
    }
    TF_MSG("\n Verifying PDCP DL RB info, DRB release verified successfully \n");
  }

  /* If DRBToAddModifiedList is present */
  if (lte_rrc_llc_test_common_data.reconfig_after_cre)
  {
    /* verify the PDCP Config parameters for DRBs before SRBs since the DRBs will
      be added to the add_modified list first */
    if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent)
    {
      mod_rb_count += lte_rrc_llc_test_pdcpdl_drb_info(radio_resrc_cfg_ptr, pdcpdl_cfg_req_ptr);
    }
  }
  else
  {
    /* Else, verify the PDCP Config parameters for SRBs before DRBs */
    if (radio_resrc_cfg_ptr->m.srb_ToAddModListPresent)
    {
      mod_rb_count += lte_rrc_llc_test_pdcpdl_srb_info(radio_resrc_cfg_ptr, pdcpdl_cfg_req_ptr, proc);
    }

    if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent)
    {
      mod_rb_count += lte_rrc_llc_test_pdcpdl_drb_info(radio_resrc_cfg_ptr, pdcpdl_cfg_req_ptr);
    }
  }

  /* While testing the first reconfig after CRE, verify if all SRB2 and all DRBs stored 
    in the test DB are being resumed */
  if (lte_rrc_llc_test_common_data.reconfig_after_cre)
  {
    loop_count = lte_rrc_llc_test_common_data.num_active_rb;
    for(i = 0; i < loop_count; i++)
    {
      if (lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_DL_DIRECTION)
      {
        /* If an SRB or a DRB is not being modified in the first reconfig after CRE, verify
          that it is still resumed at PDCP */
        if (lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx == LTE_RRC_LLC_SRB2_RB_CFG_IDX)
        {
          TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].
                    rb_cfg_idx == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
          TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].
                    cfg_act == LTE_PDCP_CFG_ACT_MODIFIED_RESUME);
          mod_rb_count++;
          
        }
        else if ((lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx <= LTE_MAX_RB) &&
                 (lte_rrc_llc_test_drb_pdcp_cfg_modified_in_ota_msg(radio_resrc_cfg_ptr, 
                                                                lte_rrc_llc_test_common_data.
                                                                rb_info[i].rb_cfg_idx) == FALSE))
        {
          /* Here verify if all active DRBs in the stored test DB , except the DRBs that are released 
            in the first reconfig message, are being resumed. */
          if (lte_rrc_llc_test_drb_released_in_ota_msg(radio_resrc_cfg_ptr,
                                                       lte_rrc_llc_test_common_data.
                                                       rb_info[i].rb_cfg_idx) == FALSE)
          {
            TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].
                      rb_cfg_idx == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
            TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[mod_rb_count].
                      cfg_act == LTE_PDCP_CFG_ACT_MODIFIED_RESUME);
            mod_rb_count++;
          }
        }
      }
    }
    TF_MSG("\n Verifying PDCP DL RB info, first reconfig after CRE verified successfully \n");
  }

  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == mod_rb_count);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == del_rb_count);
  TF_MSG("\n PDCP DL RB info verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_pdcpdl_cfg_req_and_send_cnf

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying PDCP DL config request
    parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_pdcpdl_cfg_req_and_send_cnf
(
  lte_rrc_proc_e proc
)
{
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;
  uint32 buf_len = 0;

  /* Catch the PDCPDL Config request sent by LLC to PDCPDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);

  /* Verify the config type based on the source proc */
  lte_rrc_llc_test_verify_l2_cfg_reason(proc, pdcpdl_cfg_req_ptr->cfg_reason);

  /* Verify all RB parameters in PDCP DL */
  lte_rrc_llc_test_pdcpdl_rb_info(pdcpdl_cfg_req_ptr, proc);

  /* After PDCPDL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);

  TF_MSG("\nPDCPDL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcpdl_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pdcpul_srb_info

===========================================================================*/
/*!
    @brief
    This function tests all SRB info in PDCP UL config request.

    @return
    uint8

 */
/*=========================================================================*/
uint8 lte_rrc_llc_test_pdcpul_srb_info
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr,
  lte_rrc_proc_e proc
)
{
  lte_rrc_osys_SRB_ToAddModList srb_list;
  uint8 mod_rb_count = 0, srb_idx = 0, stored_rb_idx = 0, loop_count = 0;

  srb_list = radio_resrc_cfg_ptr->srb_ToAddModList;
  /* Iterate through the SRBToAddModifiedList */
  while(loop_count < srb_list.n)
  {
    srb_idx = srb_list.elem[loop_count].srb_Identity + LTE_MAX_RB;
    /* While processing first reconfig after CRE, verifying SRB1 should be skipped */
    if ((lte_rrc_llc_test_common_data.reconfig_after_cre) && 
        (srb_idx == LTE_RRC_LLC_SRB1_RB_CFG_IDX))
    {
      loop_count++;
      continue;
    }

    stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(srb_idx);
    /* If the SRB is not found, ignore it and move to the next RB in the list */
    if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
    {
      TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].rb_cfg_idx == srb_idx);
      TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].cfg_act == LTE_PDCP_CFG_ACT_ADD);
      mod_rb_count++;
    }
    else if ((proc == LTE_RRC_PROC_CRE) || (lte_rrc_llc_test_common_data.reconfig_after_cre))
    {
      /* If resuming SRB1 during CRE, or SRB2 during reconfig after CRE, check if the action
        is MODIFIED_RESUME */
      TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].rb_cfg_idx == srb_idx);
      TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].
                cfg_act == LTE_PDCP_CFG_ACT_MODIFIED_RESUME);
      mod_rb_count++;
    }
    TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_id == srb_list.elem[loop_count].srb_Identity);
    TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_cfg_idx == srb_idx);
    TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_mode == LTE_RB_RLC_MODE_AM);
    TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rb_type == LTE_RB_TYPE_SRB);
    TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].
              discard_timer == LTE_RRC_LLC_DISCARD_TIMER_NOT_PRESENT);
    TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].rohc_mask == 0);
    TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[srb_idx].seq_len == LTE_RRC_LLC_DEFAULT_SRB_SEQ_LEN);
    loop_count++;
  }

  TF_MSG("\n Verifying PDCP UL RB info, SRB list verified successfully \n");

  return mod_rb_count;

}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pdcpul_drb_info

===========================================================================*/
/*!
    @brief
    This function tests all DRB info in PDCP UL config request.

    @return
    uint8

 */
/*=========================================================================*/
uint8 lte_rrc_llc_test_pdcpul_drb_info
(
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr,
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr
)
{
  lte_rrc_osys_DRB_ToAddModList drb_list;
  uint8 mod_rb_count = 0, drb_idx = 0, stored_rb_idx = 0, loop_count = 0;

  drb_list = radio_resrc_cfg_ptr->drb_ToAddModList;
  /* Iterate through the DRBToAddModifiedList */
  while(loop_count < drb_list.n)
  {
    drb_idx = drb_list.elem[loop_count].drb_Identity;
    /* Determine the rb_idx if the DRB is already stored in the test DB */
    stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_idx);

    /* If PDCP config is present for the DRB and either the DRB has a UL direction or
      its an AM RB then it will have a PDCP UL entity parameters to verify */
    if ((drb_list.elem[loop_count].m.pdcp_ConfigPresent) &&
        (((drb_list.elem[loop_count].m.rlc_ConfigPresent) &&
          (drb_list.elem[loop_count].rlc_Config.t != T_lte_rrc_osys_RLC_Config_um_Uni_Directional_DL)) ||
          (drb_list.elem[loop_count].pdcp_Config.m.rlc_AMPresent)))
    {
      /* If the RB status is not established in the stored DB, or if
        the same DRB being released and added together, or if is a rel 9 
        full config and the EPS bearer bearer is already active, all PDCP UL 
        parameters need to be verified */
      if ((stored_rb_idx == LTE_MAX_ACTIVE_RB) ||
          ((lte_rrc_llc_test_drb_released_in_ota_msg(radio_resrc_cfg_ptr,
                                                     lte_rrc_llc_test_common_data.
                                                     rb_info[stored_rb_idx].rb_cfg_idx)) && 
           (lte_rrc_llc_test_drb_pdcp_cfg_modified_in_ota_msg(radio_resrc_cfg_ptr,
                                                              lte_rrc_llc_test_common_data.
                                                              rb_info[stored_rb_idx].rb_cfg_idx))) ||
          (lte_rrc_llc_test_common_data.full_config))
      {
        if ((lte_rrc_llc_test_common_data.full_config) && 
            (lte_rrc_llc_test_rb_id_is_active(drb_list.elem[loop_count].drb_Identity,
                                              LTE_RRC_LLC_RB_UL_DIRECTION)))
        {
          TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);
        }
        else
        {
          TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].cfg_act == LTE_PDCP_CFG_ACT_ADD);
        }
        TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                  eps_id == drb_list.elem[loop_count].eps_BearerIdentity);
      }
      else if (lte_rrc_llc_test_common_data.reconfig_after_cre)
      {
        TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].
                  cfg_act == LTE_PDCP_CFG_ACT_MODIFIED_RESUME);          
      }
      else 
      {
        TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].
                  cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);          
      }
      TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_id == drb_idx);
      TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_cfg_idx == drb_idx);
      TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_type == LTE_RB_TYPE_DRB);
      if (drb_list.elem[loop_count].pdcp_Config.m.discardTimerPresent)
      {
        lte_rrc_llc_test_pdcp_discard_timer(&drb_list.elem[loop_count].pdcp_Config,
                                            &pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].discard_timer);
      }

      if (drb_list.elem[loop_count].pdcp_Config.headerCompression.
          t == T_lte_rrc_osys_PDCP_Config_headerCompression_rohc)
      {
        TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                  rohc_max_cid == drb_list.elem[loop_count].pdcp_Config.headerCompression.u.rohc->maxCID);
        TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rohc_mask 
                  == lte_rrc_llc_test_get_rohc_profile_mask(&drb_list.elem[loop_count].pdcp_Config.
                                                            headerCompression.u.rohc->profiles));
      }
      else
      {
        TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rohc_mask == LTE_PDCP_ROHC_PROFILE_ID_0X0000);
      }

      if (drb_list.elem[loop_count].pdcp_Config.m.rlc_AMPresent)
      {
        TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_mode == LTE_RB_RLC_MODE_AM);
        TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                  seq_len == LTE_RRC_LLC_LONG_DRB_SEQ_LEN);
      }
      else if (drb_list.elem[loop_count].pdcp_Config.m.rlc_UMPresent)
      {
        TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].rb_mode == LTE_RB_RLC_MODE_UM);
        TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[drb_idx].
                  seq_len == ((drb_list.elem[loop_count].pdcp_Config.rlc_UM.pdcp_SN_Size == 0) ? 7 : 12));
      }
      mod_rb_count++;
    }
    loop_count++;
  }
  TF_MSG("\n Verifying PDCP UL RB info, DRB list verified successfully \n");

  return mod_rb_count;

}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_pdcpul_rb_info

===========================================================================*/
/*!
    @brief
    This function performs all the test RB info in PDCP UL config request.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_pdcpul_rb_info
(
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr,
  lte_rrc_proc_e proc
)
{
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr = NULL;
  lte_rrc_osys_DRB_ToReleaseList drb_rel_list;
  uint8 mod_rb_count = 0, del_rb_count = 0, i = 0, srb_idx = 0, drb_idx = 0, 
    stored_rb_idx = 0, loop_count = 0;

  /* Get the appropriate radio resource config */
  radio_resrc_cfg_ptr = lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr(proc);

  /* Verify RRC Connection release scenario, when both SRBs and DRBs are released */
  if (radio_resrc_cfg_ptr == NULL)
  {
    /* For the RBs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if (lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION)
      {
        TF_ASSERT(pdcpul_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
        del_rb_count++;
      }
    }
    TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == del_rb_count);
    TF_MSG("\n Verifying PDCP UL RB info, connection release scenario verified successfully \n");
    /* All parameters for connection release have been tested, just return */
    return;
  }

  /* Verify DRB release during full config scenario */
  if (lte_rrc_llc_test_common_data.full_config)
  {
    /* For the RBs being released, check against the stored set of RBs in the test DB*/
    for (i = 0; i < LTE_MAX_ACTIVE_RB; i++)
    {
      if ((lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION) &&
          (lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx < LTE_MAX_RB) &&
          (lte_rrc_llc_test_drb_modified_in_ota_msg(radio_resrc_cfg_ptr,
                                                    lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx) == FALSE))
      {
        TF_ASSERT(pdcpul_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] 
                  == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
        del_rb_count++;
      }
    }
    TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == del_rb_count);
    TF_MSG("\n Verifying PDCP UL RB info, full config scenario verified successfully \n");
  }
  /* If DRBsToRelease list is present, verify the DRBs released at PDCP. First check is needed
    so that we ignore Connection Setup including DRB release list */
  else if ((proc != LTE_RRC_PROC_CEP) && 
      (radio_resrc_cfg_ptr->m.drb_ToReleaseListPresent))
  {
    drb_rel_list = radio_resrc_cfg_ptr->drb_ToReleaseList;
    while (loop_count < drb_rel_list.n)
    {
      stored_rb_idx = lte_rrc_llc_test_get_stored_idx_for_rb(drb_rel_list.elem[loop_count]);
      /* If the DRB is not found, ignore it and move to the next RB in the list */
      if (stored_rb_idx == LTE_MAX_ACTIVE_RB)
      {
        loop_count++;
        continue;
      }

      if (lte_rrc_llc_test_common_data.rb_info[stored_rb_idx].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION)
      {
        TF_ASSERT(pdcpul_cfg_req_ptr->released_rb_cfg_idx[del_rb_count] == drb_rel_list.elem[loop_count]);
        del_rb_count++;
      }
      loop_count++;
    }
    TF_MSG("\n Verifying PDCP UL RB info, DRB release verified successfully \n");
  }

  /* If DRBToAddModifiedList is present */
  if (lte_rrc_llc_test_common_data.reconfig_after_cre)
  {
    /* verify the PDCP Config parameters for DRBs before SRBs since the DRBs will
      be added to the add_modified list first */
    if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent)
    {
      mod_rb_count += lte_rrc_llc_test_pdcpul_drb_info(radio_resrc_cfg_ptr, pdcpul_cfg_req_ptr);
    }
  }
  else
  {
    /* Else, verify the PDCP Config parameters for SRBs before DRBs */
    if (radio_resrc_cfg_ptr->m.srb_ToAddModListPresent)
    {
      mod_rb_count += lte_rrc_llc_test_pdcpul_srb_info(radio_resrc_cfg_ptr, pdcpul_cfg_req_ptr, proc);
    }

    if (radio_resrc_cfg_ptr->m.drb_ToAddModListPresent)
    {
      mod_rb_count += lte_rrc_llc_test_pdcpul_drb_info(radio_resrc_cfg_ptr, pdcpul_cfg_req_ptr);
    }
  }

  /* While testing the first reconfig after CRE, verify if all SRB2 and all DRBs stored 
    in the test DB are being resumed */
  if (lte_rrc_llc_test_common_data.reconfig_after_cre)
  {
    loop_count = lte_rrc_llc_test_common_data.num_active_rb;
    for(i = 0; i < loop_count; i++)
    {
      if (lte_rrc_llc_test_common_data.rb_info[i].rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION)
      {
        /* If an SRB or a DRB is not being modified in the first reconfig after CRE, verify
          that it is still resumed at PDCP */
        if (lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx == LTE_RRC_LLC_SRB2_RB_CFG_IDX) 
        {
          TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].
                    rb_cfg_idx == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
          TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].
                    cfg_act == LTE_PDCP_CFG_ACT_MODIFIED_RESUME);
          mod_rb_count++;
          
        }
        else if ((lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx <= LTE_MAX_RB) &&
                 (lte_rrc_llc_test_drb_pdcp_cfg_modified_in_ota_msg(radio_resrc_cfg_ptr, 
                                                                lte_rrc_llc_test_common_data.
                                                                rb_info[i].rb_cfg_idx) == FALSE))
        {
          /* Here verify if all active RBs in the stored test DB , except the DRBs that are released 
            in the first reconfig message, are being resumed. */
          if (lte_rrc_llc_test_drb_released_in_ota_msg(radio_resrc_cfg_ptr,
                                                       lte_rrc_llc_test_common_data.
                                                       rb_info[i].rb_cfg_idx) == FALSE)
          {
            TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].
                      rb_cfg_idx == lte_rrc_llc_test_common_data.rb_info[i].rb_cfg_idx);
            TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[mod_rb_count].
                      cfg_act == LTE_PDCP_CFG_ACT_MODIFIED_RESUME);
            mod_rb_count++;
          }
        }
      }
    }
    TF_MSG("\n Verifying PDCP UL RB info, first reconfig after CRE verified successfully \n");
  }

  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == mod_rb_count);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == del_rb_count);
  TF_MSG("\n PDCP UL RB info verified successfully \n");
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_pdcpul_cfg_req_and_send_cnf

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying PDCP UL config request
    parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_pdcpul_cfg_req_and_send_cnf
(
  lte_rrc_proc_e proc
)
{
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;
  uint32 buf_len = 0;

  /* Catch the PDCPUL Config request sent by LLC to PDCPUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  /* Verify the config type based on the source proc */
  lte_rrc_llc_test_verify_l2_cfg_reason(proc, pdcpul_cfg_req_ptr->cfg_reason);

  /* Verify all RB parameters in PDCP UL */
  lte_rrc_llc_test_pdcpul_rb_info(pdcpul_cfg_req_ptr, proc);

  /* After PDCPUL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);

  TF_MSG("\nPDCPUL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcpul_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf

===========================================================================*/
/*!
    @brief
    This function performs all the test steps for verifying PDCP DL/UL config 
    request parameters and sends confirm back to LLC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf
(
  lte_rrc_proc_e proc
)
{
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_cfg_ptr = NULL;
  uint8 rb_direction = LTE_RRC_LLC_RB_NOT_ESTABLISHED; 

  /* Get the appropriate radio resource config */
  radio_resrc_cfg_ptr = lte_rrc_llc_test_get_RadioResourceConfigDedicated_ie_ptr(proc);

  if ((proc != LTE_RRC_PROC_CRP) && (proc != LTE_RRC_PROC_CONTROLLER))
  {
    /* Get the direction of RBs present in the in the OTA msg */
    rb_direction = lte_rrc_llc_test_get_rlc_entity_to_modify_from_ota_msg(radio_resrc_cfg_ptr);
  }

  if (lte_rrc_llc_test_common_data.reconfig_after_cre)
  {
    /* Get the direction of RBs that are suspended, that should be resumed */
    rb_direction = lte_rrc_llc_test_get_rb_direction_of_suspended_rbs();
  }

  if (((proc == LTE_RRC_PROC_CRP) || (proc == LTE_RRC_PROC_CONTROLLER)) || 
      ((proc == LTE_RRC_PROC_CONFIG) && 
      (lte_rrc_llc_test_common_data.ho_in_progress)) ||
      (rb_direction == LTE_RRC_LLC_RB_BI_DIRECTION))
  {
    lte_rrc_llc_test_verify_pdcpdl_cfg_req_and_send_cnf(proc);
    lte_rrc_llc_test_verify_pdcpul_cfg_req_and_send_cnf(proc);
  }
  else if (rb_direction == LTE_RRC_LLC_RB_DL_DIRECTION)
  {
    lte_rrc_llc_test_verify_pdcpdl_cfg_req_and_send_cnf(proc);
  }
  else if (rb_direction == LTE_RRC_LLC_RB_UL_DIRECTION)
  {
    lte_rrc_llc_test_verify_pdcpul_cfg_req_and_send_cnf(proc);
  }
  else
  {
    return;
  }
}


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_verify_cfg_cnfi

===========================================================================*/
/*!
    @brief
    This function performs the test steps for verifying LTE_RRC_CFG_CNFI.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_verify_cfg_cnfi
(
  lte_errno_e status,
  lte_rrc_proc_e proc,
  uint32 *trans_id_ptr
)
{
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  uint32 buf_len;
  
  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == proc);
  if (trans_id_ptr == NULL)
  {
    TF_ASSERT(cfg_cnfi_ptr->trans_id == lte_rrc_llc_test_common_data.trans_id[0]);
  }
  else
  {
    TF_ASSERT(cfg_cnfi_ptr->trans_id == *trans_id_ptr);
  }
  TF_ASSERT(cfg_cnfi_ptr->status == status);
  if (status == E_SUCCESS)
  {
    TF_MSG("\nLLC: Rcvd LTE_RRC_CFG_CNFI with success \n");
    if (proc == LTE_RRC_PROC_CONFIG)
    {
      /* Reset the reconfig after cre flag in test DB, when the first reconfig
        after Reestablishment has been processed successfully */
      lte_rrc_llc_test_common_data.reconfig_after_cre = FALSE;
      lte_rrc_llc_test_common_data.ho_in_progress = FALSE;
    }
  }
  else
  {
    TF_MSG("\nLLC: Rcvd LTE_RRC_CFG_CNFI with failure \n");
  }
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_basic_sib2_update_for_invalid_ul_freq

===========================================================================*/
/*!
    @brief
    This function tests the basic SIB2 update processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_basic_sib2_update_for_invalid_ul_freq
(
  void
)
{
  lte_rrc_cfg_reqi_s cfg_reqi;
  uint32 trans_id;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  
  if(LTE_RRC_LLC_TEST_COMMON_CFG_NULL == lte_rrc_llc_test_common_cfg_sts)
  {
    common_cfg_mask = 
    (
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
      LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
      LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG
    );
    lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  }
  
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: Basic SIB2 Update Test Begins \n");

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  TF_MSG("\nLLC: Send CFG_REQI for SIB2 Update \n");
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, TRUE, TRUE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();   

  if (LTE_RRC_LLC_TEST_COMMON_CFG_NEW == lte_rrc_llc_test_common_cfg_sts)
  {
    /* With sib2_dup set to 1, we are not expecting CPHY Common Cfg Req upon SIB2 update */
    lte_rrc_llc_test_common_cfg_sts = LTE_RRC_LLC_TEST_COMMON_CFG_OLD;
  }  
  else
  {
    common_cfg_mask = LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG | LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG;
    lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  }
  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_sib_ptr();
  TF_MSG("\nLLC: Basic SIB2 Update Test Ends \n");

} /* lte_rrc_llc_test_basic_sib2_update_for_invalid_ul_freq() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_basic_sib2_update

===========================================================================*/
/*!
    @brief
    This function tests the basic SIB2 update processing by LLC.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_basic_sib2_update
(
  void
)
{
  lte_rrc_cfg_reqi_s cfg_reqi;
  uint32 trans_id;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  
  if(LTE_RRC_LLC_TEST_COMMON_CFG_NULL == lte_rrc_llc_test_common_cfg_sts)
  {
    common_cfg_mask = 
    (
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
      LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
      LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG |
      LTE_CPHY_COMMON_CFG_MASK_BIT_PRACH_CFG
    );
    lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  }
  
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: Basic SIB2 Update Test Begins \n");

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  TF_MSG("\nLLC: Send CFG_REQI for SIB2 Update \n");
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, TRUE, TRUE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();   

  if (LTE_RRC_LLC_TEST_COMMON_CFG_NEW == lte_rrc_llc_test_common_cfg_sts)
  {
    /* With sib2_dup set to 1, we are not expecting CPHY Common Cfg Req upon SIB2 update */
    lte_rrc_llc_test_common_cfg_sts = LTE_RRC_LLC_TEST_COMMON_CFG_OLD;
  }  
  else
  {
    common_cfg_mask = LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG | LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG;
    lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  }

  /* Verify phy common config request */
  lte_rrc_llc_test_verify_phy_common_cfg_req_and_send_cnf();

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CSP, TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(TRUE, NULL, LTE_RRC_PROC_CSP);

  

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_sib_ptr();
  TF_MSG("\nLLC: Basic SIB2 Update Test Ends \n");

} /* lte_rrc_llc_test_basic_sib2_update() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg

===========================================================================*/
/*!
    @brief
    This function tests the basic SIB2 update processing by LLC where MBSFN Config 
    is included and SRS Common Config is not signalled. In this case the 
    CPHY_COMMON_REQ sent by LLC should indicate the disabling of SRS Config 
    Common so that CPHY will stop using the config they have.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg
(
  void
)
{
  lte_rrc_cfg_reqi_s cfg_reqi;
  uint32 trans_id;
  lte_cphy_common_cfg_mask_t common_cfg_mask;
  if(LTE_RRC_LLC_TEST_COMMON_CFG_NULL == lte_rrc_llc_test_common_cfg_sts)
  {
  common_cfg_mask = 
  (
  	LTE_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG |
    //LTE_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PUCCH_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_SRS_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL |
    //LTE_CPHY_COMMON_CFG_MASK_BIT_DRX_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH |
    LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG |
    LTE_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM |
    LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG
  );
  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  }

  /* ---------------------------------------------------------------------- */

  TF_MSG("\nLLC: Basic SIB2 Update MBSFN and SRS Common Discontinue Test Begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  TF_MSG("\nLLC: Send CFG_REQI for SIB2 Update \n");
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, TRUE, TRUE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  if (LTE_RRC_LLC_TEST_COMMON_CFG_NEW == lte_rrc_llc_test_common_cfg_sts)
  {
    /* With sib2_dup set to 1, we are not expecting CPHY Common Cfg Req upon SIB2 update */
    lte_rrc_llc_test_common_cfg_sts = LTE_RRC_LLC_TEST_COMMON_CFG_OLD;
  }  
  else
  {
    common_cfg_mask = LTE_CPHY_COMMON_CFG_MASK_BIT_MBSFN_CFG | LTE_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG;
    lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif(common_cfg_mask);
  }

  /* Verify phy common config request */
  lte_rrc_llc_test_verify_phy_common_cfg_req_and_send_cnf();

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CSP, TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(TRUE, NULL, LTE_RRC_PROC_CSP);

  
  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_free_sib_ptr();
  TF_MSG("\nLLC: Basic SIB2 Update MBSFN and SRS Common Discontinue Test Ends \n");

} /* lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_connection_setup_default

===========================================================================*/
/*!
    @brief
    This tests connection setup handling in LLC.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_connection_setup
(
  lte_rrc_pdu_buf_s encoded_pdu
)
{
  TF_MSG("\nConnection Setup Test Begins \n");

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CEP, FALSE);

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CEP);

   /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CEP);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CEP, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Setup Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();

} /* lte_rrc_llc_test_rrc_connection_setup() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_connection_setup_for_tm9

===========================================================================*/
/*!
    @brief
    This tests connection setup handling in LLC.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_connection_setup_for_tm9
(
  lte_rrc_pdu_buf_s encoded_pdu
)
{
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  uint32 buf_len;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CEP, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CEP);

   /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CEP);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CEP, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Setup Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();

} /* lte_rrc_llc_test_rrc_connection_setup_for_tm9() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_connection_setup_default

===========================================================================*/
/*!
    @brief
    This tests connection setup with default configurations.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_connection_setup_default
(
  void
)
{

  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Setup default IEs Test Begins \n");

  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_conn_setup_default_ies);
  encoded_pdu.value = llc_conn_setup_default_ies;

  lte_rrc_llc_test_rrc_connection_setup(encoded_pdu);

  TF_MSG("\nConnection Setup default IEs Test Ends \n");

} /* lte_rrc_llc_test_rrc_connection_setup_default() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband

===========================================================================*/
/*!
    @brief
    This tests connection setup with mentioned config.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband
(
  void
)
{

  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
uint32 buf_len;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Setup IEs Test Begins \n");

  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_conn_setup_with_tm1_csirs_enabled_antport_1_cqi_subband);
  encoded_pdu.value = llc_conn_setup_with_tm1_csirs_enabled_antport_1_cqi_subband;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CEP, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CEP);

   /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CEP);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CEP, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Setup Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  

  TF_MSG("\nConnection Setup IEs Test Ends \n");

} /* lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_connection_setup_with_r10_antenna_selection

===========================================================================*/
/*!
    @brief
    This tests connection setup with mentioned config.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_connection_setup_with_r10_antenna_selection
(
  void
)
{

  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
uint32 buf_len;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Setup IEs Test Begins \n");

  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_conn_setup_with_r10_antenna_selection);
  encoded_pdu.value = llc_conn_setup_with_r10_antenna_selection;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CEP, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CEP);

   /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CEP);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CEP, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Setup Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  

  TF_MSG("\nConnection Setup IEs Test Ends \n");

} /* lte_rrc_llc_test_rrc_connection_setup_with_r10_antenna_selection() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_connection_setup_with_apsrs

===========================================================================*/
/*!
    @brief
    This tests connection setup with mentioned config.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_connection_setup_with_apsrs
(
  void
)
{

  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
uint32 buf_len;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Setup IEs Test Begins \n");

  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_conn_setup_with_ap_srs);
  encoded_pdu.value = llc_conn_setup_with_ap_srs;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CEP, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CEP);

   /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CEP);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CEP, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Setup Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  

  TF_MSG("\nConnection Setup IEs Test Ends \n");

} /* lte_rrc_llc_test_rrc_connection_setup_with_apsrs() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_connection_setup_with_r8_antenna_selection

===========================================================================*/
/*!
    @brief
    This tests connection setup with mentioned config.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_connection_setup_with_r8_antenna_selection
(
  void
)
{

  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
uint32 buf_len;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Setup IEs Test Begins \n");

  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_conn_setup_with_r8_antenna_selection);
  encoded_pdu.value = llc_conn_setup_with_r8_antenna_selection;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CEP, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CEP);

   /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CEP);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CEP, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Setup Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  

  TF_MSG("\nConnection Setup IEs Test Ends \n");

} /* lte_rrc_llc_test_rrc_connection_setup_with_r8_antenna_selection() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_connection_setup_tm4_cqi_aperiodic_m22_antport_1

===========================================================================*/
/*!
    @brief
    This tests connection setup with mentioned config.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_connection_setup_tm4_cqi_aperiodic_m22_antport_4
(
  void
)
{

  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
uint32 buf_len;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Setup IEs Test Begins \n");

  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_conn_setup_with_tm4_cqi_aperiodic_m22_antport_4);
  encoded_pdu.value = llc_conn_setup_with_tm4_cqi_aperiodic_m22_antport_4;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CEP, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CEP);

   /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CEP);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CEP, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Setup Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  

  TF_MSG("\nConnection Setup IEs Test Ends \n");

} /* lte_rrc_llc_test_rrc_connection_setup_tm4_cqi_aperiodic_m22_antport_1() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_connection_setup_tm1_cqi_aperiodic_m30_antport_1

===========================================================================*/
/*!
    @brief
    This tests connection setup with mentioned config.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_connection_setup_tm1_cqi_aperiodic_m30_antport_1
(
  void
)
{

  lte_rrc_pdu_buf_s encoded_pdu;
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
uint32 buf_len;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Setup IEs Test Begins \n");

  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_conn_setup_with_tm1_cqi_aperiodic_m30_antport_1);
  encoded_pdu.value = llc_conn_setup_with_tm1_cqi_aperiodic_m30_antport_1;


  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CEP, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CEP);

   /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CEP);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CEP);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CEP, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Setup Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  

  TF_MSG("\nConnection Setup IEs Test Ends \n");

} /* lte_rrc_llc_test_rrc_connection_setup_tm1_cqi_aperiodic_m30_antport_1() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_release

===========================================================================*/
/*!
    @brief
    This function performs all connection release related checks for 
    Bi-directional DRB release.

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_llc_test_release 
(
  lte_rrc_proc_e proc, /*! Source RRC proc */
  boolean release_phy, /*! Flag to indicate if phy release is required */
  uint32 cell_id  /*! Cell id where phy release is done */
)
{
  lte_cphy_con_release_req_s* cphy_con_release_ptr = NULL;
  lte_cphy_con_release_cnf_s cphy_con_release_cnf;
  uint32 buf_len = 0;

  /* Verify PDCP UL RECFG PREP REQ */
  lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(proc);

  if (release_phy)
  {
    /* Verify MAC access abort request */
    lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
    /*--------------------------------------------------------------------------
                              CPHY_CON_RELEASE Handling Test
    ---------------------------------------------------------------------------*/
    /* Catch the CPHY con releae request sent by LLC to CPHY */
    lte_rrc_utf_get_next_ext_msg((byte **)&cphy_con_release_ptr,&buf_len);
    /* Make sure the con release request is valid */
    TF_ASSERT(sizeof(lte_cphy_con_release_req_s) == buf_len);
    /* Make sure the CPHY_CON_RELEASE_REQ is valid */
    TF_ASSERT(sizeof(lte_cphy_con_release_req_s) == buf_len);
    TF_ASSERT(cphy_con_release_ptr->cell_info.dl_cell_frequency ==  500);
    TF_ASSERT(cphy_con_release_ptr->cell_info.phy_cell_id ==  503);

    /* After sending con release req, LLC should be in the WAIT_FOR_CPHY_CFG_CNF state */
    TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
    TF_MSG("\nCPHY con release request sent to CPHY and LLC Module in WAIT_FOR_CPHY_CON_RELEASE_CNF state\n");

    /* Construct and send the LTE_CPHY_CON_RELEASE_CNF message back to LLC */
    lte_rrc_init_default_hdr(&cphy_con_release_cnf.msgr_hdr, LTE_CPHY_CON_RELEASE_CNF);
    /* Setting the fields in the LTE_CPHY_CON_RELEASE_CNF sent by CPHY */
    cphy_con_release_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cphy_con_release_cnf, sizeof(lte_cphy_con_release_cnf_s));
  }

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, proc);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(proc);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(proc);

  /* If Release is during reconfiguration processing */
  if (proc == LTE_RRC_PROC_CONFIG)
  {
    /* Verify DRX opt request being sent to ML1 */
    lte_rrc_llc_test_verify_drx_opt_req();
  }

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(proc);

} /* lte_rrc_llc_test_release() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_conn_release

===========================================================================*/
/*!
    @brief
    This tests connection releases all active RBs.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_conn_release
(
  boolean release_phy /*! Flag to indicate if phy release is required */
)
{
  uint32 cell_id = 0;

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* For HO scenarios, cell id of the target cell should be verified. Cell id
    has to be determined before LLC processes the connection release request
    when ho in progress flag will be reset */
  if (lte_rrc_llc_data.dd_ptr->oc_ptr->ho_config_in_progress)
  {
    cell_id = 10;
  }
  else
  {
    /* For other scenarios, cell id of the source cell should be verified */
    cell_id = 503;
  }

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(NULL, LTE_RRC_PROC_CRP, FALSE);
  lte_rrc_llc_test_release(LTE_RRC_PROC_CRP, release_phy, cell_id);
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRP, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nConn setup config release Test Ends \n");

} /*lte_rrc_llc_test_conn_release*/


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration with SRB2 and a UM
    bi-directional DRB3 setup.

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  /* Setting the encoded message to the byte string corresponding to radio 
     resource configuration. Please refer to lte_rrc_llc_ota_msg.h for 
     all LLC OTA message definitions */ 
  encoded_pdu.length = sizeof(llc_reconfig_msg);
  encoded_pdu.value = llc_reconfig_msg;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nConnection Reconfiguration Test Ends \n");

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
} /* lte_rrc_llc_test_rrc_connection_reconfiguration_srb2_drb3_setup() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_setup_drb_um_bi

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration with a UM bi-directional 
    DRB setup.

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
void lte_rrc_llc_test_setup_drb_um_bi
(
  lte_rb_id_t drb_id,
  lte_lc_id_t lc_id
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration DRB3 Setup Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_drb3_setup_msg);
  encoded_pdu.value = llc_reconfig_drb3_setup_msg;
  
  /* Construct asn1 pdu to be sent to the Config module after decoding the encoded pdu*/
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* Set the DRB id based on the passed parameter drb_id */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
    rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->drb_Identity = drb_id;

  /* Set the EPS bearer id based on the passed parameter drb_id */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
    rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->eps_BearerIdentity = drb_id;

  /* Set the LC id based on the passed parameter lc_id */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
    rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->logicalChannelIdentity = lc_id;

  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = dl_dcch_msg_ptr;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_setup_drb3_um_bi() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_dl

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration with SRB2 and a UM
    DL DRB3 setup.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_dl
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");
 
  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_drb_dl_um_msg);
  encoded_pdu.value = llc_reconfig_drb_dl_um_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();

} /* lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_dl() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_ul

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration with SRB2 and a UM
    UL DRB3 setup.

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_ul
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_drb_ul_um_msg);
  encoded_pdu.value = llc_reconfig_drb_ul_um_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();

} /* lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_ul() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_setup_srb2_drb3_am

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration with SRB2 and a AM
    bi-directional DRB3 setup.

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_setup_srb2_drb3_am
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_drb_am_msg);
  encoded_pdu.value = llc_reconfig_drb_am_msg;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);
  
  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_reconfig_setup_srb2_drb3_am() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_setup_duplicate_drb

===========================================================================*/
/*!
    @brief
    This API tests connection reconfiguration that includes duplicate DRB
    configuration.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_setup_duplicate_drb
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;
  uint32 buf_len = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_duplicate_drb_msg);
  encoded_pdu.value = llc_reconfig_duplicate_drb_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);  

  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RECFG);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 2);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_ADD);

  /* Verify the default values for DL RLC for rb 2*/
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[34].lc_id == 2);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[34].rb_id == 2);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[34].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[34].rb_mode == LTE_RB_RLC_MODE_AM);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[34].rb_type == LTE_RB_TYPE_SRB);

  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].lc_id == 10);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_id == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_type == LTE_RB_TYPE_DRB);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].cfg.um_cfg.sn_length == 5);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].cfg.um_cfg.t_reordering == 25);

  /* After RLCDL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);
  TF_MSG("\nRLCDL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RECFG);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_ADD);

  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[34].lc_id == 2);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[34].rb_id == 2);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[34].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[34].rb_mode == LTE_RB_RLC_MODE_AM);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[34].rb_type == LTE_RB_TYPE_SRB);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[34].pdcp_discard_timer == LTE_RRC_LLC_DISCARD_TIMER_NOT_PRESENT);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[34].pdcp_status_required == FALSE);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[34].cfg.am_cfg.max_retx_threshold == 4);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[34].cfg.am_cfg.poll_byte == LTE_OTA_IE_INFINITY_UINT32);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[34].cfg.am_cfg.poll_pdu == LTE_OTA_IE_INFINITY_UINT16);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[34].cfg.am_cfg.t_poll_retransmit == 45); 

  /* After RLCUL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);
  TF_MSG("\nRLCUL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RECFG);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 2);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_ADD);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_ADD);

  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[34].rb_id == 2);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[34].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[34].rb_mode == LTE_RB_RLC_MODE_AM);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[34].rb_type == LTE_RB_TYPE_SRB);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[34].rohc_mask == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[34].status_report == FALSE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[34].seq_len == LTE_RRC_LLC_DEFAULT_SRB_SEQ_LEN);

  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[3].eps_id == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_id == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_type == LTE_RB_TYPE_DRB);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[3].rohc_mask == LTE_PDCP_ROHC_PROFILE_ID_0X0000);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[3].seq_len == LTE_RRC_LLC_SHORT_DRB_SEQ_LEN);
  //TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[3].rohc_max_cid == 1);
  // FFS - Need to define in the interface
  //TF_ASSERT(pdcpdl_cfg_req_ptr->rb_cfg_base_ptr[3].rohc_mask == 0);
  /* After PDCPDL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCPDL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 1);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_ADD);
  TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[34].discard_timer == LTE_RRC_LLC_DISCARD_TIMER_NOT_PRESENT);
  TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[34].rb_id == 2);
  TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[34].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[34].rb_mode == LTE_RB_RLC_MODE_AM);
  TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[34].rb_type == LTE_RB_TYPE_SRB);
  TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[34].seq_len == LTE_RRC_LLC_DEFAULT_SRB_SEQ_LEN);
  TF_ASSERT(pdcpul_cfg_req_ptr->rb_cfg_base_ptr[34].rohc_mask == 0);

  /* After PDCPUL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCPUL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();
  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_reconfig_setup_duplicate_drb() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_same_drb_dl_released_ul_setup

===========================================================================*/
/*!
    @brief
    This API tests connection reconfiguration that releases a DL DRB and sets up
    the same DRB (same RB ID) as a UL DRB.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_same_drb_dl_released_ul_setup
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_same_drb_dl_released_ul_setup_msg);
  encoded_pdu.value = llc_reconfig_same_drb_dl_released_ul_setup_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);  

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_reconfig_same_drb_dl_released_ul_setup() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_same_drb_released_setup

===========================================================================*/
/*!
    @brief
    This API tests connection reconfiguration that releases a UM bi-directional 
    DRB and sets up the same um bi-directional DRB (same RB ID).

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_same_drb_released_setup
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_same_drb_released_setup_msg);
  encoded_pdu.value = llc_reconfig_same_drb_released_setup_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);  

  /* Verify PDCP UL RECFG PREP REQ */
  lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_reconfig_same_drb_released_setup() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_srb1_setup_srb2

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration with SRB1 being reconfigured
    and SRB2 being setup.

    @return
    None
 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_srb1_setup_srb2
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nReconfig SRB1, setup SRB2 test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_config_srb1_recfg_srb2_setup);
  encoded_pdu.value = llc_config_srb1_recfg_srb2_setup;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nReconfig SRB1, setup SRB2 test Begins \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();

} /* lte_rrc_llc_test_reconfig_srb1_setup_srb2() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_srb1_srb2_drb3

===========================================================================*/
/*!
    @brief
    This tests RRC connection reconfiguration message that reconfigures SRB1, 
    SRB2 and DRB3.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_srb1_srb2_drb3
(
  void
)
{

  lte_rrc_pdu_buf_s encoded_pdu;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nReconfig SRB1, SRB2 and DRB3 Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_srb1_srb2_drb3_msg);
  encoded_pdu.value = llc_reconfig_srb1_srb2_drb3_msg;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  if (lte_rrc_llc_test_common_data.reconfig_after_cre)
  {
    /* Verify PDCP DL/UL config requests */
    lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);
  }

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nReconfig SRB1, SRB2 and DRB3 Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();

} /* lte_rrc_llc_test_reconfig_srb1_srb2_drb3() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_phy_mac_drb3_rlc

===========================================================================*/
/*!
    @brief
    This tests RRC connection reconfiguration message that reconfigures PHY,
    MAC and RLC configuration for DRB3.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_phy_mac_drb3_rlc
(
  void
)
{

  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration - Reconfigure PHY, MAC, DRB3 RLC Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_phy_mac_drb3_rlc_msg);
  encoded_pdu.value = llc_reconfig_phy_mac_drb3_rlc_msg;
    
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);  
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG, FALSE);
  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration - Reconfigure PHY, MAC, DRB3 RLC Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_reconfig_phy_mac_drb3_rlc() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_phyded_cfg

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration that reconfigures the IEs
    in PhysicalConfigDedicated.

    @parameter
    boolean - if set to TRUE, the test only verifies the PHY parameters, else
    all IEs in PhysicalConfigDedicated including the ones that are sent to MAC.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_phyded_cfg
(
    boolean phy_only_reconfig   /*! If the reconfig is for PHY only */
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  /* Set the MIMO config */
  TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_mimo;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  if (phy_only_reconfig == TRUE)
  {
    encoded_pdu.length = sizeof(llc_reconfig_rel9_phy_msg);
    encoded_pdu.value = llc_reconfig_rel9_phy_msg;

    /* Set the release version to rel 9 */
    TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
	TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
	lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;
  }
  else
  {
    encoded_pdu.length = sizeof(llc_reconfig_phyded_msg);
    encoded_pdu.value = llc_reconfig_phyded_msg;
  }
    
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);  

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG, FALSE);

  /* Only if MAC related IEs are also present, MAC request needs to be tested */
  if (phy_only_reconfig != TRUE)
  {
    /* Verify MAC config request */
    lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);
  }

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();

} /* lte_rrc_llc_test_reconfig_phyded_cfg() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_cqi_fmt_indi

===========================================================================*/
/*!
    @brief
    This API tests CQI reporting periodic format indicator

 */
/*=========================================================================*/

void lte_rrc_llc_test_cqi_fmt_indi
(
    boolean phy_only_reconfig   /*! If the reconfig is for PHY only */
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  /* Set the MIMO config */
  TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_mimo;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");
  encoded_pdu.length = sizeof(llc_reconfig_phyded_msg);
  encoded_pdu.value = llc_reconfig_phyded_msg;
    
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

} /* lte_rrc_llc_test_reconfig_phyded_cfg() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_trch_cfg

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration that reconfigures transport
    channel reconfiguration only.

    @parameter
    boolean - if set to TRUE, the test only verifies the MAC parameters, else
    all transport channel parameters including the ones that are sent to CPHY,

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_trch_cfg
(
  boolean mac_only_reconfig
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  if (mac_only_reconfig)
  {
    encoded_pdu.length = sizeof(llc_reconfig_mac_msg);
    encoded_pdu.value = llc_reconfig_mac_msg;
  }
  else
  {
    encoded_pdu.length = sizeof(llc_reconfig_trch_msg);
    encoded_pdu.value = llc_reconfig_trch_msg;
  }

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  if (!mac_only_reconfig)
  {
    /* Verify phy dedicated config request */
    lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG, FALSE);
  }
  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_reconfig_trch_cfg() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mac_release_resources_ind

===========================================================================*/
/*!
    @brief
    This API tests the handling of LTE_MAC_RELEASE_RESOURCES_IND by LLC

    @parameter
    void

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mac_release_resources_ind
(
  void
)
{
  lte_mac_release_resources_ind_s mac_rel_resrc_ind;
  lte_cphy_dedicated_cfg_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_cfg_cnf_msg;
  lte_mac_cfg_req_msg_s *mac_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;

  uint32 buf_len = 0;

  /* Finally send an out of sync req to LLC*/
  lte_rrc_init_default_hdr(&mac_rel_resrc_ind.hdr, LTE_MAC_RELEASE_RESOURCES_IND);
  lte_rrc_utf_send_msg((byte*)&mac_rel_resrc_ind, sizeof(mac_rel_resrc_ind));

  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(cphy_cfg_req_ptr != NULL);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);
  TF_ASSERT(cphy_cfg_req_ptr != NULL);
  TF_ASSERT(cphy_cfg_req_ptr->dedicated_cfg_param.cfg_mask ==
            (LTE_CPHY_DEDICATED_CFG_MASK_BIT_SRS_CFG |
             LTE_CPHY_DEDICATED_CFG_MASK_BIT_CQI_CFG |
             LTE_CPHY_DEDICATED_CFG_MASK_BIT_SCHEDULING_REQ));

  TF_ASSERT(cphy_cfg_req_ptr->dedicated_cfg_param.cqi_cfg.enable == TRUE);
  TF_ASSERT(cphy_cfg_req_ptr->dedicated_cfg_param.cqi_cfg.cqi_reporting_periodic_present == TRUE);
  TF_ASSERT(cphy_cfg_req_ptr->dedicated_cfg_param.cqi_cfg.cqi_reporting_periodic.enable == FALSE);
  TF_ASSERT(cphy_cfg_req_ptr->dedicated_cfg_param.srs_cfg_dedicated.srs_periodic_cfg_ded.enable == FALSE);
  TF_ASSERT(cphy_cfg_req_ptr->dedicated_cfg_param.scheduling_req_cfg.enable == FALSE);

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);

  cphy_cfg_cnf_msg.dedicated_cfg_validity = 
    (LTE_CPHY_DEDICATED_CFG_MASK_BIT_SRS_CFG |
     LTE_CPHY_DEDICATED_CFG_MASK_BIT_CQI_CFG |
     LTE_CPHY_DEDICATED_CFG_MASK_BIT_SCHEDULING_REQ);
  cphy_cfg_cnf_msg.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  lte_rrc_utf_get_next_ext_msg((byte **) &mac_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  /* LLC should be in WAIT_FOR_MAC_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);
  TF_ASSERT(mac_req_ptr != NULL);
  TF_ASSERT(mac_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_UL_CFG_BM);
  TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->sr_cfg_included == TRUE);
  TF_ASSERT(mac_req_ptr->cfg_data.ul_cfg->sr_resource_present == FALSE);

  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_release_drb3

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration with DRB3 release.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_release_drb3
(
  lte_rrc_pdu_buf_s encoded_pdu,  /*! PDU to be decoded and sent to LLC */
  uint8 rb_direction /*! Direction of the RB being released */
)
{  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);  

  if (rb_direction & LTE_RRC_LLC_RB_UL_DIRECTION)
  {
    /* Verify PDCP UL RECFG PREP REQ */
    lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

    /* Verify MAC config request */
    lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);
  }

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_reconfig_release_drb3() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_reconfig_drb3_setup_drb4_um_bi

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration DRB3 and a AM
    bi-directional DRB4 setup.

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_drb3_setup_drb4_um_bi
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_drb3_drb4_setup_msg);
  encoded_pdu.value = llc_reconfig_drb3_drb4_setup_msg;
  
  /* Construct asn1 pdu to be sent to the Config module after decoding the encoded pdu*/
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);
  /* Set the LC id of the existing DRB to 1 */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
    rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->logicalChannelIdentity = 10;

  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = dl_dcch_msg_ptr;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_reconfig_drb3_setup_drb4_um_bi() */

/*===========================================================================

FUNCTION:  lte_rrc_llc_test_reconfig_drb3_setup_drb4_drb5_um_bi

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration DRB3 and sets up 
    UM bi-directional DRB4 & DRB5.

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
void lte_rrc_llc_test_reconfig_drb3_setup_drb4_drb5_um_bi
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  if (lte_rrc_llc_test_common_data.reconfig_after_cre == FALSE)
  {
    encoded_pdu.length = sizeof(llc_reconfig_drb3_drb4_drb5_setup_msg);
    encoded_pdu.value = llc_reconfig_drb3_drb4_drb5_setup_msg;
  }
  else
  {
    encoded_pdu.length = sizeof(llc_reconfig_drb3_pdcp_drb4_drb5_setup_msg);
    encoded_pdu.value = llc_reconfig_drb3_pdcp_drb4_drb5_setup_msg;
  }

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nConnection Reconfiguration Test Ends \n");

  lte_rrc_llc_test_free_dl_ded_msg_ptr();

} /* lte_rrc_llc_test_reconfig_drb3_setup_drb4_drb5_um_bi() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_reconfig_srb2_release_drb3_setup_drb4

===========================================================================*/
/*!
    @brief
    This API tests basic connection reconfiguration with SRB2 and a AM
    bi-directional DRB3 setup.

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_reconfig_srb2_release_drb3_setup_drb4
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_drb3_release_drb4_setup_msg);
  encoded_pdu.value = llc_reconfig_drb3_release_drb4_setup_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);  

  /* Verify PDCP UL RECFG PREP REQ */
  lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();

} /* lte_rrc_llc_test_rrc_reconfig_srb2_release_drb3_setup_drb4() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mobility_only

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information alone.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_intra_freq_mobility_only
(
  lte_rrc_pdu_buf_s encoded_pdu, /*!< Encoded reconfig message with mobility ctrl */
  boolean ho_rach_success /*!< indicates whether RACH after HO is assumed success or not*/
)
{
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Mobility Info Only Test Begins \n");
  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /* Verify PDCP UL RECFG PREP REQ */
  lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

  /* Verify phy handover request parameters when handover succeeds (TRUE) */
  lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf(TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);
  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  if (ho_rach_success) 
  {
    lte_rrc_llc_test_send_handover_completed_indi();
  }

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nConnection Reconfiguration Mobility Info Only Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_mobility_only() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_mobility_with_scc_only

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information alone.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_mobility_with_scc_only
(
  lte_rrc_pdu_buf_s encoded_pdu, /*!< Encoded reconfig message with mobility ctrl */
  boolean ho_rach_success /*!< indicates whether RACH after HO is assumed success or not*/
)
{
  int decode_status = 0;
  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;
  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;
  uint32 buf_len = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Mobility Info Only Test Begins \n");
  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /* Verify PDCP UL RECFG PREP REQ */
  lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* After CPHY Config req is sent, assert LLC is in WAIT_FOR_CPHY_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  TF_MSG("\nCPHY Config Request sent to CPHY and LLC Module in WAIT_FOR_CPHY_CFG_CNF state\n");
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Setting the fields in the LTE_CPHY_HANDOVER_CNF sent by CPHY, The bits
    set in com_mob_parm_validity field reflect the channel config that were set
    in the llc_reconfig_msg in the lte_rrc_llc_ota_msg.h */
  cphy_cfg_cnf_msg.com_mob_parm_validity = cphy_cfg_req_ptr->common_mobility_cfg_param.cfg_mask;
  cphy_cfg_cnf_msg.dedicated_parm_validity = cphy_cfg_req_ptr->dedicated_cfg_param.cfg_mask;  
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));
  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);
  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  if (ho_rach_success) 
  {
    lte_rrc_llc_test_send_handover_completed_indi();
  }

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nConnection Reconfiguration Mobility Info Only Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_mobility_with_scc_only() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_full_config_with_drb3_release

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information along with radio resource configuration including DRB 
    addition and release.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_full_config_with_drb3_release
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Mobility with full configuration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg_tdd);
  encoded_pdu.value = llc_reconfig_ho_msg_tdd;

  	TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
	lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;

  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify PDCP UL RECFG PREP REQ */
  lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

  /* Verify phy handover request parameters when handover succeeds (TRUE) */
  lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf(TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);
  
  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_send_handover_completed_indi();

  TF_MSG("\nConnection Reconfiguration Mobility with full configuration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
} /* lte_rrc_llc_test_ho_full_config_with_drb3_release() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_delta_config_with_drb3_release

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information with delta configuration.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_delta_config_with_drb3_release
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Mobility with delta configuration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_ho_delta_config_msg);
  encoded_pdu.value = llc_reconfig_ho_delta_config_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify PDCP UL RECFG PREP REQ */
  lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

  /* Verify phy handover request parameters when handover succeeds (TRUE) */
  lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf(TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Mobility with delta configuration Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_delta_config_with_drb3_release() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_fdd_to_tdd

===========================================================================*/
/*!
    @brief
    This API tests handover from an FDD cell to a TDD cell.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_fdd_to_tdd
(
  lte_rrc_pdu_buf_s encoded_pdu, /*!< Encoded reconfig message with mobility ctrl */
  boolean rel_ver_is_r11 
)
{
  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;
  lte_mac_access_abort_req_msg_s *mac_access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s mac_access_abort_cnf_msg;
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;
  lte_rrc_handover_completed_ind_s ho_completed_ind;
  lte_rrc_osys_MobilityControlInfo *mobility_info_ptr = NULL;
  lte_rrc_osys_RadioResourceConfigCommon *radio_resrc_common_cfg_ptr = NULL;
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_ded_cfg_ptr = NULL;
  uint32 buf_len = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nFDD to TDD HO Test Begins \n");
  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  mobility_info_ptr = &lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.
    rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    mobilityControlInfo;
  radio_resrc_common_cfg_ptr = &mobility_info_ptr->radioResourceConfigCommon;
  radio_resrc_ded_cfg_ptr = &lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.
    rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    radioResourceConfigDedicated;

  mobility_info_ptr->targetPhysCellId = 502;
  /* Set the DL/UL freq to the TDD band */
  mobility_info_ptr->carrierFreq.dl_CarrierFreq = 36000;
  mobility_info_ptr->carrierFreq.m.ul_CarrierFreqPresent = 0;
  /* Include TDD Config */
  radio_resrc_common_cfg_ptr->m.tdd_ConfigPresent = 1;
  radio_resrc_common_cfg_ptr->tdd_Config.subframeAssignment 
    = lte_rrc_osys_TDD_Config_subframeAssignment_sa3;
  radio_resrc_common_cfg_ptr->tdd_Config.specialSubframePatterns 
    = lte_rrc_osys_TDD_Config_specialSubframePatterns_ssp6;
  if (rel_ver_is_r11 == TRUE) 
  {
     radio_resrc_common_cfg_ptr->tdd_Config.specialSubframePatterns 
       = lte_rrc_osys_TDD_Config_specialSubframePatterns_ssp5;

     radio_resrc_common_cfg_ptr->m.tdd_Config_v1130Present = 1;
     radio_resrc_common_cfg_ptr->tdd_Config_v1130.specialSubframePatterns_v1130 = 
        lte_rrc_osys_TDD_Config_v1130_specialSubframePatterns_v1130_ssp9;
  }
  /* Include srx_max_up_pts */
  radio_resrc_common_cfg_ptr->soundingRS_UL_ConfigCommon.u.setup->m.srs_MaxUpPtsPresent = 1;
  radio_resrc_common_cfg_ptr->soundingRS_UL_ConfigCommon.u.setup->srs_MaxUpPts 
    = lte_rrc_osys_SoundingRS_UL_ConfigCommon_setup_srs_MaxUpPts_true;

  /* Include two_intervals_cfg */
  radio_resrc_ded_cfg_ptr->sps_Config.sps_ConfigUL.u.setup->m.twoIntervalsConfigPresent = 1;
  radio_resrc_ded_cfg_ptr->sps_Config.sps_ConfigUL.u.setup->twoIntervalsConfig 
    = lte_rrc_osys_SPS_ConfigUL_setup_twoIntervalsConfig_true;

  /* Mask off the CDRX config */
  radio_resrc_ded_cfg_ptr->mac_MainConfig.u.explicitValue->m.drx_ConfigPresent = 0;
  radio_resrc_ded_cfg_ptr->mac_MainConfig.u.explicitValue->ul_SCH_Config.ttiBundling = 0;
  radio_resrc_ded_cfg_ptr->physicalConfigDedicated.antennaInfo.u.explicitValue->transmissionMode = 
    lte_rrc_osys_AntennaInfoDedicated_transmissionMode_tm1;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 1);
  /* After PDCP Config prep req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCP Config Prep Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));
  /*--------------------------------------------------------------------------
                          MAC ACCESS ABORT Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the ACCESS_ABORT_REQ to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_abort_req_ptr,&buf_len);
  /* Make sure the ACCESS_ABORT_REQ is valid */
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);

  /* Send ACCESS_ABORT_CNF to Config */
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&mac_access_abort_cnf_msg.hdr,LTE_MAC_ACCESS_ABORT_CNF);
  mac_access_abort_cnf_msg.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&mac_access_abort_cnf_msg, sizeof(mac_access_abort_cnf_msg));
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Verify phy handover request parameters when handover succeeds (TRUE) */
  lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf(TRUE);
  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 2);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 1);

  /* After RLCDL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);
  TF_MSG("\nRLCDL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 2);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 1);
  
  /* After RLCUL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);
  TF_MSG("\nRLCUL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 1);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 1);

  /* After PDCPDL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCPDL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 1);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 1);

  /* After PDCPUL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCPUL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));
  
  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Construct and send HO_COMPLETED_IND to LLC */
  lte_rrc_init_default_hdr(&ho_completed_ind.msg_hdr,LTE_RRC_HANDOVER_COMPLETED_IND);
  lte_rrc_utf_send_msg((byte*)&ho_completed_ind, sizeof(ho_completed_ind));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nFDD to TDD HO Test Begins \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_fdd_to_tdd() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_fdd_to_tdd_without_ack_nak_fb_mode

===========================================================================*/
/*!
    @brief
    This API tests handover from an FDD cell to a TDD cell.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_fdd_to_tdd_without_ack_nak_fb_mode
(
  lte_rrc_pdu_buf_s encoded_pdu /*!< Encoded reconfig message with mobility ctrl */
)
{
  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;
  lte_mac_access_abort_req_msg_s *mac_access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s mac_access_abort_cnf_msg;
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;
  lte_rrc_handover_completed_ind_s ho_completed_ind;
  lte_rrc_osys_MobilityControlInfo *mobility_info_ptr = NULL;
  lte_rrc_osys_RadioResourceConfigCommon *radio_resrc_common_cfg_ptr = NULL;
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_ded_cfg_ptr = NULL;
  uint32 buf_len = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nFDD to TDD HO Test Begins \n");
  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  mobility_info_ptr = &lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.
    rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    mobilityControlInfo;
  radio_resrc_common_cfg_ptr = &mobility_info_ptr->radioResourceConfigCommon;
  radio_resrc_ded_cfg_ptr = &lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.
    rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    radioResourceConfigDedicated;

  mobility_info_ptr->targetPhysCellId = 502;
  /* Set the DL/UL freq to the TDD band */
  mobility_info_ptr->carrierFreq.dl_CarrierFreq = 36000;
  mobility_info_ptr->carrierFreq.m.ul_CarrierFreqPresent = 0;
  /* Include TDD Config */
  radio_resrc_common_cfg_ptr->m.tdd_ConfigPresent = 1;
  radio_resrc_common_cfg_ptr->tdd_Config.subframeAssignment 
    = lte_rrc_osys_TDD_Config_subframeAssignment_sa3;
  radio_resrc_common_cfg_ptr->tdd_Config.specialSubframePatterns 
    = lte_rrc_osys_TDD_Config_specialSubframePatterns_ssp6;

  /* Include srx_max_up_pts */
  radio_resrc_common_cfg_ptr->soundingRS_UL_ConfigCommon.u.setup->m.srs_MaxUpPtsPresent = 1;
  radio_resrc_common_cfg_ptr->soundingRS_UL_ConfigCommon.u.setup->srs_MaxUpPts 
    = lte_rrc_osys_SoundingRS_UL_ConfigCommon_setup_srs_MaxUpPts_true;

  /* Include two_intervals_cfg */
  radio_resrc_ded_cfg_ptr->sps_Config.sps_ConfigUL.u.setup->m.twoIntervalsConfigPresent = 1;
  radio_resrc_ded_cfg_ptr->sps_Config.sps_ConfigUL.u.setup->twoIntervalsConfig 
    = lte_rrc_osys_SPS_ConfigUL_setup_twoIntervalsConfig_true;

  /* Mask off the CDRX config */
  radio_resrc_ded_cfg_ptr->mac_MainConfig.u.explicitValue->m.drx_ConfigPresent = 0;
  radio_resrc_ded_cfg_ptr->mac_MainConfig.u.explicitValue->ul_SCH_Config.ttiBundling = 0;
  radio_resrc_ded_cfg_ptr->physicalConfigDedicated.antennaInfo.u.explicitValue->transmissionMode = 
    lte_rrc_osys_AntennaInfoDedicated_transmissionMode_tm1;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nFDD to TDD HO Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_fdd_to_tdd_without_ack_nak_fb_mode() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_tdd_to_tdd

===========================================================================*/
/*!
    @brief
    This API tests handover from a TDD cell to a TDD cell.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_tdd_to_tdd
(
  lte_rrc_pdu_buf_s encoded_pdu /*!< Encoded reconfig message with mobility ctrl */
)
{
  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;
  lte_mac_access_abort_req_msg_s *mac_access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s mac_access_abort_cnf_msg;
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;
  lte_rrc_handover_completed_ind_s ho_completed_ind;
  uint32 buf_len = 0;
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_ded_cfg_ptr = NULL;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nTDD to TDD HO Test Begins \n");

  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.
    u.c1->u.rrcConnectionReconfiguration_r8->mobilityControlInfo.targetPhysCellId = 502;
    
  radio_resrc_ded_cfg_ptr = &lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.
    rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    radioResourceConfigDedicated;

  radio_resrc_ded_cfg_ptr->m.drb_ToReleaseListPresent = 0;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);
  /* After PDCP Config prep req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCP Config Prep Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));
  /*--------------------------------------------------------------------------
                          MAC ACCESS ABORT Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the ACCESS_ABORT_REQ to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_abort_req_ptr,&buf_len);
  /* Make sure the ACCESS_ABORT_REQ is valid */
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);

  /* Send ACCESS_ABORT_CNF to Config */
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&mac_access_abort_cnf_msg.hdr,LTE_MAC_ACCESS_ABORT_CNF);
  mac_access_abort_cnf_msg.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&mac_access_abort_cnf_msg, sizeof(mac_access_abort_cnf_msg));
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Verify phy handover request parameters when handover succeeds (TRUE) */
  lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf(TRUE);
  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 0);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 0);

  /* After RLCDL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);
  TF_MSG("\nRLCDL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 0);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 0);
  
  /* After RLCUL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);
  TF_MSG("\nRLCUL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);

  /* After PDCPDL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCPDL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);

  /* After PDCPUL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCPUL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));
  
  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Construct and send HO_COMPLETED_IND to LLC */
  lte_rrc_init_default_hdr(&ho_completed_ind.msg_hdr,LTE_RRC_HANDOVER_COMPLETED_IND);
  lte_rrc_utf_send_msg((byte*)&ho_completed_ind, sizeof(ho_completed_ind));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nTDD to TDD HO Test Begins \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_tdd_to_tdd() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_tdd_to_fdd

===========================================================================*/
/*!
    @brief
    This API tests handover from a TDD cell to an FDD cell.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_tdd_to_fdd
(
  lte_rrc_pdu_buf_s encoded_pdu /*!< Encoded reconfig message with mobility ctrl */
)
{
  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;
  lte_mac_access_abort_req_msg_s *mac_access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s mac_access_abort_cnf_msg;
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;
  lte_rrc_osys_MobilityControlInfo *mobility_info_ptr = NULL;
  lte_rrc_osys_RadioResourceConfigDedicated *radio_resrc_ded_cfg_ptr = NULL;
  uint32 buf_len = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nTDD to FDD HO Test Begins \n");

  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  mobility_info_ptr = &lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.
    rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    mobilityControlInfo;

  /* Include DL/UL Carrier Freq, to imply an FDD cell */
  mobility_info_ptr->m.carrierFreqPresent = 1;
  mobility_info_ptr->carrierFreq.dl_CarrierFreq = 500;
  mobility_info_ptr->carrierFreq.m.ul_CarrierFreqPresent = 1;
  mobility_info_ptr->carrierFreq.ul_CarrierFreq = 18500;

  /* Reset the bitmask for DRB to release list */
  radio_resrc_ded_cfg_ptr = &lte_rrc_llc_test_common_data.dl_dcch_msg_ptr->message.u.c1->u.
    rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    radioResourceConfigDedicated;
  radio_resrc_ded_cfg_ptr->m.drb_ToReleaseListPresent = 0;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);
  /* After PDCP Config prep req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCP Config Prep Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));
  /*--------------------------------------------------------------------------
                          MAC ACCESS ABORT Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the ACCESS_ABORT_REQ to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_abort_req_ptr,&buf_len);
  /* Make sure the ACCESS_ABORT_REQ is valid */
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);

  /* Send ACCESS_ABORT_CNF to Config */
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&mac_access_abort_cnf_msg.hdr,LTE_MAC_ACCESS_ABORT_CNF);
  mac_access_abort_cnf_msg.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&mac_access_abort_cnf_msg, sizeof(mac_access_abort_cnf_msg));
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Verify phy handover request parameters when handover succeeds (TRUE) */
  lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf(TRUE);
  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 0);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 0);

  /* After RLCDL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);
  TF_MSG("\nRLCDL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 0);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 0);
  
  /* After RLCUL Config req is sent, assert LLC is in WAIT_FOR_RLC_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_RLC_CFG_CNF);
  TF_MSG("\nRLCUL Config Request sent to RLC and LLC Module in WAIT_FOR_RLC_CFG_CNF state\n");
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);

  /* After PDCPDL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCPDL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);

  /* After PDCPUL Config req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCPUL Config Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));
  
  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nTDD to FDD HO Test Begins \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_tdd_to_fdd() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rel9_ho_full_config

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information including full configuration option in Rel 9.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rel9_ho_full_config
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nRel 9 HO with full configuration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_rel9_ho_full_config_msg_tdd);
  encoded_pdu.value = llc_reconfig_rel9_ho_full_config_msg_tdd;
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;

  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  lte_rrc_llc_test_common_data.full_config = TRUE;

  /* Verify PDCP UL RECFG PREP REQ */
  lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

  /* Verify phy handover request parameters when handover succeeds (TRUE) */
  lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf(TRUE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_send_handover_completed_indi();

  lte_rrc_llc_test_common_data.full_config = FALSE;

  TF_MSG("\nRel 9 HO with full configuration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
} /* lte_rrc_llc_test_rel9_ho_full_config() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rel9_reconfig_after_cre_full_config

===========================================================================*/
/*!
    @brief
    This API tests the first RRC connection reconfiguration after CRE, 
    with full configuration option in Rel 9.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rel9_reconfig_after_cre_full_config
(
  boolean pdcp_drb_release
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  /* ---------------------------------------------------------------------- */
  TF_MSG("\nRel 9 Reconfig after CRE with full configuration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_after_cre_rel9_full_config_msg);
  encoded_pdu.value = llc_reconfig_after_cre_rel9_full_config_msg;

  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;

  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  lte_rrc_llc_test_common_data.full_config = TRUE;

  if (pdcp_drb_release)
  {
    /* Verify PDCP UL RECFG PREP REQ */
    lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf(LTE_RRC_PROC_CONFIG);
  }

  /* Verify phy handover request parameters when handover succeeds (TRUE) */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  TF_ASSERT (lte_rrc_llc_data.dd_ptr->cmd_db.full_config_in_progress == FALSE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_llc_test_send_handover_completed_indi();

  lte_rrc_llc_test_common_data.full_config = FALSE;

  TF_MSG("\nRel 9 Reconfig after CRE with full configuration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
} /* lte_rrc_llc_test_rel9_reconfig_after_cre_full_config() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rlf

===========================================================================*/
/*!
    @brief
    This API tests the scenario of non-HO related RLF which would cause CRE to 
    be triggered. CRE would send CFG_REQI to suspend RBs. This API also tests 
    this processing in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rlf
(
  void
)
{
  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;

  lte_mac_access_abort_req_msg_s *mac_access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s mac_access_abort_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req;
  lte_rlc_cfg_cnf_s rlcdl_cfg_cnf;

  lte_rlcul_cfg_req_s *rlcul_cfg_req;
  lte_rlc_cfg_cnf_s rlcul_cfg_cnf;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf;
  
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf;

  uint32 buf_len = 0;
  /* ---------------------------------------------------------------------- */

  TF_MSG("\n RLF Test Begins \n");

  /* Assert LLC is in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_CPHY);
  // ---------------------------------------------

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  // ---------------------------------------------
  // Catch the ACCESS_ABORT_REQ to MAC 
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_abort_req_ptr,&buf_len);
  // Make sure the ACCESS_ABORT_REQ is valid 
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);

  // Send ACCESS_ABORT_CNF to Config 
  // Setting the header for internal RRC messages 
  lte_rrc_init_default_hdr(&mac_access_abort_cnf_msg.hdr,LTE_MAC_ACCESS_ABORT_CNF);
  mac_access_abort_cnf_msg.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&mac_access_abort_cnf_msg, sizeof(mac_access_abort_cnf_msg));


  // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req != NULL);
  TF_ASSERT(mac_cfg_req->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req->cfg_data.cfg_bitmask == 0);
  TF_ASSERT(mac_cfg_req->cfg_data.lc_info_cfg.num_addmod_lc == 0);
  TF_ASSERT(mac_cfg_req->cfg_data.lc_info_cfg.num_deleted_lc == 0);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf.hdr, sizeof(mac_cfg_cnf));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req != NULL);
  TF_ASSERT(rlcdl_cfg_req->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req->num_modified_rb == 0);
  TF_ASSERT(rlcdl_cfg_req->num_released_rb == 0);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlcdl_cfg_cnf.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlcdl_cfg_cnf.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlcdl_cfg_cnf.msg_hdr, sizeof(rlcdl_cfg_cnf));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req != NULL);
  TF_ASSERT(rlcul_cfg_req->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req->num_modified_rb == 0);
  TF_ASSERT(rlcul_cfg_req->num_released_rb == 0);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlcul_cfg_cnf.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlcul_cfg_cnf.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlcul_cfg_cnf.msg_hdr, sizeof(rlcul_cfg_cnf));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req != NULL);
  TF_ASSERT(pdcpdl_cfg_req->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req->num_add_modified_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req->num_released_rb == 0);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf.hdr, sizeof(pdcpdl_cfg_cnf));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req != NULL);
  TF_ASSERT(pdcpul_cfg_req->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req->num_add_modified_rb == 0);
  TF_ASSERT(pdcpul_cfg_req->num_released_rb == 0);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf.hdr, sizeof(pdcpul_cfg_cnf));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  TF_MSG("\n RLF Test Ends \n");

} /* lte_rrc_llc_test_rlf() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_cphy_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information and CPHY_HANDOVER_CNF is returned with failure. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_cphy_failure
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;

  lte_mac_access_abort_req_msg_s *mac_access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s mac_access_abort_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req;
  lte_rlc_cfg_cnf_s rlcdl_cfg_cnf;

  lte_rlcul_cfg_req_s *rlcul_cfg_req;
  lte_rlc_cfg_cnf_s rlcul_cfg_cnf;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf;
  
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf;

  uint32 buf_len = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Mobility - CPHY Failure Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_ho_delta_config_msg);
  encoded_pdu.value = llc_reconfig_ho_delta_config_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL)
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->released_rb_cfg_idx[0] == 3);
  /* After PDCP Config prep req is sent, assert LLC is in WAIT_FOR_PDCP_CFG_CNF state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_PDCP_CFG_CNF);
  TF_MSG("\nPDCP Config Prep Request sent to PDCP and LLC Module in WAIT_FOR_PDCP_CFG_CNF state\n");
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

  /* Verify phy handover request parameters when handover fails (FALSE) */
  lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf(FALSE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_FAILURE, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_CPHY);
  // ---------------------------------------------

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  // ---------------------------------------------
  // Catch the ACCESS_ABORT_REQ to MAC 
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_abort_req_ptr,&buf_len);
  // Make sure the ACCESS_ABORT_REQ is valid 
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);

  // Send ACCESS_ABORT_CNF to Config 
  // Setting the header for internal RRC messages 
  lte_rrc_init_default_hdr(&mac_access_abort_cnf_msg.hdr,LTE_MAC_ACCESS_ABORT_CNF);
  mac_access_abort_cnf_msg.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&mac_access_abort_cnf_msg, sizeof(mac_access_abort_cnf_msg));


 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req != NULL);
  TF_ASSERT(mac_cfg_req->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req->cfg_data.cfg_bitmask == 0);
  TF_ASSERT(mac_cfg_req->cfg_data.lc_info_cfg.num_addmod_lc == 0);
  TF_ASSERT(mac_cfg_req->cfg_data.lc_info_cfg.num_deleted_lc == 0);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf.hdr, sizeof(mac_cfg_cnf));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req != NULL);
  TF_ASSERT(rlcdl_cfg_req->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req->num_modified_rb == 0);
  TF_ASSERT(rlcdl_cfg_req->num_released_rb == 0);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlcdl_cfg_cnf.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlcdl_cfg_cnf.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlcdl_cfg_cnf.msg_hdr, sizeof(rlcdl_cfg_cnf));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req != NULL);
  TF_ASSERT(rlcul_cfg_req->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req->num_modified_rb == 0);
  TF_ASSERT(rlcul_cfg_req->num_released_rb == 0);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlcul_cfg_cnf.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlcul_cfg_cnf.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlcul_cfg_cnf.msg_hdr, sizeof(rlcul_cfg_cnf));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req != NULL);
  TF_ASSERT(pdcpdl_cfg_req->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req->num_add_modified_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req->num_released_rb == 0);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf.hdr, sizeof(pdcpdl_cfg_cnf));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req != NULL);
  TF_ASSERT(pdcpul_cfg_req->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req->num_add_modified_rb == 0);
  TF_ASSERT(pdcpul_cfg_req->num_released_rb == 0);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf.hdr, sizeof(pdcpul_cfg_cnf));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  TF_MSG("\nConnection Reconfiguration Mobility - CPHY Failure Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
} /* lte_rrc_llc_test_ho_cphy_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_with_no_drb_rach_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information with no DRB information that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_with_no_drb_rach_failure
(
  void
)
{
  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;

  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;

  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;

  uint32 buf_len = 0;
  /* ---------------------------------------------------------------------- */

  TF_MSG("\nReconfiguration containing Mobility with no DRB info RACH Failure Test Begins \n");

  /* Assert LLC is in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 0);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr != NULL);
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_MODIFY);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr != NULL);
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_MODIFY);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  TF_MSG("\nReconfiguration containing Mobility with no DRB info RACH Failure Test Ends \n");

} /* lte_rrc_llc_test_ho_with_no_drb_rach_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_with_drb_rel_rach_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information and DRB release that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_with_drb_rel_rach_failure
(
  void
)
{

  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_cphy_handover_req_s *cphy_cfg_req = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_prep_req;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_prep_cnf;

  lte_mac_cfg_req_msg_s *mac_cfg_req;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req = NULL;
  lte_rlcul_cfg_req_s *rlcul_cfg_req = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf;
  
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf;

  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;

  uint32 buf_len = 0;

  TF_MSG("\nReconfiguration containing Mobility with DRB Rel RACH Failure Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_delta_config_msg);
  encoded_pdu.value = llc_reconfig_ho_delta_config_msg;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);  

  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_prep_req,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_prep_cnf.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_prep_cnf.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_prep_cnf, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf, sizeof(cphy_cfg_cnf));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);
  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  /* Construct and send the lte_mac_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  /*--------------------------------------------------------------------------
                              RLC DL/UL Request Handling Test
  ---------------------------------------------------------------------------*/
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf, sizeof(pdcpdl_cfg_cnf));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf, sizeof(pdcpul_cfg_cnf));

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* To start with, LLC should be in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_prep_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_prep_req != NULL);
  TF_ASSERT(pdcpul_prep_req->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_prep_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_prep_req->num_released_rb == 0);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_prep_cnf.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_prep_cnf.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_prep_cnf.hdr, sizeof(pdcpul_prep_cnf));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req != NULL);
  TF_ASSERT(mac_cfg_req->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req->cfg_data.lc_info_cfg.num_deleted_lc == 0);
  TF_ASSERT(mac_cfg_req->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf.hdr, sizeof(mac_cfg_cnf));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req != NULL);
  TF_ASSERT(rlcdl_cfg_req->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req->num_released_rb == 0);
  TF_ASSERT(rlcdl_cfg_req->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf.msg_hdr, sizeof(rlc_cfg_cnf));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req != NULL);
  TF_ASSERT(rlcul_cfg_req->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req->num_released_rb == 0);
  TF_ASSERT(rlcul_cfg_req->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf.msg_hdr, sizeof(rlc_cfg_cnf));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req != NULL);
  TF_ASSERT(pdcpdl_cfg_req->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req->num_add_modified_rb == 3);
  TF_ASSERT(pdcpdl_cfg_req->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_ADD);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf.hdr, sizeof(pdcpdl_cfg_cnf));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req != NULL);
  TF_ASSERT(pdcpul_cfg_req->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req->num_add_modified_rb == 3);
  TF_ASSERT(pdcpul_cfg_req->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_ADD);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf.hdr, sizeof(pdcpul_cfg_cnf));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  TF_MSG("\nReconfiguration containing Mobility with DRB Rel RACH Failure Test Ends \n");

} /* lte_rrc_llc_test_ho_with_drb_rel_rach_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_with_drb_add_rach_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information and DRB addition that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_with_drb_add_rach_failure
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;

  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;

  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;

  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;

  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;

  uint32 buf_len = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nReconfiguration containing Mobility with DRB Add RACH Failure Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */   
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg);
  encoded_pdu.value = llc_reconfig_ho_msg;
  
  /* Construct asn1 pdu to be sent to the Config module after decoding the encoded pdu*/
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* Mask off DRB To Release list */
  dl_dcch_msg_ptr->message.u.c1->u.
    rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->
    radioResourceConfigDedicated.m.drb_ToReleaseListPresent = 0;
  (((*(dl_dcch_msg_ptr->message.u.c1->u.      
  	rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
  	rrcConnectionReconfiguration_r8)).mobilityControlInfo).carrierFreq).dl_CarrierFreq = 40320;
  ((*((((dl_dcch_msg_ptr->message.u.c1->u.      
  	rrcConnectionReconfiguration->criticalExtensions.u.c1->u.rrcConnectionReconfiguration_r8->      
  	radioResourceConfigDedicated).mac_MainConfig).u).explicitValue)).ul_SCH_Config).ttiBundling = 0;
  	TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
	lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = dl_dcch_msg_ptr;

  /* Set the version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);

  /* The freq and cell id should be same as what was sent in LTE_CPHY_HANDOVER_REQ */
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            freq == cphy_cfg_req_ptr->ho_target_cell_info.dl_cell_frequency);
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            phy_cell_id == cphy_cfg_req_ptr->ho_target_cell_info.target_cell_id);

  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req_ptr,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  /* Construct and send the lte_mac_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_false;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = FALSE;

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->released_rb_cfg_idx[0] == 1);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 3);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr != NULL);
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[0] == 1);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr != NULL);
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 1);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->released_rb_cfg_idx[0] == 1);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->released_rb_cfg_idx[0] == 1);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  TF_MSG("\nReconfiguration containing Mobility with DRB Add RACH Failure Test Ends \n");
  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_with_drb_add_rach_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_with_drb_modify_rach_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information and DRB modification that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_with_drb_modify_rach_failure
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;

  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;

  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;

  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;

  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;

  uint32 buf_len = 0;
  int decode_status = 0;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nReconfiguration containing Mobility with DRB Modify RACH Failure Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */   
  encoded_pdu.length = sizeof(llc_reconfig_ho_msg_tdd);
  encoded_pdu.value = llc_reconfig_ho_msg_tdd;
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
	lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;
  /* Construct asn1 pdu to be sent to the Config module after decoding the 
     encoded pdu*/
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* Mask off DRBToRelease List */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.
    c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.m.drb_ToReleaseListPresent = 0;

  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.
    c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->m.eps_BearerIdentityPresent = 0;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.
    c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->m.rlc_ConfigPresent = 1;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.
    c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->m.pdcp_ConfigPresent = 0;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.
    c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->m.logicalChannelIdentityPresent = 0;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.
    c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->m.logicalChannelConfigPresent = 0;
  
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.
    c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->drb_Identity = 3;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.
    c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->rlc_Config.t = T_lte_rrc_osys_RLC_Config_um_Bi_Directional;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.
    c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->rlc_Config.u.um_Bi_Directional->ul_UM_RLC.
    sn_FieldLength = lte_rrc_osys_SN_FieldLength_size10;
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.
    c1->u.rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->rlc_Config.u.um_Bi_Directional->dl_UM_RLC.
    sn_FieldLength = lte_rrc_osys_SN_FieldLength_size10;

  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = dl_dcch_msg_ptr;

  /* Set the release version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);
  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);
  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req_ptr,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  /* Construct and send the lte_mac_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC DL/UL Request Handling Test
  ---------------------------------------------------------------------------*/
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_false;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = FALSE;

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 0);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr != NULL);
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_MODIFY);

  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].lc_id == 10);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_id == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_type == LTE_RB_TYPE_DRB);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].cfg.um_cfg.sn_length == 5);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].cfg.um_cfg.t_reordering == 10);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr != NULL);
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_MODIFY);

  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].lc_id == 10);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_id == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_type == LTE_RB_TYPE_DRB);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].pdcp_discard_timer == 50);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].pdcp_status_required == FALSE);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].cfg.um_cfg.sn_length == 5);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  TF_MSG("\nReconfiguration containing Mobility with DRB Modify RACH Failure Test Ends \n");

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_with_drb_modify_rach_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_with_drb_add_modify_rel_rach_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information and DRB add, modify and release that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_with_drb_add_modify_rel_rach_failure
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;

  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;

  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;

  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;

  uint32 buf_len = 0;
  /* ---------------------------------------------------------------------- */

  TF_MSG("\nReconfiguration containing Mobility with DRB add, modify and release "
         "RACH Failure Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_reconfig_ho_with_drb_add_mod_rel_msg);
  encoded_pdu.value = llc_reconfig_ho_with_drb_add_mod_rel_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);  
  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);
  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req_ptr,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  /* Construct and send the lte_mac_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC DL/UL Request Handling Test
  ---------------------------------------------------------------------------*/
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->released_rb_cfg_idx[0] == 1);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 5);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 8);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[3] == 6);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[4] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 3);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);
  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 5);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 4);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[3].rb_cfg_idx == 5);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[3].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[4].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[4].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[0] == 1);

  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[4].lc_id == 8);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[4].rb_id == 4);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[4].rb_cfg_idx == 4);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[4].rb_mode == LTE_RB_RLC_MODE_UM);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[4].rb_type == LTE_RB_TYPE_DRB);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[4].cfg.um_cfg.sn_length == 10);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[4].cfg.um_cfg.t_reordering == 20);

  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].lc_id == 10);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_id == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_type == LTE_RB_TYPE_DRB);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].cfg.um_cfg.sn_length == 5);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].cfg.um_cfg.t_reordering == 10);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 5);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 4);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[3].rb_cfg_idx == 5);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[3].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[4].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[4].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 1);

  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].lc_id == 10);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_id == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_type == LTE_RB_TYPE_DRB);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].pdcp_discard_timer == 50);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].pdcp_status_required == FALSE);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].cfg.um_cfg.sn_length == 5);

  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[4].lc_id == 8);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[4].rb_id == 4);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[4].rb_cfg_idx == 4);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[4].rb_mode == LTE_RB_RLC_MODE_UM);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[4].rb_type == LTE_RB_TYPE_DRB);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[4].cfg.um_cfg.sn_length == 10);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 5);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 4);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[3].rb_cfg_idx == 5);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[3].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[4].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[4].cfg_act == LTE_PDCP_CFG_ACT_ADD);
  TF_ASSERT(pdcpdl_cfg_req_ptr->released_rb_cfg_idx[0] == 1);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 5);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 4);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[3].rb_cfg_idx == 5);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[3].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[4].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[4].cfg_act == LTE_PDCP_CFG_ACT_ADD);
  TF_ASSERT(pdcpul_cfg_req_ptr->released_rb_cfg_idx[0] == 1);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  TF_MSG("\nReconfiguration containing Mobility with DRB add, modify and "
         "release RACH Failure Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_with_drb_add_modify_rel_rach_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rel9_ho_with_full_config_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility with
    rel 9 full config option that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rel9_ho_with_full_config_failure
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;

  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;

  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;

  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;

  uint32 buf_len = 0;

  /* ---------------------------------------------------------------------- */

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */   
  encoded_pdu.length = sizeof(llc_reconfig_rel9_ho_full_config_msg_tdd);
  encoded_pdu.value = llc_reconfig_rel9_ho_full_config_msg_tdd;
  
  /* Set the version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);

  /* The freq and cell id should be same as what was sent in LTE_CPHY_HANDOVER_REQ */
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            freq == cphy_cfg_req_ptr->ho_target_cell_info.dl_cell_frequency);
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            phy_cell_id == cphy_cfg_req_ptr->ho_target_cell_info.target_cell_id);

  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req_ptr,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  /* Construct and send the lte_mac_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_false;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = FALSE;

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 2);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->released_rb_cfg_idx[0] == 1);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->released_rb_cfg_idx[1] == 2);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 5);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[3] == 8);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[4] == 6);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[1] == 4);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[2] == 10);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr != NULL);
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 5);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 2);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[3].rb_cfg_idx == 4);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[3].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[4].rb_cfg_idx == 5);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[4].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[0] == 1);
  TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[1] == 3);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr != NULL);
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 5);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[3].rb_cfg_idx == 4);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[3].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[4].rb_cfg_idx == 5);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[4].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[1] == 2);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[2] == 3);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 5);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[3].rb_cfg_idx == 4);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[3].cfg_act == LTE_PDCP_CFG_ACT_ADD);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[4].rb_cfg_idx == 5);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[4].cfg_act == LTE_PDCP_CFG_ACT_ADD);
  TF_ASSERT(pdcpdl_cfg_req_ptr->released_rb_cfg_idx[0] == 1);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 5);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 2);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[3].rb_cfg_idx == 4);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[3].cfg_act == LTE_PDCP_CFG_ACT_ADD);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[4].rb_cfg_idx == 5);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[4].cfg_act == LTE_PDCP_CFG_ACT_ADD);
  TF_ASSERT(pdcpul_cfg_req_ptr->released_rb_cfg_idx[0] == 1);
  TF_ASSERT(pdcpul_cfg_req_ptr->released_rb_cfg_idx[1] == 2);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_rel9_ho_with_full_config_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rohc_reset_config_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility with
    rel 9 full config option that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rohc_reset_config_failure
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;

  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;

  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;

  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;

  uint32 buf_len = 0;

  /* ---------------------------------------------------------------------- */

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */   
  encoded_pdu.length = sizeof(llc_reconfig_rel9_ho_full_config_msg_tdd);
  encoded_pdu.value = llc_reconfig_rel9_ho_full_config_msg_tdd;
  
  /* Set the version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_true;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = TRUE;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);
  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);

  /* The freq and cell id should be same as what was sent in LTE_CPHY_HANDOVER_REQ */
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            freq == cphy_cfg_req_ptr->ho_target_cell_info.dl_cell_frequency);
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            phy_cell_id == cphy_cfg_req_ptr->ho_target_cell_info.target_cell_id);

  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req_ptr,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  /* Construct and send the lte_mac_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------
  TF_STUB(lte_rrc_cell_is_tdd) = lte_rrc_llc_test_cfg_is_for_tdd_cell_false;
  lte_rrc_llc_test_common_data.tdd_param.tdd_cfg = FALSE;

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 2);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->released_rb_cfg_idx[0] == 1);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->released_rb_cfg_idx[1] == 2);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  
  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr != NULL);
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  
  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr != NULL);
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  
  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  
  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  
  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_rohc_reset_config_failure() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rel9_ho_with_mode_change_full_config_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility with
    rel 9 full config option that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rel9_ho_with_mode_change_full_config_failure
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_cphy_drx_opt_req_s *drx_opt_req_ptr = NULL;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;

  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;

  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;

  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;

  uint32 buf_len = 0;

  /* ---------------------------------------------------------------------- */

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */   
  encoded_pdu.length = sizeof(llc_rel9_ho_full_config_mode_change_msg);
  encoded_pdu.value = llc_rel9_ho_full_config_mode_change_msg;
  
  /* Set the version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);

  /* The freq and cell id should be same as what was sent in LTE_CPHY_HANDOVER_REQ */
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            freq == cphy_cfg_req_ptr->ho_target_cell_info.dl_cell_frequency);
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            phy_cell_id == cphy_cfg_req_ptr->ho_target_cell_info.target_cell_id);

  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req_ptr,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_HO);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_C_RNTI_CFG_BM |
                                                      LTE_MAC_LC_INFO_CFG_BM |
                                                      LTE_MAC_UL_CFG_BM |
                                                      LTE_MAC_RACH_CFG_BM));
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].priority == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].pbr == 32);

  /* Construct and send the lte_mac_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);
  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[0] == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_AM);

  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_AM);

  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 1);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 1);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_utf_get_next_ext_msg((byte **) &drx_opt_req_ptr,&buf_len);
  TF_ASSERT(drx_opt_req_ptr->msgr_hdr.id == LTE_CPHY_DRX_OPT_REQ);
  TF_ASSERT(drx_opt_req_ptr->um_mode_enabled == FALSE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].priority == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].pbr == 16);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr != NULL);
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[0] == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr != NULL);
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_rel9_ho_with_mode_change_full_config_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_with_mode_change_from_am_to_ul_only_full_config_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility with
    rel 9 full config option that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_with_mode_change_from_am_to_ul_only_full_config_failure
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_cphy_drx_opt_req_s *drx_opt_req_ptr = NULL;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;

  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;

  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;

  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;

  uint32 buf_len = 0;

  /* ---------------------------------------------------------------------- */

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */   
  encoded_pdu.length = sizeof(llc_ho_full_config_mode_change_ul_only_msg);
  encoded_pdu.value = llc_ho_full_config_mode_change_ul_only_msg;
  
  /* Set the version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);

  /* The freq and cell id should be same as what was sent in LTE_CPHY_HANDOVER_REQ */
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            freq == cphy_cfg_req_ptr->ho_target_cell_info.dl_cell_frequency);
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            phy_cell_id == cphy_cfg_req_ptr->ho_target_cell_info.target_cell_id);

  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req_ptr,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_HO);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_C_RNTI_CFG_BM |
                                                      LTE_MAC_LC_INFO_CFG_BM |
                                                      LTE_MAC_UL_CFG_BM |
                                                      LTE_MAC_RACH_CFG_BM));
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].priority == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].pbr == 32);

  /* Construct and send the lte_mac_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);
  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 2);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[0] == 3);

  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);

  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpdl_cfg_req_ptr->released_rb_cfg_idx[0] == 3);

  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 1);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_utf_get_next_ext_msg((byte **) &drx_opt_req_ptr,&buf_len);
  TF_ASSERT(drx_opt_req_ptr->msgr_hdr.id == LTE_CPHY_DRX_OPT_REQ);
  TF_ASSERT(drx_opt_req_ptr->um_mode_enabled == TRUE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].priority == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].pbr == 16);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr != NULL);
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_AM);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr != NULL);
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_AM);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_ADD);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_with_mode_change_from_am_to_ul_only_full_config_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_with_mode_change_from_am_to_dl_only_full_config_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility with
    rel 9 full config option that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_with_mode_change_from_am_to_dl_only_full_config_failure
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_cphy_drx_opt_req_s *drx_opt_req_ptr = NULL;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;

  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;

  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;

  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;

  uint32 buf_len = 0;

  /* ---------------------------------------------------------------------- */

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */   
  encoded_pdu.length = sizeof(llc_ho_full_config_mode_change_dl_only_msg);
  encoded_pdu.value = llc_ho_full_config_mode_change_dl_only_msg;
  
  /* Set the version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 1);

  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);

  /* The freq and cell id should be same as what was sent in LTE_CPHY_HANDOVER_REQ */
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            freq == cphy_cfg_req_ptr->ho_target_cell_info.dl_cell_frequency);
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            phy_cell_id == cphy_cfg_req_ptr->ho_target_cell_info.target_cell_id);

  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req_ptr,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_HO);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_C_RNTI_CFG_BM |
                                                      LTE_MAC_LC_INFO_CFG_BM |
                                                      LTE_MAC_UL_CFG_BM |
                                                      LTE_MAC_RACH_CFG_BM));
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].priority == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].pbr == 16);

  /* Construct and send the lte_mac_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);
  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);

  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 2);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 0);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 3);

  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 1);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpul_cfg_req_ptr->released_rb_cfg_idx[0] == 3);

  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_utf_get_next_ext_msg((byte **) &drx_opt_req_ptr,&buf_len);
  TF_ASSERT(drx_opt_req_ptr->msgr_hdr.id == LTE_CPHY_DRX_OPT_REQ);
  TF_ASSERT(drx_opt_req_ptr->um_mode_enabled == TRUE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 0);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 0);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].priority == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].pbr == 16);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr != NULL);
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[0] == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_AM);
  

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr != NULL);
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 0);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_AM);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_ADD);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_with_mode_change_from_am_to_dl_only_full_config_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_with_mode_change_from_ul_only_to_am_full_config_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility with
    rel 9 full config option that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_with_mode_change_from_ul_only_to_am_full_config_failure
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_cphy_drx_opt_req_s *drx_opt_req_ptr = NULL;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;

  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;

  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;

  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;

  uint32 buf_len = 0;

  /* ---------------------------------------------------------------------- */

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */   
  encoded_pdu.length = sizeof(llc_ho_full_config_mode_change_am);
  encoded_pdu.value = llc_ho_full_config_mode_change_am;
  
  /* Set the version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);

  /* The freq and cell id should be same as what was sent in LTE_CPHY_HANDOVER_REQ */
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            freq == cphy_cfg_req_ptr->ho_target_cell_info.dl_cell_frequency);
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            phy_cell_id == cphy_cfg_req_ptr->ho_target_cell_info.target_cell_id);

  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req_ptr,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_HO);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_C_RNTI_CFG_BM |
                                                      LTE_MAC_LC_INFO_CFG_BM |
                                                      LTE_MAC_UL_CFG_BM |
                                                      LTE_MAC_RACH_CFG_BM));
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].priority == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].pbr == 32);

  /* Construct and send the lte_mac_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);
  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_AM);

  

  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_AM);

  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 1);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_ADD);

  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 1);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_utf_get_next_ext_msg((byte **) &drx_opt_req_ptr,&buf_len);
  TF_ASSERT(drx_opt_req_ptr->msgr_hdr.id == LTE_CPHY_DRX_OPT_REQ);
  TF_ASSERT(drx_opt_req_ptr->um_mode_enabled == FALSE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[2] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 10);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].priority == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].pbr == 16);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr != NULL);
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 2);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[0] == 3);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr != NULL);
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 2);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->released_rb_cfg_idx[0] == 3);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_with_mode_change_from_ul_only_to_am_full_config_failure() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_with_mode_change_from_dl_only_to_am_full_config_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility with
    rel 9 full config option that fails due to RACH. This would
    cause CRE to be triggered by Config. CRE would send CFG_REQI to revert L2
    back to old configuration and suspend RBs. This API also tests the latter
    in LLC and mimics the actions of CRE module.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_with_mode_change_from_dl_only_to_am_full_config_failure
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;

  lte_cphy_handover_req_s *cphy_cfg_req_ptr = NULL;
  lte_cphy_handover_cnf_s cphy_cfg_cnf_msg;

  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_cphy_drx_opt_req_s *drx_opt_req_ptr = NULL;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;

  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;

  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;

  lte_rrc_serving_cell_changed_indi_s *serving_cell_changed_ind_ptr = NULL;

  uint32 buf_len = 0;

  /* ---------------------------------------------------------------------- */

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */   
  encoded_pdu.length = sizeof(llc_ho_full_config_mode_change_am);
  encoded_pdu.value = llc_ho_full_config_mode_change_am;
  
  /* Set the version to rel 9 */
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel9;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /*--------------------------------------------------------------------------
                           PDCP CFG PREP Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCP CFG PREP Config request sent by LLC to PDCP */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCP Config prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 0);

  /* Construct and send the pdcpul_cfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCP */
  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCP */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg, sizeof(lte_pdcpul_recfg_prep_cnf_msg_s));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();
  /*--------------------------------------------------------------------------
                              CPHY Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the CPHY Config request sent by LLC to CPHY */
  lte_rrc_utf_get_next_ext_msg((byte **)&cphy_cfg_req_ptr,&buf_len);
  /* Make sure the CPHY Config request is valid */
  TF_ASSERT(sizeof(lte_cphy_handover_req_s) == buf_len);
  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_cfg_cnf_msg.msgr_hdr,LTE_CPHY_HANDOVER_CNF);
  cphy_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_CPHY_HANDOVER_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_cfg_cnf_msg, sizeof(cphy_cfg_cnf_msg));

  /* Catch the serving cell changed indication thrown by LLC */
  lte_rrc_utf_get_next_int_msg((byte **) &serving_cell_changed_ind_ptr,&buf_len);
  TF_ASSERT(serving_cell_changed_ind_ptr->msg_hdr.id == LTE_RRC_SERVING_CELL_CHANGED_INDI);

  /* The freq and cell id should be same as what was sent in LTE_CPHY_HANDOVER_REQ */
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            freq == cphy_cfg_req_ptr->ho_target_cell_info.dl_cell_frequency);
  TF_ASSERT(serving_cell_changed_ind_ptr->serving_cell_id.
            phy_cell_id == cphy_cfg_req_ptr->ho_target_cell_info.target_cell_id);

  /*--------------------------------------------------------------------------
                              MAC Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the MAC Config request sent by LLC to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_cfg_req_ptr,&buf_len);
  /* Make sure the MAC Config request is valid */
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_HO);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_C_RNTI_CFG_BM |
                                                      LTE_MAC_LC_INFO_CFG_BM |
                                                      LTE_MAC_UL_CFG_BM |
                                                      LTE_MAC_RACH_CFG_BM));
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 3);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 0);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].priority == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.lc_list[10].pbr == 32);

  /* Construct and send the lte_mac_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr,LTE_MAC_CFG_CNF);
  /* Setting the fields in the LTE_MAC_CFG_CNF sent by MAC */
  mac_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_MAC_CFG_CNF to LLC there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg, sizeof(mac_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);
  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_AM);

  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLCDL_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              RLC UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcul_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_AM);

  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLCUL_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP DL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPDL Config request sent by LLC to PDCPDL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 1);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  /*--------------------------------------------------------------------------
                              PDCP UL Request Handling Test
  ---------------------------------------------------------------------------*/
  /* Catch the PDCPUL Config request sent by LLC to PDCPUL*/
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_HO);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 1);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 3);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_ADD);

  /* Construct and send the lte_pdcp_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_utf_get_next_ext_msg((byte **) &drx_opt_req_ptr,&buf_len);
  TF_ASSERT(drx_opt_req_ptr->msgr_hdr.id == LTE_CPHY_DRX_OPT_REQ);
  TF_ASSERT(drx_opt_req_ptr->um_mode_enabled == FALSE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Send LTE_RRC_CFG_REQI to LLC for HO failure due to RACH*/
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf(LTE_RRC_HO_FAILURE_CAUSE_RACH);
  // ---------------------------------------------

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  TF_ASSERT(pdcpul_cfg_prep_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->num_released_rb == 1);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  /* Verify MAC access abort request */
  lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf();

 // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_type == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.cfg_bitmask == LTE_MAC_LC_INFO_CFG_BM);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_addmod_lc == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.num_deleted_lc == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[0] == 1);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.addmod_lc_list[1] == 2);
  TF_ASSERT(mac_cfg_req_ptr->cfg_data.lc_info_cfg.deleted_lc_list[0] == 10);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf_msg.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf_msg.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf_msg.hdr, sizeof(mac_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req_ptr != NULL);
  TF_ASSERT(rlcdl_cfg_req_ptr->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  TF_ASSERT(rlcdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_modified_rb == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->modified_rb[2].action == LTE_RLC_RB_ACTION_ADD);
  TF_ASSERT(rlcdl_cfg_req_ptr->released_rb_cfg_idx[0] == 3);
  TF_ASSERT(rlcdl_cfg_req_ptr->rb_cfg_base_ptr[3].rb_mode == LTE_RB_RLC_MODE_UM);
  

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req_ptr != NULL);
  TF_ASSERT(rlcul_cfg_req_ptr->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  TF_ASSERT(rlcul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(rlcul_cfg_req_ptr->num_modified_rb == 2);
  TF_ASSERT(rlcul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[0].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(rlcul_cfg_req_ptr->modified_rb[1].action == LTE_RLC_RB_ACTION_MODIFY);
  TF_ASSERT(rlcul_cfg_req_ptr->released_rb_cfg_idx[0] == 3);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg.msg_hdr, sizeof(rlc_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpdl_cfg_req_ptr->hdr.id == LTE_PDCPDL_CFG_REQ);
  TF_ASSERT(pdcpdl_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_add_modified_rb == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->num_released_rb == 0);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].rb_cfg_idx == 3);
  TF_ASSERT(pdcpdl_cfg_req_ptr->add_modified_rb[2].cfg_act == LTE_PDCP_CFG_ACT_FULL_CONFIG);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg.hdr, sizeof(pdcpdl_cfg_cnf_msg));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_req_ptr->hdr.id == LTE_PDCPUL_CFG_REQ);
  TF_ASSERT(pdcpul_cfg_req_ptr->rohc_reset == TRUE);
  TF_ASSERT(pdcpul_cfg_req_ptr->cfg_reason == LTE_L2_CFG_RADIO_FAILURE);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_add_modified_rb == 2);
  TF_ASSERT(pdcpul_cfg_req_ptr->num_released_rb == 1);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].rb_cfg_idx == 33);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[0].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].rb_cfg_idx == 34);
  TF_ASSERT(pdcpul_cfg_req_ptr->add_modified_rb[1].cfg_act == LTE_PDCP_CFG_ACT_MODIFIED);
  TF_ASSERT(pdcpul_cfg_req_ptr->released_rb_cfg_idx[0] == 3);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf_msg to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg.hdr, sizeof(pdcpul_cfg_cnf_msg));

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  lte_rrc_llc_test_free_sib_ptr();
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_with_mode_change_from_dl_only_to_am_full_config_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rohc_reset

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing R11 ROHC Reset

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_rohc_reset
(
  lte_rrc_pdu_buf_s encoded_pdu
)
{   
  lte_rrc_llc_test_intra_freq_mobility_only(encoded_pdu, TRUE);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_rohc_reset() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_tm9_wo_csi_rs_cfg

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing tm9 without CSI RS cfg

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_tm9_wo_csi_rs_cfg
(
  lte_rrc_pdu_buf_s encoded_pdu
)
{  
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  uint32 buf_len;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_dedicated_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_dedicated_cfg_req_s) == buf_len);

  /* Construct and send the lte_cphy_dedicated_cfg_cnf message back to LLC */
  lte_rrc_init_default_hdr(&cphy_dedicated_cfg_cnf.msgr_hdr,LTE_CPHY_DEDICATED_CFG_CNF);
  cphy_dedicated_cfg_cnf.status = E_SUCCESS;

  /* Send the LTE_CPHY_DEDICATED_CFG_CNF to LLC there by mimicing the action of CPHY */
  lte_rrc_utf_send_msg((byte*)&cphy_dedicated_cfg_cnf, sizeof(cphy_dedicated_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Update the stored RB info based on the OTA msg */
  lte_rrc_llc_test_update_stored_rb_info(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_tm9_wo_csi_rs_cfg() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_scell_cqi_bw_test

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing Scell

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_scell_cqi_bw_test
(
  lte_rrc_pdu_buf_s encoded_pdu
)
{  
  lte_cphy_dedicated_cfg_req_s *cphy_dedicated_cfg_req_ptr = NULL;
  lte_cphy_dedicated_cfg_cnf_s cphy_dedicated_cfg_cnf;
  uint32 buf_len;

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_scell_cqi_bw_test() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_feicic_crs_info

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing FeICIC CRS info

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_feicic_crs_info
(
  lte_rrc_pdu_buf_s encoded_pdu
)
{   
  int decode_status = 0;
  
  lte_rrc_llc_test_common_data.dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(lte_rrc_llc_test_common_data.dl_dcch_msg_ptr != NULL);

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, TRUE);

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_MSG("\nConnection Reconfiguration Test Ends \n");

  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
} /* lte_rrc_llc_test_feicic_crs_info() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_ho_validation_failure

===========================================================================*/
/*!
    @brief
    This API tests RRC connection reconfiguration containing mobility related
    information that results in a validation failure.

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_llc_test_ho_validation_failure
(
  lte_rrc_pdu_buf_s encoded_pdu
)
{  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);  

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_ho_validation_failure() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_connection_reestablishment

===========================================================================*/
/*!
    @brief
    This tests connection reestablishment handling by LLC.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_connection_reestablishment
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reestablishment Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_cre_msg);
  encoded_pdu.value = llc_cre_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CRE, FALSE);

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CRE, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CRE);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CRE);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CRE);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRE, NULL);

  /* Set the reconfig after cre flag in test DB to TRUE */
  lte_rrc_llc_test_common_data.reconfig_after_cre = TRUE;

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reestablishment Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
} /* lte_rrc_llc_test_rrc_connection_reestablishment() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_rrc_connection_rel_during_reestablishment

===========================================================================*/
/*!
    @brief
    This tests connection reestablishment handling by LLC.

    @return
    void
 */
/*=========================================================================*/
void lte_rrc_llc_test_rrc_connection_rel_during_reestablishment
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;
  uint32 buf_len = 0;
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_mac_cfg_req_msg_s *mac_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s mac_access_abort_cnf_msg;
  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf_msg;
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_recfg_prep_req_ptr = NULL;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_recfg_prep_cnf;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reestablishment Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */ 
  encoded_pdu.length = sizeof(llc_cre_msg);
  encoded_pdu.value = llc_cre_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CRE, FALSE);

  /* Verify phy dedicated config request */
  lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf(LTE_RRC_PROC_CRE, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CRE);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CRE);

  /* Catch the PDCPUL Config request sent by LLC to PDCPUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);

  // Send CONN REL
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CRP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[2] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[2];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_CONN_REL;
  cfg_reqi.cfg_info[0].cfg_u.rel_cfg.ml1_status = LTE_RRC_ML1_STATUS_NOT_RESUMED;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CONN REL that will be pended\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  /* Catch the PDCPUL reconfig prep request sent by LLC to PDCPUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_recfg_prep_req_ptr,&buf_len);
  /* Make sure the PDCPUL reconfig prep request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_recfg_prep_req_msg_s) == buf_len);

  /* Construct and send the lte_pdcpul_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  /* Construct and send the lte_pdcpul_recfg_prep_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_recfg_prep_cnf.hdr,LTE_PDCPUL_RECFG_PREP_CNF);
  /* Setting the fields in the LTE_PDCPUL_RECFG_PREP_CNF sent by PDCPUL */
  pdcpul_recfg_prep_cnf.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_RECFG_PREP_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_recfg_prep_cnf, sizeof(pdcpul_recfg_prep_cnf));
  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_next_ext_msg((byte **) &mac_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);

  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();
  
  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);

  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);

  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);

  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);

  /* Construct and send the lte_pdcpul_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_FAILURE, LTE_RRC_PROC_CRE, NULL);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CRP, NULL);

  /* Set the reconfig after cre flag in test DB to TRUE */
  lte_rrc_llc_test_common_data.reconfig_after_cre = TRUE;

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reestablishment Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
  lte_rrc_llc_test_free_sib_ptr();
} /* lte_rrc_llc_test_rrc_connection_rel_during_reestablishment() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_first_reconfig_after_cre

===========================================================================*/
/*!
    @brief
    This API tests the handling of first reconfiguration after CRE by LLC. The
    reconfig message in this test doesn't contain any RB information. Yet, all
    the suspended RBs should be resumed at PDCP.

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
void lte_rrc_llc_test_first_reconfig_after_cre
(
  void
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nConnection Reconfiguration Test Begins \n");

  /* Setting the encoded message to the byte string in lte_rrc_llc_ota_msg.h */  
  encoded_pdu.length = sizeof(llc_reconfig_mac_msg);
  encoded_pdu.value = llc_reconfig_mac_msg;
  
  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_ded_cfg_reqi(&encoded_pdu, LTE_RRC_PROC_CONFIG, FALSE);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf(FALSE, NULL, LTE_RRC_PROC_CONFIG);

  /* Verify RLC DL/UL config requests */
  lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify PDCP DL/UL config requests */
  lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf(LTE_RRC_PROC_CONFIG);

  /* Verify DRX opt request being sent to ML1 */
  lte_rrc_llc_test_verify_drx_opt_req();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONFIG, NULL);

  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nConnection Reconfiguration Test Ends \n");
  lte_rrc_llc_test_free_dl_ded_msg_ptr();
} /* lte_rrc_llc_test_first_reconfig_after_cre() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_embms_mcch_mtch_config

===========================================================================*/
/*!
    @brief
    This API tests the setup of MCCH/MTCH for eMBMS

 */
/*=========================================================================*/
void lte_rrc_llc_test_embms_mcch_mtch_config
(
  void
)
{

#ifdef FEATURE_LTE_EMBMS
  /* Copy the CC to OC in test DB */
  lte_rrc_llc_test_copy_test_db(&lte_rrc_llc_test_common_data.oc_mrb_info,
                                &lte_rrc_llc_test_common_data.cc_mrb_info);

  /* Update the MRB info in OC in test DB */
  lte_rrc_llc_test_update_stored_mrb_info(&lte_rrc_llc_test_common_data.oc_mrb_info);

  /* Verify PHY common config request for verifying eMBMS params */
  lte_rrc_llc_test_verify_phy_common_cfg_req_and_send_cnf_embms(FALSE, FALSE);

  if (lte_rrc_llc_test_common_data.llc_cfg_req.cfg_info[0].cfg_u.embms_cfg.num_mtch > 0)
  {
    /* Verify MAC config request */
    lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf_embms(FALSE, FALSE, FALSE);

    /* Verify RLC DL/UL config requests */
    lte_rrc_llc_test_verify_rlcdl_cfg_req_and_send_cnf_embms(FALSE, FALSE, FALSE);
  }

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_EMP, NULL);
  
  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Copy the OC to CC in test DB */
  lte_rrc_llc_test_copy_test_db(&lte_rrc_llc_test_common_data.cc_mrb_info,
                                &lte_rrc_llc_test_common_data.oc_mrb_info);
  #endif
} /* lte_rrc_llc_test_embms_mcch_mtch_config() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_embms_mcch_mtch_release

===========================================================================*/
/*!
    @brief
    This API tests the release of MCCH/MTCH for eMBMS

 */
/*=========================================================================*/
void lte_rrc_llc_test_embms_mcch_mtch_release
(
  boolean cfg_for_release,
  boolean release_all_mrb,
  boolean release_all_rb
)
{

  /* ---------------------------------------------------------------------- */
  TF_MSG("\neMBMS - MCCH/MTCH release Test Begins \n");

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mrb_rel(release_all_mrb);

  /* Copy the CC to OC in test DB */
  lte_rrc_llc_test_copy_test_db(&lte_rrc_llc_test_common_data.oc_mrb_info,
                                &lte_rrc_llc_test_common_data.cc_mrb_info);

  /* Update the MRB info in OC in test DB */
  lte_rrc_llc_test_update_stored_mrb_info(&lte_rrc_llc_test_common_data.oc_mrb_info);

  /* Verify PHY common config request for verifying eMBMS params */
  lte_rrc_llc_test_verify_phy_common_cfg_req_and_send_cnf_embms(cfg_for_release, release_all_mrb);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf_embms(cfg_for_release, release_all_mrb, release_all_rb);

  /* Verify RLC DL request */
  lte_rrc_llc_test_verify_rlcdl_cfg_req_and_send_cnf_embms(cfg_for_release, release_all_mrb, release_all_rb);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_EMP, NULL);
  
  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Copy the OC to CC in test DB */
  lte_rrc_llc_test_copy_test_db(&lte_rrc_llc_test_common_data.cc_mrb_info,
                                &lte_rrc_llc_test_common_data.oc_mrb_info);

  TF_MSG("\neMBMS - MCCH/MTCH release Test Ends \n");
  
} /* lte_rrc_llc_test_embms_mcch_mtch_release() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_embms_release_for_lte_stop

===========================================================================*/
/*!
    @brief
    This API tests the handling of LTE stop when MCCH/MTCH for eMBMS are configured

 */
/*=========================================================================*/
void lte_rrc_llc_test_embms_release_for_lte_stop
(
  boolean cfg_for_release,
  boolean release_all_mrb,
  boolean release_all_rb
)
{

  /* ---------------------------------------------------------------------- */
  TF_MSG("\neMBMS release during LTE stop Test Begins \n");

  /* Send LTE_RRC_CFG_REQI to LLC */
  lte_rrc_llc_test_construct_and_send_cfg_reqi_for_lte_stop();

  /* Copy the CC to OC in test DB */
  lte_rrc_llc_test_copy_test_db(&lte_rrc_llc_test_common_data.oc_mrb_info,
                                &lte_rrc_llc_test_common_data.cc_mrb_info);

  /* Update the MRB info in OC in test DB */
  lte_rrc_llc_test_update_stored_mrb_info(&lte_rrc_llc_test_common_data.oc_mrb_info);

  /* Verify MAC config request */
  lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf_embms(TRUE, release_all_mrb, release_all_rb);

  /* Verify RLC DL request */
  lte_rrc_llc_test_verify_rlcdl_cfg_req_and_send_cnf_embms(TRUE, release_all_mrb, release_all_rb);

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CONTROLLER, NULL);
  
  /* After LLC completes processing, assert its in INITIAL state */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  /* Copy the OC to CC in test DB */
  lte_rrc_llc_test_copy_test_db(&lte_rrc_llc_test_common_data.cc_mrb_info,
                                &lte_rrc_llc_test_common_data.oc_mrb_info);

  TF_MSG("\neMBMS release during LTE stop Test Ends \n");
  
} /* lte_rrc_llc_test_embms_mcch_mtch_setup() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_csp_get_camped_cell_bw

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API that returns the camped cell bandwidth. 

    @return
    lte_errno_e
*/
/*=========================================================================*/
lte_errno_e lte_rrc_llc_test_csp_get_camped_cell_bw
(
  lte_bandwidth_e *bw_ptr /*!< Pointer to BW to be filled in */
)
{
  lte_errno_e status = E_SUCCESS;
  *bw_ptr = LTE_BW_NRB_100;
  return status;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_csp_get_camped_cell_bw1

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API that returns the camped cell bandwidth. 

    @return
    lte_errno_e
*/
/*=========================================================================*/
lte_errno_e lte_rrc_llc_test_csp_get_camped_cell_bw1
(
  lte_bandwidth_e *bw_ptr /*!< Pointer to BW to be filled in */
)
{
  lte_errno_e status = E_SUCCESS;
  *bw_ptr = LTE_BW_NRB_6;
  return status;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_csp_get_camped_cell_bw2

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API that returns the camped cell bandwidth. 

    @return
    lte_errno_e
*/
/*=========================================================================*/
lte_errno_e lte_rrc_llc_test_csp_get_camped_cell_bw2
(
  lte_bandwidth_e *bw_ptr /*!< Pointer to BW to be filled in */
)
{
  lte_errno_e status = E_SUCCESS;
  *bw_ptr = LTE_BW_NRB_50;
  return status;
}


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_sec_param_prepare_config

===========================================================================*/
/*!
    @brief
    This function simulates the Security API that prepares security config
    params for initial security activation and reestablishment 

    @return
    lte_errno_e
*/
/*=========================================================================*/
lte_errno_e lte_rrc_llc_test_sec_param_prepare_config
(
  uint8 ncc,  /*! Nexthop Chaining Count */
  boolean ncc_is_valid, /*! Is ncc being passed? */
  lte_pdcp_security_cfg_s *sec_config_ptr   /*!Sec Cfg Ptr */
)
{
  return E_SUCCESS;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_sec_dcch_prepare_config

===========================================================================*/
/*!
    @brief
    This function simulates the Security API that prepares security config
    params for handover 

    @return
    lte_errno_e
*/
/*=========================================================================*/
lte_errno_e lte_rrc_llc_test_sec_dcch_prepare_config
(
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr, /*! Pointer to decoded ASN.1 DL DCCH Message */
  lte_pdcp_security_cfg_s *sec_config_ptr,   /*!Sec Cfg Ptr */
  /*!Phy cell ID of the target cell*/
  lte_phy_cell_id_t phy_cell_id,
  /*!Downlink EARFCN of the target cell*/
  lte_earfcn_t dl_freq
)
{
  return E_SUCCESS;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_sec_process_irat_to_lte_ho_config

===========================================================================*/
/*!
    @brief
    This function simulates the Security API that prepares security config
    params for handover 

    @return
    lte_errno_e
*/
/*=========================================================================*/
lte_errno_e lte_rrc_llc_test_sec_process_irat_to_lte_ho_config
(
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr, /*! Pointer to decoded ASN.1 DL DCCH Message */
  lte_pdcp_security_cfg_s *sec_config_ptr,   /*!Sec Cfg Ptr */
  /*!Phy cell ID of the target cell*/
  lte_phy_cell_id_t phy_cell_id,
  /*!Downlink EARFCN of the target cell*/
  lte_earfcn_t dl_freq
)
{
  return E_SUCCESS;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_csp_get_num_tx_antennas_simo

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API that returns the number of tx antenna,
    for SIMO tests.

    @return
    uint8
*/
/*=========================================================================*/
uint8 lte_rrc_llc_test_csp_get_num_tx_antennas_simo
(
  void
)
{
  return 1;
}

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_csp_get_num_tx_antennas_mimo

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API that returns the number of tx antenna,
    for MIMO tests.

    @return
    uint8
*/
/*=========================================================================*/
uint8 lte_rrc_llc_test_csp_get_num_tx_antennas_mimo
(
  void
)
{
  return 2;
}

#ifdef FEATURE_LTE_EMBMS

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_emp_get_mbsfnareaconfig

===========================================================================*/
/*!
    @brief
    Get MBSFN area config ptr from mcch db 

    @return
    ptr to MBSFN area config

 */
/*=========================================================================*/
lte_rrc_osys_MCCH_Message *lte_rrc_llc_test_emp_get_mbsfnareaconfig
(
  lte_mbsfn_area_id_t mbsfn_area_id
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  int decode_status = 0;

  if (lte_rrc_llc_test_common_data.dl_mcch_msg_ptr == NULL) 
  {    
    
    encoded_pdu.length = sizeof(llc_mcch_msg);
    encoded_pdu.value = llc_mcch_msg;
    lte_rrc_llc_test_common_data.dl_mcch_msg_ptr 
      = (lte_rrc_osys_MCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                                                  encoded_pdu.length,
                                                                  MCCH_Message,
                                                                  &decode_status);
    TF_ASSERT(decode_status == 0);
    TF_ASSERT(lte_rrc_llc_test_common_data.dl_mcch_msg_ptr != NULL);
  }

  return lte_rrc_llc_test_common_data.dl_mcch_msg_ptr;

} 

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_emp_get_mbsfnareaconfig_invalid

===========================================================================*/
/*!
    @brief
    Get MBSFN area config invalid msg ptr from mcch db 

    @return
    ptr to MBSFN area config

 */
/*=========================================================================*/
lte_rrc_osys_MCCH_Message *lte_rrc_llc_test_emp_get_mbsfnareaconfig_invalid
(
  lte_mbsfn_area_id_t mbsfn_area_id
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  int decode_status = 0;

  if (lte_rrc_llc_test_common_data.dl_mcch_msg_ptr == NULL) 
  {    
    
    encoded_pdu.length = sizeof(llc_mcch_invalid_msg);
    encoded_pdu.value = llc_mcch_invalid_msg;
    lte_rrc_llc_test_common_data.dl_mcch_msg_ptr 
      = (lte_rrc_osys_MCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                                                  encoded_pdu.length,
                                                                  MCCH_Message,
                                                                  &decode_status);
    TF_ASSERT(decode_status == 0);
    TF_ASSERT(lte_rrc_llc_test_common_data.dl_mcch_msg_ptr != NULL);
  }

  return lte_rrc_llc_test_common_data.dl_mcch_msg_ptr;

}
#endif

static boolean lte_rrc_is_rel_10_ca_enabled_true(void)
{
	return true;
}

/*   FUNCTION:  lte_rrc_llc_test_stub_init

===========================================================================*/
/*!
    @brief
    Initializes the stub required for llc unit test 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_llc_test_stub_init
( 
  void 
)
{
  TF_STUB(lte_rrc_csp_get_serving_cell_params) 
  = lte_rrc_llc_test_common_csp_get_serving_cell_params;
  TF_STUB(lte_rrc_csp_get_camped_cell_bw) = lte_rrc_llc_test_csp_get_camped_cell_bw;
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_basic_get_cell_sib;
  TF_STUB(lte_rrc_controller_get_state) = lte_rrc_llc_test_controller_get_state;
  TF_STUB(lte_rrc_sec_dcch_prepare_config) = lte_rrc_llc_test_sec_dcch_prepare_config;
  TF_STUB(lte_rrc_sec_param_prepare_config) = lte_rrc_llc_test_sec_param_prepare_config;
  //TF_STUB(lte_rrc_sec_process_irat_to_lte_ho_config) = lte_rrc_llc_test_sec_process_irat_to_lte_ho_config;
  TF_STUB(lte_rrc_csp_get_num_tx_antennas) = lte_rrc_llc_test_csp_get_num_tx_antennas_simo;
  TF_STUB(lte_rrc_cap_inter_freq_is_supported) = lte_rrc_llc_test_cap_inter_freq_is_supported;
  TF_STUB(lte_rrc_sib_b12_to_b17_conversion_is_needed) 
  = lte_rrc_llc_test_sib_b12_to_b17_conversion_is_not_needed;
  TF_STUB(lte_rrc_meas_get_eutra_meas_obj_param) = lte_rrc_llc_test_get_eutra_meas_obj_param;
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel8;
  //TF_STUB(lte_rrc_get_tdd_prune_required) = lte_rrc_llc_test_get_tdd_prune_required_false;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_llc_test_get_fgi_bit_1;
//  TF_STUB(lte_rrc_rohc_is_supported) = lte_rrc_llc_test_rohc_is_supported;
  //TF_STUB(lte_rrc_config_ho_to_eutra_in_progress) = lte_rrc_llc_test_ho_to_eutra_in_progress_false;
  //TF_STUB(lte_rrc_controller_tune_aways_enabled) = lte_rrc_llc_test_dsds_mode;
  #ifdef FEATURE_LTE_EMBMS
  TF_STUB(lte_rrc_esmgr_get_oos_warn_reason) = lte_rrc_llc_test_esmgr_get_oos_warn_reason_none;
  TF_STUB(lte_rrc_emp_get_mbsfnareaconfig) = lte_rrc_llc_test_emp_get_mbsfnareaconfig;
  #endif
  TF_STUB(lte_rrc_csp_lte_band_is_supported) = lte_rrc_llc_test_csp_lte_band_is_supported;
  //TF_STUB(lte_rrc_controller_get_cphy_rf_status_info) = lte_rrc_llc_test_cphy_rf_status_available;
  TF_STUB(lte_rrc_cap_transmit_antenna_selection_is_supported) = lte_rrc_llc_test_transmit_antenna_selection_is_supported_true;
  //TF_STUB(lte_ml1_validate_tdd_cfg_mbsfn_sf_alloc) = lte_rrc_llc_test_stub__ml1_validate_tdd_cfg_mbsfn_sf_alloc;

#ifdef FEATURE_LTE_CA
  TF_STUB(lte_rrc_cap_is_band_combo_valid) = lte_rrc_llc_test_get_band_combo_bitmask_FF;
  TF_STUB(lte_rrc_is_rel_10_ca_enabled) = lte_rrc_is_rel_10_ca_enabled_true;
#endif
}/*lte_rrc_llc_test_stub_init*/

/*===========================================================================

   FUNCTION:  lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif

===========================================================================*/
/*!
    @brief
    Initializes common config mask that is to be used for verification of
    received CPHY common cfg req 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif
(
  lte_cphy_common_cfg_mask_t cfg_mask
)
{
  if(LTE_CPHY_COMMON_CFG_CURRENT_VALID_MASK_BITS != cfg_mask)
  {
    cphy_common_cfg_verif_mask = cfg_mask;
  }
  lte_rrc_llc_test_common_cfg_sts = LTE_RRC_LLC_TEST_COMMON_CFG_NEW;
}




