/*!
  @file
  nb1_mac_log_ext.c

  @brief
   This header file contains the various defines, enums, and other data
  structure types which are necessary for logging binary packets via DIAG.

  @author
  tahmed

*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/19   ss      CR 2387711 : Non-Anchor carrier info in B061 and B062 log packets
01/27/17   ss      CR 1115360 : NB1 MAC Re/architecture

===========================================================================*/
#ifndef NB1_MAC_LOG_EXT_H
#define NB1_MAC_LOG_EXT_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include "nb1_as.h"
#include "nb1_mac_rrc.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief
    Length of UE ID in log packets
*/
#define NB1_MAC_LOG_UE_ID_LEN 6

/*! @brief
    Maximum length of RACH MSG3 MAC header in log packets
*/
#define NB1_MAC_LOG_MSG3_RACH_MAC_HDR_MAX_LEN 10

#define NB1_MAC_LOG_MAX_CE_LEVEL NB1_CPHY_CE_LEVEL_MAX

/*! @brief Size of RSRP thresholds PRACH Info List Size */
#define NB1_MAC_LOG_RSRP_THRESH_PRACH_LIST_SIZE NB1_MAC_RSRP_THRESH_PRACH_LIST_SIZE


#ifdef _WIN32
#pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*! @brief Structure for Config Type Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint8              cfg_reason;  /*!< NORMAL, HO, RELEASE */
  uint8              cfg_bitmask; /*!< DL, UL, RACH, LC INFO Cfg */
} nb1_mac_log_sub_pkt_cfg_type_s;

/*! @brief Structure for DL Config Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint16  ta_timer; /*!< TA timer value. Note that 0xFFFF is invalid timer value*/
} nb1_mac_log_sub_pkt_dl_cfg_s;

/*! @brief Structure for UL Config Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint32   bsr_timer;               /*!< SR periodicity timer in ms. 0xFFFFFFFF is invalid/Not present number  */
  uint32   retxed_bsr_timer;        /*!< retxed BSR timer in ms. 0xFFFFFFFF is invalid/Not present number   */
  uint32   lc_sr_prohibit_timer;    /*!< Logical Channel SR prohibit timer in ms. 0xFFFFFFFF is invalid/Not present number   */
} nb1_mac_log_sub_pkt_ul_cfg_s;

/*! @brief Structure for RACH Config Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint8               first_subcarrier;                   /*!< First Subcarrier, range (0..63) */
  uint8               multi_start_subcarrier;             /*!< Multitone Subcarrier, range (0..63) */
  uint8               last_subcarrier;                    /*!< Last Subcarrier, range (0..63) */
  uint8               max_preamble_tx_attempt_per_ce;     /*!< Max number of premable transmission attempts per CE */
  uint32              contention_resol_tmr;               /*!< Contention timer in ms values: 80, 100, 120, 160, 200, 240.0xFFFFFFFF is invalid/Not present number  */
} nb1_mac_log_sub_pkt_cell_prach_param_ce_s;

typedef PACK(struct)
{

  uint8               prach_param_ce_list_size;           /*!< Size of prach_param_ce_list */
  nb1_mac_log_sub_pkt_cell_prach_param_ce_s prach_param_ce_list[NB1_MAC_LOG_MAX_CE_LEVEL];  /*!< PRACH parameter list for enhanced coverage */
} nb1_mac_log_sub_pkt_anchor_prach_param_ce_s;

typedef PACK(struct)
{
  int16               preamble_init_power;           /*!< Initial Power Level in dB */
  uint8               power_ramping_step;            /*!< Step power */
  int16               delta_preamble_msg3;           /*!< delta preable message 3 */
  uint8               rsrp_thresh_prach_list_size;        /*!< Size of RSRP threshold list size */
  uint8               rsrp_thresh_prach_list[NB1_MAC_LOG_RSRP_THRESH_PRACH_LIST_SIZE];      /*!< UE to select PRACH resource set. Up to 3 RSRP threshold */
  uint8               prach_param_ce_list_size;           /*!< Size of prach_param_ce_list */
  nb1_mac_log_sub_pkt_cell_prach_param_ce_s prach_param_ce_list[NB1_MAC_LOG_MAX_CE_LEVEL];  /*!< PRACH parameter list for enhanced coverage */
  uint16              preamble_trans_max_ce;              /*!< MAX number of preamble transmissions in enhanced coverage */
  uint16              anchor_rach_probability[NB1_MAC_LOG_MAX_CE_LEVEL];
  uint8               non_anchor;
  nb1_mac_log_sub_pkt_anchor_prach_param_ce_s carrier_param[NB1_CPHY_MAX_NON_ANCHOR_CARRIER];
} nb1_mac_log_sub_pkt_rach_cfg_s;


/*! @brief Structure for LC Info in LC Config Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint8               lc_id;              /*!< Logical channel ID of each channel */
  uint8               priority;           /*!< Priority Level */
  uint8               lc_group;           /*!< BSR reporting LC group */
  uint8               enable_lc_sr_prohibit; /*!< LC SR prohibit timer enabled for this logical channel*/
} nb1_mac_log_sub_pkt_lc_cfg_info_s;

/*! @brief Structure for LC Config Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint8               num_deleted_lc;                           /*!< number of LC to be deleted */
  uint8               deleted_lc[NB1_MAX_LC];                   /*!< Deleted LC IDs */
  uint8               num_addmod_lc;                            /*!< number of added or modified logical channel*/
  nb1_mac_log_sub_pkt_lc_cfg_info_s    addmod_lc[NB1_MAX_LC];   /*!< Added/Modified LC Info */
} nb1_mac_log_sub_pkt_lc_cfg_s;

/*! @brief Structure for eMBMS Info in eMBMS Config Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint8               area_id;          /*!< area id */
  uint8               pmch_id;          /*!< pmch_id */
  uint8               lc_id;            /*!< lc_id */
} nb1_mac_log_sub_pkt_embms_cfg_info_s;

/*! @brief Structure for eMBMS Config Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint8               num_active_embms_lc;
  nb1_mac_log_sub_pkt_embms_cfg_info_s  active_embms_lc_info[NB1_MAX_EMBMS_LC];
} nb1_mac_log_sub_pkt_embms_cfg_s;


/*! @brief Structure for RACH reason Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint8               rach_reason;           /*!< cause due to which RACH was triggered */
  /*! The UE_ID that will be matched in case of CONTENTION_BASED RACH Procedure.  */
  uint8               matching_id[NB1_MAC_LOG_UE_ID_LEN];
  uint8               rach_contention;       /*!< Indicates if this is a contention free or contention based RACH procedure */
  uint8               preamble;              /*!< Preamble chosen in case of contention free */
  uint8               preamble_index_mask;   /*!< Preamble RA index mask in case of contention free*/
  uint8               msg3_size;             /*!< Size of MSG3 based on which the group would be chosen */
  uint8               ce_level;              /*!< Refers to the CE Level by MAC was picked */
  uint8               radio_condn;           /*!< Gives the value of the pathloss as a result of call to the L1 API */
  uint16              crnti;                 /*!< If the UE is in connected mode then this indicates the CRNTI of the UE */
  uint8               carrier_id;               /*!< Carrier id indicated by MAC/ */
} nb1_mac_log_sub_pkt_rach_reason_s;

/*! @brief Structure for Message 1 in RACH attempt Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint8               preamble_index;           /*!< Index is in the range 0..63 */
  uint8               preamble_index_mask;      /*!< Range: 0-15. 0xFF means invalid.*/
  int16               preamble_power;           /*!< Power level */
  uint8               ce_level;                 /*!< CE Level indicated by MAC/ */
  uint8               carrier_id;               /*!< Carrier id indicated by MAC/ */
} nb1_mac_log_rach_mesg1_s;

/*! @brief Structure for Message 2 in RACH attempt Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint16               backoff_val;      /*!< index into the backoff_table */
  uint8                result;           /*!< TRUE indicates that the RAPID received in the DL matched with the one the UE sent. */
  uint16               tcrnti;           /*!< Temp CRNTI received in MSG2  */
  uint16               ta_value;         /*!< Timing Adjustment value received */
} nb1_mac_log_rach_mesg2_s;

/*! @brief Structure for Message 3 in RACH attempt Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint32   msg3_grant_raw;  /*!< raw grant data byte for MSG3. From bit0 to bit20*/
  uint16   msg3_grant;      /*!< Grant in bytes */
  uint8    harq_id;         /*!< Harq ID where the data would be transmitted from */
  /*! Maximum of up to 10 bytes of MCE and MAC sub-headers */
  uint8    mac_pdu[NB1_MAC_LOG_MSG3_RACH_MAC_HDR_MAX_LEN];
} nb1_mac_log_rach_mesg3_s;

/*! @brief Structure for RACH attempt Log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint8  retx_counter;           /*!< RACH attempt number */
  uint8  rach_result;            /*!< RACH attempt result*/
  uint8  contention_procedure;   /*!< Indicates if this is a contention free or contention based RACH procedure */
  uint8  rach_msg_bmask;         /*!< Bitmask to indicate which of MSG 1, 2, 3 are actually present*/
  nb1_mac_log_rach_mesg1_s  rach_msg1;  /*!< Message 1 */
  nb1_mac_log_rach_mesg2_s  rach_msg2;  /*!< Message 2 */
  nb1_mac_log_rach_mesg3_s  rach_msg3;  /*!< Message 3 */
} nb1_mac_log_sub_pkt_rach_attempt_s;

/*! @brief Structure for UL TX statistics log sub packet "scratchpad"
*/
typedef PACK(struct)
{
  uint8    num_samples;        /*!< Number of samples collected */
  uint8    num_padding_bsr;    /*!< Number of times padding BSRs were sent */
  uint8    num_regular_bsr;    /*!< Number of times regular BSRs were sent */
  uint8    num_periodic_bsr;   /*!< Number of times periodic BSRs were sent */
  uint8    num_cancel_bsr;     /*!< Number of times cancel BSRs were happended */
  uint32   grant_received;     /*!< Grant received in bytes */
  uint32   grant_utilized;     /*!< Grant utilized in bytes */
} nb1_mac_log_ul_tx_stats_s;

#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif

#endif
