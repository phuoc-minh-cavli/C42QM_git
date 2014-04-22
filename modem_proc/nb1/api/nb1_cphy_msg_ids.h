#ifndef NB1_CPHY_MSG_IDS_H
#define NB1_CPHY_MSG_IDS_H
/*!
  @file nb1_cphy_msg_ids.h

  @brief
   The Message IDs used to interact with the NB1 L1 software module.

   This file should contain all CPHY message ids.

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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_cphy_msg_ids.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
                    RX Chain requests

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


/*! @brief
  Enum for the external control saps used to interface with L1
*/
enum
{
  /*=========================================================================*/
  /*                        Request/Confirm                                  */
  /*=========================================================================*/
  NB1_CPHY_ACQ_ID = 0x0,
  NB1_CPHY_START_ID = 0x1,
  NB1_CPHY_STOP_ID = 0x2,
  NB1_CPHY_ABORT_ID = 0x3,
  NB1_CPHY_COMMON_CFG_ID = 0x4,
  NB1_CPHY_DEDICATED_CFG_ID = 0x5,
  NB1_CPHY_CON_RELEASE_ID = 0x6,
  NB1_CPHY_BAND_SCAN_ID = 0x8,
  NB1_CPHY_SYSTEM_SCAN_ID = 0x9,
  NB1_CPHY_CELL_SELECT_ID = 0xa,
  NB1_CPHY_START_RACH_ID = 0xb,
  NB1_CPHY_RACH_RC_ID = 0xc,
  NB1_CPHY_RAR_PARAMS_ID = 0xd,
  NB1_CPHY_RACH_ABORT_ID = 0xe,
  NB1_CPHY_RACH_CFG_ID = 0xf,
  NB1_CPHY_OUT_OF_SYNC_ID = 0x10,
  NB1_CPHY_APPLY_TA_PARAMS_ID = 0x11,
  NB1_CPHY_SIB_SCHED_ID = 0x12,
  NB1_CPHY_CELL_BAR_ID = 0x13,
  NB1_CPHY_IDLE_DRX_CFG_ID = 0x14,
  NB1_CPHY_IDLE_MEAS_CFG_ID = 0x15,
  NB1_CPHY_CONN_MEAS_CFG_ID = 0x16,
  NB1_CPHY_CELL_RESEL_CANCEL_ID = 0x17,
  NB1_CPHY_CANCEL_CONN_ID = 0x18,
  NB1_CPHY_SUSPEND_ID = 0x19,
  NB1_CPHY_RESUME_ID = 0x1a,
  NB1_CPHY_IRAT_CDMA_SYSTEM_TIME_CHG_ID = 0x1b,
  NB1_CPHY_BPLMN_START_ID = 0x1c,
  NB1_CPHY_BPLMN_STOP_ID = 0x1d,
  NB1_CPHY_BPLMN_CELL_ID = 0x1e,
  NB1_CPHY_BPLMN_SKIP_ID = 0x1f,
  NB1_CPHY_SEND_UL_SR_ID = 0x20,
  NB1_CPHY_DEACTIVATE_SPS_ID = 0x21,
  NB1_CPHY_UL_TO_OOS_ID = 0x22,
  NB1_CPHY_TDD_CFG_ID   = 0x23,
  NB1_CPHY_GPS_TIMETAG_ID = 0x24,
  NB1_CPHY_NMR_INFO_ID = 0x25,
  NB1_CPHY_MAC_DRX_ID = 0x26,
  NB1_CPHY_SERV_CELL_MEAS_ID = 0x27,
  NB1_CPHY_GPS_MRL_ID = 0x28,
  NB1_CPHY_GPS_EVENT_CB_ID = 0x29,
  NB1_CPHY_RFCHAIN_ID = 0x2A,
  NB1_CPHY_IND_IRAT_CGI_START_ID = 0x2b,
  NB1_CPHY_IND_IRAT_CGI_END_ID = 0x2c,
  NB1_CPHY_PRS_START_ID = 0x2D,
  NB1_CPHY_PRS_UPDATE_CELLS_ID = 0x2E,
  NB1_CPHY_PRS_STOP_ID = 0x2F,
  NB1_CPHY_ABORT_IRAT_CGI_ID = 0x30,
  NB1_CPHY_MCCH_CHANGE_NOTIFICATION_ID = 0x31,
  NB1_CPHY_MSI_ID = 0x32,
  NB1_CPHY_CDMA_SYSTIME_ID = 0x33,
  NB1_CPHY_HRPD_MEAS_RESULTS_ID = 0x34,
  NB1_CPHY_DRX_OPT_ID = 0x35,
  NB1_CPHY_GPS_TIMEXFER_ID = 0x36,
  NB1_CPHY_GPS_TIMEXFER_ABORT_ID = 0x37,
  NB1_CPHY_BPLMN_SUSPEND_ID = 0x38,
  NB1_CPHY_MBSFN_SIGNAL_STRENGTH_ID= 0x39,
  NB1_CPHY_UEINFO_RPT_ID= 0x3a,
  NB1_CPHY_GPS_RXTX_REQ_ID = 0x3B,
  NB1_CPHY_GPS_RXTX_CNF_ID = 0x3C,
  NB1_CPHY_UEINFO_MDT_SESSION_ID = 0x3D,
  NB1_CPHY_UTC_TIME_UPDATE_REQ_ID = 0x3E,
  NB1_CPHY_UTC_TIME_UPDATE_CNF_ID = 0x3F,
  NB1_CPHY_CELL_UNBAR_ID = 0x40,
  NB1_CPHY_BEST_MBMS_NEIGH_FREQ_ID = 0x41,
  NB1_CPHY_TRM_PRIORITY_ID = 0x42,
  NB1_CPHY_RELEASE_TRM_ID = 0x43,
  NB1_CPHY_CDMA_CHANNEL_BAR_ID = 0x44,
  NB1_CPHY_DEBUG_DEADLOCK_TMR_EXP_ID = 0x45,
  NB1_CPHY_UEINFO_CONN_FAILURE_RPT_REQ_ID = 0x46,
  NB1_CPHY_UEINFO_CONN_FAILURE_RPT_CNF_ID = 0x47,
  NB1_CPHY_IND_HANDOVER_SFN_STATUS_ID = 0x48,
  NB1_CPHY_NV_UPDATE_ID = 0x49,
  NB1_CPHY_DL_TPUT_ESTM_REPORT_ID = 0x4A,
  NB1_CPHY_GPS_MRL_SUBSCRIBE_ID = 0x4B,
  NB1_CPHY_HARQ_ACK_TX_STS_ID = 0x4C,
  NB1_CPHY_REQ_EMBMS_SET_PRIORITY_ID = 0x4D,
  NB1_CPHY_REQ_EMBMS_GET_MSP_DATA_ID = 0x4E,
  NB1_CPHY_IND_EMBMS_GET_MSP_DATA_ID = 0x4F,
  NB1_CPHY_TRM_UNLOCK_ALLOWED_ID     = 0x50,
  NB1_CPHY_UL_TX_POWER_INFO_ID = 0x51,
  NB1_CPHY_SEND_GM_ADJ_SR_MAX_TX_ID = 0x52,
  NB1_CPHY_HYPER_SFN_ID = 0x53,
  NB1_CPHY_SIB1_COMMON_CFG_ID = 0x54,
  NB1_CPHY_PAGE_DONE_ID = 0x55,
  NB1_CPHY_CONTEXT_STORE_ID = 0x56,
  NB1_CPHY_CONTEXT_STORE_CNF_ID = 0x57,
  NB1_CPHY_GPS_GTS_ID = 0x58,
  NB1_CPHY_CE_LVL_ID = 0x59,
#ifdef FEATURE_FMD_SPEED_INFO  
  NB1_CPHY_FMD_SPEED_INFO_ID = 0x5A,
#endif /* FEATURE_FMD_SPEED_INFO */  
};

  /*=========================================================================*/
  /*                          Indications                                    */
  /*=========================================================================*/
enum
{
  NB1_CPHY_IND_PDCCH_ORDER_ID = 0x0,
  NB1_CPHY_IND_RA_TIMER_EXPIRED_ID = 0x1,
  NB1_CPHY_IND_MIB_ID = 0x2,
  NB1_CPHY_IND_MOD_PRD_BND_ID = 0x3,
  NB1_CPHY_IND_DL_WEAK_ID = 0x4,
  NB1_CPHY_IND_CELL_RESEL_ID = 0x5,
  NB1_CPHY_IND_OOS_ID = 0x6,
  NB1_CPHY_IND_RL_FAILURE_ID = 0x7,
  NB1_CPHY_IND_MEAS_REPORT_ID = 0x8,
  NB1_CPHY_IND_MSG3_TRANSMISSION_ID = 0x9,
  NB1_CPHY_IND_BPLMN_TIME_AVAIL_ID = 0xa,
  NB1_CPHY_IND_BPLMN_COMPLETE_ID = 0xb,
  NB1_CPHY_IND_UL_PKT_BUILD_ID = 0xc,
  NB1_CPHY_IND_UL_PKT_FAILED_ID = 0xd,
  NB1_CPHY_IND_PDCCH_ORDER_RACH_SUCCESS_ID = 0xe,
  NB1_CPHY_IND_RSSI_ID = 0xf,
  NB1_CPHY_IND_SIGNAL_REPORT_CFG_ID = 0x10,
  NB1_CPHY_IND_MAC_RAA_ID = 0x11,
  NB1_CPHY_IND_T310_STATUS_ID = 0x12,
  NB1_CPHY_IND_SCELL_STATUS_ID = 0x13,
  NB1_CPHY_IND_RA_ONE_OFF_ID = 0x14,
  NB1_CPHY_IND_UEINFO_MDT_REPORT_ID = 0x15,
  NB1_CPHY_IND_CA_EVENT_ID = 0x16,
  NB1_CPHY_IND_IRAT_ASF_NEEDED_ID = 0x17,
  NB1_CPHY_IND_RF_AVAILABLE_ID = 0x18,
  NB1_CPHY_IND_RF_UNAVAILABLE_ID = 0x19,
  NB1_CPHY_IND_IFREQ_OTDOA_ID = 0x1a,
  NB1_CPHY_IND_BPLMN_SIB_RECEIVED_ID = 0x1b,
  NB1_CPHY_IND_BLACKLISTED_CSG_PCI_RANGE_ID = 0x1c,
  NB1_CPHY_IND_UE_MODE_CHANGE_ID = 0x1d,
  NB1_CPHY_IND_TCELL_RF_UNAVAILABLE_ID = 0x1e,
  NB1_CPHY_IND_CDRX_PKT_TIMING_ID = 0x1f,
  NB1_CPHY_IND_SCELL_TUNEAWAY_STATUS_ID = 0x20,
  NB1_CPHY_IND_RACH_MSG1_SCHED_ID = 0x21,
  NB1_CPHY_IND_UL_TX_POWER_ID = 0x22,
  NB1_CPHY_IND_NPDCCH_DIRECT_IND_INFO_ID = 0x23,
  NB1_CPHY_IND_UL_SUSPEND_RESUME_RACH_ID = 0x24,
  NB1_CPHY_EDRX_UIM_PAGE_ID = 0x25,
  NB1_CPHY_PSM_IND_ID = 0x26,
  NB1_CPHY_IND_MISMATCH_RAR_ID = 0X27,
  NB1_CPHY_PAGE_PDSCH_ID = 0x28,
  NB1_CPHY_IND_TX_START_ID = 0x29,
  NB1_CPHY_IND_TX_END_ID = 0x2a,
  NB1_CPHY_SRCH_RACH_SUSPEND_RESUME_ID = 0X2D,
  NB1_CPHY_IND_CANCEL_RACH_ID = 0x2E,
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
  NB1_CPHY_IND_BPLMN_EARFCN_STATUS_ID = 0x2F,
#endif
  NB1_CPHY_RACH_SUCCESS_ID = 0x30,
};



#endif /* NB1_CPHY_MSG_IDS_H */

