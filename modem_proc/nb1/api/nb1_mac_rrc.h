/*!
  @file
  nb1_mac_rrc.h

  @brief
  This file contains all the common define structures/function prototypes
  interface between MAC and RRC.


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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_mac_rrc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/19   ss      CR 2433131: Implementation of Contention Free RACH 
09/12/18   ss      CR 2262061: FR 45229: Release Assistance Indication Support
07/03/17   ss      CR 2059662: NB1 Data Inactivity Local Connection Release
01/27/17   ss      CR 1115360: NB1 MAC Re/architecture

===========================================================================*/

#ifndef NB1_MAC_RRC_H
#define NB1_MAC_RRC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_nb1.h>
#include <nb1_cphy_msg.h>
#include <nb1_l1_types.h>
/* Start of L2 internal including */
#include "nb1_l2_comdef.h"
#include "nb1_mac.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief defined SR Prohibit infinity value
*/
#define NB1_MAC_SR_PROHIBIT_INFINITY 0xFFFFFFFF

/*! @brief defined BSR timer infinity value
*/
#define NB1_MAC_BSR_TIMER_INFINITY   0xFFFFFFFF

/*! @brief defined TA infinity value
*/
#define NB1_MAC_TA_INFINITY           NB1_OTA_IE_INFINITY_UINT16

/*! @brief defined bit mask to indicate DL configuration is present
*/
#define NB1_MAC_DL_CFG_BM       0x01

/*! @brief defined bit mask to indicate RACH configuration is present
*/
#define NB1_MAC_RACH_CFG_BM     0x02

/*! @brief defined bit mask to indicate UL configuration is present
*/
#define NB1_MAC_UL_CFG_BM       0x04

/*! @brief defined bit mask to indicate LC Info configuration is present
*/
#define NB1_MAC_LC_INFO_CFG_BM  0x08

/*! @brief defined bit mask to indicate C-RNTI configuration is present
*/
#define NB1_MAC_C_RNTI_CFG_BM   0x10

/*! @brief defined bit mask to indicate eMBMS configuration is present
*/
#define NB1_MAC_EMBMS_CFG_BM    0x20

/*! @brief defined bit mask to indicate all bitmasks are present
*/
#define NB1_MAC_ALL_CFG_BM                    \
               (NB1_MAC_DL_CFG_BM         |   \
                NB1_MAC_RACH_CFG_BM       |   \
                NB1_MAC_UL_CFG_BM         |   \
                NB1_MAC_LC_INFO_CFG_BM    |   \
                NB1_MAC_C_RNTI_CFG_BM     |   \
                NB1_MAC_EMBMS_CFG_BM)


/*! @brief defined total number of RA Preamble Index
*/
#define NB1_MAC_MAX_RA_PREAMBLE_INDEX   64

/*! @brief defined total number of RA Preamble Index
*/
#define NB1_MAC_MAX_RA_SUBCARRIER_INDEX   48

/*! @brief defined size of the NB1 connection request SDU = 9 bytes
*/
#define NB1_MAC_MSG3_CONN_REQ_SDU_SIZE   9

/*! @brief defined Compare first 6 bytes of connection request SDU
*/
#define NB1_MAC_MAX_CONN_REQ_SDU_SIZE   6

/*! @brief message power offset group B enumeration type
*/

typedef enum
{
  NB1_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_INFINITY,
  NB1_MAC_MSG_PWR_OFFSET_GROUP_B_DB0,
  NB1_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB5,
  NB1_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB8,
  NB1_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB10,
  NB1_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB12,
  NB1_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB15,
  NB1_MAC_MSG_PWR_OFFSET_GROUP_B_MINUS_DB18
} nb1_mac_msg_pwr_offset_group_b_e;

/*! @brief defined MAC internal CE-level enumration, CE level 0 to 3 is for
enhanced coverage mode, ce level A is for normal coverage group A, ce level
B is for normal coverage group B */
typedef enum
{
  NB1_MAC_CE_LEVEL_0        =      0,
  NB1_MAC_CE_LEVEL_1,
  NB1_MAC_CE_LEVEL_2,
  NB1_MAC_CE_LEVEL_MAX
} nb1_mac_ce_level_e;


/*! @brief TDD UL\DL Config enum; This is same as
    TDD subframe assignment config received in SIB1
    or in RadioResourceConfigCommon during Mobility
    IMPORTANT: Do not change the enum value, since it
    has to match the TDD UL\DL enum values of
    nb1_l1_tdd_ul_dl_cfg_index_e in ML1 interface
*/
typedef enum
{
  NB1_MAC_TDD_UL_DL_CFG_0,
  NB1_MAC_TDD_UL_DL_CFG_1,
  NB1_MAC_TDD_UL_DL_CFG_2,
  NB1_MAC_TDD_UL_DL_CFG_3,
  NB1_MAC_TDD_UL_DL_CFG_4,
  NB1_MAC_TDD_UL_DL_CFG_5,
  NB1_MAC_TDD_UL_DL_CFG_6,
  NB1_MAC_TDD_UL_DL_CFG_NONE, /* Used for FDD */
  NB1_MAC_TDD_UL_DL_CFG_MAX   /* Not Used, Invalid value */

} nb1_mac_tdd_ul_dl_cfg_e;


typedef enum
{
  NB1_RRC_PROBABILITY_ZERO,
  NB1_RRC_PROBABILITY_ONESIXTEENTH,
  NB1_RRC_PROBABILITY_ONEFIFTEENTH,
  NB1_RRC_PROBABILITY_ONEFOURTEENTH,
  NB1_RRC_PROBABILITY_ONETHIRTEENTH,
  NB1_RRC_PROBABILITY_ONETWELFTH,
  NB1_RRC_PROBABILITY_ONEELEVENTH,
  NB1_RRC_PROBABILITY_ONETENTH,
  NB1_RRC_PROBABILITY_ONENINTH,
  NB1_RRC_PROBABILITY_ONEEIGTH,
  NB1_RRC_PROBABILITY_ONESEVENTH,
  NB1_RRC_PROBABILITY_ONESIXTH,
  NB1_RRC_PROBABILITY_ONEFIFTH,
  NB1_RRC_PROBABILITY_ONEFOURTH,
  NB1_RRC_PROBABILITY_ONETHIRD,
  NB1_RRC_PROBABILITY_ONEHALF,
  NB1_RRC_PROBABILITY_ONE
}nb1_rrc_prach_anchor_probability_e;

typedef struct 
{
  boolean nprach_param_non_anchor_valid;
  
  // N PRACH Config
  uint8 prach_param_ce_list_size;

  nb1_l1_nprach_param_s prach_param_ce_list[NB1_MAX_NPRACH_RESOURCE];

  //valid range from 1 to NB1_CPHY_MAX_NON_ANCHOR_CARRIER. Indicates the index in DL non-Anchor carrier list
  uint8 npdcch_carrier_index[NB1_MAX_NPRACH_RESOURCE];
}nb1_rrc_non_anchor_nprach_parameter_list_s;

/*! @brief RACH configuration parameters structure. This contains both
RACH parameters for MAC and L1
*/
typedef struct
{
  int16   preamble_initial_power;/*!< Initial Power Level */
  uint8   power_ramping_step;   /*!< Step power */
  uint16  preamble_trans_max;          /*!< MAX preamble attempts */
  int16   delta_preamble_msg3;         /*!< delta preamble message 3 */
  boolean PowerRampingParameters_NB_enabled;/*!<CE1 Power Level Configured*/
  int16   preambleInitialReceivedTargetPowerCE1;/*!<CE1 Initial Power Level */
  uint8   powerRampingStepCE1;   /*!< CE1 Step power */

  /*The criterion for UEs to select a NPRACH resource.
    the first element corresponds to RSRP threshold 1,
    the second element corresponds to RSRP threshold 2
    If absent, there is only one NPRACH resource.*/

  /*! Size of RSRP threshold list size
   */
  uint8 rsrp_thresh_prach_list_size;

  /*! This indicates whether rach config for CFRA is signalled from Enb
      Depending on this , MAC can proceed with CFRA. Change Request 3064
      R2-1709905 */
  boolean cfra_rach_config_signalled;

  /*! The criterion for UE to select PRACH resource set. Up to 2 RSRP threshold
     values are signalled to determine the enhanced coverage level for PRACH */
  nb1_cphy_meas_eutra_thresh_t rsrp_thresh_prach_list[NB1_MAC_RSRP_THRESH_PRACH_LIST_SIZE];

  /*! Contention resolution timer Size*/
  uint8 contention_resolution_timer_size;
  /*! Contention resolution timer*/
  uint32 contention_resolution_timer_new[NB1_MAX_NPRACH_RESOURCE];

  nb1_cphy_prach_setup_param_s  l1_rach; /*!< L1 RACH configuration */

  //can be of RANGE 0 to NB1_CPHY_MAX_NON_ANCHOR_CARRIER
  //If 0, NO non-anchor carriers and all RACH are in anchor carrier
  uint8 num_non_anchor;

  //Range from 0 to NB1_MAX_NPRACH_RESOURCE
  uint8 anchor_rach_prob_size;
  
  //Anchor rach probability. Applicable only when atleast one non anchor carrier prach config is valid
  //Entire parameter is not applicable when no non-anchor is valid
  nb1_rrc_prach_anchor_probability_e anchor_rach_probability[NB1_MAX_NPRACH_RESOURCE];

  //non anchor prach parameter list. valid number passed in parameter num_non_anchor
  nb1_rrc_non_anchor_nprach_parameter_list_s non_anchor_prach_list[NB1_CPHY_MAX_NON_ANCHOR_CARRIER];
} nb1_mac_rach_cfg_s;

/*! @brief QoS Logical channel configuration parameters structure
*/
typedef struct
{
  /*! Priority Level    */
  uint8               priority;

  /*! Indicates that the logicalChannelSR-ProhibitTimer is enabled for the logical channel*/
  boolean             enable_lc_sr_prohibit;
} nb1_mac_lcqos_cfg_s;

/*! @brief LC info configuration parameters structure
*/
typedef struct
{
  /*! number of LC to be deleted */
  uint8                   num_deleted_lc;
  uint8                   deleted_lc_list[NB1_MAX_LC];
  /*! number of added or modified logical channel*/
  uint8                   num_addmod_lc;
  uint8                   addmod_lc_list[NB1_MAX_LC];
  /*! base pointer to LC info database of NB1_MAX_LC */
  nb1_mac_lcqos_cfg_s*    lc_list;
} nb1_mac_lc_info_cfg_s;

/*! @brief Up Link configuration parameters structure
*/
typedef struct
{
  boolean  bsr_cfg_included;    /*!< BSR configuration is included */
  uint32   bsr_timer_new;           /*!< BSR periodicity timer in ms*/
  uint32   retxed_bsr_timer;        /*!< retxed BSR timer in ms*/
  boolean lc_sr_prohibit_timer_included;  /*!< flag to indicate if Logical Channel SR Prorihit timer value is present*/
  uint32 lc_sr_prohibit_timer; /*!< LC SR Prohibit timer value in ms*/
  boolean npdcch_included; /*! Boolean to indicate if NPDCCH  is present or not*/
  nb1_l1_npdcch_num_rep_e num_rep; /*! NPDCCH Number of repetitions*/
  nb1_l1_npdcch_start_sf_uss_e start_sf_uss; /*! NPDCCH Offset USS*/
  nb1_l1_npdcch_offset_uss_e offset_uss; /*! NPDCCH Start SF USS*/
  boolean DataInactivityTimer_included;/*!< DataInactivityTimer configuration is included */
  uint32  DataInactivityTimer;  /*!< DataInactivityTimer is used to control Data inactivity operation.
                                     { s1, s2, s3, s5, s7, s10, s15, s20, s40, s50, s60,
                                      s80, s100, s120, s150, s180} .*/
  boolean rai_Activation;        /*!< rai_Activation configuration is enabled */
  boolean drx_configured;        /*!< Drx configuration is enabled */

} nb1_mac_ul_cfg_s;
//source/qcom/qct/modem/nb1/api/main/latest/nb1_mac_rrc.h
/*! @brief Down Link configuration parameters structure
*/
typedef struct
{
  uint16  ta_timer;               /*!< TA timer value in ms*/
} nb1_mac_dl_cfg_s;

/*! @brief eMBMS LC info structure
*/
typedef struct
{
  nb1_mbsfn_area_id_t  area_id;  /*!< Area id */
  nb1_pmch_id_t        pmch_id;  /*!< PMCH id */
  nb1_lc_id_t          lc_id;    /*!< Logical Channel id */
} nb1_embms_lc_info_s;

/*! @brief eMBMS configuration parameters structure
*/
typedef struct
{
  /*! number of active eMBMS LCs */
  uint8                    num_active_lc;

  /*! List of active eMBMS LCs*/
  nb1_embms_lc_info_s      active_lc_list[NB1_MAX_EMBMS_LC];

} nb1_mac_embms_cfg_s;

/*! @brief RRC-MAC configuration parameters structure. Depend of the
    cfg_type, The specific action will be taken by MAC as following:
  - NB1_L2_CFG_RELEASE : MAC will delete all the existing
    configuration by itself and go back to idle state.
  - NB1_L2_CFG_HO: Go to HO state and depend on the cfg_bitmask,
    MAC should take in the corresponding new configurations.
  - NB1_L2_CFG_RADIO_FAILURE: Go to radio failure state and depend on the
    cfg_bitmask, MAC should take in the corresponding new configurations.
  - NB1_L2_CFG_RECFG : No state change and depend on the
    cfg_bitmask, MAC should take in the corresponding new configurations.
  - NB1_L2_CFG_EMBMS : MAC will process the MSI based on the LC info sent by RRC
  - NB1_L2_CFG_RELEASE_EMBMS : MAC will delete all existing multicast LCs
  - NB1_L2_CFG_RELEASE_ALL : MAC will delete all the existing unicast and
    multicast configuration
  Note that: RRC can still indicate MAC to delete a given configure by
  setting the corresponding bitmask to 1 and set the pointer to NULL.
*/
typedef struct
{
  /*! type of configuration */
  nb1_l2_cfg_reason_e     cfg_type;
  /*! bit masks to indicate which configurations are present*/
  uint8  cfg_bitmask;
  /*! cfg_bitmask will be applied to the following pointers */
  /*! LC Info configuration structure */
  nb1_mac_lc_info_cfg_s   lc_info_cfg;
  /*! RACH configuration structure */
  nb1_mac_rach_cfg_s*     rach_cfg;
  /*! DL configuration structure */
  nb1_mac_dl_cfg_s*       dl_cfg;
  /*! UL configuration structure */
  nb1_mac_ul_cfg_s*       ul_cfg;
  /*! new CRNTI configured */
  uint16                  crnti_cfg;
  /*! eMBMS configuration */
  nb1_mac_embms_cfg_s     embms_cfg;

} nb1_mac_cfg_s;

/*! @brief RRC-MAC Access Request structure
*/
typedef struct
{
  nb1_mac_access_reason_e  access_reason; /*!< Access reason */
  uint8    raid;            /*!< RA index */
  uint8    raid_mask;       /*!< Explicitly signalled PRACH resource PRACH Mask Index */
  uint64   ueid;            /*!< this is the TMSI */
  uint16   msg_length;      /*!< length of RRC MSG3 */
  uint8    *msg_ptr;        /*!< pointer to MSG3 */
  nb1_mac_ce_level_e ce_level;
  uint16    estimated_msg5_length; /*!< estimated length of RRC MSG5 */
  boolean special_rach_trigger;
} nb1_mac_access_request_s;

/*-----------------------------------------------------------------------*/
/*     NB1 MAC/RRC External message definitions                          */
/*-----------------------------------------------------------------------*/

/*! @brief Access Request from RRC to MAC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /* Message payload */
  nb1_mac_access_request_s            access_info; /*!< access info */
} nb1_mac_access_req_msg_s;

/*! @brief Access Confirmation from RRC to MAC
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;            /*!< message header */
  /* Message payload */
  boolean                  crnti_included; /*!< if C-RNTI is present */
  uint16                   crnti;          /*!< C-RNTI value */
} nb1_mac_access_cnf_msg_s;

/*! @brief configuration Request from RRC to MAC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  nb1_mac_cfg_s                       cfg_data;
} nb1_mac_cfg_req_msg_s;

/*! @brief configuration confirmation from MAC to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr; /*!< message header */
  /* Message payload */
  errno_enum_type                     status; /*!< status of configuration */
} nb1_mac_cfg_cnf_msg_s;

/*! @brief Access Abort Request from RRC to MAC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr; /*!< message header */
  /* Message payload */

} nb1_mac_access_abort_req_msg_s;

/*! @brief Cancel Conn Request from RRC to MAC. Note that MAC
    must keep all the current configurations.
*/
typedef struct
{
  msgr_hdr_struct_type                hdr; /*!< message header */
  /* Message payload */
} nb1_mac_cancel_conn_req_msg_s;


/*! @brief Access Abort confirmation from MAC to RRC
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  /* Message payload */
  errno_enum_type          status; /*!< status of Abort */
} nb1_mac_access_abort_cnf_msg_s;

/*! @brief MAC start request from RRC to MAC
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  /* Message payload */

} nb1_mac_start_req_msg_s;

/*! @brief MAC stop request from RRC  to MAC
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  /* Message payload */

} nb1_mac_stop_req_msg_s;

/*! @brief MAC start confirmation from MAC to RRC
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  /* Message payload */
  errno_enum_type          status; /*!< status of mac start req */
} nb1_mac_start_cnf_msg_s;

/*! @brief MAC stop confirmation from MAC to RRC
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  /* Message payload */
  errno_enum_type          status; /*!< status of mac stop req */
} nb1_mac_stop_cnf_msg_s;

/*! @brief MAC RACH report request from RRC to MAC
*/
typedef struct
{
  msgr_hdr_struct_type   hdr;  /*!< Message header */
  /*!Indicates if RACH RPT REQ is for failed RACH (T300 expiry)
    or last successful RACH
    if TRUE, REQ is for failed RACH
    else REQ is for last successful RACH*/
  boolean failed_rach_rpt_is_req;
} nb1_mac_rach_rpt_req_msg_s;

/*! @brief MAC RACH report confirmation from MAC to RRC
*/
typedef struct
{
  msgr_hdr_struct_type   hdr;  /*!< Message header */
  /* Message payload */
  uint8 num_preambles_sent; /*!< Preambles sent in last successfull RACH */
  uint8 num_contention_detected; /*!< Contentions detected in last successfull RACH */
} nb1_mac_rach_rpt_cnf_msg_s;

#endif /* NB1_MAC_RRC_H */
