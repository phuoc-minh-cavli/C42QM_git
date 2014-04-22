/*!
  @file
  nb1_mac.h

  @brief
  This file contains all External interfaces exported by MAC layer.


*/

/*===========================================================================

  Copyright (c) 2018 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_mac.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/21   ve      CR 2875404 : Msg5 grant timer functionality removed
04/04/19   ss      CR 2425286 : Stop DL log, periodic timer & dereg FC during DMM unload
02/11/19   ss      CR 2309435 : FR 44853: Rel.14 NB2 Non-anchor Support
02/08/19   ss      CR 2392666 : Traffic Inactivity Monitor Timer Enhancement
07/15/18   ss      CR 2278167 : FR 50565: L2 task merge for NB
07/19/17   ss      CR 2063856 : CE Level limitation support through EFS
07/03/17   ss      CR 2059662 : NB1 Data Inactivity Local Connection Release
03/04/17   ss      CR 2014952 : NB1 L2 memory optmization phase-2 MAC+RLC+PDCP mainlined
01/27/17   ss      CR 1115360 : NB1 MAC Re/architecture

===========================================================================*/

#ifndef NB1_MAC_H
#define NB1_MAC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <pthread.h>
#include <nb1_as.h>
#include <nb1.h>
#include <nb1_l1_types.h>
#include "nb1_cphy_msg.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
  
  /*! @brief anchor carrier index id
  */
#define NB1_ANCHOR_CARRIER_INDEX          0

/*! @brief defined the maximum number of logical channel in MAC, including 2SRB, 8DRB and 2CCCH,  this
MACRO can be used for invalid LC ID as well */
#define NB1_MAC_MAX_LC              (NB1_MAX_ACTIVE_RB + 2)

/*! @brief Access reason enumeration type
*/
typedef enum
{
  NB1_MAC_ACCESS_REASON_CONNECTION_REQ,   /*!< connection request  */
  NB1_MAC_ACCESS_REASON_RADIO_FAILURE,    /*!< radio failure */
  NB1_MAC_ACCESS_REASON_HAND_OVER,        /*!< hand over */
  NB1_MAC_ACCESS_REASON_UL_DATA_ARRIVAL,  /*!< up link data arrival */
  NB1_MAC_ACCESS_REASON_DL_DATA_ARRIVAL,  /*!< down link data arrival */
  /*!< Data Traffic inactivity timer expiry */
  NB1_MAC_ACCESS_REASON_INACTIVITY_TMR_EXPIRY_LOCAL_RELEASE
} nb1_mac_access_reason_e;

/*! @brief MAC information action enumeration type
*/
typedef enum
{
  NB1_MAC_ACTION_NONE,               /*!< no action  */
  NB1_MAC_ACTION_CONNECTION_RELEASE, /*!< connection release */
  NB1_MAC_ACTION_HO,                 /*!< hand off */
  NB1_MAC_ACTION_RADIO_FAILURE,      /*!< radio failure */
  NB1_MAC_ACTION_CONNECTION_CANCEL,  /*!< connection cancel */
  NB1_MAC_ACTION_CFG_RECFG
} nb1_mac_inform_action_e;

/*! @brief BSR LC Group enumeration type
*/
typedef enum
{
  NB1_MAC_BSR_LCGROUP_0,   /*!< BSR LC group 0 */
  NB1_MAC_BSR_LCGROUP_1,   /*!< BSR LC group 1 */
  NB1_MAC_BSR_LCGROUP_2,   /*!< BSR LC group 2 */
  NB1_MAC_BSR_LCGROUP_3,   /*!< BSR LC group 3 */
  NB1_MAC_BSR_LCGROUP_MAX, /*!< Maximum number of BSR LC group */
} nb1_mac_bsr_lcgroup_e;

/*! @brief Enumeration of QoS BSR event type
*/
typedef enum
{
  NB1_MAC_QOS_NO_EVENT_BSR,
  NB1_MAC_QOS_PERIODIC_BSR,
  NB1_MAC_QOS_HIGH_DATA_ARRIVAL_BSR,
  NB1_MAC_QOS_ROBUSTNESS_BSR,
  NB1_MAC_QOS_UL_BSR_BASED_FC
} nb1_mac_qos_bsr_event_e;

/*! @brief Enumeration of QoS BSR triggered type
*/
typedef enum
{
  NB1_MAC_QOS_NO_BSR,
  NB1_MAC_QOS_CANCELLED_BSR,
  NB1_MAC_QOS_LONG_BSR,
  NB1_MAC_QOS_SHORT_BSR,
  NB1_MAC_QOS_PADDING_LONG_BSR,
  NB1_MAC_QOS_PADDING_SHORT_BSR,
  NB1_MAC_QOS_PADDING_TRUNCATED_BSR
} nb1_mac_qos_bsr_triggered_e;


typedef enum
{
  NB1_MAC_QOS_SUSPEND_INACTIVITY,
  NB1_MAC_QOS_RESUME_INACTIVITY
} nb1_mac_qos_suspend_resume_inactivity_type_e;

/*! @brief MAC statistics data: LT07
*/
typedef struct
{
  uint16 c_rnti;          /*!<current C-RNTI: 1 to 65523.0 = not initialized*/
  uint8  lcg_buf_size[NB1_MAC_BSR_LCGROUP_MAX];/*!< buffer size for all LCGs*/
  uint16 timing_adv;      /*!< current timing advance: 0 to 63 */
} nb1_mac_stats_data_s;

/*! @brief MAC to update the statistics metric Request MSG. Modem
    statistics manager will send this request to MAC with the data pointer */
typedef struct
{
  msgr_hdr_struct_type   hdr;  /*!< Message header */
  /* Message payload */
  nb1_mac_stats_data_s   *payload_ptr;
} nb1_mac_stats_update_req_msg_s;

/*! @brief MAC to update the statistics metric CNF MSG. MAC will send back
    this CNF when MAC done updating the statistics due to a earlier request */
typedef struct
{
  msgr_hdr_struct_type   hdr;  /*!< Message header */
  /* Message payload */
} nb1_mac_stats_update_cnf_msg_s;

/*! @brief contains the info provided to CMAPI for each active embms session*/
typedef struct
{
  nb1_mbsfn_area_id_t area_id; /*!< Area id */
  nb1_pmch_id_t pmch_id; /*!< pmch id */
  nb1_lc_id_t lc_id; /*!< lc id */
  /*!< Number of MTCH transport block bytes received on this channel since
       last activation */
  uint64 num_mtch_tb_bytes;
} nb1_mtch_info_s;

/*! @brief MAC to update the statistics of this structure provided by CMAPI*/
typedef struct
{
  uint8 num_active_embms_comb; /*Number of active area_id,pmch_id combinations*/
  nb1_mtch_info_s mtch_info_arr[NB1_EMBMS_MAX_ACTIVE_SESSIONS];
} nb1_mac_active_embms_stats_s;

/*! @brief MAC PRACH configuration parameters per CE level
*/
typedef struct
{
  /*! Indicates the presences of max_num_preamble_attempt,
      first_subcarrier and last_subcarrier as these of part of same list in OTA
  */
  boolean max_num_preamble_attempt_subcarrier_present;

  /*! Max number of preamble transmission attempts per CE level; Valid values: 3, 4, 5, 6, 7, 8, 10 */
  uint8 max_num_preamble_attempt;

  /*! First Preamble, range (0..33)
    nprach-SubcarrierOffset-r13				ENUMERATED {n0, n12, n24, n36, n2, n18, n34, spare1} */
  uint8 first_subcarrier;

  uint8 multi_start_subcarrier;

  /*! Last Preamble, range (12..47)
      nprach-NumSubcarriers-r13				ENUMERATED {n12, n24, n36, n48},*/
  uint8 last_subcarrier;

  /*! Indicates the presences of contention_resolution_timer  */
  boolean contention_resolution_timer_present;

  /*! Contention Resolution timer for Common and UE specific PDCCH search space */
  uint32 contention_resol_tmr;
 
  /*! Current CE level enabled or Disabled */
  boolean ce_level_enabled;
  
  /*! DL Carrier index for Non Anchor carrier RACH */
  uint8 dl_carrier_index;

} nb1_mac_rach_param_ce_s;

typedef struct
{

  /*
    Anchor rach probability. Applicable only when atleast one non anchor carrier prach config is valid
    Entire parameter is not applicable when no non-anchor is valid
  */
  uint16 anchor_rach_probability[NB1_MAX_NPRACH_RESOURCE];

  boolean multi_tone_support;

  uint8 ce_level_limit_enabled;
  
  /*
    can be of RANGE 0 to NB1_CPHY_MAX_NON_ANCHOR_CARRIER
    if 0, NO non-anchor carriers and all RACH are in anchor carrier
  */
  uint8 num_non_anchor;
  
  uint8 rach_param_ce_size[NB1_CPHY_MAX_NON_ANCHOR_CARRIER + 1];

  nb1_l1_nprach_param_s *prach_param_ce[NB1_CPHY_MAX_NON_ANCHOR_CARRIER + 1][NB1_MAX_NPRACH_RESOURCE];
  
  boolean nprach_param_non_anchor_valid [NB1_CPHY_MAX_NON_ANCHOR_CARRIER + 1];

  nb1_mac_rach_param_ce_s rach_param_ce[NB1_CPHY_MAX_NON_ANCHOR_CARRIER + 1][NB1_MAX_NPRACH_RESOURCE];

}nb1_mac_rach_ce_cfg_s;

/*! @brief QoS active LC info */
typedef struct
{
  nb1_lc_id_t  lcid;
  uint8        priority;     /*!< Priority Level    */
} nb1_mac_qos_active_lc_info_s;

/*! @brief QoS feedback Information Structure from
MAC Up Link Task to MAC Control Task. */
typedef struct
{
  nb1_mac_qos_bsr_triggered_e bsr_triggered; /*!< indicate the last BSR triggered*/
  uint16  grant;              /*!< grant in byte */
  uint16  padding_byte;       /*!< padding byte */
  nb1_l1_sfn_t sys_fn;        /*!< System Frame number */
  nb1_l1_subfn_t  sub_fn;     /*!< Subframe number */
  uint8      rlc_used_size;  /*!< total RLC used size */
  /*!UL FC is sending zero BSR now, stop reloading
  the rexted timer so that the timer will tick until
  it is expired for the next starting FC ON/OFF period*/
  boolean ul_fc_zero_bsr_enable;
  uint32 highest_nz_lcid;     /*!< Highest priority LCID which is not empty */
  nb1_mac_qos_bsr_event_e  bsr_event; /*!< BSR Event */
  nb1_rnti_type_e          rnti_type;
} nb1_mac_qos_feedback_ind_s;

/*===========================================================================

  FUNCTION:  nb1_mac_embms_get_stats

===========================================================================*/
/*!
    @brief

    @detail
    This function updates the entries into the structure nb1_mac_active_embms_stats
    pointer provided by CMAPI.
    The num_active_embms_comb refers to the number of active embms sessions.
    The mtch_info_arr array entries upto indices (num_active_embms_comb - 1) are filled.
    The number of accumulated MTCH TB bytes gets reset whenever an LCID stops
    being monitored.


    @return
    None

    @note

    @see related_function()

*/
/*=========================================================================*/
EXTERN void nb1_mac_embms_get_stats
(
  /*!< pointer to the structure to fill the active embms combos MTCH information. */
  nb1_mac_active_embms_stats_s* active_embms_stats_ptr
);
/*===========================================================================

  FUNCTION:  nb1_mac_ctrl_task_init

===========================================================================*/
/*!
    @brief
    This function is a initialization function for MAC Control Task.

    @detail
    Initialization consists of:
    - Initialization MAC control attributes
    - Initialization stack size and priority
    - Starting the MAC Control Task

    @return
    Thread ID

    @note

    @see related_function()

*/
/*=========================================================================*/
EXTERN pthread_t nb1_mac_ctrl_task_init
(
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  nb1_mac_ul_task_init

===========================================================================*/
/*!
    @brief
    This function is a initialization function for MAC UL Task.

    @detail
    Initialization consists of:
    - Initialization MAC UL attributes
    - Setting stack size and priority
    - Starting the MAC UL Task

    @return
    Initialized Status: TRUE or FALSE

    @note

    @see related_function()

*/
/*=========================================================================*/
EXTERN pthread_t nb1_mac_ul_task_init
(
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  nb1_mac_dl_task_init

===========================================================================*/
/*!
  @brief
  This function is a initialization function for MAC DL Task.

  @detail
    Initialization consists of:
    - Initialization MAC DL attributes
    - Setting stack size and priority
    - Starting the MAC DL Task

    @return
    Thread ID

    @note

    @see related_function()
*/
/*=========================================================================*/
EXTERN pthread_t nb1_mac_dl_task_init
(
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  nb1_mac_ctrl_test_start_periodic_process

===========================================================================*/
/*!
  @brief
  This function commences the MAC periodic QoS processing

  @detail
    - Starts the periodic Qos processing for buffer status, RACH/SR req etc
    - Starts the QoS periodic logging
    - Sets the periodic interval timer

  @return

  @note

  @see related_function()
*/
/*=========================================================================*/
EXTERN void nb1_mac_ctrl_test_start_periodic_process
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_mac_ctrl_test_set_connected

===========================================================================*/
/*!
  @brief
  This function is a test function to set the MAC state to connected

  @detail
    Sets the internal MAC state to connected for testing purposes

  @return

  @note

  @see related_function()
*/
/*=========================================================================*/
EXTERN void nb1_mac_ctrl_test_set_connected
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_mac_ul_test_set_connected

===========================================================================*/
/*!
  @brief
  This function sets the MAC UL state to connected

  @detail
  This function sets the internal state of MAC UL to connected for testing
  purposes.

  @return

  @note

  @see related_function()
*/
/*=========================================================================*/
EXTERN void nb1_mac_ul_test_set_connected
(
  void
);


/*===========================================================================

  FUNCTION:  nb1_mac_qos_get_allowed_tput

===========================================================================*/
/*!
    @brief
    This function returns the allowed throughput for each calculation cycle

    @detail
    This function returns 0 if it is called before the first calculation cycle is over.
    This function returns the same value when called anytime during the same calculation cycle.
    If the value returned by this function is less than the sum of the GBR for all bearers,
    then DS should return that value instead of the value returned by this API.


    @return
    Allowed throughput in bytes/sec

    @see

*/
/*=========================================================================*/
EXTERN uint32 nb1_mac_qos_get_allowed_tput
(
  void
);
EXTERN uint32 _nb1_mac_qos_get_allowed_tput
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_mac_ul_data_set_gps_padding_status

===========================================================================*/
/*!
    @brief
    This function sets the current gps status to decide whether to do gps padding or not

    @detail
    This function returns void and will set the gps status to decide whether
    to do gps padding or not in mac_ul task

    @return
    none

    @see

*/
/*=========================================================================*/
EXTERN void nb1_mac_ul_data_set_gps_padding_status
(
  boolean enable_gps_padding   /*!< Flag to decide whether to do gps padding or not */
);
EXTERN void _nb1_mac_ul_data_set_gps_padding_status
(
  boolean enable_gps_padding   /*!< Flag to decide whether to do gps padding or not */
);

/*===========================================================================

  FUNCTION:  nb1_mac_qos_update_suspend_resume_data_inacitivty_req

===========================================================================*/
/*!
    @brief
    API to update the suspend Traffice monitor timer when receiving suspend/resume
    request from ML1

    @detail


    @return
    None

    @see related_function()

*/
/*=========================================================================*/
EXTERN void nb1_mac_qos_update_suspend_resume_data_inacitivty_req
(
  nb1_mac_qos_suspend_resume_inactivity_type_e   suspend_resume_inactivity
);

#endif /* NB1_MAC_H */
