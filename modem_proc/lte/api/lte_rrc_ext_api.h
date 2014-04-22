/*!
  @file
  lte_rrc_ext_api.h

  @brief
  This file describes the API interfaces to LTE Radio resource Controller (RRC)
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

$Header: //components/rel/lte.mpss/4.7.2/api/lte_rrc_ext_api.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
02/28/12   kp        Moved definition of lte_rrc_get_3gpp_release_version to lte_rrc_ext_api.h
04/21/11   sk        Added prototype for lte_rrc_get_tdd_prune_required
04/20/11   np        lte_rrc_lte_tdd_is_supported & lte_rrc_lte_fdd_is_supported
08/05/10   np        Interface for Transfer of dedicated priorities during OOS
04/30/10   np        API to check if a DL EARFCN is supported based on Band Pref
03/19/10   sureshs   Added cell ID and frequency to API
03/18/10   sureshs   Created

===========================================================================*/

#ifndef LTE_RRC_EXT_API_H
#define LTE_RRC_EXT_API_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte_rrc_ext_msg.h>
#include "nv_items.h"
#include "mcfg_fs.h"

/*===========================================================================

                   EXTERNAL VARIABLES

===========================================================================*/
/*! @brief Invalid value for LTE sys band returned to the caller
*/
#define LTE_RRC_INVALID_LTE_SYS_BAND 0xFFFF

/*! @brief MAX band number in both LTE(10) and NB1(64) ACQ DB
*/
#define LTE_RRC_MAX_PSM_ACQ_DB_BANDS  2

/*! @brief Enumeration of extended RRC state
*/
typedef enum
{
  LTE_RRC_STATE_EXT_INACTIVATE = 0,      /* Applicable to LTE and NB1 */
  LTE_RRC_STATE_EXT_IDLE_NOT_CAMP,       /* Applicable to LTE and NB1 */
  LTE_RRC_STATE_EXT_IDLE_CAMPED_ANYCELL, /* Applicable to LTE ONLY    */
  LTE_RRC_STATE_EXT_IDLE_CAMPED_NORMAL,  /* Applicable to LTE and NB1 */
  LTE_RRC_STATE_EXT_CONNECTING,          /* Applicable to LTE and NB1 */
  LTE_RRC_STATE_EXT_CONNECTED,           /* Applicable to LTE and NB1 */
  LTE_RRC_STATE_EXT_CLOSING,             /* Applicable to LTE and NB1 */
  LTE_RRC_STATE_EXT_SUSPENDED,           /* Applicable to LTE ONLY    */
  LTE_RRC_STATE_EXT_IRAT_TO_LTE_STARTED, /* Applicable to LTE ONLY    */
} lte_rrc_state_ext_e;

typedef struct
{
  /*! Number of bands included in LTE/NB1 PSM db */
  uint8                                        num_bands; 
  /*! The bands information in the LTE/NB1 PSM db, and the band information 
   will order NB1 then CATM1  */
  sys_sband_lte_e_type            bands[LTE_RRC_MAX_PSM_ACQ_DB_BANDS];   
}lte_rrc_static_psm_db_band_info_type_s;

/*===========================================================================

                   EXTERNAL FUNCTIONS

===========================================================================*/

/* Returns the PLMN selected and other info for camping. For limited service, it
   returns the first PLMN in SIB1 list. Returns TRUE if success, else FALSE. 
*/
extern boolean lte_rrc_get_camped_cell_info
(
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Pointer to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Tracking area code of cell */
);

/* Returns the primary PLMN  and other camped cell info
*/
extern boolean lte_rrc_get_camped_cell_info_with_primary_plmn
(
  lte_rrc_plmn_s *primary_plmn_ptr,
   /*!< Pointer to primary PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Tracking area code of cell */
);

extern boolean lte_rrc_get_camped_cell_info_based_on_rat
(
  sys_radio_access_tech_e_type rat, 
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Pointer to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr,
  /*!< Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Tracking area code of cell */,
  boolean *is_nb1_raster_offset_present,
  sys_nb1_raster_offset_e_type* nb1_raster_offset_ptr,
  lte_rrc_rx_power_measurement_s_type *serving_cell_meas_ptr
);


/* Returns TRUE if band corresponding to the DL EARFCN is supported in the LTE band Pref */
extern boolean lte_rrc_earfcn_is_supported 
(
  lte_earfcn_t earfcn, /*!< DL EARFCN */
  sys_lte_band_mask_e_type lte_band_preference /*!< LTE band preference */
);

/* Function to get the owner (stack) of dedicated priorities */
extern lte_rrc_dedicated_pri_owner_e lte_rrc_dedicated_pri_get_owner 
(
  void
);

/* Function to set the owner (stack) of dedicated priorities, returns TRUE if owner is updated successfully */
extern boolean lte_rrc_dedicated_pri_set_owner 
(
  lte_rrc_dedicated_pri_owner_e owner
);

/* Function to determine if a cell's downlink arfcn belongs to TDD or FDD bands */
extern boolean lte_rrc_cell_is_tdd
(
  lte_earfcn_t dl_earfcn
);

/* Returns TRUE if any of the FDD bands in the LTE band Pref are present */
extern boolean lte_rrc_lte_fdd_is_supported 
(
  sys_lte_band_mask_e_type lte_band_preference /*!< LTE band preference */
);

/* Returns TRUE if any of the TDD bands in the LTE band Pref are present */
extern boolean lte_rrc_lte_tdd_is_supported 
(
  sys_lte_band_mask_e_type lte_band_preference /*!< LTE band preference */
);

/* Function to override default 10sec timer value for W2L Redirection List timer,
   
   Valid values are 1000ms - 10000ms, anything outside the range will default to spec
   defined value. 
   
   If timer is already running, the value takes effect the next time it runs.

   Function returns FALSE if LTE SW is not in a state to accept value*/
extern boolean lte_rrc_irat_from_w_set_redir_list_tmr 
( 
  uint64 test_time_ms 
);

/* Function to override default timer value for G2L Redirection List timer,
   and deciding Lte search behvior for the given timer.
   
   Valid timer (test_time_ms) values are 1000ms - 10000ms, anything outside the range will default to spec
   defined value. 
   
   Valid behavior (list_srch_only) values,
   TRUE  - will perform list search contineously on the provided frequencies till timer expires.
   FALSE - will perform one round of list search followed by one round of full search before timer expires.
   
   If timer is already running, the value takes effect the next time it runs.

   Function returns FALSE if LTE SW is not in a state to accept value*/
extern boolean lte_rrc_irat_from_g_set_redir_list_tmr 
( 
  uint64  test_time_ms,
  boolean list_srch_only
);
/*Provides LTE-RRC release version*/
extern lte_3gpp_release_ver_e lte_rrc_get_3gpp_release_version
(
  void
);

/*Delete Acq db EFS file. Returns 0 if success, -1 in case of failure or EFS is missing*/
extern mcfg_fs_status_e_type lte_rrc_delete_acq_db
(
  void
);
/* Function to provide decoded SI info */
extern int lte_rrc_get_decoded_sib_string
(
  void *encoded_si_ptr, /*!< Pointer to encoded SI */
  uint32 encoded_si_size, /*!< Size of encoded SI */
  void *buffer_ptr,
  uint32 buffer_length
);

/*API which allow setting LTE-RRC FGI*/
boolean lte_rrc_cmapi_fgi_change_req
(
  lte_fgi_rel_type_e fgi_rel_type, /*!< Indicate if the FGI is for Rel-8/9/10 */
  uint32 fgi_word, /*!< 32 bit LTE Rel-8/9/10 FGI word, , 
  MSB(Bit -31) is FGI-1 or 33 or 101 &  LSB(Bit -0) is FGI-32 or 64 or 132 */
  lte_rrc_div_duplex_e eutra_mode  /*! Indicates whether to update FDD FGI or TDD FGI*/
);

/*API which requests RRC to reread specific EFS files*/
boolean lte_rrc_cmapi_efs_reread_req
(
  /*! 
    Bit position
(starting from right most)	EFS file intended to be reread by LTE RRC
    1	                         /nv/item_files/modem/lte/common/lte_category
    2	                         /nv/item_files/modem/lte/common/ca_disable 
    3	                         /nv/item_files/modem/lte/rrc/cap/fgi
    4	                         /nv/item_files/modem/lte/rrc/cap/fgi_tdd
    5	                         /nv/item_files/modem/lte/rrc/cap/fgi_rel9
    6	                         /nv/item_files/modem/lte/rrc/cap/fgi_tdd_rel9
    7	                         /nv/item_files/modem/lte/rrc/cap/fgi_r10
    8	                         /nv/item_files/modem/lte/rrc/cap/fgi_tdd_rel10
    */
  uint64 efs_bitmask
);

/*API which allow setting LTE-RRC PCI_LOCK Info*/
boolean lte_rrc_cmapi_set_pci_lock_req
(
  lte_earfcn_t  dl_earfcn,
  /*! PCI */
  uint16 pci
);

/*API which allow setting LTE-RRC EARFCN_LOCK Info*/
boolean lte_rrc_cmapi_set_earfcn_lock_req
(
  lte_earfcn_t  dl_earfcn1,
  lte_earfcn_t  dl_earfcn2
);
/* API will return eMBMS coverage status 
  0 - implies is not camped on an LTE cell or 
      implies both SIB13 and SIB15 are not scheduled in SIB1 of current cell,
  1 - implies SIB13 and optionally SIB15 scheduled in SIB1 of current cell,
  2 - implies SIB15 alone scheduled in SIB1 of current cell. */
extern uint8 lte_rrc_cmapi_get_embms_coverage_status
(
  void
);

/* API will return MTCH data MCS for all PMCHs that are actively being monitored.
  Caller needs to allocate memory for the passed parameter */
extern void lte_rrc_cmapi_get_pmch_datamcs_info
(
  lte_rrc_datamcs_info_s *datamcs_info_ptr /*!< Ptr to datamcs info */
);

/* API will return MBSFN SNR of all active TMGIs. When TMGIs are not active this API
  returns unicast SNR. The below API call is blocking as RRC has to use msgr framework
  to fetch SNR info from ML1 */
extern void lte_rrc_cmapi_get_mbsfn_snr
(
  lte_rrc_mbsfn_snr_info_s *mbsfn_snr_info_ptr /*!< Ptr to MBSFN SNR info */
);

/*Provides LTE band for the given earfcn. band_is_valid must be verified before use*/
extern sys_sband_lte_e_type lte_rrc_get_band_from_dl_earfcn
(
  lte_earfcn_t dl_earfcn, /*!< DL EARFCN */
  boolean *band_is_valid_ptr /*!< Is it valid? */
);

/* This function returns an array of LTE_CSG_cells (max LTE_IRAT_MAX_MACRO_CELLS)
   associated with the W_Macro_Ptr */
extern uint8 lte_rrc_csg_get_fp_lcsg_info_for_w
(
 const source_cell_info_type *w_macro_ptr, /*! W MAcro cell info ptr */
 source_cell_info_type *l_csg_cells_ptr /*! LTE CSG cells array */
);

/* Convert band number to band enum (sys_sband_lte_e_type) */
extern sys_sband_lte_e_type lte_rrc_band_from_int_to_enum
(
  uint16 band  /*!< Band enum */
);

/* Converts earfcn from one band to another (MFBI band combos only allowed) */
extern errno_enum_type lte_rrc_convert_dl_earfcn_by_band
(
  lte_earfcn_t *dl_earfcn,
  sys_sband_lte_e_type from_band,
  sys_sband_lte_e_type to_band  
);

/* Converts earfcn to freq in 100 Mhz */
extern uint16 lte_rrc_get_freq_from_earfcn
(
  lte_earfcn_t earfcn /*!< Earfcn */
);

/*Provides if LTE MFBI is supported or not*/
extern boolean lte_rrc_cap_mfbi_is_supported
(
  boolean is_tdd
);

/*Provides if LTE OTDOA is supported or not*/
extern boolean lte_rrc_cap_inter_freq_rstd_meas_supported
(
  void
);

/* Provides extended RRC State */
extern lte_rrc_state_ext_e lte_rrc_get_rrc_state_ext();

/*Reads the specified NV flag. Read value is populated in item_ptr 
  If the NV reading activity is successful, return TRUE, else return FALSE*/
boolean lte_nv_read
(
  nv_items_enum_type nv_item, /* Item to get */
  nv_item_type      *item_ptr /* Pointer where to put the item */
);

extern uint8 _lte_rrc_cap_inter_freq_gap_less_meas_enabled
(
  sys_sband_lte_e_type pcell_band, 
  sys_sband_lte_e_type scell_band
);

/* Get value of PSM Threshold from CM and pass it to ML1 sleep*/
extern uint32 lte_rrc_csp_get_psm_threshhold(void);
/* Return Success if RRC is able to camp on PSM cell*/
extern boolean lte_rrc_csp_is_psm_succeed(void);

extern boolean nb1_rrc_cap_multi_drb_is_supported(void); 

extern boolean lte_static_get_psm_db_band_info
(
   lte_rrc_static_psm_db_band_info_type_s * lte_band_info
);
extern void lte_rrc_send_receive_wwan_high_priority_req
(
  void 
);

extern uint16 lte_rrc_csp_get_psm_img_switch_reason(void);

extern  boolean lte_rrc_csp_is_acq_try_for_psm (void);

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
extern boolean lte_rrc_plmn_search_get_scan_ref_time
(
  uint64 *scan_ref_time_ptr
);
#endif

#endif /* LTE_RRC_EXT_API_H */

