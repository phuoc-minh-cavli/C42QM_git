/*!
  @file
  nb1_log_codes.h

  @brief
  This file contains NB1 log code definitions.

*/

/*=============================================================================

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_log_codes.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
                   delays in PDCP WMks for all bearers
06/04/09   MNK     Defined log codes for NAS
=============================================================================*/

#ifndef NB1_LOG_CODES_H
#define NB1_LOG_CODES_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include <log_codes.h>


#define LOG_NB1_BASE_C  0xB240

/*=============================================================================

                   NB1 SPECIFIC LOG PACKET ID DEFINITIONS
                   RANGE: 0xB240-0xB7F

=============================================================================*/

/*! @brief NB1 ML1 RACH MSG1 log code */
#define LOG_NB1_ML1_ULM_RACH_MSG1_LOG_C          (0xB240)

/*! @brief NB1 ML1 RACH MSG2 log code */
#define LOG_NB1_ML1_ULM_RACH_MSG2_LOG_C          (0xB241)

/*! @brief NB1 ML1 RACH MSG3 log code */
#define LOG_NB1_ML1_GM_RACH_MSG3_LOG_C           (0xB242)

/*! @brief NB1 ML1 RACH MSG4 log code */
#define LOG_NB1_ML1_GM_RACH_MSG4_LOG_C           (0xB243)

/*! @brief NB1 GM Grant Info log code */
#define LOG_NB1_ML1_GM_DCI_INFO_LOG_C            (0xB244)

/*! @brief NB1 GM TX Report log code */
#define LOG_NB1_ML1_GM_TX_REPORT_LOG_C           (0xB245)

/*! @brief NB1 ML1 GM PDSCH STAT Ind log code */
#define LOG_NB1_ML1_GM_PDSCH_STAT_IND_LOG_C     (0xB246)

/*! @brief NB1_ML1_GM_PWR_CNTRL logging*/
#define LOG_NB1_ML1_GM_PWR_CNTRL_LOG_C           (0xB247)

/*! @brief NB1 ML1 CDRX events info log code */
#define LOG_NB1_ML1_GM_CDRX_EVENTS_INFO_LOG_C    (0xB248)

/*! @brief NB1 ML1 GM RLM log code */
#define LOG_NB1_ML1_GM_RLM_LOG_C                 (0xB249)

/*! @brief NB1 ML1 SUMMARY SYSTEM Infor log code */
#define LOG_NB1_ML1_SUM_SYS_INFO_LOG_C           (0xB24B)

/*! @brief NB1 ML1  Cell Reselection log code */
#define LOG_NB1_ML1_CELL_RESEL_LOG_C              (0xB24C)

/*! @brief NB1 ML1 SERV CELL MEAS RESULTS LOG */
#define LOG_NB1_ML1_SERV_CELL_MEAS_RESPONSE_LOG_C (0xB24D)

/*! @brief NB1 ML1  NBR Cell Measurement log code */
#define LOG_NB1_ML1_NBR_CELL_MEAS_LOG_C           (0xB24E)

/*! @brief NB1 ML1 SLEEPMGR log code */
#define LOG_NB1_ML1_SLEEPMGR_LOG_C                (0xB24F)

/*! @brief NB1 ML1 SRCH PBCH Decode log code */
#define LOG_NB1_ML1_PBCH_DECODE_LOG_C           (0xB250)

/*! @brief NB1 ML1 DLM DECODE PAGE INFO */
#define LOG_NB1_ML1_DLM_DECODE_PAGE_LOG_C        (0xB251)

/*! @brief NB1 ML1 DLM FW payload ind logging */
#define LOG_NB1_ML1_DLM_PAYLOAD_IND_LOG_C        (0xB2FA)

/*! @brief NB1 ML1 DLM FW cell configuration logging */
#define LOG_NB1_ML1_DLM_CELL_CONFIGURATION_LOG_C (0xB2FB)

/*! @brief NB1 ML1 DLM FW cell update logging */
#define LOG_NB1_ML1_DLM_CELL_UPDATE_LOG_C        (0xB2FC)

/*! @brief NB1 ML1 DLM FW decode stat logging */
#define LOG_NB1_ML1_DLM_DECODE_STATS_LOG_C       (0xB2FD)

/*! @brief NB1 ML1 DLM SIB1 CMN CFG logging */
#define LOG_NB1_ML1_DLM_SIB1_COMN_CFG_LOG_C      (0xB2FE)


/*=============================================================================

                   LTE SPECIFIC LOG PACKET ID DEFINITIONS (REUSED)

=============================================================================*/

/********************* START OF MAC LOG CODE DEFINES *************************/

/*! @brief NB1 MAC configure log code */
#define LOG_NB1_MAC_CONFIG_LOG_C                 (0xB060)

/*! @brief NB1 MAC RACH TRIGGER log code */
#define LOG_NB1_MAC_RACH_TRIGGER_LOG_C           (0xB061)

/*! @brief NB1 MAC RACH ATTEMPT log code */
#define LOG_NB1_MAC_RACH_ATTEMPT_LOG_C           (0xB062)

/*! @brief NB1 MAC DL TB log code */
#define LOG_NB1_MAC_DL_TB_LOG_C                  (0xB063)

/*! @brief NB1 MAC UL TB log code */
#define LOG_NB1_MAC_UL_TB_LOG_C                  (0xB064)

/*! @brief NB1 MAC QoS Internal code */
#define LOG_NB1_MAC_QOS_INT_LOG_C                (0xB065)

/*! @brief NB1 MAC QoS Internal code */
#define LOG_NB1_MAC_BUFFER_STATUS_INT_LOG_C      (0xB066)

/*! @brief NB1 MAC UL TX Statistics code */
#define LOG_NB1_MAC_TX_STATISTICS_LOG_C          (0xB067)


/*********************** END OF MAC LOG CODE DEFINES *************************/


/********************* START OF RLCDL LOG CODE DEFINES ***********************/

/*! @brief NB1 RLCDL configuration log code */
#define LOG_NB1_RLCDL_CONFIG_LOG_C               (0xB081)

/*! @brief NB1 RLCDL AM all pdu log code */
#define LOG_NB1_RLCDL_AM_ALL_PDU_LOG_C           (0xB082)

/*! @brief NB1 RLCDL AM control pdu log code */
#define LOG_NB1_RLCDL_AM_CONTROL_PDU_LOG_C       (0xB083)

/*! @brief NB1 RLCDL AM polling pdu log code */
#define LOG_NB1_RLCDL_AM_POLLING_PDU_LOG_C       (0xB084)

/*! @brief NB1 RLCDL AM signaling pdu log code */
#define LOG_NB1_RLCDL_AM_SIGNALING_PDU_LOG_C     (0xB085)

/*! @brief NB1 RLCDL UM data pdu log code */
#define LOG_NB1_RLCDL_UM_DATA_PDU_LOG_C          (0xB086)

/*! @brief NB1 RLCDL statistics log code */
#define LOG_NB1_RLCDL_STATISTICS_LOG_C           (0xB087)

/*! @brief NB1 RLCDL Unified log code*/
#define LOG_NB1_RLCDL_UNIFIED_LOG_C              (0xB089)  //! @todo: Remove eMBMS

/*********************** END OF RLCDL LOG CODE DEFINES ***********************/


/********************* START OF RLCUL LOG CODE DEFINES ***********************/

/*! @brief NB1 RLCUL configuration log code */
#define LOG_NB1_RLCUL_CONFIG_LOG_C               (0xB091)

/*! @brief NB1 RLCUL AM all pdu log code */
#define LOG_NB1_RLCUL_AM_ALL_PDU_LOG_C           (0xB092)

/*! @brief NB1 RLCUL AM control pdu log code */
#define LOG_NB1_RLCUL_AM_CONTROL_PDU_LOG_C       (0xB093)

/*! @brief NB1 RLCUL AM polling pdu log code */
#define LOG_NB1_RLCUL_AM_POLLING_PDU_LOG_C       (0xB094)

/*! @brief NB1 RLCUL AM signaling pdu log code */
#define LOG_NB1_RLCUL_AM_SIGNALING_PDU_LOG_C     (0xB095)

/*! @brief NB1 RLCUL UM data pdu log code */
#define LOG_NB1_RLCUL_UM_DATA_PDU_LOG_C          (0xB096)

/*! @brief NB1 RLCUL statistics log code */
#define LOG_NB1_RLCUL_STATISTICS_LOG_C           (0xB097)

/*! @brief NB1 RLCUL AM state log code */
#define LOG_NB1_RLCUL_AM_STATE_LOG_C             (0xB098)

/*! @brief NB1 RLCUL UM state log code */
#define LOG_NB1_RLCUL_UM_STATE_LOG_C             (0xB099)

/*********************** END OF RLCUL LOG CODE DEFINES ***********************/


/********************* START OF PDCPDL LOG CODE DEFINES ***********************/

/*! @brief NB1 PDCPDL configuration log code */
#define LOG_NB1_PDCPDL_CONFIG_LOG_C              (0xB0A0)

/*! @brief NB1 PDCPDL data pdu log code */
#define LOG_NB1_PDCPDL_DATA_PDU_LOG_C            (0xB0A1)

/*! @brief NB1 PDCPDL control pdu log code */
#define LOG_NB1_PDCPDL_CONTROL_PDU_LOG_C         (0xB0A2)

/*! @brief NB1 PDCPDL data pdu with ciphering log code */
#define LOG_NB1_PDCPDL_CIPHER_DATA_PDU_LOG_C     (0xB0A3)

/*! @brief NB1 PDCPDL statistics log code */
#define LOG_NB1_PDCPDL_STATISTICS_LOG_C          (0xB0A4)

/*! @brief NB1 PDCPDL data pdu with integrity log code */
#define LOG_NB1_PDCPDL_INTEG_DATA_PDU_LOG_C      (0xB0A5)

/*********************** END OF PDCPDL LOG CODE DEFINES ***********************/


/********************* START OF PDCPUL LOG CODE DEFINES ***********************/

/*! @brief NB1 PDCPUL configuration log code */
#define LOG_NB1_PDCPUL_CONFIG_LOG_C              (0xB0B0)

/*! @brief NB1 PDCPUL data pdu log code */
#define LOG_NB1_PDCPUL_DATA_PDU_LOG_C            (0xB0B1)

/*! @brief NB1 PDCPUL control pdu log code */
#define LOG_NB1_PDCPUL_CONTROL_PDU_LOG_C         (0xB0B2)

/*! @brief NB1 PDCPUL data pdu with ciphering log code */
#define LOG_NB1_PDCPUL_CIPHER_DATA_PDU_LOG_C     (0xB0B3)

/*! @brief NB1 PDCPUL statistics log code */
#define LOG_NB1_PDCPUL_STATISTICS_LOG_C          (0xB0B4)

/*! @brief NB1 PDCPUL data pdu with integrity log code */
#define LOG_NB1_PDCPUL_INTEG_DATA_PDU_LOG_C      (0xB0B5)

/*! @brief NB1 PDCPUL statistics log code */
#define LOG_NB1_PDCPUL_DELAY_STATISTICS_LOG_C    (0xB0B6)

/*********************** END OF PDCPUL LOG CODE DEFINES ***********************/


/********************* START OF RRC LOG CODE DEFINES **************************/

/*! @brief NB1 RRC OTA message log code */
#define LOG_NB1_RRC_OTA_MSG_LOG_C                (0xB0C0)

/*! @brief NB1 RRC MIB log code */
#define LOG_NB1_RRC_MIB_LOG_C                    (0xB0C1)

/*! @brief NB1 RRC serv cell info log code */
#define LOG_NB1_RRC_SERV_CELL_INFO_LOG_C         (0xB0C2)

/*! @brief NB1 RRC plmn search req log code */
#define LOG_NB1_RRC_PLMN_SEARCH_REQ_LOG_C        (0xB0C3)

/*! @brief NB1 RRC plmn search rsp log code */
#define LOG_NB1_RRC_PLMN_SEARCH_RSP_LOG_C        (0xB0C4)

/*! @brief NB1 RRC Idle meas config log code */
#define NB1_RRC_LOG_IDLE_MEAS_CONFIG_LOG_C       (0xB0C8)

/*! @brief NB1 RRC Paging UE ID log code */
#define NB1_RRC_LOG_PAGING_UE_ID_LOG_C           (0xB0CB)

/*! @brief NB1 RRC CGI INFO log code */
#define LOG_NB1_RRC_CGI_INFO_LOG_C               (0xB0CC)  //! @todo: Remove CGI not needed

/*********************** END OF RRC LOG CODE DEFINES **************************/

/********************* START OF ML1 LOG CODE DEFINES **************************/
/*! @brief NB1 ML1 log code base
    LOG_NB1_BASE_C = 0xB240
    LOG_NB1_ML1_BASE_C = 0xB240 + 0x150 = 0xB390

    DO NOT CHANGE LOG PACKETS' LOG CODES. USE NEW LOGCODES INSTEAD.
*/
#define LOG_NB1_ML1_BASE_C                       (0x150 + LOG_NB1_BASE_C)

/*! @brief NB1 ML1 DLM common cfg log code 0xB390 */
#define LOG_NB1_ML1_DLM_COMN_CFG_LOG_C           (0x0 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 DLM dedicated cfg log code 0xB391 */
#define LOG_NB1_ML1_DLM_DED_CFG_LOG_C            (0x1 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 ULM common cfg log code */
#define LOG_NB1_ML1_ULM_COMN_CFG_LOG_C           (0x2 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 ULM dedicated cfg log code */
#define LOG_NB1_ML1_ULM_DED_CFG_LOG_C            (0x3 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 GM common cfg log code */
#define LOG_NB1_ML1_GM_COMN_CFG_LOG_C            (0x4 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 GM dedicated cfg log code */
#define LOG_NB1_ML1_GM_DED_CFG_LOG_C             (0x5 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 PRACH CFG log code */
#define LOG_NB1_ML1_ULM_PRACH_CFG_LOG_C          (0x6 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 GM PDCCH Info log code */
#define LOG_NB1_ML1_GM_NPDCCH_INFO_LOG_C         (0xB + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 PUSCH Power Control log code */
#define LOG_NB1_ML1_GM_PUSCH_PWR_CNTRL_LOG_C     (0xE + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 PUCCH Power Control log code */
#define LOG_NB1_ML1_GM_PUCCH_PWR_CNTRL_LOG_C     (0xF + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 SRS TX Report log code */
#define LOG_NB1_ML1_GM_SRS_TX_REPORT_LOG_C       (0x10 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 SRS Power Control log code */
#define LOG_NB1_ML1_GM_SRS_PWR_CNTRL_LOG_C       (0x11 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 GM UL PKT Build log code */
#define LOG_NB1_ML1_GM_UL_PKT_BUILD_LOG_C        (0x12 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 SRCH List Freq Scan log code */
#define LOG_NB1_ML1_SRCH_LIST_FREQ_SCAN_LOG_C    (0x14 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 SRCH Full Freq Scan log code */
#define LOG_NB1_ML1_SRCH_FULL_FREQ_SCAN_LOG_C    (0x15 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 SRCH Initial Acquisition log code */
#define LOG_NB1_ML1_SRCH_INIT_ACQ_LOG_C          (0x16 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 SRCH MEAS CONFIG log code */
#define LOG_NB1_ML1_SRCH_MEAS_CFG_LOG_C          (0x17 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 SRCH MEAS Report log code */
#define LOG_NB1_ML1_SRCH_INTRA_SRCH_MEAS_CFG_LOG_C (0x18 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 SRCH INTRA FREQ MEAS  log code */
#define LOG_NB1_ML1_SRCH_INTRA_FREQ_MEAS_LOG_C   (0x19 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1  CELL SELECT log code */
#define LOG_NB1_ML1_S_CRITERIA_CHECK_LOG_C       (0x1A + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1  Common Config log code */
#define LOG_NB1_ML1_DRX_SERV_CELL_EVAL_LOG_C     (0x1B + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1  Paging DRX Req log code 0xB3AC */
#define LOG_NB1_ML1_PAGING_DRX_INFO_LOG_C        (0x1C + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1  Idle Meas Req log code */
#define LOG_NB1_ML1_IDLE_MEAS_REQ_LOG_C         (0x1D + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1  UE Mobility State Info log code */
#define LOG_NB1_ML1_UE_MOB_STATE_INFO_LOG_C     (0x1E + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1  Serving Cell Measurement log code */
#define LOG_NB1_ML1_SERV_MEAS_EVAL_LOG_C        (0x1F + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1  NB1 Serving meas response log code */
#define LOG_NB1_ML1_RESEL_CANDIDATES_LOG_C      (0x26 + LOG_NB1_ML1_BASE_C)

///*! @brief NB1 ML1 Sleep log code */
//#define LOG_NB1_ML1_SLEEPMGR_LOG_C              (0x2B + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 Sleep Statistics log code */
#define LOG_NB1_ML1_SLEEPMGR_STATS_LOG_C        (0x2C + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 Band Scan Results*/
#define LOG_NB1_ML1_BAND_SCAN_CNF_LOG_C         (0x2D + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 System Scan Results*/
#define LOG_NB1_ML1_SYS_SCAN_CNF_LOG_C          (0x2E + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 NEIGHBOR CELL MEAS REQ/REQP LOG*/
#define LOG_NB1_ML1_IDLE_NEIGHBOR_CELL_MEAS_REQ_RESP_LOG_C (0x32 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 NEIGHBOR CELL SEARCH REQ/REQP LOG*/
#define LOG_NB1_ML1_NEIGHBOR_CELL_SRCH_REQ_RESP_LOG_C (0x34 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 CELL MEAS RESULTS log code */
#define LOG_NB1_ML1_CELL_MEAS_RESULTS_LOG_C     (0xB196)

/*! @brief NB1 ML1 Serving cell info. log code */
#define LOG_NB1_ML1_SERV_CELL_INFO_LOG_C        (0x37 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 Band/System Scan Results*/
#define LOG_NB1_ML1_BAND_AND_SYS_SCAN_CNF_LOG_C (0x39 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 BPLMN start request log code */
#define LOG_NB1_ML1_BPLMN_START_REQ_LOG_C       (0x40 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 BPLMN cell request log code */
#define LOG_NB1_ML1_BPLMN_CELL_REQ_LOG_C        (0x41 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 BPLMN cell confirm log code */
#define LOG_NB1_ML1_BPLMN_CELL_CONFIRM_LOG_C    (0x42 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 BPLMN time avail log code */
#define LOG_NB1_ML1_BPLMN_TIME_AVAIL_IND_LOG_C  (0x43 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 BPLMN complete ind log code */
#define LOG_NB1_ML1_BPLMN_COMPLETE_IND_LOG_C    (0x44 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 BPLMN stop request log code */
#define LOG_NB1_ML1_BPLMN_STOP_REQ_LOG_C        (0x45 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 BPLMN stop confirm log code */
#define LOG_NB1_ML1_BPLMN_STOP_CONFIRM_LOG_C    (0x46 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 SCHDLR write buffer log code */
#define LOG_NB1_ML1_SCHDLR_WRITE_BUFFER_LOG_C   (0xB1A7)

/*! @brief NB1 ML1 DLM link capacity status log code */
#define LOG_NB1_ML1_DLM_RES_AVAIL_FRAC_STAT_LOG_C (0x49 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 DC offset log code */
#define LOG_NB1_ML1_DC_OFFSET_LOG_C             (0x52 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 STM profile log code */
#define LOG_NB1_ML1_STM_PROFILE_LOG_C           (0x56 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 DLM All DL decodes results log code */
#define LOG_NB1_ML1_DLM_DECODE_RESULTS_LOG_C    (0x5D + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 Scheduler profile log code */
#define LOG_NB1_ML1_SCHDLR_PROFILE_LOG_C        (0x60 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 mcvs vpe freq log code */
#define LOG_NB1_ML1_MCVS_VPE_FREQ_LOG_C         (0x64 + LOG_NB1_ML1_BASE_C)

/*! @brief NB1 ML1 DLM BLER statistics log code */
#define LOG_NB1_ML1_DLM_BLER_STATS_LOG_C        (0x69 + LOG_NB1_ML1_BASE_C)

/*! @brief PLT log code to log all MSGR messages with payload information */
#define LOG_NB1_ML1_PLT_SNOOPER_LOG_C           (0xB1D7)

/*! @brief Total number of NB1 ML1 log codes. This macro needs to be updated if more log codes are added  */
#define LOG_NB1_ML1_TOTAL_LOG_CODES             (LOG_NB1_ML1_PLT_SNOOPER_LOG_C-0xB160+1)

/*********************** END OF ML1 LOG CODE DEFINES **************************/


/********************* START OF NB1/NAS LOG CODE DEFINES **************************/

/*! @brief NB1 NAS ESM security protected OTA incoming message log code */
#define LOG_NB1_NAS_ESM_SEC_OTA_IN_MSG_LOG_C          (0xB0E0)

/*! @brief NB1 NAS ESM security protected OTA outgoing  log code */
#define LOG_NB1_NAS_ESM_SEC_OTA_OUT_MSG_LOG_C         (0xB0E1)

/*! @brief NB1 NAS ESM plain OTA incoming  log code */
#define LOG_NB1_NAS_ESM_OTA_IN_MSG_LOG_C              (0xB0E2)

/*! @brief NB1 NAS ESM plain OTA outgoing  log code */
#define LOG_NB1_NAS_ESM_OTA_OUT_MSG_LOG_C             (0xB0E3)

/*! @brief NB1 NAS ESM bearer context state code */
#define LOG_NB1_NAS_ESM_BEARER_CONTEXT_STATE_LOG_C    (0xB0E4)

/*! @brief NB1 NAS ESM BEARER CONTEXT INFO code */
#define LOG_NB1_NAS_ESM_BEARER_CONTEXT_INFO_LOG_C     (0xB0E5)

/*! @brief NB1 NAS ESM PROCEDURE LOG code */
#define LOG_NB1_NAS_ESM_PROCEDURE_STATE_LOG_C         (0xB0E6)

/*! @brief NB1 NAS EMM security protected OTA outgoing  log code */
#define LOG_NB1_NAS_EMM_SEC_OTA_IN_MSG_LOG_C          (0xB0EA)

/*! @brief NB1 NAS EMM security protected OTA outgoing  log code */
#define LOG_NB1_NAS_EMM_SEC_OTA_OUT_MSG_LOG_C         (0xB0EB)

/*! @brief NB1 NAS EMM Plain OTA outgoing message log code */
#define LOG_NB1_NAS_EMM_OTA_IN_MSG_LOG_C              (0xB0EC)

/*! @brief NB1 NAS EMM Plain OTA incoming log code */
#define LOG_NB1_NAS_EMM_OTA_OUT_MSG_LOG_C             (0xB0ED)

/*! @brief NB1 NAS EMM state log code */
#define LOG_NB1_NAS_EMM_STATE_LOG_C                   (0xB0EE)

/*! @brief NB1 NAS EMM USIM card mode log code */
#define LOG_NB1_NAS_EMM_USIM_CARD_MODE_LOG_C          (0xB0EF)

/*! @brief NB1 NAS EMM Current securuty context log code */
#define LOG_NB1_NAS_EMM_CURRENT_SECUIRY_CONTEXT_LOG_C (0xB0F2)

/*! @brief NB1 NAS EMM CKIK for PS domain log code */
#define LOG_NB1_NAS_EMM_CKIK_FOR_PS_DOMAIN_LOG_C      (0xB0F3)

/*! @brief NB1 NAS EMM native security context log code */
#define LOG_NB1_NAS_EMM_NATIVE_SECURITY_CONTEXT_LOG_C (0xB0F4)

/*! @brief NB1 NAS EMM USIM service table log code */
#define LOG_NB1_NAS_EMM_USIM_SERVICE_TABLE_LOG_C      (0xB0F5)

/*! @brief NB1 NAS EMM FORBIDDEN TRACKING AREA LIST */
#define LOG_NB1_NAS_EMM_FORBIDDEN_TRACKING_AREA_LIST_LOG_C (0xB0F6)

/*********************** END OF NB1/NAS LOG CODE DEFINES **************************/

#endif /* NB1_LOG_CODES_H */

