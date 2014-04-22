#ifndef NB1_CPHY_RSSI_MSG_H
#define NB1_CPHY_RSSI_MSG_H
/*!
  @file nb1_cphy_rssi_msg.h

  @brief
   Interface to the NB1 L1 RSSI message interface.

*/

/*==============================================================================

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_cphy_rssi_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------


==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <msgr_nb1.h>
#include "nb1_cphy_msg_ids.h"
#include "nb1_as.h"
#include <cmapi.h>
#include "nb1_cphy_msg.h"
#include "atomic_ops.h"

/*==============================================================================

                               MACROS

==============================================================================*/

/*! The periodicity which NB1_CPHY_RSSI_IND is sent in seconds */
#define NB1_CPHY_RSSI_IND_PERIOCITY_IN_SECONDS 1

/*! The invalid signal info value
*/
#define NB1_API_SIGNAL_INFO_INVALID_VALUE   0x80000000

/*! The invalid band and channel value
*/
#define NB1_API_BAND_INVALID_VALUE 0xFFFFFFFF
#define NB1_API_CHANNEL_INVALID_VALUE 0xFFFFFFFF

/*! Macro for creating a bitmask by shifting '1' left by a
    Specified number indicated by 'val'.
*/
#define NB1_API_BM( val ) ( 1<< ((int)(val)) )

/*
 * Number of layers
 */
#define NB1_API_MAX_NUM_LAYER 2

/*define it if not defined in CMAPI*/
#ifndef CMAPI_LTE_ML1_MAX_SCC
#define CMAPI_LTE_ML1_MAX_SCC 2
#endif

/*==============================================================================
                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*
 * Enums for modulation
 */
typedef enum {
   NB1_API_MODULATION_BPSK = 0,
   NB1_API_MODULATION_QPSK,
   NB1_API_MODULATION_16QAM,
   NB1_API_MODULATION_64QAM,
   NB1_API_MODULATION_UNKOWN
}nb1_api_modulation_e_type;

/*
 * Types for modulation
 */
typedef  struct
{
	uint8 num_layer;
	nb1_api_modulation_e_type modulation[NB1_API_MAX_NUM_LAYER];
} nb1_api_mod_s;


/* Enumeration of RX chains supported
*/
typedef enum nb1_cmapi_rx_chain_e{
  NB1_API_RX_CHAIN_NUM_0 = 0,
  NB1_API_RX_CHAIN_NUM_1,
  NB1_API_RX_CHAIN_NUM_MAX
}nb1_api_rx_chain_e_type;

/* Enumeration of RX chain diversity info
*/
typedef enum nb1_cmapi_rx_diversity_bit_mask_e{

  /*! Signal info are not available on all RX chains
  */
  NB1_API_RX_CHAIN_UNAVAIL = 0,

  /*! Signal info on NB1_API_RX_CHAIN_0 is available
  */
  NB1_API_RX_CHAIN_0_AVAIL = NB1_API_BM(NB1_API_RX_CHAIN_NUM_0),

  /*! Signal info on NB1_API_RX_CHAIN_NUM_1 is available
  */
  NB1_API_RX_CHAIN_1_AVAIL = NB1_API_BM(NB1_API_RX_CHAIN_NUM_1),

  /*! Signal info on all RX chains are available
  */
  NB1_API_RX_CHAIN_ALL_AVAIL = NB1_API_BM(NB1_API_RX_CHAIN_NUM_0) |
                               NB1_API_BM(NB1_API_RX_CHAIN_NUM_1)

}nb1_api_rx_diversity_bit_mask_e;

typedef struct nb1_api_rx_chain_info_s{

  /*! Whether RX is tuned to a channel. If radio is tuned,
   instantenuous values will be set for below signal info
   fields.If radio is not tuned, delayed or invalid values
   will be set depending on each technology.
  */
  boolean             is_radio_tuned;

  /*! RX power value in 1/10 dbm resolution */
  int32               rssi;

  /*! Rsrq in negtive 1/10 dbm.
  */
  int32               rsrq;

  /*! Current Reference Signal Received Power in 1/10 dbm.
     Available for NB1.
  */
  int32               rsrp;

  /*! Current phase in 1/100 degrees.  Range from 0.00 to 360.00 */
  uint32              phase;

}nb1_api_rx_chain_info_s_type;

/*! Structure of RX power related info
*/
typedef struct nb1_api_rx_pwr_info_s{

  /*! Flag identifying if the contents of the message are valid.  valid will
  be set to TRUE when the measurements are valid and identify the current
  state within L1.  When FALSE the contents of the rest of the message
  is invalid and should not be used */
  boolean valid;

  /*! Diversity indicator to show which RX chain has valid
     signal info. Used to indicate RX power available info
     of below 2 fields
  */
  nb1_api_rx_diversity_bit_mask_e   rx_diversity_indicator;

  /*! Rx signal info
  */
  nb1_api_rx_chain_info_s_type      rx_pwr_chain[NB1_API_RX_CHAIN_NUM_MAX];

}nb1_api_rx_pwr_info_s_type;

/*! Structure of TX power related info
*/
typedef struct nb1_api_tx_pwr_info_s{

  /*! TX power value in 1/10 dbm
    0x8000(-32768) stands for invalid *
  */
  int32               tx_pwr;

  /*! PA Gain State */
  uint8               pa_gain_state;
}nb1_api_tx_pwr_info_s_type;

/*! Structure for Physical Cell Id info
*/
typedef struct
{

  /*! Flag to indicate validity of the information */
  boolean valid;

  /*! Physical Cell ID */
  uint16  value;

}nb1_api_serv_cell_id_info_s;

/* Structure holding the SINR(dB) info
*/

typedef struct
{

  /*! Flag to indicate validity of the information */
  boolean valid;

  /*! Logarithmic value of SINR */
  int16   value;

}nb1_api_serv_cell_sinr_info_s;
/*!
** signal information
*/
typedef struct nb1_api_signal_info_s{

  /*! Msgr Header for the CM_RSSI_CNF */
  msgr_hdr_struct_type msgr_hdr;

  /*! Active band of current system */
  int32                band;

  /*! EARFCN. Active channel of current system */
  int32                channel;

  /*! bw of pcc */
  nb1_bandwidth_idx_e          pcc_dl_bw;

  /*! Rx Power info of the specified system mode */
  nb1_api_rx_pwr_info_s_type   rx_power;

  /*! Tx Power info of the specified system mode */
  nb1_api_tx_pwr_info_s_type   tx_power;

  /*! Physical Cell Id info of the serving cell */
  nb1_api_serv_cell_id_info_s  phy_cellid;

  /*! SINR info for serving cell */
  nb1_api_serv_cell_sinr_info_s  log_sinr10xdb;

    /*! Active band of current system */
  int32                scc_band[CMAPI_LTE_ML1_MAX_SCC];

  /*! EARFCN. Active channel of current system */
  int32                scc_channel[CMAPI_LTE_ML1_MAX_SCC];

  /*! Rx Power info of the specified system mode */
  nb1_api_rx_pwr_info_s_type   scc_rx_power[CMAPI_LTE_ML1_MAX_SCC];

    /*! Physical Cell Id info of the serving cell */
  nb1_api_serv_cell_id_info_s  scc_phy_cellid[CMAPI_LTE_ML1_MAX_SCC];

  /*! SINR info for serving cell */
  nb1_api_serv_cell_sinr_info_s  scc_log_sinr10xdb[CMAPI_LTE_ML1_MAX_SCC];

    /*! cell state of scc */
  uint8                          scc_state[CMAPI_LTE_ML1_MAX_SCC];

  /*! bw of scc */
  nb1_bandwidth_idx_e            scc_dl_bw[CMAPI_LTE_ML1_MAX_SCC];

  /*! Flag identifying if the sinr values are valid.  valid will
    be set to TRUE when the values are valid and identify the current
    state within L1.  When FALSE the linear_sinr and  log_sinr_10xdB
    are invalid and should not be used */
  boolean sinr_valid;

  /*! Log sinr estimate - 10xdB values which are from -200 to 300
    are mapped to range 0 - 250. -20 db maps to 0, -19.8 db maps
    to 1, -19.6 maps to 2 ....-30 db maps to 250*/
  uint8   log_sinr_10xdB;

  /* UL modulation */
  nb1_api_mod_s  ul_modulation;

  /* DL modulation */
  nb1_api_mod_s  dl_modulation;

}nb1_api_signal_info_s_type;

/*!
  @brief
  This message indicates to the upper layers the current ngr signal quality at L1
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

  /*! Structure to indicate the neighbor mesasurements */
  cmapi_lte_ngbr_ind_s *lte_ngbr_ind;
} nb1_ml1_common_cm_nbr_rssi_ind_s;

/*!
  @brief
  This message indicates to the upper layers the current ngr signal quality at L1
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;
} nb1_ml1_common_cm_nbr_rssi_cnf_s;


/*!
  @brief
  This message indicates to upper layers the current signal quality at L1.

  This message will be sent to CM every time the API nb1_ml1_common_cm_rssi_ind_req()
  invoked by the CM layer
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

  /*! Flag identifying if the contents of the message are valid.  valid will
    be set to TRUE when the measurements are valid and identify the current
    state within L1.  When FALSE the contents of the rest of the message
    is invalid and should not be used */
  boolean valid;

  /*! Physical cell ID of serving cell*/
  nb1_phy_cell_id_t    pci;

   /*! Earfcn of serving cell*/
  nb1_earfcn_t         freq;

  /*! Band on which serving cell is located */
  sys_sband_lte_e_type     band;

  /*! Downlink bandwidth */
  nb1_bandwidth_e          dl_bandwidth;
  /*! Number of Tx antennas detected; 0 indicates MIB invalid or unknown */
  uint8                    num_antennas;

  /*! The current Reference Signal Receive Quality as measured by L1.
   The quantities are in dB.  Range: -20..-3 dB. */
  int16 rsrq;

  /*! The current Reference Signal Receive Power in dBm as measured by L1.
      Range: -44..-140
   */
  int16 rsrp;

  /*! The current Received Signal Strength Indication as measured by L1.
   Values are in dBm with Range: -120..0 */
  int8 rssi;

  /*! Flag identifying if the sinr values are valid.  valid will
    be set to TRUE when the values are valid and identify the current
    state within L1.  When FALSE the linear_sinr and  log_sinr_10xdB
    are invalid and should not be used */
  boolean sinr_valid;

  /*! Log sinr estimate - 10xdB values which are from -200 to 300
    are mapped to range 0 - 250. -20 db maps to 0, -19.8 db maps
    to 1, -19.6 maps to 2 ....-30 db maps to 250*/
  uint8   log_sinr_10xdB;

  /*! Last Highest Tx power value for all channels
      Range is -100 to +100 dbm.
      0x8000(-32768) stands for invalid */
  int16   tx_power;
} nb1_cphy_rssi_cm_s;

/*!
  @brief
  This message indicates to upper layers the current signal quality at L1.

  This message will be sent periodically every NB1_CPHY_RSSI_IND_PERIOCITY
  seconds whenever NB1 L1 is active (following NB1_CPHY_START_REQ and prior to
  NB1_CPHY_STOP_REQ).
*/
typedef struct
{
  msgr_hdr_struct_type msgr_hdr;

  /*! Flag identifying if the contents of the message are valid.  valid will
    be set to TRUE when the measurements are valid and identify the current
    state within L1.  When FALSE the contents of the rest of the message
    is invalid and should not be used */
  boolean valid;

  /*! Physical cell ID of serving cell*/
  nb1_phy_cell_id_t    pci;

   /*! Earfcn of serving cell*/
  nb1_earfcn_t         freq;

  /*! Band on which serving cell is located */
  sys_sband_lte_e_type     band;

  /*! Downlink bandwidth */
  nb1_bandwidth_e          dl_bandwidth;
  /*! Number of Tx antennas detected; 0 indicates MIB invalid or unknown */
  uint8                    num_antennas;

  /*! The current Reference Signal Receive Quality as measured by L1.
   The quantities are in dB.  Range: -20..-3 dB. */
  int16 rsrq;

  /*! The current Reference Signal Receive Power in dBm as measured by L1.
      Range: -44..-140
   */
  int16 rsrp;

  /*! The current Received Signal Strength Indication as measured by L1.
   Values are in dBm with Range: -120..0 */
  int8 rssi;

  /*! Flag identifying if the sinr values are valid.  valid will
    be set to TRUE when the values are valid and identify the current
    state within L1.  When FALSE the linear_sinr and  log_sinr_10xdB
    are invalid and should not be used */
  boolean sinr_valid;

  /*! Log sinr estimate - 10xdB values which are from -200 to 300
    are mapped to range 0 - 250. -20 db maps to 0, -19.8 db maps
    to 1, -19.6 maps to 2 ....-30 db maps to 250*/
  uint8   log_sinr_10xdB;

  /*! Last Highest Tx power value for all channels
      Range is -100 to +100 dbm.
      0x8000(-32768) stands for invalid */
  int16   tx_power;
} nb1_cphy_rssi_ind_s;

/*! @brief
  This message is used for configuring the nb1 signal reporting config
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                       msgr_hdr;
  /*! reporting rate 1s-5s */
  cmapi_lte_signal_reporting_rate_e          reporting_rate;
  /*! averaging period 1s - 5s */
  cmapi_lte_signal_averaging_period_e        averaging_period;
} nb1_cphy_signal_report_cfg_ind_s;


/*! @brief
  Structure for storing rnti information
*/
typedef struct {
  uint16   rnti_enable_mask;     ///< lte_LL1_rnti_type_enum Mask containing info on valid RNTI's
  uint16   c_rnti;               ///< connecgted state, common and ue specific search space
  uint16   sps_c_rnti;           ///< connecgted state, semi-persistent assignment, common and ue specific search space
  uint16   p_rnti;               ///< paging in RRC_CONNECTED  or IDLE
  uint16   ra_rnti;              ///< Random access
  uint16   t_c_rnti;             ///< temp c_rnti for message 4 when contention is not resolved
  uint16   si_rnti;              ///< system info, connected and idle
  uint16   tpc_pusch_rnti;       ///< group power control for PUSCH (persistent assignment), connected state
  uint16   tpc_pucch_rnti;       ///< group power control for PUCCH, connected state
  uint16   mbms_rnti;            ///< MBMS RNTI for cell specific MBMS on PDSCH, connected state
} nb1_cphy_rnti_s_type;


/*!
  @brief
  Captures the current ML1 MGR configuration needed for CMAPI
  */
typedef struct
{
  /*! Msgr Header for the CM_ML1_MGR_INFO_CNF */
  msgr_hdr_struct_type msgr_hdr;

  /*! Structure to store pdcch cfg */
  nb1_cphy_rnti_s_type  rnti_info;

  /*! idle drx info */
  nb1_cphy_idle_drx_param_s idle_drx_cfg;

  /*! number of Tx Antenna on eNB */
  uint8 enb_num_tx_antenna;

  /*! wide band cqi info */
  uint16 cqi_wb;

} nb1_ml1_mgr_cm_ml1_mgr_info_s;


/*!
  @brief
  Captures the current ML1 GM configuration needed for CMAPI
  */
typedef struct
{
  /*! Msgr Header for the CM_ML1_GM_INFO_CNF */
  msgr_hdr_struct_type msgr_hdr;

  /*! Structure to store cdrx cfg */
  nb1_cphy_drx_cfg_dedicated_param_s cdrx_cfg;

} nb1_ml1_gm_cm_ml1_gm_info_s;


/*!
  @brief
  Bit field structure to store cell and tx power info
  */
typedef struct
{
  /*! first word*/
  /*! Active channel of current system */
  uint32                channel:32;

  /*! Second word*/
  /*! Active band of current system */
  uint32                band:8;

  /*! Reserved */
  uint32                reserved:24;

  /*! Third word*/
  /*! Physical Cell Id info of the serving cell */
  uint32                phy_cell_id:16;

  /*! Physical Cell Id info of the serving cell */
  uint32                tx_power:16;
}nb1_ml1_common_rssi_cmapi_tx_power_s;



/*!
  @brief
  Union to access the bit field as a 64 bit variable
  */
typedef union
{
  nb1_ml1_common_rssi_cmapi_tx_power_s    tx_data;

  atomic64_plain_word_t                   tx_data_atomic_plain;

} nb1_ml1_common_rssi_cmapi_tx_power_u;

/*!
  @brief
  Structure for cmapi tx info into which information will be pushed to
  */
typedef struct
{
  atomic64_word_t                         tx_data;
}nb1_ml1_common_rssi_cmapi_tx_power_atomic_s;

/*!
  brief
  CA SCell state
*/
typedef enum
{
  NB1_CPHY_SCELL_STATE_DECONFIGURED=0,
  NB1_CPHY_SCELL_STATE_CONFIGURED_DEACTIVATED,
  NB1_CPHY_SCELL_STATE_CONFIGURED_ACTIVATED,
  NB1_CPHY_SCELL_STATE_MAX,

} nb1_cphy_scell_state_e;

/*!
  brief
  Cell info
  */
typedef struct
{
  /*! Physical cell ID*/
  nb1_phy_cell_id_t      pci;

   /*! Earfcn*/
  nb1_earfcn_t           freq;

  /*! DL BW*/
  nb1_bandwidth_e        dl_bandwidth;

  /*! Band*/
  sys_sband_lte_e_type   sys_band;

  /*! State of the SCell, Not applicable for Pcell */
  nb1_cphy_scell_state_e scell_state;

} nb1_cphy_cmapi_cell_info_s;

/*!
  @brief
  Indicates a CA event has occurred
  */
typedef struct
{
  /*! Msgr Header for the NB1_CPHY_CA_EVENT_IND */
  msgr_hdr_struct_type   msgr_hdr;

  /*Pcell info*/
  nb1_cphy_cmapi_cell_info_s pcell_info;

  /*Scell info*/
  nb1_cphy_cmapi_cell_info_s scell_info;

} nb1_cphy_ca_event_ind_s;

enum
{

  MSGR_DEFINE_UMID(NB1, CPHY, IND, RSSI,
                   NB1_CPHY_IND_RSSI_ID, nb1_cphy_rssi_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, SIGNAL_REPORT_CFG,
                   NB1_CPHY_IND_SIGNAL_REPORT_CFG_ID, nb1_cphy_signal_report_cfg_ind_s),

  MSGR_DEFINE_UMID(NB1, CPHY, IND, CA_EVENT,
                   NB1_CPHY_IND_CA_EVENT_ID, nb1_cphy_ca_event_ind_s),
};

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  nb1_cphy_signal_report_cfg_req

===========================================================================*/
/*!
    @brief
    api call used by CM to send the signal reporting config to ML1.

    @return
    none.

*/
/*=========================================================================*/

void nb1_cphy_signal_report_cfg_req
(
   cmapi_lte_signal_reporting_rate_e          reporting_rate,
  /*! averaging period 1s - 5s */
  cmapi_lte_signal_averaging_period_e        averaging_period
);


/*===========================================================================

  FUNCTION:  nb1_ml1_common_rssi_cmapi_update_signal_ptr

===========================================================================*/
/*!
    @brief
    Updates the CMAPI global variable memory to store tx power info

    @return
    None

*/
/*===========================================================================*/
void nb1_ml1_common_rssi_cmapi_update_signal_ptr
(
  nb1_ml1_common_rssi_cmapi_tx_power_atomic_s *tx_ptr
);

#endif /* NB1_CPHY_RSSI_MSG_H */


