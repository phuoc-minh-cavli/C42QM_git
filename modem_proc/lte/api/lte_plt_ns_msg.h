/*!
  @file lte_plt_ns_msg.h

  @brief
  This file contains the interface to the FTM LTE Non-signaling Processor

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

$Header: //components/rel/lte.mpss/4.7.2/api/lte_plt_ns_msg.h#1 $
$Author: pwbldsvc $
$DateTime: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/16/15   sk      Created

===========================================================================*/

#ifndef LTE_PLT_NS_MSG_H
#define LTE_PLT_NS_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lte_l1_types.h"
#include "lte_cphy_msg.h"
#include "lte_cphy_ftm_msg.h"
#include "lte_mac_rrc.h"
#include "lte_rlc_rrc.h"
#include "lte_mac_msg.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define LTE_PLT_NS_MIN_DL_DATA_LCID 1
#define LTE_PLT_NS_MAX_DL_DATA_LCID 10
#define LTE_PLT_NS_MIN_UL_DATA_LCID 1
#define LTE_PLT_NS_MAX_UL_DATA_LCID 10
#define LTE_PLT_NS_MIN_MAX_TX_PWR   -50
#define LTE_PLT_NS_MAX_MAX_TX_PWR   33
#define LTE_PLT_NS_MIN_ADD_SPEC_EMI 1
#define LTE_PLT_NS_MAX_ADD_SPEC_EMI 32

#define LTE_PLT_NS_MAX_CARRIERS     5

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_START_LTE_MODE_REQ
 */
typedef struct
{
  uint32 reserved;
} lte_plt_ns_start_mode_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_START_LTE_MODE_CNF
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_start_mode_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_STOP_LTE_MODE_REQ
 */
typedef struct
{
  uint32 reserved;
} lte_plt_ns_stop_mode_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_STOP_LTE_MODE_CNF
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_stop_mode_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_ACQ_REQ
 */
typedef struct
{
  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;
  /* E-UTRA Absolute Radio Frequency Channel Number. This goes with the 
   * band to calcualte the exact frequency to tune to 
   */
  lte_earfcn_t         earfcn;

  /* NW configured customized tx-rx spacing*/
  boolean custom_duplex_spacing_enabled;
} lte_plt_ns_acq_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_ACQ_CNF
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
  lte_earfcn_t         earfcn;
  /* Cell ID of the acquired system. 0..503 */
  lte_phy_cell_id_t    cell_id;
} lte_plt_ns_acq_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_IDLE_REQ
 */
typedef struct
{
  uint32 reserved;
} lte_plt_ns_idle_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_IDLE_CNF
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_idle_cnf_msg_type;

/*! @brief This structure holds the parameters of START_DP_REQ that was last sent to LTE L1
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
  lte_rnti_val_t        c_rnti;
  uint8                 add_spec_emission_cfg;
  int8                  bs_ue_max_tx_pwr;
  lte_lc_id_t           dl_lc_id;
  lte_lc_id_t           ul_lc_id;
} lte_plt_ns_dp_req_data_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_START_DP_REQ
 *
 */
typedef struct
{
  lte_plt_ns_dp_req_data_type dp_cfg_data;
} lte_plt_ns_start_dp_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_START_DP_CNF
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_start_dp_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_DL_DATA_IND
 */
typedef struct
{
  uint32 reserved;
} lte_plt_ns_dl_data_ind_msg_type;

/*! @brief This structure holds the parameters of CONFIG_UL_WAVEFORM sent to LTE L1
           
*/
typedef struct
{
 uint8 enable_ul_grant_overide;
 uint8 tx_chain;
 uint8 start_rb_index;
 uint8 num_rbs;
 uint8 ul_mcs;
 uint8 nb_index;
} lte_plt_ns_config_ul_waveform_req_data_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_CONFIG_UL_WAVEFORM_REQ
 */
typedef struct
{
  lte_plt_ns_config_ul_waveform_req_data_type ul_waveform_data;
} lte_plt_ns_config_ul_waveform_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_CONFIG_UL_WAVEFORM_RSP
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_config_ul_waveform_cnf_msg_type;

/*! @brief This structure holds the parameters of CONFIG_UL_POWER sent to LTE FW
           
*/
typedef struct
{
 uint8 tx_chain;
 uint8 tx_power_control_mode;
 int16 tx_level_dBm;
} lte_plt_ns_config_ul_power_req_data_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_CONFIG_UL_WAVEFORM_REQ
 */
typedef struct
{
  lte_plt_ns_config_ul_power_req_data_type ul_power_data;
} lte_plt_ns_config_ul_power_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_CONFIG_UL_WAVEFORM_RSP
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_config_ul_power_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_GET_DL_LEVEL_REQ
*/
typedef struct
{
  uint8 rx_chain;
} lte_plt_ns_get_dl_level_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_GET_DL_LEVEL_CNF
 */
typedef struct
{
  errno_enum_type       status;
  int16                 rx_agc; /*! Rx AGC units */
  int16                 rx_power_dBm; /*! Rx Power in dBm units*/
} lte_plt_ns_get_dl_level_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_RESET_DL_BLER_REPORT_REQ
 */
typedef struct
{
  uint8 rx_chain;
} lte_plt_ns_reset_dl_bler_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_RESET_DL_BLER_REPORT_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} lte_plt_ns_reset_dl_bler_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_GET_DL_BLER_REPORT_REQ
 */
typedef struct
{
  uint32 reserved;
} lte_plt_ns_get_dl_bler_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_GET_DL_BLER_REPORT_CNF
 */
typedef struct
{
  errno_enum_type       status;
  uint32                total_blocks_received;
  uint32                total_block_errors; /*NACKS*/
  uint32                throughput;  /*Raw bits per sec*/
} lte_plt_ns_get_dl_bler_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_GET_ALL_CARR_DL_BLER_REPORT_REQ
 */
typedef struct
{
  uint32 reserved;
} lte_plt_ns_get_all_carr_dl_bler_req_msg_type;

/*! @brief This structure defines the data format of the
 *         LTE_PLT_NS_GET_DL_BLER_REPORT_CNF
 */
typedef struct
{
  /* Indicated which carrier id the data belongs to */
  uint8                 carrier_id;
  uint32                total_blocks_received;
  uint32                total_block_errors; /*NACKS*/
  uint32                throughput;  /*Raw bits per sec*/
} lte_plt_ns_get_all_carr_dl_bler_cnf_data_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_GET_ALL_CARR_DL_BLER_REPORT_CNF
 */
typedef struct
{
  errno_enum_type       status;
  /*! Number of valid elements in dl_bler_per_carrier[] */
  uint8 num_carriers;

  lte_plt_ns_get_all_carr_dl_bler_cnf_data_type dl_bler_per_carrier[LTE_PLT_NS_MAX_CARRIERS];
} lte_plt_ns_get_all_carr_dl_bler_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_HANDOVER_REQ
 */
typedef struct
{
  uint8         lte_band;
  lte_earfcn_t  lte_channel;

  /* NW configured customized tx-rx spacing*/
  boolean custom_duplex_spacing_enabled;
} lte_plt_ns_handover_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_HANDOVER_CNF
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_handover_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_TDD_CONFIG_REQ
 */
typedef struct
{
 uint8 subframe_assign;
 uint8 special_sub_fn_patterns;
} lte_plt_ns_tdd_config_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_TDD_CONFIG_RSP
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_tdd_config_cnf_msg_type;

/*! @brief This structure holds the parameters of ENABLE_SCELL_REQ to be sent to LTE L1
*/
typedef struct
{
  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;
  /* E-UTRA Absolute Radio Frequency Channel Number. This goes with the 
   * band to calcualte the exact frequency to tune to 
   */
  lte_earfcn_t         earfcn;

  /*! The cell id this configuration applies to or the source cell in case of
      handover */
  /* Cell ID of the acquired system. 0..503 */
  lte_phy_cell_id_t    cell_id;
} lte_plt_ns_scell_req_data_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_ENABLE_SCELL_REQ
 */
typedef struct
{
  lte_plt_ns_scell_req_data_type dl_data;
} lte_plt_ns_enable_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_ENABLE_SCELL_CNF
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_enable_scell_cnf_msg_type;

/*! @brief This structure holds the parameters of DL MULTI_ENABLE_SCELL_REQ to be sent to LTE L1
*/
typedef struct
{
  uint8                          carr_cfg_mask;
  lte_plt_ns_scell_req_data_type scell_data[LTE_PLT_NS_MAX_CARRIERS-1];
} lte_plt_ns_dl_multi_scell_req_data_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_ENABLE_MULTI_SCELL_REQ
 */
typedef struct
{
  lte_plt_ns_dl_multi_scell_req_data_type dl;
} lte_plt_ns_enable_multi_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_ENABLE_MULTI_SCELL_CNF
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_enable_multi_scell_cnf_msg_type;

/*! @brief This structure holds the UL parameters of ENABLE_SCELL_RX_TX_REQ 
 *         to be sent to LTE L1
 */
typedef struct
{
  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;
  /* E-UTRA Absolute Radio Frequency Channel Number. This goes with the 
   * band to calcualte the exact frequency to tune to 
   */
  lte_earfcn_t         earfcn;

  /*! The cell id this configuration applies to or the source cell in case of
      handover */
  /* Cell ID of the acquired system. 0..503 */
  lte_phy_cell_id_t    cell_id;

  uint16 c_rnti;
  uint8 ns_ca_value;
  uint8 uplink_lcid;
  int8 ue_pwr_limit;
  uint8 uplink_bandwidth;

} lte_plt_ns_ul_scell_req_data_type;

/*! @brief This structure holds the parameters of UL MULTI_ENABLE_SCELL_REQ to be sent to LTE L1
*/
typedef struct
{
  uint8                             carr_cfg_mask;
  lte_plt_ns_ul_scell_req_data_type scell_data [LTE_PLT_NS_MAX_CARRIERS-1];
} lte_plt_ns_ul_multi_scell_req_data_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_ENABLE_MULTI_SCELL_RX_TX_REQ
 */
typedef struct
{
  /* DL */
  lte_plt_ns_dl_multi_scell_req_data_type dl;

  /* UL */
  lte_plt_ns_ul_multi_scell_req_data_type ul;
} lte_plt_ns_enable_multi_scell_rx_tx_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_ENABLE_MULTI_SCELL_RX_TX_CNF
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_enable_multi_scell_rx_tx_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_DISABLE_SCELL_REQ
 */
typedef struct
{
  uint32 reserved;
} lte_plt_ns_disable_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_DISABLE_SCELL_CNF
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_disable_scell_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_DISABLE_MULTI_SCELL_REQ
 */
typedef struct
{
  uint8                dl_carr_cfg_mask;
} lte_plt_ns_disable_multi_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_DISABLE_MULTI_SCELL_CNF
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_disable_multi_scell_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_DISABLE_MULTI_SCELL_RX_TX_REQ
 */
typedef struct
{
  uint8                dl_carr_cfg_mask;
  uint8                ul_carr_cfg_mask;
} lte_plt_ns_disable_multi_scell_rx_tx_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_DISABLE_MULTI_SCELL_RX_TX_CNF
 */
typedef struct
{
  errno_enum_type       status;
} lte_plt_ns_disable_multi_scell_rx_tx_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_ACTIVE_DL_CARRIER_MASK_REQ
 */
typedef struct
{
  uint16 active_dl_carrier_mask;
} lte_plt_ns_active_dl_carrier_mask_req_msg_type;

/*! @brief This structure defines the message format of the
 *         LTE_PLT_NS_ACTIVE_DL_CARRIER_MASK_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} lte_plt_ns_active_dl_carrier_mask_cnf_msg_type;

/* This enum is used to define actions releated to LTE NS request */
typedef enum
{
  LTE_PLT_NS_EXT_API_MSG_START_LTE_MODE_REQ            = 0x1,
  LTE_PLT_NS_EXT_API_MSG_STOP_LTE_MODE_REQ,
  LTE_PLT_NS_EXT_API_MSG_ACQ_REQ,
  LTE_PLT_NS_EXT_API_MSG_IDLE_REQ,
  LTE_PLT_NS_EXT_API_MSG_START_DP_REQ,
  LTE_PLT_NS_EXT_API_MSG_IS_CONN_REQ,
  LTE_PLT_NS_EXT_API_MSG_CONFIG_UL_WAVEFORM_REQ,
  LTE_PLT_NS_EXT_API_MSG_CONFIG_UL_POWER_REQ,
  LTE_PLT_NS_EXT_API_MSG_GET_DL_LEVEL_REQ,
  LTE_PLT_NS_EXT_API_MSG_RESET_DL_BLER_REPORT_REQ,
  LTE_PLT_NS_EXT_API_MSG_GET_DL_BLER_REPORT_REQ,
  LTE_PLT_NS_EXT_API_MSG_HANDOVER_REQ,
  LTE_PLT_NS_EXT_API_MSG_TDD_CONFIG_REQ,
  LTE_PLT_NS_EXT_API_MSG_ENABLE_SCELL_REQ,
  LTE_PLT_NS_EXT_API_MSG_DISABLE_SCELL_REQ,
  LTE_PLT_NS_EXT_API_MSG_GET_ALL_CARR_DL_BLER_REQ,
  LTE_PLT_NS_EXT_API_MSG_ENABLE_MULTI_SCELL_REQ,
  LTE_PLT_NS_EXT_API_MSG_DISABLE_MULTI_SCELL_REQ,
  LTE_PLT_NS_EXT_API_MSG_ENABLE_MULTI_SCELL_RX_TX_REQ,
  LTE_PLT_NS_EXT_API_MSG_DISABLE_MULTI_SCELL_RX_TX_REQ,
  LTE_PLT_NS_EXT_API_MSG_REQ_MAX                        = 0xFF
} lte_plt_ns_ext_api_req_msg_e;

/* This enum is used to define actions releated to LTE NS confirmation */
typedef enum
{
  LTE_PLT_NS_EXT_API_MSG_START_LTE_MODE_CNF            = 0x1,
  LTE_PLT_NS_EXT_API_MSG_STOP_LTE_MODE_CNF,
  LTE_PLT_NS_EXT_API_MSG_ACQ_CNF,
  LTE_PLT_NS_EXT_API_MSG_IDLE_CNF,
  LTE_PLT_NS_EXT_API_MSG_START_DP_CNF,
  LTE_PLT_NS_EXT_API_MSG_IS_CONN_CNF,
  LTE_PLT_NS_EXT_API_MSG_CONFIG_UL_WAVEFORM_CNF,
  LTE_PLT_NS_EXT_API_MSG_CONFIG_UL_POWER_CNF,
  LTE_PLT_NS_EXT_API_MSG_GET_DL_LEVEL_CNF,
  LTE_PLT_NS_EXT_API_MSG_RESET_DL_BLER_REPORT_CNF,
  LTE_PLT_NS_EXT_API_MSG_GET_DL_BLER_REPORT_CNF,
  LTE_PLT_NS_EXT_API_MSG_HANDOVER_CNF,
  LTE_PLT_NS_EXT_API_MSG_TDD_CONFIG_CNF,
  LTE_PLT_NS_EXT_API_MSG_ENABLE_SCELL_CNF,
  LTE_PLT_NS_EXT_API_MSG_DISABLE_SCELL_CNF,
  LTE_PLT_NS_EXT_API_MSG_GET_ALL_CARR_DL_BLER_CNF,
  LTE_PLT_NS_EXT_API_MSG_ENABLE_MULTI_SCELL_CNF,
  LTE_PLT_NS_EXT_API_MSG_DISABLE_MULTI_SCELL_CNF,
  LTE_PLT_NS_EXT_API_MSG_ENABLE_MULTI_SCELL_RX_TX_CNF,
  LTE_PLT_NS_EXT_API_MSG_DISABLE_MULTI_SCELL_RX_TX_CNF,
  LTE_PLT_NS_EXT_API_MSG_CNF_MAX                        = 0xFF
} lte_plt_ns_ext_api_cnf_msg_e;

/*! @brief Union of all msgs for LTE NS request
*/
typedef union
{
  /* FTM LTE Non-signaling messages */
  lte_plt_ns_start_mode_req_msg_type                 start_mode_req_msg;
  lte_plt_ns_stop_mode_req_msg_type                  stop_mode_req_msg;
  lte_plt_ns_acq_req_msg_type                        acq_req_msg;
  lte_plt_ns_start_dp_req_msg_type                   start_dp_req_msg;
  lte_plt_ns_idle_req_msg_type                       idle_req_msg;
//  lte_plt_ns_is_conn_req_msg_type                    is_conn_req_msg;
  lte_plt_ns_config_ul_waveform_req_msg_type         config_ul_waveform_req_msg;
  lte_plt_ns_config_ul_power_req_msg_type            config_ul_power_req_msg;
  lte_plt_ns_get_dl_level_req_msg_type               get_dl_level_req_msg;
  lte_plt_ns_reset_dl_bler_req_msg_type              reset_dl_bler_req_msg;
  lte_plt_ns_get_dl_bler_req_msg_type                get_dl_bler_req_msg;
  lte_plt_ns_handover_req_msg_type                   handover_req_msg;
  lte_plt_ns_tdd_config_req_msg_type                 tdd_config_req_msg;
  lte_plt_ns_enable_scell_req_msg_type               enable_scell_req_msg;
  lte_plt_ns_disable_scell_req_msg_type              disable_scell_req_msg;
  lte_plt_ns_enable_multi_scell_req_msg_type         enable_multi_scell_req_msg;
  lte_plt_ns_enable_multi_scell_rx_tx_req_msg_type   enable_multi_scell_rx_tx_req_msg;
  lte_plt_ns_disable_multi_scell_req_msg_type        disable_multi_scell_req_msg;
  lte_plt_ns_disable_multi_scell_rx_tx_req_msg_type  disable_multi_scell_rx_tx_req_msg;
  lte_plt_ns_active_dl_carrier_mask_req_msg_type     active_dl_carrier_mask_req_msg;
  lte_plt_ns_get_all_carr_dl_bler_req_msg_type       get_all_carr_dl_bler_req_msg;

} lte_plt_ns_ext_api_req_msg_u;

/*! @brief Union of all msgs for LTE NS confirmation
*/
typedef union
{
  lte_plt_ns_start_mode_cnf_msg_type                start_mode_cnf_msg;
  lte_plt_ns_stop_mode_cnf_msg_type                 stop_mode_cnf_msg;
  lte_plt_ns_acq_cnf_msg_type                       acq_cnf_msg;
  lte_plt_ns_start_dp_cnf_msg_type                  start_dp_cnf_msg;
  lte_plt_ns_idle_cnf_msg_type                      idle_cnf_msg;
//  lte_plt_ns_is_conn_cnf_msg_type                   is_conn_cnf_msg;
  lte_plt_ns_config_ul_waveform_cnf_msg_type        config_ul_waveform_cnf_msg;
  lte_plt_ns_config_ul_power_cnf_msg_type           config_ul_power_cnf_msg;
  lte_plt_ns_get_dl_level_cnf_msg_type              get_dl_level_cnf_msg;
  lte_plt_ns_reset_dl_bler_cnf_msg_type             reset_dl_bler_cnf_msg;
  lte_plt_ns_get_dl_bler_cnf_msg_type               get_dl_bler_cnf_msg;
  lte_plt_ns_handover_cnf_msg_type                  handover_cnf_msg;
  lte_plt_ns_tdd_config_cnf_msg_type                tdd_config_cnf_msg;
  lte_plt_ns_enable_scell_cnf_msg_type              enable_scell_cnf_msg;
  lte_plt_ns_disable_scell_cnf_msg_type             disable_scell_cnf_msg;
  lte_plt_ns_enable_multi_scell_cnf_msg_type        enable_multi_scell_cnf_msg;
  lte_plt_ns_enable_multi_scell_rx_tx_cnf_msg_type  enable_multi_scell_rx_tx_cnf_msg;
  lte_plt_ns_disable_multi_scell_cnf_msg_type       disable_multi_scell_cnf_msg;
  lte_plt_ns_disable_multi_scell_rx_tx_cnf_msg_type disable_multi_scell_rx_tx_cnf_msg;
  lte_plt_ns_active_dl_carrier_mask_cnf_msg_type    active_dl_carrier_mask_cnf_msg;
  lte_plt_ns_get_all_carr_dl_bler_cnf_msg_type      get_all_carr_dl_bler_cnf_msg;

} lte_plt_ns_ext_api_cnf_msg_u;



/*! @brief External API callback function for response to exteraml API request
  
*/
typedef errno_enum_type (*lte_plt_ns_ext_api_cb_func_type)( lte_plt_ns_ext_api_cnf_msg_e msg_id );

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
 
  FUNCTION:  lte_plt_ns_register_ext_api_cb 

===========================================================================*/
/*!
  @brief This function registers callback function and data structure for
  the request/response of Non-Signaling external API request 

  @param Pointer to the External API callback and req/cnf data structure  
  
  @return TRUE if suceeded
          FALSE if others
*/
/*=========================================================================*/
boolean lte_plt_ns_register_ext_api_cb ( lte_plt_ns_ext_api_cb_func_type cb,
                                         lte_plt_ns_ext_api_req_msg_u    *req_msg_ptr,
                                         lte_plt_ns_ext_api_cnf_msg_u    *cnf_msg_ptr);

/*===========================================================================
 
  FUNCTION:  lte_plt_ns_ext_api_processor 

===========================================================================*/
/*!
  @brief This function handles Non-signaling request from other tasks
  (RF FTM, Off target) 

  @param Message ID 
  
  @return E_SUCCESS if external api process has sucessfully initaited
          others if failed 
*/
/*=========================================================================*/
errno_enum_type lte_plt_ns_ext_api_processor ( lte_plt_ns_ext_api_req_msg_e msg_id);

#endif /*  LTE_PLT_NS_MSG_H */

