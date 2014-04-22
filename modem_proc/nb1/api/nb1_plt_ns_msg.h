/*!
  @file nb1_plt_ns_msg.h

  @brief
  This file contains the interface to the FTM NB1 Non-signaling Processor

*/

/*============================================================================
                Copyright (c) 2008 - 2009 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_plt_ns_msg.h#1 $
$Author: pwbldsvc $
$DateTime: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/16/15   sk      Created

===========================================================================*/

#ifndef NB1_PLT_NS_MSG_H
#define NB1_PLT_NS_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "nb1_l1_types.h"
#include "nb1_cphy_msg.h"
#include "nb1_cphy_ftm_msg.h"
#include "nb1_mac_rrc.h"
#include "nb1_rlc_rrc.h"
#include "nb1_mac_msg.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define NB1_PLT_NS_MIN_DL_DATA_LCID 1
#define NB1_PLT_NS_MAX_DL_DATA_LCID 10
#define NB1_PLT_NS_MIN_UL_DATA_LCID 1
#define NB1_PLT_NS_MAX_UL_DATA_LCID 10
#define NB1_PLT_NS_MIN_MAX_TX_PWR   -50
#define NB1_PLT_NS_MAX_MAX_TX_PWR   33
#define NB1_PLT_NS_MIN_ADD_SPEC_EMI 1
#define NB1_PLT_NS_MAX_ADD_SPEC_EMI 32

#define NB1_PLT_NS_MAX_CARRIERS     5

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_START_NB1_MODE_REQ
 */
typedef struct
{
  uint32 reserved;
} nb1_plt_ns_start_mode_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_START_NB1_MODE_CNF
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_start_mode_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_STOP_NB1_MODE_REQ
 */
typedef struct
{
  uint32 reserved;
} nb1_plt_ns_stop_mode_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_STOP_NB1_MODE_CNF
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_stop_mode_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_ACQ_REQ
 */
typedef struct
{
  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;
  /* E-UTRA Absolute Radio Frequency Channel Number. This goes with the 
   * band to calcualte the exact frequency to tune to 
   */
  nb1_earfcn_t         earfcn;
  /* NW configured customized tx-rx spacing*/
  boolean custom_duplex_spacing_enabled;
} nb1_plt_ns_acq_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_ACQ_CNF
 */
typedef struct
{
  errno_enum_type       status;
  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;
  /* E-UTRA Absolute Radio Frequency Channel Number. This goes with the 
   * band to calcualte the exact frequency to tune to 
   */
  nb1_earfcn_t         earfcn;
  /* Cell ID of the acquired system. 0..503 */
  nb1_phy_cell_id_t    cell_id;
} nb1_plt_ns_acq_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_IDLE_REQ
 */
typedef struct
{
  uint32 reserved;
} nb1_plt_ns_idle_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_IDLE_CNF
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_idle_cnf_msg_type;

/*! @brief This structure holds the parameters of START_DP_REQ that was last sent to NB1 L1
           These parameters will be used to configure DP data path
    @param c_rnti the C-RNTI used by the UE
    @param add_spec_emission_cfg The NS_XX value for additional spectrum emission configuration
    @param bs_ue_max_tx_pwr The maximum UE TX power limited by the BS
    @param dl_lc_id  The Logical Channel ID used for the DL channel, 
                     This ID will be used also as Radio Bearer ID, which is
                     needed for RLC configuration but not relevant in 
                     Non-singaling
    @param ul_lc_id  The Logical Channel ID used for UL channel,
                     This ID will be used also as Radio Bearer ID, which is
                     needed for RLC configuration but not relevant in
                     Non-Signaling
    @param ta_adj    Timing Advancement adjustment
*/
typedef struct
{
  uint32                ta_adj;
  nb1_rnti_val_t        c_rnti;
  uint8                 add_spec_emission_cfg;
  int8                  bs_ue_max_tx_pwr;
  nb1_lc_id_t           dl_lc_id;
  nb1_lc_id_t           ul_lc_id;
  /*! NPDCCH Number of repetitions*/
  nb1_l1_npdcch_num_rep_e num_rep;
  /*! NPDCCH Start SF USS*/
  nb1_l1_npdcch_start_sf_uss_e start_sf_uss;
} nb1_plt_ns_dp_req_data_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_START_DP_REQ
 *
 */
typedef struct
{
  nb1_plt_ns_dp_req_data_type dp_cfg_data;
} nb1_plt_ns_start_dp_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_START_DP_CNF
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_start_dp_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_DL_DATA_IND
 */
typedef struct
{
  uint32 reserved;
} nb1_plt_ns_dl_data_ind_msg_type;

/*! @brief This structure holds the parameters of CONFIG_UL_WAVEFORM sent to NB1 L1
           
*/
typedef struct
{
 /* MCS Index */
 uint8 mcs_idx; 
 /* Number of RU */
 uint8 num_ru; 
 /* Tone BW type.  */
 uint8 tone_bw; 
 /* Starting tone for RU */
 uint8 start_tone_idx; 
 /* Tx power in dBm */
 int8 tx_pow_dbm;
} nb1_plt_ns_config_ul_waveform_req_data_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_CONFIG_UL_WAVEFORM_REQ
 */
typedef struct
{
  nb1_plt_ns_config_ul_waveform_req_data_type ul_waveform_data;
} nb1_plt_ns_config_ul_waveform_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_CONFIG_UL_WAVEFORM_RSP
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_config_ul_waveform_cnf_msg_type;

/*! @brief This structure holds the parameters of CONFIG_UL_POWER sent to NB1 FW
           
*/
typedef struct
{
 uint8 tx_chain;
 uint8 tx_power_control_mode;
 int16 tx_level_dBm;
} nb1_plt_ns_config_ul_power_req_data_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_CONFIG_UL_WAVEFORM_REQ
 */
typedef struct
{
  nb1_plt_ns_config_ul_power_req_data_type ul_power_data;
} nb1_plt_ns_config_ul_power_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_CONFIG_UL_WAVEFORM_RSP
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_config_ul_power_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_GET_DL_LEVEL_REQ
*/
typedef struct
{
  uint8 rx_chain;
} nb1_plt_ns_get_dl_level_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_GET_DL_LEVEL_CNF
 */
typedef struct
{
  errno_enum_type       status;
  int16                 rx_agc; /*! Rx AGC units */
  int16                 rx_power_dBm; /*! Rx Power in dBm units*/
} nb1_plt_ns_get_dl_level_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_RESET_DL_BLER_REPORT_REQ
 */
typedef struct
{
  uint8 rx_chain;
} nb1_plt_ns_reset_dl_bler_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_RESET_DL_BLER_REPORT_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} nb1_plt_ns_reset_dl_bler_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_GET_DL_BLER_REPORT_REQ
 */
typedef struct
{
  uint32 reserved;
} nb1_plt_ns_get_dl_bler_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_GET_DL_BLER_REPORT_CNF
 */
typedef struct
{
  errno_enum_type       status;
  uint32                total_blocks_received;
  uint32                total_block_errors; /*NACKS*/
  uint32                throughput;  /*Raw bits per sec*/
} nb1_plt_ns_get_dl_bler_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_GET_ALL_CARR_DL_BLER_REPORT_REQ
 */
typedef struct
{
  uint32 reserved;
} nb1_plt_ns_get_all_carr_dl_bler_req_msg_type;

/*! @brief This structure defines the data format of the
 *         NB1_PLT_NS_GET_DL_BLER_REPORT_CNF
 */
typedef struct
{
  /* Indicated which carrier id the data belongs to */
  uint8                 carrier_id;
  uint32                total_blocks_received;
  uint32                total_block_errors; /*NACKS*/
  uint32                throughput;  /*Raw bits per sec*/
} nb1_plt_ns_get_all_carr_dl_bler_cnf_data_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_GET_ALL_CARR_DL_BLER_REPORT_CNF
 */
typedef struct
{
  errno_enum_type       status;
  /*! Number of valid elements in dl_bler_per_carrier[] */
  uint8 num_carriers;

  nb1_plt_ns_get_all_carr_dl_bler_cnf_data_type dl_bler_per_carrier[NB1_PLT_NS_MAX_CARRIERS];
} nb1_plt_ns_get_all_carr_dl_bler_cnf_msg_type;

/*! @brief This structure holds the parameters of ENABLE_SCELL_REQ to be sent to NB1 L1
*/
typedef struct
{
  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;
  /* E-UTRA Absolute Radio Frequency Channel Number. This goes with the 
   * band to calcualte the exact frequency to tune to 
   */
  nb1_earfcn_t         earfcn;

  /*! The cell id this configuration applies to or the source cell in case of
      handover */
  /* Cell ID of the acquired system. 0..503 */
  nb1_phy_cell_id_t    cell_id;
} nb1_plt_ns_scell_req_data_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_ENABLE_SCELL_REQ
 */
typedef struct
{
  nb1_plt_ns_scell_req_data_type dl_data;
} nb1_plt_ns_enable_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_ENABLE_SCELL_CNF
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_enable_scell_cnf_msg_type;

/*! @brief This structure holds the parameters of DL MULTI_ENABLE_SCELL_REQ to be sent to NB1 L1
*/
typedef struct
{
  uint8                          carr_cfg_mask;
  nb1_plt_ns_scell_req_data_type scell_data[NB1_PLT_NS_MAX_CARRIERS-1];
} nb1_plt_ns_dl_multi_scell_req_data_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_ENABLE_MULTI_SCELL_REQ
 */
typedef struct
{
  nb1_plt_ns_dl_multi_scell_req_data_type dl;
} nb1_plt_ns_enable_multi_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_ENABLE_MULTI_SCELL_CNF
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_enable_multi_scell_cnf_msg_type;

/*! @brief This structure holds the UL parameters of ENABLE_SCELL_RX_TX_REQ 
 *         to be sent to NB1 L1
 */
typedef struct
{
  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;
  /* E-UTRA Absolute Radio Frequency Channel Number. This goes with the 
   * band to calcualte the exact frequency to tune to 
   */
  nb1_earfcn_t         earfcn;

  /*! The cell id this configuration applies to or the source cell in case of
      handover */
  /* Cell ID of the acquired system. 0..503 */
  nb1_phy_cell_id_t    cell_id;

  uint16 c_rnti;
  uint8 ns_ca_value;
  uint8 uplink_lcid;
  int8 ue_pwr_limit;
  uint8 uplink_bandwidth;

} nb1_plt_ns_ul_scell_req_data_type;

/*! @brief This structure holds the parameters of UL MULTI_ENABLE_SCELL_REQ to be sent to NB1 L1
*/
typedef struct
{
  uint8                             carr_cfg_mask;
  nb1_plt_ns_ul_scell_req_data_type scell_data [NB1_PLT_NS_MAX_CARRIERS-1];
} nb1_plt_ns_ul_multi_scell_req_data_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_ENABLE_MULTI_SCELL_RX_TX_REQ
 */
typedef struct
{
  /* DL */
  nb1_plt_ns_dl_multi_scell_req_data_type dl;

  /* UL */
  nb1_plt_ns_ul_multi_scell_req_data_type ul;
} nb1_plt_ns_enable_multi_scell_rx_tx_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_ENABLE_MULTI_SCELL_RX_TX_CNF
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_enable_multi_scell_rx_tx_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_DISABLE_SCELL_REQ
 */
typedef struct
{
  uint32 reserved;
} nb1_plt_ns_disable_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_DISABLE_SCELL_CNF
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_disable_scell_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_DISABLE_MULTI_SCELL_REQ
 */
typedef struct
{
  uint8                dl_carr_cfg_mask;
} nb1_plt_ns_disable_multi_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_DISABLE_MULTI_SCELL_CNF
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_disable_multi_scell_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_DISABLE_MULTI_SCELL_RX_TX_REQ
 */
typedef struct
{
  uint8                dl_carr_cfg_mask;
  uint8                ul_carr_cfg_mask;
} nb1_plt_ns_disable_multi_scell_rx_tx_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_DISABLE_MULTI_SCELL_RX_TX_CNF
 */
typedef struct
{
  errno_enum_type       status;
} nb1_plt_ns_disable_multi_scell_rx_tx_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_ACTIVE_DL_CARRIER_MASK_REQ
 */
typedef struct
{
  uint16 active_dl_carrier_mask;
} nb1_plt_ns_active_dl_carrier_mask_req_msg_type;

/*! @brief This structure defines the message format of the
 *         NB1_PLT_NS_ACTIVE_DL_CARRIER_MASK_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} nb1_plt_ns_active_dl_carrier_mask_cnf_msg_type;

/* This enum is used to define actions releated to NB1 NS request */
typedef enum
{
  NB1_PLT_NS_EXT_API_MSG_START_NB1_MODE_REQ            = 0x1,
  NB1_PLT_NS_EXT_API_MSG_STOP_NB1_MODE_REQ,
  NB1_PLT_NS_EXT_API_MSG_ACQ_REQ,
  NB1_PLT_NS_EXT_API_MSG_IDLE_REQ,
  NB1_PLT_NS_EXT_API_MSG_START_DP_REQ,
  NB1_PLT_NS_EXT_API_MSG_IS_CONN_REQ,
  NB1_PLT_NS_EXT_API_MSG_CONFIG_UL_WAVEFORM_REQ,
  NB1_PLT_NS_EXT_API_MSG_CONFIG_UL_POWER_REQ,
  NB1_PLT_NS_EXT_API_MSG_GET_DL_LEVEL_REQ,
  NB1_PLT_NS_EXT_API_MSG_RESET_DL_BLER_REPORT_REQ,
  NB1_PLT_NS_EXT_API_MSG_GET_DL_BLER_REPORT_REQ,
  NB1_PLT_NS_EXT_API_MSG_ENABLE_SCELL_REQ,
  NB1_PLT_NS_EXT_API_MSG_DISABLE_SCELL_REQ,
  NB1_PLT_NS_EXT_API_MSG_GET_ALL_CARR_DL_BLER_REQ,
  NB1_PLT_NS_EXT_API_MSG_ENABLE_MULTI_SCELL_REQ,
  NB1_PLT_NS_EXT_API_MSG_DISABLE_MULTI_SCELL_REQ,
  NB1_PLT_NS_EXT_API_MSG_ENABLE_MULTI_SCELL_RX_TX_REQ,
  NB1_PLT_NS_EXT_API_MSG_DISABLE_MULTI_SCELL_RX_TX_REQ,
  NB1_PLT_NS_EXT_API_MSG_REQ_MAX                        = 0xFF
} nb1_plt_ns_ext_api_req_msg_e;

/* This enum is used to define actions releated to NB1 NS confirmation */
typedef enum
{
  NB1_PLT_NS_EXT_API_MSG_START_NB1_MODE_CNF            = 0x1,
  NB1_PLT_NS_EXT_API_MSG_STOP_NB1_MODE_CNF,
  NB1_PLT_NS_EXT_API_MSG_ACQ_CNF,
  NB1_PLT_NS_EXT_API_MSG_IDLE_CNF,
  NB1_PLT_NS_EXT_API_MSG_START_DP_CNF,
  NB1_PLT_NS_EXT_API_MSG_IS_CONN_CNF,
  NB1_PLT_NS_EXT_API_MSG_CONFIG_UL_WAVEFORM_CNF,
  NB1_PLT_NS_EXT_API_MSG_CONFIG_UL_POWER_CNF,
  NB1_PLT_NS_EXT_API_MSG_GET_DL_LEVEL_CNF,
  NB1_PLT_NS_EXT_API_MSG_RESET_DL_BLER_REPORT_CNF,
  NB1_PLT_NS_EXT_API_MSG_GET_DL_BLER_REPORT_CNF,
  NB1_PLT_NS_EXT_API_MSG_ENABLE_SCELL_CNF,
  NB1_PLT_NS_EXT_API_MSG_DISABLE_SCELL_CNF,
  NB1_PLT_NS_EXT_API_MSG_GET_ALL_CARR_DL_BLER_CNF,
  NB1_PLT_NS_EXT_API_MSG_ENABLE_MULTI_SCELL_CNF,
  NB1_PLT_NS_EXT_API_MSG_DISABLE_MULTI_SCELL_CNF,
  NB1_PLT_NS_EXT_API_MSG_ENABLE_MULTI_SCELL_RX_TX_CNF,
  NB1_PLT_NS_EXT_API_MSG_DISABLE_MULTI_SCELL_RX_TX_CNF,
  NB1_PLT_NS_EXT_API_MSG_CNF_MAX                        = 0xFF
} nb1_plt_ns_ext_api_cnf_msg_e;

/*! @brief Union of all msgs for NB1 NS request
*/
typedef union
{
  /* FTM NB1 Non-signaling messages */
  nb1_plt_ns_start_mode_req_msg_type                 start_mode_req_msg;
  nb1_plt_ns_stop_mode_req_msg_type                  stop_mode_req_msg;
  nb1_plt_ns_acq_req_msg_type                        acq_req_msg;
  nb1_plt_ns_start_dp_req_msg_type                   start_dp_req_msg;
  nb1_plt_ns_idle_req_msg_type                       idle_req_msg;
//  nb1_plt_ns_is_conn_req_msg_type                    is_conn_req_msg;
  nb1_plt_ns_config_ul_waveform_req_msg_type         config_ul_waveform_req_msg;
  nb1_plt_ns_config_ul_power_req_msg_type            config_ul_power_req_msg;
  nb1_plt_ns_get_dl_level_req_msg_type               get_dl_level_req_msg;
  nb1_plt_ns_reset_dl_bler_req_msg_type              reset_dl_bler_req_msg;
  nb1_plt_ns_get_dl_bler_req_msg_type                get_dl_bler_req_msg;
  nb1_plt_ns_enable_scell_req_msg_type               enable_scell_req_msg;
  nb1_plt_ns_disable_scell_req_msg_type              disable_scell_req_msg;
  nb1_plt_ns_enable_multi_scell_req_msg_type         enable_multi_scell_req_msg;
  nb1_plt_ns_enable_multi_scell_rx_tx_req_msg_type   enable_multi_scell_rx_tx_req_msg;
  nb1_plt_ns_disable_multi_scell_req_msg_type        disable_multi_scell_req_msg;
  nb1_plt_ns_disable_multi_scell_rx_tx_req_msg_type  disable_multi_scell_rx_tx_req_msg;
  nb1_plt_ns_active_dl_carrier_mask_req_msg_type     active_dl_carrier_mask_req_msg;
  nb1_plt_ns_get_all_carr_dl_bler_req_msg_type       get_all_carr_dl_bler_req_msg;

} nb1_plt_ns_ext_api_req_msg_u;

/*! @brief Union of all msgs for NB1 NS confirmation
*/
typedef union
{
  nb1_plt_ns_start_mode_cnf_msg_type                start_mode_cnf_msg;
  nb1_plt_ns_stop_mode_cnf_msg_type                 stop_mode_cnf_msg;
  nb1_plt_ns_acq_cnf_msg_type                       acq_cnf_msg;
  nb1_plt_ns_start_dp_cnf_msg_type                  start_dp_cnf_msg;
  nb1_plt_ns_idle_cnf_msg_type                      idle_cnf_msg;
//  NB1_plt_ns_is_conn_cnf_msg_type                   is_conn_cnf_msg;
  nb1_plt_ns_config_ul_waveform_cnf_msg_type        config_ul_waveform_cnf_msg;
  nb1_plt_ns_config_ul_power_cnf_msg_type           config_ul_power_cnf_msg;
  nb1_plt_ns_get_dl_level_cnf_msg_type              get_dl_level_cnf_msg;
  nb1_plt_ns_reset_dl_bler_cnf_msg_type             reset_dl_bler_cnf_msg;
  nb1_plt_ns_get_dl_bler_cnf_msg_type               get_dl_bler_cnf_msg;
  nb1_plt_ns_enable_scell_cnf_msg_type              enable_scell_cnf_msg;
  nb1_plt_ns_disable_scell_cnf_msg_type             disable_scell_cnf_msg;
  nb1_plt_ns_enable_multi_scell_cnf_msg_type        enable_multi_scell_cnf_msg;
  nb1_plt_ns_enable_multi_scell_rx_tx_cnf_msg_type  enable_multi_scell_rx_tx_cnf_msg;
  nb1_plt_ns_disable_multi_scell_cnf_msg_type       disable_multi_scell_cnf_msg;
  nb1_plt_ns_disable_multi_scell_rx_tx_cnf_msg_type disable_multi_scell_rx_tx_cnf_msg;
  nb1_plt_ns_active_dl_carrier_mask_cnf_msg_type    active_dl_carrier_mask_cnf_msg;
  nb1_plt_ns_get_all_carr_dl_bler_cnf_msg_type      get_all_carr_dl_bler_cnf_msg;

} nb1_plt_ns_ext_api_cnf_msg_u;



/*! @brief External API callback function for response to exteraml API request
  
*/
typedef errno_enum_type (*nb1_plt_ns_ext_api_cb_func_type)( nb1_plt_ns_ext_api_cnf_msg_e msg_id );

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
 
  FUNCTION:  nb1_plt_ns_register_ext_api_cb 

===========================================================================*/
/*!
  @brief This function registers callback function and data structure for
  the request/response of Non-Signaling external API request 

  @param Pointer to the External API callback and req/cnf data structure  
  
  @return TRUE if suceeded
          FALSE if others
*/
/*=========================================================================*/
boolean nb1_plt_ns_register_ext_api_cb ( nb1_plt_ns_ext_api_cb_func_type cb,
                                         nb1_plt_ns_ext_api_req_msg_u    *req_msg_ptr,
                                         nb1_plt_ns_ext_api_cnf_msg_u    *cnf_msg_ptr);

/*===========================================================================
 
  FUNCTION:  nb1_plt_ns_ext_api_processor 

===========================================================================*/
/*!
  @brief This function handles Non-signaling request from other tasks
  (RF FTM, Off target) 

  @param Message ID 
  
  @return E_SUCCESS if external api process has sucessfully initaited
          others if failed 
*/
/*=========================================================================*/
errno_enum_type nb1_plt_ns_ext_api_processor ( nb1_plt_ns_ext_api_req_msg_e msg_id);

#endif /*  NB1_PLT_NS_MSG_H */

