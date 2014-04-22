/*!
  @file lte_rrc_llc_test_common.h

  @brief
  Utilities for llc unit tests.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/llc/lte_rrc_llc_test_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/08   da     Initial Revision
===========================================================================*/

#ifndef LTE_RRC_LLC_TEST_COMMON_H
#define LTE_RRC_LLC_TEST_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <stm2.h>
#include "lte_cphy_msg.h"
#include "lte_mac_msg.h"
#include "lte_mac_rrc.h"
#include "lte_rlc_msg.h"
#include "lte_pdcp_msg.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "__lte_rrc_llc.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_llci.h"
#include "lte_rrc_osys_asn1util.h"
}

/*===========================================================================

                        INTERNAL DEFINITIONS AND VARIABLES

===========================================================================*/
/* When a UMID is added or removed from the list below, LTE_RRC_LLC_TEST_UMID_COUNT
  macro should be updated accordingly */

#define LTE_RRC_LLC_TEST_UMID_LIST    \
{                                     \
  LTE_CPHY_COMMON_CFG_REQ,            \
  LTE_CPHY_DEDICATED_CFG_REQ,         \
  LTE_CPHY_HANDOVER_REQ,              \
  LTE_CPHY_ABORT_REQ,                 \
  LTE_CPHY_CON_RELEASE_REQ,           \
  LTE_CPHY_DRX_OPT_REQ,               \
  LTE_MAC_CFG_REQ,                    \
  LTE_MAC_ACCESS_ABORT_REQ,           \
  LTE_RLCDL_CFG_REQ,                  \
  LTE_RLCUL_CFG_REQ,                  \
  LTE_PDCPDL_CFG_REQ,                 \
  LTE_PDCPUL_CFG_REQ,                 \
  LTE_PDCPUL_RECFG_PREP_REQ,           \
  LTE_CPHY_ACQ_REQ \
};       

//ML1 Definitions
#define LTE_RRC_LLC_TEST_UMID_COUNT 14
#define LTE_RRC_LLC_TEST_MAX_SRS_BW_CFG 7
#define LTE_RRC_LLC_TEST_MAX_SRS_SUBFRAME_CFG 14
#define LTE_RRC_LLC_TEST_MAX_ALPHA 7
#define LTE_RRC_LLC_TEST_DELTAF_PUCCH_FORMAT 3
#define LTE_RRC_LLC_TEST_DELTAF_PUCCH_FORMAT1B 3
#define LTE_RRC_LLC_TEST_DELTAF_PUCCH_FORMAT2 4
#define LTE_RRC_LLC_TEST_MAX_DELTA_SHIFT 2
#define LTE_RRC_LLC_TEST_MAX_HOPPING_MODE 1
#define LTE_RRC_LLC_TEST_MAX_ANTENNA_PORTS 2
#define LTE_RRC_LLC_TEST_MAX_PHICH_DURATION 1
#define LTE_RRC_LLC_TEST_MAX_PHICH_RESOURCE 3
#define LTE_RRC_LLC_TEST_MIN_DL_EARFCN 0
#define LTE_RRC_LLC_TEST_MAX_DL_EARFCN 39649
#define LTE_RRC_LLC_TEST_MIN_UL_EARFCN 18000
#define LTE_RRC_LLC_TEST_MAX_UL_EARFCN 39649
#define LTE_RRC_LLC_TEST_MAX_BANDWIDTH 5
#define LTE_RRC_LLC_TEST_MAX_T310 6
#define LTE_RRC_LLC_TEST_MAX_N310 7
#define LTE_RRC_LLC_TEST_MAX_N311 7
#define LTE_RRC_LLC_TEST_MAX_RADIO_FRAME_ALLOCATION_PERIOD 5
#define LTE_RRC_LLC_TEST_MAX_SUBFRAME_ASSIGN 6
#define LTE_RRC_LLC_TEST_MAX_SUBFRAME_PATTERN 8
#define LTE_RRC_LLC_TEST_MAX_P_A 7
#define LTE_RRC_LLC_TEST_MAX_REPETITION_FACTOR 2
#define LTE_RRC_LLC_TEST_MAX_SRS_BANDWIDTH 3
#define LTE_RRC_LLC_TEST_MAX_SRS_HOPPING_BANDWIDTH 3
#define LTE_RRC_LLC_TEST_MAX_SRS_CYCLIC_SHIFT 7
#define LTE_RRC_LLC_TEST_MAX_FILTER_COEFFICIENT 14
#define LTE_RRC_LLC_TEST_MAX_TRANSMISSION_MODE 6
#define LTE_RRC_LLC_TEST_MAX_CQI_REPORTING_MODE_APERIODIC 4
#define LTE_RRC_LLC_TEST_DSR_TRANS_MAX 4
#define LTE_RRC_LLC_TEST_MAX_SPS_INTERVAL 9
#define LTE_RRC_LLC_TEST_MAX_UL_CYCLIC_PREFIX_LENGTH 1
//MAC Definitions
#define LTE_RRC_LLC_TEST_MAX_RA_PREAMBLES_GROUP_A 14
#define LTE_RRC_LLC_TEST_MAX_MESSAGE_SIZE_GROUP_A 3
#define LTE_RRC_LLC_TEST_MAX_MESSAGE_PWR_OFFSET_GROUP_B 7
#define LTE_RRC_LLC_TEST_MAX_RA_PREAMBLES 15
#define LTE_RRC_LLC_TEST_MAX_PREAMBLE_INITIAL_PWR 15
#define LTE_RRC_LLC_TEST_MAX_PWR_RAMPING_STEP 3
#define LTE_RRC_LLC_TEST_PREAMBLE_TRANS_MAX 10
#define LTE_RRC_LLC_TEST_MAX_RA_RESPONSE_WIN_SIZE 7
#define LTE_RRC_LLC_TEST_MAX_CONTENTION_RESOLUTION_TMR 7
#define LTE_RRC_LLC_TEST_MAX_HARQ_TX 13
#define LTE_RRC_LLC_TEST_MAX_PERIODIC_BSR_TMR 14
#define LTE_RRC_LLC_TEST_MAX_RETX_BSR_TMR 5
#define LTE_RRC_LLC_TEST_MAX_TA_TMR 7
#define LTE_RRC_LLC_TEST_MAX_PERIODIC_PHR_TMR 7
#define LTE_RRC_LLC_TEST_MAX_PROHIBIT_PHR_TMR 7
#define LTE_RRC_LLC_TEST_MAX_DL_PATHLOSS_CHANGE 3
#define LTE_RRC_LLC_TEST_MAX_PBR 10
#define LTE_RRC_LLC_TEST_MAX_BUCKET_SIZE_DURATION 5
#define LTE_RRC_LLC_TEST_MAX_ON_DURATION_TMR 15
#define LTE_RRC_LLC_TEST_MAX_DRX_INACTIVITY_TMR 22
#define LTE_RRC_LLC_TEST_MAX_DRX_RETX_TMR 7
#define LTE_RRC_LLC_TEST_MAX_LONG_DRX_CYCLE_START_OFFSET 16
#define LTE_RRC_LLC_TEST_MAX_SHORT_DRX_CYCLE 15
//RLC Definitions
#define LTE_RRC_LLC_TEST_MAX_T_REORDERING 30
#define LTE_RRC_LLC_TEST_MAX_T_STATUS_PROHIBIT 55
#define LTE_RRC_LLC_TEST_MAX_T_POLL_RETRANSMIT 54
#define LTE_RRC_LLC_TEST_MAX_T_POLL_PDU 7
#define LTE_RRC_LLC_TEST_MAX_T_POLL_BYTE 14
#define LTE_RRC_LLC_TEST_MAX_RETX_THRESHOLD 7
//PDCP Definitions
#define LTE_RRC_LLC_TEST_MAX_DISCARD_TIMER 7

/*! @brief The flags below are used in test cases for cross verification purpose.
  They are set when the respective parameters are configured in the test case and 
  reset when the configuration is released at some later point in the same test case.
*/
typedef struct
{
  boolean cqi_reporting_configured;
  boolean srs_configured;
  boolean sr_configured;
} lte_rrc_llc_test_cqi_srs_sr_info_s;

/*! @brief The flags below are used in test cases for cross verification purpose.
  They are set when the respective parameters are configured in the test case and 
  reset when the configuration is released at some later point in the same test case.
*/
typedef struct
{
  boolean tdd_cfg;
  boolean tdd_ack_nak_feedback_mode_enabled;
  boolean srx_max_up_pts_configured;
  boolean two_intervals_cfg_enabled;
  lte_l1_tdd_ul_dl_cfg_index_e tdd_ul_dl_cfg;
} lte_rrc_llc_test_tdd_param_s;

/*! @brief Structure used for storing RB-LC mapping in the test DB.
*/
typedef struct  
{
  lte_rb_cfg_idx_t rb_cfg_idx;
  lte_lc_id_t lc_id;
  lte_eps_id_t eps_id;
  uint8 rb_direction;
  lte_rb_rlc_mode_e  rb_mode;          
  boolean pdcp_status_required;
} rb_info_s;

/*! @brief Structure used for storing MRB-PMCH-LC mapping in the test DB.
*/
typedef struct  
{
  lte_mbsfn_area_id_t area_id;
  lte_pmch_id_t pmch_id;
  lte_lc_id_t lc_id;
  lte_rb_id_t mrb_id;

} mrb_info_s;

/*! @brief The flags below are used in test cases for cross verification purpose.
  They are set when the respective parameters are configured in the test case and 
  reset when the configuration is released at some later point in the same test case.
*/
typedef struct
{
  boolean cqi_mask_enabled;
  boolean pmi_ri_reporting_configured;
  boolean ded_rlf_enabled;
} lte_rrc_llc_rel9_param_s;

/*! @brief This structure stores MRB info
*/
typedef struct
{
  uint8 num_active_mrb;
  mrb_info_s mrb_info[LTE_MAX_ACTIVE_MRB];
} lte_rrc_llc_mrb_info_s;

/*! @brief Typedef of variables internal to module lte_rrc_llc_common_test_s.cpp
  These constitute the test DB that contain information used for cross verification
  in the unit tests.
*/
typedef struct
{
  uint32 trans_id[LTE_RRC_PENDQ_MAX_QUEUE_LEN]; 
  lte_l1_mib_info_param_s llc_basic_mib;
  lte_rrc_cell_info_s cell_info;
  lte_rrc_cfg_reqi_s llc_cfg_req;
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib1_ptr;
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib2_ptr;
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib5_ptr;
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib13_ptr;
  lte_rrc_osys_DL_CCCH_Message *dl_ccch_msg_ptr;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr;
#ifdef FEATURE_LTE_EMBMS
  lte_rrc_osys_MCCH_Message *dl_mcch_msg_ptr;
#endif  
  lte_rrc_llc_test_cqi_srs_sr_info_s cqi_srs_sr_info;
  lte_rrc_llc_rel9_param_s rel9_param;
  boolean ho_in_progress;
  boolean reconfig_after_cre;
  boolean full_config;
  lte_rrc_llc_test_tdd_param_s tdd_param;
  uint8 num_active_rb;
  rb_info_s rb_info[LTE_MAX_ACTIVE_RB];
  lte_rrc_llc_mrb_info_s cc_mrb_info;
  lte_rrc_llc_mrb_info_s oc_mrb_info;

} lte_rrc_llc_test_common_s;

/*===========================================================================

                          EXTERNAL VARIABLES

===========================================================================*/
extern lte_rrc_llc_test_common_s lte_rrc_llc_test_common_data;

extern lte_l1_mib_info_param_s llc_basic_mib;

extern uint32 lte_rrc_llc_test_umid_list[LTE_RRC_LLC_TEST_UMID_COUNT];

extern byte llc_basic_sib1_ssf9[17];

extern byte llc_basic_sib1_ssf9_1[17];
extern byte llc_basic_sib1_1[15];

extern byte llc_basic_sib2_2[27];

extern byte llc_invalid_sib2_2[29];

extern byte llc_invalid_sib5[29];

extern byte llc_sib1_tdd[16];

extern byte llc_sib2_with_mbsfn[29];

extern byte llc_conn_setup_default_ies[20];

extern byte llc_conn_setup_with_srs_ap[22];

extern byte llc_conn_setup_with_csi_ies[27];

extern byte llc_conn_setup_with_tm9_ies[30];

extern byte llc_conn_setup_with_tm1_csirs_enabled_antport_1_cqi_subband[31];

extern byte llc_conn_setup_with_r10_antenna_selection[21];

extern byte llc_conn_setup_with_ap_srs[20];

extern byte llc_conn_setup_with_r8_antenna_selection[21];
extern byte llc_conn_setup_with_csi_ies[27];

extern byte llc_rcfg_with_pcc_ap_cqi[21];

extern byte llc_rcfg_with_default_mac_cfg[19];

extern byte llc_rcfg_with_scc_ap_cqi[31];

extern byte llc_rcfg_with_invalid_bw_scc_ap_cqi[31];

extern byte llc_rcfg_with_scc_csi[36];

extern byte llc_rcfg_with_pcc_prd_cqi[24];

extern byte llc_rcfg_with_scc_rel[18];

extern byte llc_rcfg_with_scc_added_back[30];

extern byte llc_conn_setup_with_csi_pmi_ies[34];

extern byte llc_conn_setup_with_ack_nak_and_pucch_fmt[26];

extern byte llc_conn_setup_with_tm1_cqi_aperiodic_m30_antport_1[28];

extern byte llc_conn_setup_with_tm4_cqi_aperiodic_m22_antport_4[28];

extern byte llc_conn_setup_with_no_pucch[20];

extern byte llc_conn_setup_msg[26];

extern byte llc_reconfig_msg[44];

extern byte llc_feicic_crs_info_msg[37];

extern byte llc_feicic_crs_info_rel_msg[7];

extern byte llc_config_srb1_recfg_srb2_setup[5];

extern byte llc_reconfig_srb1_srb2_drb3_msg[16];

extern byte llc_conn_setup_with_drb_rel[16];

extern byte llc_reconfig_drb_am_msg[24];

extern byte llc_reconfig_drb_dl_um_msg[21];

extern byte llc_reconfig_drb_ul_um_msg[21];

extern byte llc_reconfig_duplicate_drb_msg[18];

extern byte llc_reconfig_same_drb_dl_released_ul_setup_msg[15];

extern byte llc_reconfig_same_drb_released_setup_msg[15];

extern byte llc_reconfig_drb3_setup_msg[13];

extern byte llc_reconfig_with_drb_rel_msg[5];

extern byte llc_reconfig_drb3_drb4_setup_msg[22];

extern byte llc_reconfig_drb3_release_drb4_setup_msg[21];

extern byte llc_reconfig_drb3_drb4_drb5_setup_msg[32];

extern byte llc_reconfig_drb3_pdcp_drb4_drb5_setup_msg[36];

extern byte llc_reconfig_drb3_drb4_drb5_rel_msg[9];

extern byte llc_reconfig_mac_msg[4];

extern byte llc_reconfig_phy_mac_drb3_rlc_msg[11];

extern byte llc_reconfig_phyded_msg[39];

extern byte llc_eicic_csi_msg[26];

extern byte llc_valid_eicic_csi_msg[26];

extern byte llc_eicic_csi_rel_msg[23];

extern byte llc_eicic_release_msg[25];

extern byte llc_csi_rs_msg[12];

extern byte llc_eicic_set_release_msg[10];

extern byte llc_eicic_set_FDD_msg[20];

extern byte llc_eicic_set_TDD_msg[22];

extern byte llc_eicic_zero_subframe_msg[16];

extern byte llc_eicic_set_TDD_msg1[16];

extern byte llc_eicic_TDD_invalid[29];

extern byte llc_eicic_csi_rs_cfg_invalid[16];

extern byte llc_eicic_csi_setup[25];

extern byte llc_eicic_csi_rel[10];

extern byte llc_eicic_msg[30];

extern byte llc_eicic_TDD_msg[28];

extern byte llc_eicic_TDD1_msg[34];

extern byte llc_eicic_TDD2_msg[33];

extern byte llc_reconfig_phy_msg[35];

extern byte llc_reconfig_trch_msg[11];

extern byte llc_conn_setup_with_no_phy_ded[3];

extern byte llc_invalid_conn_setup_both_srbs[4];

extern byte llc_invalid_conn_setup_non_rel8[2];

extern byte llc_invalid_conn_setup_id[3];

extern byte llc_invalid_conn_setup_mode[5];

extern byte llc_invalid_conn_setup_with_drb[8];

extern byte llc_invalid_drb_no_pdcp_msg[9];

extern byte llc_reconfig_unwanted_pdcp_msg[13];

extern byte llc_invalid_drb_no_rbmap_msg[13];

extern byte llc_invalid_drb_no_rlc_msg[12];

extern byte llc_invalid_multiple_rb_single_lc[24];

extern byte llc_invalid_rb_mode_change[7];

extern byte llc_invalid_sib2_spare_delta_field[29];

extern byte llc_invalid_conn_setup_crit_ext[1];

extern byte llc_invalid_drb_no_logchan_msg[12];

extern byte llc_invalid_drb_no_discard_timer_msg[13];

extern byte llc_invalid_ho_antenna_info_msg[15];

extern byte llc_invalid_crnti_msg[15];

extern byte llc_invalid_ho_inter_freq_msg[17];

extern byte llc_rohc_reset_ho_inter_freq_msg[20];

extern byte llc_wo_rohc_reset_ho_inter_freq_msg[17];

extern byte llc_invalid_ho_with_pdcp_modify_msg[43];

extern byte llc_invalid_duplicate_drb_msg[23];

extern byte llc_invalid_ap_srs_msg[10];

extern byte llc_invalid_ap_srs_0_msg[10];

extern byte llc_invalid_srs_aperiodic_msg[10];

extern byte llc_valid_srs_aperiodic_msg[10];

extern byte llc_valid_srs_msg[11];

extern byte llc_valid_apsrs_antenna_selection_rel_msg[12];

extern byte llc_reconfig_ho_msg[113];

extern byte llc_reconfig_ho_msg1[106];

extern byte llc_reconfig_ho_msg2[106];

extern byte llc_reconfig_ho_msg21[106];

extern byte llc_reconfig_ho_msg3[111];

extern byte llc_reconfig_ho_msg4[111];

extern byte llc_reconfig_ho_msg41[108];

extern byte llc_reconfig_ho_msg_tdd[106];

extern byte llc_reconfig_csi_rs_config_msg [45];

extern byte llc_reconfig_ho_with_drb_add_mod_rel_msg[110];

extern byte llc_reconfig_intra_freq_mobility_info_only_msg[40];

extern byte llc_reconfig_rel9_ho_full_config_msg_tdd[123];
  
extern byte llc_reconfig_ho_delta_config_msg[17];

extern byte llc_reconfig_ho_pdcp_modify_msg[40];

extern byte llc_reconfig_ho_rlc_pdcp_modify_msg[46];

extern byte llc_cre_msg[17];

extern byte llc_cre_msg_without_srs_dedicated[14];

extern byte llc_valid_cqi_fgi_105[17];

extern byte llc_reconfig_tm9_pmi_ri_enabled[11]; 
extern byte llc_reconfig_tm9_fgi_110_config[17];
extern byte llc_reconfig_tm9_fgi_109_config[17];
extern byte llc_invalid_tm9_csi_mode[17];
extern byte llc_reconfig_tm9_pmi_ri_enabled_ports_8[14];
extern byte llc_reconfig_tm9_aperiodic_rm22_pmiri_enabled_anport_4[14];
extern byte llc_reconfig_tm9_aperiodic_rm22_pmiri_enabled_anport_8[14];
extern byte llc_reconfig_tm9_aperiodic_rm20[11];
extern byte llc_reconfig_tm9_pmiri_enable_aperiodic_rm30[11];
extern byte llc_reconfig_tm9_pmi_ri_enabled_ports_2[14];
extern byte llc_reconfig_tm9_pmi_ri_disabled_ports_1[17];
extern byte llc_reconfig_rel9_phy_msg[43];

extern byte llc_reconfig_rel9_cqi_pmi_explicit_rel_msg[40];

extern byte llc_reconfig_rel9_cqi_pmi_implicit_rel_msg[33];

extern byte llc_reconfig_rel9_ho_full_config_msg[123];

extern byte llc_rel9_ho_full_config_mode_change_msg[59];

extern byte llc_ho_full_config_mode_change_ul_only_msg[56];

extern byte llc_ho_full_config_mode_change_am[59];

extern byte llc_ho_full_config_mode_change_dl_only_msg[56];

extern byte llc_reconfig_after_cre_rel9_full_config_msg[81];

extern byte llc_reconfig_ho_to_simo_msg[18];

extern byte llc_reconfig_ho_to_mimo_msg[18];

extern byte llc_sib13_msg[13];

extern byte llc_mcch_msg[43];
extern byte llc_reconfig_PHR_msg[25];

extern byte llc_reconfig_TTI_msg[32];

extern byte llc_reconfig_tm9_cbr_109_msg[48];

extern byte llc_reconfig_tm9_cbr_6_msg[35];

extern byte llc_reconfig_tm9_cbr_64_msg[42];

extern byte llc_reconfig_tm9_wo_csi_msg[24];

extern byte llc_scell_cqi_bw_test[32];
extern byte llc_scell_tm7_test[32];

extern byte llc_reconfig_tm9_wo_csi_pmi_msg[17];

extern byte llc_reconfig_ca_scell_add_with_apsrs_msg[12];
extern byte llc_reconfig_ca_scell_add_msg[23];
extern byte llc_mcch_invalid_msg[43];

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
extern void lte_rrc_llc_test_cphy_dedicated_cfg_cnf
(
   lte_cphy_dedicated_cfg_cnf_s *cnf_ptr, /*!< pointer to dedicated cfg cnf */ 
   boolean success, /*!< boolean to indicate whether to send success or failure */ 
   boolean invalid /*!< boolean to indicate whether to send failure with cause invalid cfg */ 
);

extern void lte_rrc_llc_test_cphy_common_cfg_cnf
(
   lte_cphy_common_cfg_cnf_s *cnf_ptr, /*!< pointer to common cfg cnf */ 
   boolean success, /*!< boolean to indicate whether to send success or failure */ 
   boolean invalid /*!< boolean to indicate whether to send failure with cause invalid cfg */ 
);

extern void lte_rrc_llc_test_mac_cfg_cnf
( 
  lte_mac_cfg_cnf_msg_s *cnf_ptr, /*!< pointer to mac cfg cnf */ 
  boolean success /*!< boolean to indicate whether to send success or failure */ 
);

extern void lte_rrc_llc_test_construct_common_cfg_reqi
( 
  lte_rrc_cfg_reqi_s *cfg_reqi_ptr,  /*!< pointer to cfg reqi */ 
  boolean sib1_update,               /*!< boolean to indicate SIB1 update */
  boolean sib2_update,               /*!< boolean to indicate SIB2 update */
  boolean default_cfg,               /*!< boolean to indicated default cfg */
  uint32 *trans_id
);

extern void lte_rrc_llc_test_construct_and_send_ded_cfg_reqi
(
  lte_rrc_pdu_buf_s *encoded_pdu_ptr, /*! Ptr to PDU to be decoded and sent to LLC */
  lte_rrc_proc_e source_proc,  /*!< source RRC proc */ 
  boolean pdu_decoded   /*!< if the PDU has been decoded */
);

extern void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_rlf
(
  lte_rrc_ho_failure_cause_e ho_failure_cause
);

extern void lte_rrc_llc_test_verify_cfg_cnfi
(
  lte_errno_e status,  /*!< status indicating success or failure */
  lte_rrc_proc_e proc,  /*!< source RRC proc */
  uint32 *trans_id_ptr   /*!< ptr to trans_id of CFG_REQI */
);

extern void lte_rrc_llc_test_basic_sib2_update
(
  void
);

extern void lte_rrc_llc_test_basic_sib2_update_for_invalid_ul_freq
(
  void
);

extern void lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg
(
  void
);

extern void lte_rrc_llc_test_verify_phy_common_cfg_req_and_send_cnf
(
  void
);

extern void lte_rrc_llc_test_verify_phy_dedicated_cfg_req_and_send_cnf
(
  lte_rrc_proc_e proc,  /*!< source RRC proc */ 
  boolean test_default_cfg  /*!< if the test is for default config */ 
);

extern void lte_rrc_llc_test_verify_drx_opt_req
(
  void
);

extern void lte_rrc_llc_test_verify_phy_handover_cfg_req_and_send_cnf
(
  boolean handover_success  /* if handover succeeded */
);

extern void lte_rrc_llc_test_verify_mac_cfg_req_and_send_cnf
(
  boolean common_config,  /*!< if verifying common config from SIB2 */
  lte_rrc_osys_SystemInformationBlockType2 *sib2_ptr,  /*!< SIB2 pointer */
  lte_rrc_proc_e proc  /*!< source RRC proc */ 
);

extern void lte_rrc_llc_test_verify_mac_access_abort_req_and_send_cnf
(
  void
);

extern void lte_rrc_llc_test_verify_rlc_cfg_req_and_send_cnf
(
  lte_rrc_proc_e proc
);

extern void lte_rrc_llc_test_verify_pdcpul_recfg_prep_req_and_send_cnf
(
  lte_rrc_proc_e proc
);

extern void lte_rrc_llc_test_verify_pdcp_cfg_req_and_send_cnf
(
  lte_rrc_proc_e proc
);

extern void lte_rrc_llc_test_update_stored_rb_info
(
  lte_rrc_proc_e proc /*!< source RRC procedure */
);

extern void lte_rrc_llc_test_rrc_connection_setup_default
(
  void
);
extern void lte_rrc_llc_test_rrc_connection_setup_tm1_csirs_enabled_antport_1_cqi_subband
( 
 void 
);

extern void lte_rrc_llc_test_rrc_connection_setup_with_r10_antenna_selection
(
  void
);

extern void lte_rrc_llc_test_rrc_connection_setup_with_apsrs
(
  void
);

extern void lte_rrc_llc_test_rrc_connection_setup_with_r8_antenna_selection
(
  void
);

extern void lte_rrc_llc_test_rrc_connection_setup_tm4_cqi_aperiodic_m22_antport_4
(
  void
);

extern void lte_rrc_llc_test_rrc_connection_setup_tm1_cqi_aperiodic_m30_antport_1
( 
 void 
);

extern void lte_rrc_llc_test_rrc_connection_setup
(
  lte_rrc_pdu_buf_s encoded_pdu
);

extern void lte_rrc_llc_test_rrc_connection_setup_for_tm9
(
  lte_rrc_pdu_buf_s encoded_pdu
);

extern void lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi
(
  void
);

extern void lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_dl
(
  void
);

extern void lte_rrc_llc_test_reconfig_setup_srb2_drb3_um_ul
(
  void
);

extern void lte_rrc_llc_test_reconfig_setup_duplicate_drb
(
  void
);

extern void lte_rrc_llc_test_reconfig_same_drb_dl_released_ul_setup
(
  void
);

extern void lte_rrc_llc_test_reconfig_same_drb_released_setup
(
  void
);

extern void lte_rrc_llc_test_setup_drb_um_bi
(
  lte_rb_id_t drb_id,
  lte_lc_id_t lc_id
);

extern void lte_rrc_llc_test_reconfig_release_drb3
(
  lte_rrc_pdu_buf_s encoded_pdu, /*! PDU to be decoded and sent to LLC */
  uint8 rb_direction /*! Direction of the RB being released */
);

extern void lte_rrc_llc_test_reconfig_setup_srb2_drb3_am
(
  void
);

extern void lte_rrc_llc_test_release
(
  lte_rrc_proc_e proc, /*! Source RRC proc */
  boolean release_phy, /*! Flag to indicate if phy release is required */
  uint32 cell_id  /*! Cell id where phy release is done */
);

extern void lte_rrc_llc_test_conn_release
(
  boolean release_phy /*! Flag to indicate if phy release is required */
);

extern void lte_rrc_llc_test_reconfig_srb1_setup_srb2
(
  void
);

extern void lte_rrc_llc_test_reconfig_srb1_srb2_drb3
(
  void
);

extern void lte_rrc_llc_test_reconfig_phy_mac_drb3_rlc
(
  void
);

extern void lte_rrc_llc_test_reconfig_phyded_cfg
(
  boolean phy_only_reconfig   /*! If the reconfig is for PHY only */
);

extern void lte_rrc_llc_test_cqi_fmt_indi
(
  boolean phy_only_reconfig   /*! If the reconfig is for PHY only */
);

extern void lte_rrc_llc_test_reconfig_trch_cfg
(
  boolean mac_only_reconfig   /*! If the reconfig is for MAC-only */
);

extern void lte_rrc_llc_test_mac_release_resources_ind
(
  void
);

extern void lte_rrc_llc_test_reconfig_drb3_setup_drb4_um_bi
(
  void
);

extern void lte_rrc_llc_test_reconfig_drb3_setup_drb4_drb5_um_bi
(
  void
);

extern void lte_rrc_llc_test_rrc_reconfig_srb2_release_drb3_setup_drb4
(
  void
);

extern void lte_rrc_llc_test_intra_freq_mobility_only
(
  lte_rrc_pdu_buf_s encoded_pdu, /*!< Encoded reconfig message with mobility ctrl */
  boolean ho_rach_success /*!< indicates whether RACH after HO is assumed success or not*/
);

extern void lte_rrc_llc_test_mobility_with_scc_only
(
  lte_rrc_pdu_buf_s encoded_pdu, /*!< Encoded reconfig message with mobility ctrl */
  boolean ho_rach_success /*!< indicates whether RACH after HO is assumed success or not*/
);


extern void lte_rrc_llc_test_ho_full_config_with_drb3_release
(
  void
);

extern void lte_rrc_llc_test_ho_delta_config_with_drb3_release
(
  void
);

extern void lte_rrc_llc_test_ho_fdd_to_tdd
(
  lte_rrc_pdu_buf_s encoded_pdu, /*!< Encoded reconfig message with mobility ctrl */
  boolean rel_ver_is_r11
);

extern void lte_rrc_llc_test_ho_fdd_to_tdd_without_ack_nak_fb_mode
(
  lte_rrc_pdu_buf_s encoded_pdu /*!< Encoded reconfig message with mobility ctrl */
);



extern void lte_rrc_llc_test_ho_tdd_to_tdd
(
  lte_rrc_pdu_buf_s encoded_pdu /*!< Encoded reconfig message with mobility ctrl */
);

extern void lte_rrc_llc_test_ho_tdd_to_fdd
(
  lte_rrc_pdu_buf_s encoded_pdu /*!< Encoded reconfig message with mobility ctrl */
);

extern void lte_rrc_llc_test_rlf
(
  void
);

extern void lte_rrc_llc_test_ho_cphy_failure
(
  void
);

extern void lte_rrc_llc_test_ho_with_no_drb_rach_failure
(
  void
);

extern void lte_rrc_llc_test_ho_with_drb_add_rach_failure
(
  void
);

extern void lte_rrc_llc_test_ho_with_drb_modify_rach_failure
(
  void
);

extern void lte_rrc_llc_test_ho_with_drb_rel_rach_failure
(
  void
);
extern void lte_rrc_llc_test_ho_with_drb_add_modify_rel_rach_failure
(
  void
);

extern void lte_rrc_llc_test_ho_validation_failure
(
  lte_rrc_pdu_buf_s encoded_pdu
);

extern void lte_rrc_llc_test_tm9_wo_csi_rs_cfg
(
  lte_rrc_pdu_buf_s encoded_pdu
);

extern void lte_rrc_llc_test_feicic_crs_info
(
  lte_rrc_pdu_buf_s encoded_pdu
);

extern void lte_rrc_llc_test_rohc_reset
(
  lte_rrc_pdu_buf_s encoded_pdu
);

extern void lte_rrc_llc_scell_cqi_bw_test
(
  lte_rrc_pdu_buf_s encoded_pdu
);

extern void lte_rrc_llc_test_rrc_connection_reestablishment
( 
  void 
);

extern void lte_rrc_llc_test_rrc_connection_rel_during_reestablishment
( 
  void 
);


extern void lte_rrc_llc_test_first_reconfig_after_cre
( 
  void
);

extern void lte_rrc_llc_test_stub_init
( 
  void 
);

extern void *lte_rrc_llc_test_basic_get_cell_sib
(
  lte_rrc_sib_bitmask_t sib, /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/
  lte_rrc_sib_db_key_t key /*!< sib db index */
);

extern void *lte_rrc_llc_test_tdd_get_cell_sib
(
  lte_rrc_sib_bitmask_t sib, /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/
  lte_rrc_sib_db_key_t key /*!< sib db index */
);
extern lte_rrc_div_duplex_e lte_rrc_llc_test_get_last_camped_mode_fdd
(
  void
);
extern void *lte_rrc_llc_test_srs_get_serv_cell_sib
(
  lte_rrc_sib_bitmask_t sib, /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/
  lte_rrc_sib_db_key_t key /*!< sib db index */
);

extern void *lte_rrc_llc_test_srs_release_get_cell_sib
(
  lte_rrc_sib_bitmask_t sib, /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/
  lte_rrc_sib_db_key_t key /*!< sib db index */
);

extern stm_state_t lte_rrc_llc_test_controller_get_connected_state
(
  void
);

extern boolean lte_rrc_llc_test_cfg_is_for_tdd_cell_true( lte_earfcn_t freq );

extern void lte_rrc_llc_test_cphy_rf_status_un_available( lte_rrc_rf_status_info_s *info_ptr );

extern void lte_rrc_llc_test_cphy_rf_status_available( lte_rrc_rf_status_info_s *info_ptr );

extern boolean lte_rrc_llc_test_no_dsds_mode( void );

extern boolean lte_rrc_llc_test_dsds_mode( void );

extern boolean lte_rrc_llc_test_cfg_is_for_tdd_cell_false( lte_earfcn_t freq );

extern lte_errno_e lte_rrc_llc_test_common_csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Pointer to cell info */
);

extern lte_errno_e lte_rrc_llc_test_common_csp_tdd_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Pointer to cell info */
);

extern lte_errno_e lte_rrc_llc_test_csp_get_camped_cell_bw
(
  lte_bandwidth_e *bw_ptr /*!< Pointer to BW to be filled in */
);

extern lte_errno_e lte_rrc_llc_test_csp_get_camped_cell_bw1
(
  lte_bandwidth_e *bw_ptr /*!< Pointer to BW to be filled in */
);

extern lte_errno_e lte_rrc_llc_test_csp_get_camped_cell_bw2
(
  lte_bandwidth_e *bw_ptr /*!< Pointer to BW to be filled in */
);

extern uint8 lte_rrc_llc_test_csp_get_num_tx_antennas_simo
(
  void
);

extern uint8 lte_rrc_llc_test_csp_get_num_tx_antennas_mimo
(
  void
);

extern boolean lte_rrc_llc_test_sib_b12_to_b17_conversion_is_needed
(
  void
);

extern boolean lte_rrc_llc_test_sib_b12_to_b17_conversion_is_not_needed
(
  void
);

extern void lte_rrc_llc_test_free_sib_ptr
(
  void
);

extern void lte_rrc_llc_test_free_dl_ded_msg_ptr
(
  void
);

extern lte_errno_e lte_rrc_llc_test_sec_param_prepare_config
(
  uint8 ncc,  /*! Nexthop Chaining Count */
  boolean ncc_is_valid, /*! Is ncc being passed? */
  lte_pdcp_security_cfg_s *sec_config_ptr   /*!Sec Cfg Ptr */
);

extern lte_errno_e lte_rrc_llc_test_sec_dcch_prepare_config
(
  lte_rrc_osys_RRC_TransactionIdentifier trans_id, /*! DL DCCH Message trans_id */
  lte_pdcp_security_cfg_s *sec_config_ptr,   /*!Sec Cfg Ptr */
  /*!Phy cell ID of the target cell*/
  lte_phy_cell_id_t phy_cell_id,
  /*!Downlink EARFCN of the target cell*/
  lte_earfcn_t dl_freq
);

extern void lte_rrc_llc_test_send_stopped_indi
(
  void
);

extern boolean lte_rrc_llc_test_common_mh_dlm_processing_is_active
(
  void
);

extern boolean lte_rrc_llc_test_common_mh_dlm_processing_is_not_active
(
  void
);

extern boolean lte_rrc_llc_test_cap_inter_freq_is_not_supported
(
  void
);

extern lte_3gpp_release_ver_e lte_rrc_llc_test_get_3gpp_release_version_for_rel8
(
  void
);
extern lte_3gpp_release_ver_e lte_rrc_llc_test_get_3gpp_release_version_for_rel11
(
  void
);

extern lte_3gpp_release_ver_e lte_rrc_llc_test_get_3gpp_release_version_for_rel9
(
  void
);

extern void lte_rrc_llc_test_rel9_ho_full_config
(
  void
);

extern void lte_rrc_llc_test_rel9_ho_with_full_config_failure
(
  void
);

extern void lte_rrc_llc_test_rohc_reset_config_failure
(
  void
);

extern void lte_rrc_llc_test_rel9_ho_with_mode_change_full_config_failure
(
  void
);

extern void lte_rrc_llc_test_rel9_reconfig_after_cre_full_config
(
  boolean pdcp_drb_release
);

extern lte_rrc_div_duplex_e lte_rrc_llc_test_get_last_camped_mode_tdd
(
  void
);

extern uint8 lte_rrc_llc_test_get_fgi_bit_0
(
  uint8 bit_pos
);

extern uint8 lte_rrc_llc_test_get_fgi_bit_1
(
  uint8 bit_pos
);

extern uint8 lte_rrc_llc_test_get_fgi_bit_disable_105
( 
   uint8 bit_pos
);
extern uint8 lte_rrc_llc_test_get_fgi_bit_disable_110
( 
   uint8 bit_pos
);
extern uint8 lte_rrc_llc_test_get_fgi_bit_disable_109
( 
   uint8 bit_pos
);
extern uint8 lte_rrc_llc_test_get_fgi_bit_disable_106
( 
   uint8 bit_pos
);

extern uint8 lte_rrc_llc_test_get_fgi_bit_disable_107
( 
   uint8 bit_pos
);


extern uint8 lte_rrc_llc_test_get_fgi_bit_disable_108
( 
   uint8 bit_pos
);

extern void lte_rrc_llc_test_phy_common_cfg
(
  lte_rrc_osys_SystemInformationBlockType2 *sib2_ptr,
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr
);

extern void lte_rrc_llc_test_phy_dedicated_cfg
(
  lte_cphy_dedicated_cfg_req_param_s *cphy_cfg_req_param_ptr,
  boolean is_default_cfg,
  lte_rrc_proc_e source_proc
);

extern void lte_rrc_llc_test_mac_common_cfg
(
  lte_rrc_osys_SystemInformationBlockType2 *sib2_ptr,
  lte_mac_cfg_req_msg_s *mac_req_ptr
);

extern void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_setup
(
  void
);

extern void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mtch_setup
(
  void
);

extern void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_same_pmch
(
  void
);

extern void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mcch_mtch_setup
(
  void
);

extern void lte_rrc_llc_test_scc_cqi_reporting
(
  lte_rrc_pdu_buf_s encoded_pdu
);

extern void lte_rrc_llc_test_invalid_scc_cqi_reporting
(
  lte_rrc_pdu_buf_s encoded_pdu
);


extern void lte_rrc_llc_test_construct_and_send_cfg_reqi_for_mrb_reconfig
(
  void
);

extern void lte_rrc_llc_test_embms_mcch_mtch_config
(
  void
);

#ifdef FEATURE_LTE_EMBMS
extern lte_rrc_osys_MCCH_Message *lte_rrc_llc_test_emp_get_mbsfnareaconfig_invalid
(
  lte_mbsfn_area_id_t mbsfn_area_id
);
#endif
extern void lte_rrc_llc_test_embms_mcch_mtch_release
(
  boolean cfg_for_release,
  boolean release_all_mrb,
  boolean release_all_rb
);

extern void lte_rrc_llc_test_embms_release_for_lte_stop
(
  boolean cfg_for_release,
  boolean release_all_mrb,
  boolean release_all_rb
);

extern boolean lte_rrc_llc_test_ho_to_eutra_in_progress_true
(
  void
);

extern void lte_rrc_llc_test_init_cphy_common_cfg_mask_for_verif
(
  lte_cphy_common_cfg_mask_t cfg_mask
);

extern lte_3gpp_release_ver_e lte_rrc_llc_test_get_3gpp_release_version_for_rel10
(
  void
);

extern boolean lte_rrc_llc_test_get_rohc_enabled
(
  uint8 cap_bit
);

extern boolean lte_rrc_llc_test_get_ho_in_progress
(
  void
);


extern boolean lte_rrc_llc_test_get_r11_cap_bit_ssf9_enabled
(
  uint8 cap_bit
);

extern boolean lte_rrc_llc_test_get_band_combo_bitmask_FF
(
  sys_sband_lte_e_type camp_band,
  uint8 num_cc, /* NUmber of Configured cells */
  const sys_sband_lte_e_type *cell_band, /* Configured cell bands */
  const lte_bandwidth_e *cell_bw, /* Configured cell bandwith */
  const uint8 *cell_bw_class, /* Configured cell bandwith class*/
  boolean disable_bw_validation /* Disable all BW related validations */
);

#ifdef FEATURE_LTE_CA
extern boolean lte_rrc_llc_test_cap_is_ca_bandwidth_combo_valid_true
(
  sys_sband_lte_e_type pcell_band, 
  sys_sband_lte_e_type scell_band,
  lte_bandwidth_e pcell_dl_bandwidth, /*Pcell bandwidth */
  lte_bandwidth_e scell_dl_bandwidth, /*Scell bandwidth */
  lte_rrc_osys_CA_BandwidthClass_r10_Root bw_calss
);
#endif

extern void lte_rrc_llc_test_ho_with_mode_change_from_am_to_ul_only_full_config_failure
(
  void
);

extern void lte_rrc_llc_test_ho_with_mode_change_from_am_to_dl_only_full_config_failure
(
  void
);

extern void lte_rrc_llc_test_ho_with_mode_change_from_ul_only_to_am_full_config_failure
(
  void
);

extern void lte_rrc_llc_test_ho_with_mode_change_from_dl_only_to_am_full_config_failure
(
  void
);

extern boolean lte_rrc_llc_test_stub__ml1_validate_tdd_cfg_mbsfn_sf_alloc
(
  lte_cphy_tdd_cfg_param_s *tdd_cfg, 
  lte_cphy_common_cfg_req_param_s *cfg_req
);

#endif /* LTE_RRC_LLC_TEST_TEST_COMMON_H */
