#ifndef GERAN_NV_H
#define GERAN_NV_H

/*! \file geran_nv.h
 
  This is the header file for geran_nv.c.
  This module contains access functions for NV data.
 
                Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/gcommon/inc/geran_nv.h#11 $ */
/* $DateTime: 2023/09/29 01:38:57 $$Author: pwbldsvc $ */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   -------- ---------------------------------------------
15/09/20   sal      CR2777109 FSK feature control using NV74417.
14/02/17   mk       CR2004610 : FR 40251 (SeNS) - Add NV item to enable SeNS feature
21/04/17   br       CR1041084 FR 36219: GDET � GMSK Modulation Detector
14/11/18   sk       CR2341754 Geran Memopt changes
04/10/18   kh		Geran compiler errors new features
11/09/15   dmb      Updates for EFS usage per subscription
dd/mm/yy   who      Sample text for edit history
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "sys.h"
#include "geran_dual_sim.h"
#include "mcfg_fs.h"

#include "geran_variation.h"

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#include "geran_dual_sim.h"
#include "rr_nv_g.h"
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
#include "string.h"
#include <stringl.h>
#include "fs_public.h"
#include "msg.h"
#include "gprs_mem.h"
#include "geran_multi_sim.h"
#include "mmgsdilib.h"
#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
#include "sys_cnst.h"
#endif /* FEATURE_GERAN_R13_TEST_FRAMEWORK */
#include "mcfg_refresh.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define GERAN_TA_SUPPORTED 1
#define GERAN_TA_NOT_SUPPORTED 0
#define GERAN_NV_MEM_NOT_VALID (-1)

/* GERAN macros for calling MCFG APIs */
#define GERAN_EFS_OPEN(path,oflag,mode,as_id) mcfg_fopen(path, oflag, mode, MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)as_id)
#define GERAN_EFS_MKDIR(path,mode) mcfg_mkdir(path, mode, MCFG_FS_TYPE_EFS)
#define GERAN_EFS_WRITE(filedes,buf,size) mcfg_fwrite(filedes, buf, size, MCFG_FS_TYPE_EFS)
#define GERAN_EFS_CLOSE(filedes) mcfg_fclose(filedes,MCFG_FS_TYPE_EFS)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct
{
  boolean valid;
#ifdef FEATURE_DUAL_SIM
  uint8 default_rf_chain;
  uint8 g2x_ta_support;
#endif // FEATURE_DUAL_SIM
#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
  uint8 si9_content_type;
  uint8 si21_content_type;
#endif /* FEATURE_GERAN_R13_TEST_FRAMEWORK */
} geran_nv_data_t;

extern geran_nv_data_t geran_nv_data;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
typedef struct
{
  /* order registerd with MCFG */
  mcfg_refresh_order_e_type    reg_order;
  /* slots registerd with MCFG to receive refresh update */
  mcfg_refresh_index_mask_type reg_slot_mask;
  /* subs registerd with MCFG to receive refresh update */
  mcfg_refresh_index_mask_type reg_sub_mask;
  /* groups registerd with MCFG to receive refresh update */
  mcfg_refresh_group_type      reg_group;
  /* Type of refresh (slot/sub) registered with MCFG */
  mcfg_refresh_type_e_type     reg_type;
  /* Reference ID received from MCFG in call back function */
  uint32                       ref_ids[MCFG_REFRESH_INDEX_MAX + 1];
} geran_nv_refresh_data_t;

#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*----------------------------------------------------------------------------
 * EFS Item paths
 * -------------------------------------------------------------------------*/
#define GERAN_EFS_ANT_SWTCHNG_TX_DIV_EN   "/nv/item_files/gsm/gl1/antenna_switching_tx_diversity_enable"
#define GERAN_EFS_ANT_SWTCHNG_TX_DIV_TH   "/nv/item_files/gsm/gl1/antenna_switching_tx_diversity_threshold"
#define GERAN_EFS_ANT_SWTCH_TX_DIV_MIN_DELTA_BTWN_ANT "/nv/item_files/gsm/gl1/antenna_switch_tx_div_min_delta_btw_antenna"
#define GERAN_EFS_WCDMA_L1_QTA_ROTA "/nv/item_files/wcdma/l1/wl1_qta_rota"
#define GERAN_EFS_GBTA_SUPPORT "/nv/item_files/modem/geran/gbta_support"
#define GERAN_EFS_LTE_CELL_RSRP_TH "/nv/item_files/modem/geran/lte_cell_rsrp_threshold"
#define GERAN_EFS_COMM_RECOVERY_RESTART "/nv/item_files/gsm/gl1/gsm_commercial_recovery_restart"
#ifdef FEATURE_GSM_SEARCH_ALL_WCELL
#define GERAN_EFS_SRCH_ALL_W_CELL "/nv/item_files/gsm/gl1/search_all_w_cell"
#endif
#define GERAN_EFS_DEF_RF_CHAIN "/nv/item_files/modem/geran/default_rf_chain"
#define GERAN_EFS_G2X_TA_SUPPORT "/nv/item_files/modem/geran/g2x_ta_support"

#define GERAN_EFS_ANT_SWTCHNG_GSM_T2_IDL_SUSP_TIME "/nv/item_files/gsm/gl1/antenna_switching_gsm_type_2_idle_suspension_time"
#define GERAN_EFS_ANT_SWTCHNG_GSM_T2_TRFC_CRISIS_MODE_THRESH "/nv/item_files/gsm/gl1/antenna_switching_gsm_type_2_traffic_crisis_mode_thre"
#define GERAN_EFS_ANT_SWTCHNG_GSM_T2_IDL_PINGPONG_MIT_THRESH "/nv/item_files/gsm/gl1/antenna_switching_gsm_type_2_idle_pingpong_mitigation_thre"

#define GERAN_EFS_ANT_SWTCHNG_GSM_T2_EN "/nv/item_files/gsm/gl1/antenna_switching_gsm_type_2_en"
#define GERAN_EFS_ANT_SWTCHNG_GSM_TRAFI_MDM_TH "/nv/item_files/gsm/gl1/antenna_switching_gsm_trafi_mdm_threshold"
#define GERAN_EFS_ANT_SWTCHNG_GSM_IDLE_MDM_TH "/nv/item_files/gsm/gl1/antenna_switching_gsm_idle_mdm_threshold"
#define GERAN_EFS_ANT_SWTCHNG_GSM_TRAFI_SENS "/nv/item_files/gsm/gl1/antenna_switching_gsm_trafi_sens"
#define GERAN_EFS_ANT_SWTCHNG_GSM_IDLE_SENS "/nv/item_files/gsm/gl1/antenna_switching_gsm_idle_sens"
#define GERAN_EFS_ANT_SWTCHNG_GSM_QSC_LARGE_DELTA "/nv/item_files/gsm/gl1/antenna_switching_gsm_qsc_large_delta"
#define GERAN_EFS_ANT_SWTCHNG_GSM_QSC_SMALL_DELTA "/nv/item_files/gsm/gl1/antenna_switching_gsm_qsc_small_delta"
#define GERAN_EFS_ANT_SWTCHNG_GSM_QSC_HYST_TIME "/nv/item_files/gsm/gl1/antenna_switching_gsm_qsc_hysteriesis_time"

#define GL1_EFS_MDSP_DEBUG "/nv/item_files/modem/geran/gfw_debug"
#define GL1_EFS_MDSP_DIAG1 "/nv/item_files/modem/geran/gfw_diag_group1"
#define GL1_EFS_MDSP_DIAG2 "/nv/item_files/modem/geran/gfw_diag_group2"
#define GL1_EFS_MDSP_DIAG3 "/nv/item_files/modem/geran/gfw_diag_group3"
#define GL1_EFS_MDSP_DIAG4 "/nv/item_files/modem/geran/gfw_diag_group4"
#define GL1_EFS_MDSP_AEQPH4_CTL "/nv/item_files/modem/geran/gfw_aeq_phase4_control"

#define GL1_EFS_DEBUG "/nv/item_files/gsm/l1/l1_debug"
#define GL1_EFS_DEBUG_LIF_SUPPORT "/nv/item_files/gsm/gl1/lif_feature_control"
#define GL1_EFS_SLP_DLY_SETTINGS_PATH "/nv/item_files/gsm/gl1/l1_slp_dly"
#define GL1_EFS_RxD "/nv/item_files/gsm/gl1/gsm_rx_diversity" 

#define GL1_GDET_SUPPORT "/nv/item_files/modem/geran/gdet_support"
#define GL1_GDET_THRESHOLD "/nv/item_files/modem/geran/gdet_threshold"
#define GL1_EFS_FSK_FEATURE_CTL "/nv/item_files/modem/geran/fsk_feature_control"


#define GL1_EFS_SENS_FR_CONTROL   "/nv/item_files/gsm/gl1/gl1_sens_fr_control"

/***********************************/
/* Hardware-Specific EFS items used by GRR */
/**********************************/
#define GERAN_EFS_G2X_MEASUREMENT_CONTROL "/nv/item_files/modem/geran/grr/g2x_measurement_control"
#define GERAN_EFS_TRRC_SPECIAL_TEST_SETTING_ENABLED "/nv/item_files/modem/tdscdma/rrc/special_test_setting_enabled"
#define GERAN_EFS_RR_ARFCN_LIST "/nv/item_files/modem/geran/rr_efs_arfcn_list"
#define GERAN_EFS_PSCAN_RESULTS_REUSE_TIME_SECS "/nv/item_files/modem/geran/pscan_results_reuse_time_secs"
#define GERAN_EFS_ADD_CRH_DB "/nv/item_files/modem/geran/add_crh_db"
#define GERAN_EFS_ADD_CRH_SCELL_THRESH_DB "/nv/item_files/modem/geran/add_crh_scell_thresh_db"
#define GERAN_EFS_SYS_INFO_CACHE_ENABLED "/nv/item_files/modem/geran/grr/sys_info_cache_enabled"
#define GERAN_EFS_BAND_AVOID_ENABLED "/nv/item_files/modem/geran/band_avoid_enabled"
#define GERAN_EFS_BAND_AVOID_NCELL_THRESHOLD "/nv/item_files/modem/geran/band_avoid_ncell_threshold"
#define GERAN_EFS_BAND_AVOID_MAX_MEAS_RPTS "/nv/item_files/modem/geran/band_avoid_max_meas_rpts"
#define GERAN_EFS_ITERATIVE_SI_ACQ_ENABLED "/nv/item_files/modem/geran/grr/iterative_si_acq_enabled"
#define GERAN_EFS_SCELL_RESYNC_TIME_SECS "/nv/item_files/modem/geran/scell_resync_time_secs"
#define GERAN_EFS_LRRC_BPLMN_CONTROL "/nv/item_files/modem/lte/rrc/bplmn/bplmn_control"
#define GERAN_EFS_CSFB_FCCH_SNR_OFFSET "/nv/item_files/modem/geran/grr/csfb_fcch_snr_offset"
#define GERAN_EFS_CSFB_RSSI_OFFSET "/nv/item_files/modem/geran/grr/csfb_rssi_offset"
#define GERAN_EFS_ENG_MODE_DEBUG_ENABLED  "/nv/item_files/modem/geran/eng_mode_debug_enabled"
#define GERAN_EFS_VAMOS_SUPPORT "/nv/item_files/modem/geran/vamos_support"
#define GERAN_EFS_LAST_CAMPED_CELL_ENABLED "/nv/item_files/modem/geran/grr/last_camped_cell_enabled"
#define GERAN_EFS_BCCH_DECODES_PER_BAND_ENABLED "/nv/item_files/modem/geran/grr/bcch_decodes_per_band_enabled"
#define GERAN_EFS_IDLE_DRX_SCALING_SUSPEND_TIME_SECS "/nv/item_files/modem/geran/idle_drx_scaling_suspend_time_secs"
#define GERAN_EFS_PEO_ENABLED                  "/nv/item_files/modem/geran/peo_enabled"
#define GERAN_EFS_PEO_DSC                 "/nv/item_files/modem/geran/peo_dsc"
#define GERAN_EFS_PEO_RCC                 "/nv/item_files/modem/geran/peo_rcc"
#define GERAN_EFS_PEO_C1_DELTA_MIN        "/nv/item_files/modem/geran/peo_c1_delta_min"
#define GERAN_EFS_PEO_C1_DELTA_MAX        "/nv/item_files/modem/geran/peo_c1_delta_max"
#define GERAN_EFS_PEO_BCCH_CHANGE_MARK    "/nv/item_files/modem/geran/peo_bcch_change_mark"
#define GERAN_EFS_IMPLICIT_REJECT          "/nv/item_files/modem/geran/implicit_reject"
#define GERAN_EFS_ALLOW_PEO_RACH          "/nv/item_files/modem/geran/allow_peo_rach"
#define GERAN_EFS_IR_EAB_ENABLED          "/nv/item_files/modem/geran/ir_eab_enabled"
#define GERAN_EFS_EAB_ENABLED             "/nv/item_files/modem/geran/eab_enabled"



/*************************************/
/* Subscription-Specific EFS items used by GRR */
/************************************/
#define GERAN_EFS_FAST_SI_ACQ_DURING_CSFB_CONTROL "/nv/item_files/modem/geran/grr/fast_si_acq_during_csfb_control"
#define GERAN_EFS_FAST_SI_ACQ_WHEN_CS_ONLY_ENABLED "/nv/item_files/modem/geran/grr/fast_si_acq_when_cs_only_enabled"
#define GERAN_EFS_NEGATIVE_C1_WHITE_LIST "/nv/item_files/modem/geran/negative_c1_white_list"
#define GERAN_EFS_PLMN_SEARCH_FREQ_PRUNING_ENABLED "/nv/item_files/modem/geran/plmn_search_freq_pruning_enabled"
#define GERAN_EFS_ENABLE_SI3_ONLY "/nv/item_files/modem/geran/enable_si3_only"
#define GERAN_EFS_G2L_BLIND_REDIR_CONTROL "/nv/item_files/modem/geran/grr/g2l_blind_redir_control"
#define GERAN_EFS_G2L_BLIND_REDIR_AFTER_CSFB_CONTROL "/nv/item_files/modem/geran/grr/g2l_blind_redir_after_csfb_control"
#define GERAN_EFS_G2L_BLIND_REDIR_AFTER_SRVCC_CONTROL "/nv/item_files/modem/geran/grr/g2l_blind_redir_after_srvcc_control"
#define GERAN_EFS_FAST_G2L_USING_PSEUDO_SI2Q_ENABLED "/nv/item_files/modem/geran/fast_g2l_using_pseudo_si2q_enabled"
#define GERAN_EFS_SGLTE_G2X_CONN_MODE_RESEL_ENABLED "/nv/item_files/modem/geran/sglte_g2x_conn_mode_resel_enabled"
#define GERAN_EFS_X2G_FAST_SI_ACQ_WHITE_LIST "/nv/item_files/modem/geran/x2g_fast_si_acq_white_list"
#define GERAN_EFS_READ_PCH_DURING_TRANSFER_WHITELIST "/nv/item_files/modem/geran/read_pch_during_transfer_whitelist"
#define GERAN_EFS_LAST_CAMPED_CELL  "/nv/item_files/modem/geran/grr/last_camped_cell"
#define GERAN_EFS_PS_PREFERRED_ENABLED "/nv/item_files/modem/geran/ps_preferred_cell_enabled"
#define GERAN_EFS_NUM_PS_SUPPORTED_ARFCN_THRES "/nv/item_files/modem/geran/num_ps_supported_arfcn_thres"
#define GERAN_EFS_ACQ_DB "/nv/item_files/modem/geran/grr/acq_db"

/*************************************/
/* Subscription-Specific EFS items used by GDATA */
/************************************/
#define GERAN_EFS_LLC_PSM_INFO "/nv/item_files/modem/geran/llc_psm_info"
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Wrapper API for mcfg_fs_read for reading EFS Items for primary subscription
 * - consistent with legacy efs_get API usage 
 * 
 * \return length of data read if successful else -1
 */
extern int geran_efs_read_primary(const char *path, void *data, uint32 length);

/*!
 * \brief Wrapper API for mcfg_fs_read for reading EFS Items per subscription
 * 
 * \return length of data read if successful else -1
 */
extern int geran_efs_read_per_sub(const char *path, void *data, uint32 length);

/*!
 * \brief Wrapper API for mcfg_fs_stat for getting the EFS item stats per subscription
 * 
 * \return boolean - TRUE if successful, FALSE otherwise
 */
extern boolean geran_efs_stat_per_sub(const char *path, struct fs_stat *data);

/*!
 * \brief Wrapper API for mcfg_fs_write for writing EFS Items per subscription
 * 
 * \return length of data wrote if successful else -1
 */
extern int geran_efs_write_per_sub(const char *path, void *data, uint32 length);

/*!
 * \brief Read general GERAN EFS-NV items.
 */
extern void geran_read_efs_nv_items(void);

#ifdef FEATURE_DUAL_SIM
/*!
 * \brief Returns the value of EFS-NV item default_rf_chain.
 * 
 * \return int - {0,1} or -1 if not defined
 */
extern int geran_nv_get_default_rf_chain(void);
#endif /* FEATURE_DUAL_SIM */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Returns the GPRS->WCDMA tuneaway support based on the values of EFS-NV items.
 * 
 * \return int - {GERAN_TA_SUPPORTED, GERAN_TA_NOT_SUPPORTED, GERAN_NV_MEM_NOT_VALID} 
 */
extern int geran_nv_g2w_ta_supported(void);

/*!
 * \brief Returns the GPRS->TDSCDMA tuneaway support based on the values of EFS-NV items.
 * 
 * \return int - {GERAN_TA_SUPPORTED, GERAN_TA_NOT_SUPPORTED, GERAN_NV_MEM_NOT_VALID} 
 */
extern int geran_nv_g2t_ta_supported(void);

/*!
 * \brief Returns the GPRS->CDMA tuneaway support based on the values of EFS-NV items.
 * 
 * \return int - {GERAN_TA_SUPPORTED, GERAN_TA_NOT_SUPPORTED, GERAN_NV_MEM_NOT_VALID} 
 */
extern int geran_nv_g21x_ta_supported(void);

/*!
 * \brief Returns the GPRS->LTE tuneaway support based on the values of EFS-NV items.
 * 
 * \return int - {GERAN_TA_SUPPORTED, GERAN_TA_NOT_SUPPORTED, GERAN_NV_MEM_NOT_VALID} 
 */
extern int geran_nv_g2l_ta_supported(void);

#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Module initialisation function. Called during RR task initialisation.
 */
extern void geran_nv_init(void);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
extern boolean geran_nv_refresh_cb(mcfg_refresh_info_s_type *p_info);

extern geran_nv_refresh_data_t *geran_nv_refresh_get_data_ptr(void);
#endif

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
extern void geran_nv_get_SI9_content(uint8* l3_msg_ptr);
extern void geran_nv_update_SI9_content_type(uint8 content_type);
extern void geran_nv_get_SI21_content(uint8* l3_msg_ptr);
extern void geran_nv_update_SI21_content_type(uint8 content_type);
#endif /* FEATURE_GERAN_R13_TEST_FRAMEWORK */


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*!
 * \brief function for informing MCFG about refresh done. Called from RR task.
 */
extern void geran_nv_refresh_done(const boolean done);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

extern boolean geran_ciphering_logging_enabled(void );

#endif /* GERAN_NV_H */

/* EOF */

