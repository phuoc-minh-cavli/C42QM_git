#ifndef WWANME_COMMON_DEFS_H
#define WWANME_COMMON_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Location LTE OTDOA Common Definitions Header File

GENERAL DESCRIPTION
This file contains common definitions shared between all Location LTE OTDOA
module files.

Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2012 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 - 2023 Qualcomm Technologies, Inc. 
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/wwanme/common/inc/wwanme_common_defs.h#4 $
$DateTime: 2023/04/10 07:21:50 $
$Author: pwbldsvc $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "gps_types.h"
#include "custcgps.h"

#include "msg.h"
#include <math.h>
#include "queue.h"
#include "log.h"

#include "aries_os_api.h"
#include "aries_os_globals.h"


/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

#define WME_MSG_LOW(a,b,c,d)            MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_LOW, a,b,c,d)
#define WME_MSG_MED(a,b,c,d)            MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d)
#define WME_MSG_MED_4(a,b,c,d,e)        MSG_4(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e)
#define WME_MSG_MED_5(a,b,c,d,e,f)      MSG_5(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f)
#define WME_MSG_MED_6(a,b,c,d,e,f,g)    MSG_6(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f,g)
#define WME_MSG_MED_7(a,b,c,d,e,f,g,h)    MSG_7(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f,g,h)
#define WME_MSG_MED_8(a,b,c,d,e,f,g,h,i)    MSG_8(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f,g,h,i)
#define WME_MSG_HIGH(a,b,c,d)           MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c,d)
#define WME_MSG_ERROR(a,b,c,d)          MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a,b,c,d)
#define WME_MSG_FATAL(a,b,c,d)          MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_FATAL, a,b,c,d)

#define WME_MSG_SPRINTF_1(a,b);         MSG_SPRINTF_1( MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a, b);

/* Message macros for GTS */
#define GTS_MSG_LOW(a,b,c,d)            MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_LOW, a,b,c,d)
#define GTS_MSG_MED(a,b,c,d)            MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d)

#define GTS_MSG_0(a)                    MSG(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a)
#define GTS_MSG_1(a,b)                  MSG_1(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b)
#define GTS_MSG_2(a,b,c)                MSG_2(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c)
#define GTS_MSG_3(a,b,c,d)              MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d)
#define GTS_MSG_4(a,b,c,d,e)            MSG_4(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e)
#define GTS_MSG_5(a,b,c,d,e,f)          MSG_5(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f)
#define GTS_MSG_6(a,b,c,d,e,f,g)        MSG_6(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f,g)
#define GTS_MSG_7(a,b,c,d,e,f,g,h)      MSG_7(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f,g,h)
#define GTS_MSG_8(a,b,c,d,e,f,g,h,i)    MSG_8(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f,g,h,i)
#define GTS_MSG_9(a,b,c,d,e,f,g,h,i,j)  MSG_9(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f,g,h,i,j)

#define GTS_MSG_HIGH_0(a)                    MSG(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a)
#define GTS_MSG_HIGH_1(a,b)                  MSG_1(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b)
#define GTS_MSG_HIGH_2(a,b,c)                MSG_2(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c)
#define GTS_MSG_HIGH_3(a,b,c,d)              MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c,d)
#define GTS_MSG_HIGH_4(a,b,c,d,e)            MSG_4(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c,d,e)
#define GTS_MSG_HIGH_5(a,b,c,d,e,f)          MSG_5(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c,d,e,f)
#define GTS_MSG_HIGH_6(a,b,c,d,e,f,g)        MSG_6(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c,d,e,f,g)
#define GTS_MSG_HIGH_7(a,b,c,d,e,f,g,h)      MSG_7(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c,d,e,f,g,h)
#define GTS_MSG_HIGH_8(a,b,c,d,e,f,g,h,i)    MSG_8(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c,d,e,f,g,h,i)
#define GTS_MSG_HIGH_9(a,b,c,d,e,f,g,h,i,j)  MSG_9(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c,d,e,f,g,h,i,j)

#define GTS_MSG_HIGH(a,b,c,d)           MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c,d)
#define GTS_MSG_ERR(a,b,c,d)          MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a,b,c,d)
#define GTS_MSG_ERR_0(a)                MSG(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a)
#define GTS_MSG_ERR_1(a,b)              MSG_1(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a,b)
#define GTS_MSG_ERR_2(a,b,c)            MSG_2(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a,b,c)
#define GTS_MSG_FATAL(a,b,c,d)          MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_FATAL, a,b,c,d)
#define GTS_MSG_SPRINTF_1(a,b);         MSG_SPRINTF_1( MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a, b);

/* Message macros for xofestrm */
#define XOFESTRM_MSG_HIGH(a,b,c,d)           MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c,d)
#define XOFESTRM_MSG_ERROR(a,b,c,d)          MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a,b,c,d)
#define XOFESTRM_MSG_ERR_0(a)                MSG(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a)
#define XOFESTRM_MSG_ERR_1(a,b)              MSG_1(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a,b)
#define XOFESTRM_MSG_SPRINTF_1(a,b);         MSG_SPRINTF_1( MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a, b);

#define XOFESTRM_MSG_0(a)                    MSG(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a)
#define XOFESTRM_MSG_1(a,b)                  MSG_1(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b)
#define XOFESTRM_MSG_2(a,b,c)                MSG_2(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c)
#define XOFESTRM_MSG_3(a,b,c,d)              MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_LOW, a,b,c,d)
#define XOFESTRM_MSG_4(a,b,c,d,e)            MSG_4(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e)


/*--------------------------------------------------------------------------
 * LTE Related Constants
 *-----------------------------------------------------------------------*/

#define LLO_MSG_LOW(a,b,c,d)            MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_LOW, a,b,c,d)
#define LLO_MSG_MED(a,b,c,d)            MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d)
#define LLO_MSG_MED_4(a,b,c,d,e)        MSG_4(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e)
#define LLO_MSG_MED_5(a,b,c,d,e,f)      MSG_5(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f)
#define LLO_MSG_MED_6(a,b,c,d,e,f,g)    MSG_6(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f,g)
#define LLO_MSG_MED_7(a,b,c,d,e,f,g,h)    MSG_7(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f,g,h)
#define LLO_MSG_MED_8(a,b,c,d,e,f,g,h,i)    MSG_8(MSG_SSID_WWAN_LOC, MSG_LEGACY_MED, a,b,c,d,e,f,g,h,i)
#define LLO_MSG_HIGH(a,b,c,d)           MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a,b,c,d)
#define LLO_MSG_ERROR(a,b,c,d)          MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a,b,c,d)
#define LLO_MSG_ERROR_0(a)              MSG(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a)
#define LLO_MSG_ERROR_1(a, b)           MSG_1(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a, b)
#define LLO_MSG_ERROR_2(a, b, c)        MSG_2(MSG_SSID_WWAN_LOC, MSG_LEGACY_ERROR, a, b, c)
#define LLO_MSG_FATAL(a,b,c,d)          MSG_3(MSG_SSID_WWAN_LOC, MSG_LEGACY_FATAL, a,b,c,d)
#define LLO_COMPARE_CELLS(a,b,c,d)      ((a == b) && (c == d))

#define LLO_MSG_SPRINTF_1(a,b);         MSG_SPRINTF_1( MSG_SSID_WWAN_LOC, MSG_LEGACY_HIGH, a, b);

/* 1/(15000*2048) secs */
#define LLOC_TS_UNIT       3.25520833e-8

#define LLOC_US_PER_TS      0.0325520833
#define LLOC_US_PER_3TS     0.09765625
#define LLOC_US_TO_MS       0.001
#define LLOC_US_TO_NS       1000
#define LLOC_MS_TO_NS       1e6
#define LLOC_MS_PER_TS      LLOC_US_PER_TS*LLOC_US_TO_MS
#define LLOC_NS_PER_TS      LLOC_US_PER_TS*LLOC_US_TO_NS

/* Defines for inflation rate calculation
 * 1Ts/sec = 9.7588 m/s which is close to 9 m/s rate in AFLT.
 * Ts = 1/(2048 * 15000) = 1/(30720000) */
#define LLOC_INFLATION_RATE_TS_PER_SEC        1.0
#define LLOC_TS_PER_SEC                       30720000
#define LLOC_TS_PER_MSEC                      30720
#define LLOC_TS_PER_USEC                      30.72

/* Mts/Ts = 299792458 / (2048 * 150000) */
#define LLOC_MTS_PER_TS                       9.7588690

#define MIN_PHYSICAL_ID                         0
#define MAX_PHYSICAL_ID                         503
#define UNKNOWN_PHYSICAL_ID                     0xFFFF
#define PHYSICAL_ID_OK(x)   (  ( (x) >= ((MIN_PHYSICAL_ID)) ) && ( x <= MAX_PHYSICAL_ID ) )

#define MIN_EARFCN                              0
#define MAX_EARFCN                              262143
#define UNKNOWN_EARFCN                          0xFFFFFFFF
#define EARFCN_OK(x)        (  ( (x) >= ((MIN_EARFCN)) ) && ( x <= MAX_EARFCN ) )

#define MAX_RSRP                                 97
#define RSRP_OK(x)                               ( (x) <= MAX_RSRP )

#define MAX_RSRQ                                 34
#define RSRQ_OK(x)                               ( (x) <= MAX_RSRQ )

#define MAX_RXTX_TIMEDIFF                        4095
#define RXTX_TIMEDIFF_OK(x)                      ( (x) <= MAX_RXTX_TIMEDIFF )


/*--------------------------------------------------------------------------
 * General Constants Definitions
 *-----------------------------------------------------------------------*/
#define SEC_TO_MSEC                             1000u
#define MSEC_TO_SEC                             0.001f
#define NSEC_TO_MSEC                            1e-6f
#define MSEC_TO_NSEC                            1e6

/*--------------------------------------------------------------------------
 * Timer Id definitions
 *-----------------------------------------------------------------------*/
/* LTE OTDOA Timers */
#define LTE_OTDOA_CONTROL_TIMER_ID_SM_RPT      1   /* 00001 */
#define LTE_OTDOA_CONTROL_TIMER_ID_ML1_SANITY  2   /* 00010 */
#define LTE_OTDOA_CONTROL_TIMER_ID_MP_SANITY   4   /* 00100 */
#define LTE_OTDOA_CONTROL_TIMER_ID_SM_SANITY   8   /* 01000 */
#define LTE_OTDOA_CONTROL_ALL_TIMERS           15  /* 01111 */

/* N0te that the first 16 bits are reserved for OTDOA timers with
rest being for TDP timers */

#define LTE_OTDOA_CONTROL_TIMER_CNT   (4)

/* TDP timer id offset */
#define TDP_CONTROL_TIMER_ID_OFFSET            16

#define TDP_TIMER_ID_DIAG_MEAS_PERIOD            (1 << ( TDP_CONTROL_TIMER_ID_OFFSET ))
#define TDP_TIMER_ID_SESSION_TIMEOUT             (1 << ( TDP_CONTROL_TIMER_ID_OFFSET + 1))
#define TDP_TIMER_ID_TESTMODE_CS_MEAS_PERIOD     (1 << ( TDP_CONTROL_TIMER_ID_OFFSET + 2))
#define TDP_TIMER_ID_TESTMODE_POS_MEAS_PERIOD    (1 << ( TDP_CONTROL_TIMER_ID_OFFSET + 3))
#define TDP_TIMER_ID_TESTMODE_CSPOS_MEAS_PERIOD  (1 << ( TDP_CONTROL_TIMER_ID_OFFSET + 4))

#define WWANME_TDP_ALL_TIMERS   (TDP_TIMER_ID_DIAG_MEAS_PERIOD | \
                                 TDP_TIMER_ID_SESSION_TIMEOUT | \
                                 TDP_TIMER_ID_TESTMODE_CS_MEAS_PERIOD | \
                                 TDP_TIMER_ID_TESTMODE_POS_MEAS_PERIOD | \
                                 TDP_TIMER_ID_TESTMODE_CSPOS_MEAS_PERIOD )

/* Note that to avoid changing OTDOA files, currently 11/10/2015 only
the TDP timers will be handled in this file - which is the better architecture.
If desired OTDOA team can also have this file handle all the timers for WWAN ME 
CTRL task */
#define WWANME_CONTROL_TIMER_CNT   (5)
#define TDP_TIMER_CNT      (5)

#define WWANME_CONTROL_TIMER_VALID(x) ( (x == TDP_TIMER_ID_DIAG_MEAS_PERIOD) || \
                                        (x == TDP_TIMER_ID_SESSION_TIMEOUT ) || \
                                        (x == TDP_TIMER_ID_TESTMODE_CS_MEAS_PERIOD ) || \
                                        (x == TDP_TIMER_ID_TESTMODE_POS_MEAS_PERIOD ) || \
                                        (x == TDP_TIMER_ID_TESTMODE_CSPOS_MEAS_PERIOD ) )

/* GTS Timers */
#define GTS_CDMA_TTR_WAIT_TIMER            64
#define GTS_LTE_TTR_WAIT_TIMER             65 
#define GTS_EFS_PMIC_LOG_DELAY_TIMER       67
#define GTS_DRSYNC_PROG_ASSERT_TIMER       68
#define GTS_DRSYNC_PROG_DEASSERT_TIMER     69

/* WWAN ME Config NV Bit definitions */
/* Disable LTE Time transfer when this bit is set*/
#define WWANME_CONFIG_DISABLE_GTS_LTE_TTR                (1 << 0)
/* NV 73547 bit 1 to indicate if meas in connected mode will be time stamped or not.
Default behavior will be only meas in idle mode will be stored and time stamped. */
#define WWANME_CONFIG_ACCEPT_CONNECTED_MODE_TDP_MEAS     (1 << 1)
#define WWANME_CONFIG_SET_CONNECTED_VSRC_THRESHOLD       (1 << 2)
/* OTDOA Bits */
#define WWANME_CONFIG_OTDOA_ENABLE_PRSIC                 (1 << 3)

/* TDP Bits */
#define WWANME_CONFIG_TDP_TESTMODE_BOOTUP_TIMER_ENABLED  (1 << 4)

/* GTS Bits */
#define WWANME_CONFIG_DISABLE_GTS_CDMA_TTR               (1 << 5)
#define WWANME_CONFIG_DISABLE_GTS_TIMETAG                (1 << 6)
#define WWANME_CONFIG_ENABLE_GTS_DEBUG                   (1 << 7)
#define WWANME_CONFIG_ENABLE_SIB16_PROCESS_TTR           (1 << 8)

/* OTDOA NV config for inflating the search window size when serving cell is not part of the AD.
 * Note: 2 bits (#8 and #9) are used 
 * Value 0: Inflate the search size for +0 symbol offset only (i.e. full hypothesis).
 *          Once valid peak is detected for any cell, revert back to exp. RSTD/unc. provided in AD.
 * Value 1/2/3: Use 4 symbol offsets (-1, 0, 1 and 2) with full size for each hypothesis. 
 *          Once valid peak is detected for any cell, revert back to exp. RSTD/unc. provided in AD.
 */
#define WWANME_CONFIG_OTDOA_INFLATE_SRCH_SIZE_SHIFT      (8)
#define WWANME_CONFIG_OTDOA_INFLATE_SRCH_SIZE            (3 << WWANME_CONFIG_OTDOA_INFLATE_SRCH_SIZE_SHIFT)

/* NC integration config for OTDOA */
#define WWANME_CONFIG_OTDOA_NON_COH                      (1 << 10)

#define WWANME_CONFIG_ENABLE_PMIC_CORRECTION             (1 << 13)

#define WWANME_CONFIG_ENABLE_PMIC_USB_ERR_RATE           (1 << 14)

/*--------------------------------------------------------------------------
 * WWAN ME Meas Proc Task Message Definitions
 *-----------------------------------------------------------------------*/
typedef enum
{

  /* IPC 0... C_USR_MSG_ID_FIRST-1 is reserved by OS */
  C_LOC_LTE_OTDOA_MEASPROC_MSG_FIRST = C_USR_MSG_ID_FIRST,  
  
  /* ----------------------LTE OTDOA Messages---------------------- */
  C_LOC_LTE_OTDOA_MEASPROC_START_MEAS_PROCESSING,     /* 11 */
  C_LOC_LTE_OTDOA_MEASPROC_MEAS_RESULTS_AVAIL,        /* 12 */
  C_LOC_LTE_OTDOA_MEASPROC_STOP_MEAS_PROCESSING,      /* 13 */
  C_LOC_LTE_OTDOA_MEASPROC_FLUSH_IPC_QUEUE,           /* 14 */
  C_LOC_LTE_OTDOA_MEASPROC_RESET_NC_DATABASE,         /* 15 */
  C_LOC_LTE_OTDOA_MEASPROC_ADJUST_FA_THRESHOLD,       /* 16 */
  C_LOC_LTE_OTDOA_MEASPROC_MSG_LAST,
  C_LOC_LTE_OTDOA_MEASPROC_MSG_MAX = 99,

  /* ----------------------LTE TDP Messages---------------------- */
  C_LLOMP_TDP_MSG_FIRST = C_LOC_LTE_OTDOA_MEASPROC_MSG_MAX,  
  C_LLOMP_TDP_COMPUTE_EAP,   /* 100 : msg sent by OC to OMP task to compute Eap */
  C_LLOMP_TDP_MSG_LAST,                               /* 101 */
  C_LLOMP_TDP_MSG_MAX = 200
} wwanme_measproc_IpcMsgEnumType;


/*--------------------------------------------------------------------------
 * WWAN ME Control Task Message Definitions
 *-----------------------------------------------------------------------*/
typedef enum
{
  /* IPC 0... C_USR_MSG_ID_FIRST-1 is reserved by OS */

  /* WWANME-DIAG interface */
  C_LOC_LTE_OTDOA_CONTROL_DIAG_PKT_RECEIVED = C_USR_MSG_ID_FIRST,

  C_LOC_LTE_OTDOA_CONTROL_MSG_FIRST = C_USR_MSG_ID_FIRST,  

  /* ----------------------LTE OTDOA Messages---------------------- */
  /* OTDOA-ML1 interface */
  C_LOC_LTE_OTDOA_CONTROL_ML1_MEAS_ERROR, /* 11 */
  C_LOC_LTE_OTDOA_CONTROL_ML1_MEAS_RESULTS_AVAIL,   /* 12 */
  C_LOC_LTE_OTDOA_CONTROL_ML1_UPDATE_SERVING_CELL,  /* 13 */

  /* OTDOA-SM interface */
  C_LOC_LTE_OTDOA_CONTROL_SM_START_SESS_CMD,        /* 14 */
  C_LOC_LTE_OTDOA_CONTROL_SM_STOP_SESS_CMD,         /* 15 */
  C_LOC_LTE_OTDOA_CONTROL_SM_REQ_MEAS_RPT_CMD,      /* 16 */
  C_LOC_LTE_OTDOA_CONTROL_SM_UPDATE_TIMEOUT,        /* 17 */
  C_LOC_LTE_OTDOA_CONTROL_SM_UPDATE_FTA_DATA,       /* 18 */
  C_LOC_LTE_OTDOA_CONTROL_SM_OTA_RESET_CMD,         /* 19 */

  /* OTDOA-DIAG interface - obsolete */
  C_LOC_LTE_OTDOA_CONTROL_DIAG_PKT_RECEIVED_OBSOLETE,        /* 20 */

  /* MeasProc-Ctrl task interface */
  C_LOC_LTE_OTDOA_CONTROL_MP_MEAS_PROC_COMPLETE,   /* 21 */
  C_LOC_LTE_OTDOA_CONTROL_MP_HYPOTHESES_COMPLETE,  /* 22 */
  C_LOC_LTE_OTDOA_CONTROL_MP_STOP_CMD_ACK,         /* 23 */
  C_LOC_LTE_OTDOA_CONTROL_MSG_LAST,                /* 24 */

  C_LOC_LTE_OTDOA_CONTROL_MSG_MAX = 99, 

  /* ----------------------LTE TDP Messages---------------------- */
  C_LOC_LTE_TDP_CONTROL_MSG_FIRST = C_LOC_LTE_OTDOA_CONTROL_MSG_MAX,
  /* ML1-OC interface */
  C_LLOC_TDP_ML1_OPCRS_MEAS_RESULTS_AVAIL,   //100

  /* TLM->OC interface */
  C_LLOC_TDP_UPLOAD_ENABLE,   /* 101: TDP Upload enable sent by TLM task */
  C_LLOC_TDP_UPLOAD_DISABLE,  /* 102: TDP Upload disable sent by TLM task */         
  C_LLOC_TDP_MEASBLK_REQ_0,     /* 103: TDP CS meas blk request from TLM task for TDP 1.0 */

  /* OMP -> OC interface */
  C_LLOC_TDP_EAP_CALC_COMPLETE_0, /* 104: Obsolete -TDP Meas response from OMP task */

  /* ME -> OC interface */
  C_LOC_LTE_TDP_GPSFIX_TIME_UPDATE, /* 105: Time Update message from MC Task */
  C_LOC_LTE_TDP_UPLOAD_CONFIG, /* 106: TDP Upload Config from TLM task */
  C_LLOC_TDP_EAP_CALC_COMPLETE, /* 107: TDP Meas response from OMP task */

  C_LLOC_TDP_ML1_WBCRS_MEAS_RESULTS_AVAIL,   //108: WB CRS meas from ML1
  C_LLOC_TDP_MEASBLK_REQ,    // 109: TDP meas req from TLE task for TDP 2.0
  C_LLOC_TDP_ML1_DETECTED_CELLS_0, /* 110: Searcher detected list */
  C_LLOC_TDP_OTDOA_STATE,  /* 111: OTDOA state change info */
  C_LLOC_TDP_INTERNAL_EVT, /* 112: Self IPC to control task */
  C_LLOC_TDP_GPS_TIME_UPDATE, /* 113: Time Update message from GTS */
  C_LLOC_TDP_ML1_DETECTED_CELLS, /* 114: Searcher detected list + blacklist */

  C_LOC_LTE_TDP_CONTROL_MSG_LAST, /* 115 */
  C_LLOC_TDP_MSG_MAX = 200,

  C_LOC_GTS_MSG_FIRST = C_LLOC_TDP_MSG_MAX,
  C_LOC_GTS_MSG_FREQ_ERR_EST,
  C_LOC_GTS_MSG_LTE_TIME,
  C_LOC_GTS_MSG_GPS_POS_REP,
  C_LOC_GTS_MSG_GPS_TIME_REP,
  C_LOC_GTS_MSG_REGISTER,
  C_LOC_GTS_MSG_GPS_TIME_REQ,
  C_LOC_GTS_MSG_HIGH_ACCU_MODE,
  C_LOC_GTS_MSG_SENSOR_REP,
  C_LOC_GTS_MSG_FEST_HAT_STATUS,
  C_LOC_GTS_MSG_DIAG_CMDS,
  C_LOC_GTS_MSG_GSM_TIME,
  C_LOC_GTS_MSG_WCDMA_TIME,
  C_LOC_GTS_MSG_TDSCDMA_TIME,
  C_LOC_GTS_MSG_SS_CHANGE,
  C_LOC_GTS_MSG_TTR_REQ,
  C_LOC_GTS_MSG_CDMA_TTR_RPT,
  C_LOC_GTS_MSG_CDMA_START_FTCAL,
  C_LOC_GTS_MSG_CDMA_STOP_FTCAL,  
  C_LOC_GTS_MSG_CDMA_SCLK_TTR_RPT,  
  C_LOC_GTS_MSG_LTE_TTR_RPT,
  C_LOC_GTS_MSG_LPP_FTA_PUT,
  C_LOC_GTS_MSG_CLK_STOP,
  C_LOC_GTS_MSG_DRSYNC_GET_CONFIG,
  C_LOC_GTS_MSG_DRSYNC_SET_CONFIG,  
  C_LOC_GTS_MSG_DRSYNC_TRIGGER,  
  C_LOC_GTS_MSG_DRSYNC_ARM,
  C_LOC_GTS_MSG_LEAP_SEC_UPDATE,
  C_LOC_GTS_MSG_SLOW_CLK_TIME_UPDATE,
  C_LOC_GTS_MSG_INJECT_ABS_TIME,
  C_LOC_GTS_MSG_DEL_TIME,
  C_LOC_GTS_MSG_PMIC_CORRECTION_TIME,
  C_LOC_GTS_MSG_LAST,
  C_LOC_GTS_MSG_MAX = 300,
  C_LOC_XOFESTRM_MSG_FIRST = C_LOC_GTS_MSG_MAX,
  C_LOC_XOFESTRM_MSG_TEMP_UPDATE,
  C_LOC_XOFESTRM_MSG_CAL_COEFF_UPDATE,
  C_LOC_XOFESTRM_MSG_RPUSH_UPDATE,
  C_LOC_XOFESTRM_MSG_HAT_TEMP_READ_TIMER,
  C_LOC_XOFESTRM_MSG_ME_FUNC_UPDATE,
  C_LOC_XOFESTRM_MSG_LAST,
  C_LOC_XOFESTRM_MSG_MAX = 400
  
}
wwanme_control_IpcMsgEnumType;

#define IS_OS_CONTROL_MSG(x)  ( ( (x) >= C_OS_MSG_ID_FIRST) && \
                          ( (x) < C_USR_MSG_ID_FIRST) ) 


#define IS_LTE_OTDOA_CONTROL_MSG(x)  ( ( (x) > C_LOC_LTE_OTDOA_CONTROL_MSG_FIRST) && \
                          ( (x) < C_LOC_LTE_OTDOA_CONTROL_MSG_LAST) ) 


#define IS_LTE_TDP_CONTROL_MSG(x)  ( ( (x) > C_LOC_LTE_TDP_CONTROL_MSG_FIRST) && \
                           ( (x) < C_LOC_LTE_TDP_CONTROL_MSG_LAST) ) 

#define IS_GTS_MSG(x)  ( ( (x) > C_LOC_GTS_MSG_FIRST) && \
                           ( (x) < C_LOC_GTS_MSG_LAST) ) 

#define IS_XOFESTRM_MSG(x)  ( ( (x) > C_LOC_XOFESTRM_MSG_FIRST) && \
                           ( (x) < C_LOC_XOFESTRM_MSG_LAST) ) 


#define IS_LTE_OTDOA_MEASPROC_MSG(x)  ( ( (x) > C_LOC_LTE_OTDOA_MEASPROC_MSG_FIRST) && \
                          ( (x) < C_LOC_LTE_OTDOA_MEASPROC_MSG_LAST) ) 


#define IS_LTE_TDP_MEASPROC_MSG(x)  ( ( (x) > C_LLOMP_TDP_MSG_FIRST) && \
                           ( (x) < C_LLOMP_TDP_MSG_LAST) ) 


/* DIAG sub-commands allocation */
#define LOC_LTE_OTDOA_DIAG_SUBCMD_START  1
#define LOC_LTE_OTDOA_DIAG_SUBCMD_END    99
#define LOC_LTE_TDP_DIAG_SUBCMD_START    (LOC_LTE_OTDOA_DIAG_SUBCMD_END + 1)
#define LOC_LTE_TDP_DIAG_SUBCMD_END      (LOC_LTE_TDP_DIAG_SUBCMD_START + 99) 


/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
typedef struct
{
  uint32 q_TimerId;
  os_TimerMsgType* p_Timer;
  boolean b_Status;
}
wwanme_control_TimersType;


/*
 ******************************************************************************
 * Function: wwanme_control_StartTimer
 *
 * Description:
 *  Start a timer - can only provide one timer to start
 *
 * Parameters:
 *  None
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
 */
void wwanme_control_StartTimer
(
  uint32 q_TimerId,
  uint32 q_TimeoutMsec
);

/*
 ******************************************************************************
 * Function: wwanme_control_StopTimer
 *
 * Description:
 *  Stop a timer
 *
 * Parameters:
 *  q_TimerId : Timer id to stop
 *
 * Dependencies:
 *  wwanme_control_z_Params.q_TimerMask
 *
 * Return value:
 *  TRUE: Timer stopped successfully
 *  FALSE in case of any error
 *
 ******************************************************************************
 */
boolean wwanme_control_StopTimer
(
  uint32 q_TimerIds
);

/*
 ******************************************************************************
 * Function: wwanme_control_InitTimers
 *
 * Description:
 *  Create Control task's timers
 *
 * Parameters:
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  FALSE in case of any error
 *
 ******************************************************************************
 */
boolean wwanme_control_InitTimers(void);

void wwanme_control_WwanMeNvConfig();

#endif /* WWANME_COMMON_DEFS_H */
