
/*!
  @file
  nb1_ml1_log_ext.h

  @brief
  This file describes the external log interfaces to NB1 ML1
*/


/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_ml1_log_ext.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef NB1_ML1_LOG_EXT_H
#define NB1_ML1_LOG_EXT_H
#include "nb1_as.h"
#include "nb1_cphy_msg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! Maximum number of neighbor cells per request */
#define NB1_ML1_LOG_MAX_NCELLS_PER_REQ  16

/*! Maximum number of HRPD band config structures */
#define NB1_ML1_SM_LOG_HRPD_BAND_CONFIG_MAX  32

/* Max number of CCs currently supported (including primary)*/
#define NB1_ML1_GM_LOG_MAX_CC                                             3

/*! Maximum number of HRPD pilots */
#define NB1_ML1_SM_LOG_HRPD_PILOT_INFO_MAX   32

#define NB1_ML1_GM_LOG_MAX_PDCCH_RESULTS_PER_TTI                         (8)

#define NB1_ML1_GM_LOG_DCI_INFO_MAX_CNT                                 (10)

#define NB1_ML1_GM_LOG_PDSCH_STAT_IND_MAX_CNT                           (10)

#define NB1_ML1_LOG_GM_PUCCH_TX_N_1_PUCCH_NUM                            (4)
/*! Maximum number of HRPD pilots */
#define NB1_ML1_SM_LOG_HRPD_PILOT_INFO_MAX   32

#define NB1_ML1_LOG_MAX_MBSFN_ALLOCATIONS                       (8)
#define NB1_ML1_DLM_LOG_PDSCH_STAT_MAX_CNT                      (25)

#define NB1_ML1_DLM_LOG_BLER_STATS_CARRIER_MAX_CNT  LTE_LL1_CARRIER_COUNT

#define NB1_ML1_DLM_LOG_MAX_CARRIERS LTE_LL1_CARRIER_COUNT
#define NB1_ML1_DLM_LOG_LINK_CAP_MAX_CNT                        (10)
#define NB1_ML1_DLM_LOG_RES_AVAIL_FRAC_MAX_CNT                  (20)

#define NB1_ML1_BOLT_LOG_PACKET_BASE_VERSION                    (16)

#define NB1_ML1_DLM_LOG_MAX_DECODE_STAT_RECORDS (32)

#define NB1_ML1_DLM_LOG_MAX_DECODE_PAGE_RECORDS (1)

/*! @brief Main log structure for all serving cell information */
typedef PACK(struct)
{
  /*! Version */
  uint16  version:8;

    /*! Downlink bandwidth */
  uint16  dl_bandwidth:8;

  /*! System frame number (at ref time) */
  uint16   sfn;

  ///< serving cell earfcn
  uint32  earfcn;

  ///< serving  cell_ID
  uint16  cell_ID:9;

  ///< decode result: 1 = pass, 0= fail;
  uint16  decode_result:1;

  /*! The phich duration configures lower limit on the size of the control
      region signaled by the PCFICH. */
  uint16 phich_duration:3;

   /*! Parameter for determining the number of PHICH groups */
  uint16 phich_resource:3;

  uint16  reserved0;

   ///< serving cell PSS
  uint32  pss_corr_result;

  ///< serving cell PSS
  uint32  sss_corr_result;

  /*------------------------*/
  ///< 10 ms boundary;
  uint32 ref_time[2];

   ///< PBCH payload
  uint32 mib_payload;

   ///< updated frequency offset estimate (in Hz);
  ///< invalid if freq_offset_invalid
  uint16  freq_offset;

  ///< number of antennas for cell: 1, 2, or 4;
  uint16  num_antennas:2;


  uint16  reserved1:14;

} nb1_ml1_sm_log_serv_cell_info_pkt_s;


/*! @brief Serving Cell Measurement & Evaluation
*/
typedef struct
{
  /*! Version info */
  uint32  version:8;
  /*! standards version */
  uint32  standards_version:8;

  uint32  reserved0:16;

  /*! E-ARFCN */
  uint32  earfcn;

  /*! Physical cell ID, 0 - 504 */
  uint16  phy_cell_id:9;
  /*! Serving cell priority */
  uint16 serv_layer_prio:4;
  /*! reserved */
  uint32  reserved1:19;

  /*! measured RSRP, -140 ~ -40 dBm*/
  uint32  measured_rsrp:12;
  /*! True measured RSRP, -180 ~ -30 dBm*/
  uint32  true_measured_rsrp:12;
  /*! reserved */
  uint32  reserved2:8;

  /*! averge RSRP, -140 ~ -40 dBm*/
  uint32  avg_rsrp:12;
  /*! True averge RSRP, -180 ~ -30 dBm*/
  uint32  true_avg_rsrp:12;
  /*! reserved */
  uint32  reserved3:8;

  /*! measured RSRQ, -30 ~ 0*/
  uint32  measured_rsrq:10;
  /*! True measured RSRQ, -30 ~ +10 */
  uint32  true_measured_rsrq:10;
  /*! averge RSRQ, -30 ~ 0 */
  uint32  avg_rsrq:10;
  /*! reserved */
  uint32  reserved4:2;

  /*! True averge RSRQ, -30 ~ +10 */
  uint32  true_avg_rsrq:10;
  /*! measured RSSI, -110 ~ -10 */
  uint32  measured_rssi:11;
  /*! reserved */
  uint32  reserved5:11;

  /*! Qrxlevmin */
  uint32  q_rxlevmin:6;
  /*! P_max, unit of db, -30 to 33, 0x00=-30, 0x1=-29, ...0x3F=33,
      0x40=NP for not present */
  uint32  p_max:7;
  /*! max UE Tx Power */
  uint32  max_ue_tx_pwr:6;
  /*! Srxlev */
  uint32  s_rxlev:7;
  /*! num drxs S < 0 */
  uint32  num_drx_S_fail:6;

  /*! S_intra_search*/
  uint32  s_intra_search:6;
  /*! S_non_intra_search */
  uint32  s_non_intr_search:6;
  /*! meas rules updated or not */
  uint32  meas_rules_updated:1;
  /*! Measurement rules */
  uint32  meas_rules:4;
  /*! reserved */
  uint32  reserved6:15;

#ifdef FEATURE_NB1_REL9
  /*! Range -34 to -3 */
  uint32  q_qualmin:7;
  /*! Range -35 to +34 */
  uint32  s_qual:7;
  /*! Range 0 to +31 */
  uint32  s_intra_search_q:6;
  /*! Range 0 to +31 */
  uint32  s_non_intra_search_q:6;
  uint32  reserved7:6;
#endif
} nb1_ml1_sm_log_idle_serv_meas_eval_s;


/*! @brief Subpacket struct for serv cell measurement result*/
typedef struct
{
  /*! E-ARFCN, 0 ~ 39649 */
  uint32 earfcn;
  /*! Physical cell ID, 0 ~ 504*/
  uint16 phy_cell_id:9;
  /*! Serving cell index: 0..7 */
  uint16 serv_cell_index:3;
  uint16 is_serv_cell:1;
  uint16 reserved_0:3;
  uint16 rserved_00;

  /*! System frame time when measurements are made, 0 ~ 1023*/
  uint32 current_sfn:10;
  /*! System subframe time when measurements are made, 0 ~ 9*/
  uint32 current_subfn:4;
  uint32 reserved_1:18;

  /*!Measurement done on restricted subframe , 0 or 1 */
  uint32 is_subfm_restricted:1;
  /*! cell timing for rx antenna 0, 0 ~ 307199*/
  uint32 cell_timing_0:19;
  uint32 reserved_2:12;

  /*! cell timing for rx antenna 1, 0 ~ 307199*/
  uint32 cell_timing_1:19;
  /*! SFN for rx antenna 0 corresponding to cell timing, 0 ~ 1023*/
  uint32 cell_timing_sfn_0:10;
  uint32 reserved_3:3;

  /*! SFN for rx antenna 1 corresponding to cell timing, 0 ~ 1023*/
  uint32 cell_timing_sfn_1:10;
  /*! Inst RSRP value for antenna 0, -140 ~ -40 dBm*/
  uint32 inst_rsrp_rx_0:12;
  uint32 reserved_4:10;

  /*! True Inst RSRP value for antenna 0, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp_rx_0:12;
  /*! Inst RSRP value for antenna 1, -140 ~ -40 dBm*/
  uint32 inst_rsrp_rx_1:12;
  uint32 reserved_5:8;

  /*! True Inst RSRP value for antenna 0, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp_rx_1:12;
  /*! Inst RSRP value combined across tx-rx pairs, -140 ~ -40 dBm*/
  uint32 inst_rsrp:12;
  uint32 reserved_6:8;

  /*! True inst RSRP value combined across tx-rx pairs, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp:12;
  /*! Inst RSRQ value for antenna 0, -30 to 0 dBm*/
  uint32 inst_rsrq_rx_0:10;
  /*! True inst RSRQ value for antenna 0, -30 to +10 dBm*/
  uint32 true_inst_rsrq_rx_0:10;

  /*! Inst RSRQ value for antenna 0, -30 to 0 dBm*/
  uint32 inst_rsrq_rx_1:10;
  /*! True inst RSRQ value for antenna 1, -30 to +10 dBm*/
  uint32 true_inst_rsrq_rx_1:10;
  /*! Inst RSRQ value combined across tx-rx pairs, -30 ~ 0 dBm*/
  uint32 inst_rsrq:10;
  uint32 reserved_7:2;

  /*! True inst RSRQ value combined across tx-rx pairs, -30 ~ +10 dBm*/
  uint32 true_inst_rsrq:10;
  /*! Inst RSSI value for antenna 0, -110 ~ -10 dBm*/
  uint32 inst_rssi_rx_0:11;
  /*! Inst RSSI value for antenna 1, -110 ~ -10 dBm*/
  uint32 inst_rssi_rx_1:11;

  /*! Inst RSRQ value combined across tx-rx pairs, -110 ~ -10 dBm*/
  uint32 inst_rssi:11;
  uint32 reserved_8:21;

  /*! DVGA gain in log domain for antenna 0 and 1*/
  int16  dvga_rx_log_0;
  int16  dvga_rx_log_1;
  /*! LNA gain in log domain for antenna 0 and 1*/
  int16  lna_gain_rx_log_0;
  int16  lna_gain_rx_log_1;

  /*! Wideband CQI */
  uint16 cqi_web;
  /*! Residual frequency error */
  int16  res_freq_error;

  /*! FTL-SNR in linear scale for antenna 0 and 1*/
  uint32 ftl_snr_linear_rx_0;
  uint32 ftl_snr_linear_rx_1;
  /*! FTL-SNR value for antenna 0, -20 to 30 dBm*/
  uint32 ftl_snr_rx_0:9;
  /*! FTL-SNR value for antenna 1, -20 to 30 dBm*/
  uint32 ftl_snr_rx_1:9;
  uint32 reserved_9:14;

  /*! RS-SNR in linear scale for antenna 0 and 1*/
  uint32 rs_snr_linear_rx_0;
  uint32 rs_snr_linear_rx_1;
  /*! RS-SNR value for antenna 0, -20 to 30 dBm*/
  uint32 rs_snr_rx_0:9;
  /*! RS-SNR value for antenna 1, -20 to 30 dBm*/
  uint32 rs_snr_rx_1:9;
  uint32 reserved_10:14;

  /*! Projected SIR in Q_format 4 */
  int32 projected_sir;
  /*! Post IC RSRQ */
  uint32 post_ic_rsrq;

} nb1_ml1_sm_log_serv_cell_meas_response_subpkt_s;


/*! @brief Idle cell measurement info struct */
typedef struct
{
  /*! physical cell id, 0 ~ 503*/
  uint32 cell_id:9;
  /*! Cumulative frequency offset, 0 ~ 0xffff*/
  uint32 ftl_cumu_freq_offset:16;
  uint32 reserved_0:7;

  /*! Inst RSRP value for rx antenna 0, -140 ~ -40 dBm*/
  uint32 inst_rsrp_rx_0:12;
  /*! True Inst RSRP value for rx antenna 0, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp_rx_0:12;
  uint32 reserved_1:8;

  /*! Inst RSRP value for rx antenna 1, -140 ~ -40 dBm*/
  uint32 inst_rsrp_rx_1:12;
  /*! True Inst RSRP value for rx antenna 1, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp_rx_1:12;
  uint32 reserved_2:8;

  /*! Inst RSRP value combined across tx-rx pairs, -140 ~ -40 dBm*/
  uint32 inst_rsrp:12;
  /*! True Inst RSRP value combined across tx-rx pairs, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp:12;
  uint32 reserved_3:8;

  /*! Inst RSRQ value for rx antenna 0, -30 ~ 0 dBm*/
  uint32 inst_rsrq_rx_0:10;
  /*! True Inst RSRQ value for rx antenna 0, -30 ~ +10 dBm*/
  uint32 true_inst_rsrq_rx_0:10;
  /*! Inst RSRQ value for rx antenna 1, -30 ~ 0 dBm*/
  uint32 inst_rsrq_rx_1:10;
  uint32 reserved_4:2;

  /*! True Inst RSRQ value for rx antenna 1, -30 ~ +10 dBm*/
  uint32 true_inst_rsrq_rx_1:10;
  /*! Inst RSRQ value combined across tx-rx pairs, -30 ~ 0 dBm*/
  uint32 inst_rsrq:10;
  /*! True Inst RSRQ value combined across tx-rx pairs, -30 ~ 0 dBm*/
  uint32 true_inst_rsrq:10;
  uint32 reserved_5:2;

  /*! Inst RSRQ value for rx antenna 0, -110 ~ -10 dBm*/
  uint32 inst_rssi_rx_0:11;
  uint32 reserved_6:21;

  /*! Inst RSSI value for rx antenna 0, -110 ~ -10 dBm*/
  uint32 inst_rssi_rx_1:11;
  /*! Inst RSSI value combined acros tx-rx pairs, -110 ~ -10 dBm*/
  uint32 inst_rssi:11;
  uint32 reserved_7:10;

  /*! DVGA gain in log domain for antenna 0 and 1*/
  int32  dvga_rx_log_0:16;
  int32  dvga_rx_log_1:16;
  /*! LNA gain in log domain for antenna 0 and 1*/
  int32  lna_gain_rx_log_0:16;
  int32  lna_gain_rx_log_1:16;

  /*! Frame bounrary ref time 0, 0 ~ 0xffff_ffff*/
  uint32 frame_bdry_ref_time_0;
  /*! Frame bounrary ref time 1, 0 ~ 0xffff_ffff*/
  uint32 frame_bdry_ref_time_1;

  /*! Total timing adj to CIR 0, 0 ~ 0xffff*/
  uint32 total_timing_adj_cir_0:16;
  /*! Total timing adj to CIR 1, 0 ~ 0xffff*/
  uint32 total_timing_adj_cir_1:16;

} nb1_ml1_sm_log_idle_ncell_meas_response_info_s;


/*! @brief Subpacket struct for idle cell measurement response*/
typedef struct
{
  /*! Cell frequency measured*/
  uint32 earfcn:32;

  /*! Number of cell measurements, 0 ~ 16 */
  uint32 num_cells:5;
  /*! Current count of SWRP in MP, 1 ~ 4*/
  uint32 cnt_swrp:2;
  uint32 duplexing_mode:2;
  /*! Serving cell index: 0..7 */
  uint32 serv_cell_index:3;
  uint32 reserved_0:20;

  /*! Cell measuerment info list */
  nb1_ml1_sm_log_idle_ncell_meas_response_info_s
        meas_resp_info_list[NB1_ML1_LOG_MAX_NCELLS_PER_REQ];

} nb1_ml1_sm_log_ncell_meas_response_subpkt_s;


/*! @brief Connected neighbor cell meas paramters*/
typedef struct
{
  /*! SW reporting period in ms, 40, 80 or 160*/
  uint16 swrp:8;
  /*! Scheduling period in ms, 40, 80 or 160*/
  uint16 sp:8;

  /*! ncell occasion enum*/
  uint16 occasion:2;
  /*! Number of scheduling periods per measurement period, 1, 2 or 4*/
  uint16 num_sp_per_mp:3;
  /*! Duration of measurement gap in ms, cannot be more than 15 */
  uint16 gap_duration:4;
  /*! One bit per scheduling period.
    0: no gap
    1:Measurement gap in the end of scheduling period */
  uint16 gap_mask:1;
  uint16 reserved:6;

} nb1_ml1_sm_log_ncell_conn_s;


/*! @brief Idle neighbor cell measurement request paramters*/
typedef struct
{
  /*! Physical cell id, 0 ~ 503 */
  uint16 cell_id:9;

  /*! cell cyclic prefix type
      0x00 = NB1_L1_CP_MODE_NORMAL
      0x01 = NB1_L1_CP_MODE_EXTENDED
      0x02 = NB1_L1_CP_MODE_EXTENDED_MBSFN
      0x03 = NB1_L1_MAX_NUM_CP_MODES*/
  uint16 cp_type:2;

  /*! Number of tx antenna for the eNb, 1 or 2*/
  uint16 eNb_tx_ant:2;
  uint16 ttl_enable:1;
  uint16 ftl_enable:1;
  uint16 reserved_0:1;

  /*! valid if ftl_enable is true*/
  uint16 ftl_cumu_freq_offset;

  /*! valid if ttl_enable is true*/
  uint32 frame_bdry_ref_time_0;
  uint32 frame_bdry_ref_time_1;

  uint16 total_timing_adj_cir_0;
  uint16 total_timing_adj_cir_1;

} nb1_ml1_sm_log_ncell_meas_req_param_s;


/*! @brief Subpacket struct for connected ncell meas request*/
typedef struct
{
  /*! Cell earfcn, 0 ~ 65535 */
  uint32 earfcn:32;

  /*! Number of neighbor cells to measure, 1 ~ 16 */
  uint32 num_ncells:4;
  /*! Number of TX antennas, 1 ~ 2 */
  uint32 num_rx_ant:2;
  /*! Serving cell index: 0..7 */
  uint32 serv_cell_index:3;
  uint32 reserved_0:23;

  nb1_ml1_sm_log_ncell_conn_s conn_params;

  nb1_ml1_sm_log_ncell_meas_req_param_s
          cell_list[NB1_ML1_LOG_MAX_NCELLS_PER_REQ];

} nb1_ml1_sm_log_conn_ncell_meas_req_subpkt_s;


/*! @brief DRX Request
*/
typedef struct
{
  /*! Version info */
  uint32  version:8;
  uint32  reserved0:24;

  /*! E-ARFCN Range: 0-262143*/
  uint32   earfcn;  /* 32 bits */

  /*! Physical cell ID, 0 - 504 */
  uint16  phy_cell_id;

	/*! Nb 0x0 = fourT 0x1 = twoT 0x2 = oneT 0x3 =  halfT 0x4 = quarterT
	0x5 = oneEightT 0x6 = onSixteenthT 0x7 = oneThirtySecondT*/
  uint16  nb;

  /*! UE ID IMSI mod 1024. Range 0 to 1023 */
  uint16 ueid;

  /*! UE ID IMSI mod 1024. Range 0 to 1023 */
  uint16 num_rep;

  /*! Paging SFN offset Range 0 to 1023 */
  uint16  paging_occ_sfn_offset;

  /*! Paging sub-FN offset Range 0 to 9*/
  uint8  paging_occ_subfn_offset;
	
  /*! paging cycle index 0x0 = 1280 ms
	  0x1 = 2560 ms
	  0x2 = 5120 ms
	  0x3 = 10240 ms
	  */
  uint8	paging_cycle_index;
} nb1_ml1_common_log_paging_drx_info_s;


typedef struct
{
  /*! Results pertaining to each cell */
  /* PN position In units of chipx8 Range 0...307200 (38400*8) */
  uint32                                          pn_pos;

  /*!Scrambling code for this peak  Range 0..511*/
  uint16                                          psc;

  /*********************************************************************
  ** Units of the three items below (RSSI, RSCP, Ec/Io) are two times **
  ** the actual dBm value, giving a resolution for each of 0.5 dBm.   **
  *********************************************************************/

  /*!The received wide band power, including thermal noise and
  noise generated in the receiver,within the bandwidth defined
  by the receiver pulse shaping filter Ref 3gpp 25.215*/
  int16                                           cpich_rssi;

  /*!Received Signal Code Power, the received power on one code
  measured on the Primary CPICH. Ref 3gpp 25.215*/
  int16                                           cpich_rscp;

  /*!The received energy per chip divided by the power density
  in the band. The CPICH Ec/No is identical to CPICH RSCP/UTRA
  Carrier RSSI. Measurement shall be performed on the Primary
  CPICH. Ref : 3gpp spec 25.215*/
  int16                                           cpich_ecno;

  /*!Filtered RSCP*/
  int16                                           cpich_rscp_filtered;

  /*!Filtered ECNO*/
  int16                                           cpich_ecno_filtered;

}nb1_ml1_sm_log_conn_meas_wcdma_cell_results_s;


/*! CDMA band class configuration structure */
typedef struct
{
  /*! Band class (Range 0...31) */
  uint8 band;
  /*! Cell reselection priority (Range 0...7) */
  uint8 priority;
  /*! Threshold, high-prio layer (Range 0...63) */
  uint8 threshX_high;
  /*! Threshold, low-prio layer (Range 0...63) */
  uint8 threshX_low;

} nb1_ml1_sm_log_idle_cdma_meas_config_band_config_s;


/*! CDMA neighbor pilot information structure */
typedef struct
{
  /*! Band class (Range 0...31) */
  uint8 band;
  /*! Reserved */
  uint8  reserved1;
  uint16 reserved2;
  /*! Channel (Range 0...2047) */
  uint16 channel;
  /*! Pilot id (Range 0...511) */
  uint16 pilot;

} nb1_ml1_sm_log_idle_cdma_meas_config_pilot_info_s;


/*! CDMA reselection parameters structure */
typedef struct
{
  /*! Treselection in seconds (Range 0...7) */
  uint8 Tresel;
  /*! Medium mobility scaling factor
     0x01 = 0.25, 0x2 = 0.50, 0x3 = 0.75, 0x4 = 1.00 */
  uint8 Tresel_medium;
  /*! High mobility scaling factor
     0x01 = 0.25, 0x2 = 0.50, 0x3 = 0.75, 0x4 = 1.00 */
  uint8 Tresel_high;
  /*! Number of band class configurations */
  uint8 num_bands;
  /*! Number of neighbor pilots */
  uint8 num_pilots;
  /*! Reserved */
  uint8  reserved1;
  uint16 reserved2;
  /*! Band class configuration list */
  nb1_ml1_sm_log_idle_cdma_meas_config_band_config_s
    band_list[NB1_ML1_SM_LOG_HRPD_BAND_CONFIG_MAX];
  /*! Neighbor pilot list */
  nb1_ml1_sm_log_idle_cdma_meas_config_pilot_info_s
    pilot_list[NB1_ML1_SM_LOG_HRPD_PILOT_INFO_MAX];

} nb1_ml1_sm_log_idle_cdma_meas_config_resel_params_s;


/*! NB1 ML1 CDMA Idle Meas Config Subpacket */
typedef struct
{
  /*! CDMA window size  (Range 0...15) */
  uint8 window_size;
  /*! Reserved */
  uint8  reserved1;
  uint16 reserved2;
  /*! HRPD reselection parameters */
  nb1_ml1_sm_log_idle_cdma_meas_config_resel_params_s hrpd;
  /*! 1x reselection parameters */
  nb1_ml1_sm_log_idle_cdma_meas_config_resel_params_s onex;

} nb1_ml1_sm_log_idle_cdma_meas_config_subpkt_s;

/*! @brief WCDMA Idle Measurement Configuration subpackets*/
typedef struct
{
  uint16  frequency:       16;
  uint16  cell_resel_prio:  8;
  uint16  thresh_high:      8;

  uint16  thresh_low:       8;
  uint16  Qrxlevmin_act:    8;
  uint16  P_max:            8;
  uint16  Q_qual_min:       8;

} nb1_ml1_sm_log_idle_wcdma_meas_config_freq_info_s;


typedef struct
{
  uint8   tresel:         8;
  uint8   sf_med:         8;
  uint8   sf_high:        8;
  uint8   num_wcdma_freq: 8;

  nb1_ml1_sm_log_idle_wcdma_meas_config_freq_info_s
      frequency [NB1_CPHY_MEAS_MAX_WCDMA_FREQ];
} nb1_ml1_sm_log_idle_wcdma_meas_config_subpkt_s;


typedef struct
{
  uint16 freq;
  uint16 padding;
} nb1_ml1_sm_log_idle_gsm_meas_config_freq_s;


typedef struct
{
  uint8  resel_prio:    8;
  uint8  thresh_x_high: 8;
  uint8  thresh_x_low:  8;
  uint8  qrxlev_min:    8;

  uint8  P_max:         8;
  uint8  NCC_permitted: 8;
  uint16 num_freqs:    16;

} nb1_ml1_sm_log_idle_gsm_meas_config_freq_group_help_s;


typedef struct
{
  nb1_ml1_sm_log_idle_gsm_meas_config_freq_group_help_s info;
  nb1_ml1_sm_log_idle_gsm_meas_config_freq_s freqs[NB1_CPHY_MEAS_MAX_GERAN_FREQS_IN_GROUP];
} nb1_ml1_sm_log_idle_gsm_meas_config_freq_group_s;


typedef struct
{

  uint8 t_resel: 8;
  uint8 sf_med:  8;
  uint8 sf_high: 8;

  uint8 num_freq_groups: 8;
}nb1_ml1_sm_log_idle_gsm_meas_config_subpkt_help_s;


typedef struct
{
  nb1_ml1_sm_log_idle_gsm_meas_config_subpkt_help_s info;

  nb1_ml1_sm_log_idle_gsm_meas_config_freq_group_s freq_groups[NB1_CPHY_MEAS_MAX_GERAN_FREQ_GROUPS];/*16*/
}nb1_ml1_sm_log_idle_gsm_meas_config_subpkt_s;


/*! @brief GM DCI Info Log Record definition
*/
typedef struct
{
  /* First Word */
  uint32  npdcch_timing_hsfn:10;  ///< NPDCCH end HSFN
  uint32  npdcch_timing_sfn:10;   ///< NPDCCH end SFN
  uint32  npdcch_timing_sub_fn:4; ///< NPDCCH end sub frame
  uint32  rnti_type:3;			  ///< RNTI type   -- can this column be display in APEX at the right side of "npdcch_timing_sub_fn"
								  ///< 0x0 - SI-RNTI
								  ///< 0x1 - C-RNTI
								  ///< 0x2 - P-RNIT
								  ///< 0x3 - RA-RNTI
								  ///< 0x4 - TC-RNTI
  uint32  ul_grant_present:1;     ///< flag if UL grant is present
                                  ///< 0x0: no UL grant
                                  ///< 0x1: DCI grant is UL grant(DCI N0)
                                  
  uint32  dl_grant_present:1;     ///< flag if DL grant is present
                                  ///< 0x0: no DL grant
                                  ///< 0x1: DCI grant is DL grant(DCI N1)
                                  
  uint32  pdcch_order_present:1;  ///< flag if this is PDCCH order DCI N1 grant
                                  ///< 0x0: not PDCCH order DCI N1 grant
                                  ///< 0x1: is PDCCH order DCI N1 grant
                                  
  uint32  ndi:1;                  ///< NDI
  uint32  reserved1:1;           ///< reserved

  /* second word */
  uint32  sc_idx:6;               ///< Subcarrier indication
  uint32  rv:1;                   ///< Redundancy version, only valid if ul_grant_present = 1
  uint32  res_asign:3;            ///< resource assignment
  uint32  schld_delay:3;          ///< scheduling delay
  

  
  uint32  mcs:4;                  ///< modulation and coding scheme
  uint32  rep_num:4;              ///< repetition number
  uint32  carr_ind:4;             ///< carrier indication for non anchor
  uint32  dci_rep_num:2;          ///< DCI subframe repetition number
  uint32  harq_res:4;             ///< HARQ-ACK resource, this IE is only valid when dl_grant_presnet=1
                                  ///< and pdcch_order_present = 0
  uint32  harq_id:1;              ///< HARQ ID received in on the DCI
}nb1_ml1_gm_log_dci_info_record_s;


/*! @brief GM DCI Info Log packet definition
*/
typedef struct
{
  /* Log header */
  log_hdr_type header;

  /* First Word */
  uint32  version:8;           ///< version
  uint32  reserved0:8;        ///< reserved
  uint32  num_records:8;       ///< number of record
  uint32  carrier_index:8;     ///< Anchor/Non-Anchor Carrier index
  nb1_ml1_gm_log_dci_info_record_s
    dci_info_record [NB1_ML1_GM_LOG_DCI_INFO_MAX_CNT];
} nb1_ml1_gm_log_dci_info_s;

/*! @brief N1 PDSCH Stat Info Log Record definition
*/
typedef struct
{
  uint32  sfn:10;
  uint32  sub_fn:4;
  uint32  rnti_type:3;            ///<rnti type: 0->SI-RNTI, 1->C-RNTI, 2->P-RNTI, 3->RA-RNTI, 4->TC-RNTI
  uint32  crc:1;                  ///<crc info: 0x0->fail, 0x01->pass
  uint32  trblk_size:14;          ///<tb size decoded
  
  uint32  hsfn:10;                 //Hyper frame number
  uint32  mcs:4;                  ///< modulation and coding scheme
  uint32  sf:3;                   ///< resource assignment
  uint32  autoAck:1;              ///indicate decode skipped and auto-acked previous one , 0:FALSE, 1:TRUE
  uint32  ndi:1;                  ///indicate new transmission or retransmission,  
  uint32  harq_id:1;              ///Indicates the HARQ ID corresponding to the transmission
  uint32  reserved:12;            ///< reserved
}nb1_ml1_gm_log_pdsch_stat_ind_record_s;


/*! @brief GM PDSCH Stat Ind Log packet definition
*/
typedef struct
{
  /* Log header */
  log_hdr_type header;

  /* First Word */
  uint32  version:8;           ///< version
  uint32  reserved0:16;        ///< reserved
  uint32  num_records:8;       ///< number of record
  nb1_ml1_gm_log_pdsch_stat_ind_record_s
    pdsch_stat_ind_record [NB1_ML1_GM_LOG_PDSCH_STAT_IND_MAX_CNT];
} nb1_ml1_gm_log_pdsch_stat_ind_s;


/*! @brief GM Tx N_1_pucch record
*/
typedef struct
{
  uint16  n_1_pucch_i:12;
  uint16  reserved:4;
} nb1_ml1_gm_log_n_1_pucch_record_s;

/*! @brief GM PUSCH Tx Report Record definition */
typedef struct
{
  /* First word */
  uint32 chan_type:1;
  uint32 sfn:10;
  uint32 sub_fn:4;
  uint32 trblk_size:14;
  uint32 csf_present_flag:1;
  uint32 ack_nak_present_flag:2;                   //32 bits

  /* Second word */
	uint32 ca_mode_enabled:1;
	uint32 ack_reporting_mode:3;
	uint32 ack_nak_to_send_mask:9;
	uint32 ack_nak_to_send_mask_scell_1:9;
	uint32 ack_nak_to_send_mask_scell_2:9;
	uint32 dci_0_present:1;

	/* Third word */
	uint32 w_ul_dai:4;
  uint32 n_bundled:4;
  uint32 end_of_bundling:4;
  uint32 ack_nak_len:3;
  uint32 beta_pusch:16;
	uint32 reserved1:1;															//32 bits

  /* Fourth word */
  uint32 total_tx_power:8;
  uint32 cyclic_shift_dmrs:3;
  uint32 rb_start:7;
  uint32 rv:2;
  uint32 mod_type:2;
  uint32 num_rbs:7;
  uint32 harq_id:3;																//32 bits

  /* Fifth word */
  uint32 retx_index:5;
  uint32 hopping_flag:1;
  uint32 reserved2:2;
  uint32 harq_ack_offset:4;
  uint32 cqi_offset:4;
  uint32 ri_offset:4;
  uint32 hopping_payload:2;
  uint32 srs_present_flag:1;
  uint32 srs_ue_or_cell_specific:1;
  uint32 n_dmrs:3;
  uint32 antenna_num:2;
  uint32 tti_bundl_index:3;												  //32 bits

  /* Sixth word */
  uint32 eib_index:1;
  uint32 mcs_index:5;
  uint32 padding:26;                               //32 bits

}nb1_ml1_gm_log_pusch_tx_report_record_s;

/*! @brief GM PUCCH Tx Report Record definition */
typedef struct
{
  /* First word */
  uint32 chan_type:1;
  uint32 sfn:10;
  uint32 sub_fn:4;
  uint32 total_tx_power:8;
  uint32 ack_nak_present_flag:2;
	uint32 ca_mode_enabled:1;
	uint32 ack_reporting_mode:3;
  uint32 ack_nak_len:2;
  uint32 reserved0:1;												  	//32 bits

  /* Second word */
	uint32 ack_nak_to_send_mask:9;
	uint32 ack_nak_to_send_mask_scell_1:9;
	uint32 ack_nak_to_send_mask_scell_2:9;
	uint32 reserved1:5;

	/* Third word */
  uint32 beta_pucch:16;
	uint32 reserved2:1;
  uint32 n_1_pucch:12;
  uint32 n_1_pucch_num:3;                       //32 bits

  /* Fourth and Fifth words */
  nb1_ml1_gm_log_n_1_pucch_record_s
    n_1_pucch_records[NB1_ML1_LOG_GM_PUCCH_TX_N_1_PUCCH_NUM];   // 64 bits

  /* Sixth word */
  uint32 n_2_pucch:10;
  uint32 n_3_pucch:10;
  uint32 sr_present_flag:1;
  uint32 trni_ack_flag:1;
  uint32 srs_present_flag:1;
  uint32 srs_ue_or_cell_specific:1;
  uint32 csf_present_flag:1;
  uint32 padding:7;                 	         //32 bits
}nb1_ml1_gm_log_pucch_tx_report_record_s;

/*! @brief GM Tx Report Log packet union */
typedef union
{
  nb1_ml1_gm_log_pucch_tx_report_record_s pucch_tx_report;
  nb1_ml1_gm_log_pusch_tx_report_record_s pusch_tx_report;
} nb1_ml1_gm_log_tx_report_u;

/*! @brief GM PDCCH PHICH Info Log Record definition
*/
typedef struct
{
  uint32  num_pdcch_results:3;
  uint32  pdcch_timing_sfn:10;
  uint32  pdcch_timing_sub_fn:4;
  uint32  phich_present:1;
  uint32  phich_1_present:1;
  uint32  phich_timing_sfn:10;
  uint32  reserved0:3;                   //32 bits
  uint32  phich_timing_sub_fn:4;
  uint32  phich_val:1;
  uint32  phich_1_val:1;
  uint32  reserved1:26;                  //32 bits

  struct
  {
    uint32 carr_id:3;
    uint32 rnti_type:4;
    uint32 payload_size:7;
    uint32 aggregation_lvl:2;
    uint32 srch_space:1;
    uint32 sps_grant_type:3;
    uint32 new_dl_tx:1;
    uint32 num_dl_trblks:2;
    uint32 reserved2:9;                 //32 bits
  }pdcch_info[NB1_ML1_GM_LOG_MAX_PDCCH_RESULTS_PER_TTI];
}nb1_ml1_gm_log_npdcch_info_record_s;

/*! @brief GM Tx Report Log packet struct */
typedef struct
{
  /* First Word */
  uint32 npusch_timing_hsfn:10; ///< NPUSCH OTA HSFN
  uint32 npusch_timing_sfn:10;  ///< NPUSCH OTA SFN
  uint32 npusch_timing_sub_fn:4; ///< NPUSCH OTA sub frame
  uint32 npusch_format:1; ///< NPUSCH format
                          ///< 0x0: NPUSCH format 1
                          ///< 0x1: NPUSCH format 2

  uint32 is_msg3:1;       ///< if NPUSCH is MSG3
                          ///< 0x0: NPUSCH is not MSG3
                          ///< 0x1: NPUSCH is MSG3

  uint32 itbs:4;          ///< ITBS, this IE is only valid if npusch_format = 0
  uint32 reserved1:2;      ///< reserved

  /* Second Word */
  uint32 rep_num:8;       ///< number of NPUSCH repetition
                          ///< valid: 1,2,4,8,...,128
  uint32 num_ru:4;        ///< number of RU, valid: 1~10

  
  uint32 rv_idx:1;        ///< RV index, this IE is only valid if npusch_format = 0
  uint32 num_tone:4;      ///< number of tones, valid: 1,3,6,12
  uint32 start_tone:7;    ///< start tone index, valid 0~47
  uint32 tx_power:8;      ///< NPUSCH transmit power in dBm

  /* Third Word */
  uint32 npusch_fmt_1_tx_type:1;  ///< flag if NPUSCH foramt 1 is new transmission, this
                                  ///< IE is only valid if npusch_format = 0
                                  ///< 0x0: NPUSCH format 1 is retransmission
                                  ///< 0x1: NPUSCH format 1 is new transmission

  uint32 ack_nack:1;              ///< NPUSCH format 2 ACK/NACK payload this
                                  ///< IE is only valid if npusch_format = 1
                                  ///< 0x0: NACK
                                  ///< 0x1: ACK

  uint32 rflm:8;                  ///< RFLM handler index
  uint32 prach_coll_valid:1;      ///< flag if PRACH collision happens
                                  ///< 0x0: not collide with network configured PRACH
                                  ///< 0x1: collide with network configured PRACH

  uint32 prach_coll_7680_ms:1;    ///< when PRACH collision happens, if this is 7680 ms 
                                  ///< long bitmask case, this IE is only valid if 
                                  ///< prach_coll_valid = 1
                                  ///< 0x0: not 7680ms long bitmask
                                  ///< 0x1: use 7680ms long bitmask
  uint32 harq_id:1;               ///< HARQ ID Corresponding to the Tx
  uint32 ul_carrier_freq:19;      ///< UL carrier frequency

  /* Forth Word */
  uint32 scramb_mask;             ///< scrambling value, equals to (n_rnti << 14) + N_Ncell_ID
} nb1_ml1_gm_log_tx_report_record_s;

/*! @brief ULM Common config log struct
*/
typedef struct
{
  /* Version */
  uint32 version:8;
  uint32 reserved:24;                                  //32 bits

  /* PUSCH Config */
  uint32 pusch_cfg_present:1;
  uint32 pusch_n_sb:2;
  uint32 pusch_hopping_mode:1;
  uint32 pusch_hopping_offset:7;
  uint32 pusch_enable_64_qam:1;
  uint32 pusch_group_hopping_enabled:1;
  uint32 pusch_group_assignment_pusch:5;
  uint32 pusch_seq_hopping_enabled:1;
  uint32 pusch_cyclic_shift:3;

  /* PUCCH Config */
  uint32 pucch_cfg_present:1;
  uint32 pucch_delta_shift:2;
  uint32 pucch_n_rb_cqi:7;                             //32 bits
  uint32 pucch_n_cs_an:3;
  uint32 pucch_n1_pucch_an:11;

  /* SRS Config */
  uint32 srs_cfg_present:1;
  uint32 srs_enable:1;
  uint32 srs_bw_cfg:3;
  uint32 srs_subframe_config:4;
  uint32 srs_sim_ack_nak_srs:1;
  uint32 srs_max_up_pts:1;
  uint32 reserved1:7;                                  //32 bits

} nb1_ml1_ulm_log_common_cfg_s;

/*! @brief ULM Dedicated config log struct
*/
typedef struct
{
  /* Version */
  uint32 version:8;
  uint32 std_ver:8;
  uint32 reserved:16;                                   //32 bits

  /* SRS Config */
  uint32 srs_cfg_present:1;
  uint32 srs_enable:1;
  uint32 srs_bw:2;
  uint32 srs_hopping_bw:2;
  uint32 srs_freq_domain_position:5;
  uint32 srs_duration:1;
  uint32 srs_cfg_index:10;
  uint32 srs_tx_comb:1;
  uint32 srs_cyclic_shift:3;

  /* CQI Config */
  uint32 cqi_cfg_present:1;
  uint32 cqi_enable:1;
  uint32 cqi_reporting_aperiodic_present:1;
  uint32 cqi_reporting_mode_aperiodic:3;                  // 32 bits
  uint32 cqi_reporting_periodic_present:1;
  uint32 cqi_format_indicator:1;
  uint32 cqi_nom_pdsch_rs_epre_offset:4;
  uint32 cqi_pucch_resource_index:11;
  uint32 cqi_ri_cfg_enabled:1;
  uint32 cqi_ri_cfg_index:10;
  uint32 cqi_subband_cqi_k:3;
  uint32 cqi_sim_ack_nak_cqi:1;                           //32 bits
  uint32 cqi_pmi_cfg_index:10;
  uint32 cqi_mask:1;

  /* Antenna Config */
  uint32 antenna_cfg_present:1;
  uint32 antenna_transmission_mode:3;
  uint32 antenna_tx_antenna_sel_enable:1;
  uint32 antenna_tx_antenna_sel_ctrl:1;
  uint32 reserved1:15;                                    //32 bits
  uint32 antenna_codebook_subset_rest[2];                 //64 bits

} nb1_ml1_ulm_log_dedicated_cfg_s;

/*! @brief DLM common cfg log structure
	Packet ID: LOG_NB1_ML1_DLM_COMN_CFG_LOG_C
*/
typedef struct
{
  /*!***************** Version *****************/
  /*! Log packet version */
  uint32  version:8;                                      //32bit word1 start

  /*!***************** UL freq cfg *****************/
  /*! UL freq cfg: cfg valid if present */
  uint32  ul_freq_comn_cfg_present:1;                     
  uint32  ul_freq:16;
  uint32  ul_freq_offset_present:1;
  uint32  offset:6;                                       //32bit word1 end

  uint32  spec_emisson_present:1;                         //32bit word2 start
  uint32  spec_emission:6;                                

  /*!***************** PCCH cfg *****************/
  /*! PCCH cfg: cfg valid if present */
  uint32  pcch_present:1;                                   
  uint32  pcch_paging_cycle:16;                          
  uint32  pcch_nb:4;
  uint32  pcch_rep:4;                                     //32bit word2 end

  /*!***************** GAP cfg *****************/
  /*! GAP cfg: cfg valid if present */
  uint32  gap_present:1;                                   //32bit word3 start
  uint32  gap_enable:1;
  uint32  gap_thresh:3;
  uint32  gap_per:3;
  uint32  gap_coeff:3;

  /*!***************** MIB info *****************/
  /*! MIB info: cfg valid if present */                  
  uint8 mib_info_present:1;
  uint8 num_enodeb_tx_ant:3;

  /*!***************** PDSCH common cfg *****************/
  /*! PDSCH common cfg: cfg valid if present */
  uint32 pdsch_comn_cfg_present:1;
  uint32 pad:16;                                            //32bit word3 end
  int8  ref_signal_pwr;                                     //32bit word4 start
  int8  pade[3];                                            //32bit word4 end

} nb1_ml1_log_dlm_comn_cfg_s;

/*! @brief DLM common cfg log structure
	Packet ID: LOG_NB1_ML1_DLM_SIB1_COMN_CFG_LOG_C
*/
typedef struct
{
  /*!***************** Version *****************/
  /*! Log packet version */
  uint32  version:8;                                   //32bit word1 start

  /*!***************** DL BITMAP cfg *****************/
  /*! DL BITMAP cfg: cfg valid if present */
  uint32  dl_bm_cfg_present:1;                     
  uint32  dl_bm_cfg_type:1;
  uint32  dl_bm_cfg_pattern:1;
  
  /*!***************** EUTRA CNTRL cfg *****************/
  /*! DL BITMAP cfg: cfg valid if present */
  uint32  eutra_cfg_present:1;                     
  uint32  eutra_cfg_size:2;
  
  /*!***************** NRS CRS cfg *****************/
  /*! DL BITMAP cfg: cfg valid if present */
  uint32  nrscrs_cfg_present:1;                     
  uint32  nrscrs_cfg_pwr_offset:5;                       
  uint32  pad:12;                                  //32bit word1 end

  uint32  dl_bm_cfg_bm_lo;                         //32bit word2
  uint32  dl_bm_cfg_bm_hi;                         //32bit word3      

} nb1_ml1_log_dlm_sib1_comn_cfg_s;

/*! @brief DLM dedicated cfg log structure
    Packet ID: LOG_NB1_ML1_DLM_DED_CFG_LOG_C
*/
typedef struct
{
  /*!***************** Version *****************/
  uint32  version:8;                                        //32bit word1 start

  /*!***************** CARRIER cfg *****************/
  uint32  carrier_cfg_present:1;
  uint32  carrier_ul_freq_offset_present:1;
  uint32  carrier_dl_freq_offset_present:1;
  uint32  carrier_ul_freq_offset:5;

  uint32  carrier_dl_freq_offset:5;
  uint32  carrier_dl_na_bm:2;                                
  uint32  carrier_dl_na_sf_pattern:1;

  uint32  carrier_dl_in_band_idx_to_mid_prb:8;              //32bit word1 end

  uint32  carrier_ul_freq;                                  //32bit word2
  uint32  carrier_dl_freq;                                  //32bit word3
  uint32  carrier_dl_na_dl_bitmap_hi;                       //32bit word4
  uint32  carrier_dl_na_dl_bitmap_lo;                       //32bit word5

  uint32  carrier_dl_na_dl_gap:2;                           //32bit word6 start
  uint32  carrier_dl_na_exp_dl_gap_enable:1;
  uint32  carrier_dl_na_exp_dl_gap_thresh:2;
  uint32  carrier_dl_na_exp_dl_gap_periodicity:2;
  uint32  carrier_dl_in_band_carrier_info:1;

  uint32  carrier_dl_na_exp_dl_gap_coeff:2;
  uint32  carrier_dl_in_band_pci_indicator:1;
  uint32  carrier_dl_in_band_num_crs_ports:1;
  uint32  carrier_dl_in_band_control_size:2;
  uint32  carrier_pad:18;                                  //32bit word6 end

  /*!***************** NPDCCH cfg *****************/
  uint32  npdcch_cfg_present:1;                      //32bit word8 start
  uint32  npdcch_num_rep:4;
  uint32  npdcch_start_sf_uss:3;
  uint32  npdcch_offset_uss:2;
  uint32  npdcch_pad:22;                            //32bit word8 end

  /*!***************** NPUSCH cfg *****************/
  uint32   npusch_cfg_present:1;                        //32bit word9 start
  uint32   npusch_enable:4;
  uint32   npusch_enable_ack_nak_num_rep:1;
  uint32   npusch_ack_nak_num_rep:3;
  uint32   npusch_npusch_all_symbols:1;
  uint32   npusch_group_hopping_disabled:1;
  uint32   npusch_pad:21;                            //32bit word9 end

  /*!***************** UL PWR CNTL cfg *****************/
  uint32   ulpwr_cfg_present:1;                        //32bit word10 start
  uint32   ulpwr_p0_ue_pusch:8;

  /*!***************** RLF Timer cfg *****************/
  uint32   rlftmr_cfg_present:1;    
  uint32   rlftmr_enable:1;                       
  uint32   rlftmr_t310:4;
  uint32   rlftmr_n310:5;
  uint32   rlftmr_n311:4;                               
  

  /*!***************** CDRX cfg *****************/
  uint32   cdrx_cfg_present:1;                   
  uint32   cdrx_drx_enable:1;
  uint32   cdrx_pad:6;                          //32bit word10 end

  uint32   cdrx_on_duration_timer:8;            //32bit word11 start
  uint32   cdrx_inactivity_timer_new:8;
  uint32   cdrx_retx_timer:8;
  uint32   cdrx_drx_start_offset:8;             //32bit word11 end

  uint32   cdrx_drx_cycle:16;                   //32bit word12 start
  uint32   cdrx_drx_ul_retx_tmr:16;             //32bit word12 end

} nb1_ml1_log_dlm_ded_cfg_s;

/*! DLM decode statistics 
    Packet ID: LOG_NB1_ML1_DLM_DECODE_STATS_LOG_C
*/
typedef struct 
{
  uint8 type;
  uint8 crc_pass;

  uint16 frame_num;   
  uint16 subframe_num;
  uint16 rep_accum;   

  uint16 block_size;
  uint16 egy_metric;
  uint16 sym_err;

} nb1_ml1_log_dlm_decode_stat_result_s;

/*! DLM decode statistics 
    Packet ID: LOG_NB1_ML1_DLM_DECODE_STATS_LOG_C
*/
typedef struct 
{
  uint8     version;
  uint8     num_records;
  uint16    pad;  

  nb1_ml1_log_dlm_decode_stat_result_s stats[NB1_ML1_DLM_LOG_MAX_DECODE_STAT_RECORDS];
} nb1_ml1_log_dlm_decode_stat_record_s;

/*! DLM page decode statistics 
    Packet ID: LOG_NB1_ML1_DLM_DECODE_PAGE_LOG_C
*/
typedef struct 
{
	uint32	sfn:10;
	uint32	sub_fn:4;
	uint32	rnti_type:3;			///<rnti type: 0->SI-RNTI, 1->C-RNTI, 2->P-RNTI, 3->RA-RNTI, 4->TC-RNTI
	uint32	crc:1;					///<crc info: 0x0->fail, 0x01->pass
	uint32	format:1;			    ///<Format info: 0x0->Direct indicaiton info, 0x01->PAGE info
	uint32	hsfn:10;				//Hyper frame number
	uint32	dci_rep:3;			    ///< DCI subframe repetition number
	
	uint32	rep:4;					///< Repetition Number
	uint32	mcs:4;					///< modulation and coding scheme
	uint32	ru:3;					///< resource assignment
	uint32  direct_ind:6;           ///< direct indication information if present
	uint8	carrier_index:4;		//Carrier Index 0 to 15 (Anchor Index == 15)
	uint32  reserved:11;            //Second word complete
	
	uint32	dl_carrier_freq:18; 	//<DL carrier Frequency: (18bits)-> cell's frequency Range: 0..262143
	uint32	reserved2:14;			//Third word complete
} nb1_ml1_log_dlm_decode_page_result_s;

/*! DLM page decode statistics 
    Packet ID: LOG_NB1_ML1_DLM_DECODE_PAGE_LOG_C
*/
typedef struct 
{
  /* First Word */
  uint32	version:8;			 ///< version
  uint32	reserved0:16;		 ///< reserved
  uint32	num_records:8;		 ///< number of record

  nb1_ml1_log_dlm_decode_page_result_s records;
} nb1_ml1_log_dlm_decode_page_record_s;

/*! DLM FW cell configuration 
    Packet ID: LOG_NB1_ML1_DLM_CELL_CONFIGURATION_LOG_C
*/
typedef struct 
{
  uint8     version;
  
  // TODO - PACK fields efficiently
  uint8      hfn_offset;
  uint8      ttl_mode;
  uint8      ftl_mode;
  
  uint16     pcell_id;
  uint16     sfn_offset;
  
  uint16     pos_offset;
  int16      init_ferr_ppm_q10;

  uint16     nbrs_valid_mask[4];

  uint32     dl_carrier_freq;
  uint32     inv_dl_freq;
  uint32     dl_earfcn;
  uint32     rflm_handle;
} nb1_ml1_log_dlm_cell_config_record_s;

/*! DLM FW cell updated configuration 
    Packet ID: LOG_NB1_ML1_DLM_CELL_UPDATE_LOG_C
*/
typedef struct 
{
  uint8     version;
  
  // TODO - PACK fields efficiently
  uint8      hfn_offset;
  uint8      num_tx_ant;
  uint8      op_mode;

  int8       raster_offset_khz_q4;
  uint8      lte_ctrl_syms;
  uint8      crs_num_ports;
  uint8      crs_seq_info;

  uint16     ul_dl_freq_ratio;
  uint16     sfn_offset;

  int16      crs_power_offset;
  int16      pad1;

  uint16     nbrs_valid_mask[4];

} nb1_ml1_log_dlm_cell_update_record_s;

/*! DLM FW payload indication 
    Packet ID: LOG_NB1_ML1_DLM_PAYLOAD_IND_LOG_C
*/
typedef struct 
{
  uint8     version;
  
  // TODO - PACK fields efficiently
  uint8      type;
  uint8      crc_pass;
  uint8      rnti_type;

  uint16 cell_id;
  uint16 pad;

  uint32 earfcn;
  
} nb1_ml1_log_dlm_payload_ind_s;

/*! @brief SM NB1 SYSTEM Info Log packet definition
*/
typedef struct
{
  /* Log header */
  log_hdr_type header;

  /* First Word */
  uint32  version:8;   ///< version

  /*! NB1 operation/deployment mode
     0 = NB1_OP_MODE_SAME_PCI
     1 = NB1_OP_MODE_INBAND_DIFFERENT_PCI
     2 = NB1_OP_MODE_GUARDBAND 
     3 = NB1_OP_MODE_STANDALONE
     4 = NB1_OP_MODE_UNKNOW     */
  uint32  opmode:3; 
  /*!Bandwidth of the system
      0x0 = 6 RBs
      0x1 = 15 RBs
      0x2 = 25 RBs
      0x3 = 50 RBs
      0x4 = 75 RBs
      0x5 = 100 RBs */
  uint32 meas_bw:3;
  /*!cell ID of the system Range: 0-504 */
  uint32  cell_id:9;
  uint32 reserved:9;

  /* Second word */
  /*! frequency */
  uint32  freq;        

  /* Third word */
  /*! inst meas rsrp,  units of dBm, display as int type*/
  uint32 inst_meas_rsrp:11;

  /*!Srxlev, In units of dB, display as int type  */
  uint32 srxlev:11;

  uint32 reserved2:10;

} nb1_ml1_log_sm_system_info_s;

/*! @brief NB1 ML1 SM NGBR MEAS Log packet definition
*/
typedef struct
{
  uint32 pcell_id:9;        ///<Pcell Id 
  uint32 sfn_offset:10;     ///<SFN (systems frame number) offset from reftime (0..1023)
  uint32 reserved1:13;      ///<reserved

  uint32  earfcn;           /*! E-ARFCN info */

  uint32 pos_offset;        ///< Position offset
  
  int32 ferr_ppm_q10;		///<Sub radio frame position (0..19200-1)
	
  int32 rssi;			///<cell RSSI. dB, Q0 
  
  int32 rsrp;			///<cell RSRP. dB, Q0
  
  int32 rsrq;			///<cell RSRQ. dB, Q0
} nb1_ml1_sm_ngbr_meas_cell_info_s;

typedef struct
{
  uint32  version:8;   ///< version
  uint32  num_cells:4;   ///<number of cells logged
  uint32  reserved:20;

  nb1_ml1_sm_ngbr_meas_cell_info_s cell_info[NB1FW_DL_NUM_MEAS_CELLS]; ///<meas cell info
  
} nb1_ml1_sm_ngbr_meas_log_pkt_s;


/*! @brief LOG_NB1_ML1_SERV_CELL_MEAS_RESPONSE_LOG_C Log packet definition
*/
typedef struct
{
  int32   version:8;
  int32   pci:9;            ///<Pcell Id 
  int32   reserved:15;
  
  /*! E-ARFCN */
  uint32   earfcn;          /* 32 bits */

  int32  ferr_ppm_q10;      ///<Sub radio frame position (0..19200-1) 
  
  int32  rssi_int;          ///<cell RSSI 

  int32  rsrp_int;          ///<cell RSRP 
  
  int32  rsrq_int;          ///<cell RSRQ 

  int32  Srxlev_int;        ///<cell Srxlev
  
  int32  rank_int;          ///<cell rank 

  uint32  sss_snr_linear;   ///<cell SNR  Q7 linear 

  int32   sss_snr_db;      ///< in units of dB, Q7 format display as value/128
} nb1_ml1_sm_serv_meas_cell_info_s;


/*! @brief LOG_NB1_ML1_CELL_RESEL_LOG_C Log packet definition
*/
typedef struct
{
  uint32  pci:16;                     ///<Pcell Id
  int32   rank:16;                    ///<Rank
  uint32  tresel:16;                  ///<Tresel value
  uint32  reserved:16;                ///<Reserved bits
}nb1_ml1_mgmt_db_cell_info_log;

typedef struct
{
  uint32 num_cells:16;              ///<number of cells on each layer
  uint32 freq:16;                   ///<frequency
  
  nb1_ml1_mgmt_db_cell_info_log  cell_info[NB1_ML1_MAX_NUM_MEAS_CELLS];      ///<cell specific info
 
}nb1_ml1_mgmt_db_layer_info_log;

typedef struct
{
  uint32          version:8;                     ///<version
  uint32          num_layers:8;                 ///<number of layers
  uint32          reserved:16;                  ///<reserved
  
  nb1_ml1_mgmt_db_layer_info_log  layer_info[NB1_ML1_MDB_MAX_LAYERS_AGGREGATE];   ///<layer specific info
             
} nb1_ml1_mgmt_db_cell_resel_log_s;

/*! @brief LOG_NB1_ML1_PBCH_DECODE_LOG_C Log packet definition
*/
typedef struct
{
  uint32   version:8;
  uint32   reserved_0:24;

  /*! MIB payload */
  uint32   payload;
  
  uint32   earfcn;
  
  uint32   cell_id:16;

  /*! 0 = Fail, 1 = Pass */
  uint32    decode_result:1;

  /*! NB1 operation/deployment mode
     0 = NB1_OP_MODE_SAME_PCI
     1 = NB1_OP_MODE_INBAND_DIFFERENT_PCI
     2 = NB1_OP_MODE_GUARDBAND 
     3 = NB1_OP_MODE_STANDALONE
     4 = NB1_OP_MODE_UNKNOW     */
  uint32   opmode:3;

  /*!Bandwidth of the system
      0x0 = 6 RBs
      0x1 = 15 RBs
      0x2 = 25 RBs
      0x3 = 50 RBs
      0x4 = 75 RBs
      0x5 = 100 RBs */
  uint32   meas_bw:3;

  /*! Flag to indicate if access barring is enabled */
  uint32   access_barring_enabled:1;
  /*! Number of eNB Tx antenna */
  uint32   num_tx_antenna:8;

  /*! MSB of system frame number */
  uint32   sfn_msb:8;
  /*! LSB of hyper system frame number */
  uint32   hypersfn_lsb:8;
  /*! SIB1 scheduling index/ repetition. Range 0,1,..11*/
  uint32   schedulingInfoSIB1:8;
  /*! SIB information tag */
  uint32   systemInfoValueTag:8;

  /*! SFN offset for timeslam */
  uint32   sfn_offset:16;
  /*! HSFN offset for timeslam */
  uint32   hfn_offset:16;
  } nb1_ml1_sm_pbch_decode_resp_info_s;


#endif /* NB1_ML1_LOG_EXT_H */
