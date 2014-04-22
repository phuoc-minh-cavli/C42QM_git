/*!
  @file
  custnb1.h

  @brief
  This file contains definitions for the features tech areas like NAS, 1X, etc.
  may need and  featuriztion differences that NB1 Access Stratum(AS)
  may need to control.

*/

/*=============================================================================

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/cust/inc/custnb1.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/12/2015 ar      CR 803880: Define FEATURE_NB1_ELS_ENABLED and
                   FEATURE_NB1_UDC_ENABLED
                   under FEATURE_BOLT_MODEM
                   feautre enabled by default
=============================================================================*/

#ifndef CUSTNB1_H
#define CUSTNB1_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/


/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

/*! @brief NB1 supported by the UE
*/
#if defined(FEATURE_NB1) && defined (FEATURE_WCDMA)
//top level feature removed:   #define FEATURE_NB1_TO_WCDMA
#endif /* defined(FEATURE_NB1) && defined (FEATURE_WCDMA) */

#if defined (FEATURE_NB1) && defined (FEATURE_HDR)
//top level feature removed: #define FEATURE_NB1_TO_HDR
#endif

#if defined (FEATURE_NB1) && defined (FEATURE_CDMA)
//top level feature removed: #define FEATURE_NB1_TO_1X
#endif

#if defined (FEATURE_NB1) && defined (FEATURE_GSM)
//top level feature removed: #define FEATURE_NB1_TO_GSM
#endif

//top level feature removed: #define FEATURE_NB1_3GPP_REL8_MAR09
#define FEATURE_NB1_3GPP_REL8_JUN09
#define FEATURE_NB1_3GPP_REL8_SEP09
//top level feature removed: #define FEATURE_NB1_3GPP_REL8_DEC09
#define FEATURE_NB1_ML1_FTM_WORKAROUND
#define FEATURE_NB1_ML1_IDLE_LOG

/* @brief Feature to support connected mode DRX */
#define FEATURE_NB1_CDRX

/* @brief Interact with RF even on offtarget */
//#define FEATURE_NB1_RF

/* @brief Feature to support generic IRAT operations */
#define FEATURE_NB1_TO_IRAT \
  ( defined(FEATURE_NB1_TO_GSM) || defined(FEATURE_NB1_TO_WCDMA) || \
    defined(FEATURE_NB1_TO_1X)  || defined(FEATURE_NB1_TO_HDR) )

#define FEATURE_NB1_SIB_COMB

/* @brief Feature to enable PLMN Search */
#define FEATURE_NB1_BPLMN

/* @brief Feature to support GPS WWAN Search */
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
#define NB1_GPS_WWAN_PRIORITY_SEARCH
#endif

#define FEATURE_NB1_AS_RESET

#if defined (FEATURE_TDSCDMA)
#define FEATURE_NB1_TO_TDSCDMA
#endif
//top level feature removed: #define FEATURE_3GPP_CSFB

//top level feature removed: #define FEATURE_NB1_REL9

#define FEATURE_NB1_RRC_CEP_OPT

#define FEATURE_NB1_RF

#undef FEATURE_NB1_WDOG_DYNAMIC

#if ( defined(FEATURE_NB1_TO_GSM) || defined(FEATURE_NB1_TO_WCDMA) || \
    defined(FEATURE_NB1_TO_1X)  || defined(FEATURE_NB1_TO_HDR) )
  #define NB1_MEAS_EVENT_B1_DEFINED  TRUE
#else
  #define NB1_MEAS_EVENT_B1_DEFINED FALSE
#endif

// Features related to rrc caps for 3.3 and 4.0 PLs.
#ifdef FEATURE_NB1_TO_WCDMA
  #define NB1_EUTRA_CONN_TO_UTRA_CELL_DCH_PS_HO_DEFINED TRUE
  #define NB1_MEAS_UTRAN_REP_IN_EUTRA_CONN_DEFINED      TRUE
#else
  #define NB1_EUTRA_CONN_TO_UTRA_CELL_DCH_PS_HO_DEFINED FALSE
  #define NB1_MEAS_UTRAN_REP_IN_EUTRA_CONN_DEFINED      FALSE
#endif /* FEATURE_NB1_TO_WCDMA */

#ifdef  FEATURE_NB1_TO_1X
  #define NB1_EUTRA_CONN_TO_1XRTT_CS_ACTIVE_HO_DEFINED  TRUE /* Only for SRVCC*/
  #define NB1_MEAS_1X_REP_IN_EUTRA_CONN_DEFINED         TRUE
  #define FEATURE_NB1_1XCSFB_SUPPORT_DEFAULT            TRUE
#else
  #define NB1_EUTRA_CONN_TO_1XRTT_CS_ACTIVE_HO_DEFINED  FALSE
  #define NB1_MEAS_1X_REP_IN_EUTRA_CONN_DEFINED         FALSE
#endif /* FEATURE_NB1_TO_1X */

#if defined (FEATURE_NB1_TO_HDR)
  #define NB1_EUTRA_CONN_TO_HRPD_ACTIVE_HO_DEFINED        TRUE
  #define NB1_MEAS_HRPD_REP_IN_EUTRA_CONN_DEFINED         TRUE
#else
  #define NB1_EUTRA_CONN_TO_HRPD_ACTIVE_HO_DEFINED        FALSE
  #define NB1_MEAS_HRPD_REP_IN_EUTRA_CONN_DEFINED         FALSE
#endif

// Default Release 8.
//top level feature removed: #define NB1_3GPP_DEFAULT_RELEASE_VERSION NB1_3GPP_REL9_SEP10


#ifdef FEATURE_NB1_TO_GSM
//top level feature removed:
  #define NB1_MEAS_GERAN_REP_IN_EUTRA_CONN_DEFINED         TRUE
  #define NB1_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_DEFINED      TRUE
  #define NB1_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_WITH_NACC_DEFINED    TRUE
//top level feature removed:   #define FEATURE_NB1_TO_GSM_CGI                          TRUE
#else
  #define NB1_MEAS_GERAN_REP_IN_EUTRA_CONN_DEFINED         FALSE
  #define NB1_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_DEFINED      FALSE
  #define NB1_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_WITH_NACC_DEFINED    FALSE
//top level feature removed:   #define FEATURE_NB1_TO_GSM_CGI                          FALSE
#endif

/* Maximum number of NB1 instances that can be up at any given time */
#define NB1_MEM_MAX_INST                 1

#define FEATURE_NB1_ML1_POS_ECID
#define FEATURE_NB1_ML1_POS_RXTX
#ifdef FEATURE_LOC_GTS_SUPPORT
#define FEATURE_NB1_ML1_POS_GTS
#endif

#ifdef FEATURE_LOC_NB1_OTDOA_OPCRS
#define FEATURE_NB1_ML1_POS_OPCRS
#endif

#if (!defined(FEATURE_TABASCO_MODEM) && !defined(FEATURE_NB1_ATLAS_MODEM))
#define FEATURE_NB1_ML1_POS_PRS_FD_BUF_LOG
#endif

#ifdef FEATURE_CGPS
#define FEATURE_NB1_ML1_POS_TIME_TRANSFER
#define FEATURE_NB1_ML1_POS_TIMETAG
#define FEATURE_NB1_ML1_POS_GTS
#endif

#define FEATURE_NB1_DRX_OPT

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

/*Define DI-2.x feature flags*/


#define FEATURE_NB1_TDD_TO_UMTS_WITH_MEAS  TRUE
#define NB1_HANDOVER_BETWEEN_FDD_AND_TDD_DEFINED TRUE


#define FEATURE_NB1_TDD_TO_GSM_SRVCC


#define NB1_ML1_SM_CONN_NEW_IMPL
#define NB1_ML1_SM_CONN_NEW_PBCH

/*
#ifdef FEATURE_CUST_1
#define FEATURE_NB1_CDRX_IMS_VOLTE_OPT
#endif
*/

/*! @brief Feature flag for NB1 EMBMBS */
//#define FEATURE_NB1_EMBMS
//#undef FEATURE_NB1_EMBMS

/*! @brief If target has defined FEATURE_NB1_ELS_DISABLED then
  undefine FEATURE_NB1_ELS_ENABLED feature */
#ifdef FEATURE_NB1_ELS_DISABLED
    #undef FEATURE_NB1_ELS_ENABLED
#endif /* FEATURE_NB1_ELS_DISABLED */


/*! @brief If target has defined FEATURE_NB1_UDC_DISABLED then
  undefine FEATURE_NB1_UDC_ENABLED feature */
#ifdef FEATURE_NB1_UDC_DISABLED
    #undef FEATURE_NB1_UDC_ENABLED
#endif /* FEATURE_NB1_UDC_DISABLED */

#ifdef FEATURE_NB1_UDC_ENABLED
  #ifndef FEATURE_NB1_ELS_ENABLED
    #error �ELS Disabled and UDC Enabled � wrong feature flag setting�
  #endif /* FEATURE_NB1_ELS_ENABLED */
#endif /* FEATURE_NB1_UDC_ENABLED */

/* IRAT compile out defines */
#ifndef FEATURE_NB1_TO_1X
   #define NB1_1X_IRAT_COMPILE_OUT
#endif
#ifndef FEATURE_NB1_TO_HDR
   #define NB1_DO_IRAT_COMPILE_OUT
#endif
#ifndef FEATURE_NB1_TO_GSM
   #define NB1_G_IRAT_COMPILE_OUT
#endif
#ifndef FEATURE_NB1_TO_WCDMA
   #define NB1_W_IRAT_COMPILE_OUT
#endif
#ifndef FEATURE_NB1_TO_TDSCDMA
   #define NB1_TDS_IRAT_COMPILE_OUT
#endif

//Remove the 3GPP2 IRAT for DT
//Debug code
#ifdef FEATURE_NB1_TO_1X
#undef FEATURE_NB1_TO_1X
#endif

#ifdef FEATURE_NB1_TO_HDR
#undef FEATURE_NB1_TO_HDR
#endif
//Remove feature HST :#define FEATURE_NB1_HST

// Remove feature MDT:
//#ifndef FEATURE_NB1_MDT
//#define FEATURE_NB1_MDT
//#endif

// TODO - @chammond - Remove once integration creates an NB1 build command/definition file
#ifndef TEST_FRAMEWORK
#define FEATURE_NB1_PLT
#endif

// define to use the CELL_CFG module, do not define to use RX_CFG module
#define FEATURE_NB1_ML1_DLM_CELL_CFG

//#define FEATURE_NB1_TPUT_ESTM

#define NEW_IDLE_STM

//feature to support Non-anhcor mode
#define FEATURE_NB1_NON_ANCHOR

#endif /* CUSTNB1_H */
