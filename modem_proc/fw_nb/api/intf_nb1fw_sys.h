/*!
  @file
  intf_nb1fw_sys.h

  @brief
  FW-SW interface system message definitions for NB1 FW

  @detail
  FW-SW interface system message definitions for NB1 FW

*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/intf_nb1fw_sys.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef INTF_NB1FW_SYS_H
#define INTF_NB1FW_SYS_H

#include "intf_nb1fw_common.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/

/* number of LNA gain statges for WBDC table */
#define NB1FW_NUM_LNA_GAIN_STAGE 9

/*===========================================================================

      Typedefs

===========================================================================*/

/* NB1 tech resource modes.  */
typedef enum
{
  NB1FW_UE_MODE_DISABLE,              /* NB1 disable request = NO_LTE */

  /* NB1 IDLE request.
     DISABLE->IDLE : NB1 FW init, no module thread created
     ACTIVE->IDLE  : NB1 FW deinit. Can be used for sleep */
  NB1FW_UE_MODE_IDLE,

  /* Following will create FW threads and load TCM overlays.
     If MODE_ENABLE has not yet come, these will do enable first */
  NB1FW_UE_MODE_SRCH_DL_ONLY,       /* searcher & DL mode (used for page) */
  NB1FW_UE_MODE_DL_UL_ONLY,         /* DL & UL */
  NB1FW_NUM_UE_MODE
} nb1fw_ue_mode_e;

/* NB1 FTM modes.  */
typedef enum
{
  NB1FW_FTM_MODE_INVALID,     
  NB1FW_FTM_MODE_ONLINE,     /* default on-line (full stack) mode, non-ftm */
  NB1FW_FTM_MODE_CAL,      /* FTM calibration mode, NB power is controled by RF */
  NB1FW_FTM_MODE_NONSIG,   /* FTM non-signaling mode, NB power is controled by FW RX, FW TX will work on FTM mode */ 
  NB1FW_FTM_MODE_FTM,      /* FTM mode, NB power is controled by RF, FW TX will work on FTM mode */
  NB1FW_NUM_FTM_MODE
} nb1fw_ftm_mode_e;

/* WBDC value table */
typedef struct
{
  uint32 est1_load_i; /* EST1 I DC estimate value */
  uint32 est1_load_q; /* EST1 Q DC estimate value */
  uint32 est2_load_i; /* EST2 I DC estimate value */
  uint32 est2_load_q; /* EST2 Q DC estimate value */
} nb1fw_wbdc_cfg_s;

/* NB1 FW config request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  nb1fw_ue_mode_e   mode;  /* UE NB1 FW mode */
  nb1fw_ftm_mode_e  ftm_mode; /* FTM mode indication (for RFCal) */
  boolean tcm_retention; /* True   if TCM can be retained, False  If TCM has to be released */

  /* SW should set this flag one at start and clear for subsequent requests */
  boolean latch_wbdc_table;    /* indicate following table should be latched in FW,
                                  it will override FW internal saved info */
  nb1fw_wbdc_cfg_s wbdc_table[NB1FW_NUM_LNA_GAIN_STAGE]; /* WBDC value table */

} nb1fw_sys_config_app_req_s;

NB1FW_INTF_MSG_S(sys_config_app_req);

/* NB1 cell operating mode */
typedef enum
{
  NB1FW_CELL_OP_MODE_IN_BAND_SAME_PCI,
  NB1FW_CELL_OP_MODE_IN_BAND,
  NB1FW_CELL_OP_MODE_GAURD_BAND,
  NB1FW_CELL_OP_MODE_STANDALONE
} nb1fw_cell_op_mode_e;

typedef enum
{
  NB1FW_TL_DISABLE, /* disables tracking loop */
  NB1FW_TL_ACQ,     /* operate loop in high gain mode to do fast acquire */
  NB1FW_TL_NORM     /* operate loop in normal mode to do tracking */
} nb1fw_cell_tl_mode_e;

/*NBRS CRS power offsets per Spec in dB scale and are derived from
 NRS-LTEcrs-pwrOffset from SIB1 allowed values are (dB)
 -6, -4.77, -3, -1.77, 0, 1, 1.23, 2, 3, 4, 4.23, 5, 6, 7, 8, 9
 Above values are converted into 16S11 and stored in FW */

typedef enum
{
 NB1FW_CRSNRS_POWOFF_MINUS6_dB,        /* -6 dB Power offset    */
 NB1FW_CRSNRS_POWOFF_MINUS4Point77_dB, /* -4.77 dB Power Offset */
 NB1FW_CRSNRS_POWOFF_MINUS3_dB,        /* -3 dB Power Offset    */
 NB1FW_CRSNRS_POWOFF_MINUS1Point77_dB, /* -1.77 dB Power Offset */
 NB1FW_CRSNRS_POWOFF_0_dB,             /*  0 dB Power Offset    */
 NB1FW_CRSNRS_POWOFF_1_dB,             /*  1 dB Power Offset    */
 NB1FW_CRSNRS_POWOFF_1Point23_dB,      /* 1.23 dB Power Offset  */
 NB1FW_CRSNRS_POWOFF_2_dB,             /* 2 dB Power Offset     */
 NB1FW_CRSNRS_POWOFF_3_dB,             /* 3 dB Power Offset     */
 NB1FW_CRSNRS_POWOFF_4_dB,             /* 4 dB Power Offset     */
 NB1FW_CRSNRS_POWOFF_4point23_dB,      /* 4.23dB Power Offset   */
 NB1FW_CRSNRS_POWOFF_5_dB,             /* 5 dB Power Offset     */
 NB1FW_CRSNRS_POWOFF_6_dB,             /* 6 dB Power Offset     */
 NB1FW_CRSNRS_POWOFF_7_dB,             /* 7 dB Power Offset     */
 NB1FW_CRSNRS_POWOFF_8_dB,             /* 8 dB Power Offset     */
 NB1FW_CRSNRS_POWOFF_9_dB,             /* 9 dB Power Offset     */
 NB1FW_CRSNRS_POWOFF_INVALID      /* Used for modes other than INBAND PCI */
}nb1fw_nrs_crs_powOffset_e;

typedef enum
{
  NB1FW_WAKE_UP_REACQ,          /* RE ACQ mode */
  NB1FW_WAKE_UP_REACQ_LESS,     /* RE ACQ less mode - RAFTT has to be enabled */
} nb1fw_wakeup_mode_e;


/* LTE CRS information for NB1 FW to do better channel estimation */
/* TIll the point CRSValid is set FW will behave as if we are in IN-Band Different PCI mode*/
/* CRS Valid is supposed to be set at the CELL UPDATE command received after SIB1 decode */
typedef struct
{
  boolean CRSValid;         /* will be set to valid to TRUE only after SIB1 gives right CRS-NRS power offset index */
  uint8 num_ports;            /* Number of LTE CRS ports 1, 2 or 4 */
  nb1fw_nrs_crs_powOffset_e powOffsetIndex;
  uint8 lteCrsSequenceInfo;   /* 5 bit value from MIB which tells PRB index offset from center */
} nb1fw_cell_crs_info_s;

typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  uint16     ul_dl_freq_ratio; /* UL to DL carrier frequency ratio in Q15 */

  uint16     sfn_offset;  /* SFN (systems frame number) offset from ref time (0..1023) */
  uint8      hfn_offset;  /* HFN (modulo 16) offset from ref time (0..15) */
  uint8      num_tx_ant;  /* cell # of tx ant (1 ~ 2) */

  uint16     nbrs_valid_mask[4];   /* NB RS (reference signal) valid mask for 4 frame at (SFN mod 4) */
  int8       raster_offset_khz_q4; /* Raster offset in KHz Q4 units */
  nb1fw_cell_op_mode_e   op_mode;  /* cell operating mode */
  uint8      lte_ctrl_syms;        /* Number of LTE control symbols (not part of RU) 0, 1, 2, 3 */
  nb1fw_cell_crs_info_s crs_info;  /* LTE CRS info for NB cell in IN-BAND op mode */
} nb1fw_sys_cell_update_req_s;

NB1FW_INTF_MSG_S(sys_cell_update_req);

typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  uint32     dl_carrier_freq; /* DL carrier freq in MHz 32Q8. Used for freq error PPM to Hz conversion */
  uint32     inv_dl_freq;     /* 2^33/DL_F in Q19 units. Used by XO utils to calculate XO related RXLM params */

  uint32     dl_earfcn;   /* DL EARFCN value */
  uint32     rflm_handle; /* RFLM handle to use to tuning to channel */

  uint16     pcell_id;   /* Pcell Id */
  uint16     sfn_offset; /* SFN (systems frame number) offset from ref time (0..1023) or (0..7) in case not known */
  uint8      hfn_offset; /* HFN (modulo 16) offset from ref time (0..15) */
  uint16     pos_offset; /* Sub radio frame position (0..19200-1) */
  uint16     nbrs_valid_mask[4]; /* NB RS (reference signal) valid mask for 4 frame at (SFN mod 4) */

  int16      init_ferr_ppm_q10;  /* initial frequency error in Q10 ppm units */
  nb1fw_cell_tl_mode_e ttl_mode; /* TTL mode */
  nb1fw_cell_tl_mode_e ftl_mode; /* FTL mode */

  uint32     snrForMeas;       /* Avg SNR */
  boolean    snrValid;         /* snr value shared is valid for deciding meas param */
  boolean    useLNAGainDcEst;  /* Use LNA gain and DC Estimates from previous wake up either search or cell mode */
  uint16     sleepDuration;    /* UE sleep duration in mili secs, if >= 20,000 should be set to 0xFFFF, for new cell set to 0 */
  boolean    disableSibMibRot; /* specCR_R1_1703913 enables updated SIB scrambling seq
                                * and specCR_R1_1703964 - enables  PBCH rotation */
  nb1fw_wakeup_mode_e    drxWakeupMode;  /* DRX wake up mode. default will be RE ACQ mode */
  boolean    severing_cell_flag;   /*new indication for the severing cell */
  boolean    dopplerReportEnable;  /* this indicate the Doppler reprot ind is need */
  uint32     dopplerMeasurementPeriod; /* this indicate the measurement period for the doppler estimation */
} nb1fw_sys_cell_config_req_s;

NB1FW_INTF_MSG_S(sys_cell_config_req);


/***********************************************NON-ANCHOR ZONE**********************************************************/

typedef enum 
{ 
  NB1FW_CARRIER_NRS_POWER_OFFSET_MINUS12_DB, /* New Carrier = Anchor Carrier - 12dB*/
  NB1FW_CARRIER_NRS_POWER_OFFSET_MINUS10_DB, /* New Carrier = Anchor Carrier - 10dB*/
  NB1FW_CARRIER_NRS_POWER_OFFSET_MINUS8_DB, /* New Carrier = Anchor Carrier - 8dB*/
  NB1FW_CARRIER_NRS_POWER_OFFSET_MINUS6_DB, /* New Carrier = Anchor Carrier - 6dB*/
  NB1FW_CARRIER_NRS_POWER_OFFSET_MINUS4_DB, /* New Carrier = Anchor Carrier - 4dB*/
  NB1FW_CARRIER_NRS_POWER_OFFSET_MINUS2_DB, /* New Carrier = Anchor Carrier - 2dB*/
  NB1FW_CARRIER_NRS_POWER_OFFSET_0_DB, /* New Carrier = Anchor Carrier - 0dB*/
  NB1FW_CARRIER_NRS_POWER_OFFSET_3_DB, /* New Carrier = Anchor Carrier + 3dB*/
  NB1FW_CARRIER_NRS_POWER_OFFSET_CNT      
}nb1fw_carrier_nrs_power_offset_e; 


/* DL PDCCH search space types */
typedef enum
{
  NB1FW_DL_PDCCH_SS_CSS_PAGE,
  NB1FW_DL_PDCCH_SS_CSS_NON_PAGE,
  NB1FW_DL_PDCCH_SS_USS,
  NB1FW_DL_NUM_PDCCH_SS
} nb1fw_dl_pdcch_ss_e;

typedef struct
{
  boolean valid;   /* indicate Gap information valid */
  uint16 period;   /* Gap period in sub frame units */
  uint16 duration; /* Gap duration in sub frame units (always at the end of period */
} nb1fw_dl_pdcch_gap_info_s;

/* DL PDCCH config req */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  nb1fw_dl_pdcch_ss_e ss;        /* PDCCH search space */
  uint16 max_rep;                /* Max repetition in units of valid sub frames */
  uint16 valid_sf_mask[4];       /* Valid sub frame mask (excluding PSS, SSS, PBCH SIBs) */
  nb1fw_dl_pdcch_gap_info_s gap; /* PDCCH gap info in period to mark invalid frame */
  uint16 ss_period;              /* Search space window period in subframe units */
  uint16 ss_offset;              /* Starting offset of SS period from SFN aligned to SS period
                                    SS start SubFN (0..1024x10 - 1) is where "SubFN mod ss_period = ss_offset" */
  uint16 crc_rnti_mask;          /* CRC mask based on RNTI */
  uint16 rnti_type;              /* RNTI type - to be looped back to SW in data ind */

  boolean rnti2_valid;           /* indicate if RNTI 2 info is valid */
  uint16 crc_rnti2_mask;         /* CRC mask based on second RNTI */
  uint16 rnti2_type;             /* Second RNTI type - to be looped back to SW in data ind */

  /* only used when ss = NB1FW_DL_PDCCH_SS_CSS_PAGE */
  uint16 page_frame;             /* Page radio frame number */
  uint16 page_subframe;          /* Page sub frame number */
  uint8  warmupCount;            /* Num of subframes of warm up for demfront. range [2 8] */
   
  boolean rlm_enable;
  boolean iRandomize_enable;     /* interference randomization for NPDCCH/NPDSCH is enabled or not */

  boolean multi_grant_monitor;   /* Enable Multiple Grant Monitor in a searchspace */

  boolean pdschKeepLLR;          /* Default value is FALSE ; when the SW Trigger the deep sleep opt , set the value to TRUE */
} nb1fw_dl_pdcch_config_req_s;

NB1FW_INTF_MSG_S(dl_pdcch_config_req);
/*
Mode 1: operations include disable all the DL on existing Carrier, and enable DL on New Carrier, 
        NPDCCH will disable and enable NPDCCH  in case there is configuration update.
Mode 2: Enable DL on New Carrier with PDCCH Configuration for PO.
Mode 3: Disable DL on Old Carrier, wait for UL RACH and PDCCH Config,
        then enable DL on New Carrier.. etc..
Mode 4: Disable DL on Old Carrier, wait for UL PUSCH transmission, 
        enable DL on the new carrier in HD switching.
Mode 5: No DL disable in the Old Carrier and just Enable the DL on the New Carrier in HD switching 
        without NPDCCH configuration.
*/

/* Carrier Switch: Configuration Scenario */
typedef enum
{
  NB1FW_MC_CONFIG_MODE_1,
  NB1FW_MC_CONFIG_MODE_2,
  NB1FW_MC_CONFIG_MODE_3,
  NB1FW_MC_CONFIG_MODE_4,
  NB1FW_MC_CONFIG_MODE_5
} nb1fw_mc_config_mode_e;

typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  nb1fw_mc_config_mode_e mcConfigMode;

  boolean    isTuneToAnchor;

  uint32     dl_carrier_freq; /* DL carrier freq in MHz 32Q8. Used for freq error PPM to Hz conversion */
  uint32     inv_dl_freq;     /* 2^33/DL_F in Q19 units. Used by XO utils to calculate XO related RXLM params */

  uint32     dl_earfcn;   /* DL EARFCN value:  Does not include Raster Offset component */

  int16      init_ferr_ppm_q10;  /* initial frequency error in Q10 ppm units: Last known valid frequency error in ML1 */

  uint16     ul_dl_freq_ratio; /* UL to DL carrier frequency ratio in Q15: Non Anchor: Includes raster offset, Anchor: Does not include raster offset*/

  int8       raster_offset_khz_q4; /* Raster offset in KHz Q4 units: Non-Anchor: Zero,  Anchor: raster offset from PBCH decode */

  uint16     nbrs_valid_mask[4]; /* NB RS (reference signal) valid mask for 4 frame at (SFN mod 4) */

  nb1fw_cell_op_mode_e   op_mode;  /* cell operating mode */
  uint8      lte_ctrl_syms;        /* Number of LTE control symbols (not part of RU) 0, 1, 2, 3 */
  nb1fw_cell_crs_info_s crs_info;  /* LTE CRS info for NB cell in IN-BAND op mode */

  int32      rsrp_carr_x_Q24;      /*serving cell RSRP get from L1 side.*/
  nb1fw_carrier_nrs_power_offset_e  rsrp_power_offset_carr_X; /* NRS power offset of the Old carrier wrt. Anchor */
  nb1fw_carrier_nrs_power_offset_e  rsrp_power_offset_carr_Y; /* NRS power offset of the New carrier wrt. Anchor */

  boolean    isPdcchCfgValid;  /* Flag to indicate whether Fw has to latch the new PDCCH Configuration */
  nb1fw_dl_pdcch_config_req_s  pdcchCfg;  /* Add PDCCH CONFIG Interface Here.??*/

} nb1fw_sys_mc_config_req_s;

NB1FW_INTF_MSG_S(sys_mc_config_req);

/***********************************************NON-ANCHOR ZONE**********************************************************/


typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  uint16     pcell_id; /* Pcell Id */
} nb1fw_sys_cell_deconfig_req_s;

NB1FW_INTF_MSG_S(sys_cell_deconfig_req);

typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  boolean enable; /* indicate MC operation enable or disable */

  /* following information only applicable if enable = TRUE*/

  uint32  dl_earfcn;      /* DL EARFCN value for MC operation */
  int16   offset_khz_q4;  /* MC DL freq offset in KHz Q4 units */
  uint32  dl_rflm_handle; /* MC RFLM handle for DL */
  uint32  ul_rflm_handle; /* MC RFLM handle for UL */
} nb1fw_sys_cell_mc_config_req_s;

NB1FW_INTF_MSG_S(sys_cell_mc_config_req);

typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */
} nb1fw_sys_cell_stat_req_s;

NB1FW_INTF_MSG_S(sys_cell_stat_req);

typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */
  uint64 mask;             /* Bitmask of NB1 FW log IDs. Bit 0 = LOG_NB1FW_BASE_C */
} nb1fw_sys_log_mask_config_req_s;

NB1FW_INTF_MSG_S(sys_log_mask_config_req);

/* RF Rx sample capture request */
typedef struct
{
  nb1fw_req_hdr_s req_hdr; /* message header */

  uint32     dl_carrier_freq; /* DL carrier freq in MHz 32Q8. Used for freq error PPM to Hz conversion */
  uint32     inv_dl_freq;     /* 2^33/DL_F in Q19 units. Used by XO utils to calculate XO related RXLM params */

  uint32     dl_earfcn;   /* DL EARFCN value */
  uint32     rflm_handle; /* RFLM handle to use to tuning to channel */

  int16      init_ferr_ppm_q10; /* initial frequency error in Q10 ppm units */

  uint16     capture_len;       /* must be less than equal to 4800 samples @ 480 KHz */
  uint32*    buffer_start_addr; /* Ptr of buffer allocated by RFSW to hold the IQ data captured in FW.
                                   Buffer addr MUST be 64 bit (8 byte aligned) and allocated for capture_len */ 
  uint16     delay_cnt;         /* Number of subframes (ms) to delay after LNA setlles to stable gain state */
} nb1fw_sys_rx_sample_capture_req_s;

NB1FW_INTF_MSG_S(sys_rx_sample_capture_req);

/* confirm messages for request message above */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */

  /* return latest WBDC table values*/
  nb1fw_wbdc_cfg_s wbdc_table[NB1FW_NUM_LNA_GAIN_STAGE]; /* WBDC value table */

} nb1fw_sys_config_app_cnf_s;

NB1FW_INTF_MSG_S(sys_config_app_cnf);

typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_sys_cell_update_cnf_s;

NB1FW_INTF_MSG_S(sys_cell_update_cnf);

typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_sys_cell_config_cnf_s;

NB1FW_INTF_MSG_S(sys_cell_config_cnf);

typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_sys_cell_deconfig_cnf_s;

NB1FW_INTF_MSG_S(sys_cell_deconfig_cnf);


/***********************************************NON-ANCHOR ZONE**********************************************************/

typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_sys_mc_config_cnf_s;

NB1FW_INTF_MSG_S(sys_mc_config_cnf);

/***********************************************NON-ANCHOR ZONE**********************************************************/


typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_sys_cell_mc_config_cnf_s;

NB1FW_INTF_MSG_S(sys_cell_mc_config_cnf);

/* cell stat info structure */
typedef struct
{
  //uint16  dl_sfn_offset;      /* System frame offset from ref (0..1023) */
  uint16  dl_fullFr_offset;   /* 2 unused bits + HFN_offset:4 bits (0..15) + SFN_offset: 10bits(0..1023) */
  uint16  dl_pos_offset;      /* Sub radio frame offset (0..19200-1) */
  int32   rssi;               /* cell RSSI. dB10 Q7 */
  int32   pilotRssi;          /* pilot RSSI. dB10 Q7 */  
  int32   totalRssi;          /* totalRssi in dB10 Q7 */
  int32   rsrp;               /* cell RSRP. in log scale Q7 */
  int32  rsrq;                /* cell RSRQ. in log scale Q7 */
  int32   snr;                /* cell SNR . dB10 Q7 */
  int32  snr_linear;          /* cell SNR   Q7 linear */
  uint8   num_samp_done;      /* number of coh ave RSRP samples collected so far */
  uint8   num_samp_reqd;      /* total number of coh ave samples required for meas */
  int16   total_ferr_ppm_q10; /* total frequency error as seen by cell */
  boolean tx_enabled;         /* indicate if Tx is enabled */
  int16   dl_ul_pos_delta;    /* DL-UL pos delta */
  uint16  ul_ta;              /* current UL TA value, In steady state ul_ta = dl_ul_pos_delta */
  boolean cell_stat_valid;
} nb1fw_sys_cell_stat_s;

/* cell stat confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
  nb1fw_sys_cell_stat_s stat; /* cell stat info */
} nb1fw_sys_cell_stat_cnf_s;

NB1FW_INTF_MSG_S(sys_cell_stat_cnf);

typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */
} nb1fw_sys_log_mask_config_cnf_s;

NB1FW_INTF_MSG_S(sys_log_mask_config_cnf);

/* RF Rx sample capture confirm */
typedef struct
{
  nb1fw_cnf_hdr_s cnf_hdr; /* message header */

  uint8 last_lna_gain_state; /* last LNA gain state used */
  uint16 buffer_size;        /* buffer size */
  uint32* buffer_start_addr; /* starting address of buffer */
  uint32* sample_start_addr; /* starting address of first sample */
} nb1fw_sys_rx_sample_capture_cnf_s;

NB1FW_INTF_MSG_S(sys_rx_sample_capture_cnf);

/* cell stat indication */
typedef struct
{
  nb1fw_ind_hdr_s ind_hdr; /* message header */
  nb1fw_sys_cell_stat_s stat; /* cell stat info */
} nb1fw_sys_cell_stat_ind_s;

NB1FW_INTF_MSG_S(sys_cell_stat_ind);

/* cell stat indication */
typedef struct
{
  nb1fw_ind_hdr_s ind_hdr; /* message header */
} nb1fw_sys_mc_config_ind_s;

NB1FW_INTF_MSG_S(sys_mc_config_ind);

/* Union of all sys req messages */
typedef union
{
  msgr_hdr_struct_type hdr;
  NB1FW_INTF_MSG(sys_config_app_req);
  NB1FW_INTF_MSG(sys_cell_update_req);
  NB1FW_INTF_MSG(sys_cell_config_req);
  NB1FW_INTF_MSG(sys_cell_deconfig_req);
  NB1FW_INTF_MSG(sys_cell_mc_config_req);
  NB1FW_INTF_MSG(sys_cell_stat_req);
  NB1FW_INTF_MSG(sys_log_mask_config_req);
  NB1FW_INTF_MSG(sys_rx_sample_capture_req);
} nb1fw_sys_msg_req_u;

/* Union of all sys cnf messages */
typedef union
{
  msgr_hdr_struct_type hdr;
  NB1FW_INTF_MSG(sys_config_app_cnf);
  NB1FW_INTF_MSG(sys_cell_update_cnf);
  NB1FW_INTF_MSG(sys_cell_config_cnf);
  NB1FW_INTF_MSG(sys_cell_deconfig_cnf);
  NB1FW_INTF_MSG(sys_cell_mc_config_cnf);
  NB1FW_INTF_MSG(sys_cell_stat_cnf);
  NB1FW_INTF_MSG(sys_log_mask_config_cnf);
  NB1FW_INTF_MSG(sys_rx_sample_capture_cnf);
} nb1fw_sys_msg_cnf_u;

/* Union of all sys ind messages */
typedef union
{
  msgr_hdr_struct_type hdr;
  NB1FW_INTF_MSG(sys_cell_stat_ind);
} nb1fw_sys_msg_ind_u;

#endif /* INTF_NB1FW_SYS_H */

