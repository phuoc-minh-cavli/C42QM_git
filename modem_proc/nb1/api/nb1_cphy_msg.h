/*!
  @file   nb1_cphy_msg.h

  @brief  Defines the CPHY SAPs used to interface with L1.

  @detail

  This file describes the interface from upper layers to L1.  This interface
  is expected to be used principally by RRC, although there is a portion
  of this interface to be used by MAC as well.

  This interface is generated from
  $Header:

  @mainpage
  This documentation describes the L1 module.  The interface to L1 is
  broken down into @ref cphy_sap and @ref phy_sap.  L1 also exposes a
  test-only interface which exposes additional functionality to be used in
  PLT in @ref cphy_test_sap


*/
/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly
  reserved. By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_cphy_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
                   due to HARQ failures in Tuneaway
                   specified in per 36.133 sec 4.2.2.9 and 8.1.2.1.1.1.
                   specified in per 36.133 sec 4.2.2.9.
                    RX Chain requests
                   to ML1 only in bplmn_cell_req during HPLMN search.
                   defined a default value maro for search window size
                     nb1_l1_tdd_ul_dl_cfg_index_e
                   shareable interface file.
                   to RRC
                   types to nb1_cphy_idle_meas_config_req
                   from dedicated cfg req
                   nb1_cphy_common_cfg_req_param_s, required for RLF detection/
                   recovery.
                     removed: cfg_bitmask
                     added:
                       requested_cfg_bitmask
                       disabled_cfg_bitmask
                   for PUCCH power control
                   req, updated num of si msgs value
                   UMID NB1_CPHY_CANCEL_CONN_REQ
                   Scott; removed paging params from the req
                   Added in the RNTI type in nb1_cphy_ul_pkt_build_ind_s

===========================================================================*/
#ifndef NB1_CPHY_MSG_H
#define NB1_CPHY_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

//#include "msgr_umid.h"
#include <msgr_nb1.h>
#include <msgr.h>
#include "nb1.h"
#include "nb1_cphy_msg_ids.h"
#include "nb1_l1_types.h"
#include <sys.h>
#include <sys_v.h>
#include "trm.h"
#include <nb1_irat_types.h>
#include "nb1_l1_ftm.h"
#include "qmi_mmode_msgr_msg.h"

/*===========================================================================

                      DEFINITIONS

===========================================================================*/
/*! @brief Maximum size of the black list = (4 + 16)
           4 cells are those blacklisted by RRC in previous acq attempts
           16 cells is the max blacklist passed from another RAT on IRAT to
              NB1 idle mode mobility
 */
#define NB1_CPHY_MAX_ACQ_BLACKLISTED_CELLS      20


/*! @brief Fill in a correct value here */
#define NB1_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS   1


/*! @brief Number of band scan candidates to be returned */
#define NB1_CPHY_BANDSCAN_NUM_CANDIDATES_FDD    30

/*! @brief Number of band scan candidates to be returned for TDD */
#define NB1_CPHY_BANDSCAN_NUM_CANDIDATES        30

/*! @brief Number of system scan candidates to be returned */
#define NB1_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN 10


/*! @brief Maximum number of RNTIs in the PDCCH setup interface */
#define NB1_CPHY_MAX_NUM_PDCCH_RNTIS            NB1_MAX_NUM_RNTI_TYPE

/*! @brief Maximum number of SI messages that can be specified in a SIB
    schedule
*/
#define NB1_CPHY_MAX_SI_MSGS                    8

/*! @brief uint8 infinity value */
#define NB1_CPHY_UINT8_INFINITY                 0xFF

/*! @brief int8 minus infinity value */
#define NB1_CPHY_INT8_NEG_INFINITY              -128

/*! @brief Maximum number of bands passed into for BPLMN band scan */
#define NB1_CPHY_BPLMN_MAX_BANDS                 64

/*! @brief Maximum number of frequency passed into for BPLMN frequency scan */
#define NB1_CPHY_BPLMN_MAX_FREQS                 64

/*! @brief special number to indicate "unlimited" time for a BPLMN search */
#define NB1_CPHY_BPLMN_UNLIMITED_TIME           0xFFFFFFFF

/*! @brief current band + Max_additional frequency bands*/
#define NB1_CPHY_MAX_OVERLAP_BANDS         9

/*! @brief Defines a measurement threshold for EUTRA
           For RSRP threshold, range 0..97
           For RSRQ threshold, range 0..34
*/
typedef int16 nb1_cphy_meas_eutra_thresh_t;

/*! @brief
  Maximum number of neighbour cells referenced with a measurement object
  Defined in 36.331 Section 6.4
*/
#define NB1_CPHY_MEAS_MAX_MEAS_OBJ_CELLS 32

/*! @brief the max SNR level for frequency scan support in NB1*/
#define NB1_CPHY_FSCAN_MAX_SNR_LEVEL_SUPPORT           3
#define NB1_CPHY_FSCAN_MAX_HCORR_LEVEL_SUPPORT         2 //Shallow and Deep

/* CE Reporting periodicity in msec */
#define NB1_CPHY_CE_RPT_PERIODICITY 500

/*! @brief Size of RSRP thresholds PRACH Info List Size */
#define NB1_MAC_RSRP_THRESH_PRACH_LIST_SIZE 2


/*!
  @brief
  An enum describing the SNR level using LFS/FFS
  */
typedef enum
{
//ML1 will Map HCORR level to SNR level during Init ACQ. RRC will pass only
//HCORR levels.
#if 0 
  NB1_CPHY_FSCAN_SNR_LEVEL_0               = 0x00, /*SNR level 0*/
  NB1_CPHY_FSCAN_SNR_LEVEL_1               = 0x01, /*SNR level 1*/
  NB1_CPHY_FSCAN_SNR_LEVEL_2               = 0x02, /* SNR level 2*/
  NB1_CPHY_FSCAN_SNR_LEVEL_3               = 0x03,
#endif
  NB1_CPHY_HCORR_SCAN_LEVEL_SHALLOW        = 0x00,
  NB1_CPHY_HCORR_SCAN_LEVEL_DEEP           = 0x01,                        

}nb1_cphy_fscan_snr_level_type_e;

/* Power class enum indicated to L1 */
typedef enum
{
  NB1_CPHY_POWER_CLASS1 = 1,
  NB1_CPHY_POWER_CLASS2 = 2,
  NB1_CPHY_POWER_CLASS3 = 3,   /*Default power class*/
  NB1_CPHY_POWER_CLASS_DEFAULT = NB1_CPHY_POWER_CLASS3,
  NB1_CPHY_POWER_CLASS4 = 4,
  NB1_CPHY_POWER_CLASS5 = 5,
  NB1_CPHY_POWER_CLASS6 = 6,
  NB1_CPHY_POWER_CLASS_MAX = 7,
}nb1_cphy_power_class_e;

/*! Image switch to full once condition statify */
typedef enum
{
  NB1_ML1_POI_PSM_IMG_SWITCH_NONE,
  NB1_ML1_POI_PSM_IMG_SWITCH_OOS,
  NB1_ML1_POI_PSM_IMG_SWITCH_PAGE,
  NB1_ML1_POI_PSM_IMG_SWITCH_RESEL,
  NB1_ML1_POI_PSM_IMG_SWITCH_ACQ_FAIL,
  NB1_ML1_POI_PSM_IMG_SWITCH_SLEEP_ENTRY,
  NB1_ML1_POI_PSM_IMG_SWITCH_JAMMER_DETECT,
  NB1_ML1_POI_PSM_IMG_SWITCH_HSFN_JMP,
  NB1_ML1_POI_PSM_IMG_SWITCH_RSN_SI_MOD
} nb1_ml1_poi_psm_img_switch_type_e;

/*===========================================================================

                      TYPEDEF USED IN THE CPHY INTERFACE

===========================================================================*/
/******************************************************************************
                BAND SCAN & SYSTEM SCAN structures
*******************************************************************************/
/*! @brief
    Fields defining an NB1 system
*/
typedef struct
{
  /*! System center frequency */
  nb1_earfcn_t          earfcn;

  /*! System band 0, 1..14, 33..40 */
  sys_sband_lte_e_type  band;

  /*! System bandwidth */
  nb1_bandwidth_e       bandwidth;
} nb1_cphy_system_s;

/*! @brief
  Scan results structure
*/
typedef struct
{
  /*! Contains the band, freq and bandwidth */
  nb1_cphy_system_s  system;

  /*! Detected cell ID (0 - 503) */
  nb1_phy_cell_id_t      phy_cell_id;

  /*! Energy metric in units of dBm/100KHz */
  int16              energy;

  /** ASN.1-decoded MIB */
  nb1_l1_nb_mib_info_param_s mib;

  /** SFN on which MIB was received and decoded on PBCH */
  nb1_l1_sfn_t               mib_sfn;
} nb1_cphy_scan_result_s;

/*! @brief
  Parameters for the unusable systems in band scan request
*/
typedef struct
{
  /*! the list of EARFCNs that ML1 should ignore during band scans */
  nb1_cphy_system_s             system[NB1_CPHY_BANDSCAN_NUM_CANDIDATES_FDD];

  /*! the number of systems in the list that ML1 should not search on */
  uint8                         num_systems;
}nb1_cphy_band_scan_req_unusable_systems_s;

/*! @brief Defines bandwidth masks.
*/
typedef uint16 nb1_cphy_bw_mask_t;

/*! @brief
  Parameters for the band scan request
*/
typedef struct
{
  /*! Number of bands to scan 1.. MAX_SUPPORTED_BANDS(TBD) */
  uint8                 num_bands;

  /*! List of bands to scan*/
  sys_sband_lte_e_type  band[NB1_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS];

  /*! Flag to indicate if an actual scan is needed */
  boolean               scan_needed;

  /*! timeout value for the procedure (in seconds) */
  uint32                t_timeout;

  /*!Flag to indicate whether it is the new search for band scan */
  boolean             new_band_scan;

  /*! SNR level will be used during band scan for NB1 */
  nb1_cphy_fscan_snr_level_type_e              snr_level;

   /*! index which will be used by ML1 start search frequency index during band scan 
   for new band scan, search start index should be 0 */
  uint16                         search_start_index;

  /*! Unusable systems */
  nb1_cphy_band_scan_req_unusable_systems_s unusable_systems;

}nb1_cphy_band_scan_req_params_s;

/*! @brief
  Parameters for the system scan request
*/
typedef struct
{
  /*! Whether ML1 should use early abort threshold if available; threshold to be
     configured at compile time and provided by systems team */
  boolean            use_early_abort;

  /*!Flag to indicate whether it is the new search for system scan */
  boolean          new_system_scan;

  /*! The minimum number of cells ML1 should return despite early abort
      threshold criterion being satisfied */
  uint8              min_num_systems_to_scan;

  /*! Number of systems to scan 1..10 */
  uint8              num_systems;

  /*! Index which will be used by ML1 to start search frequency index during system scan. 
      For new system scan, search start index should be 0 */
  uint16             search_start_index;

  /*! timeout value for the procedure (in seconds) */
  uint32             t_timeout;

  /*! SNR level will be used during system scan for NB1 */
  nb1_cphy_fscan_snr_level_type_e                 snr_level;

  /*! List of systems to scan */
  nb1_cphy_system_s  system[NB1_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];

} nb1_cphy_system_scan_req_params_s;

/*! @brief
  Band scan results structure; one per band
*/
typedef struct
{
  /*! System band */
  sys_sband_lte_e_type    band;

  /*! Number of candidates found 0..1000 */
  uint16                  num_candidates;

   /*! List of candidates found in band, sorted according to energy (highest first)*/
  nb1_cphy_scan_result_s  candidates[NB1_CPHY_BANDSCAN_NUM_CANDIDATES];
} nb1_cphy_band_scan_result_s;

/*! @brief
  Parameters for the band scan cnf
*/
typedef struct
{
  /*! Number of band results; matches number of bands requested
      1.. NB1_BANDSCAN_MAX_SUPORTED_BANDS */
  uint8                        num_bands;

  /*! List of scan results, one for each band requested; order matches
      requested list of bands struct */
  nb1_cphy_band_scan_result_s  band[NB1_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS];
} nb1_cphy_band_scan_cnf_params_s;

/*! @brief
  Parameters for system scan cnf
*/
typedef struct
{
  /*! Number of systems scanned; 0 to num requested */
  uint8                   num_systems;

  /*! List of results, sorted according to energy (highest first) */
  nb1_cphy_scan_result_s  system[NB1_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];

  /*! Number of nb1 systems scanned; 0 to num requested */
  uint8                   num_nb1_systems;

  /*! nb1 systems scan result */
  nb1_cphy_scan_result_s  nb1_system;

} nb1_cphy_system_scan_cnf_params_s;

/*!
  @brief
  An enum describing the bit positions in the nb1_l1_dms_base_seq_cfg_mask_t
  */
enum
{
  NB1_L1_DMS_BASE_SEQ_CFG_MASK_BIT_THREE_TONE = 0x01,
  NB1_L1_DMS_BASE_SEQ_CFG_MASK_BIT_SIX_TONE = 0x02,
  NB1_L1_DMS_BASE_SEQ_CFG_MASK_BIT_TWELVE_TONE = 0x04,
  NB1_L1_DMS_BASE_SEQ_CFG_CURRENT_VALID_MASK_BITS = 0xFFFF
};

typedef uint16 nb1_l1_dms_base_seq_cfg_mask_t;

typedef enum
{
  NB1_CPHY_SIB1_NRS_POWER_OFFSET_NON_ANCHOR_MINUS12_DB = -12,
  NB1_CPHY_SIB1_NRS_POWER_OFFSET_NON_ANCHOR_MINUS10_DB = -10,
  NB1_CPHY_SIB1_NRS_POWER_OFFSET_NON_ANCHOR_MINUS8_DB = -8,
  NB1_CPHY_SIB1_NRS_POWER_OFFSET_NON_ANCHOR_MINUS6_DB = -6,
  NB1_CPHY_SIB1_NRS_POWER_OFFSET_NON_ANCHOR_MINUS4_DB = -4,
  NB1_CPHY_SIB1_NRS_POWER_OFFSET_NON_ANCHOR_MINUS2_DB = -2,
  NB1_CPHY_SIB1_NRS_POWER_OFFSET_NON_ANCHOR_0_DB = 0,
  NB1_CPHY_SIB1_NRS_POWER_OFFSET_NON_ANCHOR_3_DB = 3
}nb1_cphy_nrs_power_offset_non_anchor_s;

/*! @brief
    DMS Cfg
*/
typedef struct
{
  /*Define 3 cyclic shifts for the 3-tone case*/
  uint8 three_tone_cyclic_shift;

  /*Define 4 cyclic shifts for the 6-tone case*/
  uint8 six_tone_cyclic_shift;

  /*Base sequence Cfg mask*/
  nb1_l1_dms_base_seq_cfg_mask_t cfg_mask;

  /*The base sequence of DMRS sequence in a cell for 3 tones  transmission;
    If absent, it is given by NB-IoT CellID mod 12.*/
  uint8 three_tone_base_seq;

  /*The base sequence of DMRS sequence in a cell for 6 tones transmission;
    If absent, it is given by NB-IoT CellID mod 14.*/
  uint8 six_tone_base_seq;

  /*The base sequence of DMRS sequence in a cell for 12 tones  transmission;
    If absent, it is given by NB-IoT CellID mod 30.*/
  uint8 twelve_tone_base_seq;

}  nb1_l1_dmrs_cfg_s ;

/******************************************************************************
                              PDSCH config common
*******************************************************************************/

/*! @brief
    PDSCH config common params  (ref: 36.331, 6.3.2)
*/
typedef struct {

  /*! The reference signal power - Range is -60 - 50
  */
  int8 ref_signal_pwr;

} nb1_cphy_pdsch_cfg_common_param_s;


/******************************************************************************
                              PUSCH config
*******************************************************************************/

/*! @brief
    PUSCH config params (ref: 36.331, 6.3.2)
*/
typedef struct {
  // ACK NAK Number of repetition for MSG4
  uint8 num_nprach_resource;
  nb1_l1_ack_nack_num_rep_e ack_nak_num_rep_msg4[NB1_MAX_NPRACH_RESOURCE];

  // SRS Sub frame config
  boolean enable_srs_sf_cfg;
  nb1_l1_srs_sf_cfg_e srs_sf_cfg;

  // DMRS Config
  boolean enable_dmrs_cfg;
   nb1_l1_dmrs_cfg_s  dmrs_cfg;

  /********************** UL refernce signal PUSCH ****************************/

  /*! Provides information if group hopping is enabled or disabled */
  boolean group_hopping_enabled;

    /*! Provides information about the assigned PUSCH group */
  nb1_cphy_pusch_grp_pusch_assign_t group_assignment_pusch;
} nb1_cphy_pusch_cfg_common_param_s;

/*! @brief
    N PRACH Params
*/
typedef struct {
  nb1_l1_nprach_periodicity_e periodicity;
  nb1_l1_nprach_start_time_e start_time;
  nb1_l1_nprach_sub_carr_offset_e sub_carr_offset;
  nb1_l1_nprach_num_sub_carr_e num_sub_carr;
  nb1_l1_nprach_sc_msg3_range_start_e sc_msg3_range_start;
  nb1_l1_nprach_max_num_preamble_attempt_e max_num_preamble_attempt;
  nb1_l1_nprach_num_rep_per_preamble_attempt_e num_rep_per_preamble_attempt;
  nb1_l1_nprach_npdcch_num_rep_e npdcch_num_rep;
  nb1_l1_nprach_npdcch_start_sf_css_e npdcch_start_sf_css;
  nb1_l1_nprach_npdcch_offset_e npdcch_offset;

  /*! Boolean to indicate if num_cbra_start_sub_carr is enabled or not */
  boolean num_cbra_start_sub_carr_enable;
  nb1_l1_nprach_num_cbra_start_sub_carr_e num_cbra_start_sub_carr;
} nb1_l1_nprach_param_s;

/*! @brief
  PRACH static parameters
}
*/
typedef struct
{
  // Carrier idx ranges from 0-14 for Non-Anchor and MAX value for Anchor as of now
  uint32 ul_carrier_idx;
  uint32 dl_carrier_idx;
  // Cyclic Prefix length
  nb1_l1_prach_cp_len_e cp_len;

  uint8 num_ra_rsp_win_size;
  /*! Window size for reception of RACH message2.
      Defined in 36.331 Section 6.3.2 - GenericRandomAccessParams */
  nb1_l1_ra_rsp_win_size_e  ra_rsp_win_size_new[NB1_MAX_NPRACH_RESOURCE];

  // N PRACH Config
  uint8 prach_param_ce_list_size;

  nb1_l1_nprach_param_s prach_param_ce_list[NB1_MAX_NPRACH_RESOURCE];
} nb1_cphy_prach_setup_param_s;

/******************************************************************************
                            UL power control common
*******************************************************************************/

/*! @brief
    UL power control common config params (ref: 36.331, 6.3.2)
*/
typedef struct {
  /*!p0_nominal value for PUSCH. Range: -126..24 in dBm */
  int16 p0_nominal_pusch;

  /*! Alpha is a 3-bit cell specific parameter provided by higher layers */
  nb1_l1_ul_power_ctrl_alpha_e alpha;

  /*! Parameter delta preamble msg 3*/
  int8 delta_preamble_msg3;
} nb1_cphy_ul_power_ctrl_common_param_s;

/******************************************************************************
                            DRX information
*******************************************************************************/

typedef struct
{
  //TRUE if pcch cfg for non-anchor is valid
  boolean pcch_cfg_valid;

  /*! nPDCCH - NumberRepetitionPaging. Sib2 rep_paging applies if not configured*/
  nb1_l1_num_rep_paging_e num_rep_paging;

  //non-Anchor weight. Default value is w1
  nb1_l1_paging_weight_e page_weight;

}nb1_l1_pcch_cfg_non_anchor_s;

/*! @brief
    Idle DRX information
*/
typedef struct
{
  /*! UE paging cycle in milli-seconds */
  uint16 paging_cycle;

  /*! nb value - used to derive the number of paging groups */
  nb1_l1_nb_e nb;

  /*! UE identity (IMSI/STMSI) Complete IMSI is sent */
  nb1_l1_drx_ue_id_t ue_id;

  /*! Mod per coefficient from sib2*/
  uint16 mod_per_coeff;
  /*! nPDCCH - NumberRepetitionPaging*/
  nb1_l1_num_rep_paging_e num_rep_paging;

  //non-Anchor paging parameters
  //can be of RANGE 0 to NB1_CPHY_MAX_NON_ANCHOR_CARRIER
  //If 0, NO non-anchor carriers and all pages are in anchor carrier
  uint8 num_non_anchor;

  //Anchor weight. Applicable only when atleast one non anchor carrier pcch config is valid
  nb1_l1_paging_weight_e page_anchor_weight;

  nb1_l1_pcch_cfg_non_anchor_s pcch_cfg_non_anchor[NB1_CPHY_MAX_NON_ANCHOR_CARRIER];

} nb1_cphy_idle_drx_param_s;

/*! @brief
    Idle Extended DRX information, sent by NAS to RRC; Ref NAS Spec TS 23.682
*/
typedef struct
{
  /*! UE EDRX paging cycle length in ms */
  /* EDRX paging cycles in ms are 5120, 10240, 20480, 40960,61440, 81920, 102400, 122880, 143360,163840, 327680,
                                   655360, 1310720, 2621440 , 5242880, 10485760ms */
   uint32     edrx_paging_cycle;

  /*! Page Time Window (PTW) Length in ms ( Multiple of 1280ms from 0 to 15 )*/
  /* Values PTW can take are 1280, 2560, 3840, 5120, 6400, 7680, 8960, 10240,
     11520, 12800, 14080, 15360 16640, 17920, 19200, 20480, 35840, 30840 ,40960 */
  uint32      edrx_ptw_len_ms;

  /* UE identity (S-TSMI) 32bit, S-TMSI is used for Computing 32 bit CRC to 
     get EDRX PTW Start HSFN*/
  uint32	ue_id_s_tmsi;

} nb1_cphy_idle_edrx_param_s;

/*! @brief
  UIM Page IND sent to UIM with time to next page start time
  IND would be set when page decode is started or when page
  start is rescheduled due to successful UE page decode ind from RRC
*/

typedef struct{

  /*! Message header */
  msgr_hdr_struct_type     msgr_hdr;

  /* Time till next page start time in ms*/
  uint32                   next_page_start_time_ms;

  /*! This flag is used to indicate if IND is sent at page start time or not
      TRUE if IND is sent at page start time,
      FALSE Otherwise Eg. Page reschedule scenarios during OFF time */
  boolean                 is_page_start;

}nb1_cphy_edrx_uim_page_ind_s;

/*! @brief
    Connected mode DRX dedicated information
*/
typedef struct
{
  /*! DRX Enable flag - Indicates whether to enable or disable Dedicate mode DRX */
  boolean drx_enable;

  /*! On Duration timer in PDCCH Periods*/
  uint8 on_duration_timer;

  /*! Inactivity timer in PDCCH Periods */
  uint8 inactivity_timer_new;

  /*! Retx timer in PDCCH Periods*/
  uint8 retx_timer;

  /*! DRX Cyle in number of Sub frames*/
  uint16 drx_cycle;

  /*! DRX Start Offset in number of Sub frames */
  uint8 drx_start_offset;

  /*! DRX UL Retx timer in PDCCH Periods*/
  uint16 drx_ul_retx_tmr;

} nb1_cphy_drx_cfg_dedicated_param_s;

/******************************************************************************

                            Cell Select config - S-criteria information
*******************************************************************************/
/*! @brief
  SIB1 S-criteria information
*/
typedef struct {
  /*! Minimum Rx level to camp on cell (-70..-22) * 2

   @todo update values based upon discussion within l1
   */
  nb1_l1_q_rxlevmin_t q_rxlevmin;

  /*! Range: 1..8 when included in NB1_CPHY_BPLMN_CELL_REQ during HPLMN search.
      Set to 0 in all other cases.

    Offset to the signalled Qrxlevmin taken into account in the Srxlev
    evaluation as a result of a periodic search for a higher priority
    PLMN while camped normally in a VPLMN
   */
  nb1_l1_q_rxlevmin_offset_t q_rxlevmin_offset;

  /*! Maximum value for output power that the UE should use */
  int8 p_max;

#ifdef FEATURE_NB1_TO_BE_REMOVED
  /*! Boolean to indicate if Cell selection info for Release 9 is present;
    For Rel 8 this boolean will always be set to FALSE. For Rel 9 the field will
    always be set to TRUE and either signaled values or default values will be passed */

  boolean cell_select_info_rel9_present;

  /*! Cell selection info Release 9 */
  nb1_l1_cell_select_info_rel9_s cell_select_info_rel9;
#endif
   /*! Minimum required quality level in the cell in dB, Range (-34 .. -3),
    default value NB1_CPHY_INT8_NEG_INFINITY when not signaled */
  int8 q_qualmin;

  boolean  Qoffset_temp_valid;
 
 /*! Qoffset_temp to be used for cell selection. Range (0-15) 
	 Default value 0xFF.  ML1 needs to apply this when T300 expired on the cell*/
 uint8 Qoffset_temp;

  /* powerClass14dBm-Offset
     Parameter "Poffset" in TS 36.304. Only applicable for UE supporting powerClassNB-14dBm. Value in dB.
     If the fied is absent, the UE applies the (default) value of 0 dB for "Poffset".
  */
  int8 Poffset;

  /*! Enum to indicate what's kind of the power class for this UE. */
  nb1_cphy_power_class_e power_class_cfg;
} nb1_cphy_cell_select_cfg_s;

/*! @brief
        DL Gap Cfg
*/
typedef struct
{

  boolean enable;

  /*! DL Gap Threshold*/
  nb1_l1_dl_gap_thresh_e thresh;

  /*! DL Gap Periodicity*/
  nb1_l1_dl_gap_periodicity_e periodicity;

  /*! DL Gap Duration Coeff*/
  nb1_l1_dl_gap_duration_coeff_e coeff;

} nb1_cphy_dl_gap_cfg_param_s;

/*! @brief
         UL non- Anchor config 
*/
typedef struct
{
  //can be of RANGE 0 to NB1_CPHY_MAX_NON_ANCHOR_CARRIER
  //If 0, NO non-anchor carriers and all RACH are in anchor carrier
  uint8 num_non_anchor;

  /*!UL Carrier Frequency */
  nb1_carrier_freq_s non_anchor_ul_carrier_freq[NB1_CPHY_MAX_NON_ANCHOR_CARRIER];

}nb1_cphy_ul_non_anchor_cfg_param_s;

/*! @brief
        In Band Carrier Info params
*/
typedef struct
{
  // PCI Indicator
  nb1_l1_pci_indicator_e pci_indicator;

  // Index to MID Prb (-55 to 54)
  // To be used only when PCI Indicator points to Same PCI
  int8 idx_to_mid_prb;

  // Number of CRS Ports
  // To be used only when PCI Indicator points to Diff PCI
  nb1_eutra_num_crs_ports_e num_crs_ports;

  // EUTRA Control Region Size
  nb1_cphy_eutra_control_region_size_e control_size;
} nb1_cphy_in_band_carrier_info_param_s;

/*!
  @brief
  A Struct describing the DL Bitmap
*/
typedef struct
{
  /* Indicates whether default or explicit config is being passed for DL bitmap */
  nb1_cphy_dl_bitmap_cfg_type_e dl_bitmap_cfg_type;

  //Size of Sub frame patter. Can be either 10 bits or 40 bits
  nb1_cphy_dl_bitmap_type_e sf_pattern_size;

  /*! downlink subframe configuration over 10ms or 40ms for inband
   and 10ms for standalone/guardband. The first/leftmost bit corresponds
   to the subframe #0 of the radio frame satisfying SFN mod x = 0, where x
   is the size of the bit string divided by 10. Value 0 in the bitmap
   indicates that the corresponding subframe is invalid for downlink transmission.
   Value 1 in the bitmap indicates that the corresponding subframe is valid for
   downlink transmission

   The bit mask will be placed in the lower order bits.

   Bits 0 to 9 corresponds to pattern size 10 and
   Bits 0 to 39 corresponds to pattern size 40
  */
  uint64 dl_bitmap;
}nb1_cphy_dl_bitmap_cfg_s;


/*! @brief
    DL Carrier Cfg dedicated params
*/
typedef struct
{
  /*! DL Carrier Frequency*/
  nb1_carrier_freq_s dl_carrier_freq;

  /*! DL Bitmap for Non Anchor
          Can be either None (not signalled) or Use No Bitmap
          or Use Anchor Bitmap or Use Explicit Bitmap*/
  nb1_l1_dl_cfg_non_anchor_e dl_bm_non_anchor;
  /*! Explicit DL Bitmap for Non Anchor.  To be used only when the above enum says
          Explicit Bitmap to used*/
  nb1_cphy_dl_bitmap_cfg_s explicit_dl_bm;

  /*! DL gap for Non Anchor
          Can be either None (not signalled) or Use No Gap
          or Use Anchor gap or Use Explicit gap*/
  nb1_l1_dl_cfg_non_anchor_e dl_gap_non_anchor;
  /*! Explicit DL gap for Non Anchor.  To be used only when the above enum says
          Explicit gap to used*/
  nb1_cphy_dl_gap_cfg_param_s explicit_dl_gap;

  /*! In Band Carrier Info params*/
  boolean enable_in_band_carrier_info;
  nb1_cphy_in_band_carrier_info_param_s in_band_carrier_info;
  
  nb1_cphy_nrs_power_offset_non_anchor_s  nrs_poweroffset_non_anchor;
} nb1_cphy_dl_carrier_cfg_dedicated_param_s;

/*!
  @brief
  An enum describing the NRS CRS power offset in NB Sib1
  */
typedef enum
{
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_MINUS6_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_MINUS4_77__DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_MINUS3_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_MINUS1_77_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_0_DB,            //Default NRS CRS power offset
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_1_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_1_23_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_2_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_3_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_4_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_4_23_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_5_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_6_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_7_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_8_DB,
  NB1_CPHY_SIB1_NRS_CRS_POWER_OFFSET_9_DB
}nb1_cphy_nrs_crs_power_offset_e;

/*! @brief
         DL non- Anchor config 
*/
typedef struct
{
  //can be of RANGE 0 to NB1_CPHY_MAX_NON_ANCHOR_CARRIER
  //If 0, NO non-anchor carriers and all DL are in anchor carrier
  uint8 num_non_anchor;

  nb1_cphy_dl_carrier_cfg_dedicated_param_s non_anchor_cfg_param[NB1_CPHY_MAX_NON_ANCHOR_CARRIER];

  /*! NRS power offset non-Anchor*/
  nb1_cphy_nrs_power_offset_non_anchor_s nrs_poweroffset[NB1_CPHY_MAX_NON_ANCHOR_CARRIER];
  
  boolean nrs_crs_power_offset_except_achor_present;
  
  nb1_cphy_nrs_crs_power_offset_e nrs_crs_power_offset_except_anchor;
}nb1_cphy_dl_non_anchor_cfg_param_s;

/*! @brief
    UL Carrier Cfg dedicated params
*/
typedef struct
{
  /*!UL Carrier Frequency */
  nb1_carrier_freq_s ul_carrier_freq;
} nb1_cphy_ul_carrier_cfg_dedicated_param_s;

/*! @brief
    two HARQ Cfg dedicated params
*/
typedef struct
{
  /*!is TWO HARQ configured flag */
  boolean two_harq_configured;
} nb1_cphy_two_harq_cfg_dedicated_param_s;

 /*! @brief
	 interferenceRandomisation Cfg dedicated params
 */
 typedef struct
 {
   /*!is interferenceRandomisation configured flag */
   boolean interference_random_configured;
 } nb1_cphy_interference_random_cfg_dedicated_param_s;  


/*! @brief
    Carrier Cfg dedicated params
*/
typedef struct
{
  /*! UL Carrier Dedidcated Config*/
  nb1_cphy_ul_carrier_cfg_dedicated_param_s ul_carrier_ded_cfg;

  /*! DL Carrier Dedidcated Config*/
  nb1_cphy_dl_carrier_cfg_dedicated_param_s dl_carrier_ded_cfg;
} nb1_cphy_carrier_cfg_dedicated_param_s;


/*! @brief
    NPDCCH Cfg dedicated params
*/
typedef struct
{
  /*! NPDCCH Number of repetitions*/
 nb1_l1_npdcch_num_rep_e num_rep;

 /*! NPDCCH Start SF USS*/
 nb1_l1_npdcch_start_sf_uss_e start_sf_uss;

 /*! NPDCCH Offset USS*/
 nb1_l1_npdcch_offset_uss_e offset_uss;

} nb1_cphy_npdcch_cfg_dedicated_param_s;

/*! @brief
    NPUSCH Cfg dedicated params
*/
typedef struct
{
  boolean enable;

  /*!
  Number of repetitions for the ACK NACK resource unit carrying HARQ response to NPDSCH
  If absent, the value of ack-NACK-NumRepetitions-Msg4 signalled in SIB2 is used
  */
  boolean enable_ack_nak_num_rep;
  nb1_l1_ack_nack_num_rep_e ack_nak_num_rep;

  /*!
  If set to TRUE,  the UE shall use all NB-IoT symbols for NPUSCH transmission
    (either Set to TRUE in OTA or not signaled)
  If set to FALSE, the UE punctures the NPUSCH  transmissions in the symbols that collides with SRS
  */
  boolean npusch_all_symbols;

  /*!
    Disable-sequence-group-hopping
  */
  boolean group_hopping_disabled;
} nb1_cphy_npusch_cfg_dedicated_param_s;

/*! @brief
    UL power control dedicated params (ref: 36.331, 6.3.2)
*/
typedef struct {
  /*! P0_UE value for PUSCH. Range: -8..7 in dB - Field applicable for non
      persistent scheduling only
  */
  nb1_l1_ul_power_ctrl_p0_ue_t p0_ue_pusch;

} nb1_cphy_ul_power_ctrl_dedicated_param_s;

/*! @brief
  UE related configuration parameters
*/
typedef struct
{
  /*! UE CRNTI */
  nb1_rnti_val_t crnti;
} nb1_cphy_ue_related_cfg_s;

/*! @brief
  Parameter struct for cell info in L1M acquisition confirmation
*/
typedef struct
{
  /*! Detected cell ID (0 - 503) */
  nb1_phy_cell_id_t                     phy_cell_id;

  /** ASN.1-decoded MIB */
  nb1_l1_nb_mib_info_param_s            mib;

  /** SFN on which MIB was received and decoded on PBCH */
  nb1_l1_sfn_t                          mib_sfn;

  /*! Detected cyclic prefix */
  nb1_l1_cp_e                           cp;

  /* NW configured customized tx-rx spacing applied */
  boolean                               custom_duplex_spacing_applied;  
} nb1_cphy_acq_cell_info_s;

/*! @brief
  Parameter struct for cell info describing frequency in bandwith
*/
typedef struct
{

  /*! The cell id this configuration applies to or the source cell in case of
      handover */
  nb1_phy_cell_id_t phy_cell_id;

  /*! The frequency the configuration applies to or the source cell in case of handover */
  nb1_earfcn_t dl_cell_frequency;

}nb1_cphy_cell_info_s;

/*! @brief
  Parameter struct for the reselection candidate
*/
typedef struct
{
  /*! LTE specific cell info for reselection candidate*/
  nb1_cphy_cell_info_s  nb1_cell_info;
        
}nb1_cphy_resel_poi_params_s;

/*! @brief
  Parameter struct for cell info describing frequency in bandwith
*/
typedef struct
{
  nb1_carrier_freq_s ul_carrier_freq;
  /* additionalSpectrumEmission Range: 0-31 */
  uint8 add_spectrum_emission;
}nb1_cphy_ul_freq_cfg_s;

/*! @brief
    Hyper SFN indication is sent to ML1 from RRC with Hyper SFN value
    obtained in SIB 1
*/
typedef struct{

  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The information of the current cell for which this request is sent
      used for sanity purposes */
  nb1_cphy_cell_info_s curr_cell_info;

  /* SFN value when Hyper SFN ind is sent */
  nb1_l1_sfn_t	       curr_sfn_val;

  nb1_l1_hyper_sfn_t   hyper_sfn_val;

}nb1_cphy_hyper_sfn_req_s;

/*! @brief 
  NB1 TX start info provided to GNSS
*/

typedef struct{

   /*! Time of next scheduled NB1 Tx start, in MPSS time ticks in ms */
   uint32                 tx_start_time;
   
   /*! Duration of the next scheduled LTE Tx in 1ms */
   uint32                 tx_duration;   
   
   /*! NB1 Tx Band information */
   sys_sband_lte_e_type   tx_band;
   
   /*! Bandwidth of Tx in kHz */
   nb1_bandwidth_e        tx_bw;
   
   /*! Power of the concerned NB1 Tx in dBm */
   int32                  tx_pwr;
   
   /*! Sequence number of the message sent.Gets incremented for 
        every start message sent and would get set to 0 
        if exceeds the MAX boundary.*/
   uint16                 tx_seq_cnt; 
   
} nb1_cphy_tx_start_info_s;

/*! @brief structure for indicating the start 
    of the NB1 Tx session to its clients through MSGR 
*/

typedef struct{

  /*! Message header */
   msgr_hdr_struct_type       msg_hdr;

   /*! Information regarding TX start */
   nb1_cphy_tx_start_info_s   tx_start_info;
   
} nb1_cphy_tx_start_ind_s;

/*! @brief 
  NB1 TX end info provided to GNSS
*/

typedef struct{

  /*! Time of next scheduled NB1 Tx start, in MPSS time ticks in ms */
  uint32  tx_end_time;

  /*! Sequence number of the message sent.Gets incremented for 
       every start message sent and would get set to 0 
       if exceeds the MAX boundary.*/
  uint16  tx_seq_cnt;
  
} nb1_cphy_tx_end_info_s;

/*! @brief structure for indicating the end of the 
    NB1 Tx session to its clients through MSGR 
*/

typedef struct{

  /*! Message header */
  msgr_hdr_struct_type         msg_hdr;

  /*! Information regarding TX end */
  nb1_cphy_tx_end_info_s       tx_end_info;
  
} nb1_cphy_tx_end_ind_s;

/******************************************************************************
                              UE Timers and Constants Parameters
*******************************************************************************/

/*! @brief UE Timers and Constants parameters
  */
typedef struct
{

  /* Timer to detect radio link failure. Started when n310 out-of-sync
    indications received. Stopped when n311 in-sync indications received. */
  nb1_cphy_t310_e t310;

  /* Maximum Number of out-of-sync indications to detect physical layer problems */
  nb1_cphy_n310_e n310;

  /* Maximum Number of in-sync indications to detect recovery from physical layer problems */
  nb1_cphy_n311_e n311;

} nb1_cphy_tmr_and_const_param_s;

/*! @brief Dedicated RLF Timers and Constants
  */
typedef struct
{

  /* If dedicated RLF timers and constants are enabled */
  boolean enable;

  /* Dedicated timers and constants */
  nb1_cphy_tmr_and_const_param_s tmr_and_const;

} nb1_cphy_rlf_tmr_and_const_dedicated_param_s;

/*! @brief
    Idle DRX information needed for CSI-RS from SIB2
*/
typedef struct
{
  /*! UE paging cycle in milli-seconds */
  uint16 paging_cycle;

  /*! nb value - used to derive the number of paging groups */
  nb1_l1_nb_e nb;

  /*! nPDCCH - NumberRepetitionPaging*/
  nb1_l1_num_rep_paging_e num_rep_paging;

} nb1_cphy_pcch_cfg_common_param_s;


/*!
  @brief
  An enum describing the bit positions in the nb1_cphy_common_cfg_mask_t
  */
enum
{
  NB1_CPHY_COMMON_CFG_MASK_BIT_UL_FREQ_CFG = 0x01,
  NB1_CPHY_COMMON_CFG_MASK_BIT_PDSCH_CFG = 0x02,
  NB1_CPHY_COMMON_CFG_MASK_BIT_PUSCH_CFG = 0x04,
  NB1_CPHY_COMMON_CFG_MASK_BIT_UL_POWER_CTRL = 0x08,
  NB1_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG = 0x10,
  NB1_CPHY_COMMON_CFG_MASK_BIT_TMR_AND_CONST_PARAM = 0x20,
  NB1_CPHY_COMMON_CFG_MASK_BIT_PCCH_CFG = 0x40,
  NB1_CPHY_COMMON_CFG_MASK_BIT_DL_GAP_CFG = 0x80,
  NB1_CPHY_COMMON_CFG_MASK_BIT_NON_ANCHOR_UL_CFG = 0x100,
  NB1_CPHY_COMMON_CFG_MASK_BIT_NON_ANCHOR_DL_CFG = 0x200,
  NB1_CPHY_COMMON_CFG_CURRENT_VALID_MASK_BITS = 0x3FFFF
};

/*! @brief
  Channel config mask to enable the channel configuration changes. Only used
  in CPHY interface.

  @see nb1_cphy_common_cfg_mask_bit_e for a description of the bits
       in this mask.
*/
typedef uint16 nb1_cphy_common_cfg_mask_t;

/*!
  @brief The parameters for the l1 common config request.

  This sets up the common parameters received in the SIBs.  This command
  can be used any time after a cell has been acquired.
  */
typedef struct
{
  /*! Mask of the commands to be configured (or setup)*/
  nb1_cphy_common_cfg_mask_t cfg_mask;
  /*! Parameters required to ul frequency cfg info */
  nb1_cphy_ul_freq_cfg_s ul_freq_cfg;
  /*! Parameters required to configure PDSCH physical channel */
  nb1_cphy_pdsch_cfg_common_param_s pdsch_cfg_common;
  /*! Parameters required to configure PUSCH physical channel */
  nb1_cphy_pusch_cfg_common_param_s pusch_cfg_common;
  /*! Parameters required to configure UL Power Control information */
  nb1_cphy_ul_power_ctrl_common_param_s ul_power_ctrl_common;
  /*! Cell selection params: required for S-criteria check */
  nb1_cphy_cell_select_cfg_s cell_selection_cfg;
  /*! UE timers and constant params: required for RLF detection/recovery */
  nb1_cphy_tmr_and_const_param_s tmr_and_const_param;
  /*! Parameters required to configure Rel-10 CSI-RS from SIB2*/
  nb1_cphy_pcch_cfg_common_param_s       pcch_cfg_common;
  /*! DL Gap Config*/
  nb1_cphy_dl_gap_cfg_param_s      dl_gap_cfg_common;
  /*! UL non-Anchor config*/
  nb1_cphy_ul_non_anchor_cfg_param_s ul_non_anchor_cfg_common;
  /*! DL non-Anchor config*/
  nb1_cphy_dl_non_anchor_cfg_param_s dl_non_anchor_cfg_common;
} nb1_cphy_common_cfg_req_param_s;

/*!
  @brief
  An enum describing the bit positions in the nb1_cphy_common_cfg_mask_t
  */
enum
{
  NB1_CPHY_SIB1_COMMON_CFG_DL_BITMAP = 0x1,
  NB1_CPHY_SIB1_COMMON_CFG_EUTRA_CONTROL_REGION_SIZE = 0x2,
  NB1_CPHY_SIB1_COMMON_CFG_NRS_CRS_POWER_OFFSET = 0x4,
  NB1_CPHY_SIB1_COMMON_CFG_CURRENT_VALID_MASK_BITS = 0x7
};

/*! @brief
  Channel config mask to enable the channel configuration changes. Only used
  in CPHY interface.
*/
typedef uint16 nb1_cphy_sib1_common_cfg_mask_t;

/*!
  @brief The parameters for the SIB1 common config request.

  This sets up the common parameters received in the SIBs.  This command
  can be used any time after a cell has been acquired.
  */
typedef struct
{
  /*! SIB1 configuration parameters bitmask */
  nb1_cphy_sib1_common_cfg_mask_t    sib1_common_cfg_mask;

  /*! DL Bitmap. If not present in Sib1 Common cfg req, All UE shall assume
   that all subframes are valid (except for subframes carrying NPSS/NSSS/NPBCH/SIB1-NB) */
  nb1_cphy_dl_bitmap_cfg_s           dl_bitmap_cfg;

  /* Control region size of EUTRA and applicable only for Inband configuration.
   Indicates the control region size of the E-UTRA cell for the in-band operation mode.
   Unit is in number of OFDM symbols */
  nb1_cphy_eutra_control_region_size_e  cr_size;

  /*Appliable only for Inband same PCI.
   NRS power offset between NRS and E-UTRA CRS. Unit in dB. Default value of 0*/
  nb1_cphy_nrs_crs_power_offset_e   nrs_crs_power_offset;

} nb1_cphy_sib1_common_cfg_req_param_s;

/*!
  @brief The parameters for the CPHY SIB1 common config request.

  This sets up the common parameters received in the SIB1.  This command
  can be used any time after a cell has been acquired.
  */
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type               msgr_hdr;

  /*! The information of the cell info for which this SIB1 comm cfg is sent, used for sanity purposes */
  nb1_cphy_cell_info_s               cell_info;

  /*! Configuration parameters from SIB1 */
  nb1_cphy_sib1_common_cfg_req_param_s sib1_common_cfg_param;

} nb1_cphy_sib1_common_cfg_req_s;

/*!
  @brief
  An enum describing the bit positions in the
  nb1_cphy_common_mobility_cfg_mask_t
  */
enum
{
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_FREQ_CFG = 0x0001,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PDSCH_CFG = 0x0002,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PUCCH_CFG = 0x0004,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PUSCH_CFG = 0x0008,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_SRS_CFG = 0x0010,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_POWER_CTRL = 0x0020,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PHICH_CFG = 0x0040,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_ANTENNA_CFG = 0x0080,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_UL_CYCLIC_PREFIX_LENGTH = 0x100,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_TDD_CFG = 0x0200,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_P_MAX = 0x0400,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_NCELL_CFG = 0x0800,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_PRACH_CFG = 0x8000,
  NB1_CPHY_COMMON_MOBILITY_CFG_MASK_BIT_DUPLEX_SPACING_CFG = 0x10000,
  /*! UPDATE the value below whenever any of the above bitmasks change */
  NB1_CPHY_COMMON_MOBILITY_CFG_CURRENT_VALID_MASK_BITS = 0x0FFF
};

/*! @brief
  Channel config mask to enable the channel configuration changes. Only used
  in CPHY interface.

  @see nb1_cphy_common_mobility_cfg_mask_bit_e for a description of the bits
       in this mask.
*/
typedef uint16 nb1_cphy_common_mobility_cfg_mask_t;

/*!
  @brief The common mobility parameters passed upon handover.

  This is a union of the parameters detected and found on the PBCH and
  the ones found on the SIB.  Any parameters not passed in this request
  will default to the ones on the current serving cell.

  */
typedef struct
{
  /*! Mask of the commands to be configured (or setup)*/
  nb1_cphy_common_mobility_cfg_mask_t cfg_mask;
  /*! Target Cell uplink information */
  nb1_cphy_ul_freq_cfg_s ul_freq_cfg;
  /*! Parameters required to configure PDSCH physical channel */
  nb1_cphy_pdsch_cfg_common_param_s pdsch_cfg_common;
  /*! Parameters required to configure PUSCH physical channel */
  nb1_cphy_pusch_cfg_common_param_s pusch_cfg_common;
  /*! Parameters required to configure UL Power Control information */
  nb1_cphy_ul_power_ctrl_common_param_s ul_power_ctrl_common;
  /*! Parameters required to configure UL prefix cyclic length */
  nb1_cphy_ul_cyclic_prefix_length_e ul_cyclic_prefix_length;
  /*! P_max value required for Uplink transmissions, maximum value for output
    power that the UE should use */
  int8 p_max;
  /*! neighCellConfig : Provides information related to MBSFN and TDD UL/DL configuration
   of neighbour cells of this frequency  */
  uint8 neighbourCellConfig;

} nb1_cphy_common_mobility_req_param_s;

/*!
  @brief
  An enum describing the bit positions in the
  nb1_cphy_dedicated_cfg_mask_t
  */
enum
{
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_PDSCH_CFG = 0x0001,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_PUCCH_CFG = 0x0002,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_PUSCH_CFG = 0x0004,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_SRS_CFG = 0x0008,
//  NB1_CPHY_DEDICATED_CFG_MASK_BIT_UL_POWER_CTRL = 0x0010,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_CQI_CFG = 0x0020,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_ULSCH_CFG = 0x0040,
//  NB1_CPHY_DEDICATED_CFG_MASK_BIT_DRX_CFG   = 0x0080,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_ANTENNA_CFG = 0x0100,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_TPC_PDCCH_CFG_PUSCH = 0x0200,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_TPC_PDCCH_CFG_PUCCH = 0x0400,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_SCHEDULING_REQ = 0x0800,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_UE_RELATED_CFG = 0x1000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_SPS_CFG = 0x2000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_PHR_CFG = 0x4000,
//  NB1_CPHY_DEDICATED_CFG_MASK_BIT_TMR_AND_CONST_PARAM = 0x8000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_SCELL_CFG = 0x10000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_PCELL_REL10_CFG = 0x20000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_CSI_RS_CFG = 0x40000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_SUBFR_PATTERN_PCELL = 0x80000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_NEIGH_CELL_CRS_CFG = 0x100000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_SUBFR_PATTERN_INTRA_NCELL = 0x200000,
  /*! UPDATE the value below whenever any of the above bitmasks change */
//  NB1_CPHY_DEDICATED_CFG_CURRENT_VALID_MASK_BITS = 0x3FFFFF
} ;

/*!
  @brief
  An enum describing the bit positions in the
  nb1_cphy_dedicated_cfg_mask_t
  */
enum
{
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_NPDCCH_CFG = 0x400000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_NPUSCH_CFG = 0x800000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_UL_POWER_CTRL = 0x0010,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_TWO_HARQ_CFG = 0x2000000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_TMR_AND_CONST_PARAM = 0x8000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_CARRIER_DED_CFG = 0x1000000,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_DRX_CFG   = 0x0080,
  NB1_CPHY_DEDICATED_CFG_MASK_BIT_INTERFERENCE_RANDOM_CFG = 0x4000000,
  /*! UPDATE the value below whenever any of the above bitmasks change */
  NB1_CPHY_DEDICATED_CFG_CURRENT_VALID_MASK_BITS = 0x7FFFFFF
} ;




/*! @brief
  Channel config mask to enable the channel configuration changes. Only used
  in CPHY interface.

  @see nb1_cphy_common_mobility_cfg_mask_bit_e for a description of the bits
       in this mask.
*/
typedef uint32 nb1_cphy_dedicated_cfg_mask_t;

/*!
  @brief The parameters for the dedicated configuration request.

  The common parameters must be configured prior to receiving these
  parameters.
  */
typedef struct
{
  /*! Mask of the commands to be configured (or setup)*/
  nb1_cphy_dedicated_cfg_mask_t cfg_mask;

  /*! Parameters required to configure Carrier Config DED information */
  nb1_cphy_carrier_cfg_dedicated_param_s carrier_cfg_dedicated;
  /*! Parameters required to configure NPDCCH Config DED information */
  nb1_cphy_npdcch_cfg_dedicated_param_s npdcch_cfg_dedicated;
  /*! Parameters required to configure NPUSCH Config DED information */
  nb1_cphy_npusch_cfg_dedicated_param_s npusch_cfg_dedicated;
  /*! Parameters required to configure UL Power Control information */
  nb1_cphy_ul_power_ctrl_dedicated_param_s ul_power_ctrl_dedicated;
  /*! Dedicated RLF timers and constant params: used for RLF detection/recovery */
  nb1_cphy_rlf_tmr_and_const_dedicated_param_s rlf_tmr_and_const_param;
  /*! The parameters for connected-mode drx parameters */
  nb1_cphy_drx_cfg_dedicated_param_s      con_drx_cfg;
  /*! Parameters for two HARQ is configured*/
  nb1_cphy_two_harq_cfg_dedicated_param_s two_harq_cfg;
  /*! Parameters for interferenceRandomisation is configured*/
  nb1_cphy_interference_random_cfg_dedicated_param_s interference_random_cfg;
} nb1_cphy_dedicated_cfg_req_param_s;

/*!
  @brief
  An enum describing the bit positions in the nb1_cphy_sib_mask_t

  CHANGE SIB MASKS IN LL1/intf/intf_dl_si.h INTERFACE FILE IF NEED TO CHANGE HERE
  */
enum
{
  NB1_CPHY_SIB_MASK_BIT_MIB=0x1,
  NB1_CPHY_SIB_MASK_BIT_SIB1=0x2,
  NB1_CPHY_SIB_MASK_BIT_SIB2=0x4,
  NB1_CPHY_SIB_MASK_BIT_SIB3=0x8,
  NB1_CPHY_SIB_MASK_BIT_SIB4=0x10,
  NB1_CPHY_SIB_MASK_BIT_SIB5=0x20,
  NB1_CPHY_SIB_MASK_BIT_SIB9=0x200,  
  NB1_CPHY_SIB_MASK_BIT_SIB14=0x4000,
  NB1_CPHY_SIB_MASK_BIT_SIB16=0x10000,
  NB1_CPHY_SIB_MASK_BIT_SIB22=0x400000,
  NB1_CPHY_SIB_MASK_VALID_BITS_MASK = 0x41403F
};

/*! @brief Mask used to represent SI-MSG bits */
#define NB1_CPHY_VALID_SIMSG_BITS_MASK (NB1_CPHY_SIB_MASK_VALID_BITS_MASK & \
                                        ~(NB1_CPHY_SIB_MASK_BIT_MIB | NB1_CPHY_SIB_MASK_BIT_SIB1))

/*! @brief Bitmask that defines System Information Block numbers.

  @see nb1_cphy_sib_mask_bit_e for the bit positions in this mask
 */
typedef uint32 nb1_cphy_sib_mask_t;

/*! @brief SI message reception window length in ms; used in SIB Sched Req
*/
typedef enum
{
  /** 160 ms */
  NB1_CPHY_SI_WINDOW_LENGTH_MS_160 = 0,
  /** 320 ms */
  NB1_CPHY_SI_WINDOW_LENGTH_MS_320,
  /** 480 ms */
  NB1_CPHY_SI_WINDOW_LENGTH_MS_480,
  /** 640 ms */
  NB1_CPHY_SI_WINDOW_LENGTH_MS_640,
  /** 960 ms */
  NB1_CPHY_SI_WINDOW_LENGTH_MS_960,
  /** 1280 ms */
  NB1_CPHY_SI_WINDOW_LENGTH_MS_1280,
  /** 1600 ms */
  NB1_CPHY_SI_WINDOW_LENGTH_MS_1600,

  NB1_CPHY_SI_WINDOW_LENGTH_MS_MAX
}nb1_cphy_si_window_length_e;


typedef enum
{
  /** 64 radio frames */
  NB1_CPHY_SI_PERIODICITY_RF_64,
  /** 128 radio frames */
  NB1_CPHY_SI_PERIODICITY_RF_128,
  /** 256 radio frames */
  NB1_CPHY_SI_PERIODICITY_RF_256,
  /** 512 radio frames */
  NB1_CPHY_SI_PERIODICITY_RF_512,
  /** 1024 radio frames */
  NB1_CPHY_SI_PERIODICITY_RF_1024,
  /** 2048 radio frames */
  NB1_CPHY_SI_PERIODICITY_RF_2048,
  /** 4096 radio frames */
  NB1_CPHY_SI_PERIODICITY_RF_4096,
  NB1_CPHY_SI_PERIODICITY_RF_MAX
}nb1_cphy_si_periodicity_e;


typedef enum
{
  NB1_CPHY_SI_REPETITION_EVERY_2_RF,
  NB1_CPHY_SI_REPETITION_EVERY_4_RF,
  NB1_CPHY_SI_REPETITION_EVERY_8_RF,
  NB1_CPHY_SI_REPETITION_EVERY_16_RF
}nb1_si_repitition_pattern_e;

typedef enum
{
  NB1_CPHY_SI_TB_SIZE_BITS_56 = 56,
  NB1_CPHY_SI_TB_SIZE_BITS_120 = 120,
  NB1_CPHY_SI_TB_SIZE_BITS_208 = 208,
  NB1_CPHY_SI_TB_SIZE_BITS_256 = 256,
  NB1_CPHY_SI_TB_SIZE_BITS_328 = 328,
  NB1_CPHY_SI_TB_SIZE_BITS_440 = 440,
  NB1_CPHY_SI_TB_SIZE_BITS_552 = 552,
  NB1_CPHY_SI_TB_SIZE_BITS_680 = 680
}nb1_si_transport_block_size_e;

/*! @brief SIB scheduling information; used in SIB Sched Req;
   one per SI message
*/
typedef struct
{
  /** Periodicity of the SI message in radio frames */
  nb1_cphy_si_periodicity_e si_periodicity;

  /** Bitmask to indicate the SIBs carried in this SI message. Eg.
  *  if this SI message carries SIB1 and SIB3, bits 1 and 3 will
  *  be set. This bitmask is similar in nature to the "SIB needed"
  *  bitmasks in the SIB Sched Req hence a direct AND
  *  operation of the two fields would determine the need to
  *  decode this SI message */
  nb1_cphy_sib_mask_t sib_map_bitmask;

  /*Indicates the starting radio frames within the SI window used for SI message transmission*/
  nb1_si_repitition_pattern_e si_rep_pattern;

  /*Indicates the Transport Block size in Bits*/
  nb1_si_transport_block_size_e si_tb_size;
}nb1_cphy_si_msg_info_s;

/*! @brief
  Configuration for applying partial roaming restriction (36.304 5.2.4.4).
*/
typedef enum
{
  /*! Default case with no exceptions for restrictions */
  NB1_CPHY_PARTIAL_ROAMING_ALLOW_NONE,

  /*! Allow target frequency during an acq_req after a cell redirection
      from connected mode. */
  NB1_CPHY_PARTIAL_ROAMING_ALLOW_TARGET_FREQ,

  /*! Allow all frequencies during 'any cell selection' */
  NB1_CPHY_PARTIAL_ROAMING_ALLOW_ALL,

  /*! Invalid max value */
  NB1_CPHY_PARTIAL_ROAMING_MAX

} nb1_cphy_partial_roaming_restriction_e;



/******************************************************************************
                Cell Reselection indication and cancel requests
*******************************************************************************/

/*! @brief
  CDMA reselection type enum
*/
typedef enum
{
  /*! Normal neighbor reselection */
  NB1_CPHY_CDMA_RESEL_NORMAL,
  /*! CDMA reverse time transfer reselection */
  NB1_CPHY_CDMA_RESEL_REVERSE_TIME_TRANSFER,

} nb1_cphy_cdma_resel_type_e;
/*! @brief
  Union of RAT specific cell info
*/
typedef union
{
  /*! NB1 specific cell info */
  nb1_cphy_cell_info_s  nb1_cell_info;

  /*! TBD: More params needed like RAT specific target cell info,
      PLMN related etc. (if any)
  */
} nb1_cphy_rat_specific_cell_info_u;

/*! @brief
  Cell reselection target cell structure
 */
typedef struct
{
  /*! Priority of the cell */
  nb1_l1_resel_priority_t priority;

  /*! RAT type of target cell */
  nb1_rat_e    rat_type;

  /*! RAT specific cell info */
  nb1_cphy_rat_specific_cell_info_u  rat_cell_info;

} nb1_cphy_resel_target_cell_info_s;

/*! @brief
  Parameter struct for cdma channel bar info
*/
typedef struct
{
  /*! RAT type of barred channel */
  nb1_rat_e    rat_type;

  /*! CDMA/HDR channel */
  sys_channel_type      channel;

    /*! barred time in seconds*/
  uint64                t_barred;

}nb1_cphy_cdma_channel_bar_info_s;

/*! @brief
  Structure to bar 1x/DO channels
 */
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

  uint16                     num_bar_channels;

  nb1_cphy_cdma_channel_bar_info_s  bar_list[NB1_RRC_MAX_1X_DO_AVOID_CHANNELS];

} nb1_cphy_cdma_channel_bar_req_s;

/*! @brief Ref Time Structure
*/
typedef struct
{
  /*! The time in RTC ticks */
  uint64            curr_rtc;

  /*! The SFN at which indication is sent */
  nb1_l1_sfn_t      curr_sfn;

  /*! The SubFn count at which indication was sent*/
  nb1_l1_subfn_t    curr_subfn;

}nb1_ref_time_s;

/*! @brief
  Cell reselection cancel cause.
 */
typedef enum
{
  NB1_CPHY_RESEL_CANCEL_CAUSE_NONE,
  NB1_CPHY_RESEL_CANCEL_CAUSE_S_FAILURE,
  NB1_CPHY_RESEL_CANCEL_CAUSE_SIB_READ_FAILURE,
  NB1_CPHY_RESEL_CANCEL_CAUSE_SIB_READ_TIMEOUT,
  NB1_CPHY_RESEL_CANCEL_CAUSE_DL_WEAK_IND,
  NB1_CPHY_RESEL_CANCEL_CAUSE_CELL_BARRED,
  NB1_CPHY_RESEL_CANCEL_CAUSE_FREQ_BARRED,
  NB1_CPHY_RESEL_CANCEL_CAUSE_NON_EQ_PLMN,
  NB1_CPHY_RESEL_CANCEL_CAUSE_FORBIDDEN_TA,
  NB1_CPHY_RESEL_CANCEL_CAUSE_ABORTED,
  NB1_CPHY_RESEL_CANCEL_CAUSE_FORBIDDEN_CSG_ID,
  NB1_CPHY_RESEL_CANCEL_CAUSE_IRAT_RESEL_FAILURE,
  NB1_CPHY_RESEL_CANCEL_CAUSE_IRAT_GSM_BSIC_MISMATCH,
  NB1_CPHY_RESEL_CANCEL_CAUSE_NOT_CSG,
  NB1_CPHY_RESEL_CANCEL_CAUSE_BAND_CHANGE,
  NB1_CPHY_RESEL_CANCEL_CAUSE_MAX
} nb1_cphy_resel_cancel_cause_e;

/*! @brief Enumeration for bar causes
*/
typedef enum
{
  NB1_CPHY_CELL_BAR_UNKNOWN,
  NB1_CPHY_CELL_BAR_SIB_READ_FAILURE,
} nb1_cphy_cell_bar_cause_e;

/*! @brief Enumeration for Unbar causes
*/
typedef enum
{
  NB1_CPHY_CELL_UNBAR_ALL,
  NB1_CPHY_CELL_UNBAR_FORBIDDEN_TA,
  NB1_CPHY_CELL_UNBAR_FORBIDDEN_CSG_ID,
  NB1_CPHY_CELL_UNBAR_PLMN_MISMATCH,
} nb1_cphy_cell_unbar_cause_e;


/******************************************************************************
                Idle-mode Measurement Configuration
*******************************************************************************/

/*! @brief
  Maximum number of intra-freq whitelisted cells
  Defined in 36.331 Section 6.4
*/
#define NB1_CPHY_MEAS_MAX_CELL_INTRA 16

/*! @brief
  Maximum number of intra-freq blacklisted cells
  Defined in 36.331 Section 6.4
*/
#define NB1_CPHY_MEAS_MAX_CELL_BLACK 16

/*! @brief
  Maximum number of inter-freq whitelisted cells
  Defined in 36.331 Section 6.4
*/
#define NB1_CPHY_MEAS_MAX_CELL_INTER 16

/*! @brief
  Maximum number of EUTRA Carrier Frequencies
  Defined in 36.331 Section 6.4
*/
#define NB1_CPHY_MEAS_MAX_NUM_FREQ 8

/*! @brief
  Maximum allowed Carrier Frequencies (including serving freq) across all RATs
  Defined in 36.133 Section 4.2.2.9 and 8.1.2.1.1
*/
#define NB1_CPHY_MEAS_MAX_ALLOWED_FREQ (NB1_CPHY_MEAS_MAX_NUM_FREQ+1)

/*! @brief
TDS Freq per Layer
  */
#define NB1_CPHY_MEAS_NUM_TDSCDMA_FREQ_PER_LAYER 3

/*! @brief
Maximum number of TDS Meas layers in Active list
  */
#define NB1_CPHY_MEAS_MAX_ACTIVE_TDSCDMA_MEAS_LAYERS 2

/*! @brief
Maximum number of TDSCDMA Freqs (in Active list)
1TDS meas layer count =
  (NB1_CPHY_MEAS_NUM_TDSCDMA_FREQ_PER_LAYER) freq layer count
  */
#define NB1_CPHY_MEAS_MAX_TDSCDMA_FREQS (NB1_CPHY_MEAS_NUM_TDSCDMA_FREQ_PER_LAYER * NB1_CPHY_MEAS_MAX_ACTIVE_TDSCDMA_MEAS_LAYERS)

/*! @brief
Maximum number of All RAT's neighbor frequencies
1 Inter freq meas layer count = 1 freq layer count
1 WCDMA meas layer count = 1 freq layer count
1 TDS meas layer count = NB1_CPHY_MEAS_NUM_TDSCDMA_FREQ_PER_LAYER freq layer count
1 CDMA meas layer count = 1 freq layer count
1 GSM freq grp layer count = 1 freq layer count
  */
#define NB1_CPHY_MEAS_ALL_RAT_NEIGH_FREQS 22


/*! @brief
  Maximum number of CDMA band classes
  Defined in 36.331 Section 6.3
*/
#define NB1_CPHY_MEAS_MAX_CDMA_BAND_CLASS 32

/*! @brief
  Maximum number of 1X neighbor cells
  Defined in 36.331 Section 6.3
*/
#define NB1_CPHY_MEAS_MAX_CDMA_1X_NEIGH_CELLS_REL9 40

/*! @brief
  Maximum number of CDMA neighbor cells possible in Rel 9
  Defined in 36.331 Section 6.3
*/
#define NB1_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS_REL9 40

/*! @brief
  Maximum CDMA neighbor cells in Rel 8
  Defined in 36.331 Section 6.3
*/
#define NB1_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS 32

/*! @brief Maximum number of GERAN frequency groups to be measured in
    idle mode.

  Defined in 36.331 Section 6.4
  */
#define NB1_CPHY_MEAS_MAX_GERAN_FREQ_GROUPS 16

/*! @brief Maximum number of GERAN frequencies in a frequency group

  Defined in 36.331 Section 6.4
  */
#define NB1_CPHY_MEAS_MAX_GERAN_FREQS_IN_GROUP 32

/*! @brief Maximum number of GERAN frequencies supported by ML1.

  This is based upon the minimum required number of frequencies defined
  in 36.133 Section 4.2.2.9.

  */
#define NB1_CPHY_MEAS_MAX_GERAN_FREQ 32

/*! @brief
Maximum number of UTRA frequencies in the neighbour
list
  */
#define NB1_CPHY_MEAS_MAX_WCDMA_FREQ 16

/*! @brief
Maximum number of TDD frequencies in the neighbour
list
  */
#define NB1_CPHY_MEAS_MAX_TDSCDMA_FREQ 16

/*! Number of WCDMA frequencies supported for CSG */
#define NB1_CPHY_MEAS_NUM_WCDMA_CSG_FREQS 3

/*! Number of NB1 cells per frequency supported for CSG */
#define NB1_CPHY_MEAS_NUM_WCDMA_CSG_CELLS 16

/*! Number of NB1 frequencies supported for CSG */
#define NB1_CPHY_MEAS_NUM_NB1_CSG_FREQS 3

/*! Number of WCDMA cells per frequency supported for CSG */
#define NB1_CPHY_MEAS_NUM_NB1_CSG_CELLS 16

/*!
  @brief
  An enum describing the bit positions in the nb1_cphy_idle_meas_cfg_mask_t
  */
enum
{
  NB1_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON  = 0x01,
  NB1_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ  = 0x02,
  NB1_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ    = 0x04,
  NB1_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ    = 0x08,
};

/*! @brief
  Idle measurements config  mask to enable the idle meas config. Only used
  in CPHY interface.
*/
typedef uint16 nb1_cphy_idle_meas_cfg_mask_t;

/*! @brief Mobility state parameters for deciding to enter medium/high mobility
*/
typedef struct
{
  /*! Eval duration to enter mobility states (in sec) */
  uint8 t_eval;

  /*! Additional eval duration to enter normal mobility state (in sec) */
  uint8 t_hyst_normal;

  /*! Num cell changes to enter medium mobility state */
  uint8 n_cell_change_medium;

  /*! Num cell changes to enter high mobility state */
  uint8 n_cell_change_high;

} nb1_cphy_meas_mobility_state_config_param_s;


/*! @brief Mobility state parameters for medium/high mobility
*/
typedef struct
{
  /*! If FALSE, then mobility state management is disabled, ignore rest of parameters */
  boolean is_valid;

  /*! Mobility State config params */
  nb1_cphy_meas_mobility_state_config_param_s config_params;

  /*! Hysteresis correction applied in medium mobility state (in +/- dB) */
  int8 q_hyst_medium;

  /*! Hysteresis correction applied in high mobility state (in +/- dB) */
  int8 q_hyst_high;

} nb1_cphy_meas_mobility_state_param_s;
typedef enum
{
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB0 = 0,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB1 = 1,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB2 = 2,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB3 = 3,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB4 = 4,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB5 = 5,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB6 = 6,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB8 = 8,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB10 = 10,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB12 = 12,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB14 = 14,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB16 = 16,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB18 = 18,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB20 = 20,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB22 = 22,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB24 = 24,
  NB1_CPHY_REDIR_CARRIER_OFFSET_DED_DB26 = 26,
}nb1_redir_carrier_offset_ded_e;


/*! Default Relaxed Monitoring Search Delta value to convey s_searchdeltap_r14 field 
    is not valid in lte_cphy_meas_cell_resel_common_s structure */
#define NB1_CPHY_IDLE_MEAS_DEF_S_SEARCHDELTAP_R14 0xFF

/*! @brief Measurement configuration common to intra-freq, inter-freq
           and inter-rat reselection procedures
*/
typedef struct
{
  /*! Hysteresis for cell reselection (in dB) */
  uint8 q_hyst;
  nb1_redir_carrier_offset_ded_e redir_carier_offset_ded;

  /*! Relaxed Monitoring Search delta parameter (in dB)
      CellReselectionInfoCommon-NB-v1450 ::=	SEQUENCE {
          s-SearchDeltaP-r14                 ENUMERATED {dB6, dB9, dB12, dB15}
      }

      The relaxed monitoring criterion is fulfilled when:
      -    (SrxlevRef - Srxlev) <  SSearchDeltaP
      Where:
      -    Srxlev = current Srxlev value of the serving cell (dB).
      -    SrxlevRef = reference Srxlev value of the serving cell (dB) */
  uint8 s_searchdeltap_r14;

} nb1_cphy_meas_cell_resel_common_s;

/*! @brief Serving frequency reselection info applicable for Rel 9
*/
typedef struct
{
  /*! s-NonIntraSearchQ (0..31) threshold applicable only for Rel9. If not signaled
    default 0 will be passed */
  nb1_l1_srch_meas_threshold_t s_non_intra_searchQ_r9;

  /*! if threshServingLowQ-r9 is passed */
  boolean thresh_serving_lowQ_r9_present;

  /*! threshServingLowQ-r9 (0..31) */
  nb1_l1_resel_threshold_t thresh_serving_lowQ_r9;

} nb1_cphy_meas_serving_freq_resel_info_rel9_s;


/*! @brief Parameters to used for serving freq when evaluating reselection
           candidates
*/
typedef struct
{
  /*! s-NonIntraSearch (0..62); For Rel 9, this field takes the value of the OTA field
    s-NonIntraSearchP if it is signaled. If not signaled, it will take the value of the
    Rel 8 OTA field s-NonIntraSearch. If neither of the OTA fields s-NonIntraSearchP or
    s-NonIntraSearch are signaled, default value NB1_CPHY_UINT8_INFINITY is used */
  nb1_l1_srch_meas_threshold_t s_non_intra_search;

} nb1_cphy_meas_serving_freq_resel_info_s;


/*! @brief Enumeration of signalled bandwidth used for measurements
*/
typedef enum
{
    NB1_CPHY_MEAS_BANDWIDTH_6,       /*!<  6  RBs */
    NB1_CPHY_MEAS_BANDWIDTH_15,      /*!< 15  RBs */
    NB1_CPHY_MEAS_BANDWIDTH_25,      /*!< 25  RBs */
    NB1_CPHY_MEAS_BANDWIDTH_50,      /*!< 50  RBs */
    NB1_CPHY_MEAS_BANDWIDTH_75,      /*!< 76  RBs */
    NB1_CPHY_MEAS_BANDWIDTH_100,     /*!< 100 RBs */
    NB1_CPHY_MEAS_BANDWIDTH_DL       /*!< Same downlink bandwidth as in MIB */

} nb1_cphy_meas_bandwidth_e;


/*! @brief Enumeration of timing-related scaling factors
*/
typedef enum
{
    NB1_CPHY_MEAS_TIME_SF_0DOT25,   /*!< 0.25 */
    NB1_CPHY_MEAS_TIME_SF_0DOT5,    /*!< 0.50 */
    NB1_CPHY_MEAS_TIME_SF_0DOT75,   /*!< 0.75 */
    NB1_CPHY_MEAS_TIME_SF_1DOT0     /*!< 1.00 */

} nb1_cphy_meas_time_sf_e;


/*! @brief Parameters for whitelisted cells for intra/inter frequency meas
*/
typedef struct
{
  nb1_phy_cell_id_t pci;    /*!< Physical cell id */
  int8 q_offset;            /*!< Cell-specific offset (in +/- dB) */

} nb1_cphy_meas_eutra_cell_s;


/*! @brief Parameters for blacklisted cells for intra/inter frequency meas
*/
typedef struct
{
  nb1_phy_cell_id_and_range_s pci_range;    /*!< Range of PCIs */

} nb1_cphy_meas_eutra_black_cell_s;

/*! @brief Parameters for blacklisted EARFCN PCIs
*/
typedef struct
{
  nb1_earfcn_t earfcn; /*!< EARFCN */
  nb1_phy_cell_id_and_range_s pci_range;    /*!< Range of PCIs */

} nb1_cphy_meas_eutra_freq_pci_range_s;

typedef enum
{
  NB1_T_RESELECTION_S0 = 0,
  NB1_T_RESELECTION_S3 = 3,
  NB1_T_RESELECTION_S6 = 6,
  NB1_T_RESELECTION_S9 = 9,
  NB1_T_RESELECTION_S12 = 12,
  NB1_T_RESELECTION_S15 = 15,
  NB1_T_RESELECTION_S18 = 18,
  NB1_T_RESELECTION_S21 = 21,
}nb1_t_reselection_e;
/*! @brief Intra-freq cell reselection measurement configuration parameters
*/
typedef struct
{
  /*! Brief description of this member */
  nb1_l1_q_rxlevmin_t q_rxlevmin;

  boolean q_qual_min_r13_present;

    /*! q_QualMin_r13 (-34..-3) , default
    NB1_CPHY_INT8_NEG_INFINITY will be passed */
  int8 q_qual_min_r13;

  /*! Maximum value for output power that the UE should use */
  int8 p_max;

  /*! Threshold current cell meas must fall below to consider intra freq for reselection;
    For Rel 9, this field takes the value of the OTA field s-IntraSearchP (0..62) if it is signaled,
    else if not signaled, the field takes the value of the Rel 8 OTA field s-IntraSearch. If neither of
    s-IntraSearchP or s-IntraSearch are signaled, default value NB1_CPHY_UINT8_INFINITY is used */
  nb1_l1_srch_meas_threshold_t s_intra_search;

  /*! t-ReselectionEUTRAN */
  //! @todo: Remove nb1_ once NB1 param is removed
  nb1_t_reselection_e   nb1_t_resel_eutran;

  /*! Cell-specific offsets */
  uint8 whitelist_num_cells;
  nb1_cphy_meas_eutra_cell_s whitelist[NB1_CPHY_MEAS_MAX_CELL_INTRA];

  /*! Blacklisted cells */
  uint8 blacklist_num_cells;
  nb1_cphy_meas_eutra_black_cell_s blacklist[NB1_CPHY_MEAS_MAX_CELL_BLACK];

  /*! Enum to indicate what's kind of the power class for this UE. */
  nb1_cphy_power_class_e power_class_cfg;

  /* powerClass14dBm-Offset
     Parameter "Poffset" in TS 36.304. Only applicable for UE supporting powerClassNB-14dBm. Value in dB.
     If the fied is absent, the UE applies the (default) value of 0 dB for "Poffset".
  */
  int8 Poffset;
} nb1_cphy_meas_intra_freq_resel_info_s;

/*! @brief Inter-freq cell reselection measurement configuration parameters
*/
typedef struct
{
  /*! E-UTRA DL carrrier frequency to measure on */
  //! @todo: Remove nb1_ once NB1 param is removed
  nb1_carrier_freq_s nb1_dl_carrier_freq;

  /*! Minimum required Rx RSRP level */
  nb1_l1_q_rxlevmin_t q_rxlevmin;

  /*! Maximum value for output power that the UE should use */
  int8 p_max;

  /*! Freq-specific offset */
  int8 q_offset_freq;

  /*! Cell-specific offsets */
  uint8 whitelist_num_cells;        /*!< 0 indicates parameter is absent */
  //! @todo: Remove nb1_ once NB1 param is removed
  nb1_phy_cell_id_t nb1_whitelist[NB1_CPHY_MEAS_MAX_CELL_INTER];

  /*! Blacklisted cells */
  uint8 blacklist_num_cells;        /*!< 0 indicates parameter is absent */
  //! @todo: Remove nb1_ once NB1 param is removed
  nb1_phy_cell_id_t nb1_blacklist[NB1_CPHY_MEAS_MAX_CELL_BLACK];

  boolean inter_freq_q_qualmin_present;
    /*! q_QualMin_r13 (-34..-3) . If not signaled, default
    NB1_CPHY_INT8_NEG_INFINITY will be passed */
  int8 q_qual_min_r13;

  /*! Enum to indicate what's kind of the power class for this UE. */
  nb1_cphy_power_class_e power_class_cfg;

  /* powerClass14dBm-Offset
     Parameter "Poffset" in TS 36.304. Only applicable for UE supporting powerClassNB-14dBm(NB1_CPHY_POWER_CLASS6). Value in dB.
     If the fied is absent, the UE applies the (default) value of 0 dB for "Poffset".
  */
  int8 Poffset;
} nb1_cphy_meas_inter_freq_resel_freq_info_s;

/*! @brief Inter-freq cell reselection list
*/
typedef struct
{
  /*! Cell reselection timer */
  nb1_t_reselection_e t_resel_eutran;

  /*! Number of configured measurement frequencies */
  uint8 num_freq;

  /*! List of frequency specific measurement configuration */
  nb1_cphy_meas_inter_freq_resel_freq_info_s freq_list[NB1_CPHY_MEAS_MAX_NUM_FREQ];

} nb1_cphy_meas_inter_freq_resel_info_s;

/*! @brief GERAN frequency group.

  This defines a group of GERAN frequencies with associated priority and
  reselection parameters.  This GERAN frequency group is analogous to
  a frequency in another RAT.

  Defined in 36.331 Section 6.4
  */
typedef struct
{
  /*! The priority of this frequency group */
  nb1_l1_resel_priority_t  cell_resel_priority;

  /*! Reselection threshold for high priority layers.
    */
  nb1_l1_resel_threshold_t thresh_gsm_high;
  /*! Reselection threshold for low priority layers.
    */
  nb1_l1_resel_threshold_t thresh_gsm_low;

  /*! Parameter Q_rxlevmin as defined in 45.008.

    This is the actual Q_rxlev_min value in Dbm.

    Needs to be converted from the IE value defined in 3GPP 36.331
    Section 6.3.1
    */
  int8 q_rxlev_min;

  /*! Maximum allowed transmission power for GERAN on an uplink carrier
    frequency as defined in 45.008.

    Value in dBm. Applicable for the neighbouring GERAN cells on this carrier
    frequency.

    Range 0..39

    */
  int8 p_max;

  /*! GSM band flag identifying ARFCNs as specified in 3GPP 45.005.

    Allows UE to differentiate between GSM DCS 1800 and PCS 1900 frequencies
    which have overlapping ARFCN values.

    This flag does not specify the band for all ARFCNs in this group, but
    only serves as a differentiator for overlapping ARFCN values.  Must
    be set to either NB1_L1_GSM_BAND_DCS_1800 or NB1_L1_GSM_BAND_PCS_1900
    and is only relevant to ARFCN values which fall in either of those bands.
   */
  nb1_l1_gsm_band_e gsm_band;

  /*!  Bitmask specifying whether a neighbour with a particular
    Network Color Code is to be reported.  Bit "n" set to 1 means
    that a neighbour with NCC "n" should be included in the report.

    This flag is synonymous with a blacklist in other RATs
   */
  uint8 ncc_permitted;

} nb1_cphy_meas_irat_gsm_freq_group_s;


/*! @brief GERAN Frequencies.

  This flattened structure of GSM frequencies is used to minimize space.

  */
typedef struct
{
  /*! The frequency group that this frequency is associated with.
     This is the index into the frequency group array in the GERAN configuration
   */
  uint16 freq_group_index;

  /*! Array of gsm frequencies to measure. */
  nb1_l1_gsm_arfcn_t freq;

} nb1_cphy_meas_irat_gsm_freq_s;

/*!
  @brief UE mode types
*/
typedef enum
{
  /*! UE mode is CSFB */
  NB1_CPHY_UE_MODE_DEFAULT,

  /*! UE mode is SGLTE */
  NB1_CPHY_UE_MODE_SGLTE,

  /*! UE mode is 1xSrnb1 */
  NB1_CPHY_UE_MODE_1XSRLTE,

  /*! UE mode is SVLTE */
  NB1_CPHY_UE_MODE_SVLTE,

  /*! UE mode is SGLTE but GSM is inactive during VOLTE
      ML1 must map this as default, and RRC as SGLTE */
  NB1_CPHY_UE_MODE_SGLTE_INACTIVE

} nb1_cphy_ue_mode_type_e;

/*==========================================================================
  Coex DSDA conflict enum
===========================================================================*/
typedef enum
{
  /*! No conflict need for this device/ue*/
  NB1_ML1_COEX_DSDA_NO_CONFLICT_NEEDED = 0,
  /*! TX_backoff is needed. It's SGLTE or SGLTE+G*/
  NB1_ML1_COEX_DSDA_TX_BACKOFF_NEEDED,
  /*! Blanking is needed. It's NON SGLTE or SGLTE+G with DSDA*/
  NB1_ML1_COEX_DSDA_BLANKING_NEEDED
}nb1_ml1_coex_dsda_conflict_type_e;
/*! @brief Idle Mode GERAN measurement configuration structure.

  This structure defines the idle-mode reselection and measurement
  parameters to be used by ML1.  Any new configuration will be applied
  at the next paging occasion.

  */
typedef struct
{

  /*! UE mode to configure */
  nb1_cphy_ue_mode_type_e ue_mode;

  /*! T_Reselection timer dictating how long a given cell's receive power
    exceeds a reselection threshold prior to reselection.

    See 3GPP 36.304 5.2.4.5 for usage details.
    */
  uint8 t_reselection;

  /*! The speed scaling factor to be applied while in medium mobility
    state as defined in 3GPP 36.304 5.2.4.5
    */
  nb1_cphy_meas_time_sf_e sf_medium;
  /*! The speed scaling factor to be applied while in high mobility
    state as defined in 3GPP 36.304 5.2.4.5
    */
  nb1_cphy_meas_time_sf_e sf_high;

  /*! The maximum ue transmit power as defined as MS_TXPWR_MAX_CCH  in
    3GPP 45.008.

    Value in dBm.
    */
  int8 max_ue_gsm_transmit_power;

  /*! The number of GSM frequency groups corresponding to the number of
    frequency groups specified in the freq_groups array. */
  uint8 num_frequency_groups;

  /*! Array of frequency groups */
  nb1_cphy_meas_irat_gsm_freq_group_s freq_groups[NB1_CPHY_MEAS_MAX_GERAN_FREQ_GROUPS];

  /*! The number of frequencies being configured.
    Range 0 .. NB1_CPHY_MEAS_MAX_GERAN_FREQS_IN_GROUP */
  uint8 num_freqs;

  /*! The GSM frequencies to be configured in L1 */
  nb1_cphy_meas_irat_gsm_freq_s freqs[NB1_CPHY_MEAS_MAX_GERAN_FREQ];
} nb1_cphy_meas_irat_gsm_info_s;

/*! @brief typedef for CDMA system time type: uint64
*/
typedef uint64 nb1_cphy_irat_cdma_system_time_t;

/*! @brief
  Structure with the CDMA system time type: uint64
*/
typedef struct
{
  /*! CDMA system time */
  nb1_cphy_irat_cdma_system_time_t system_time;

} nb1_cphy_irat_cdma_system_time_s;

/*! @brief enumeration to identify the CDMA system time type
*/
typedef enum
{
  NB1_CPHY_IRAT_CDMA_SYS_TIME_SYNCHRONOUS_TYPE, /*!<  Synchronous system time */
  NB1_CPHY_IRAT_CDMA_SYS_TIME_ASYNCHRONOUS_TYPE /*!<  Asynchronous system time */

} nb1_cphy_irat_cdma_system_time_type_e;

/*! @brief enumeration to identify the dayLightSavingTime type
*/
typedef enum
{
  NB1_CPHY_DAYLIGHT_NONE,     /*!< NO adjustment needed*/
  NB1_CPHY_DAYLIGHT_PLUS_ONE, /*!< +1 hour adjustment*/
  NB1_CPHY_DAYLIGHT_PLUS_TWO, /*!< +2 hour adjustment*/
  NB1_CPHY_DAYLIGHT_RESERVED  /*!< reserved */

}nb1_cphy_utc_dayLightSaving_type_e;



/*! @brief enumeration to identify BPLMN request type
*/
typedef enum
{
  /*! UE active on NB1, PLMN Search in NB1 */
  NB1_CPHY_BPLMN_TYPE_NB1_NB1,

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
  /*! WWAN SCAN NEW TYPW will be used */
  NB1_CPHY_BPLMN_TYPE_WWAN_SCAN,
#endif
} nb1_cphy_bplmn_type_e;

/*! @brief enumeration to indicate if the cell is barred,
           the frequency is barred, or can be regularily
           used
*/
typedef enum
{
  NB1_CPHY_BPLMN_NOT_BARRED,
  NB1_CPHY_BPLMN_CELL_BARRED,
  NB1_CPHY_BPLMN_FREQ_BARRED
} nb1_cphy_bplmn_cell_barred_type_e;

/*! @brief enumeration to BPLMN complete type */
typedef enum
{
  NB1_CPHY_BPLMN_PARTIAL_DONE,
  NB1_CPHY_BPLMN_ALL_DONE,
  NB1_CPHY_BPLMN_FAIL
} nb1_cphy_bplmn_complete_type_e;

/*! @brief enumeration to BPLMN priority type */
typedef enum
{
  NB1_CPHY_BPLMN_PRIORITY_HIGH, /* The default value; BPLMN search should be prioritised
                                   over other procedures */
  NB1_CPHY_BPLMN_PRIORITY_LOW /* Other procedures can be prioritised over BPLMN search */
} nb1_cphy_bplmn_priority_e;

/*! @brief enum of CDMA system time status states */
typedef enum
{
  NB1_CPHY_IRAT_CDMA_SYSTEM_TIME_SOURCE_SIB8,
  NB1_CPHY_IRAT_CDMA_SYSTEM_TIME_SOURCE_RTT,
} nb1_cphy_irat_cdma_system_time_source_type_e;

/*! @brief union of synchronous and asynchronous system time
*/
typedef union
{
  /*! Synchronous system time */
  nb1_cphy_irat_cdma_system_time_s cdma_synchronous_system_time;

  /*! Asynchronous system time */
  nb1_cphy_irat_cdma_system_time_s cdma_asynchronous_system_time;

} nb1_cphy_cdma_system_time_u;


/*! @brief CDMA system time info structure
*/
typedef struct
{
  /*! CDMA system time type - synchronous or asynchronous, indicates if CDMA2000
    and EUTRA networks are in sync, i.e there is no drift between their timing */
  nb1_cphy_irat_cdma_system_time_type_e cdma_system_time_type;

  /*! CDMA system time */
  nb1_cphy_cdma_system_time_u cdma_system_time;

  /*! SFN boundary at or after the ending boundary of the SI-window in which
    SIB8 is transmitted */
  nb1_l1_sfn_t sib8_sfn_boundary;

} nb1_cphy_irat_cdma_sib8_system_time_info_s;

/*! @brief UTC time info structure
*/
typedef struct
{
  /*! UTC system time indicating the integer count of 10 ms units
   since 00:00:00 on 1 January, 1900*/
  uint64 utc_system_time;

  /*! SFN boundary at or after the ending boundary of the SI-window in which
    SIB16 is transmitted */
  nb1_l1_sfn_t sfn_boundary;

  /*! If UTC dayLightSavingTime is enabled - TRUE
  ELSE - ML1 should delete any values configured*/
  boolean utc_dayLightSavingTime_enabled;

  /*! UTC dayLightSavingTime Application*/
  nb1_cphy_utc_dayLightSaving_type_e dayLightSavingTime;

  /*! If UTC leapSeconds is enabled - TRUE
  ELSE - ML1 should delete any values configured*/
  boolean utc_leapSeconds_enabled;

  /*! UTC leapSeconds in units of seconds */
  int16 leapSeconds;

  /*! If UTC localTimeOffset is enabled - TRUE
  ELSE - ML1 should delete any values configured*/
  boolean utc_localTimeOffset_enabled;

  /*! UTC localTimeOffset in units of seconds */
  int16 localTimeOffset;

} nb1_cphy_utc_time_info_s;

/*! NB1 STMR value - 22 bit value */
typedef uint32 nb1_stmr_type;

typedef struct
{
  /*! NB1 STMR value - 22 bit value */
  nb1_stmr_type                    nb1_ostmr;
  /*! CDMA system time in CDMA chip 1.2288MHZ - 64 bit value */
  nb1_cphy_irat_cdma_system_time_t cdma_sys_time;
}nb1_stmr_cdma_rtc_pair;

/*! @brief Reverse CDMA system time status enum */
typedef enum
{
  NB1_CPHY_IRAT_CDMA_REVERSE_SYSTEM_TIME_SUCCESS,
  NB1_CPHY_IRAT_CDMA_REVERSE_SYSTEM_TIME_FAILURE,

} nb1_cphy_irat_cdma_reverse_system_time_status_e;

/*! @brief CDMA system time info structure
*/
typedef struct
{
  /*! Reverse time transfer status */
  nb1_cphy_irat_cdma_reverse_system_time_status_e status;

  /*! RAT type */
  nb1_rat_e rat_type;

  /*! CDMA time information */
  nb1_stmr_cdma_rtc_pair cdma_time_info;

  /*! CDMA layer info */
  sys_channel_type channel;

} nb1_cphy_irat_cdma_rtt_system_time_info_s;

/*! @brief CDMA system time info union
*/
typedef union
{
  /*! SIB8 provided CDMA system time */
  nb1_cphy_irat_cdma_sib8_system_time_info_s sib8;

  /*! Reverse time transfer provided CDMA system time */
  nb1_cphy_irat_cdma_rtt_system_time_info_s rtt;

} nb1_cphy_irat_cdma_system_time_info_u;

/*! @brief CDMA2000 band class info structure
*/
typedef struct
{
  /*! CDMA2000 band class, as per 36.331 section 6.3.4, the valid band classes
    are bc0 to bc17 */
  sys_band_class_e_type band_class;

  /*! Srxlev-value of an evaluated cell must be greater than these values to
  be considered for reselection */
  nb1_l1_resel_threshold_t threshX_low;
  nb1_l1_resel_threshold_t threshX_high;

  /*! Cell reselection priority */
  nb1_l1_resel_priority_t cell_resel_priority;

} nb1_cphy_meas_irat_cdma_band_class_info_s;

/*! @brief CDMA2000 band channel info structure
*/
typedef struct
{
  /*! CDMA2000 band class, as per 36.331 section 6.3.4, the valid band classes
    are bc0 to bc17 */
  sys_band_class_e_type band_class;

  /*! CDMA channel num */
  sys_channel_num_type channel;

  /*! CDMA Cell id of the neighbor cell */
  sys_pilot_pn_type cell_id;

} nb1_cphy_meas_irat_cdma_neigh_cell_info_s;

/*! @brief CDMA Reselection info structure
*/
typedef struct
{

  /*! Add num of band classes in the list below */
  uint8 band_class_list_size;

  /*! List of CDMA band classes */
  nb1_cphy_meas_irat_cdma_band_class_info_s band_class_list[NB1_CPHY_MEAS_MAX_CDMA_BAND_CLASS];

  /*! num of CDMA neighbor cell info elements in the list below */
  uint8 neigh_cell_info_list_size;

  /*! List of CDMA neighbor cells */
  nb1_cphy_meas_irat_cdma_neigh_cell_info_s   neigh_cell_info_list[NB1_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS_REL9];

  /*! Cell reselection timer */
  nb1_l1_treselection_t t_resel_cdma;

  /*! t-ReselectionCDMA-SF-Medium, default value is NB1_CPHY_MEAS_TIME_SF_1DOT0
    when not signaled. There is no scaling for medium / high speeds when using
    the default value */
  nb1_cphy_meas_time_sf_e t_resel_cdma_medium;

  /*! t-ReselectionCDMA-SF-High, default value is NB1_CPHY_MEAS_TIME_SF_1DOT0
    when not signaled. There is no scaling for medium / high speeds when using
    the default value */
  nb1_cphy_meas_time_sf_e t_resel_cdma_high;

} nb1_cphy_meas_irat_cdma_resel_param_s;

/*! @brief CDMA info structure for eHRPD and 1x
*/
typedef struct
{
  /*! If CDMA search_window_size is enabled */
  boolean irat_cdma_search_window_size_enabled;

  /*! CDMA search_window_size */
  uint8 irat_cdma_search_window_size;

  /*! If eHRPD reselection is enabled */
  boolean irat_cdma_ehrpd_resel_enabled;

  /*! eHRPD reselection parameters */
  nb1_cphy_meas_irat_cdma_resel_param_s irat_cdma_ehrpd_resel_param;

  /*! If 1x reselection is enabled */
  boolean irat_cdma_1x_resel_enabled;

  /*! 1x reselection parameters */
  nb1_cphy_meas_irat_cdma_resel_param_s irat_cdma_1x_resel_param;

} nb1_cphy_meas_irat_cdma_info_s;

/*! @brief
Idle mode inter-rat per WCDMA frequency information
*/
typedef struct
{
  /*! UTRAN frequency 0..16383 */
  nb1_uarfcn_t freq;

  /*! Cell reselection priority 0..7*/
  nb1_l1_resel_priority_t  cell_resel_priority;

  /*! Reselection threshold*/
  nb1_l1_resel_threshold_t thresh_Xhigh;

  /*! Reselection threshold*/
  nb1_l1_resel_threshold_t thresh_Xlow;

  /*! if Squal thresholds are passed; For Rel 8 this boolean will always be set
    to FALSE. For Rel 9 the field will be set to TRUE if threshX-Q-r9 is siganled */
  boolean thresh_rel9_present;

  /*!Actual value = IE value * 2+1. Specifies the minimum required Rx level
  in the cell expressedin dBm. Minimum required RX level in the cell.
  Defined in 25.304  Values in the range -60..-13*/
  int8 q_rx_lev_min_actual;

  /*! The maximum allowed transmission power on the (uplink) carrier
  frequency in dBm. Defined in 25.304. Values in the range -50..33 */
  int8 p_max;

  /*! Minimum required quality level in the cell (dB). Defined in 25.304
  Values in the range -24..0*/
  int8 q_qual_min;

}nb1_cphy_meas_wcdma_freq_list_s;


/*! @brief
Idle mode inter rat WCDMA measurement config paramaeters
*/
typedef struct
{
  /*! Cell reselection timer value. This is time during which
  if the new cell is better ranked than the serving cell prior
  to cell reselection evaluation
  */
  nb1_l1_treselection_t   t_resel;

  /*! When medium mobility state is detected
  t-Reselection-UTRA should be multiplied by SF-Medium and rounded off
  to the nearest second. Sec 5.2.4.3.1 36.304*/
  nb1_cphy_meas_time_sf_e sf_medium;

  /*! When high mobility state is detected
  t-Reselection-UTRA should be multiplied by SF-high and rounded off
  to the nearest second. Sec 5.2.4.3.1 36.304*/
  nb1_cphy_meas_time_sf_e sf_high;

  /*! Number of UTRA neighbours Can Range from 0...16 */
  uint8 num_ngbr_freq;

  /*!Information pertaining to each utra fdd frequency needed for the
  measurements*/
  nb1_cphy_meas_wcdma_freq_list_s  fdd_freq_list[NB1_CPHY_MEAS_MAX_WCDMA_FREQ];
}nb1_cphy_meas_irat_wcdma_resel_info_s;

/*! @brief
Idle mode inter-rat per TDSCDMA frequency information
*/
typedef struct
{
  /*! UTRAN frequency 0..16383 */
  nb1_uarfcn_t freq;

  /*! Cell reselection priority 0..7*/
  nb1_l1_resel_priority_t  cell_resel_priority;

  /*! Reselection threshold*/
  nb1_l1_resel_threshold_t thresh_Xhigh;

  /*! Reselection threshold*/
  nb1_l1_resel_threshold_t thresh_Xlow;

  /*!Actual value = IE value * 2+1. Specifies the minimum required Rx level
  in the cell expressedin dBm. Minimum required RX level in the cell.
  Defined in 25.304  Values in the range -60..-13*/
  int8 q_rx_lev_min_actual;

  /*! The maximum allowed transmission power on the (uplink) carrier
  frequency in dBm. Defined in 25.304. Values in the range -50..33 */
  int8 p_max;

  /*! Boolean to indicate if UARFCN is from TDS ACQ DB or not*/
  boolean freq_in_tds_acq_db;

}nb1_cphy_meas_tdscdma_freq_list_s;


/*! @brief
Idle mode inter rat TDSCDMA measurement config paramaeters
*/
typedef struct
{
  /*! Cell reselection timer value. This is time during which
  if the new cell is better ranked than the serving cell prior
  to cell reselection evaluation
  */
  nb1_l1_treselection_t   t_resel;

  /*! When medium mobility state is detected
  t-Reselection-UTRA should be multiplied by SF-Medium and rounded off
  to the nearest second. Sec 5.2.4.3.1 36.304*/
  nb1_cphy_meas_time_sf_e sf_medium;

  /*! When high mobility state is detected
  t-Reselection-UTRA should be multiplied by SF-high and rounded off
  to the nearest second. Sec 5.2.4.3.1 36.304*/
  nb1_cphy_meas_time_sf_e sf_high;

  /*! Number of UTRA neighbours Can Range from 0...16 */
  uint8 num_ngbr_freq;

  /*! Number of UTRA neighbours in active list
         For TDS supported network, the range can be 1 to 6
           So Active list ngbr entries will be tdd_freq_list[0] .. tdd_freq_list[5]
           and remaining ngbr entires will be dormant list
         For Non TDS supported network, num_active_list_ngbr_freq =
           num_ngbr_freq (i.e. no dormant list)*/
  uint8 num_active_list_ngbr_freq;

  /*!Information pertaining to each utra tdd frequency needed for the
  measurements*/
  nb1_cphy_meas_tdscdma_freq_list_s  tdd_freq_list[NB1_CPHY_MEAS_MAX_TDSCDMA_FREQ];
}nb1_cphy_meas_irat_tdscdma_resel_info_s;


/*! WCDMA cell information */
typedef struct
{
  /*! Scrambling code of the resel candidate cell */
  uint16                     psc;

  /*! Whether timing and measurements are present.
    if FALSE, only the PSC is valid */
  boolean                    timing_present;

  /*! PN position In units of chipx1 Range 0...38400 */
  uint32                     pn_pos;

  /*! 2x the ecno for the cell */
  int16                      ecno_2x;
  /*! The RSCP for the cell */
  int16                      rscp;

  /*!Actual value = IE value * 2+1. Specifies the minimum required Rx level
  in the cell expressedin dBm. Minimum required RX level in the cell.
  Defined in 25.304  Values in the range -60..-13*/
  int8 q_rx_lev_min_actual;

  /*! The maximum allowed transmission power on the (uplink) carrier
  frequency in dBm. Defined in 25.304. Values in the range -50..33 */
  int8 p_max;

  /*! Minimum required quality level in the cell (dB). Defined in 25.304
  Values in the range -24..0*/
  int8 q_qual_min;

} nb1_cphy_meas_csg_w_cell_info_s;

/*! @brief
  WCDMA CSG information
 */
typedef struct
{
  /*! Frequency */
  nb1_uarfcn_t uarfcn;

  /*! Number of cells's */
  uint8 num_cells;

  /*! List of PSCs for CSG cells */
  nb1_cphy_meas_csg_w_cell_info_s cell[NB1_CPHY_MEAS_NUM_WCDMA_CSG_CELLS];
}nb1_cphy_meas_csg_wcdma_freq_info_s;

/*!
  @brief
  Idle meas config CSG info

  */
typedef struct
{
  /*! Number of frequencies in freq array */
  uint8 num_w_freq;

  nb1_cphy_meas_csg_wcdma_freq_info_s wcdma_freq_info[NB1_CPHY_MEAS_NUM_WCDMA_CSG_FREQS];

} nb1_cphy_meas_csg_info_s;


/*! @brief
  NB1 CSG information
 */
typedef struct
{
  /*! Frequency */
  nb1_earfcn_t earfcn;

  /*! Number of cells's */
  uint8 num_cells;

  /*! List of PCIs for CSG cells */
  nb1_phy_cell_id_t pci[NB1_CPHY_MEAS_NUM_NB1_CSG_CELLS];

  /*! CSG PCI range */
  nb1_phy_cell_id_and_range_s csg_pci_range;

}nb1_cphy_meas_csg_nb1_freq_info_s;

/*!
  @brief
  Idle meas config NB1 CSG info

  */
typedef struct
{
  /*! Number of frequencies in freq array */
  uint8 num_l_freq;

  nb1_cphy_meas_csg_nb1_freq_info_s nb1_freq_info[NB1_CPHY_MEAS_NUM_NB1_CSG_FREQS];

} nb1_cphy_meas_nb1_csg_info_s;

/*! @brief
  Idle mode measurement config parameters.
*/
typedef struct
{
  /*! Bitmask to indicate which meas config has been requested in the current message,
      bit masks defined above, see NB1_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON */
  nb1_cphy_idle_meas_cfg_mask_t requested_cfg_bitmask;

  /* ! Bitmask to indicate which meas configs have been disbaled,
     meas config bitmask that has been disabled should not be included in the
     requested_cfg_bitmask */
  nb1_cphy_idle_meas_cfg_mask_t disabled_cfg_bitmask;

  /*! Common reselection configuration */
  nb1_cphy_meas_cell_resel_common_s *resel_common_info_ptr;

  /*! Reselection configuration for serving freq */
  nb1_cphy_meas_serving_freq_resel_info_s *serving_freq_info_ptr;

  /*! Configuration for intra-freq measurements */
  nb1_cphy_meas_intra_freq_resel_info_s *intra_freq_info_ptr;

  /*! Configuration for inter-freq measurements */
  nb1_cphy_meas_inter_freq_resel_info_s *inter_freq_info_ptr;
} nb1_cphy_idle_meas_cfg_info_s;


/******************************************************************************
                Connected-mode Measurement Configuration
*******************************************************************************/

/*! @brief
  Maximum number of measurement object ids
  Defined in 36.331 Section 6.4
*/
#define NB1_CPHY_MEAS_MAX_MEAS_OBJ_ID 32

/*! @brief
  Maximum number of measurement ids
  Defined in 36.331 Section 6.4
*/
#define NB1_CPHY_MEAS_MAX_MEAS_ID 32

/*! @brief
  Maximum number of measurement reporting config ids
  Defined in 36.331 Section 6.4
*/
#define NB1_CPHY_MEAS_MAX_REPORT_CFG_ID 32

/*! @brief Measurement ID type. Range 1..32
*/
typedef uint8 nb1_cphy_meas_id_t;

/*! @brief Measurement object ID type. Range 1..32
*/
typedef uint8 nb1_cphy_meas_obj_id_t;

/*! @brief Reporting configuration ID type. Range 1..32
*/
typedef uint8 nb1_cphy_report_cfg_id_t;


/*! @brief Measurement ID config (links meas object to report config)
*/
typedef struct
{
  /*! Measurement object ID. Range 1..32 */
  nb1_cphy_meas_obj_id_t meas_obj_id;

  /*! Reporting config ID. Range 1..32 */
  nb1_cphy_report_cfg_id_t report_cfg_id;

} nb1_cphy_meas_id_s;


/*! @brief Measurement configuration for a whitelisted EUTRA cell
*/
typedef struct
{
  /*! Physical cell identity */
  nb1_phy_cell_id_t pci;

  /*! Cell specific offset (in +/- dB). Range -24..24 */
  int16 cell_offset;

} nb1_cphy_meas_eutra_neigh_cell_s;


/*! @brief List of whitelisted EUTRA neighbour cells
*/
typedef struct
{
  /*! Indicates which elements of cell_info list are active. A '1' indicates
      active; e.g. bit0 = 1 indicates element 0 active. */
  uint32 active_bitmask;

  /*! List of whitelisted cells (with their cell-specific offsets) */
  nb1_cphy_meas_eutra_neigh_cell_s cell_info[NB1_CPHY_MEAS_MAX_MEAS_OBJ_CELLS];

} nb1_cphy_meas_eutra_neigh_cell_list_s;


/*! @brief Measurement configuration for a blacklisted EUTRA cell
*/
typedef struct
{
  /*! Physical cell identity or range */
  nb1_phy_cell_id_and_range_s pci_range;

} nb1_cphy_meas_eutra_blacklisted_cell_s;


/*! @brief List of blacklisted EUTRA neighbour cells
*/
typedef struct
{
  /*! Indicates which elements of cell_info list are active. A '1' indicates
      active; e.g. bit0 = 1 indicates element 0 active. */
  uint32 active_bitmask;

  /*! List of blacklisted cells */
  nb1_cphy_meas_eutra_blacklisted_cell_s cell_info[NB1_CPHY_MEAS_MAX_MEAS_OBJ_CELLS];

} nb1_cphy_meas_eutra_blacklisted_cell_list_s;


/*! @brief Measurement object for EUTRA frequencies
*/
typedef struct
{
  /*! Measurement carrier freq */
  nb1_earfcn_t carrier_freq;

  /*! Bandwidth to restrict measurements on */
  nb1_cphy_meas_bandwidth_e meas_bandwidth;

  /*! Flag indicates if all the neighboring cells use antenna port 1 */
  boolean presence_antenna_port1;

  /*! neighCellConfig : Provides information related to MBSFN and TDD UL/DL configuration
      of neighbour cells of this frequency
      00: Not all neighbour cells have the same MBSFN subframe allocation as serving cell
      10: The MBSFN subframe allocations of all neighbour cells are identical to or subsets
          of that in the serving cell
      01: No MBSFN subframes are present in all neighbour cells
      11: Different UL/DL allocation in neighbouring cells for TDD compared to the serving cell
      For TDD, 00, 10 and 01 are only used for same UL/DL allocation in neighbouring cells
      compared to the serving cell.
  */
  uint8 neighbourCellConfig;

  /*! Freq specific offset (in +/- dB). Range -24..24 */
  int16 freq_offset;

  /*! Whitelisted cells */
  nb1_cphy_meas_eutra_neigh_cell_list_s neigh_cells;

  /*! Blacklisted cells */
  nb1_cphy_meas_eutra_blacklisted_cell_list_s blacklisted_cells;

  /*! Flag to indicate if reportCGI cell is included in the measObj */
  boolean report_cgi_cell_present;

  /*! Cell for which CGI needs to be reported */
  nb1_phy_cell_id_t report_cgi_cell_id;

  /*!Parameter: Tmeasure_scc See TS 36.133 [16, 8.3.3]. The parameter is used only when an SCell is
    configured on the frequency indicated by the measObject and is in deactivated state, but the field may
    also be signalled when an SCell is not configured. Value 160 corresponds to 160 sub-frames, 256
    corresponds to 256 sub-frames and so on.*/
  boolean meas_cycle_scell_present;
  uint32 measCycleSCell;

  /*! Flag to indicate if gaps are needed for this measObj based on CA band combo*/
  boolean gaps_needed;
} nb1_cphy_meas_obj_eutra_s;


/*! @brief cdma2000 type enum
*/
typedef enum
{
  /*! CDMA 2000 1xRTT */
  NB1_CPHY_CDMA_OBJTYPE_1X = 0,

  /*! CDMA 2000 HRPD */
  NB1_CPHY_CDMA_OBJTYPE_HRPD,

  /*! MAX number of the CDMA 2000 type */
  NB1_CPHY_CDMA_MAX_NUM_OBJTYPES
} nb1_cphy_meas_cdma_type_e;

/*! @brief List of active cdma2000 neighbour cells
*/
typedef struct
{
  /*! Indicates which elements of cell_info list are active. A '1' indicates
      active; e.g. bit0 = 1 indicates element 0 active. */
  uint32 active_bitmask;

  /*! List of pilot pn of neighbor cells */
  sys_pilot_pn_type phy_cell_id[NB1_CPHY_MEAS_MAX_MEAS_OBJ_CELLS];
} nb1_cphy_meas_cdma_nbr_cell_list_s;


/*! @brief Measurement object for HRPD frequencies
*/
typedef struct
{
  /*! CDMA 2000 type */
  nb1_cphy_meas_cdma_type_e  cdma_type;

  /*! Measurement carrier freq include band and channel number */
  sys_channel_type  channel;

  /*! CDMA search_window_size */
  uint8 search_window_size;

  /*! Freq specific offset (in +/- dB). Range -15..15 */
  int16 freq_offset;

  /*! To remove cell list */
  nb1_cphy_meas_cdma_nbr_cell_list_s nbr_cells;

  /*! Flag indicating if the cgi cell id is included */
  boolean cgi_cell_included;

  /*! Cell for which CGI needs to be reported */
  nb1_phy_cell_id_t report_cgi_cell_id;
} nb1_cphy_meas_obj_cdma_s;

/*! @brief GERAN Phy Cell id structure
*/
typedef struct
{
  /* PhyCellIDGERAN Network Colour Code */
  uint8 ncc;

  /* PhyCellIDGERAN BaseStation Colour Code */
  uint8 bcc;

} nb1_cphy_geran_phy_cell_id_s;


/*! @brief Measurement object for GERAN frequency groups
*/
typedef struct
{
  /*! UE mode to configure */
  nb1_cphy_ue_mode_type_e ue_mode;

  /*! The number of frequencies in freqs */
  uint32 num_freqs;

  /*! Array of gsm frequencies to measure. */
  nb1_l1_gsm_arfcn_t freqs[NB1_CPHY_MEAS_MAX_GERAN_FREQS_IN_GROUP];

  /*! GSM band flag identifying ARFCNs as specified in 3GPP 45.005.

    Allows UE to differentiate between GSM DCS 1800 and PCS 1900 frequencies
    which have overlapping ARFCN values.

    This flag does not specify the band for all ARFCNs in this group, but
    only serves as a differentiator for overlapping ARFCN values.  Must
    be set to either NB1_L1_GSM_BAND_DCS_1800 or NB1_L1_GSM_BAND_PCS_1900
    and is only relevant to ARFCN values which fall in either of those bands.
   */
  nb1_l1_gsm_band_e gsm_band;

  /*! The Q-offset for this measurement report.  This offset
    is to be applied to the measurements prior to reporting */
  int16 q_offset;

  /*!  Bitmask specifying whether a neighbour with a particular
    Network Color Code is to be reported.  Bit "n" set to 1 means
    that a neighbour with NCC "n" should be included in the report.

    This flag is synonymous with a blacklist in other RATs
   */
  uint8 ncc_permitted;

  /*! Flag to indicate if reportCGI cell is included in the measObj */
  boolean report_cgi_cell_present;

  /* PhyCellIDGERAN of the CGI cell */
  nb1_cphy_geran_phy_cell_id_s cgi_phy_cell_id_geran;

} nb1_cphy_meas_obj_gsm_s;

typedef enum
{
  NB1_CPHY_MEAS_OBJ_WCDMA_TYPE_FDD = 0,
    NB1_CPHY_MEAS_OBJ_WCDMA_TYPE_TDD,
  NB1_CPHY_MEAS_OBJ_WCDMA_TYPE_MAX
}nb1_cphy_meas_obj_wcdma_type_e;

/*! @brief Measurement object for WCDMA frequency
*/
typedef struct
{
  /* Whether all information below corresponds to
  fdd mode or tdd mode
    If W supported, then Mode will be set to FDD
    If T supported then Mode will be set to TDD
    if Both supported then Mode will be determined based on camped PLMN*/
  nb1_cphy_meas_obj_wcdma_type_e wcdma_mode;

  /*! wcdma frequency to measure. */
  nb1_uarfcn_t  freq;

  /*! The Q-offset for this measurement report.  This offset
    is to be applied to the measurements prior to reporting
    default - 0
    range -15..15
  */
  int16 q_offset;

  /*! Bitmask which indicates which cell indices are active. Bit 0
      corresponds to cell index 1, ..., bit 31 corresponds to cell index 32.
      implicitly it will be determined which ones were deleted
  */
  uint32 active_psc_bitmask;

  /*! cell info, mapping of the cell index & physical cell id
  Only the active cell indices will have valid psc in the range 0..511
  */

  uint16  psc[NB1_CPHY_MEAS_MAX_MEAS_OBJ_CELLS];

  /*! Flag to indicate if reportCGI cell is included in the measObj */
  boolean report_cgi_cell_present;

  /*! Physical cell id of the cell for which to report the cgi*/
  uint16  psc_for_cgi_report;

}nb1_cphy_meas_obj_wcdma_s;

/*! @brief Enumeration of types of measurement objects
*/
typedef enum
{
  /*! EUTRA */
  NB1_CPHY_MEAS_OBJ_TYPE_EUTRA,

  /*! WCDMA */
  NB1_CPHY_MEAS_OBJ_TYPE_WCDMA,

  /*! GERAN */
  NB1_CPHY_MEAS_OBJ_TYPE_GERAN,

  /*! CDMA */
  NB1_CPHY_MEAS_OBJ_TYPE_CDMA,

} nb1_cphy_meas_obj_type_e;

/*! @brief Union of different types of measurement objects */
typedef union
{
  /*! EUTRA measurement object */
  nb1_cphy_meas_obj_eutra_s eutra;

  /*! WCDMA measurement object*/
  nb1_cphy_meas_obj_wcdma_s wcdma;

  /*! GSM Measurement Object */
  nb1_cphy_meas_obj_gsm_s   gsm;


  /*! HRPD Measurement Object */
  nb1_cphy_meas_obj_cdma_s  cdma;

} nb1_cphy_meas_obj_info_u;


/*! @brief Measurement Object
*/
typedef struct
{
  /*! Type of measurement object */
  nb1_cphy_meas_obj_type_e type;

  /*! Union of different types of measurement objects */
  nb1_cphy_meas_obj_info_u        obj_info;
} nb1_cphy_meas_obj_s;



/*! @brief Enumeration of different trigger types for report configs
*/
typedef enum
{
  /*! a1 */
  NB1_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A1,

  /*! a2 */
  NB1_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A2,

  /*! a3 */
  NB1_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A3,

  /*! a4 */
  NB1_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A4,

  /*! a5 */
  NB1_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A5,

  /*! a6 */
  NB1_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_A6,

  /*! b1 */
  NB1_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_B1,

  /*! b2 */
  NB1_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_B2,

  /*! periodic */
  NB1_CPHY_MEAS_REPORT_CFG_TRIG_TYPE_PERIODIC,

} nb1_cphy_meas_report_cfg_trig_type_e;


/*! @brief Enumeration of inter-rat measurement threshold types
*/
typedef enum
{
  /*! CDMA */
  NB1_CPHY_MEAS_IRAT_THRESH_TYPE_CDMA,

  /*! GERAN */
  NB1_CPHY_MEAS_IRAT_THRESH_TYPE_GERAN,

  /*! WCDMA RSCP */
  NB1_CPHY_MEAS_IRAT_THRESH_TYPE_WCDMA_RSCP,

  /*! WCDMA EcNO */
  NB1_CPHY_MEAS_IRAT_THRESH_TYPE_WCDMA_ECNO,

} nb1_cphy_meas_irat_thresh_type_e;


/*! @brief Defines a measurement threshold for IRAT measurements
*/
typedef struct
{
  /*! Threshold type */
  nb1_cphy_meas_irat_thresh_type_e type;

  /*! Measurement threshold.
      For CDMA2000/GERAN threshold, range 0..63
      For WCDMA RSCP threshold, range -5..91
      WCDMA EcNO threshold, range 0..49
  */
  int16 thresh;

} nb1_cphy_meas_irat_thresh_s;


/*! @brief Enumeration of different types for periodical report configs
*/
typedef enum
{
  /*! Report strongest cell */
  NB1_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_STRONGEST_CELL,

  /*! SON */
  NB1_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_SON,

  /*! Report cgi */
  NB1_CPHY_MEAS_REPORT_CFG_PERIODICAL_TYPE_CGI,

} nb1_cphy_meas_report_cfg_periodical_type_e;


/*! @brief Report config for periodical reports
*/
typedef struct
{
  /*! Purpose of this periodical report */
  nb1_cphy_meas_report_cfg_periodical_type_e purpose;

} nb1_cphy_meas_report_cfg_periodical_s;


/*! @brief Trigger config for a1 triggers
*/
typedef struct
{
  /*! Threshold for event a1 */
  nb1_cphy_meas_eutra_thresh_t thresh;

} nb1_cphy_meas_report_cfg_trig_a1_s;


/*! @brief Trigger config for a2 triggers
*/
typedef struct
{
  /*! Threshold for event a2 */
  nb1_cphy_meas_eutra_thresh_t thresh;

} nb1_cphy_meas_report_cfg_trig_a2_s;


/*! @brief Trigger config for a3 triggers
*/
typedef struct
{
  /*! Event offset - Range: -30..30 */
  int16 offset;

  /*! Send report on exit, if TRUE */
  boolean report_on_leave;

} nb1_cphy_meas_report_cfg_trig_a3_s;


/*! @brief Trigger config for a4 triggers
*/
typedef struct
{
  /*! Threshold for event a4 */
  nb1_cphy_meas_eutra_thresh_t thresh;

} nb1_cphy_meas_report_cfg_trig_a4_s;


/*! @brief Trigger config for a5 triggers
*/
typedef struct
{
  /*! Threshold1 for event a5 */
  nb1_cphy_meas_eutra_thresh_t thresh1;

  /*! Threshold2 for event a5 */
  nb1_cphy_meas_eutra_thresh_t thresh2;

} nb1_cphy_meas_report_cfg_trig_a5_s;


/*! @brief Trigger config for a6 triggers
*/
typedef struct
{
  /*! Event offset - Range: -30..30 */
  int16 offset;

  /*! Send report on exit, if TRUE */
  boolean report_on_leave;

} nb1_cphy_meas_report_cfg_trig_a6_s;


/*! @brief Trigger config for b1 triggers
*/
typedef struct
{
  /*! Threshold for event b1 */
  nb1_cphy_meas_irat_thresh_s thresh;

} nb1_cphy_meas_report_cfg_trig_b1_s;


/*! @brief Trigger config for b2 triggers
*/
typedef struct
{
  /*! Threshold1 for event b2 */
  nb1_cphy_meas_eutra_thresh_t thresh1;

  /*! Threshold2 for event b2 */
  nb1_cphy_meas_irat_thresh_s thresh2;

} nb1_cphy_meas_report_cfg_trig_b2_s;


/*! @brief Union of trigger configs
*/
typedef union
{
  /*! A1 */
  nb1_cphy_meas_report_cfg_trig_a1_s a1;

  /*! A2 */
  nb1_cphy_meas_report_cfg_trig_a2_s a2;

  /*! A3 */
  nb1_cphy_meas_report_cfg_trig_a3_s a3;

  /*! A4 */
  nb1_cphy_meas_report_cfg_trig_a4_s a4;

  /*! A5 */
  nb1_cphy_meas_report_cfg_trig_a5_s a5;

  /*! A6 */
  nb1_cphy_meas_report_cfg_trig_a6_s a6;

  /*! B1 */
  nb1_cphy_meas_report_cfg_trig_b1_s b1;

  /*! B2 */
  nb1_cphy_meas_report_cfg_trig_b2_s b2;

  /*! Periodic report */
  nb1_cphy_meas_report_cfg_periodical_s periodical;

} nb1_cphy_meas_report_cfg_trig_u;



/*! @brief Enumeration of trigger quantity types of EUTRA report configs
*/
typedef enum
{
  /*! RSRP */
  NB1_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRP,

  /*! RSRQ */
  NB1_CPHY_MEAS_REPORT_CFG_EUTRA_TRIG_QTY_RSRQ,

} nb1_cphy_meas_report_cfg_eutra_trig_qty_e;


/*! @brief Enumeration of report quantity types of EUTRA report configs
*/
typedef enum
{
  /*! Same as trigger quantity */
  NB1_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_SAME,

  /*! Both RSRP and RSRQ */
  NB1_CPHY_MEAS_REPORT_CFG_EUTRA_REPORT_QTY_BOTH,

} nb1_cphy_meas_report_cfg_eutra_report_qty_e;



/*! @brief Trigger config to be used as part of the measurement report configuration
*/
typedef struct
{
  /*! Trigger type */
  nb1_cphy_meas_report_cfg_trig_type_e type;

  /*! Trigger parameters based on trigger type */
  nb1_cphy_meas_report_cfg_trig_u data;

} nb1_cphy_meas_report_cfg_trig_cfg_s;



/*! @brief Measurement report Config
*/
typedef struct
{
  /*! Report interval to use (for both periodical and event based reports), in ms */
  uint32 report_int;

  /*! Trigger config */
  nb1_cphy_meas_report_cfg_trig_cfg_s trig_cfg;

  /*! Trigger quantity type for EUTRA reports */
  nb1_cphy_meas_report_cfg_eutra_trig_qty_e trig_qty;

  /*! Report quantity type for EUTRA reports */
  nb1_cphy_meas_report_cfg_eutra_report_qty_e report_qty;

  /*! Time to trigger (in ms) */
  uint16 time_to_trig;

  /*! Max number of cells to include in a report, range 1..8 */
  uint8 max_cells;

  /*! Num of reports to send. Should be set to NB1_CPHY_MEAS_REPORT_AMT_INFINITY
  if infinity */
  uint8 report_amt;

  /*! Hysteresis, range 0..30 */
  uint16 hyst;

  /*! If ue-RxTxTimeDiffPeriodical is configured or not; Should be configured
    only for reporting on the serving cell. If TRUE, trig_qty, report_qty
    and max_cells are NOT Applicable */
  boolean ue_rxtx_time_diff_periodical_enabled;

  /*! If UE is configured to read SIB1 in connected mode using A-gaps or not;
      Valid only if trigger type is set to Periodical and purpose is set to
      reportCGI */
  boolean si_request_for_ho;

  /*! Boolean which control if UE need to report neighbouring cell */
  boolean report_add_neigh_meas_r10;

  /*! Boolean which indicates presence of 'includeLocationInfo-r10' for this report cfg*/
  boolean location_info_enabled;

} nb1_cphy_meas_report_cfg_s;


/*! @brief Maximum number of neighbor cells that can be included in a
    connected-mode measurement report
*/
#define NB1_CPHY_MAX_CELL_REPORT  8

/*! @brief RSRP dBm value mapped to range 0..97 as defined in 36.133; negative
    value indicates invalid, not known, or not present
*/
typedef int8 nb1_cphy_conn_meas_rsrp_t;

/*! @brief RSRQ dB value mapped to range 0..34 as defined in 36.133; negative
    value indicates invalid, not known, or not present
*/
typedef int8 nb1_cphy_conn_meas_rsrq_t;

typedef enum
{
  NB1_CPHY_STOP_REQ_DEFAULT                     = 0,
  NB1_CPHY_STOP_REQ_DMM_SUSPEND                 = 1,
}nb1_cphy_stop_req_cause_e;

/*! @brief Defines the type of acquisition required by RRC
*/
typedef enum
{
  /*! Acq type is based on frequency */
  NB1_CPHY_ACQ_FREQ = 0,
  /*! Acq type is based on cell */
  NB1_CPHY_ACQ_CELL = 1
}nb1_cphy_acq_type_e;


/*! @brief Defines the type (RAT) of neighbor measurement results in a
    measurement report
*/
typedef enum
{
  /*! EUTRA neighbor measurement results */
  NB1_CPHY_CONN_MEAS_REPORT_TYPE_EUTRA,
  /*! WCDMA neighbor measurement results */
  NB1_CPHY_CONN_MEAS_REPORT_TYPE_WCDMA,
  /*! GERAN neighbor measurement results */
  NB1_CPHY_CONN_MEAS_REPORT_TYPE_GERAN,
  /*! CDMA2000 neighbor measurement results */
  NB1_CPHY_CONN_MEAS_REPORT_TYPE_CDMA2K
} nb1_cphy_conn_meas_report_type_e;

/*! @brief Structure for EUTRA measurement result list element
*/
typedef struct
{
  /*! Physical cell ID of reported cell */
  nb1_phy_cell_id_t          pci;
  /*! RSRP of reported cell (negative value indicates RSRP not reported) */
  nb1_cphy_conn_meas_rsrp_t  rsrp;
  /*! RSRQ of reported cell (negative value indicates RSRQ not reported) */
  nb1_cphy_conn_meas_rsrq_t  rsrq;
} nb1_cphy_conn_meas_report_list_eutra_s;

/*! @brief Structure for WCDMA measurement result list element
*/
typedef struct
{
  /* Whether all information below corresponds to fdd mode or tdd mode */
  nb1_cphy_meas_obj_wcdma_type_e wcdma_mode;

  /*! cell id for which measurments are reported */
  uint16   psc;

  /*! RSCP of reported cell (NB1_CPHY_WCDMA_RSCP_UNREPORTED_VALUE indicates RSCP
  not reported)
  Range -5..91 (corresponding to -125..-20 dbm)
  */
  int8     cpich_rscp;

  /*! EcNo of reported cell (NB1_CPHY_WCDMA_ECNO_UNREPORTED_VALUE indicates EcNo
  not reported)
  Range 0..49 (corresponding to -24..0 db)
  */
  int8     cpich_ecno;

} nb1_cphy_conn_meas_report_list_wcdma_s;

/*! @brief Structure for GERAN measurement result list element
*/
typedef struct
{

  /*! GSM frequency being reported */
  nb1_l1_gsm_arfcn_t freq;

  /*! GSM band of the frequency being reported

    Allows UE to differentiate between GSM DCS 1800 and PCS 1900 frequencies
    which have overlapping ARFCN values.

    This flag does not specify the band for all ARFCNs in this group, but
    only serves as a differentiator for overlapping ARFCN values.  Must
    be set to either NB1_L1_GSM_BAND_DCS_1800 or NB1_L1_GSM_BAND_PCS_1900
    and is only relevant to ARFCN values which fall in either of those bands.
   */
  nb1_l1_gsm_band_e gsm_band;

  /* PhyCellIDGERAN of the cell being reported */
  nb1_cphy_geran_phy_cell_id_s phy_cell_id;

  /*! Measured RSSI value for the cell being reported, required field.
    Value range (0..63)*/
  uint8 rssi;

} nb1_cphy_conn_meas_report_list_gsm_s;

/*! CDMA2000 measurement result phase not specified */
#define NB1_CPHY_CONN_MEAS_REPORT_CDMA2K_PHASE_NOT_SPECIFIED (-1)

/*! @brief Structure for CDMA2000 measurement result list element
*/
typedef struct
{
  /*! pilot pn of the cell being reported */
  sys_pilot_pn_type phy_cell_id;

  /*! pilot strength of the cell being reported, required field.
    Value range (0..63) */
  uint8 pilot_strength;

  /*! pilot_pn_phase of the cell being reported, optional field. (negative value
    1 indicates pilot_pn_phase not reported). Reported value range (0..32767) */
  int16 pilot_pn_phase;

} nb1_cphy_conn_meas_report_list_cdma2k_s;

/*! @brief Union containing the measurement result lists for each supported
    RAN
*/
typedef union
{
  /*! EUTRA neighbor measurement results list */
  nb1_cphy_conn_meas_report_list_eutra_s   eutra[NB1_CPHY_MAX_CELL_REPORT];
  /*! WCDMA neighbor measurement results list */
  nb1_cphy_conn_meas_report_list_wcdma_s    wcdma[NB1_CPHY_MAX_CELL_REPORT];
  /*! GERAN neighbor measurement results list */
  nb1_cphy_conn_meas_report_list_gsm_s   gsm[NB1_CPHY_MAX_CELL_REPORT];
  /*! CDMA2000 neighbor measurement results list */
  nb1_cphy_conn_meas_report_list_cdma2k_s  cdma2k[NB1_CPHY_MAX_CELL_REPORT];
} nb1_cphy_conn_meas_report_list_u;

/*! @brief Structure that defines the measurement result list, used in
    measurement reports
*/
typedef struct
{
  /*! Number of results in list. Range: 0..NB1_CPHY_MAX_CELL_REPORT */
  uint8                             size;
  /*! Measurement results list type, e.g. EUTRA, WCDMA, etc. */
  nb1_cphy_conn_meas_report_type_e  type;
  /*! Measurement results list; union of all types */
  nb1_cphy_conn_meas_report_list_u  list;
} nb1_cphy_conn_meas_report_list_s;


/*! @brief Defines filter coefficient value.
*/
typedef uint8 nb1_cphy_meas_filter_coeff_t;

#define NB1_CPHY_MEAS_FILTER_COEFF_DISABLED 0


/*! @brief Filter coefficients for EUTRA measurements
*/
typedef struct
{
  /*! FC for RSRP, 0 indicates disabled */
  nb1_cphy_meas_filter_coeff_t fc_rsrp;
  /*! FC for RSRQ, 0 indicates disabled */
  nb1_cphy_meas_filter_coeff_t fc_rsrq;

} nb1_cphy_meas_qty_cfg_eutra_s;

/*! measurement quantity for WCDMA FDD */
typedef enum
{
  NB1_CPHY_MEAS_WCDMA_QTY_TYPE_CPICH_RSCP,
  NB1_CPHY_MEAS_WCDMA_QTY_TYPE_CPICH_ECNO
} nb1_cphy_meas_wcdma_fdd_qty_type_e;

/*! measurement quantity for WCDMA TDD */
typedef enum
{
  NB1_CPHY_MEAS_WCDMA_QTY_TYPE_PCCPCH_RSCP
} nb1_cphy_meas_wcdma_tdd_qty_type_e;

/*! @brief Defines the type of meas quantity for GSM
*/
typedef enum
{
  /*! RSSI */
  NB1_CPHY_MEAS_QTY_GSM_RSSI = 0

}nb1_cphy_meas_qty_gsm_e;

/*! @brief Filter coefficients for GSM measurements
*/
typedef struct
{
  /*!< Meas Quantity for GSM */
  nb1_cphy_meas_qty_gsm_e meas_qty;
  /*!< FC for GSM, default fc2 */
  nb1_cphy_meas_filter_coeff_t fc;

} nb1_cphy_meas_qty_cfg_gsm_s;

/*! @brief quantity config for wcdma measurements
*/
typedef struct
{
  /*! Flag to indicate if quantityConfigUTRA rel 10 is present.
    TRUE implies, UE applys filter coefficient specified in QuantityConfigUTRA
    for the measurement specified in measQuantityUTRA-FDD and apply filterCoefficient2-FDD-r-10
    for the other measurement not specified in measQuantityUTRA-FDD
    and UE will send measurement report containing both ecno and rscp

    FALSE implies UE applys filter coefficient specified in QuantityConfigUTRA
    for both ecno and rscp and UE will send measurement report containing
    only the measurement specified in measQuantityUTRA-FDD*/

  boolean      qty_cfg_rel10_present;

  /*! filter coefficient for rscp, default fc4 */
  nb1_cphy_meas_filter_coeff_t     rscp_fc;

  /*! filter coefficient for ecno, default fc4 */
  nb1_cphy_meas_filter_coeff_t     ecno_fc;

  /*! indicates what FDD quantity has to be measured */
  nb1_cphy_meas_wcdma_fdd_qty_type_e   qty_type_fdd;

  /*! indicates what TDD quantity has to be measured */
  nb1_cphy_meas_wcdma_tdd_qty_type_e qty_type_tdd;

} nb1_cphy_meas_qty_cfg_wcdma_s;

/*! @brief Quantity configs for CDMA
*/
typedef enum
{
  /*! No cdma qty config */
  NB1_CPHY_MEAS_QTY_CFG_CDMA_NONE=0,

  /*! Pilot strength measurement */
  NB1_CPHY_MEAS_QTY_CFG_PILOT_STRENGTH,

  /*! PN phase and pilot strength measurement */
  NB1_CPHY_MEAS_QTY_CFG_PN_PHASE_PILOT_STRENGTH,

  /*! Maximum number of cfgs */
  NB1_CPHY_MEAS_QTY_CFG_MAX_NUM_CFGS,
}nb1_cphy_meas_qty_cfg_cdma_e;

/*! @brief Quantity configs for all RATs
*/
typedef struct
{
  /*! EUTRA quantity config */
  nb1_cphy_meas_qty_cfg_eutra_s eutra;

  /*! WCDMA quantity config */
  nb1_cphy_meas_qty_cfg_wcdma_s wcdma;

  /*! GERAN quantity config */
  nb1_cphy_meas_qty_cfg_gsm_s gsm;

  /*! cdma quantity config */
  nb1_cphy_meas_qty_cfg_cdma_e cdma;

  /*! Quantity configs for other RATs to be added later */

} nb1_cphy_meas_qty_cfg_s;

/*! @brief Measurement gap patterns
*/
typedef enum
{
  /*! Pattern ID 0 - repeats every 40ms */
  NB1_CPHY_MEAS_GAP_PATTERN_ID_0 = 0,
  /*! Pattern ID 1 - repeats every 80ms */
  NB1_CPHY_MEAS_GAP_PATTERN_ID_1,
  /*! Max Pattern ID */
  NB1_CPHY_MEAS_GAP_MAX_PATTERNS
} nb1_cphy_meas_gap_pattern_type_e;

/*! @brief Measurement gap configs
*/
typedef struct
{
  /*! Gap config enabled or not */
  boolean                              gap_cfg_enabled;
  /*! Gap pattern */
  nb1_cphy_meas_gap_pattern_type_e     pattern_id;
  /*! Gap offset (Range either 0-39 or 0-79 depending on pattern id) */
  uint8                                gap_offset;
} nb1_cphy_meas_gap_cfg_s;


/*! @brief Speed scaling parameters for connected mode EUTRA measurements
*/
typedef struct
{
  /*! False if speed scaling is disabled, True otherwise */
  boolean enabled;

  /*! Mobility State config params */
  nb1_cphy_meas_mobility_state_config_param_s config_params;

  /*! Time to trigger scaling factor for medium mobility */
  nb1_cphy_meas_time_sf_e time_to_trig_sf_medium;

  /*! Time to trigger scaling factor for high mobility */
  nb1_cphy_meas_time_sf_e time_to_trig_sf_high;

} nb1_cphy_conn_meas_speed_scaling_cfg_s;


/*! @brief Connected mode measurement configuration
*/
typedef struct
{
  /*! Measurement ID list. Index 0 corresponds to meas ID 1, ..., index 31
      corresponds to meas ID 32. */
  nb1_cphy_meas_id_s meas_id_list[NB1_CPHY_MEAS_MAX_MEAS_ID];

  /*! Measurement object list. Index 0 corresponds to meas object 1, ...,
      index 31 corresponds to meas object 32. */
  nb1_cphy_meas_obj_s meas_obj_list[NB1_CPHY_MEAS_MAX_MEAS_OBJ_ID];

  /*! Reporting configuration list. Index 0 corresponds to report config 1, ...,
      index 31 corresponds to report config 32. */
  nb1_cphy_meas_report_cfg_s report_cfg_list[NB1_CPHY_MEAS_MAX_REPORT_CFG_ID];

  /*! Quantity Config */
  nb1_cphy_meas_qty_cfg_s qty_cfg;

  /*! Gap configuration */
  nb1_cphy_meas_gap_cfg_s gap_cfg;

  /*! S-Measure (-140 to -44), 0 indicates disabled */
  int16 s_measure;

  /*! Speed scaling config */
  nb1_cphy_conn_meas_speed_scaling_cfg_s sc_cfg;

} nb1_cphy_conn_meas_cfg_info_s;


/*! @brief Enumeration for NMR report types
*/
typedef enum
{

  /* Initial value - should be used just for initialization  */
  NB1_CPHY_NMR_REPORT_NONE = 0x00,
  /* Used for requesting report info containing NB1 intra frequency neighbors */
  NB1_CPHY_NMR_REPORT_INTRA_FREQ = 0x01,
  /* Used for requesting report info containing NB1 inter frequency neighbors */
  NB1_CPHY_NMR_REPORT_INTER_FREQ = 0x02,
  /* Used for requesting report info containing WCDMA neighbors */
  NB1_CPHY_NMR_REPORT_IRAT_WCDMA = 0x03,
  /* Used for requesting report info containing GERAN neighbors */
  NB1_CPHY_NMR_REPORT_IRAT_GERAN = 0x04,
  /* Used for requesting report info containing C2K neighbors */
  NB1_CPHY_NMR_REPORT_IRAT_CDMA2K = 0x05

}nb1_cphy_nmr_report_type_e;

typedef enum
{
  /*! CE Level = 0 */
  NB1_CPHY_CE_LEVEL_0 = 0,

  /*! CE Level = 1 */
  NB1_CPHY_CE_LEVEL_1,

  /*! CE Level = 2 */
  NB1_CPHY_CE_LEVEL_2,

  /* Total num of CE levels possible */
  NB1_CPHY_CE_LEVEL_MAX,

  /* Invalid CE level */
  NB1_CPHY_CE_LEVEL_INVALID = 0xFF
}nb1_cphy_ce_level_type_e;

/*===========================================================================

                   CPHY SAPS( REQ, CNF, IND)

===========================================================================*/

/*! @defgroup cphy_sap Control Plane (CPHY) Interface

  @brief This module describes the control interface to L1

  @detail The umids for this module are defined in @ref umids in the
          CPHY section.  The structures associated with the messages
          are defined in this module, as well as the documentation
          describing these messages

          This interface is generated from
          $Header: //components/rel/nb1.mpss/2.2/api/nb1_cphy_msg.h#1 $ //source/qcom/qct/modem/nb1/ML1/l1_common/main/latest/inc/nb1_cphy_sap.h#46 $

          The following log describes the recent changes of the interface:
          @verbatim
          $Log:$

          @endverbatim

  @{*/

typedef enum
{
  NB1_CPHY_START_REQ_DEFAULT                    = 0,
  NB1_CPHY_START_REQ_DMM_RESUME                 = 1,
}nb1_cphy_start_req_cause_e;

/*! @brief
  Structure for NB1 capability info
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Below info is valid only of this flag is set */
  boolean valid;

  /*! UE Category */
  uint8   ue_cat;

  /*! Max CC supported on DL */
  uint8   max_num_dl_carriers;

  /*! Max CC supported on UL */
  uint8   max_num_ul_carriers;

} nb1_ue_cap_info_s;


/*! @brief
  This message is used to signal ML1 to start handling commands in NB1 mode of
  operation.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Device mode type */
  sys_modem_device_mode_e_type  device_mode;

  /*!< Number of active SIMs */
  sys_modem_dual_standby_pref_e_type  standby_mode;
  /*! UE mode type */
  nb1_cphy_ue_mode_type_e  ue_mode;

  /* Start cause for ML1 to decide TRM exchange */
  nb1_cphy_start_req_cause_e    start_cause;

  /* NB1 earfcn for ML1 for TRM reservation */
  nb1_earfcn_t    earfcn;

  /* NB1 UE capability info */
  nb1_ue_cap_info_s    cap_info;

} nb1_cphy_start_req_s;


/*! @brief
  This message confirms the start request
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Whether or not the start was successful */
  errno_enum_type      status;
} nb1_cphy_start_cnf_s;

/*! @brief
  This message is used to signal ML1 about the NV params shared between RRC/ML1
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /* Following fields are populated based on EFS files if present or set to defaults */

  lte_3gpp_release_ver_e nb1_rrc_3gpp_release_ver;  /*!< Variable denoting the NB1 3GPP Spec Release */

  boolean axgp; /*!< Var for indicating that axgp band is supported*/

  nb1_l1_extnd_bands_updated_info_s  nb1_rrc_update_band; /*!< Bands updated info via EFS ("update_band_range") */

  nb1_l1_bs_bw_supported_s  nb1_rrc_bs_bw_supported; /*!< BW supported info via EFS ("bs_bw_supported") */

#ifdef FEATURE_GNSS_LTE_CATM_COEX
  boolean gnss_higher_prio_over_nb1; /*!< Var for indicating that GNSS has higher priority than NB1*/
#endif
} nb1_cphy_nv_update_req_s;

/*! @brief
  This message is used to signal ML1 to stop handling commands in NB1 mode of
  operation.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Specific cause for stop */
  nb1_cphy_stop_req_cause_e stop_cause;

} nb1_cphy_stop_req_s;


/*! @brief
  This message is used to confirm for stop request
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether or not the stop was successful */
  errno_enum_type      status;

  /*! CDMA time info: Valid only if suspended for the same cause */
  nb1_stmr_cdma_rtc_pair     cdma_time_info;
} nb1_cphy_stop_cnf_s;


/*! @brief
  This message is used to command L1 to acquire a cell.

  L1 performs a cell search and attempts to decode PBCH on the best detected
  cell.  If L1 acquires a cell that is listed in the blacklist, it will attempt
  to acquire another cell. L1 will attempt to acquire at most N+1 cells, where
  N is the number of blacklisted cells.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Transaction ID Range: 0..255 */
  uint8 trans_id;

  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;

  /*! 0..65535 (allowing for expansion) */
  nb1_earfcn_t  earfcn;

  /*! SNR level which will be apply for acquisition */
  nb1_cphy_fscan_snr_level_type_e   snr_level;
  
  /*! Configuration to apply partial roaming restriction */
  nb1_cphy_partial_roaming_restriction_e restriction_cfg;

  /*! Indicates the number of cells in the blacklist Range: 0..4 */
  uint8 num_black_cells;

  /*! Cell blacklist; L1 must try to acquire a cell not in this list; used for
      e.g. barred cells, can be empty */
  nb1_phy_cell_id_t blacklist[NB1_CPHY_MAX_ACQ_BLACKLISTED_CELLS];

  /*! Acq_type indictates the type of acquisition i.e. cell specific or any cell on a particular freq etc */
  nb1_cphy_acq_type_e acq_type;

  /*! Cell id to acquire if acq type has been set to NB1_CPHY_ACQ_CELL
      This field is irrelevant for acq type NB1_CPHY_ACQ_FREQ
      Cell id provided in thie field will not be a part of the RRC populated
      blacklist field. */
  nb1_phy_cell_id_t acq_cell_id;

  /*! Timing info to skip init ACQ search */
  uint64                ref_time;

  /*! timeout value for the procedure (in seconds) */
  uint32 t_timeout;

  /* NW configured customized tx-rx spacing*/
  boolean custom_duplex_spacing_enabled;
} nb1_cphy_acq_req_s;


/*! @brief
  This message is to confirm and carry the results of the requested acquisition
  attempt. L1 returns results for the acquired cell if acquisition is
  successful.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Transaction ID; matches the one in the request */
  uint8 trans_id;

  /*! Acquisition success/failure */
  errno_enum_type      status;

  /*! Confirmation of the requested band */
  sys_sband_lte_e_type band;

  /*! Confirmation of the requested EARFCN */
  nb1_earfcn_t earfcn;

  /*! Cell info for acquired cell; only valid if status indicates acquisition
      success */
  nb1_cphy_acq_cell_info_s acquired_cell;

  /*! is cell barred */
  boolean cell_barred;
} nb1_cphy_acq_cnf_s;

/*! @brief
  This message is used for configuring UL/DL channels for common configuration.
  This message allows multiple channel setup operations in a single primitive.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! The information of the current serving cell, used for sanity purposes */
  nb1_cphy_cell_info_s serving_cell_info;
  nb1_cphy_common_cfg_req_param_s common_cfg_param;
} nb1_cphy_common_cfg_req_s;


/*! @brief
  This message is used for configuring idle drx parameters.
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;
  /*! The information of the current serving cell, used for sanity purposes */
  nb1_cphy_cell_info_s serving_cell_info;
  nb1_cphy_idle_drx_param_s idle_drx_param;

  /*! EDRX Enabled Flag - If enabled use edrx params for page scheduling,
      Otherwise use legacy idle drx params */
  boolean                    edrx_enabled;

  /*! when EDRX is enabled, page is scheduled based on edrx params*/
  nb1_cphy_idle_edrx_param_s edrx_param;

} nb1_cphy_idle_drx_cfg_req_s;

/*! Sent from ML1 RFMGR->L2.
    RFMGR TRM checks if to allow an unlock request*/
typedef struct
{
  msgr_hdr_struct_type                msgr_hdr;  /*!< message header */

  /*! When is unlock requested -
      in the unit of subframe from last SF_IND.
      This also includes duration needed for suspend procedure at ML1 */
  nb1_sfn_s                           unlock_by_start_time;

  /*! How long is unlock requested for. In units of msecs.
      This also includes the warm up time needed at ML1 */
  uint16                              unlock_by_duration;

  /*! Unlock requested for which client. 0 - 1x 8 - GSM*/
  uint8                               winning_client_id;

  /*! If the TRM priority in unlock is higher than EMBMS high, set this to TRUE*/
  boolean                             winning_client_trm_priority_high;
} nb1_cphy_trm_unlock_allowed_req_s;

/*! Sent from L2-> ML1 RFMGR
    L2 informs RFMGR if ML1 can proceed a unlock request, or ignore the request*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;        /*!< message header */

  /*! TRUE - proceed with unlock request
      FALSE - Ignore unlock request*/
  boolean                          unlock_is_allowed;
} nb1_cphy_trm_unlock_allowed_cnf_s;

/*! @brief
  This message is used for configuring idle mode measurement parameters.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The information of the current serving cell, used for sanity purposes */
  nb1_cphy_cell_info_s serving_cell_info;

  /*! Measurement config information */
  nb1_cphy_idle_meas_cfg_info_s meas_cfg;

} nb1_cphy_idle_meas_cfg_req_s;


/*! @brief
  This message confirms the idle mode measurement config request
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether or not the configuration was successfully applied */
  errno_enum_type status;

} nb1_cphy_idle_meas_cfg_cnf_s;

/*! @brief
  This message is used for configuring CDMA system time only. This is used
  for passing CDMA system time during periodic SIB8 reading or during reverse
  time transfer.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! If CDMA system time is enabled or not. Disabled when not signaled or when
    signaled value fails validation */
  boolean enabled;

  /*! CDMA system time type */
  nb1_cphy_irat_cdma_system_time_source_type_e source_type;

  /*! CDMA system time info */
  nb1_cphy_irat_cdma_system_time_info_u irat_cdma_system_time_info;

  /*! The cell on which SIB8 is received/reverse time transfer was peformed on */
  nb1_cphy_cell_info_s cell_info;

} nb1_cphy_irat_cdma_system_time_chg_req_s;

/*! @brief
  This message is used for configuring UTC system time only. This is used
  for passing UTC system time during periodic SIB16 reading.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! If UTC system time is enabled or not. Disabled when not signaled or when
    signaled value fails validation */
  boolean enabled;

  /*! UTC system time info */
  nb1_cphy_utc_time_info_s utc_time_info;

  /*! The cell on which SIB16 is received */
  nb1_cphy_cell_info_s cell_info;

} nb1_cphy_utc_time_update_req_s;

/*! @brief
  This message confirms the configuring UTC system time request
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether or not the configuration was successfully applied */
  errno_enum_type status;

} nb1_cphy_utc_time_update_cnf_s;

/*!
  @brief
  An enum describing the bit positions in the other_mod_bitmask field in
  nb1_cphy_conn_meas_cfg_req_s
  */
enum
{
  NB1_CPHY_MEAS_CFG_BIT_QTY_CFG         = 0x01,
  NB1_CPHY_MEAS_CFG_BIT_S_MEASURE       = 0x02,
  NB1_CPHY_MEAS_CFG_BIT_GAP_CFG         = 0x04,
  NB1_CPHY_MEAS_CFG_BIT_SPEED_SCALING   = 0x08,
};

/*! @brief
  This message is used for configuring connected mode measurement parameters.
  Note: The meas_cfg_ptr points to a buffer that is maintained by RRC, ML1
  is expected to copy the relevent configuration as needed, prior to sending the
  NB1_CPHY_CONN_MEAS_CFG_CNF msg. After the cnf msg is received by RRC, ML1 should
  not attempt to access this pointer.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The information of the current serving cell, used for sanity purposes */
  nb1_cphy_cell_info_s serving_cell_info;

  /*! Bitmask which indicates which measurement objects are valid. Bit 0
      corresponds to meas object 1, ..., bit 31 corresponds to
      meas object 32. */
  uint32 meas_obj_valid_bitmask;

  /*! Bitmask which indicates which measurement ids are valid. Bit 0
      corresponds to meas ID 1, ..., bit 31 corresponds to meas ID 32. */
  uint32 meas_id_valid_bitmask;

  /*! Bitmask which indicates which report configs are valid. Bit 0
      corresponds to report config 1, ..., bit 31 corresponds to
      report config 32. */
  uint32 report_cfg_valid_bitmask;

  /*! Bitmask which indicates which meas objects have been touched
      (add/modify/delete). Bit 0 corresponds to meas object 1, ...,
      bit 31 corresponds to meas object 32. */
  uint32 meas_obj_mod_bitmask;

  /*! Bitmask which indicates which meas ids have been touched
      (add/modify/delete). Bit 0 corresponds to meas ID 1, ..., bit 31
      corresponds to meas ID 32. */
  uint32 meas_id_mod_bitmask;

  /*! Bitmask which indicates which report configs have been touched
      (add/modify/delete). Bit 0 corresponds to report config 1, ..., bit 31
      corresponds to report config 32. */
  uint32 report_cfg_mod_bitmask;

  /*! Bitmask which indicates which other cfgs have been modified */
  uint32 other_mod_bitmask;

  /*! Pointer to measurement config information */
  nb1_cphy_conn_meas_cfg_info_s *meas_cfg_ptr;

} nb1_cphy_conn_meas_cfg_req_s;


/*! @brief
  This message confirms the conn mode measurement config request
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether or not the configuration was successfully applied */
  errno_enum_type status;

} nb1_cphy_conn_meas_cfg_cnf_s;


/*! @brief Measurement report CA serving freq list structure
*/
typedef struct
{
  /*! Physical cell ID of reported cell */
  nb1_phy_cell_id_t          pci;
  /*! RSRP of reported  neigh cell (negative value indicates RSRP not reported) */
  nb1_cphy_conn_meas_rsrp_t  neighCell_rsrp;
  /*! RSRQ of reported  neigh cell (negative value indicates RSRQ not reported) */
  nb1_cphy_conn_meas_rsrq_t  neighCell_rsrq;
} nb1_cphy_conn_meas_report_best_neighCell_s;


/*! @brief Measurement report CA serving freq list structure
*/
typedef struct
{
  /*! RSRP of serving cell */
  nb1_cphy_conn_meas_rsrp_t         serving_rsrp;
  /*! RSRQ of reported cell */
  nb1_cphy_conn_meas_rsrq_t         serving_rsrq;

} nb1_cphy_conn_meas_report_serving_cell_s;

/*! @brief Measurement report CA serving freq list structure
*/
typedef struct
{
  /*ServCellIndex-r10   ::=   INTEGER   (0 .  . 7 )  */
  uint8 serv_cell_index;

  /*L1 to look at meas_report_scell structure only when meas_report_scell_present = TRUE*/
  boolean meas_report_scell_present;
  nb1_cphy_conn_meas_report_serving_cell_s meas_report_scell;

  /*L1 to look at meas_report_best_neighCell_present structure only when meas_report_best_neighCell = TRUE*/
  boolean meas_report_best_neighCell_present;
  nb1_cphy_conn_meas_report_best_neighCell_s meas_report_best_neighCell;
} nb1_cphy_conn_meas_report_scell_s;

/*! @brief Measurement report CA serving freq list structure
*/
typedef struct
{
  /*maxServCell-r10   INTEGER   ::= 4   --   Maximum   number   of   Serving   cells
   Note:- there is issue with RRC ASN.1 it says maxServCell as 5, Spec will be clarified soon*/
  uint8 num_scell_list;
  nb1_cphy_conn_meas_report_scell_s meas_report_scell_list[NB1_CPHY_MAX_SERVING_CELL];
} nb1_cphy_conn_meas_report_scell_freq_list_s;


/*! @brief UE Rx-Tx time diff meas result structure
*/
typedef struct
{
  /*! ue-RxTxTimeDiffResult for the serving cell; Range (0..4095) */
  uint32 ue_rxtx_timediff_result;
  /*! Current SFN in which UE rx_tx time diff meas result is sent to RRC */
  uint32 current_sfn;

} nb1_cphy_rxtx_timediff_meas_result_s;

/*! @brief Measurement report message structure
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Measurement ID for report. Range 1..32. */
  nb1_cphy_meas_id_t                meas_id;
  /*! TRUE if the trigger type is periodical and this is the last report. This
      indicates that ML has autonomously deleted the meas ID from the VMC. */
  boolean                           auto_delete_meas_id;
  /*! RSRP of serving cell */
  nb1_cphy_conn_meas_rsrp_t         serving_rsrp;
  /*! RSRQ of reported cell */
  nb1_cphy_conn_meas_rsrq_t         serving_rsrq;
  /*! Neighbor measurement results list */
  /*! For inter-freq and irat NMR reports RRC will not read this
  but will look at n_freq_report_ptr */
  nb1_cphy_conn_meas_report_list_s  neigh_results;
  /*! If UE rxtx time diff result is included for serving cell */
  boolean rxtx_timediff_included;
  /*! UE rxtx time diff result for serving cell */
  nb1_cphy_rxtx_timediff_meas_result_s rxtx_result;

  nb1_cphy_conn_meas_report_scell_freq_list_s meas_report_ca_srv_freq_list;
} nb1_cphy_conn_meas_report_ind_s;

/*! @brief GSM NMR report message structure
*/
typedef struct
{
  /*! gsm frequency */
  nb1_l1_gsm_arfcn_t freq;

  /*! Number of results in list. Range: 0..NB1_CPHY_MAX_CELL_REPORT */
  uint8 size;

} nb1_rrc_nmr_geran_info_s;

/*! @brief WCDMA NMR report message structure
*/
typedef struct
{
  /*! wcdma frequency */
  nb1_uarfcn_t freq;

  /*! Number of results in list. Range: 0..NB1_CPHY_MAX_CELL_REPORT */
  uint8 size;

} nb1_rrc_nmr_utra_info_s;

/*! @brief NB1 NMR report message structure
*/
typedef struct
{
  /*! nb1 Frequency */
  nb1_earfcn_t freq;

  /*! Number of results in list. Range: 0..NB1_CPHY_MAX_CELL_REPORT */
  uint8 size;
} nb1_rrc_nmr_eutra_info_s;

/*! @brief IRAT neighbor freq measurement info
*/
typedef union
{
  /*! EUTRA neighbor freq measurement info */

  nb1_rrc_nmr_eutra_info_s eutra_report[NB1_CPHY_MEAS_MAX_NUM_FREQ];

  /*! UTRA neighbor freq measurement info */

  nb1_rrc_nmr_utra_info_s utra_report[NB1_CPHY_MEAS_MAX_NUM_FREQ];

  /*! GERAN neighbor freq measurement info */

  nb1_rrc_nmr_geran_info_s geran_report[NB1_CPHY_MEAS_MAX_NUM_FREQ];

  /*! TBD: Add similar structure for CDMA later if necessary */
} nb1_cphy_nmr_rat_info_u;

/*! @brief NMR report message structure for n freq
*/
typedef struct
{
  /*! Number of reports for inter-freq eutra or inter-RAT measurements */
  uint8 num_report;

  /*! Indicates the type of report populated */
  nb1_cphy_nmr_report_type_e report_type;

  /*! RAT neighbor freq measurement info */
  nb1_cphy_nmr_rat_info_u rat_info;

} nb1_cphy_nmr_n_freq_report_s;

/*! @brief NMR request message structure
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Indicate the type of report requested */
  nb1_cphy_nmr_report_type_e report_type;
  /*! Pointer to the RRC memory for NMR n_freq_report
  Must be used for report_type = NB1_CPHY_NMR_REPORT_INTER_FREQ,
                                 NB1_CPHY_NMR_REPORT_IRAT_WCDMA,
                                 NB1_CPHY_NMR_REPORT_IRAT_GERAN
  Must not be used for NB1_CPHY_NMR_REPORT_INTRA_FREQ and others
  */
  nb1_cphy_nmr_n_freq_report_s *n_freq_report_ptr;
} nb1_cphy_nmr_info_req_s;

/*! @brief
  NMR confirm message structure.  The meas_id field shall be set to 1 for all
  reports triggered by NMR (according to 36.111 section 8.22).
  If for any reason ML1 cannot send a valid report, it shall indicate this
  by setting the serving cell RSRP and RSRQ to -1.
*/
typedef nb1_cphy_conn_meas_report_ind_s nb1_cphy_nmr_info_cnf_s;

/*! @brief Serving cell measurement results
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*!Boolean to indicate if ML1 should report best neighbor cell measurements */
  boolean report_add_neigh_meas_r10;

} nb1_cphy_serv_cell_meas_req_s;

/*! @brief Serving cell measurement results
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! set to E_SUCCESS if the cnf is valid; if set to
   * E_NOT_AVAILABLE, the srv cell might have changed since
   * the req was sent and RRC should disregard this cnf */
  errno_enum_type                   status;

  /*! RSRP of serving cell */
  nb1_cphy_conn_meas_rsrp_t         serving_rsrp;
  /*! RSRQ of reported cell */
  nb1_cphy_conn_meas_rsrq_t         serving_rsrq;

  /*! CA Serving freq list */
  nb1_cphy_conn_meas_report_scell_freq_list_s meas_report_ca_srv_freq_list;

} nb1_cphy_serv_cell_meas_cnf_s;

/*! @brief
  This message is used to confirm common channel configuration request.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The success of the configuration if the status was not success no
      configurations were applied */
  errno_enum_type      status;

  /*! @brief A bit-mask specifying the per-configuration results of the
    common configuration request.

    Bit turned on means configuration was valid. */
  nb1_cphy_common_cfg_mask_t common_cfg_validity;
} nb1_cphy_common_cfg_cnf_s;

/*! @brief This message is used for configuring UL/DL channels for dedicated
  configuration.

  This message can only be sent after a common configuration has been
  sent and should only be sent when the UE is in connected mode.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The information of the current serving cell, used for sanity purposes */
  nb1_cphy_cell_info_s serving_cell_info;

  /*! Identifies whether this command is specifying the default parameters
      or is a configuration update from the network. */
  boolean is_default_cfg;

  nb1_cphy_dedicated_cfg_req_param_s dedicated_cfg_param;

} nb1_cphy_dedicated_cfg_req_s;

/*! @brief
  This message is used to confirm dedicated channel configuration request.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! The success of the configuration */
  errno_enum_type      status;

  /*! A bit-mask specifying the per-subconfiguration validity of the
    dedicated configuration request.

    Bit turned on means configuration was successful. */
  nb1_cphy_dedicated_cfg_mask_t dedicated_cfg_validity;
} nb1_cphy_dedicated_cfg_cnf_s;


/*! @brief
  This message is used for releasing the dedicated paramters and stopping
  the dedicated procedures when moving to IDLE state.

  Upon receiving this command L1 will drop all dedicated parameters and
  continue in idle mode, with the provided common parameters.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  nb1_cphy_cell_info_s cell_info;
} nb1_cphy_con_release_req_s;

/*! @brief
  Parameter struct for cell info describing cell id, frequency and bandwith
*/
typedef struct
{
  /*! The cell id this configuration applies to */
  nb1_phy_cell_id_t     phy_cell_id;
  /*! The frequency this configuration applies to */
  nb1_earfcn_t          dl_cell_frequency;
  /*! System bandwidth */
  nb1_bandwidth_e       bandwidth;
  /*! Timing info to skip init ACQ search */
  uint64                ref_time;
}nb1_cphy_conn_release_acq_cell_info_s;

/*! @brief
  This message is used to confirm that L1 has processed the release request.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                  msgr_hdr;
  /*! The status of the release request*/
  errno_enum_type                       status;
  /*! Connection Release to do ACQ */
  boolean                               is_acq_needed;
  /*! ACQ cell info*/
  nb1_cphy_conn_release_acq_cell_info_s acq_cell_info;
} nb1_cphy_con_release_cnf_s;


/*! @brief
  This message to abort and clean up any active operations and transitions
  to Inactive state
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
} nb1_cphy_abort_req_s;


/*! @brief
  This message is used to confirm the abort reqeust.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Abort successful or not */
  errno_enum_type      status;
} nb1_cphy_abort_cnf_s;


/*! @brief
  This message requests band scan to be performed by L1.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                    msgr_hdr;

  /*! Transaction identifier Range: 0..255 */
  uint8                                   trans_id;

  /*! Configuration to apply partial roaming restriction */
  nb1_cphy_partial_roaming_restriction_e  restriction_cfg;

  /*! Bands to scan */
  nb1_cphy_band_scan_req_params_s         req_bands;
} nb1_cphy_band_scan_req_s;


/*! @brief
  This message confirms the band scan request and contains the results
  of the operation.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type             msgr_hdr;

  /*! List of scan results. band_ptr points to a static array of
      nb1_cphy_band_scan_result_s of size NB1_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS,
      one for each band requested; order matches requested list of bands struct.
      THERE IS NO MEMORY ALLOCATION TO BE DONE. */
  nb1_cphy_band_scan_result_s      *band_ptr;

  /*! Transaction ID; matches the one in the request */
  uint8                            trans_id;

  /*! Number of band results; matches number of bands requested
      1.. NB1_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS */
  uint8                            num_bands;

  /*! Flag indicated whether the band scan for current band is complete or not
      boolean: TRUE  the band scan for current band is incomplete, FALSE complete */
  boolean                       ffs_incomplete;

  /*! Index which will be used for interleave resume for current band scan 
      this should be valid only when ffs incomplete as TRUE */
  uint16                         search_stop_index;

  /*! Status of operation */
  IxErrnoType                        status;
} nb1_cphy_band_scan_cnf_s;


/*! @brief
  This message requests system scan to be performed by L1.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                    msgr_hdr;

  /*! Transaction ID */
  uint8                                   trans_id;

  /*! Configuration to apply partial roaming restriction */
  nb1_cphy_partial_roaming_restriction_e  restriction_cfg;

  /*! Requested systems */
  nb1_cphy_system_scan_req_params_s       req_systems;
} nb1_cphy_system_scan_req_s;

/*! @brief
  This message confirms the system scan request and contains the results
  of the operation. Note L1 eliminates any results below noise thresh, and
  aborts scan early if a sufficiently strong system is detected.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type               msgr_hdr;

  /*! Transaction ID; matches the one in the request */
  uint8                              trans_id;

  /*! System scan results */
  nb1_cphy_system_scan_cnf_params_s  sys_scan_results;

  /*! Flag indicated whether the system scan is complete or not
        Boolean: FLASE  this time system scan is complete, TRUE: incomplete */
  boolean lfs_incomplete;

  /*! Index which will be used after MARB Load to resume current ongoing system scan 
      this should be valid only when lfs incomplete as TRUE */
  uint16                             search_stop_index;

  /*! Status of operation */
  IxErrnoType                        status;
} nb1_cphy_system_scan_cnf_s;

/*! @brief
  This message is used to provide L1 with the minimum system information
  needed to camp on the cell, and also to request L1 to calculate and return
  the S-criteria value.

  @todo resolve the details of this structure
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! E-UTRA Band */
  sys_sband_lte_e_type band;
  /*! EARFCN*/
  nb1_earfcn_t earfcn;
  /*! Physical cell ID */
  nb1_phy_cell_id_t phy_cell_id;
  /*! S criteria parameters */
  nb1_cphy_cell_select_cfg_s cell_select_cfg;
  /*! CSG cell */
  boolean csg_cell;
} nb1_cphy_cell_select_req_s;


#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
typedef enum
{
  nb1_meas_type_rsrp,
  nb1_meas_type_rxlevel
}nb1_cphy_rx_power_meas_e_type;
#endif

/*! @brief
  This message confirms the camp request; it indicates whether or not L1
  is able to camp on the cell, and provides the S criteria and a failure
  reason if camping fails.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Command success/failure */
  errno_enum_type      status;
  /*! TBD */
  nb1_l1_TBD_t failure_reason;
  /*! E-UTRA band */
  sys_sband_lte_e_type band;
  /*! EARFCN */
  nb1_earfcn_t earfcn;
  /*! Physical cell ID */
  nb1_phy_cell_id_t phy_cell_id;

  /*! the measurement of RSRP */
  int16 rsrp;

  /*! TBD */
  int8  s_rxlev;

  /*! Boolean to indicate if Cell selection info for Release 9 is present;
    For Rel 8 this boolean will always be set to FALSE. For Rel 9 the field will
    always be set to TRUE and either signaled values or default values will be passed */
  boolean cell_select_info_rel9_present;

  /*! S_Qual info Release 9 */
  int32 Squal;

} nb1_cphy_cell_select_cnf_s;

typedef struct
{
  /*! The cell id this configuration applies to or the source cell in case of
      handover */
  nb1_phy_cell_id_t phy_cell_id;

  /*! The frequency the configuration applies to or the source cell in case of handover */
  nb1_earfcn_t dl_cell_frequency;
} nb1_cphy_cell_specific_info;

/*! @brief
    Cell bar info

  @todo remove this type
*/
typedef struct
{
  /*! Barred cell timeout value (in seconds) */
  uint32 t_barred;
  /*! Frequency also barred or not */
  boolean freq_is_barred;
  uint8 num_cells;
  nb1_cphy_cell_bar_cause_e cause;
  nb1_cphy_cell_specific_info cell_info_list[NB1_CPHY_MAX_OVERLAP_BANDS];
} nb1_cphy_cell_bar_info_s;


/*! @brief
  RRC sends this request to indicate a barred cell to L1; no confirm is expected.

*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  nb1_cphy_cell_bar_info_s  bar_info;

  boolean clear_cdb;

} nb1_cphy_cell_bar_req_s;

/*! @brief
  RRC sends this request to unbar cells at L1; no confirm is expected.

*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  nb1_cphy_cell_unbar_cause_e unbar_cause;

} nb1_cphy_cell_unbar_req_s;

/*! @brief
    Reselection cancel info
 */
typedef struct
{
  /*! resel target cell info */
  nb1_cphy_resel_target_cell_info_s  target_cell_info;

  /*! resel cancel  cause */
  nb1_cphy_resel_cancel_cause_e      cancel_cause;

   /*! Barred cell timeout value (in seconds) */
  uint32 t_barred;

} nb1_cphy_resel_cancel_info_s;

/*! @brief
  This msg is sent by RRC to ML1 in IDLE state when currently
  ongoing reselection is aborted.
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! current serving cell info */
  nb1_cphy_cell_info_s               serv_cell_info;

  /*! resel cancel info */
  nb1_cphy_resel_cancel_info_s  cancel_info;
} nb1_cphy_cell_resel_cancel_req_s;

 /*! @brief
  This msg is sent by RRC to CPHY when ConnReject is received from the eNB
  after a successful RACH; so that CPHY can clear the connection state
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /* flag to indicate conn cancel is due to t300 expiry*/
  boolean              is_t300_tmr_expired;  	
} nb1_cphy_cancel_conn_req_s;

/*! @brief
  This msg is sent by RRC to CPHY after a successful RACH; 
  so that CPHY can clear QOffsetTemp related parameters as required
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
} nb1_cphy_rach_success_ind_s;

/*! @brief
  This msg is sent by RRC to ML1 in CONNECTED state when a RLC UM Bearer
  is activated so that ML1 can apply DRX related optimizations.
  This will not be sent during connection release and cphy abort
  and ML1 will implicitly disable this field in those cases.
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! UM mode configured */
  boolean              um_mode_enabled;
} nb1_cphy_drx_opt_req_s;

/*! @brief
  This message is sent to ML1 from MAC when MAC wants to request ML1 to start
  a RACH Preamble.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Randomly selected Subcarrier index (Nint) by MAC Range: 0-47 */
  uint8 subcarrier_index;
  /*! Preamble Pwr used for transmission of PRACH sequence Range: TBD */
  int16 preamble_power;
  /*! Initial Preamble Power Level */
  int16 initial_preamble_power;
  /*! CE Level indicated by MAC as per the RSRP meas/thresh*/
  nb1_cphy_ce_level_type_e ce_level;
  /* This flag is set by MAC for Connecte mode RACH max retry attempt case */
  boolean  max_retry_conn_rach_attempt;
  
  // Cyclic Prefix lenght
  nb1_l1_prach_cp_len_e cp_len;
	  
  uint8 num_ra_rsp_win_size;
  /*! Window size for reception of RACH message2.
	  Defined in 36.331 Section 6.3.2 - GenericRandomAccessParams */
  nb1_l1_ra_rsp_win_size_e	ra_rsp_win_size_new[NB1_MAX_NPRACH_RESOURCE];
  // N PRACH Config
  
  uint8 prach_param_ce_list_size;

  nb1_l1_nprach_param_s* prach_param_ce_list[NB1_MAX_NPRACH_RESOURCE];

  uint8 ul_carrier_idx;
  
  uint8 dl_carrier_idx;
  
  /*! flag to indicate if network enable enhanced random access power control */ 
  boolean enable_enhanced_ra_pwr_cntrl;                                

} nb1_cphy_start_rach_req_s;


/*! @brief
  This message is sent to MAC from ML1 to confirm that ML1 receives the
  NB1_ML1_START_RACH_REQ and lets MAC knows if ML1 can perform the request
  or not.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Indicates success/failure for START_RACH_REQ received from MAC. */
  boolean result;

#ifdef FEATURE_NB1_CSS_USS_MUX
/*
1. Find max of USS period and RAR window duration to be sent with START_RACH_CNF – T
2. Do M RACH attempts and then switch to monitor USS for N*T duration.
   Continue this pattern until RACH process is concluded. 
3. M and N should be configurable from EFS 
4. At present M = 1, N = 0. Recommend default value of N =2 and M = 1.
*/
  uint32  uss_monitor_time;
#endif
} nb1_cphy_start_rach_cnf_s;


/*! @brief
  This message is sent to ML1 from MAC when MAC is about to start a RACH
  procedure and wants to request ML1 for the radio condition measurement.
  The radio condition helps MAC to select the PREAMBLE index group.
*/
typedef struct
{
  /*! @todo Fill in these pameters */
  msgr_hdr_struct_type msgr_hdr;

} nb1_cphy_rach_rc_req_s;


/*! @brief
  This message is sent to MAC from ML1 to report the radio condition
  measurement.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! TBD */
  int16 rc_measurement;

  /*to determine CE level.*/
  nb1_cphy_conn_meas_rsrp_t rsrp_measurement;
} nb1_cphy_rach_rc_cnf_s;


/*! @brief
  This message is send to ML1 from MAC when MAC gets a match in message 2.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Indicate if there is a new TA value */
  boolean ta_included;
  /*! Indicate if there is a T-RNTI */
  boolean trnti_included;
  /*! Timing advance to adjust UL timing Range: TDB */
  uint16 timing_advance;
  /*! Temporary RNTI assigned as part of the Message 2 for decoding Message 3 */
  nb1_rnti_val_t temp_rnti;
  /*! The 20 bit grant for msg 3 (shouldn't this be a range?) */
  uint32 grant;
  /* The system time at the time of receiving the RAR */
  nb1_l1_cell_systime_s  receive_time;
  /* TRUE when NW timeAlignmentTimer is configured as infinity, FALSE otherwise */
  boolean is_ta_timer_infinity;
  /* a counter to count how many times contention failure has happened for this MAC RACH procedure */
  uint16 num_ct_failure;
} nb1_cphy_rar_params_req_s;

/*! @brief
  This message is send to ML1 from MAC to indicate that there is no matched RAR decode for
  RA-RNTI TB received .
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  uint16               sfn;
} nb1_cphy_mismatch_rar_ind_s;

typedef enum
{
  NB1_ML1_SUSPEND_RACH,
  NB1_ML1_RESUME_RACH
} nb1_cphy_suspend_resume_rach_type_e;

/*! @brief
  This message is send from ML1 to MAC to continue RACH procedure.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  nb1_cphy_suspend_resume_rach_type_e suspend_resume_type;
} nb1_cphy_srch_rach_suspend_resume_ind_s;

/*! @brief
  This message is sent to MAC from ML1 when ML1 receives a PDCCH order for
  DL data arrival.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Dedicated Subcarrier index to use Valid Range:0-0x2F Invalid Value = 0xFF */
  uint8 subcarrier_index;

  nb1_cphy_ce_level_type_e ce_level;

  boolean non_anchor_configured;

  uint8 carrier_ind;
} nb1_cphy_pdcch_order_ind_s;

/*! @brief
  This message is sent to MAC from ML1 when ML1 transmits MSG3 of RACH
  procedure .
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether or not the MSG3 transmission was successful */
  errno_enum_type      status;

  /*! delay to start MAC contention resolution timer after received this message
      , unit is ms */
  uint16               cont_res_tmr_start_delay_in_ms;
} nb1_cphy_msg3_transmission_ind_s;


/*! @brief
  This message is sent to MAC from ML1 when the RA timer is expired.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
} nb1_cphy_ra_timer_expired_ind_s;

/*! @brief
  Sent when the downlink becomes weak when a cell is acquired.

  This is used to notify upper-layers that the cell quality has become poor.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Frequency of the cell; used with phy cell ID to identify
      the cell */
  nb1_earfcn_t               freq;

  /*! Physical cell id of the cell; used with freq to identify
      the cell */
  nb1_phy_cell_id_t          phy_cell_id;
} nb1_cphy_dl_weak_ind_s;


/*! @brief
  Sent when the RL Failure is detected.

  This is used to notify upper-layers to start Re-etablishment procedure.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msg_hdr;

  /*! rlf reason */
  nb1_l1_rlf_reason_e rlf_reason;

} nb1_cphy_rl_failure_ind_s;

/*! @brief
  This msg is sent by ML1 to RRC when a better neighbor cell is
  found in IDLE state or in the case of DO reverse time transfer.
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! current serving cell info */
  nb1_cphy_cell_info_s   serv_cell_info;

  /*! resel target cell info */
  nb1_cphy_resel_target_cell_info_s  target_cell_info;

 /*! The time at which this indication is sent */
  nb1_ref_time_s  sfn;

} nb1_cphy_cell_resel_ind_s;

/*! @brief
    This msg is sent by ML1 to RRC when the NB1 CSG cell is not good
    enough to reselect
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

} nb1_cphy_irat_asf_needed_ind_s;

/*! @brief
    This msg is sent by RRC to ML1 to notify the PCI range to be blocked,
    if CSG feature is not supported
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Number of CSG PCI ranges */
  uint8 num_black_cell_freq;

  /*! CSG PCI range */
  nb1_cphy_meas_eutra_freq_pci_range_s black_cell_freq[NB1_CPHY_MEAS_NUM_NB1_CSG_FREQS];

} nb1_cphy_blacklisted_csg_pci_range_ind_s;

/*! @brief
    This msg is sent by ML1 to RRC when it detects OOS condition in
    IDLE state
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! current serving cell info */
  nb1_cphy_cell_info_s  serv_cell_info;

  /*! Target cell */
  nb1_cphy_conn_release_acq_cell_info_s target_cell_info;

  /*! OOS reason */
  nb1_l1_oos_reason_e oos_reason;

} nb1_cphy_oos_ind_s;

/*! @brief
  This message is sent to ML1 from MAC when MAC decides to abort the RACH
  procedure and requests ML1 to do so.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! flag to indicate whether ML1 should continue process C-RNTI grant
      and only drop RACH
      TRUE: ML1 only drop RACH and keep process C-RNTI DCI grant
      FALSE: ML1 drop all channels
  */
  boolean process_grant;
} nb1_cphy_rach_abort_req_s;


/*! @brief
  This message is sent to MAC from ML1 to confirm with MAC that the request
  to abort RACH procedure is done.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
} nb1_cphy_rach_abort_cnf_s;


/*! @brief
  This message is sent to ML1 from MAC when MAC wants to update ML1 a new
  RACH configuration.
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! TRUE to indicate there is a new RACH config.
  FALSE to indicate no RACH config also
  the existing RACH config should be trashed*/
  boolean      rach_cfg_included;
  /*! ML1 RACH configuration pointer */
  nb1_cphy_prach_setup_param_s  rach_cfg;
} nb1_cphy_rach_cfg_req_s;


/*! @brief
  This message is sent to MAC from ML1 to confirm with MAC that ML1 has
  received the new RACH configuration from MAC.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  errno_enum_type      status; /*!< status of configuration */
} nb1_cphy_rach_cfg_cnf_s;


/*! @brief Message sent from ML1 to RRC to inform RRC that a phychan schedule
    request has been sent to LL1 for RACH MSG1
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
}nb1_cphy_rach_msg1_sched_ind_s;


/*! @brief Used to tell L1 that the uplink has gone out of sync. This
  should be sent from MAC */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type     hdr;

  /*! STAG-Id-r11	 Range (1..maxSTAG-r11). Set to 0 for Pcell TAG */
  uint8  stag_id;
} nb1_cphy_out_of_sync_req_s;


typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /*! STAG-Id-r11	 Range (1..maxSTAG-r11). Set to 0 for Pcell TAG */
  uint8                      stag_id;

  /*! The timing Advance */
  uint16                     timing_advance;

  /*! The timing on which the TA command was received by MAC*/
  nb1_l1_cell_systime_s      ta_rcvd_timing;

  /*! Flag to indicate the origin of the TA value FALSE: From TA_CMD,
      TRUE: From MSG2 during RACH */
  boolean                    is_ta_frm_msg2;
} nb1_cphy_apply_ta_params_req_s;


/*! @brief This message is sent from MAC to ML1 to update the TA value if BCR condition is met
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;

  /* TA value to be used */
  int16 ta_adjust_val;

} nb1_cphy_ra_one_off_ind_s;

/*! @brief MAC Scell Activation/De-activation config sent by MAC to L1 */
typedef struct
{
  /* Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /* Octet of bits indicating active/inactive status for 8 SCells  */
  uint8                      scell_status_mask;

  /*! System Frame number */
  nb1_l1_sfn_t               sys_fn;

  /*! Subframe number */
  nb1_l1_subfn_t             sub_fn;
} nb1_cphy_scell_status_ind_s;

/*
* @brief Parameter struct for NB1_CPHY_HARQ_ACK_TX_STS_REQ. If there?
is a request from RRC to check if RLC ACK for Conn release has reached enB?
then RLC will send this request to ML1 passing HARQ_ID on?
which last SRB1 status PDU was built.indication will ensure DL to rebuilt the control
* PDU.
*/
typedef struct
{
  msgr_hdr_struct_type    msgr_hdr; /*!< Message header */
  uint8                   harq_id; /*!< Logical channel ID */
}nb1_cphy_harq_ack_tx_sts_req_s;

/*! @brief TX status confirmation which RRC listens */
typedef struct
{
  msgr_hdr_struct_type    msgr_hdr; /*!< Message header */
  errno_enum_type         status; /*!< Logical channel ID */
}nb1_cphy_harq_ack_tx_sts_cnf_s;


//Raster Offset types in NB
typedef enum
{
  NB1_RASTER_OFFSET_MINUS_7_5_KHZ,
  NB1_RASTER_OFFSET_MINUS_2_5_KHZ,
  NB1_RASTER_OFFSET_2_5_KHZ,
  NB1_RASTER_OFFSET_7_5_KHZ
}nb1_raster_offset_type_e;

//guard band params in NB
typedef struct
{
  nb1_raster_offset_type_e raster_offset;
}nb1_guardband_params_s;

//Inband same PCI params in NB
typedef struct
{
  //CRS sequence info for In band with same PCI. Valid values from (0 to 31)
  uint8 crs_sequence_info;
}nb1_inband_samepci_s;

//Inband with Different PCI params in NB
typedef struct
{
  nb1_eutra_num_crs_ports_e num_crs_ports;
  nb1_raster_offset_type_e raster_offset;
}nb1_inband_diffpci_s;

//Unknown parameters
//Must keep structure definition for PLT
typedef struct
{
  uint8 pad;
}nb1_unknown_params_s;

//Standalone parameters
//Must keep structure definition for PLT
typedef struct
{
  uint8 pad;
}nb1_standalone_params_s;

/* Operation mode Information in MIB*/
typedef union
{
  // Must keep structure definition for PLT
  nb1_unknown_params_s unknown_params;

	//Applicable when Operation mode is in InBand Same PCI
	nb1_inband_samepci_s ib_samepci_params;      //Applicable if nb_operation_mode_e is 1

	//Applicable when Operation mode is in InBand Diff PCI
	nb1_inband_diffpci_s ib_diffpci_params;      //Applicable if nb_operation_mode_e is 2

	//Applicable when Operation mode is in guardBand
	nb1_guardband_params_s gb_params;      //Applicable if nb_operation_mode_e is 3

  // Must keep structure definition for PLT
  nb1_standalone_params_s standalone_params;
}nb1_cphy_operation_mode_info_u;

/*! @brief SIB Schedule Request from RRC to L1

    @detail RRC sends this message to request MIB/SIB decoding from L1. RRC
    includes a semi-static SIB schedule, and bitmasks that indicate which SIBs
    should be received. As SIBs are received, RRC can disable further
    reception by removing them from the bitmasks and sending another request.
    One bitmask controls which SIBs are received in the current modification
    period; this bitmask is always applied immediately by L1 to determine how
    to schedule SI message reception. Another bitmask controls which SIBs are
    received in the next modification period. This can be used to trigger a
    synchronized transition to a new schedule at a modification period
    boundary. RRC specifies the modification period boundary SFN in this case.
    L1 also sends a Boundary Indication to RRC if RRC includes the boundary
    SFN; this facilitates invalidation of parameters when SIBs change. Note
    that MIB decoding is implicit; L1 assumes that whenever SIB1 reception is
    active, it should also receive MIB. If RRC removes SIB1 from the bitmask,
    L1 assumes that it can also stop decoding MIB. If MIB is successfully
    decoded, L1 will also send a MIB Indication.
     */
typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /** Frequency of the cell; used with phy cell ID to identify
   *  the cell */
  nb1_earfcn_t               freq;

  /** Physical cell id of the cell; used with freq to identify
   *  the cell */
  nb1_phy_cell_id_t          phy_cell_id;

  /** SIB bitmask used in current modification period. It
   *  defines the SIBs that L1 must receive, and is applied
   *  immediately. While SIB collection is active, it always
   *  supersedes the mask sent in any previous requests. */
  nb1_cphy_sib_mask_t        curr_mod_prd_sib_mask;

  /** Modification period boundary SFN. If set to a valid value,
   *  L1 will send a Boundary Indication at that SFN and activate
   *  the next_mod_prd_sib_mask. At the boundary SFN, the
   *  next_mod_prd_sib_mask will supersede the
   *  curr_mod_prd_sib_mask sent in this request. If the
   *  modification period boundary is unknown, or no Boundary
   *  Indication is needed, this field must be set to
   *  NB1_CPHY_NULL_SFN. */
  nb1_l1_sfn_t               mod_period_boundary_sfn;


  /** Modification period boundary Hyper SFN. If set to a valid value,
   *  L1 will send a Boundary Indication at that Hyper SFN combined with
   *  modification boundary sfn value and activate the next_mod_prd_sib_mask.
   *  At the boundary Hyper SFN and mod_period_boundary_sfn, the
   *  next_mod_prd_sib_mask will supersede the
   *  curr_mod_prd_sib_mask sent in this request. If the
   *  modification period boundary Hyper SFN is unknown, or no Boundary
   *  Indication is needed, or legacy mod period boundary needs to be used,
   *  this field must be set to NB1_CPHY_HYPER_NULL_SFN. */

   nb1_l1_hyper_sfn_t        mod_period_bnd_hyper_sfn;

  /** Last SFN/SUB FN/HSFN read by RRC in SIBs or Page ( EDRX cases).
  *  Helpful in disambiguating modification boundary HSFN/SFN 
     if SFN wraps around when we receive this request.
  */
  nb1_l1_sfn_t               last_sfn_in_sib_rsp;

  nb1_l1_subfn_t             last_subfn_in_sib_rsp;

  nb1_l1_hyper_sfn_t	     last_hsfn_in_sib_rsp;


  /** SIB bitmask to be used in the next modification period,
   *  i.e. after the specified boundary SFN. Before that SFN,
   *  the curr_mod_prd_sib_mask will be active. At the boundary
   *  SFN, L1 will immediately switch to the
   *  next_mod_prd_sib_mask. If the boundary SFN is set to
   *  NB1_CPHY_NULL_SFN, the next_mod_prd_sib_mask is ignored. */
  nb1_cphy_sib_mask_t        next_mod_prd_sib_mask;

 /** SI message window length in ms as specified in SIB1 */
 nb1_cphy_si_window_length_e si_window_length;

 /** Number of SI messages in the schedule as specified in
  *  SIB1; can be 0 if schedule is unknown, e.g. before SIB1
  *  is received. */
 uint8 num_si_msgs;

 /** SI scheduling information from SIB1. This is always the
  *  absolute schedule defined by the network; the sib masks
  *  are used to determine which SI message needs to be received.
  *  MIB and SIB1 have fixed schedules and should not be included
  *  in this structure. Note that if a modification period
  *  boundary is specified, this schedule applies to both the
  *  current and next modification period. Also note this
  *  schedule can be empty, e.g. before SIB1 has been
  *  received. */
 nb1_cphy_si_msg_info_s      si_msg_info[NB1_CPHY_MAX_SI_MSGS];

  /** Sib1 scheduling Info as specific in MIB. Will be specified when
     Sib1 is scheduled to be read. Valid values 0 - 15. FF if not
	 obtained from MIB yet*/
  uint8 sib1_sch_info;

  /*NB operation mode type. Set to now known if MIB is not read*/
  nb1_l1_operation_mode_e op_mode_type;

  /*Choice based on op_mode_type*/
  nb1_cphy_operation_mode_info_u op_mode_param;

  /** SI radio frame offset in Sib1. Will be specified when
     Sib1 is already read. Valid values 1 - 15. FF if not
	 obtained from SIB1 / Not present*/
  uint8 si_radio_frame_offset;
}nb1_cphy_sib_sched_req_s;

/*! @brief MIB indication from L1 to RRC.

    @detail MIB indications are sent upon successful PBCH decode whenever RRC
    requests the MIB in the SIB Schedule Request (Note that MIB is implicitly
    requested whenever SIB1 is requested). While MIB decoding is active,
    this indication is sent upon the first successful decode, or any time the
    MIB changes (SFN change is not considered a MIB change).

*/

/*! @brief This message is sent by MAC whenever it receives Timing Advance command
           over the downlink
*/
typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /** ASN.1-decoded MIB */
  nb1_l1_nb_mib_info_param_s mib;

  /** Frequency of the cell */
  nb1_earfcn_t               freq;

  /** Physical cell id of the cell */
  nb1_phy_cell_id_t          phy_cell_id;

  /** SFN on which MIB was received and decoded on PBCH */
  nb1_l1_sfn_t               sfn;

}nb1_cphy_mib_ind_s;

/*! @brief Modification period boundary indication from L1 to RRC

    @detail Boundary Indications are sent when RRC include the modification
    period boundary SFN in the SIB Schedule Request. When the boundary is
    reached, L1 sends this indication. This is intended to facilitate the
    invalidation of SI parameters upon SIB change.
*/
typedef struct
{
  /** Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /** Frequency of the cell */
  nb1_earfcn_t               freq;

  /** Physical cell id of the cell */
  nb1_phy_cell_id_t          phy_cell_id;

  /** SFN on which this boundary occurred */
  nb1_l1_sfn_t               sfn;

}nb1_cphy_mod_prd_boundary_ind_s;

/*!
  @brief
  This message is used by RRC/ML1 to request/respond for MBMS neigh freqs in the order of its cell power
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Num of eutra freqs */
  uint8 num_freq;

  /*! nb1 frequency list */
  nb1_earfcn_t freq[NB1_CPHY_MEAS_MAX_NUM_FREQ];

} nb1_cphy_mbms_neigh_freq_s;

/*! @brief
  RRC will pass a maximum of 8 earfcns to ML1 to get them arranged in order of best cell power
*/
typedef nb1_cphy_mbms_neigh_freq_s nb1_cphy_best_mbms_neigh_freq_req_s;

/*! @brief
  ML1 will return the valid earfcn(s) in the order of cell power. num_freq could be zero in case
  ML1 couldn't find any earfcn with valid cell info
*/
typedef nb1_cphy_mbms_neigh_freq_s nb1_cphy_best_mbms_neigh_freq_cnf_s;

/*! @brief
  This message is sent to MAC from ML1 to report the radio condition
  measurement.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! TBD */
  int16 rc_measurement;

  /*to determine CE level.*/
  nb1_cphy_conn_meas_rsrp_t rsrp_measurement;

  /* ce_rpt req or ce_lvl req */
  uint32 req_rcvd_umid;
} nb1_cphy_ce_lvl_req_s;


typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  nb1_l1_nprach_param_s* prach_param_ce_list;

  nb1_cphy_ce_level_type_e ce_lvl;

  /* ce_rpt req or ce_lvl req */
  uint32 req_rcvd_umid;
} nb1_cphy_ce_lvl_rsp_s; 



/******************************************************************************
                              Suspend/Resume structs
*******************************************************************************/

/*! @brief Suspend/resume cause enum
*/
typedef enum
{
  NB1_CPHY_IRAT_1X_RESEL,           /*!<  L->1X [CDMA1XRTT] resel */
  NB1_CPHY_IRAT_DO_RESEL,           /*!<  L->DO [CDMAHRPD] resel */
  NB1_CPHY_IRAT_WCDMA_RESEL,        /*!<  L->WCDMA [UTRAN] resel */
  NB1_CPHY_IRAT_TDSCDMA_RESEL,        /*!<  L->TDSCDMA [UTRAN] resel */
  NB1_CPHY_IRAT_GERAN_RESEL,        /*!<  L->GERAN [GSM] resel */
  NB1_CPHY_IRAT_1X_HANDOVER,        /*!<  L->1X [CDMA1XRTT] handover */
  NB1_CPHY_IRAT_DO_HANDOVER,        /*!<  L->DO [CDMAHRPD] handover */
  NB1_CPHY_IRAT_WCDMA_HANDOVER,     /*!<  L->WCDMA [UTRAN] handover */
  NB1_CPHY_IRAT_GERAN_HANDOVER,     /*!<  L->GERAN [GSM] handover */
  NB1_CPHY_IRAT_BPLMN,              /*!<  L->other RAT BPLMN */
  NB1_CPHY_IRAT_CDMA_REVERSE_TIME_TRANSFER, /*!<  L->CDMA reverse time transfer */
  NB1_CPHY_IRAT_GERAN_CCO_NACC,     /*!<  L->G RAT cell change order recvd in NB1 conncted mode  */
  NB1_CPHY_IRAT_CGI,                /*!<  L->other RAT CGI */
  NB1_CPHY_IRAT_TDSCDMA_HANDOVER,   /*!<  L->WCDMA [UTRAN] handover */
  NB1_CPHY_TX_UNAVAILABILITY,       /*!<  Tx unavailability */
}nb1_cphy_suspend_resume_cause_e;

/*! @brief CPHY suspend req struct
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! Suspend cause */
  nb1_cphy_suspend_resume_cause_e   cause;

  /*! Reserve TRM for exchange*/
  boolean                           trm_irat_exchange;

  /*! TBD: More params need to be added like RAT specific target cell info,
      PLMN related etc. if needed
  */
}nb1_cphy_suspend_req_s;

/*! @brief CPHY suspend cnf struct
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /*! Suspend L1 status */
  errno_enum_type            status;

  /*! Suspend cause */
  nb1_cphy_suspend_resume_cause_e suspend_cause;

  /*! CDMA time info: Valid only if suspended for the same cause */
  nb1_stmr_cdma_rtc_pair     cdma_time_info;

  /*! The time at which this indication is sent */
  nb1_ref_time_s                   ref_time;

  /*! TBD: More params need to be added like RAT specific timing info,
      PLMN related etc. if needed
  */
}nb1_cphy_suspend_cnf_s;

/*! @brief CPHY resume req struct
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type               msgr_hdr;

  /*! Resume cause: should match with suspend cause */
  nb1_cphy_suspend_resume_cause_e    cause;

  /*! TRM Timeout Value in ms
          For Initial CPHY_RESUME_REQ, timeout val = 500ms
          If Initial CPHY_RESUME fails, then next CPHY_RESUME_REQ's
            timeout val = 5s (5000 ms)*/
  uint16    trm_timeout_val;

  /*! TBD: More params need to be added like PLMN related etc. if needed
  */
}nb1_cphy_resume_req_s;

/*! @brief CPHY resume cnf struct
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type       msgr_hdr;

  /*! Resume L1 status
     If CPHY_RESUME_REQ fails due to TRM being not available for the
     duration of the timeout (500 ms), status should be set to E_TIMER_EXP.
     The status E_TIMER_EXP should not be used for other resume failure cases*/
  errno_enum_type            status;

  /*! Resume cause: should match with suspend cause */
  nb1_cphy_suspend_resume_cause_e    cause;

  /*! The time at which this indication is sent */
  nb1_ref_time_s                     ref_time;

  /*! TBD: More params if needed
  */
}nb1_cphy_resume_cnf_s;


#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
/* Map to SYS_MAX_EARFCN_SCAN_REQ */
#define NB1_CPHY_MAX_WWAN_CANDIDATES 60

/* Map to SYS_MAX_PCI_SCAN_REQ */
#define NB1_CPHY_MAX_PCI_SCAN_PER_CHANNEL 8

typedef enum
{ /* will add all enum later currently not sure about it */
 NB1_OFFSET_MINUS_TEN,
 NB1_OFFSET_MINUS_NINE,
 NB1_OFFSET_MINUS_EIGHT,
 NB1_OFFSET_MINUS_SEVEN,
 NB1_OFFSET_MINUS_SIX,
 NB1_OFFSET_MINUS_FIVE,
 NB1_OFFSET_MINUS_FOUR,
 NB1_OFFSET_MINUS_THREE,
 NB1_OFFSET_MINUS_TWO,
 NB1_OFFSET_MINUS_ONE,
 NB1_OFFSET_ZERO,
 NB1_OFFSET_PLUS_ONE,
 NB1_OFFSET_PLUS_TWO,
 NB1_OFFSET_PLUS_THREE,
 NB1_OFFSET_PLUS_FOUR,
 NB1_OFFSET_PLUS_FIVE,
 NB1_OFFSET_PLUS_SIX,
 NB1_OFFSET_PLUS_SEVEN,
 NB1_OFFSET_PLUS_EIGHT,
 NB1_OFFSET_PLUS_NINE
}nb1_cphy_earfcn_offset_e_type;


typedef enum
{
  
  NB1_SET_RASTER_OFFSET_MIN_7_DOT_5_KHZ, /* -7.5 kHz */
  NB1_SET_RASTER_OFFSET_MIN_2_DOT_5_KHZ, /* -2.5 kHz */
  NB1_SET_RASTER_OFFSET_2_DOT_5_KHZ,  /*  2.5 kHz */
  NB1_SET_RASTER_OFFSET_7_DOT_5_KHZ   /*  7.5 kHz */
  
}nb1_cphy_raster_offset_e_type;

/*! @brief
     Data structure for WWAN control PCI PARAMs */
typedef struct
{
  /*! the DL earfcn that the cell is on */
  boolean is_system_frame_offset_present;

  boolean is_earfcn_offset_present;

  boolean is_raster_offset_present;
    
  /* Physical cell id (0 to C_MAX_PHYSICAL_CELL_ID) */
  nb1_phy_cell_id_t       cell_id;

  nb1_cphy_earfcn_offset_e_type nb1_earfcn_offset;

  nb1_cphy_raster_offset_e_type nb1_raster_offset;
  
  /* Cell timing system frame offset */
  uint32 system_frame_offset;
  
} nb1_cphy_wwan_pci_param_s;


/*! @brief
     Data structure for WWAN control EARFCN PARAMs */
typedef struct
{
  /* decode SIB1*/
  boolean decode_sib1;

  /* measure RSRP or RxLevel also get cell timing with power  */
  boolean  measure_power_and_time;

  /* number of cells to return on this channel overriding default limit */
  uint8 max_num_of_return_pci;

  /* Per channel assistance data and controls */
  uint8   num_pci;

  nb1_cphy_wwan_pci_param_s  wwan_scan_ad_per_pci_nb1[NB1_CPHY_MAX_PCI_SCAN_PER_CHANNEL];

} nb1_cphy_wwan_candidate_param_s;

#endif
/*! @brief
     Data structure for PLMN earfcn number and its bandwidth.
*/
typedef struct
{
  /*! channel nunmber is EARFCN number */
  nb1_earfcn_t         chan;

  /*! the bandwidth info is needed for system scanning */
  nb1_bandwidth_e      bw;

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
  nb1_cphy_wwan_candidate_param_s wwan_candidate_info_list;
#endif
} nb1_cphy_bplmn_chan_s;


/*! @brief
     Data structure for PLMN frequency number and its bandwidth
*/
typedef struct
{
  /*! in units of 100Khz */
  uint32               freq;

  /*! the band information is needed to convert freq int earfcn */
  sys_sband_lte_e_type band;

  /*! the bandwidth information might not be needed. this is a
      place holder. the number is in units of 100Khz
   */
  uint32               bw;
} nb1_cphy_bplmn_freq_s;

/*! @brief enumeration to identify BPLMN search cause type
*/
typedef enum
{
  /*! UE active on NB1, PLMN request in NB1 */
  NB1_CPHY_BPLMN_SEARCH_CAUSE_NB1_DEFAULT,

  /*! UE active on NB1, PLMN search is due to MARB LOAD request in NB1 */
  NB1_CPHY_BPLMN_SEARCH_CAUSE_MARB_LOAD,

  /*! UE active on NB1, PLMN search is due to MARB UNLOAD request in NB1 */
  NB1_CPHY_BPLMN_SEARCH_CAUSE_MARB_UNLOAD,
} nb1_cphy_bplmn_search_cause_e;

/*! @brief
    Data Structure for NB1_CPHY_BPLMN_START_REQ

    @details
      RRC uses this to send a BPLMN request. depending on the scenario,
      it may pass in new request data, or indicate ML1 to use previous
      request data to continue the PLMN search.

*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! if new_req is TRUE, ML1 shall remove previous status, if any, and
      initialize the request with the data given by this request.
      if new_req is FALSE, do not update the status of the previous request.
      if new_req is FALSE, and no previous request had been received,
      ML1 shall treat it as an empty request
    */
  boolean                          new_req;

  /*! request type */
  nb1_cphy_bplmn_type_e            req_type;

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
  boolean                          is_wwan_search;
#endif
  /*! Priority of the PLMN search, if set to NB1_CPHY_BPLMN_PRIORITY_HIGH
    BPLMN search should be prioritised */
  nb1_cphy_bplmn_priority_e        priority;

  /* the following fields will be meaningful only to NB1 to NB1 and IRAT
     to NB1 cases
   */

  /*! if start_now is TRUE, ML1 starts the PLMN search immediately
      if start_now is FALSE, ML1 starts the PLMN search when it is appropriate
      based on the NB1 activity on ML1 side.
      When RRC can not decide which one to use, it is always a safe way to use
      FALSE. in this case, if ML1 can start it immediately, it will.

      specifically, RRC sets this to be TRUE when it expects a foreground PLMN,
      while FALSE when it expects a background PLMN
   */
  boolean                         start_now;

  /*! number of frequency included in this request */
  uint8                           num_chans;

  /*! the list of channels to be searched for PLMN */
  nb1_cphy_bplmn_chan_s          chans[NB1_CPHY_BPLMN_MAX_FREQS];

  /*! number of bands included in this request */
  uint8                           num_bands;

  /*! the bands to be searched for PLMN */
  sys_sband_lte_e_type            bands[NB1_CPHY_BPLMN_MAX_BANDS];

  /*! the number of frequency in the list that ML1 should not search on */
  uint8                           num_black_freqs;

  /*! the list of frequency that ML1 should not search on for PLMN, even
      if it appears in the band scan result list
   */
  nb1_cphy_bplmn_freq_s           black_freqs[NB1_CPHY_BPLMN_MAX_FREQS];
  
  /* manual PCI search 
  Cant be used along with CSG search */
  boolean                         pci_scan;
  
  /*!Max Num PCIs to report per scan freq
  PCIs reported are assumed to be ranked in the order reported
  Applicable only when cell_scan is set to TRUE */
  uint8                           max_num_pci;

  /*! Start cause type */
  nb1_cphy_bplmn_search_cause_e   start_cause;
} nb1_cphy_msg_bplmn_start_req_s;

/*! @brief
    Data Structure for NB1_CPHY_BPLMN_STOP_REQ

    @details
      RRC uses this to instruct ML1 to stop any PLMN search activities.
      ML1 should stop all the current PLMN activities if any. Further more,
      ML1 should not start any new PLMN acticities until next
      NB1_CPHY_BPLMN_START_REQ.

      ML1 keeps the current request and the intermediate PLMN
      search status.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! Stop cause type */
  nb1_cphy_bplmn_search_cause_e    stop_cause;
} nb1_cphy_msg_bplmn_stop_req_s;

/*! @brief
    Data Structure for NB1_CPHY_BPLMN_SUSPEND_REQ

    @details
      RRC uses this to instruct ML1 to suspend any PLMN search activity.
      ML1 should suspend all the current PLMN activities if any. Further more,
      ML1 should not start any new PLMN acticity until next
      NB1_CPHY_BPLMN_START_REQ.

      ML1 keeps the current request and the intermediate PLMN
      search status.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;
} nb1_cphy_msg_bplmn_suspend_req_s;

/*! @brief
    Data Structure for NB1_CPHY_BPLMN_CELL_REQ

    @details
       RRC uses this command to:
         a) indicate the cell is barred; or
         b) indicate the frequency is barred; or
         c) if the cell is not barred, ask ML1 to calculate rsrp and Srxlev
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! the frequency that the cell is on */
  nb1_earfcn_t                     freq;

  /*! the Updated frequency caluculated from SIB-1 Band indicator */
  nb1_earfcn_t                     updated_freq;

  /*! the cell ID */
  nb1_phy_cell_id_t                cell_id;

   /*! this can be set to be one of the values
       in the nb1_cphy_bplmn_cell_barred_type_e
   */
  nb1_cphy_bplmn_cell_barred_type_e  barred_status;


  /*! this is the cell configuration to help calculating Srxlev
      TODO: does RRC have the information of this?
   */
  nb1_cphy_cell_select_cfg_s      cell_select_cfg;

  /** SI message window length in ms as specified in SIB1 */
  nb1_cphy_si_window_length_e     si_window_length;

  /** Number of SI messages in the schedule as specified in
   *  SIB1; can be 0 if schedule is unknown, e.g. before SIB1
   *  is received. */
  uint8                           num_si_msgs;

  /** SI scheduling information from SIB1. This is always the
   *  absolute schedule defined by the network; the sib masks
   *  are used to determine which SI message needs to be received.
   *  MIB and SIB1 have fixed schedules and should not be included
   *  in this structure. Note that if a modification period
   *  boundary is specified, this schedule applies to both the
   *  current and next modification period. Also note this
   *  schedule can be empty, e.g. before SIB1 has been
   *  received. */
  nb1_cphy_si_msg_info_s          si_msg_info[NB1_CPHY_MAX_SI_MSGS];

} nb1_cphy_msg_bplmn_cell_req_s;


/*! @brief
    Data Structure for NB1_CPHY_BPLMN_CELL_CNF

    @details
      ML1 sends this CNF to RRC in response to NB1_CPHY_BPLMN_CELL_REQ.

      RRC expects this CNF message only when the cell and the frequency
      are not barred. please also refer to NB1_CPHY_BPLMN_CELL_REQ
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! the DL earfcn that the cell is on */
  nb1_earfcn_t                     earfcn;

  /*! the frequency that the cell is on, in 100Hz */
  uint16                           freq;

  /*! the bandwidth info obtained from PLMN search */
  nb1_bandwidth_e                  bw;

  /*! the cell ID */
  nb1_phy_cell_id_t                cell_id;

  /*! the measurement of RSRP */
  int16                            rsrp;

  /*! the measurement of RSRQ */
  int16                            rsrq;

  /*! the measurement of RSSI */
  int16                            rssi;

  /*! TRUE Srxlev is calculated, FALSE if the value is not supposed to be used
      only when RRC expects a CNF message even when the cell is barred or the
      frequency is barred. in that case, it sets the srxlev_calculated to be FALSE,
      so the message is just serving as a CNF to the NB1_CPHY_BPLMN_CELL_REQ
   */
  boolean                         srxlev_calculated;

  /*! the Srxlev value. meaningful only when srxlev_calculated is TRUE */
  int16                           srxlev;

  /*! Flag to indicate if release 9 parameters are present or not */
  boolean rel9_param_present;

  /*! the squal parameter appears only for Release 9 */
  int16   squal;
} nb1_cphy_msg_bplmn_cell_cnf_s;

/*! @brief
    Data Structure for NB1_CPHY_BPLMN_SIB_RECEIVED_IND

    @details
      RRC sends this command to let ML1 know that SIB9 was received
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;
} nb1_cphy_msg_bplmn_sib_received_ind_s;

/*! @brief
    Data Structure for NB1_CPHY_BPLMN_SKIP_REQ

    @details
      RRC uses this command to let ML1 continue its own NB1 activity without
      having to suspend for IRAT PLMN search.

      Usually when ML1 sends NB1_CPHY_BPLMN_TIME_AVAIL_IND to RRC, RRC either
      suspends ML1 so that IRAT can perform PLMN search, or sends
      NB1_CPHY_BPLMN_SKIP_REQ to ML1 in case RRC decides not to give the time
      to IRAT to perform the PLMN search.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;
  /* Flag to indicate that ML1 should stay dormant for the full duration of time given in the time available indication */
  boolean                          disable_asap_resched;
} nb1_cphy_msg_bplmn_skip_req_s;

/*! @brief
    Data Structure for NB1_CPHY_BPLMN_TIME_AVAIL_IND

    @details
      ML1 sends this indication to RRC in response to NB1 to IRAT PLMN Search.
      When ML1 sends this indication to RRC, it means ML1 is ready to be
      suspended for IRAT to perform PLMN search.

      after receiving this, RRC should either suspend ML1, or send
      NB1_CPHY_BPLMN_SKIP_REQ to ML1 in case it decides not to give
      the time to IRAT to perform the PLMN search.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! the time available for NB1 to IRAT PLMN search. if "unlimited"
      time is possible (say in OOS scenario), use NB1_CPHY_BPLMN_UNLIMITED_TIME
      the unit is milli-seconds
   */
  uint32                          time_avail;
} nb1_cphy_msg_bplmn_time_avail_ind_s;



/*! @brief
    Data Structure for NB1_CPHY_BPLMN_COMPLETE_IND

    @details
      ML1 sends this indication to RRC in the following cases:
        1). in NB1 to NB1 case, all the PLMN search is done
        2). in IRAT to NB1 case, the PLMN search is partially done
            due to the given time expired.
        3). PLMN search failed
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! for NB1 to NB1, NB1 to IRAT, it has to be NB1_CPHY_BPLMN_ALL_DONE

      for IRAT to NB1, if the PLMN search is partially done during the given time,
      the status is NB1_CPHY_BPLMN_PARTIAL_DONE. if the PLMN request is
      completely done, the status is NB1_CPHY_BPLMN_ALL_DONE.

      if any errors occur during the PLMN search and PLMN search can not continue,
      the status is NB1_CPHY_BPLMN_FAIL
  */
  nb1_cphy_bplmn_complete_type_e  status;

  /*! if serving cell available, ML1 also attaches serving cell measurement info
      into the BPLMN COMPLETE IND
   */

  /* a flag to indicate whether serving cell is available
     normally, if it is a BPLMN, this flag should be TRUE, and for foreground
     PLMN cases, this flag should be FALSE

     only when this flag is TRUE, the next field of srv_cell_meas is meaningful
   */
  boolean srv_cell_avail;

  /* we re-use the CELL CNF payload to carry the serving cell measurement info */
  nb1_cphy_msg_bplmn_cell_cnf_s srv_cell_meas;
} nb1_cphy_msg_bplmn_complete_ind_s;

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
typedef enum
{
  /*! Default None */
  NB1_CPHY_BPLMN_EARFCN_SCAN_NONE,

  /*!  serving cell is not present and there is no sib decode request present in 
       given scan request */
  NB1_CPHY_BPLMN_EARFCN_SCAN_INVALID,

  /*! SUCCESS: If found atleast one cell for that earfcn */
  NB1_CPHY_BPLMN_EARFCN_SCAN_SUCCESS,

  /*! If no cell is found on that earfcn */
  NB1_CPHY_BPLMN_EARFCN_SCAN_NO_CELL_FOUND,

  /*! If ML1 received BPLMN STOP and search not started */
  NB1_CPHY_BPLMN_EARFCN_SCAN_NOT_STARTED,

  /*! If EARFCN BAND not present in band pref sent by NAS*/
  NB1_CPHY_BPLMN_EARFCN_SCAN_BAND_NOT_SUPPORTED,

  /*! If search aborted due to guard timer expiry - TBD */
  NB1_CPHY_BPLMN_EARFCN_SCAN_ABORTED_DUETO_GUARD_TIMER
}nb1_cphy_bplmn_earfcn_status_e;


#define NB1_CPHY_MAX_PCI_RESULT 8

typedef struct 
{
  boolean is_serving_cell;

  boolean is_earfcn_offset_present;

  boolean is_raster_offset_present;
    
  
/* Physical cell id (0 to C_MAX_PHYSICAL_CELL_ID) */
    /*! the cell ID */
  nb1_phy_cell_id_t                cell_id;

  boolean is_rx_power_meas_present;
  
  nb1_cphy_rx_power_meas_e_type    rx_power_meas_type;

  /*! the measurement of RSRP */
  int16                            rsrp;

  /*! Rxlevel measurement*/
  int16 rxlevel; /* -110 to -48 dBm */

  /* Cell observation delta time offset from start of scan. 
   * ML1 will explictly set 0xFFFFFFFF if delta time offset not present */
  uint32 delta_time_offset;
  
  /* Cell timing system frame offset    
   * ML1 will explictly set 0xFFFFFFFF if system_frame_offset not present */
  uint32  system_frame_offset;

  /* NB1 offset from downlink center frequency */
  nb1_cphy_earfcn_offset_e_type  nb1_earfcn_offset;

  nb1_cphy_raster_offset_e_type nb1_raster_offset;
}nb1_cphy_bplmn_earfcn_candidate_result_s;

/*! @brief
    Data Structure for LTE_CPHY_BPLMN_UNSOLICITED_EARFCN_STATUS_IND
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /* Channel number / downlink center frequency (0 to C_MAX_EXTENDED_EARFCN) */
  uint32  earfcn;

  nb1_cphy_bplmn_earfcn_status_e  status;

  uint8  num_result_per_earfcn;

  nb1_cphy_bplmn_earfcn_candidate_result_s candidates[NB1_CPHY_MAX_PCI_RESULT];

} nb1_cphy_bplmn_unsolicited_earfcn_status_ind_s;
#endif

/*! @brief
    Data Structure for NB1_CPHY_BPLMN_STOP_CNF

    @details
       ML1 uses this to indicate that PLMN search activities
       within ML1 is completely stopped.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  /*! the status has to be TRUE which means successful
       if it is FALSE, for now, RRC should ASSERT
   */
  boolean                          status;

  /*! if serving cell available, ML1 also attaches serving cell measurement info
      into the BPLMN COMPLETE IND
   */

  /* a flag to indicate whether serving cell is available
     normally, if it is a BPLMN, this flag should be TRUE, and for foreground
     PLMN cases, this flag should be FALSE

     only when this flag is TRUE, the next field of srv_cell_meas is meaningful
   */
  boolean srv_cell_avail;

  /* we re-use the CELL CNF payload to carry the serving cell measurement info */
  nb1_cphy_msg_bplmn_cell_cnf_s srv_cell_meas;
} nb1_cphy_msg_bplmn_stop_cnf_s;

/*! @brief
    Data Structure for NB1_CPHY_BPLMN_SUSPEND_CNF

    @details
       ML1 uses this to indicate that PLMN search within ML1 is suspended.
*/
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;
} nb1_cphy_msg_bplmn_suspend_cnf_s;

/******************************************************************************
                              GPS structs
*******************************************************************************/

/*! @brief CPHY GPS timetag request types
*/
typedef enum
{
  NB1_CPHY_GPS_TIMETAG_REQUEST,
  NB1_CPHY_GPS_TIMETAG_PRE_CELL_CHANGE,
  NB1_CPHY_GPS_TIMETAG_POST_CELL_CHANGE,
  NB1_CPHY_GPS_TIMETAG_MAX

} nb1_cphy_gps_timetag_type_t;


/*! @brief CPHY GPS timetag structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! GPS timetag request type */
  nb1_cphy_gps_timetag_type_t       timetag_type;

  /*! Whether a sleep timetag is ok */
  boolean                           allow_sleep_timetag;

} nb1_cphy_gps_timetag_req_s;


/*! @brief CPHY GPS event callback structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! GPS event callback function pointer */
  void
  *event_cb; /* Fool autogen.pl by putting this on the next line */

} nb1_cphy_gps_event_cb_req_s;

/*! @brief CPHY GPS MRL subscribe structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! Whether GPS wants to subscribe */
  boolean                           subscribe;

} nb1_cphy_gps_subscribe_req_s;

/*! @brief CPHY GPS time transfer priority enum
*/
typedef enum
{
  NB1_CPHY_GPS_TIMEXFER_PRIO_LOW = 0,
  NB1_CPHY_GPS_TIMEXFER_PRIO_HIGH,
  NB1_CPHY_GPS_TIMEXFER_PRIO_MAX

} nb1_cphy_gps_timexfer_prio_e;


/*! @brief CPHY GPS time transfer request structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! GPS time transfer sequence number */
  uint32                            seq_num;

  /*! GPS time transfer priority */
  nb1_cphy_gps_timexfer_prio_e      priority;

  /*! Whether SIB8 is required */
  boolean                           sib8_required;

  /*! Number of samples requested */
  uint8                             number_of_tt_samples;

} nb1_cphy_gps_timexfer_req_s;

/*! @brief CPHY GPS time transfer abort structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! GPS time transfer sequence number */
  uint32                            seq_num;

} nb1_cphy_gps_timexfer_abort_req_s;

typedef enum {
  CGPS_NB1_RXTX_SUCCESS                         = 0,
  CGPS_NB1_RXTX_ERROR_NB1_NOT_IN_CONNECTED_MODE = 1,
  CGPS_NB1_RXTX_ERROR_RXTX_DISABLED             = 2,
  CGPS_NB1_RXTX_ERROR_UNKNOWN                   = 3,
  CGPS_NB1_RXTX_ERROR_MAX,
} nb1_cphy_gps_rxtx_prio_e;

/*! @brief CPHY GPS RXTX request structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

} nb1_cphy_gps_rxtx_req_s;

/*! @brief CPHY GPS RXTX done cnf structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type          msgr_hdr;

  /*! Whether report was captured successfully */
  nb1_cphy_gps_rxtx_prio_e      return_val;

} nb1_cphy_gps_rxtx_cnf_s;

/*! @brief CPHY GPS gts req structure
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type            msgr_hdr;

} nb1_cphy_gps_gts_req_s;


/******************************************************************************
                              PRS structs
*******************************************************************************/


/*! Type of PRS session */
typedef enum
{
  NB1_CPHY_PRS_NORMAL_SESSION = 0,  /*!< Normal PRS session */
  NB1_CPHY_PRS_CONTINUOUS_SESSION,  /*!< Continuous PRS session */
  NB1_CPHY_PRS_SCANNER_SESSION,     /*!< Scanner PRS session */

} nb1_cphy_prs_session_type_e;

/*! Type of PRS search */
typedef enum
{
  NB1_CPHY_PRS_SRCHTYPE_WBPRS = 0,  /*!< Normal PRS session */
  NB1_CPHY_PRS_SRCHTYPE_WBCRS,      /*!< CRS using PRS uK   */
} nb1_cphy_prs_srch_type_e;

/*! Type of PRS mode */
typedef enum
{
  NB1_CPHY_PRS_SRCHMODE_PRS_CER = 0,  /*!< PRS CER mode */
  NB1_CPHY_PRS_SRCHMODE_PRS_CIR,      /*!< PRS CIR mode */
} nb1_cphy_prs_srch_mode_e;

/*! NB1 PRS start request structure */
typedef struct nb1_cphy_prs_start_req_s
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;

  /*! PRS duration in milliseconds */
  uint32 prs_duration;

  /*! Type of PRS session */
  nb1_cphy_prs_session_type_e session;

  /*! Obsolete: PRS Srch Type */
  //uint8 srchtype;         /*!< Same as lte_LL1_prs_srch_type_e in intf_prs.h */

  /*! Srch Type of PRS: WBPRS or WBCRS */
  nb1_cphy_prs_srch_type_e prs_crs_occ_type;

  /*! Srch Mode of PRS: PRS or PRS CIR */
  nb1_cphy_prs_srch_mode_e cer_cir_occ_type;

  /*! Number of hypothesis per batch*/
  uint8 num_hyps_per_batch;

  /*! Number of hypothesis per occasion*/
  uint8 num_hyps_per_occassion;

} nb1_cphy_prs_start_req_s;

/*! Maximum number of PRS neighbors */
#define NB1_CPHY_PRS_MAX_NGHBR_CELLS 72

/*! Maximum number of PRS cell hypotheses */
#define NB1_CPHY_PRS_MAX_CELL_HYP 4

/*! PRS number of downlink subframes */
typedef enum
{
  NB1_CPHY_PRS_1_SF = 0,  /*!< 1 PRS subframe in PRS occasion */
  NB1_CPHY_PRS_2_SF,      /*!< 2 PRS subframe in PRS occasion */
  NB1_CPHY_PRS_4_SF,      /*!< 4 PRS subframe in PRS occasion */
  NB1_CPHY_PRS_6_SF,      /*!< 6 PRS subframe in PRS occasion */
  NB1_CPHY_PRS_MAX_SF,    /*!< Maximum PRS subframes */

} nb1_cphy_prs_num_dl_subframes_e;

/*! PRS antenna port config */
typedef enum
{
  NB1_CPHY_PRS_ANT_PORT_CONFIG_1_2 = 0,  /*!< 1-or-2-ports */
  NB1_CPHY_PRS_ANT_PORT_CONFIG_4,        /*!< 4-ports */
  NB1_CPHY_PRS_ANT_PORT_CONFIG_MAX,      /*!< Maximum antenna port config */

} nb1_cphy_prs_ant_port_config_e;

/*! PRS muting repeat enum */
typedef enum
{
  NB1_CPHY_PRS_MUTING_REPEAT_2 = 0,  /*!< Repeat every 2 occasions  */
  NB1_CPHY_PRS_MUTING_REPEAT_4,      /*!< Repeat every 4 occasions  */
  NB1_CPHY_PRS_MUTING_REPEAT_8,      /*!< Repeat every 8 occasions  */
  NB1_CPHY_PRS_MUTING_REPEAT_16,     /*!< Repeat every 16 occasions  */
  NB1_CPHY_PRS_MUTING_REPEAT_MAX,    /*!< Maximum repeat field value */

} nb1_cphy_prs_muting_repeat_e;

/*! NB1 PRS cell muting structure */
typedef struct
{
  /* Whether the muting information is valid */
  boolean valid;

  /*! Number of occasions before repeating */
  nb1_cphy_prs_muting_repeat_e repeat;

  /*! Muting pattern */
  uint16 pattern;

} nb1_cphy_prs_cell_muting_s;

/*! NB1 PRS cell hypothesis structure */
typedef struct
{
  /*! Symbol offset index */
  int8 symbol_offset_index;

  /*! Prune window size */
  uint16 prune_win_size;

  /*! Prune window start index */
  uint16 prune_win_start_index;

} nb1_cphy_prs_cell_hypothesis_s;

/*! NB1 PRS cell structure */
typedef struct
{
  /*! Physical cell id */
  nb1_phy_cell_id_t phy_cell_id;

  /*! E-ARFCN */
  nb1_earfcn_t earfcn;

  /*! Antenna port config */
  nb1_cphy_prs_ant_port_config_e ant_port_config;

  /*! PRS bandwidth */
  nb1_bandwidth_idx_e bandwidth;

  /*! Configuration index */
  uint16 config_index;

  /*! Slot Number Offset */
  uint8 slot_num_offset;

  /*! PRS Subframe Offset*/
  uint16 prs_subframe_offset;

  /*! Number of downlink subframes */
  nb1_cphy_prs_num_dl_subframes_e num_dl_subframes;

  /*! Number of consecutive measurements required*/
  uint8 NumOfConsecutiveMeas;

  /*! Is strong enough to cause x-corr */
  boolean b_is_offending_cell;

  /*! PRS Gain Bias */
  int8 prs_gain_bias;

  /*! Muting pattern */
  nb1_cphy_prs_cell_muting_s muting;

  /*! Number of hypotheses */
  uint8 num_hypotheses;

  /*! Hypotheses */
  nb1_cphy_prs_cell_hypothesis_s hypothesis[ NB1_CPHY_PRS_MAX_CELL_HYP ];

  /*! Cell has exceeded PRS IC threshold*/
  boolean prs_ic_offending_cell;

} nb1_cphy_prs_cell_info_s;

/*! NB1 PRS update cells request structure */
typedef struct nb1_cphy_prs_update_cells_req_s
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Reference cell */
  nb1_cphy_prs_cell_info_s ref;

  /*! Whether this update is a partial or a full update.*/
  boolean full_update;

  /*! Number of neighbor cells */
  uint8 num_nghbrs;

  /*! Neighbor cells */
  nb1_cphy_prs_cell_info_s nghbrs[ NB1_CPHY_PRS_MAX_NGHBR_CELLS ];

} nb1_cphy_prs_update_cells_req_s;

/*! NB1 PRS stop request structure */
typedef struct nb1_cphy_prs_stop_req_s
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;

} nb1_cphy_prs_stop_req_s;

#define MAX_NUM_RSTD_IFREQ 3

typedef enum
{
  NB1_CPHY_IFREQ_OTDOA_MEAS_ACTION_STOP = 0,
  NB1_CPHY_IFREQ_OTDOA_MEAS_ACTION_START,

} nb1_cphy_ifreq_otdoa_meas_action_e;

typedef struct nb1_cphy_rstd_info_s
{

  /*! E-ARFCN */
  nb1_earfcn_t earfcn;

  /*! PRS offset needed to perform ifreq measurements(0...39) */
  uint8 prs_offset;

} nb1_cphy_rstd_info_s;

typedef struct nb1_cphy_ifreq_otdoa_ind_s
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Meas Action */
  nb1_cphy_ifreq_otdoa_meas_action_e meas_action;

  /* Num freqs populated in the array below */
  uint8 num_freq;

  /*! Measurement information needed for each frequency */
  nb1_cphy_rstd_info_s meas_freqs[ MAX_NUM_RSTD_IFREQ ];

}nb1_cphy_ifreq_otdoa_ind_s;

/*! @brief Message sent from ML1 to IMS to inform IMS of current CDRX cycle information
 *  so VoLTE voice packets can be synchronized with the CDRX on-duration.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /* Start time of next CDRX cycle in qtimer units */
  uint64 cdrx_cycle_start_time;

  /* Offset from start of next start of CDRX cycle to account for
   * msgr + processing delay (in qtimer units) */
  uint32 delta_time;

  /* The CDRX cycle lenght in ms, 0 indicates CDRX not configured */
  uint16 cdrx_cycle_len_ms;

  /* System frame number of start of CDRX  cycle */
  uint16 cdrx_cycle_start_sfn;

  /* Subframe number of start of CDRX  cycle */
  uint8  cdrx_cycle_start_subfn;

}nb1_cphy_cdrx_pkt_timing_ind_s;

/******************************************************************************
            UE INFO REPORT - SON RLF and MDT related structures
*******************************************************************************/

/*! Maximum   number   of   carrier   frequencies */
#define NB1_CPHY_MAX_FREQUENCIES 8

/*! @brief Types of SON Reports
*/
typedef enum
{
  NB1_CPHY_UEINFO_RPT_NONE = 0,
  NB1_CPHY_UEINFO_RPT_RLF = 1,
  NB1_CPHY_UEINFO_RPT_RACH = 2,
  NB1_CPHY_UEINFO_RPT_LOGGED_MEAS = 3,
  NB1_CPHY_UEINFO_RPT_CON_FAILURE = 4,
  NB1_CPHY_UEINFO_RPT_MAX = 255,
} nb1_cphy_ueinfo_rpt_type_e;

typedef enum
{
  NB1_CPHY_UEINFO_RPT_MASK_NONE         = 0,
  NB1_CPHY_UEINFO_RPT_MASK_RLF          = SYS_BM(NB1_CPHY_UEINFO_RPT_RLF),
  NB1_CPHY_UEINFO_RPT_MASK_RACH                 = SYS_BM(NB1_CPHY_UEINFO_RPT_RACH),
  NB1_CPHY_UEINFO_RPT_MASK_CON_FAILURE  = SYS_BM(NB1_CPHY_UEINFO_RPT_CON_FAILURE),
  NB1_CPHY_UEINFO_RPT_MASK_MAX,
} nb1_cphy_ueinfo_rpt_mask_e;

/*! @brief Types for MDT Report
*/

typedef enum
{
  NB1_CPHY_UEINFO_MDT_SESSION_START         = 0,
  NB1_CPHY_UEINFO_MDT_SESSION_STOP
} nb1_cphy_ueinfo_mdt_session_status_e;

typedef enum
{
  NB1_CPHY_UEINFO_MDT_LOGGING_INTERVAL_NONE         = 0,
  NB1_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS1280,
  NB1_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS2560,
  NB1_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS5120,
  NB1_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS10240,
  NB1_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS20480,
  NB1_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS30720,
  NB1_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS40960,
  NB1_CPHY_UEINFO_MDT_LOGGING_INTERVAL_MS61440
} nb1_cphy_ueinfo_mdt_logging_interval_e;

/*! @brief Types of Conn failure Reports
*/
enum
{
  NB1_CPHY_UEINFO_CONN_FAILURE_RPT_NONE = 0,
  NB1_CPHY_UEINFO_CONN_FAILURE_RPT_MEAS = 0x01,
  NB1_CPHY_UEINFO_CONN_FAILURE_RPT_UL = 0x02,
  NB1_CPHY_UEINFO_CONN_FAILURE_RPT_MAX = 0xFF,
};

/*! @brief Structure that defines the measurement result list, used in
    measurement reports
*/

typedef struct
{
  uint8 size;
  nb1_cphy_conn_meas_report_list_eutra_s eutra[NB1_CPHY_MAX_CELL_REPORT];
} nb1_cphy_ueinfo_meas_report_eutra_list_s;

typedef struct
{
  /*! Number of results in list. Range: 0..NB1_CPHY_MAX_CELL_REPORT */
  uint8 size;

  /*! WCDMA neighbor measurement results list */
  nb1_cphy_conn_meas_report_list_wcdma_s wcdma[NB1_CPHY_MAX_CELL_REPORT];
} nb1_cphy_ueinfo_meas_report_wcdma_list_s;


typedef struct
{
  /*! Number of results in list. Range: 0..NB1_CPHY_MAX_CELL_REPORT */
  uint8 size;

  /*! CDMA2000 neighbor measurement results list */
  nb1_cphy_conn_meas_report_list_cdma2k_s cdma2k[NB1_CPHY_MAX_CELL_REPORT];
} nb1_cphy_ueinfo_meas_report_cdma2k_list_s;

typedef struct
{
  /*! ARFCN-ValueEUTRA   ::=   INTEGER   (0 .  . 262143) */
  nb1_earfcn_t carrierFreq;

  /*! Measurement results listfor eutra*/
  nb1_cphy_ueinfo_meas_report_eutra_list_s eutra_list;
} nb1_cphy_ueinfo_meas_neigh_results_eutran_s;

typedef struct
{
  /*! ARFCN-ValueUTRA   ::=   INTEGER   (0 .  . 16383) */
  uint16 carrierFreq;

  /*! Measurement results list */
  nb1_cphy_ueinfo_meas_report_wcdma_list_s wcdma_list;
} nb1_cphy_ueinfo_meas_neigh_results_wcdma_s;

typedef struct
{
  /*CDMA200 band class bc0 to bc17*/
  uint8 band_class;

  /*! ARFCN-ValueCDMA2000  ::=   INTEGER   (0 .  . 2047 ) */
  uint16 arfcn;

  /*! Measurement results list */
  nb1_cphy_ueinfo_meas_report_cdma2k_list_s cdma2k_list;
} nb1_cphy_ueinfo_meas_neigh_results_cdma2k_s;


typedef struct
{
  /*! Number of results in list. Range: 0..NB1_CPHY_MAX_FREQUENCIES */
  uint8                           eutra_size;

  /*! Measurement results list EUTRA */
  nb1_cphy_ueinfo_meas_neigh_results_eutran_s eutra_neigh[NB1_CPHY_MAX_FREQUENCIES];

  /*! Number of results in list. Range: 0..NB1_CPHY_MAX_CELL_REPORT */
  uint8                           wcdma_size;

  /*! Measurement results list EUTRA */
  nb1_cphy_ueinfo_meas_neigh_results_wcdma_s wcdma_neigh[NB1_CPHY_MAX_FREQUENCIES];

  /*! Number of results in list. Range: 0..NB1_CPHY_MAX_CELL_REPORT */
  uint8                           gsm_size;

  /*! GERAN neighbor measurement results list */
  nb1_cphy_conn_meas_report_list_gsm_s   gsm_neigh[NB1_CPHY_MAX_CELL_REPORT];


  /*! Number of results in list. Range: 0..NB1_CPHY_MAX_CELL_REPORT */
  uint8                           cdma2k_size;

  /*! Measurement results list EUTRA */
  nb1_cphy_ueinfo_meas_neigh_results_cdma2k_s cdma2k_neigh[NB1_CPHY_MAX_FREQUENCIES];

} nb1_cphy_ueinfo_meas_neigh_results_s;

/*! @brief Measurement data structure for SON RLF feature
*/
typedef struct
{
  /*! RSRP of serving cell */
  nb1_cphy_conn_meas_rsrp_t         serving_rsrp;

  /*! RRC to look at RSRQ of reported cell only if rsrq_info_present is TRUE*/
  boolean rsrq_info_present;

  /*! RSRQ of reported cell */
  nb1_cphy_conn_meas_rsrq_t         serving_rsrq;

  /*! Neighbor measurement results list */
  nb1_cphy_ueinfo_meas_neigh_results_s  neigh_results;

} nb1_cphy_ueinfo_rlf_rpt_s;


/*! @brief Confirmation data structure for SON Measurement Reports
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Report Type*/
  nb1_cphy_ueinfo_rpt_mask_e type;

  /*! Whether or not the report was populated successfully */
  errno_enum_type status;
} nb1_cphy_ueinfo_rpt_rsp_s;


/*! @brief Request data structure for SON Measurement Reports
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Report Type*/
  nb1_cphy_ueinfo_rpt_mask_e type;

    /*! Report */
  nb1_cphy_ueinfo_rlf_rpt_s *rlf_rpt;
} nb1_cphy_ueinfo_rpt_req_s;


/*! @brief MDT report indication from ML1 to RRC
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! RSRP of serving cell */
  nb1_cphy_conn_meas_rsrp_t         serving_rsrp;

  /*! RRC to look at RSRQ of reported cell only if rsrq_info_present is TRUE*/
  boolean rsrq_info_present;

  /*! RSRQ of reported cell */
  nb1_cphy_conn_meas_rsrq_t         serving_rsrq;

  /*! Neighbor measurement results list */
  nb1_cphy_ueinfo_meas_neigh_results_s  neigh_results;

} nb1_cphy_ueinfo_mdt_report_ind_s;

/*! @brief Request data structure to start/stop MDT logging
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Session Type*/
  nb1_cphy_ueinfo_mdt_session_status_e session_status;
  /*! Logging interval, LOGGING_INTERVAL_NONE to be used for STOP */
  nb1_cphy_ueinfo_mdt_logging_interval_e mdt_logging_interval;

} nb1_cphy_ueinfo_mdt_session_req_s;

/*! @brief Connection Failure report from ML1 to RRC
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Meas related params */
  nb1_cphy_ueinfo_rlf_rpt_s rlf_rpt;

  /* To indicate whether or not the maximum power level was used
      for the last transmitted preamble
   */
  boolean maxTxPowerReached;

} nb1_cphy_ueinfo_conn_failure_report_cnf_s;

/*! @brief Connection Failure report request from RRC to ML1
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

} nb1_cphy_ueinfo_conn_failure_report_req_s;


/******************************************************************************
                IRAT CGI related definitions
*******************************************************************************/

/*!< Minimum time required to suspend / resume NB1 stack - 35 ms */
#define NB1_RRC_MIN_TIME_NEEDED_FOR_NB1_SUSP_RESUME   35

/*<! Minimum time required to read IRAT CGI in W,
     Min. Time for Acq - 230 ms
     Min. Time to setup/teardown BCH - 60 ms
     Min. Time to receive MIB/SIB1/SIB3 - 120 ms
     Min. Time to start/suspend WCDMA - 20 ms
      */
#define NB1_RRC_MIN_TIME_NEEDED_FOR_W_CGI_ACQ   430

/*!< Total time needed for W CGI reading */
#define NB1_RRC_MIN_TIME_NEEDED_FOR_W_CGI_FROM_L   (NB1_RRC_MIN_TIME_NEEDED_FOR_W_CGI_ACQ + \
                                                    NB1_RRC_MIN_TIME_NEEDED_FOR_NB1_SUSP_RESUME)

/*!< Minimum time required to read IRAT CGI in G,
     Min. Time to acquire SI3 or SI13 - 420 ms
     Min. Time to activate/deactivate L1 - TBD
     Min. Time to tune to specific BCCH - TBD,
     for now 30 ms have been added for the above two steps */
#define NB1_RRC_MIN_TIME_NEEDED_FOR_G_CGI_ACQ   450

/*!< Total time needed for G CGI reading */
#define NB1_RRC_MIN_TIME_NEEDED_FOR_G_CGI_FROM_L   (NB1_RRC_MIN_TIME_NEEDED_FOR_G_CGI_ACQ + \
                                                    NB1_RRC_MIN_TIME_NEEDED_FOR_NB1_SUSP_RESUME)

/*!  Minimum time required to read IRAT CGI in HRPD,
     Min. Time to acquire HRPD and enter Idle - 70 ms
     Min. Time to acquire SectorParameters message - 35 ms
     Min. Time to activate/deactivate HRPD stack - 20 ms */
#define NB1_RRC_MIN_TIME_NEEDED_FOR_HRPD_CGI_ACQ   125

/*!  Total time needed for HRPD CGI reading */
#define NB1_RRC_MIN_TIME_NEEDED_FOR_HRPD_CGI_FROM_L   (NB1_RRC_MIN_TIME_NEEDED_FOR_HRPD_CGI_ACQ + \
                                                       NB1_RRC_MIN_TIME_NEEDED_FOR_NB1_SUSP_RESUME)

/*!  Minimum time required to read IRAT CGI in 1X */
#define NB1_RRC_MIN_TIME_NEEDED_FOR_1X_CGI_ACQ   150

/*!  Total time needed for 1X CGI reading */
#define NB1_RRC_MIN_TIME_NEEDED_FOR_1X_CGI_FROM_L   (NB1_RRC_MIN_TIME_NEEDED_FOR_1X_CGI_ACQ + \
                                                     NB1_RRC_MIN_TIME_NEEDED_FOR_NB1_SUSP_RESUME)


/*! @brief This message is sent from ML1 to RRC to indicate that IRAT CGI reading
    can be attempted on the other RAT. RRC may suspend ML1 soon after
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! Time available for IRAT CGI reading (in ms) */
  uint16 time_avail_for_irat_cgi;

} nb1_cphy_irat_cgi_start_ind_s;

/*! @brief This message is sent from ML1 to RRC to indicate that IRAT CGI reading
    needs to be aborted. This could be due to RACH/SR getting triggered or for
    any other reason NB1 stack needs to be activated during DRX. RRC will resume
    ML1 soon after.
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

} nb1_cphy_irat_cgi_end_ind_s;

/*! @brief This message is sent from RRC to ML1 to indicate that IRAT CGI is being
    aborted due to T321 expiry or other causes. So NB1 stack will not be suspended
    if ML1 receives this req from RRC. There is no CNF needed for this REQ.
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

} nb1_cphy_abort_irat_cgi_req_s;

/*! @brief Random Access Attempt information */
typedef struct
{
  uint16                                 earfcn;
  uint16                                 phys_cell_id;
  uint16                                 ra_rnti;
  uint8                                  preamble_count;
  uint8                                  last_tx_power;
  uint32                                 ul_grant;
  uint16                                 ra_temp_c_rnti;
  uint16                                 timing_advance;
  uint8                                  ra_resp_successful;
} nb1_cphy_random_access_attempt_s;

/*! @brief Payload of Indication sent by NB1 to MMAL about MAC RAA */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                   hdr;

  /*! Metric type */
  nb1_cphy_random_access_attempt_s       msg;
} nb1_cphy_raa_ind_msg_s;

/*! @brief This message is sent by DO CP to request the current CDMA systime */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

} nb1_cphy_cdma_systime_req_s;

/*! @brief This message is sent back to DO CP with current CDMA systime */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  /*! Whether the CDMA system time is valid */
  boolean valid;
  /*! CDMA system time */
  uint64 cdma_systime;

} nb1_cphy_cdma_systime_cnf_s;

/*! @brief This message is sent by DO CP to request the current HRPD
           measurement results */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

} nb1_cphy_hrpd_meas_results_req_s;

/*! Maximum number of HRPD cells */
#define NB1_CPHY_HRPD_MEAS_RESULT_CELLS_MAX  16

/*! @brief This message is sent back to DO CP with current HRPD
           measurement results */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*! Whether the HRPD results are valid */
  boolean valid;

  /*! Number of HRPD pilots */
  uint8 num_pilots;

  /*! HRPD pilot information */
  struct
  {
    /*! HRPD channel */
    sys_channel_type chan;

    /*! HRPD pilot PN */
    sys_pilot_pn_type pilot_pn;

    /*! HRPD pilot phase */
    uint16 pilot_phase;

    /*! HRPD pilot strength in -db/2 units */
    uint8 pilot_strength;

  } pilot[ NB1_CPHY_HRPD_MEAS_RESULT_CELLS_MAX ];

} nb1_cphy_hrpd_meas_results_cnf_s;


/*!
  @brief
  This message indicates to upper layers the current signal quality at L1 for each MBSFN Area.
*/
typedef struct
{

  /*! MBSFN area id: 0 - 255 */
  nb1_mbsfn_area_id_t                       area_id;

/*! The average SNR of the serving cell over last
      measurement period in decibels Q3[2 ^3] format
      result SNR in cnf = raw SNR value * 2^3
      Range -10..30
   */
  int16 snr;

/*! The excess SNR of the serving cell over last
      measurement period in decibels Q3[2 ^3] format
      Range -10..30
   */
  int16 excess_snr;

  /*! The signal level of the serving cell over last
      measurement period
      Range 0 to 5
   */
  int8 signal_level;

} nb1_cphy_mbsfn_area_signal_strength_s;

/*!
  @brief
  This message indicates to upper layers the current signal quality at L1 for each MBSFN Area.
*/

typedef struct
{
  msgr_hdr_struct_type                       msgr_hdr;

  /*! Number of MBSFN areas valid info schedule config */
  uint8                                    num_mbsfn_areas;

/*! Defines the trace id of the request corresponding to
     *    which this response is being sent.
     *    0 to 32768 - Represents a valid trace id
     *    -1 - Represents that this parameter is not used */
  int16                                    debug_trace_id;

 /*! RSSI ndication per MBSFN Area */
  nb1_cphy_mbsfn_area_signal_strength_s        mbsfn_area_signal_strength[NB1_EMBMS_MAX_MBSFN_AREA];
} nb1_cphy_mbsfn_signal_strength_cnf_s;

/*! @brief The parameters from SIB13
  needed to calculate SNR for MBSFN area
  */
typedef struct
{

  /*! MBSFN area id: 0 - 255 */
  nb1_mbsfn_area_id_t                      area_id;


  /*! Non-MBSFN region length */
  nb1_l1_non_mbsfn_region_length_e         non_mbsfn_symbols;

  /*! Signalling MCS */
  nb1_l1_mbsfn_signalling_mcs_e            mcs;

} nb1_cphy_mbsfn_area_snr_param_s;

/*!
  @brief
  This message is used by upper layers to request the current signal quality at L1 for each MBSFN Area.
*/

typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

/*! Defines the trace id of the request corresponding to
     *    which this response is being sent.
     *    0 to 32768 - Represents a valid trace id
     *    -1 - Represents that this parameter is not used */

  int16            debug_trace_id;

  /*! If set to TRUE, SNR calculation params are passed by RRC */
  boolean         snr_param_present;

  /*! Number of MBSFN areas */
  uint8           num_mbsfn_areas;

} nb1_cphy_mbsfn_signal_strength_req_s;

/*! @brief Struct for req sent from RRC to ML1 requesting debug data to be printed
 * in case of a deadlock timer expiry for certain UMIDs... Putting this here to
 * avoid including ML1 headers in RRC
 */
typedef struct
{
  msgr_hdr_struct_type             msgr_hdr;

  msgr_umid_type                   recvd_umid;
} nb1_cphy_debug_deadlock_tmr_exp_req_s;


/******************************************************************************
                Tx Power Report Msgs, enums, structs
*******************************************************************************/

/*! @brief
      Tx Power Report Bins

    @details
      Tx Power categorized into five bins
 */
typedef enum
{
  /*! x < 0dBm */
  NB1_TX_POWER_BIN_1,
  /*! 0dBm < x < 8dBm */
  NB1_TX_POWER_BIN_2,
  /*! 8dBm < x < 15dBm */
  NB1_TX_POWER_BIN_3,
  /*! 15dBm < x < 20dBm */
  NB1_TX_POWER_BIN_4,
  /*! x > 20dBm */
  NB1_TX_POWER_BIN_5,
  /*! Max Tx Power */
  NB1_TX_POWER_BIN_MAX = NB1_TX_POWER_BIN_5
}nb1_tx_power_bin_e_type;

/*! @brief
      Tx Power Report upper bounds

    @details
      Tx Power Report upper bounds for each bin
 */
typedef enum
{
  /*! 0dBm */
  NB1_TX_POWER_UB_1 = 0,
  /*! 0dBm */
  NB1_TX_POWER_UB_2 = 8,
  /*! 0dBm */
  NB1_TX_POWER_UB_3 = 15,
  /*! 0dBm */
  NB1_TX_POWER_UB_4 = 20,
  /*! Max Tx Power UB */
  NB1_TX_POWER_UB_MAX = NB1_TX_POWER_UB_4
}nb1_tx_power_ub_e_type;

/*! @brief
      Data Structure for NB1_CPHY_TX_POWER_REPORT_IND

    @details
      REQ sent from DS to L1 to start or stop collecting Tx Power stats
 */
typedef struct
{
  /*!< message header */
  msgr_hdr_struct_type              msgr_hdr;

  /*!< Start or Stop Tx Power Reporting */
  boolean                           enable;
}nb1_cphy_tx_power_report_ind_s;


/*! @brief
      Data Structure for NB1_CPHY_TX_POWER_REPORT_INFO_REQ

    @details
      REQ sent from DS to L1 to get Tx Power stats
 */
typedef struct
{
  /*!< message header */
   msgr_hdr_struct_type              msgr_hdr;
}nb1_cphy_tx_power_report_info_req_s;


/*! @brief
      Data Structure for NB1_CPHY_TX_POWER_REPORT_INFO_CNF

    @details
      RSP sent by L1 to DS containing Tx Power stats and NB1 instance
 */
typedef struct
{
  /*!< message header */
   msgr_hdr_struct_type              msgr_hdr;

   /*! subscription ID */
  sys_modem_as_id_e_type             subs_id;

  /*!< Tx Power info counter array - keeps counts of each power bin */
   uint32                            tx_power_info[ NB1_TX_POWER_BIN_MAX + 1 ];
}nb1_cphy_tx_power_report_info_cnf_s;

/*! @brief
  UE Page IND sent by RRC when Page is decoded
  Used in page occasoin and repition scheduling for EDRX
*/

typedef struct{

  /*! Message header */
  msgr_hdr_struct_type     msgr_hdr;

  /*! The information of the current serving cell for which this request is sent,
      used for sanity purposes */
  nb1_cphy_cell_info_s    serving_cell_info;

  /* SFN value when Page ind is sent */
  nb1_l1_sfn_t            curr_sfn_val;

  /*! This flag is used to indicate if decoded page is UE specific page or common page
      TRUE if NAS UE id is present in page contents, FALSE Otherwise */
  boolean                 is_ue_page;

} nb1_cphy_page_done_req_s;


/*! @brief
  Page PDSCH ind sent by DLM to RRC at Page PDSCH Start/End 
  Used by RRC to unblock RACH for MT call page until Page decode ended
*/

typedef struct{

  /*! Message header */
  msgr_hdr_struct_type     msgr_hdr;

  /*! The information of the current serving cell for which this request is sent,
      used for sanity purposes */
  nb1_cphy_cell_info_s    serving_cell_info;

  /*! This flag is used to indicate if this IND is for Page PDSCH Start or End 
          TRUE - Page Start, FALSE - Page End*/
  boolean                 is_page_pdsch_start;

  /*! SFN value when indication is sent */
  nb1_l1_sfn_t         curr_sfn_val;

  /*! Hyper SFN value when indication is sent */
  nb1_l1_hyper_sfn_t   curr_hyper_sfn_val;

} nb1_cphy_page_pdsch_ind_s;


/*! @brief Direct information indication from NPDCCH
  */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type       msgr_hdr;

  /*! Serving cell info where direct information indication is received */
  nb1_cphy_cell_info_s serving_cell_info;

  /*! Per 36.331 6.7.5, Direction Indication Information mask; 8 bit, if a particular bit
      is set it implies the corresponding information as per the below mapping
      is changing
    LSB 1 - SI modification
    LSB 2 - SI modification for eDRX
    Other bits are not used
    */
  uint8                      dii_mask;

  /*! SFN value when indication is sent */
  nb1_l1_sfn_t         curr_sfn_val;

  /*! Hyper SFN value when indication is sent */
  nb1_l1_hyper_sfn_t   hyper_sfn_val;
} nb1_cphy_npdcch_direct_ind_info_ind_s;

#ifdef FEATURE_FMD_SPEED_INFO
/*! @brief Message for FMD (Flight Mode Decision) Speed Info Request (NB1:RRC to ML1)
*/
typedef struct
{
  /*!< Message router header */
  msgr_hdr_s msg_hdr; 
}nb1_cphy_fmd_speed_info_req_s;

/*! @brief Message for FMD (Flight Mode Decision) Speed Info Cnf (NB1:ML1 to RRC)
*/
typedef struct {
  /*!< Message router header */
  msgr_hdr_s msg_hdr; 
  
  /*!< Provides the absolute speed in kmph as estimated by the modem firmware.
       Agreement is, ML1 will send response only with valid speed info */
  uint16 speed;
}nb1_cphy_fmd_speed_info_cnf_s;
#endif /* FEATURE_FMD_SPEED_INFO */

/*! @brief
  Enum for the external control saps used to interface with L1
*/
enum
{
  /*=========================================================================*/
  /*                        Request/Confirm                                  */
  /*=========================================================================*/
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, ACQ, NB1_CPHY_ACQ_ID, nb1_cphy_acq_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, ACQ, NB1_CPHY_ACQ_ID, nb1_cphy_acq_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, START, NB1_CPHY_START_ID, nb1_cphy_start_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, START, NB1_CPHY_START_ID, nb1_cphy_start_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, STOP, NB1_CPHY_STOP_ID, nb1_cphy_stop_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, STOP, NB1_CPHY_STOP_ID, nb1_cphy_stop_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, ABORT, NB1_CPHY_ABORT_ID, nb1_cphy_abort_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, ABORT, NB1_CPHY_ABORT_ID, nb1_cphy_abort_cnf_s),


  MSGR_DEFINE_UMID(NB1, CPHY, REQ, COMMON_CFG,
                   NB1_CPHY_COMMON_CFG_ID, nb1_cphy_common_cfg_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, COMMON_CFG,
                   NB1_CPHY_COMMON_CFG_ID, nb1_cphy_common_cfg_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, DEDICATED_CFG,
                   NB1_CPHY_DEDICATED_CFG_ID, nb1_cphy_dedicated_cfg_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, DEDICATED_CFG,
                   NB1_CPHY_DEDICATED_CFG_ID, nb1_cphy_dedicated_cfg_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, CON_RELEASE,
                   NB1_CPHY_CON_RELEASE_ID, nb1_cphy_con_release_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, CON_RELEASE,
                   NB1_CPHY_CON_RELEASE_ID, nb1_cphy_con_release_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, BAND_SCAN, NB1_CPHY_BAND_SCAN_ID, nb1_cphy_band_scan_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, BAND_SCAN, NB1_CPHY_BAND_SCAN_ID, nb1_cphy_band_scan_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, SYSTEM_SCAN,
                   NB1_CPHY_SYSTEM_SCAN_ID, nb1_cphy_system_scan_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, SYSTEM_SCAN,
                   NB1_CPHY_SYSTEM_SCAN_ID, nb1_cphy_system_scan_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, CELL_SELECT,
                   NB1_CPHY_CELL_SELECT_ID, nb1_cphy_cell_select_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, CELL_SELECT,
                   NB1_CPHY_CELL_SELECT_ID, nb1_cphy_cell_select_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, START_RACH,
                   NB1_CPHY_START_RACH_ID, nb1_cphy_start_rach_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, START_RACH,
                   NB1_CPHY_START_RACH_ID, nb1_cphy_start_rach_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, RACH_RC, NB1_CPHY_RACH_RC_ID, nb1_cphy_rach_rc_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, RACH_RC, NB1_CPHY_RACH_RC_ID, nb1_cphy_rach_rc_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, RAR_PARAMS,
                   NB1_CPHY_RAR_PARAMS_ID, nb1_cphy_rar_params_req_s),
  // No NB1_CPHY_RAR_PARAM_CNF
  
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, RACH_ABORT,
                   NB1_CPHY_RACH_ABORT_ID, nb1_cphy_rach_abort_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, RACH_ABORT,
                   NB1_CPHY_RACH_ABORT_ID, nb1_cphy_rach_abort_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, RACH_CFG, NB1_CPHY_RACH_CFG_ID, nb1_cphy_rach_cfg_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, RACH_CFG, NB1_CPHY_RACH_CFG_ID, nb1_cphy_rach_cfg_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, OUT_OF_SYNC,
                   NB1_CPHY_OUT_OF_SYNC_ID, nb1_cphy_out_of_sync_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, APPLY_TA_PARAMS,
                   NB1_CPHY_APPLY_TA_PARAMS_ID, nb1_cphy_apply_ta_params_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, SIB_SCHED,
                   NB1_CPHY_SIB_SCHED_ID, nb1_cphy_sib_sched_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, CELL_BAR,
                   NB1_CPHY_CELL_BAR_ID, nb1_cphy_cell_bar_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, CELL_UNBAR,
                   NB1_CPHY_CELL_UNBAR_ID, nb1_cphy_cell_unbar_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, IDLE_DRX_CFG,
                   NB1_CPHY_IDLE_DRX_CFG_ID, nb1_cphy_idle_drx_cfg_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, IDLE_MEAS_CFG,
                   NB1_CPHY_IDLE_MEAS_CFG_ID, nb1_cphy_idle_meas_cfg_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, IDLE_MEAS_CFG,
                   NB1_CPHY_IDLE_MEAS_CFG_ID, nb1_cphy_idle_meas_cfg_cnf_s),

 MSGR_DEFINE_UMID(NB1, CPHY, REQ, TRM_UNLOCK_ALLOWED,
                   NB1_CPHY_TRM_UNLOCK_ALLOWED_ID, nb1_cphy_trm_unlock_allowed_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, TRM_UNLOCK_ALLOWED,
                   NB1_CPHY_TRM_UNLOCK_ALLOWED_ID, nb1_cphy_trm_unlock_allowed_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, CONN_MEAS_CFG,
                   NB1_CPHY_CONN_MEAS_CFG_ID, nb1_cphy_conn_meas_cfg_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, CONN_MEAS_CFG,
                   NB1_CPHY_CONN_MEAS_CFG_ID, nb1_cphy_conn_meas_cfg_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, CELL_RESEL_CANCEL,
                   NB1_CPHY_CELL_RESEL_CANCEL_ID, nb1_cphy_cell_resel_cancel_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, CANCEL_CONN,
                   NB1_CPHY_CANCEL_CONN_ID, nb1_cphy_cancel_conn_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, SUSPEND,
                   NB1_CPHY_SUSPEND_ID, nb1_cphy_suspend_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, SUSPEND,
                   NB1_CPHY_SUSPEND_ID, nb1_cphy_suspend_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, RESUME,
                   NB1_CPHY_RESUME_ID, nb1_cphy_resume_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, RESUME,
                   NB1_CPHY_RESUME_ID, nb1_cphy_resume_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, IRAT_CDMA_SYSTEM_TIME_CHG,
                   NB1_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_ID, nb1_cphy_irat_cdma_system_time_chg_req_s),

  /*! @brief NB1_CPHY_BPLMN_START_REQ

      @details
      RRC uses this to send a BPLMN request. depending on the scenario,
      it may pass in new request data, or indicate ML1 to use previous
      request data to continue the PLMN search.
  */
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, BPLMN_START, NB1_CPHY_BPLMN_START_ID,
                                   nb1_cphy_msg_bplmn_start_req_s),
  /*! @brief NB1_CPHY_BPLMN_STOP_REQ

     @details
      RRC uses this to instruct ML1 to stop any PLMN search activities.
      ML1 should stop all the current PLMN activities if any. Further more,
      ML1 should not start any new PLMN acticities until next
      NB1_CPHY_BPLMN_START_REQ.

      ML1 keeps the current request and the intermediate PLMN
      search status.
   */
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, BPLMN_STOP, NB1_CPHY_BPLMN_STOP_ID,
                                   nb1_cphy_msg_bplmn_stop_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, BPLMN_SUSPEND, NB1_CPHY_BPLMN_SUSPEND_ID,
                                   nb1_cphy_msg_bplmn_suspend_req_s),
  /*! @brief NB1_CPHY_BPLMN_STOP_CNF

      @details
      ML1 sends this confirmation when the PLMN search activity is fully stopped
  */
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, BPLMN_STOP, NB1_CPHY_BPLMN_STOP_ID,
                                   nb1_cphy_msg_bplmn_stop_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, CNF, BPLMN_SUSPEND, NB1_CPHY_BPLMN_SUSPEND_ID,
                                   nb1_cphy_msg_bplmn_suspend_cnf_s),

  /*! @brief NB1_CPHY_BPLMN_CELL_REQ

      @details
       RRC uses this command to:
         a) indicate the cell is barred; or
         b) indicate the frequency is barred; or
         c) if the cell is not barred, ask ML1 to calculate rsrp and Srxlev
  */
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, BPLMN_CELL, NB1_CPHY_BPLMN_CELL_ID,
                                   nb1_cphy_msg_bplmn_cell_req_s),

  /*! @brief NB1_CPHY_BPLMN_CELL_CNF

      @details
      ML1 sends this CNF to RRC in response to NB1_CPHY_BPLMN_CELL_REQ.
  */
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, BPLMN_CELL, NB1_CPHY_BPLMN_CELL_ID,
                                   nb1_cphy_msg_bplmn_cell_cnf_s),

  /*! @brief NB1_CPHY_BPLMN_SKIP_REQ

      @details
      RRC uses this command to let ML1 continue its own NB1 activity without
      having to suspend for IRAT PLMN search.

      Usually when ML1 sends NB1_CPHY_BPLMN_TIME_AVAIL_IND to RRC, RRC either
      suspends ML1 so that IRAT can perform PLMN search, or sends
      NB1_CPHY_BPLMN_SKIP_REQ to ML1 in case RRC decides not to give the time
      to IRAT to perform the PLMN search.
  */

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, BPLMN_SKIP, NB1_CPHY_BPLMN_SKIP_ID,
                                   nb1_cphy_msg_bplmn_skip_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, TDD_CFG,
                   NB1_CPHY_TDD_CFG_ID, nb1_cphy_tdd_cfg_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, TDD_CFG,
                   NB1_CPHY_TDD_CFG_ID, nb1_cphy_tdd_cfg_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, GPS_TIMETAG,
                   NB1_CPHY_GPS_TIMETAG_ID, nb1_cphy_gps_timetag_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, NMR_INFO,
                   NB1_CPHY_NMR_INFO_ID, nb1_cphy_nmr_info_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, NMR_INFO,
                   NB1_CPHY_NMR_INFO_ID, nb1_cphy_nmr_info_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, SERV_CELL_MEAS,
                   NB1_CPHY_SERV_CELL_MEAS_ID, nb1_cphy_serv_cell_meas_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, SERV_CELL_MEAS,
                   NB1_CPHY_SERV_CELL_MEAS_ID, nb1_cphy_serv_cell_meas_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, GPS_MRL,
                   NB1_CPHY_GPS_MRL_ID, msgr_hdr_struct_type),
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, GPS_EVENT_CB,
                   NB1_CPHY_GPS_EVENT_CB_ID, nb1_cphy_gps_event_cb_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, GPS_MRL_SUBSCRIBE,
                   NB1_CPHY_GPS_MRL_SUBSCRIBE_ID, nb1_cphy_gps_subscribe_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, PRS_START,
                   NB1_CPHY_PRS_START_ID, nb1_cphy_prs_start_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, PRS_UPDATE_CELLS,
                   NB1_CPHY_PRS_UPDATE_CELLS_ID, nb1_cphy_prs_update_cells_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, PRS_STOP,
                   NB1_CPHY_PRS_STOP_ID, msgr_hdr_struct_type),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, ABORT_IRAT_CGI,
                   NB1_CPHY_ABORT_IRAT_CGI_ID, nb1_cphy_abort_irat_cgi_req_s),

  /* MBSFN areas: MCH scheduling information per PMCH */
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, MSI,
                   NB1_CPHY_MSI_ID, nb1_cphy_msi_req_s),
  // No NB1_CPHY_MSI_CNF

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, CDMA_SYSTIME,
                   NB1_CPHY_CDMA_SYSTIME_ID, nb1_cphy_cdma_systime_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, CDMA_SYSTIME,
                   NB1_CPHY_CDMA_SYSTIME_ID, nb1_cphy_cdma_systime_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, HRPD_MEAS_RESULTS,
                   NB1_CPHY_HRPD_MEAS_RESULTS_ID,
                   nb1_cphy_hrpd_meas_results_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, HRPD_MEAS_RESULTS,
                   NB1_CPHY_HRPD_MEAS_RESULTS_ID,
                   nb1_cphy_hrpd_meas_results_cnf_s),
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, DRX_OPT,
                   NB1_CPHY_DRX_OPT_ID,
                   nb1_cphy_drx_opt_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, GPS_TIMEXFER,
                   NB1_CPHY_GPS_TIMEXFER_ID, nb1_cphy_gps_timexfer_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, GPS_TIMEXFER_ABORT,
                   NB1_CPHY_GPS_TIMEXFER_ABORT_ID, nb1_cphy_gps_timexfer_abort_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, GPS_GTS,
				   NB1_CPHY_GPS_GTS_ID, nb1_cphy_gps_gts_req_s),

MSGR_DEFINE_UMID(NB1, CPHY, REQ, CDMA_CHANNEL_BAR,
                   NB1_CPHY_CDMA_CHANNEL_BAR_ID, nb1_cphy_cdma_channel_bar_req_s),
  MSGR_DEFINE_UMID( NB1, CPHY, REQ, GPS_RXTX,
                    NB1_CPHY_GPS_RXTX_REQ_ID, nb1_cphy_gps_rxtx_req_s ),

  MSGR_DEFINE_UMID( NB1, CPHY, CNF, GPS_RXTX,
                    NB1_CPHY_GPS_RXTX_CNF_ID, nb1_cphy_gps_rxtx_cnf_s ),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, UEINFO_RPT,
                   NB1_CPHY_UEINFO_RPT_ID, nb1_cphy_ueinfo_rpt_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, UEINFO_RPT,
                   NB1_CPHY_UEINFO_RPT_ID, nb1_cphy_ueinfo_rpt_rsp_s),

  /*! @brief NB1_CPHY_MDT_SESSION_REQ

      @detail
      RRC sends this request to ML1 to start or stop an MDT session
  */
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, UE_INFO_MDT_SESSION,
                   NB1_CPHY_UEINFO_MDT_SESSION_ID, nb1_cphy_ueinfo_mdt_session_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, UTC_TIME_UPDATE, NB1_CPHY_UTC_TIME_UPDATE_REQ_ID, nb1_cphy_utc_time_update_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, UTC_TIME_UPDATE, NB1_CPHY_UTC_TIME_UPDATE_CNF_ID, nb1_cphy_utc_time_update_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, BEST_MBMS_NEIGH_FREQ,
                   NB1_CPHY_BEST_MBMS_NEIGH_FREQ_ID, nb1_cphy_best_mbms_neigh_freq_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, BEST_MBMS_NEIGH_FREQ,
                   NB1_CPHY_BEST_MBMS_NEIGH_FREQ_ID, nb1_cphy_best_mbms_neigh_freq_cnf_s),

  /*! @brief Request sent from RRC to ML1 rquesting debug info to be printed out
   * on a deadlock timer expiry for certain UMIDs...
   * Putting this here to avoid including ML1 headers in RRC
  */
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, DEBUG_DEADLOCK_TMR_EXP,
                  NB1_CPHY_DEBUG_DEADLOCK_TMR_EXP_ID, nb1_cphy_debug_deadlock_tmr_exp_req_s),

  /*! @brief NB1_CPHY_UEINFO_CONN_FAILURE_RPT_REQ/CNF

      @detail
      RRC sends this request to ML1 at t-330 expiry for rach and meas parameters
  */
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, UEINFO_CONN_FAILURE_RPT,
                   NB1_CPHY_UEINFO_CONN_FAILURE_RPT_REQ_ID, nb1_cphy_ueinfo_conn_failure_report_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, UEINFO_CONN_FAILURE_RPT,
                   NB1_CPHY_UEINFO_CONN_FAILURE_RPT_REQ_ID, nb1_cphy_ueinfo_conn_failure_report_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, NV_UPDATE, NB1_CPHY_NV_UPDATE_ID, nb1_cphy_nv_update_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, HARQ_ACK_TX_STS,
                   NB1_CPHY_HARQ_ACK_TX_STS_ID, nb1_cphy_harq_ack_tx_sts_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, CNF, HARQ_ACK_TX_STS,
                   NB1_CPHY_HARQ_ACK_TX_STS_ID, nb1_cphy_harq_ack_tx_sts_cnf_s),

  /* message ID: NB1_CPHY_TX_POWER_REPORT_INFO_REQ */
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, TX_POWER_REPORT_INFO,
                   NB1_CPHY_UL_TX_POWER_INFO_ID, nb1_cphy_tx_power_report_info_req_s),

  /* message ID: NB1_CPHY_TX_POWER_REPORT_INFO_CNF */
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, TX_POWER_REPORT_INFO,
                   NB1_CPHY_UL_TX_POWER_INFO_ID, nb1_cphy_tx_power_report_info_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, SIB1_COMMON_CFG, NB1_CPHY_SIB1_COMMON_CFG_ID, nb1_cphy_sib1_common_cfg_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, CONTEXT_STORE, NB1_CPHY_CONTEXT_STORE_ID, nb1_cphy_context_store_req_s),

  MSGR_DEFINE_UMID(NB1, CPHY, CNF, CONTEXT_STORE, NB1_CPHY_CONTEXT_STORE_CNF_ID, nb1_cphy_context_store_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, GET_CE_LVL, NB1_CPHY_CE_LVL_ID, nb1_cphy_ce_lvl_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, RSP, GET_CE_LVL, NB1_CPHY_CE_LVL_ID, nb1_cphy_ce_lvl_rsp_s),

#ifdef FEATURE_FMD_SPEED_INFO
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, FMD_SPEED_INFO, NB1_CPHY_FMD_SPEED_INFO_ID, nb1_cphy_fmd_speed_info_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, FMD_SPEED_INFO, NB1_CPHY_FMD_SPEED_INFO_ID, nb1_cphy_fmd_speed_info_cnf_s),
#endif /* FEATURE_FMD_SPEED_INFO */

  /*=========================================================================*/
  /*                          Indications                                    */
  /*=========================================================================*/
  MSGR_DEFINE_UMID(NB1, CPHY, IND, PDCCH_ORDER,
                   NB1_CPHY_IND_PDCCH_ORDER_ID, nb1_cphy_pdcch_order_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, RA_TIMER_EXPIRED,
                   NB1_CPHY_IND_RA_TIMER_EXPIRED_ID, nb1_cphy_ra_timer_expired_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, MIB,
                   NB1_CPHY_IND_MIB_ID, nb1_cphy_mib_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, MOD_PRD_BND,
                   NB1_CPHY_IND_MOD_PRD_BND_ID, nb1_cphy_mod_prd_boundary_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, DL_WEAK,
                   NB1_CPHY_IND_DL_WEAK_ID, nb1_cphy_dl_weak_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, CELL_RESEL,
                   NB1_CPHY_IND_CELL_RESEL_ID, nb1_cphy_cell_resel_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, IRAT_ASF_NEEDED,
                   NB1_CPHY_IND_IRAT_ASF_NEEDED_ID, nb1_cphy_irat_asf_needed_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, BLACKLISTED_CSG_PCI_RANGE,
                   NB1_CPHY_IND_BLACKLISTED_CSG_PCI_RANGE_ID, nb1_cphy_blacklisted_csg_pci_range_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, RACH_SUCCESS,
                   NB1_CPHY_RACH_SUCCESS_ID, nb1_cphy_rach_success_ind_s),
                   
  MSGR_DEFINE_UMID(NB1, CPHY, IND, OOS,
                   NB1_CPHY_IND_OOS_ID, nb1_cphy_oos_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, RL_FAILURE,
                   NB1_CPHY_IND_RL_FAILURE_ID, nb1_cphy_rl_failure_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, MEAS_REPORT,
                   NB1_CPHY_IND_MEAS_REPORT_ID, nb1_cphy_conn_meas_report_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, MSG3_TRANSMISSION,
                   NB1_CPHY_IND_MSG3_TRANSMISSION_ID, (nb1_cphy_msg3_transmission_ind_s)),
  MSGR_DEFINE_UMID(NB1, CPHY, IND, BPLMN_SIB_RECEIVED,
                   NB1_CPHY_IND_BPLMN_SIB_RECEIVED_ID, nb1_cphy_msg_bplmn_sib_received_ind_s),
  MSGR_DEFINE_UMID(NB1, CPHY, IND, TCELL_RF_UNAVAILABLE,
                   NB1_CPHY_IND_TCELL_RF_UNAVAILABLE_ID, nb1_cphy_tcell_rf_unavailable_ind_s),
  MSGR_DEFINE_UMID(NB1, CPHY, IND, MISMATCH_RAR,
                   NB1_CPHY_IND_MISMATCH_RAR_ID, nb1_cphy_mismatch_rar_ind_s),

			   
  MSGR_DEFINE_UMID(NB1, CPHY, IND, SRCH_RACH_SUSPEND_RESUME,
                   NB1_CPHY_SRCH_RACH_SUSPEND_RESUME_ID, nb1_cphy_srch_rach_suspend_resume_ind_s),

  /*! @brief NB1_CPHY_BPLMN_TIME_AVAIL_IND

      @detail
      ML1 sends this indication to RRC in response to NB1 to IRAT PLMN Search.
      When ML1 sends this indication to RRC, it means ML1 is ready to be
      suspended for IRAT to perform PLMN search.

      after receiving this, RRC should either suspend ML1, or send
      NB1_CPHY_BPLMN_SKIP_REQ to ML1 in case it decides not to give
      the time to IRAT to perform the PLMN search.
  */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, BPLMN_TIME_AVAIL, NB1_CPHY_IND_BPLMN_TIME_AVAIL_ID,
                                   nb1_cphy_msg_bplmn_time_avail_ind_s),

  /*! @brief NB1_CPHY_BPLMN_COMPLETE_IND

      @details
      ML1 sends this indication to RRC in the following cases:
        1). in NB1 to NB1 case, all the PLMN search is done
        2). in IRAT to NB1 case, the PLMN search is partially done
            due to the given time expired.
        3). PLMN search failed
  */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, BPLMN_COMPLETE, NB1_CPHY_IND_BPLMN_COMPLETE_ID,
                                   nb1_cphy_msg_bplmn_complete_ind_s),

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
  /* NB1_CPHY_BPLMN_UNSOLICITED_EARFCN_STATUS_IND */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, BPLMN_UNSOLICITED_EARFCN_STATUS, NB1_CPHY_IND_BPLMN_EARFCN_STATUS_ID,
                                   nb1_cphy_bplmn_unsolicited_earfcn_status_ind_s),
#endif

  /*! @brief This message is sent from ML1 to RRC to indicate that IRAT CGI reading
    can be attempted on the other RAT. RRC may suspend ML1 soon after
  */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, IRAT_CGI_START, NB1_CPHY_IND_IRAT_CGI_START_ID,
                                   nb1_cphy_irat_cgi_start_ind_s),

  /*! @brief This message is sent from ML1 to RRC to indicate that IRAT CGI reading
    needs to be aborted on the other RAT. RRC will resume ML1 soon after
  */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, IRAT_CGI_END, NB1_CPHY_IND_IRAT_CGI_END_ID,
                                   nb1_cphy_irat_cgi_end_ind_s),
  /*! @brief This message is sent from MAC to ML1 to indicate Scell activation/deactivation
      MAC Control Element
  */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, SCELL_STATUS,
                   NB1_CPHY_IND_SCELL_STATUS_ID, nb1_cphy_scell_status_ind_s),

  /*! @brief This message is sent from MAC to ML1 to indicate the TA Value when
      RAID is off by one
  */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, RA_ONE_OFF,
                   NB1_CPHY_IND_RA_ONE_OFF_ID, nb1_cphy_ra_one_off_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, CNF, MBSFN_SIGNAL_STRENGTH,
                   NB1_CPHY_MBSFN_SIGNAL_STRENGTH_ID, nb1_cphy_mbsfn_signal_strength_cnf_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, MBSFN_SIGNAL_STRENGTH,
                   NB1_CPHY_MBSFN_SIGNAL_STRENGTH_ID, nb1_cphy_mbsfn_signal_strength_req_s),

  /*! @brief MDT report Indication sent from ML1 to RRC.
      Sent at regular time intervals as configured by eNB
  */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, UE_INFO_MDT_REPORT,
                   NB1_CPHY_IND_UEINFO_MDT_REPORT_ID, nb1_cphy_ueinfo_mdt_report_ind_s),

  /*! @brief Indication sent from ML1 to RRC indicating we want to reconfigure for
             for inter-freq OTDOA.
    */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, IFREQ_OTDOA,
                   NB1_CPHY_IND_IFREQ_OTDOA_ID, nb1_cphy_ifreq_otdoa_ind_s),

  /*! @brief Message sent from ML1 to IMS to inform IMS of current CDRX cycle information
   *  so VoLTE voice packets can be synchronized with the CDRX on duration.
   */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, CDRX_PKT_TIMING,
                   NB1_CPHY_IND_CDRX_PKT_TIMING_ID, nb1_cphy_cdrx_pkt_timing_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, SCELL_TUNEAWAY_STATUS,
                   NB1_CPHY_IND_SCELL_TUNEAWAY_STATUS_ID, nb1_cphy_scell_tuneaway_status_s),

  /*! @brief Message sent from ML1 to RRC to inform RRC that a phychan schedule
   *  request has been sent to LL1 for RACH MSG1
   */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, RACH_MSG1_SCHED,
                   NB1_CPHY_IND_RACH_MSG1_SCHED_ID, nb1_cphy_rach_msg1_sched_ind_s),

  /* message ID: NB1_CPHY_TX_POWER_REPORT_IND */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, TX_POWER_REPORT,
                   NB1_CPHY_IND_UL_TX_POWER_ID, nb1_cphy_tx_power_report_ind_s),

  /* NB1_CPHY_HYPER_SFN_REQ,  no cnf */
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, HYPER_SFN,
                   NB1_CPHY_HYPER_SFN_ID, nb1_cphy_hyper_sfn_req_s),

  /* NB1_CPHY_NPDCCH_DIRECT_IND_INFO_IND */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, NPDCCH_DIRECT_IND_INFO,
                   NB1_CPHY_IND_NPDCCH_DIRECT_IND_INFO_ID, nb1_cphy_npdcch_direct_ind_info_ind_s),

  /* NB1_CPHY_PAGE_DONE_REQ, no cnf */
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, PAGE_DONE,
                   NB1_CPHY_PAGE_DONE_ID, nb1_cphy_page_done_req_s),

  /* EMTC EDRX specific message ID: NB1_CPHY_EDRX_UIM_PAGE_IND,  */
      MSGR_DEFINE_UMID(NB1, CPHY, IND, EDRX_UIM_PAGE,
                       NB1_CPHY_EDRX_UIM_PAGE_ID, nb1_cphy_edrx_uim_page_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, PSM,
                   NB1_CPHY_PSM_IND_ID, nb1_cphy_psm_ind_s),

  /* NB1_CPHY_PAGE_PDSCH_IND, */
  MSGR_DEFINE_UMID(NB1, CPHY, IND, PAGE_PDSCH,
                   NB1_CPHY_PAGE_PDSCH_ID, nb1_cphy_page_pdsch_ind_s),

  /* NB1 TX and GNSS coex message ID: NB1_CPHY_TX_START_IND,  */           
  MSGR_DEFINE_UMID(NB1, CPHY, IND, TX_START,
                   NB1_CPHY_IND_TX_START_ID, nb1_cphy_tx_start_ind_s ),

  /* NB1 TX and GNSS coex message ID: NB1_CPHY_TX_END_IND,  */  
  MSGR_DEFINE_UMID(NB1, CPHY, IND, TX_END,
                   NB1_CPHY_IND_TX_END_ID, nb1_cphy_tx_end_ind_s ),  

};

/*@}*//* end cphy_sap doxygen group */

/*===========================================================================

                   DATA PATH SAPS ( REQ, CNF, IND)

===========================================================================*/

/*! @defgrousp phy_sap Data Plane (CPHY) Interface

  @brief This module describes the data path interface to/from L1

  @detail The umids for this module are defined in @ref umids in the
          CPHY section.  The structures associated with the messages
          are defined in this module, as well as the documentation
          describing these messages

          This interface is generated from
          $Header: //components/rel/nb1.mpss/2.2/api/nb1_cphy_msg.h#1 $ //source/qcom/qct/modem/nb1/ML1/l1_common/main/latest/inc/nb1_cphy_sap.h#46 $

  @todo Perhaps update CPHY->PHY

  @{*/

/*!
  @brief This indication is sent from ML1 to UL-MAC task to indicate to MAC to
  start building an uplink packet to be transmitted on PUSCH.

  */

typedef enum
{
  NB1_CPHY_NONE_PHR,
  NB1_CPHY_PHR,
  NB1_CPHY_EPHR,
  NB1_CPHY_DPR
} nb1_cphy_phr_type_e;

/*!
  @brief This is the request type sent from DS to ML1 to start/update/stop DL
  throughput estimation.

*/
typedef enum
{
  NB1_CPHY_DL_TPUT_ESTM_REPORT_TYPE_START,
  NB1_CPHY_DL_TPUT_ESTM_REPORT_TYPE_UPDATE,
  NB1_CPHY_DL_TPUT_ESTM_REPORT_TYPE_STOP
} nb1_cphy_dl_tput_estm_report_type_e;

/*!
  @brief This is the confidence level type sent from ML1 to DS as part of
  throughput estimation report.

*/
typedef enum
{
  NB1_CPHY_DL_TPUT_ESTM_CONFIDENCE_LEVEL_LOW,     /*  0  */
  NB1_CPHY_DL_TPUT_ESTM_CONFIDENCE_LEVEL_MEDIUM,  /* 0.5 */
  NB1_CPHY_DL_TPUT_ESTM_CONFIDENCE_LEVEL_HIGH     /*  1  */
} nb1_cphy_dl_tput_estm_confidence_level_e;

typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

  /*! subscription ID */
  sys_modem_as_id_e_type subs_id;

  /*! report type */
  nb1_cphy_dl_tput_estm_report_type_e req_type;

  /*! T_accumulate timer value.
      Report is sent every T_accumulate.
  */
  uint16 t_accumulate;

} nb1_cphy_dl_tput_estm_report_req_s;

typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

  /*! subscription ID */
  sys_modem_as_id_e_type subs_id;

  /*! report type */
  nb1_cphy_dl_tput_estm_report_type_e rsp_type;

} nb1_cphy_dl_tput_estm_report_rsp_s;

typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

  /*! subscription ID */
  sys_modem_as_id_e_type subs_id;

  /*! Additional available bytes */
  uint64 additional_avail_bytes;

  /*! confidence level about this report indication */
  nb1_cphy_dl_tput_estm_confidence_level_e confidence_level;

  /*! Flag indicates if data freeze happen (no estimation performed) */
  boolean data_freeze;

  /*! Flag indicates if UE move from connected state to idle state */
  boolean moved_to_idle;

} nb1_cphy_dl_tput_estm_report_ind_s;

typedef struct
{
  /*! RNTI type used to decode the PDCCH */
  nb1_rnti_type_e     rnti_type;

  /*! System Frame number - Only populated for the first carrier in the array*/
  nb1_l1_sfn_t sys_fn;

  /*! Subframe number - Only populated for the first carrier in the array*/
  nb1_l1_subfn_t  sub_fn;

  /*! Transmission type */
  nb1_l1_tx_type_e  tx_type;

  /*! Size of the transport block to be built.  Units are in bytes */
  uint16 trblk_size;

  /*! Address of the destination buffer */
  uint32 ddr_address;

  /*! NBPHR payload, only the lowest 2 bits are valid */
  uint8 ms3_nbphr_payload;

  uint8              tone_bw;   /* Tone BW type.  */
  uint8              num_ru;    /* Number of RU */
  uint8              itbs;      /* Block size and modulation indicator */
  uint8              harq_id;   /* UL harq id, only 0, 1 is valid value */
} nb1_cphy_ul_pkt_build_ind_s;

typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type     hdr;

  /*! Message payload */
  nb1_cphy_ul_pkt_build_ind_s payload;

} nb1_cphy_ul_pkt_build_ind_msg_s;


/*! @brief This primitive is used by ML1 to inform MAC that the
    RACH procedure in connected mode due to PDCCH order is complete
        due to reception of C-RNTI
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

} nb1_cphy_pdcch_order_rach_sucess_ind_s;


/*! @brief This message is sent to ML1 from MAC when the Up Link Timer is
           expired.

    @todo define the contents of this structure
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;
}nb1_cphy_ul_ta_oos_req_s;


/*! @brief CPHY MAC DRX CMD This message is sent from MAC to ML1 to indicate
           that a DRX command was received in the MAC CE
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! current sfn for drx req msg*/
  uint16                            mac_drx_ce_sf;

} nb1_cphy_mac_drx_req_s;


/*! @brief Indication sent from ML1 to MAC to indicate SFN is unknown/known.
  This will only be sent at the time of handovers.

*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type  msgr_hdr;

  /*! SFN Known flag. Set to FALSE when handover starts and later to TRUE once PBCH is decoded */
  boolean               sfn_known;
  /*! System frame number/ sub frame number (valid only if sfn_known == TRUE) */
  nb1_l1_cell_systime_s curr_sfn;
} nb1_cphy_handover_sfn_status_ind_s;




typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;

   /*! Message router header */
  nb1_cphy_suspend_resume_rach_type_e suspend_resume_type;
} nb1_cphy_suspend_resume_rach_ind_type_s;

typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type msgr_hdr;
} nb1_cphy_cancel_rach_ind_type_s;


/*! @brief
  Enum for the external data-plane saps used to interface with L1
*/
enum
{

  MSGR_DEFINE_UMID(NB1, CPHY, IND, UL_PKT_BUILD,
                   NB1_CPHY_IND_UL_PKT_BUILD_ID, nb1_cphy_ul_pkt_build_ind_msg_s),
  MSGR_DEFINE_UMID(NB1, CPHY, IND, PDCCH_ORDER_RACH_SUCCESS,
                   NB1_CPHY_IND_PDCCH_ORDER_RACH_SUCCESS_ID, nb1_cphy_pdcch_order_rach_sucess_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, UL_TO_OOS,
                   NB1_CPHY_UL_TO_OOS_ID, nb1_cphy_ul_ta_oos_req_s),
                   
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, MAC_DRX,
                   NB1_CPHY_MAC_DRX_ID, nb1_cphy_mac_drx_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, IND, HANDOVER_SFN_STATUS,
                   NB1_CPHY_IND_HANDOVER_SFN_STATUS_ID, nb1_cphy_handover_sfn_status_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, REQ, DL_TPUT_ESTM_REPORT,
                   NB1_CPHY_DL_TPUT_ESTM_REPORT_ID, nb1_cphy_dl_tput_estm_report_req_s),
  MSGR_DEFINE_UMID(NB1, CPHY, RSP, DL_TPUT_ESTM_REPORT,
                   NB1_CPHY_DL_TPUT_ESTM_REPORT_ID, nb1_cphy_dl_tput_estm_report_rsp_s),
  MSGR_DEFINE_UMID(NB1, CPHY, IND, DL_TPUT_ESTM_REPORT,
                   NB1_CPHY_DL_TPUT_ESTM_REPORT_ID, nb1_cphy_dl_tput_estm_report_ind_s),
  MSGR_DEFINE_UMID(NB1, CPHY, IND, UL_SUSPEND_RESUME_RACH,
                   NB1_CPHY_IND_UL_SUSPEND_RESUME_RACH_ID, nb1_cphy_suspend_resume_rach_ind_type_s),
  MSGR_DEFINE_UMID(NB1, CPHY, IND, CANCEL_RACH,
                   NB1_CPHY_IND_CANCEL_RACH_ID, nb1_cphy_cancel_rach_ind_type_s),
};

enum
{
  MSGR_DEFINE_UMID(NB1, CPHY, IND, MAC_RAA, NB1_CPHY_IND_MAC_RAA_ID,
                   nb1_cphy_raa_ind_msg_s ),
  MSGR_DEFINE_UMID(NB1, CPHY, IND, T310_STATUS, NB1_CPHY_IND_T310_STATUS_ID,
                   nb1_cphy_t310_status_ind_s ),
};

/*! @brief After the packet is built, following status will be written
           in some shared  memory array.  Shared memory is indexed by harq_id.
           This will eventually be defined in the tx swi
  */
#define NB1_PHY_UL_PKT_READY 0xdeadbeef


/*! @brief
    Data Structure for NB1_CPHY_RFCHAIN_REQ

    @details
      DS uses NB1_CPHY_RFCHAIN_REQ to enable/disable DRX/PRX.
      the message of the payload is a byte to indicate the actions
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! the action code
      Bit 7: 0 -- does not need NB1_CPHY_RFCHAIN_CNF
             1 -- needs to send NB1_CPHY_RFCHAIN_CNF
      Bit 6 to bit 3: reserved, all 0's
      Bit 2: specify the action of enabling or disabling.
           0  -- enable
           1  -- disable
      Bit 1: specify whether the action applies to RX1
           0 - not applicable. The action defined by bit 2 will not apply to RX1
           1 - applicable. The action defined by bit 2 will apply to RX1
      Bit 0: specify whether the action applies to RX0
           0 - not applicable. The action defined by bit 2 will not apply to RX0
           1 - applicable. The action defined by bit 2 will apply to RX0


      The following table is a usage summary for the bit 2 to bit 0:
         Byte value      Enable/Disable Rx1     Rx0     Summary
               0                 0                 0     0   No action
               1           0                   0   1     Enables RX0 only
               2           0               1   0         Enables RX1 only
               3           0               1   1         Enables RX0 and RX1
               4           1               0     0   No action
               5           1               0   1   Disables RX0 only
               6           1               1   0         Disables RX1 only
               7           1             1   1   Disables RX0 and RX1

  */
  uint8                             action_code;
  /*! the carrier ID
      To support Carrier Aggregation(CA), the carrier ID needs to be specified:

      0: Primary Carrier Component (PCC)
      1: Secondary Carrier Component (SCC)

      As for now, only one SCC is deployed.
  */
  uint8                             carrier_id;
} nb1_cphy_rfchain_req_s;


/*! @brief
    Data Structure for NB1_CPHY_RFCHAIN_CNF

    @details
      when bit 7 of action_code in NB1_CPHY_RFCHAIN_REQ payload is set to 1,
      ML1 sends back this response message NB1_CPHY_RFCHAIN_CNF, with the following
      payload definition.
 */
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /* the following is the staus code */
  /*
      0: NB1_CPHY_RFCHAIN_REQ successfully processed
      1: ML1 is busy and can not process the command
      2: RF cannot process the command
      3: RF returns error
      4: general error
   */
  uint8                             status;
} nb1_cphy_rfchain_cnf_s;

/*! @brief
  Enum of UMID to pass down RF Chain request
*/
enum
{

  /* message ID: NB1_CPHY_RFCHAIN_REQ */
  MSGR_DEFINE_UMID(NB1, CPHY, REQ, RFCHAIN,
                   NB1_CPHY_RFCHAIN_ID, nb1_cphy_rfchain_req_s)
};


/*! @brief
  Enum of UMID to send back RF Chain request process status
*/
enum
{

  /* message ID: NB1_CPHY_RFCHAIN_CNF */
  MSGR_DEFINE_UMID(NB1, CPHY, CNF, RFCHAIN,
                   NB1_CPHY_RFCHAIN_ID, nb1_cphy_rfchain_cnf_s)
};


/*! @brief indication to RRC from ML1 to go into the PSM mode. */ 
typedef struct 
{ 
  /*! standard msgr header */ 
  msgr_hdr_struct_type       msgr_hdr;      
  /*! PSM duration in seconds as CM will expected duration in sec*/ 
  uint32               psm_duration_in_sec;  

  /*! PSM duration in ms*/ 
  uint64               psm_duration_in_msec; 
}nb1_cphy_psm_ind_s; 

/*! @brief
  RRC sends context store request to ML1 after getting entering PSM request*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
}nb1_cphy_context_store_req_s;

typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  boolean  status;
}nb1_cphy_context_store_cnf_s;

/*! @brief Sleep start indication */
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type       msg_hdr;

  /*! Estimated sleep duration in ms till STMR on */
  uint32                     est_sleep_duration;

  /*! Sleep start in sclk */
  timetick_type              sleep_start_sclk;

/*! Estimated time till wakeup obj currebtly used by RRC */
 uint32                     est_duration_till_wakeup_obj;

} nb1_ml1_sleepmgr_sleep_start_ind_s;


/*! @brief Wakeup ISR indication */
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type       msg_hdr;

  /*! Actual sleep duration in ms till wakeup ISR */
  int32                     actual_sleep_duration;

  /*! Time of STMR on */
  uint32   stmr_on_time;

  /*! Online wakeup status */
  boolean                    online_wakeup;

  /*! Sclk value at  STMR on */
  uint32                     stmr_on_count_sctl;

} nb1_ml1_sleepmgr_wakeup_isr_ind_s;


enum
{
  MSGR_DEFINE_UMID( NB1, ML1_SLEEPMGR, IND, SLEEP_START, 0x04,
                    nb1_ml1_sleepmgr_sleep_start_ind_s ),

  MSGR_DEFINE_UMID( NB1, ML1_SLEEPMGR, IND, WAKEUP_ISR, 0x05,
                    nb1_ml1_sleepmgr_wakeup_isr_ind_s ),

};

/*! @brief Status of deinit of mgr modules
*/
typedef enum
{
  DEINIT_NOT_TRIGGERED = 0,
  DEINIT_TRIGGERED_IN_STOP,
  DEINIT_TRIGGERED_BY_HIGHER_LYR
} nb1_ml1_mgr_modules_deinit_status_e;

/*===========================================================================

  FUNCTION:  nb1_ml1_dlm_cfg_get_paging_carrier_db_index

===========================================================================*/
/*!
  @brief
  Get selected non anchor carrier database index for paging

  @return
  selected carrier index

*/
/*=========================================================================*/
uint8 nb1_ml1_dlm_cfg_get_paging_carrier_db_index
(
  /*! Pointer to right instance */
  nb1_mem_instance_type               instance
);

/*@}*//* end phy_sap doxygen group */

#endif /* NB1_CPHY_MSG_H */
