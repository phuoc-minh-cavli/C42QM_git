#ifndef RR_NV_H
#define RR_NV_H

/*! \file rr_nv.h
 
  This is the header file for rr_nv.c.
  This module contains access functions for NV data.
 
                Copyright (c) 2011-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_nv.h#5 $ */
/* $DateTime: 2023/09/29 01:38:57 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "mm_rr.h"
#include "rr_defs.h"
#include "sys.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  /* GERAN Information */
  uint8 geran_priority;
  uint8 thresh_priority_search;
  uint8 thresh_gsm_low;
  uint8 h_prio;
  uint8 t_reselection;
} rr_geran_def_data_t;

typedef struct
{
  /* UTRAN information */
  uint8 priority;
  uint8 thresh_high;
  uint8 thresh_low;
  uint8 qrxlevmin;

} rr_utran_def_data_t;

typedef struct
{    
  /* LTE Information */
  uint8 priority;
  uint8 meas_bandwidth;
  uint8 thresh_high;
  uint8 thresh_low;
  uint8 qrxlevmin;  

} rr_lte_def_data_t;
    
typedef struct
{
  boolean pseudo_si2q_def_data_valid;
  rr_geran_def_data_t rr_geran_def_data;
  rr_utran_def_data_t rr_utran_def_data;
  rr_lte_def_data_t  rr_lte_def_data;
  
} rr_efs_pseudo_si2q_def_data_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns the value of SPLIT_PAGE_CYCLE stored in NV.
 * 
 * 
 * \return uint16 
 */
extern uint16 rr_get_nv_split_page_cycle(void);

/*!
 * \brief Returns the value of NON_DRX_TIMER.
 * 
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_non_drx_timer(void);

/*!
 * \brief Returns the value of MULTISLOT_CLASS stored in NV.
 * 
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_multislot_class(void);

/*!
 * \brief Returns the value of ANITE_GCF_FLAG stored in NV. 
 * 
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
extern boolean rr_get_nv_anite_gcf_flag(void);

/*!
 * \brief Indicates if EGPRS is enabled in NV.
 * 
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
extern boolean rr_get_nv_egprs_enabled(void);

/*!
 * \brief Returns the value of EGPRS_MULTISLOT_CLASS stored in NV.
 * 
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_egprs_multislot_class(void);

/*!
 * \brief Returns TRUE if the NV parameter EGPRS_8PSK_POWER_CLASS is non-zero; FALSE if zero.
 * 
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
extern boolean rr_get_nv_egprs_8psk_ul_enabled(void);

/*!
 * \brief Returns the EGPRS 8PSK power capability.
 * 
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_egprs_8psk_power_capability();

/*!
 * \brief Indicates if GERAN Feature Pack 1 is enabled in NV
 * 
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
extern boolean rr_get_nv_geran_feature_pack_1_enabled(void);

#ifdef FEATURE_WCDMA
/*!
 * \brief Returns the current mask from NV that indicates support for inter-RAT NACC.
 * 
 * 
 * \return uint8 - 0x01 indicates W2G, 0x02 indicates G2W, 0x03 indicates both
 */
extern uint8 rr_get_nv_interrat_nacc_support(void);
#endif

#ifdef FEATURE_GSM_DTM

/*!
 * \brief Indicates if DTM is enabled in NV.
 * 
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
extern boolean rr_get_nv_dtm_enabled(void);

/*!
 * \brief Returns the value of DTM_EGPRS_MULTISLOT_SUBCLASS stored in NV.
 * 
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_dtm_egprs_multislot_subclass(void);

/*!
 * \brief Returns the value of DTM_MULTISLOT_CLASS, derived from DTM_EGPRS_MULTISLOT_SUBCLASS.
 * 
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_dtm_multislot_class(void);

#ifdef FEATURE_GSM_EDTM
/*!
 * \brief Indicates if EDTM is enabled in NV.
 * 
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
extern boolean rr_get_nv_edtm_enabled(void);
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

/*!
 * \brief Returns the value of GEA_SUPPORTED stored in NV.
 * 
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_gea_supported(void);

#ifdef FEATURE_GSM_GPRS_MSC33

/*!
 * \brief Returns the value of HMSC_GPRS_CODED_MS_CLASS stored in NV.
 * 
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_hmsc_gprs_coded_ms_class(void);

/*!
 * \brief Returns the value of HMSC_EGPRS_CODED_MS_CLASS stored in NV.
 * 
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_hmsc_egprs_coded_ms_class(void);

#endif /* FEATURE_GSM_GPRS_MCS33 */

#ifdef FEATURE_VAMOS
/*!
 * \brief Indicates if VAMOS is enabled in NV.
 * 
 * 
 * \return boolean - TRUE is VAMOS is enabled in NV (VAMOS-I or VAMOS-II); FALSE otherwise
 */
extern boolean rr_get_nv_vamos_enabled(void);
#endif /* FEATURE_VAMOS */

#ifdef FEATURE_LTE
/*!
 * \brief Indicates if Fast SI Acq during CSFB is enabled in NV.
 * 
 * 
 * \return TRUE if Fast SI Acq during CSFB is enabled, FALSE otherwise.
 */
extern boolean rr_get_nv_fast_si_acq_during_csfb_enabled(void);
extern boolean rr_nv_x2g_fast_si_acq_feature_is_enabled(void);
extern boolean rr_nv_x2g_fast_si_acq_is_plmn_match(sys_plmn_id_s_type plmn);
#endif

/*!
 * \brief Inidicates if Fast SI Acq when CS-only is enabled in NV.
 * 
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_fast_si_acq_when_cs_only_is_enabled(void);

#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
/*!
 * \brief Indicates if the supplied PLMN is Read PCH during transfer is enabled in NV.
 * 
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_read_pch_during_transfer_is_plmn_match(sys_plmn_id_s_type plmn);

#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */
/*!
 * \brief Indicates if the supplied PLMN is present in the white-list for C1 relaxation info.
 * 
 * \return boolean - TRUE if present, FALSE otherwise
 */
extern boolean rr_nv_c1_relaxation_is_plmn_match(sys_plmn_id_s_type plmn);

/*!
 * \brief Returns the value of the power-scan re-use timer (0=disabled)
 * 
 * \return rex_timer_cnt_type - timer value in milliseconds
 */
extern rex_timer_cnt_type rr_nv_get_pscan_results_reuse_time_millisecs(void);

/*!
 * \brief Returns the value of the plmn_search_freq_pruning_enabled.
 * 
 * \return TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_get_plmn_search_freq_pruning_enabled(void);

/*!
 * \brief Returns the value of the enable_si3_only.
 * 
 * \return TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_get_enable_si3_only(void);

#ifdef FEATURE_SGLTE
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
/*!
 * \brief Indicates if SGLTE Connected Mode Reselection feature is enabled in NV.
 * 
 * \return boolean - TRUE if SGLTE SGLTE Connected Mode Reselection is enabled, FALSE otherwise
 */
extern boolean rr_get_nv_sglte_g2x_conn_mode_resel_enabled(void);
#endif /* FEATURE_SGLTE_G2X_CONN_MODE_RESEL */
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
/**
  @brief Returns the Fast G2L Using Pseudo SI2Q Enabled NV value

  @return boolean
*/
extern boolean rr_is_efs_nv_fast_g2l_using_pseudo_si2q_enabled(void);

extern boolean rr_is_efs_nv_fast_g2l_using_pseudo_si2q_data_valid(void);

extern rr_efs_pseudo_si2q_def_data_t* rr_get_efs_nv_fast_g2l_pseudo_si2q_data(void);

#endif /*FEATURE_FAST_G2L_PSEUDO_SI2Q*/


/*!
 * \brief Returns the additional C2 hysteresis values.
 * 
 * \return TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_get_add_crh_enabled(
  int8 *add_crh_ptr,
  int8 *add_crh_scell_thresh_db_ptr
);

/*!
 * \brief Indicates if the sys info cache functionality is enabled.
 * 
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_sys_info_cache_is_enabled();

/*!
 * \brief Indicates if the iterative updating during SI acq feature is enabled.
 *        Note: Feature is not disabled for a Type Approval SIM.
 * 
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_iterative_si_acq_is_enabled(void);

#ifdef FEATURE_GSM_BAND_AVOIDANCE

extern boolean rr_is_band_avoidance_enabled(void);

/**
  @brief Returns the threshold for band avoidance ncells as 
         rxlev (0.63)
*/
extern uint8 rr_get_band_avoid_ncell_threshold(void);

/**
  @brief Returns the max number of meas repts that can be sent 
  when doing band avoidance. 
*/
extern uint8 rr_get_band_avoid_max_meas_rpts(void);

#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

#ifdef FEATURE_LTE
#ifdef FEATURE_GERAN_CS_SUPPORT
extern boolean rr_nv_get_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(void);
extern boolean rr_nv_get_g2l_blind_redir_after_csfb_enabled(void);
extern boolean rr_nv_get_g2l_blind_redir_after_srvcc_enabled(void);
#endif /* FEATURE_GERAN_CS_SUPPORT */

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB

/**
  @brief get the csfb fcch snr offset for csfb cell selection

  @return the value of csfb_fcch_snr_offset
*/ 
extern uint16 rr_nv_get_csfb_fcch_snr_offset(void);

/**
  @brief get the csfb rssi offset for csfb cell selection

  @return the value of csfb_rssi_offset
*/ 
extern uint16 rr_nv_get_csfb_rssi_offset(void);
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
#endif /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
extern boolean rr_nv_get_g2w_measurements_enabled(void);
#endif

#ifdef FEATURE_GSM_TDS
extern boolean rr_nv_get_g2t_measurements_enabled(void);
#endif

#if defined(FEATURE_LTE) || defined(FEATURE_SGLTE)
extern boolean rr_nv_get_g2l_measurements_enabled(void);
#endif

#ifdef FEATURE_IDLE_DRX_SCALING
extern uint32 rr_nv_get_idle_drx_scaling_suspend_time_millisecs(void);
#endif /* FEATURE_IDLE_DRX_SCALING */

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 
extern boolean rr_nv_get_peo_enabled(void);
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK  

/*!
 * \brief Indicates if PEO Test params in EFS  is present or not.
 *        
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */

extern boolean  rr_nv_is_peo_testing_params_present(void);

/*!
 * \brief Indicates if PEO Test  DSC params from  EFS .
 *        
 * 
 * \return PEO DSC
 */

extern uint8 rr_nv_get_peo_dsc_from_efs(void);

/*!
 * \brief Indicates if PEO Test  RCC params from  EFS .
 *        
 * 
 * \return uint8 - PEO RCC
 */

extern uint8 rr_nv_get_peo_rcc_from_efs(void);

/*!
 * \brief Indicates if PEO Test  C1 DELTA MIN params from  EFS .
 *        
 * 
 * \return uint8 - PEO C1 DELTA MIN
 */

extern uint8 rr_nv_get_peo_c1_delta_min_from_efs(void);

/*!
 * \brief Indicates if PEO Test  C1 DELTA MAX params from  EFS .
 *        
 * 
 * \return uint8 - PEO C1 DELTA MAX
 */

extern uint8 rr_nv_get_peo_c1_delta_max_from_efs(void);
/*!
 * \brief Indicates if PEO Test  BCCH CHANGE MARK params from  EFS .
 *        
 * 
 * \return uint8 - PEO BCCH CHANGE MARK
 */

extern uint8 rr_nv_get_peo_bcch_change_mark_from_efs(void);

/*!
 * \brief Indicates if  IR and EAB testing param si present from EFS.
 *        
 * 
 * \return boolean - TRUE or FALSE
 */

extern boolean rr_nv_is_ir_eab_testing_params_present(void);

/*!
 * \brief Indicates if  Implicit Reject  params from  EFS .
 *        
 * 
 * \return uint8 - Implicit Reject
 */

extern uint8 rr_nv_get_implicit_reject_from_efs(void);

/*!
 * \brief Indicates if  Implicit Reject  during RACH params from  EFS .
 *        
 * 
 * \return uint8 - Implicit Reject during RACH
 */

extern uint8 rr_nv_get_implicit_reject_during_rach_from_efs(void);

/*!
 * \brief Indicates if  PEO rach is allowed from  EFS .
 *        
 * 
 * \return boolean  - allow_peo_rach
 */

extern boolean rr_nv_get_allow_peo_rach_from_efs(void);

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - peo_params present
 */

extern void rr_nv_update_peo_params_present_flag(boolean peo_test_param_present);

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - peo_dsc
 */

extern void rr_nv_update_peo_dsc_in_efs(uint8 peo_dsc);

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - rcc
 */

extern void rr_nv_update_peo_rcc_in_efs(uint8 rcc);

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - c1_delta_min
 */

extern void rr_nv_update_peo_c1_delta_min_in_efs(uint8 c1_delta_min);

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - c1_delta_max
 */

extern void rr_nv_update_peo_c1_delta_max_in_efs(uint8 c1_delta_max);

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - bcch_change_mark
 */

extern void rr_nv_update_peo_bcch_change_mark_in_efs(uint8 bcch_change_mark);

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - Implicit reject
 */

extern void rr_nv_update_implict_reject_in_efs(uint8 implicit_reject);

/*!
 * \brief Called when QXDM command is received to update the params.
 *  
 * \param msg_ptr (in) - Implicit reject during rach
 */

extern void rr_nv_update_implict_reject_during_rach_in_efs(uint8 implicit_reject_during_rach);

extern void rr_nv_update_allow_peo_rach_in_efs(boolean allow_peo_rach );

extern boolean rr_nv_get_SI21_indicator(void);
extern void rr_nv_update_SI21_indicator(boolean si21_indicator);
extern uint8 rr_nv_get_SI21_position(void);
extern void rr_nv_update_SI21_position(uint8 si21_position);
extern boolean rr_nv_get_SI9_where_indicator(void);
extern void rr_nv_update_SI9_where_indicator(boolean SI9_where_indicator);
extern uint8 rr_nv_get_SI9_where(void);
extern void rr_nv_update_SI9_where(uint8 si9_where);
extern boolean rr_nv_get_EAB_config(void);
extern void rr_nv_update_EAB_config(boolean eab_config);
extern boolean rr_nv_get_special_access_class(void);
extern void rr_nv_update_special_access_class(boolean special_access_class);
#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/

#ifdef FEATURE_GERAN_EAB
extern boolean rr_nv_get_eab_enabled();
#endif /* FEATURE_GERAN_EAB */


/*!
 * \brief Called when the NV data is received from NAS (RR_GMM_GPRS_NV_PARAMS_IND).
 * 
 * \param nv_params_ind_ptr 
 */
extern void rr_nv_process_nv_params_ind(rr_gmm_gprs_nv_params_ind_t *nv_params_ind_ptr);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*!
 * \brief Called when the NV refresh ind received from MCFG.
 *  
 * Re-reads carrier specific EFS/NV items. 
 * 
 */
extern void rr_nv_process_nv_refresh_ind( );
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#ifdef FEATURE_PS_PREFERRED_CELL
extern boolean rr_nv_ps_preferred_enabled();
extern uint8 rr_nv_get_num_ps_supported_arfcn_thres();
#endif /* FEATURE_PS_PREFERRED_CELL */

#endif /* #ifndef RR_NV_H */

/* EOF */

