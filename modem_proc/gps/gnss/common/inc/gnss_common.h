/** @file gnss_common.h
   This file contains the structure definitions and function
   prototypes relevant for the entire GPS module (essentially
   the shared structures).
*/

#ifndef GNSS_COMMON_H
#define GNSS_COMMON_H
/*=============================================================================

                             GPS Common Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for the entire GPS module (essentially the shared structures).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2009-2016 by Qualcomm Technologies INCORPORATED. All Rights Reserved.
Copyright (c) 2013-2015 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.

Copyright (c) 2013 - 2021 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/gnss_common.h#2 $
  $DateTime: 2021/09/01 02:25:50 $
  $Author: pwbldsvc $

  when        who      what, where, why
  ----------  ---  -----------------------------------------------------------
  29/04/20   ska     Added changes for asking less proc duration when EPH is valid
  09/24/15     mj       Added changes to delete struct for Eph, Alm and health
  03/20/17     rn       Added support for XTRA Integrity Phase2  
  10/17/17     rn       Added support for XTRA Integrity  
  09/24/15     mj       Added changes to delete struct for Eph, Alm and health
  08/02/16     gurdars  Added changes to reflect the constellation mask for XTRA3.0/3.1 data
  07/15/16     mj       Added enum gnss_AssistDataSourceEnumType to indicate the source of assist data  
  09/24/15     mj       Added changes to delete struct for Eph, Alm and health
  2014-05-07   rk       Enhanced earfcn to support extended value for LTE.
  2012-12-19   rk       Update gnss_NavSolutionStructType struct with sensor (gyro/accel)
                        bias parameter structure.
  2012-12-03   rk       Updated fix report & GPS measurement struct with SV extension parameters.
  2010-01-29   pn       Updated DmLog structs with PACKED_POST.




=============================================================================*/



#include "gps_variation.h"
#ifdef WIN32
#pragma once
#endif

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "gps_types.h"
#include "gnss_consts.h"
#include "gps_common.h"
#include "cgps_api.h"
#include "tcxomgr.h"
#include "trm.h"
#include "sys.h"
#include "queue.h"
#include "qw.h"
#include "cmd.h"
#include "timetick.h"
#include "log.h"
#include "msg.h"
#include <stddef.h>
#include "memheap.h"
#include "err.h"
#include "pdapi.h"
#include "npa.h"

#ifdef FEATURE_CGPS_UMTS_CELLDB
#include "rr_lsm_if.h"
#include "rrclsmif.h"
#endif /* FEATURE_CGPS_UMTS_CELLDB */

/* LTE API available always (even on non-LTE builds) */
#include "lte_as.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/

/*
  The decimator filter bug fix related defines.
  Some of these maybe replace by more global defines.
*/

/* Total Bp3 HW channels */
#define C_MGP_TOTAL_GLO_SUB_CHAN_NUM (12)
/* Unused Bp3 HW channel representation */
#define C_MGP_UNUSED_GLO_SUB_CHAN (-8)
/* HW subchannel assignments for G1 and G2 */
#define C_MGP_GLO_G1_SUB_CHAN_MIN (0)
#define C_MGP_GLO_G1_SUB_CHAN_MAX (5)
#define C_MGP_GLO_G2_SUB_CHAN_MIN (6)
#define C_MGP_GLO_G2_SUB_CHAN_MAX (11)


#define GNSS_LE_FILTER_DEFAULT_PUNC_THRESHOLD  (50.0F)

#define GNSS_LE_FILTER_MASK_QUALITY_OUTDOOR    (0x00000001)
#define GNSS_LE_FILTER_MASK_QUALITY_CHALLENGED (0x00000002)
#define GNSS_LE_FILTER_MASK_QUALITY_INDOOR     (0x00000004)
#define GNSS_LE_FILTER_MASK_CHECK_PUNC         (0x00000008)
/*=============================================================================

       Macros

=============================================================================*/

/* The MEAS_STATUS defines identify the bits store in the q_MeasStatus field
   of the me_Meas data structure. */
#define MEAS_STATUS_GLO_MEANDER_VALID (1<<16) /* TRUE -> GLO Meander Bit Edge Valid */
#define MEAS_STATUS_GLO_TIMEMARK_VLID (1<<17) /* TRUE -> GLO Time mark valid */
/* RxD Measurement Status*/
#define MEAS_STATUS_RXD_MP_HIGH   (1<<18) /* TRUE - GPS Round Robin RxD Measurement*/
#define MEAS_STATUS_RXD_MP_LOW    (1<<19) /* TRUE - GPS  RxD Measurement*/
#define MEAS_STATUS_RXD_COMBINED  (1<<20) /* TRUE - GPS  RxD Combined Measurement*/
#define MEAS_STATUS_MEAS_PROPAGATED (1<<21) /* TRUE - GPS HBW GPS NU4 Measurement */
#define MEAS_STATUS_HBW_GPS_NU8   (1<<22) /* TRUE - GPS HBW GPS NU4 Measurement */
#define MEAS_STATUS_HBW_GPS_U     (1<<23) /* TRUE - GPS HBW GPS Uniform Measurement */
#define MEAS_STATUS_MULTIPATH     (1<<24) /* TRUE - GNSS Measurement Multipath indication */
#define MEAS_STATUS_IMD_JAMMING   (1<<25) /* TRUE - GNSS Measurement IMD Jamming indication. */
#define MEAS_STATUS_LTE_B13_JAMMING (1<< 26) /* TRUE LTE B13 TX jamming */
#define MEAS_STATUS_GNSS_FRESH_MEAS (1<< 27) /* TRUE - Fresh GNSS measurement observed in last second */
#define MEAS_STATUS_SUSPICIOUS_MEAS (1<< 28) /* TRUE - Suspicious measurement observed during LTE B13 TX */
#define MEAS_STATUS_BLACK_LIST_SV   (1<< 29) /* SV is marked as black listed. Do not use this measurement */
/* Additional SV time ambiguities */
#define MEAS_STATUS_100MS_VALID   (1<<30)  /* TRUE -> SV time known with 100 ms ambiguity */
#define MEAS_STATUS_2S_VALID      (1U<<31) /* TRUE -> SV time known with 2 s ambiguity */

/* Max TBF supported (for DPO) is 10 sec + some buffer */
#define C_MAX_TIME_LIMIT_FOR_DPO_OPN  ((FLT)12.0)

#define C_GLO_TB_UNKNOWN          (0)       /*!< default initialization 
                                                 value for unknown GLO Tb */
#define C_GLO_DAY_UNKNOWN         (0xFFFF)  /*!< default initialization 
                                                 value for unknown GLO day */
#define C_GLO_FOUR_YEAR_UNKNOWN   (0xFF)    /*!< default initialization 
                                                 value for unknown GLO 4-year */
#define C_BDS_WEEK_UNKNOWN        (0xFFFF)  /*!< default initialization 
                                                 value for unknown BDS week */
#define C_GAL_WEEK_UNKNOWN        (0xFFFF)  /*!< default initialization 
                                                 value for unknown GAL week */
#define C_GNSS_WEEK_UNKNOWN       (0xFFFF)  /*!< default initialization 
                                                 value for unknown GNSS week */

#define C_MAX_GPSGLOTB_UNC_MS       ((FLT)(0.02))//Changed to 20us (2)
#define C_MAX_ICD_GPSGLOTB_MS       ((FLT)(2.00)) /*  2ms  */
#define C_MAX_GPSBDSTB_UNC_MS       ((FLT)(0.02))
#define C_MAX_ICD_GPSBDSTB_MS       ((FLT)(2.00)) /* TBD */
#define C_MAX_BDSGLOTB_UNC_MS       ((FLT)(0.02))
#define C_MAX_ICD_BDSGLOTB_MS       ((FLT)(2.00)) /* TBD */
#define C_MAX_GPSGALTB_UNC_MS       ((FLT)(0.02))
#define C_MAX_ICD_GPSGALTB_MS       ((FLT)(2.00)) /* TBD */
#define C_MAX_GALGLOTB_UNC_MS       ((FLT)(0.02))
#define C_MAX_ICD_GALGLOTB_MS       ((FLT)(2.00)) /* TBD */
#define C_MAX_GALBDSTB_UNC_MS       ((FLT)(0.02))
#define C_MAX_ICD_GALBDSTB_MS       ((FLT)(2.00)) /* TBD */
#define C_MAX_GPS_LEAP_SEC_UNC      (14)

/* The minimum value of TUNC to enable computation of a WLS SFT fix */
#define C_MIN_SFT_TUNC_MS       (30)

/* Maximum TUNC for time to be used as valid within PE */
#define PE_VALID_TIME_UNC_MS  ((float)60000.0)

#define GNSS_ARRAY_SIZEOF(a) (sizeof(a)/sizeof((a)[0]))

#define GNSS_GEN8_LOG_VERSION                (0)

/* log 0x14DE, 0x14E0, 0x14E1  VERSION 2 identifiers */
#define GNSS_GEN8_LOG_VERSION_OEMDRE_V2                (2)

#define GNSS_GEN8_LOG_VERSION_MODIFIED_GEN7  (50) /* gen 7 logs modified to fit gen 8 */

/* log 0x1516 VERSION 0 identifiers */
#define LOG_GNSS_CONFIGURATION_STATE_VERSION              (0)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_NF      (0)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_SDP     (1)

/* log Ox1516 VERSION 1 identifiers */
#define LOG_GNSS_CONFIGURATION_STATE_VERSION_V1              (1)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_NF_V1      (0)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_SDP_V1     (1)

/* log Ox1516 VERSION 2 identifiers */
#define LOG_GNSS_CONFIGURATION_STATE_VERSION_V2              (2)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_NF_V2      (0)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_SDP_V2     (1)

/* log Ox1516 VERSION 3 identifiers */
#define LOG_GNSS_CONFIGURATION_STATE_VERSION_V3              (3)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_NF_V3      (0)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_SDP_V3     (1)

/* log Ox1516 VERSION 4 identifiers */
#define LOG_GNSS_CONFIGURATION_STATE_VERSION_V4              (4)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_NF_V4      (0)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_SDP_V4     (1)

/* log Ox1516 VERSION 5 identifiers */
#define LOG_GNSS_CONFIGURATION_STATE_VERSION_V5              (5)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_NF_V5      (0)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_SDP_V5     (1)

/* log Ox1516 VERSION 6 identifiers */
#define LOG_GNSS_CONFIGURATION_STATE_VERSION_V6              (6)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_NF_V6      (0)
#define LOG_GNSS_CONFIGURATION_STATE_SUBSYSTEM_ID_SDP_V6     (1)
/* add other subsystem ids here for log LOG_GNSS_CONFIGURATION_STATE */

/* update the latest version info */
#define LOG_GNSS_CONFIGURATION_STATE_VERSION_LATEST \
  LOG_GNSS_CONFIGURATION_STATE_VERSION_V6

/* Define the bits which control Extended Receiver ON operation 
   Extended Receiver ON feature is controlled by NV item NV_GNSS_ENABLE_EXTENDED_ON_FEATURE_FOR_GNSS_I. 
*/

#define C_GNSS_EXTENDED_RCVR_ON_FEATURE_ENABLE_GPS_DATA_DECODE   (1<<0)  /* Enables Extended Recevier ON feature for GPS Nav data */
#define C_GNSS_EXTENDED_RCVR_ON_FEATURE_ENABLE_GLO_DATA_DECODE   (1<<1)  /* Enables Extended Recevier ON feature for GLONASS Nav data */
#define C_GNSS_EXTENDED_RCVR_ON_FEATURE_ENABLE_SBAS_DATA_DECODE  (1<<2)  /* Enables Extended Recevier ON feature for SBAS Nav data (reserved) */
#define C_GNSS_EXTENDED_RCVR_ON_FEATURE_ENABLE_QZSS_DATA_DECODE  (1<<3)  /* Enables Extended Recevier ON feature for QZSS Nav data (reserved) */
#define C_GNSS_EXTENDED_RCVR_ON_FEATURE_ENABLE_BDS_DATA_DECODE   (1<<4)  /* Enables Extended Recevier ON feature for BEIDOU Nav data (reserved) */
#define C_GNSS_EXTENDED_RCVR_ON_FEATURE_ENABLE_GAL_DATA_DECODE   (1<<5)  /* Enables Extended Recevier ON feature for GALILEO Nav data (reserved) */
#define C_GNSS_EXTENDED_RCVR_ON_FEATURE_ENABLE_TIMETAG_UPDATE    (1<<6)  /* Enables Extended Recevier ON feature for time-tag update */

/*
 * Default NV Item Values for XTRA
 */
#define GNSS_NV_DEFAULT_XTRA_ENABLED          (1)
#define GNSS_NV_DEFAULT_XTRA_AUTO_DL_DISABLED (0)

#define C_BDS_TIMEOFFSET_GPS_VALID (0x01)
#define C_BDS_TIMEOFFSET_GLO_VALID (0x02)
#define C_BDS_TIMEOFFSET_GAL_VALID (0x04)

#define C_GAL_TIMEOFFSET_GPS_VALID (0x01)
#define C_GAL_TIMEOFFSET_GLO_VALID (0x02)
#define C_GAL_TIMEOFFSET_BDS_VALID (0x04)

/* Possible sources for Leap Second */
#define   C_LS_SRC_INVALID      (0) /* Not valid or from hard coded */
#define   C_LS_SRC_OTA_UTC      (1) /* Decoded */
#define   C_LS_SRC_XTRA_UTC     (2) /* Downloaded */
#define   C_LS_SRC_PE_FIX       (3) /* Calculated by PE */
#define   C_LS_SRC_MC_CALC      (4) /* Calculated by ME */
#define   C_LS_SRC_UTC_NV       (5) /* From Non-Volatile memory */

/* Possible UTC sources */
#define C_UTC_SRC_INVALID       (0) /* Not valid */
#define C_UTC_SRC_XTRA1         (1) /* XTRA1 */
#define C_UTC_SRC_XTRA2         (2) /* XTRA2 */
#define C_UTC_SRC_XTRA3         (3) /* XTRA3 */
#define C_UTC_SRC_XTRA3_QZSS    (4) /* XTRA3 QZSS */
#define C_UTC_SRC_OTA           (5) /* GPS OTA decoded */
#define C_UTC_SRC_OTA_GAL       (6) /* GAL OTA decoded */
#define C_UTC_SRC_AGNSS         (7) /* AGNSS injected */
#define C_UTC_SRC_EFS           (8) /* from EFS */

/* Bit Mask for Receiver GNSS Configuration */
#define C_RCVR_GNSS_CONFIG_GPS_ENABLED                (1<<0)
#define C_RCVR_GNSS_CONFIG_GLO_ENABLED                (1<<1)
#define C_RCVR_GNSS_CONFIG_BDS_ENABLED_OUTSIDE_OF_US  (1<<2) /* Used only for GNSS Config NV set and get */
#define C_RCVR_GNSS_CONFIG_GAL_ENABLED_OUTSIDE_OF_US  (1<<3)
#define C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE      (1<<4) /* Used only for GNSS Config NV set and get */
#define C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE      (1<<5)
#define C_RCVR_GNSS_CONFIG_QZSS_ENABLED_WORLDWIDE     (1<<6) /* Used only for GNSS Config NV set and get */
#define C_RCVR_GNSS_CONFIG_HBW_ENABLED                (1<<7)

/* QZSS TBD -PS there is no final decision on QZSS enabling bits. 
   Note, MC uses internally 8 bit word presenting GNSS configuration but NV item has 32 bits.*/
#define C_RCVR_GNSS_CONFIG_QZSS_ENABLED_OUTSIDE_OF_US (1<<8) /* Used only for GNSS Config NV set and get */
#define C_RCVR_GNSS_CONFIG_BDS_ENABLED (C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE | C_RCVR_GNSS_CONFIG_BDS_ENABLED_OUTSIDE_OF_US) /* Use this definition for BDS capablity of receiver */
#define C_RCVR_GNSS_CONFIG_GAL_ENABLED (C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE | C_RCVR_GNSS_CONFIG_GAL_ENABLED_OUTSIDE_OF_US) /* Use this definition for GAL capablity of receiver */
#define C_RCVR_GNSS_CONFIG_QZSS_ENABLED (C_RCVR_GNSS_CONFIG_QZSS_ENABLED_WORLDWIDE) /* Use this definition for QZSS capablity of receiver (8 bit word) */


/* Gen9 VT-V2 has RF-BP configuration bits defined as below in bits 0 through 3. This is the externally facing NV definition. */
#define C_RCVR_GNSS_NV_CONFIG_RFBP_MODE_MASK               0xF

#define C_RCVR_GNSS_NV_CONFIG_L2_G2_B2_DISABLED            0   /* Corresponds to config0 in MB SRS Document (L1 only)  */

/* Value 1 is skipped here since config1 is chosen based on L5/E5 shown down below */
#define C_RCVR_GNSS_NV_CONFIG_L2_G2_ENABLED                2   /* Corresponds to config2 in MB SRS Document. No B2 */
#define C_RCVR_GNSS_NV_CONFIG_L2_B2_ENABLED                3   /* Corresponds to config3 in MB SRS Document. No G2 */
#define C_RCVR_GNSS_NV_CONFIG_B2_G2_ENABLED                4   /* Corresponds to config4 in MB SRS Document. No L2 */
/* Enums 5~15 are reserved */

/* Some more configuration bits in 4 and 5 */
#define C_RCVR_GNSS_NV_CONFIG_L5_E5_ENABLED              (1<<4)
#define C_RCVR_GNSS_NV_CONFIG_HIGH_ACCURACY_DISABLED     (1<<5) /* High Accuracy is L2, G2, B2 Wideband, not L5 and E5 */

/* Enum definition for the bit mask immediately following this (C_RCVR_GNSS_CONFIG_L1_WB_ENABLED). This definition 
 * is used by CC and MC for configuration. Each band has it's own bit, giving CC and MC full flexibility to add or 
 * change this in future.
 */
typedef enum
{
  C_GNSS_MULTIBAND_TYPE_L1WB,
  C_GNSS_MULTIBAND_TYPE_L2NB,
  C_GNSS_MULTIBAND_TYPE_L2WB,
  C_GNSS_MULTIBAND_TYPE_G1WB,
  C_GNSS_MULTIBAND_TYPE_G2NB,
  C_GNSS_MULTIBAND_TYPE_G2WB,
  C_GNSS_MULTIBAND_TYPE_B1WB,
  C_GNSS_MULTIBAND_TYPE_B2NB,
  C_GNSS_MULTIBAND_TYPE_B2WB,
  C_GNSS_MULTIBAND_TYPE_L5,
  C_GNSS_MULTIBAND_TYPE_E5A,
  C_GNSS_MULTIBAND_TYPE_ERROR
} gnss_MultibandType;

#define C_RCVR_GNSS_MULTIBAND_CONFIG_MASK             ( ( (uint32)1 << C_GNSS_MULTIBAND_TYPE_ERROR) - 1)

/* "Flat bit mask" for use to reconfigure CC at CC enable and during a session. 0 value is to disable WB */
/* This bit format is also used in NV74054. If this undergoes changes, update the NV definition */
#define C_RCVR_GNSS_CONFIG_L1_WB_ENABLED              (0 << C_GNSS_MULTIBAND_TYPE_L1WB)
#define C_RCVR_GNSS_CONFIG_L2_NB_ENABLED              (1 << C_GNSS_MULTIBAND_TYPE_L2NB)
#define C_RCVR_GNSS_CONFIG_L2_WB_ENABLED              (0 << C_GNSS_MULTIBAND_TYPE_L2WB)
#define C_RCVR_GNSS_CONFIG_G1_WB_ENABLED              (0 << C_GNSS_MULTIBAND_TYPE_G1WB)
#define C_RCVR_GNSS_CONFIG_G2_NB_ENABLED              (1 << C_GNSS_MULTIBAND_TYPE_G2NB)
#define C_RCVR_GNSS_CONFIG_G2_WB_ENABLED              (0 << C_GNSS_MULTIBAND_TYPE_G2WB)
#define C_RCVR_GNSS_CONFIG_B1_WB_ENABLED              (0 << C_GNSS_MULTIBAND_TYPE_B1WB)
#define C_RCVR_GNSS_CONFIG_B2_NB_ENABLED              (1 << C_GNSS_MULTIBAND_TYPE_B2NB)
#define C_RCVR_GNSS_CONFIG_B2_WB_ENABLED              (0 << C_GNSS_MULTIBAND_TYPE_B2WB)
#define C_RCVR_GNSS_CONFIG_L5_ENABLED                 (1 << C_GNSS_MULTIBAND_TYPE_L5)
#define C_RCVR_GNSS_CONFIG_E5A_ENABLED                (1 << C_GNSS_MULTIBAND_TYPE_E5A)
#define C_RCVR_GNSS_CONFIG_ERROR                      (1 << C_GNSS_MULTIBAND_TYPE_ERROR)

#define C_RCVR_GNSS_NV_DEBUG_CONFIG_RFBP_CONFIG_BIT_POS       27
#define C_RCVR_GNSS_NV_DEBUG_CONFIG_RFBP_CONFIG_NUM_BITS       4       
#define C_RCVR_GNSS_NV_DEBUG_CONFIG_ENABLE_NV_BIT_POS         31



/* Combine some of the often-used band combinations for cleaner looking code */
#define C_RCVR_GNSS_CONFIG_L2_G2_NB_ENABLED           (C_RCVR_GNSS_CONFIG_L2_NB_ENABLED|C_RCVR_GNSS_CONFIG_G2_NB_ENABLED)
#define C_RCVR_GNSS_CONFIG_L2_B2_NB_ENABLED           (C_RCVR_GNSS_CONFIG_L2_NB_ENABLED|C_RCVR_GNSS_CONFIG_B2_NB_ENABLED)
#define C_RCVR_GNSS_CONFIG_B2_G2_NB_ENABLED           (C_RCVR_GNSS_CONFIG_B2_NB_ENABLED|C_RCVR_GNSS_CONFIG_G2_NB_ENABLED)
#define C_RCVR_GNSS_CONFIG_ALL_L2_NB_ENABLED          (C_RCVR_GNSS_CONFIG_L2_NB_ENABLED|C_RCVR_GNSS_CONFIG_B2_NB_ENABLED |C_RCVR_GNSS_CONFIG_G2_NB_ENABLED)
#define C_RCVR_GNSS_CONFIG_ALL_L2_L5_ENABLED          (C_RCVR_GNSS_CONFIG_ALL_L2_NB_ENABLED|C_RCVR_GNSS_CONFIG_L5_ENABLED|C_RCVR_GNSS_CONFIG_E5A_ENABLED)
#define C_RCVR_GNSS_CONFIG_L2_G2_L5_NB_ENABLED        (C_RCVR_GNSS_CONFIG_L2_G2_NB_ENABLED |C_RCVR_GNSS_CONFIG_L5_ENABLED)


/* Bit0 in a separate config variable is provided to CC at CC Enable saying if L5, E5A is needed at IMEM entry */
#define C_RCVR_GNSS_CONFIG_IMEM_L5E5A_ENABLED         (1 << 0)

/* All Constellations are enabled worldwide */
#define C_RCVR_GNSS_CONFIG_ALL_ENABLED_WORLDWIDE (C_RCVR_GNSS_CONFIG_GPS_ENABLED           |\
                                                  C_RCVR_GNSS_CONFIG_GLO_ENABLED           |\
                                                  C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE |\
                                                  C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE |\
                                                  C_RCVR_GNSS_CONFIG_QZSS_ENABLED_WORLDWIDE )

/* Bit Mask for Constellation configuration Control */
#define C_GNSS_CONSTEL_CFG_GPS    (1<<0)
#define C_GNSS_CONSTEL_CFG_GLO    (1<<1)
#define C_GNSS_CONSTEL_CFG_BDS    (1<<2) 
#define C_GNSS_CONSTEL_CFG_GAL    (1<<3)
#define C_GNSS_CONSTEL_CFG_QZSS   (1<<4)
#define C_GNSS_CONSTEL_CFG_ALL  ( C_GNSS_CONSTEL_CFG_GPS | \
                                  C_GNSS_CONSTEL_CFG_GLO | \
                                  C_GNSS_CONSTEL_CFG_BDS | \
                                  C_GNSS_CONSTEL_CFG_GAL | \
                                  C_GNSS_CONSTEL_CFG_QZSS )

#define   C_GLOUTC_SRC_INVALID (0) /* Not valid */
#define   C_GLOUTC_SRC_OTA     (1) /* over-the-air decoded */
#define   C_GLOUTC_SRC_XTRA    (2) /* XTRA file */
#define   C_GLOUTC_SRC_ASSIST  (3) /* AGLO server */

#define BOUND(a, min, max) {if (a < (((min)))) {a = (min);} if (a > (max)) {a = (max);}}

#define C_GNSS_MAX_USTMR_LATCH       (1)
/* Measurement status bitmask for not-to-use measurements in PE */
#define C_PE_MEAS_STATUS_DONT_USE ( MEAS_STATUS_ACQ_OR_PROBATION | MEAS_STATUS_FROM_RNG_DIFF | \
  MEAS_STATUS_DONT_USE | MEAS_STATUS_XCORR | MEAS_STATUS_NEED_SIR_CHECK )
#define C_SBAS_INVALID_AREA_NODE (127)
#define GNSS_NEW_GLO_TYPE_AVAILABLE  (1) /* GLO M or newer is available */
/* Maximum Iono grid points supported in BDS */
#define C_BDS_MAX_IONO_GRID_POINTS      (320)

/* NHz related bit masks */
#define GNSS_NHZ_ENABLED         (1<<0)
#define GNSS_NHZ_MODE_AUTOMOTIVE (1<<1)
#define GNSS_NHZ_MODE_DRONE      (1<<2)

/* Injection type used to distinguish between XTRA and XTRA Integrity */
/* injections.                                                        */
#define INJECTION_TYPE_XTRA           (0)
#define INJECTION_TYPE_XTRA_INTEGRITY (1) 

/* XTRA Integrity file will have up to 37 BDS SV, but mobile N_BDS_SV is 30 */
#define TM_XTRAINT_N_BDS_SV                    (37)  
#define XTRAINT_N_ALL_GNSS_SV                  ((N_ALL_GNSS_SV - N_BDS_SV) + TM_XTRAINT_N_BDS_SV) /* 134 */
#define XTRAINT_MB_N_ALL_GNSS_SV               (2*XTRAINT_N_ALL_GNSS_SV + N_GPS_SV + N_QZSS_SV)
                                                /* L1+L2 for all including GAL E5a
                                                L5 for GPS and QZSS: total 305 */
#define XTRAINT_MAX_MB_EVENT_RECORDS           (2*XTRAINT_MB_N_ALL_GNSS_SV) /* 610 */

/* See Beidou ICD Table 5-15:  
- LSB Bit 1 set (Nav Message Bad (IOD over limit)
- LSB Bit 8 set (Satellite clock not ok) */
#define BDS_HEALTH_PAGE_BAD 258

/*============================================================================= 


       Typedefs

=============================================================================*/
/* Typedef struct for all the response/request from Searcher to PDSM.
*/

#if defined(WIN32) && defined(ADS_120_COMPILER_4_BYTE_ALIGNMENT)
#pragma pack(push, GNSS_COMMON_H, 4)
#endif

/*=============================================================================
   GNSS Clock structure

   The gnsss_Clock structure is used to describe the clock state in a
   number of other data structures. 

=============================================================================*/
/*struct to store the UTC time for NMEA messages */
typedef struct
{
  uint32 q_FullMsecs; /* UTC full millisecond */
  /* The following fields are derived from q_FullMsecs */
  uint32 q_Hrs;       /* Hours */
  uint32 q_Mins;      /* Minutes */
  FLT f_Secs;         /* Seconds */
} gnss_NmeaUtcTimeStructType;
/**
  @brief Glonass time data structure.  
*/
typedef struct
{
  uint8                 e_SourceOfTime; /*!< Source of the time */
  uint8                 u_FourYear;     /*!< GLO 4 year number from 1996 at the reference tick*/
  uint16                w_Days;         /*!< GLO day number in 4 years at the reference tick*/
  uint32                q_GloMsec;      /*!< GLO msec in the day at reference tick [msecs] */
  FLT                   f_ClkTimeBias;  /*!< Clock bias [msecs] */
  FLT                   f_ClkTimeUncMs; /*!< 1 sided max time bias uncertainty [msecs] */
} glo_TimeStructType;

/**
  @brief Beidou time data structure. 
*/
typedef struct
{
  uint8  e_SourceOfTime; /*!< Source of the time */
  uint16 w_BdsWeek;      /*!< BDS week number at reference tick [weeks] */
  uint32 q_BdsMsec;      /*!< BDS msec at reference tick [msecs] */
  FLT    f_ClkTimeBias;  /*!< Clock bias [msecs]. Constrait to >-0.5 and <0.5 */
  FLT    f_ClkTimeUncMs; /*!< 1 sided max time bias uncertainty [msecs] */
} bds_TimeStructType;

/**
  @brief GPS time data structure 
*/
typedef struct
{
  uint8  e_SourceOfTime; /*!< Source of the time */
  uint16 w_GalWeek;      /*!< GAL week number at reference tick [weeks] */
  uint32 q_GalMsec;      /*!< GAL msec at reference tick [msecs] */
  FLT    f_ClkTimeBias;  /*!< Clock bias [msecs]. Constrait to >-0.5 and <0.5 */
  FLT    f_ClkTimeUncMs; /*!< 1 sided max time bias uncertainty [msecs] */
} gal_TimeStructType;

/**
  @brief Beidou time offset data structure. 
  Contains the decoded time offset parameters for Beidou against 
  GPS, GLO and GAL 
*/
typedef struct
{
  /* GPS-related offsets */
  uint16  w_A0Gps;        /*!< BDT clock bias relative to GPS time (14 bits) [nsec]*/
  uint16  w_A1Gps;        /*!< BDT clock rate relative to GPS time (16 bits) [nsec/sec]*/

  /*GLO-related offsets */
  uint16  w_A0Glo;        /*!< BDT clock bias relative to GLO time (14 bits) [nsec] */
  uint16  w_A1Glo;        /*!< BDT clock rate relative to GLO time (16 bits) [nsec/sec]*/

  /*GAL-related offsets */
  uint16  w_A0Gal;        /*!< BDT clock bias relative to GAL time */
  uint16  w_A1Gal;        /*!< BDT clock rate relative to GAL time */

  uint8   u_TimeOffsetValidMask; /*!< bits:  0 -- GPS-related offset is valid 
                                             1 -- GLO-related offset is valid 
                                             2 -- GAL-related offset is valid 
                                            3-7 -- unused */
  uint32  q_Sow;          /*!< BDS milliseconds of the week */
  boolean b_Valid;
} bds_TimeOffsetStructType;

/**
  @brief Struct for a Inter GNSS time bias 
*/
typedef struct
{
  FLT f_TBMs;           /*!< Total/observed time bias (RFGD + system delay) between systems */
  FLT f_TBUncMs;        /*!< Uncertainty for TB */
  FLT f_GrpDelayNs;     /*!< RF Group delay difference in nanoseconds between systems */
  FLT f_GrpDelayUncNs;  /*!< Uncertainty of RF Group Delay difference in ns */
  FLT f_TimeDiffNs;     /*!< Time difference between systems */
  FLT f_TimeDiffUncNs;  /*!< Time difference uncertainty in nano seconds */
} gnss_SingleInterGnssTimeBiasStructType;

/**
  @brief Struct for leap second parameters and Inter GNSS time 
         biases
*/
typedef struct
{
  uint8   u_DeltaLeapSec;        /*!< GPS time leap second delta to UTC time */
  uint8   u_DeltaLeapSecUnc;     /*!< Uncertainty for GPS leap second */
  uint8   u_DeltaLeapSecSrc;     /*!< Source of Leap second, see LS definition above */

  gnss_SingleInterGnssTimeBiasStructType z_GpsGlo; /*!< GPS to GLONASS time bias */
  gnss_SingleInterGnssTimeBiasStructType z_GpsBds; /*!< GPS to BDS time bias     */
  gnss_SingleInterGnssTimeBiasStructType z_BdsGlo; /*!< BDS to GLONASS time bias */
  gnss_SingleInterGnssTimeBiasStructType z_GpsGal; /*!< GPS to GAL time bias     */
  gnss_SingleInterGnssTimeBiasStructType z_GalGlo; /*!< GAL to GLONASS time bias */
  gnss_SingleInterGnssTimeBiasStructType z_GalBds; /*!< GAL to BDS time bias     */
} gnss_InterGnssTimeBias;  /* Inter GNSS time bias */

/**
  @brief Struct for GNSS times  
*/
typedef struct
{
  gps_TimeStructType        z_GpsTime;      /*!< GPS Time at w_RefFCount */
  glo_TimeStructType        z_GloTime;      /*!< GLO Time at the reference count */
  bds_TimeStructType        z_BdsTime;      /*!< BDS Time at the reference count */
  gal_TimeStructType        z_GalTime;      /*!< GAL Time at the reference count */
  gnss_InterGnssTimeBias    z_GnssTimeBias; /*!< Inter GNSS time bias structure */
} gnss_TimeStructType;

typedef gps_FreqStructType  gnss_FreqStructType;  /*!<  Frequency components are common for all GNSS system */

/**
  @brief Struct for GNSS times ans frequency  
*/
typedef struct
{
  boolean             u_SystemRtcValid; /*! System RTC validity */
  uint32              q_RefFCount;      /*!< FCount value at reference tick */
  uint32              q_NumClockResets; /*!< Cumulative # of Clock resets. */
  uint64              t_SystemRtcMs;    /*!< System RTC value at clock validity */
  gnss_TimeStructType z_Time;   /*!< Time at q_RefFCount */
  gnss_FreqStructType z_Freq;   /*!< Freq at q_RefFCount */
} gnss_ClockStructType;

/* 
 *  Source of Slow Clock request 
 */
typedef enum
{
  GNSS_SCLK_REQ_REASON_DONT_CARE, /* request reason is not used */
  GNSS_SCLK_REQ_REASON_PE_ZPP,  /* Time request during a ZPP session */
  GNSS_SCLK_REQ_REASON_PE_ASSIST, /* Assitance status request from SM */
  GNSS_SCLK_REQ_REASON_PE_XTRA, /* Time request during an XTRA download */
  GNSS_SCLK_REQ_REASON_LPPM, /* Time request during an LPPM session */
  GNSS_SCLK_REQ_MAX = GNSS_SCLK_REQ_REASON_LPPM,
} gnss_SlowClkReqReasonEnumTypeVal;
typedef uint32 gnss_SlowClkReqReasonEnumType; /* Populated using gnss_SlowClkReqReasonEnumTypeVal type */

typedef struct
{
  gnss_ClockStructType z_SlowClkTime;
  gnss_SlowClkReqReasonEnumType e_ReqReason;
} gnss_ConsSlowClkStructType;

/* 
*   SBAS Ref Sv info 
*/
typedef enum
{
  PE_SBAS_CORR_NOT_VALID = 0,
  PE_SBAS_CORR_INTER_STATE,
  PE_SBAS_CORR_VALID
} gnss_SbasSvStatusEnumtype;

typedef struct
{
  uint16 w_GeoPrn;
  uint16 w_Cn0;
} gnss_SbasSvStructType;

typedef struct
{
  gnss_SbasSvStatusEnumtype e_SbasSvState;
  uint16 u_SbasSvCnt;
  gnss_SbasSvStructType z_SbasSvInfo[N_ACTIVE_CHAN];
} gnss_SbasSvStatusStructType;

/* 
===============================================================================
  GNSS Slow Clock Type. 

================================================================================ 
*/

/*
 * Type of GNSS time scale used
 */
typedef enum
{
  GNSS_SLOW_CLOCK_SRC_NONE = 0,
  GNSS_SLOW_CLOCK_SRC_GPS,
  GNSS_SLOW_CLOCK_SRC_UTC,
  GNSS_SLOW_CLOCK_SRC_GLONASS,
  GNSS_SLOW_CLOCK_SRC_BDS
}gnss_SlowClkSrcEnumTypeVal;
typedef uint8 gnss_SlowClkSrcEnumType;

/*
 *  gps_SlowClkTimeInjectionType -
 *
 *  This data structure is used to hold injected time when passed into
 *  the gps_SlowClkExtTimePut() function.
 */
typedef struct
{
  /*
   *  IF source is HOST, then this field is "relative" not absolute time
   *  IF source is network, then this is milliseconds since 00:00:00
   *    Jan 6, 1980
   */
  uint64                          t_Milliseconds;

  /*
   *  Time Uncertainty in milliseconds
   *
   */
  uint32                          q_Uncertainty;

  /*
   *  Time Injection Source
   */
  gps_SlowClkExtTimeSourceType    e_TimeSource;

  /*
   *  Source of the time type
   */
  gnss_SlowClkSrcEnumType         e_TimeSrcType;
  /*
   *  TRUE => Time Base is GPS
   *  FALSE => Time Base is UTC
   *
   *  NOTE - This field is only applicable if e_TimeSource is "NETWORK"
   */
  boolean                         b_TimeIsGps;
  mgp_GnssType                    e_MgpGnssType;

  /*
   *  If TRUE, then force slow clock "time" to equal the injected time.
   */
  boolean                         b_ForceFlag;

  /*
   *  If the source is Host (ie. relative time), then it is very important to
   *  know about any discontinuities.  Signal them with this counter.  
   *  Each discontinuity should cause the Cnt to be incremented.
   */
  uint8                           u_DiscontinuityCnt;
} gnss_SlowClkTimeInjectionType;




/*=============================================================================
   SV Observation State enumeration
=============================================================================*/
typedef enum
{
  C_GNSS_SV_STATE_IDLE,
  C_GNSS_SV_STATE_SEARCH,
  C_GNSS_SV_STATE_SEARCH_VERIFY,
  C_GNSS_SV_STATE_BIT_EDGE,
  C_GNSS_SV_STATE_VERIFY_TRACK,
  C_GNSS_SV_STATE_TRACK,
  C_GNSS_SV_STATE_RESTART,
  C_GNSS_SV_STATE_DPO_TRACK,
  C_GNSS_SV_STATE_GLO_10MSEC_BIT_EDGE,
  C_GNSS_SV_STATE_IMD_SCAN,
  C_GNSS_SV_STATE_LPPM_SEARCH,
  C_GNSS_SV_STATE_MAX

} gnss_SvStateEnumTypeVal;
typedef uint8 gnss_SvStateEnumType;

/*=============================================================================
   GNSS Engine Type Enum
=============================================================================*/
typedef enum
{
  C_GNSS_ENGINE_TYPE_UNKNOWN = 0,
  C_GNSS_ENGINE_TYPE_Gen6c,
  C_GNSS_ENGINE_TYPE_Gen6w,
  C_GNSS_ENGINE_TYPE_Gen7,
  C_GNSS_ENGINE_TYPE_Gen8,
  C_GNSS_ENGINE_TYPE_Gen8a,
  C_GNSS_ENGINE_TYPE_Gen8b,
  C_GNSS_ENGINE_TYPE_Gen8c,
  C_GNSS_ENGINE_TYPE_QCA1530,
  C_GNSS_ENGINE_TYPE_Gen9,
  C_GNSS_ENGINE_TYPE_MAX = 0xffffffff

} gnss_EngineEnumType;


/*=============================================================================
   GNSS SV Time structure
   This is the fundamental component of measurement time and Steering.
   It is defined in units of msecs to facilitate easy
   independent manipulation of the epoch and sub epoch components of time.
=============================================================================*/
typedef struct
{
  FLT f_SubMs; /* Range of 0 thru 0.99999 [msecs] */
  uint32 q_Ms; /* GPS: Range of 0 thru (WEEK_MSECS-1) [msecs] */
  /*  GLO: Range of 0 thru (DAY_MSECS-1)  [msecs] */

} gnss_SvTime;

/*=============================================================================
   GNSS SV Time structure
   This is the fundamental component of measurement time and Steering.
   It is defined in units of msecs to facilitate easy
   independent manipulation of the epoch and sub epoch components of time.
=============================================================================*/
typedef struct
{
  gnss_SvTime z_SvTime;            /* SV Time information */
  FLT         f_SvTimeUncMs;       /* Time uncertainty (msec) */
  FLT         f_SvSpeed;           /* Speed estimate (meters/sec) */
  FLT         f_SvSpeedUnc;        /* Speed uncertainty estimate (meters/sec) */

} gnss_TimeSpeedStructType;

/*=============================================================================
   GNSS Measurement Block structure

=============================================================================*/
typedef struct
{
  uint16  w_Sv;                /* GPS/BDS/GAL SVPRN or GLONASS SV Slot Id */
  int8    b_FreqNum;           /* GLONASS frequency number + 7 (1 - 14)  */
  uint32  q_SvState;           /* SV Observation state, use gnss_SvStateEnumType to fill the field */
  uint8   u_Observe;           /* Count of all observation (both success and failure) */
  uint8   u_GoodObs;           /* Count of Good observations */
  uint8   u_FilterN;           /* Carrier to Code filtering N count */
  uint8   u_PreInt;            /* Pre-Detection (Coherent) Interval (msecs) */
  uint8   u_CSlipCount;        /* Increments when a CSlip is detected */
  uint16  w_PostInt;           /* Num Post-Detections (uints of PreInts) */
  uint32  q_MeasStatus;        /* Bit mask indicating measurement status */
  uint32  q_MeasStatus2;       /* 2nd bit mask indicating measurement status */
  uint16  w_Cno;               /* CNo. Units of 0.1 dB */
  uint16  w_RfLoss;            /* GLO Rf loss reference to Antenaa, units of .1 dB */
  int32   l_LatencyMs;         /* Age of the measurement in msecs (+ve meas Meas precedes ref time) */

  gnss_TimeSpeedStructType z_FiltMeas;    /* Filtered measurement */
  gnss_TimeSpeedStructType z_UnfiltMeas;  /* Un-filtered measurement */

  boolean u_MultipathEstValid; /* TRUE is Multipath estimate is valid */
  FLT     f_MultipathEst;      /* Multipath estimate (meters) */
  boolean u_SvDirValid;        /* SV direction validity flag */
  FLT     f_Azimuth;           /* Azimuth (radians) */
  FLT     f_Elevation;         /* Elevation (radians) */
  FLT     f_DopplerAccel;      /* Acceleration in Hz/Sec */
  FLT     f_FineSpeed;         /* Carrier phase derived speed */
  FLT     f_FineSpeedUnc;      /* Carrier phase derived speed UNC */
  DBL     d_CarrierPhase;      /* Carrier phase measurement [L1 cycles] */
  uint32  q_FCount;

  uint16  w_ParityErrorCount;  /* The 10 LSB to indicate parity errors in the 10 words of current subframe
                                  1: error  0: no error
                                  bit0 for word 1, …, bit9 for word 10  */
  uint8   u_GoodParity;        /* Provides indication whether any parity errors have been observed in 
                                  the last second for each GPS SV.  1:Good Parity 0:Bad Parity*/

} gnss_MeasStructType;

typedef struct
{
  uint16  w_Sv;                /* GPS SVPRN or GLONASS SV Slot Id */
  int8    b_FreqNum;           /* GLONASS frequency number + 7 (1 - 14)  */
  uint32  q_SvState;           /* SV Observation state, use gnss_SvStateEnumType to fill the field */
  uint8   u_Observe;           /* Count of all observation (both success and failure) */
  uint8   u_GoodObs;           /* Count of Good observations */
  uint8   u_FilterN;           /* Carrier to Code filtering N count */
  uint8   u_PreInt;            /* Pre-Detection (Coherent) Interval (msecs) */
  uint8   u_CSlipCount;        /* Increments when a CSlip is detected */
  uint16  w_PostInt;           /* Num Post-Detections (uints of PreInts) */
  uint32  q_MeasStatus;        /* Bit mask indicating measurement status */
  uint16  w_Cno;               /* CNo. Units of 0.1 dB */
  uint16  w_RfLoss;            /* GLO Rf loss reference to Antenaa, units of .1 dB */
  int32   l_LatencyMs;         /* Age of the measurement in msecs (+ve meas Meas precedes ref time) */

  gnss_TimeSpeedStructType z_FiltMeas;    /* Filtered measurement */
  gnss_TimeSpeedStructType z_UnfiltMeas;  /* Un-filtered measurement */

  boolean u_MultipathEstValid; /* TRUE is Multipath estimate is valid */
  FLT     f_MultipathEst;      /* Multipath estimate (meters) */
  boolean u_SvDirValid;        /* SV direction validity flag */
  FLT     f_Azimuth;           /* Azimuth (radians) */
  FLT     f_Elevation;         /* Elevation (radians) */
  FLT     f_DopplerAccel;      /* Acceleration in Hz/Sec */
  FLT     f_FineSpeed;         /* Carrier phase derived speed */
  FLT     f_FineSpeedUnc;      /* Carrier phase derived speed UNC */
  DBL     d_CarrierPhase;      /* Carrier phase measurement [L1 cycles] */
  uint32  q_FCount;

} gnss_L2L5MeasStructType;

typedef struct
{
  uint16  w_Sv;                /* GPS SVPRN or GLONASS SV Slot Id */
  int8    b_FreqNum;           /* GLONASS frequency number + 7 (1 - 14)  */
  uint32  q_SvState;           /* SV Observation state, use gnss_SvStateEnumType to fill the field */
  uint8   u_Observe;           /* Count of all observation (both success and failure) */
  uint8   u_GoodObs;           /* Count of Good observations */
  uint8   u_FilterN;           /* Carrier to Code filtering N count */
  uint8   u_PreInt;            /* Pre-Detection (Coherent) Interval (msecs) */
  uint8   u_CSlipCount;        /* Increments when a CSlip is detected */
  uint16  w_PostInt;           /* Num Post-Detections (uints of PreInts) */
  uint32  q_MeasStatus;        /* Bit mask indicating measurement status */
  uint16  w_Cno;               /* CNo. Units of 0.1 dB */
  uint16  w_RfLoss;            /* GLO Rf loss reference to Antenaa, units of .1 dB */
  int32   l_LatencyMs;         /* Age of the measurement in msecs (+ve meas Meas precedes ref time) */

  gnss_TimeSpeedStructType z_FiltMeas;    /* Filtered measurement */
  gnss_TimeSpeedStructType z_UnfiltMeas;  /* Un-filtered measurement */

  boolean u_MultipathEstValid; /* TRUE is Multipath estimate is valid */
  FLT     f_MultipathEst;      /* Multipath estimate (meters) */
  boolean u_SvDirValid;        /* SV direction validity flag */
  FLT     f_Azimuth;           /* Azimuth (radians) */
  FLT     f_Elevation;         /* Elevation (radians) */
  FLT     f_DopplerAccel;      /* Acceleration in Hz/Sec */
  FLT     f_FineSpeed;         /* Carrier phase derived speed */
  FLT     f_FineSpeedUnc;      /* Carrier phase derived speed UNC */
  DBL     d_CarrierPhase;      /* Carrier phase measurement [L1 cycles] */
  uint32  q_FCount;

} gnss_L2CMeasStructType;

typedef struct
{
  boolean v_FixValid;     /* TRUE if fix is valid, FALSE if invalid */
  uint16 w_GpsWeek;       /* GPS week number */
  uint32 q_GpsTimeMs;     /* GPS time in week (milliseconds) */
  uint32 q_RefFcount;     /* Receiver's ms counter value. */
  uint64 t_TimeTickMsec;  /* Receiver's time tick ms counter value */
  DBL d_PosLla[3];        /* LLA position (rad, m) */
  FLT f_Punc3dMeters;     /* Uncertainty of position (m) */
  FLT f_PuncLatLonMeters[2];  /** Latitude, longitude uncertianty (m), 
                                  68% conf. within +/- this 1-sigma value */
  FLT f_PuncVertMeters;         /* Vertical uncertainty (m) */
  FLT f_ErrorEllipse[3];  /** Unc angle (deg), unc a (m), unc b (m)
                              39% conf. within this one-sigma ellipse */
  pdsm_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
  pdsm_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/
  DBL d_VelEcef[3];       /* Velocity in ECEF (m/s) */
  DBL d_VelEnu[3];        /* Velocity in ENU (m/s) */
  FLT f_HeadingRad;       /* Heading angle, [0,2*pi) (rad). Referenced to North
                            (0 rad) and positive clock-wise. This is the "best" estimate
                            of heading and uses sensor data (if available and enabled).
                            See also: f_GnssHeadingRad below */
  FLT f_HeadingUncRad;    /* Heading uncertainty in Radians */
  FLT f_Vunc3dMps;              /* Velocity uncertainty (m/s) */
  FLT f_VuncEastNorthMps[2];    /* East, North uncertianty (m/s) */
  FLT f_VuncVertMps;      /* Vertical uncertainty (m/s) */
} gnss_PosVelSolutionStructType; /* Pos Vel data to GTS */

typedef enum
{
  GNSS_MEAS_BLK_SRC_GPS,       /* Source of the measurement block is GPS */
  GNSS_MEAS_BLK_SRC_GLO,       /* Source of the measurement block is GLONASS */
  GNSS_MEAS_BLK_SRC_BDS,       /* Source of the measurement block is BDS */
  GNSS_MEAS_BLK_SRC_GAL,       /* Source of the measurement block is GAL */
  GNSS_MEAS_BLK_SRC_HBW_GPS,
  GNSS_MEAS_BLK_SRC_RXD_GPS,
  GNSS_MEAS_BLK_SRC_QZSS_SBAS, /*!< Source of the measurement block is QZSS + SBAS */ 
  GNSS_MEAS_BLK_SRC_GLO_L2,
  GNSS_MEAS_BLK_SRC_GPS_L2,        /* Source of the measurement block is GPS */
  GNSS_MEAS_BLK_SRC_MAX
} gnss_MeasBlkSrcEnumTypeVal;
typedef uint8 gnss_MeasBlkSrcEnumType;

typedef struct
{
  uint8                         u_NumSvs;             /* Num SV for which measurements are valid */
  uint8                         u_SeqNum;             /* Sequence Number */
  uint8                         u_MaxMessageNum;      /* Maximum Number of message */
  uint32                        q_EngineType;         /* See the enumeration above, use gnss_EngineEnumType */
  uint8                         u_HwVersion;          /* Hardware version for the specific Engine type */
  gnss_ClockStructType          z_MeasClock;          /* GNSS clock information for the GPS or GLONASS measurements */
  gps_CDMAClockInfoStructType   z_CDMAClockInfo;      /* CDMA and RTC Values at Time Transfer */
  uint32                        e_MeasBlkSrc;         /* Source of the measurements in the GNSS measurement block */
  gnss_MeasStructType           z_Gnss[N_ACTIVE_CHAN];

} gnss_MeasBlkStructType;

typedef struct
{
  uint8                         u_NumSvs;             /* Num SV for which measurements are valid */
  uint8                         u_SeqNum;             /* Sequence Number */
  uint8                         u_MaxMessageNum;      /* Maximum Number of message */
  uint32                        q_EngineType;             /* See the enumeration above, use gnss_EngineEnumType */
  uint8                         u_HwVersion;              /* Hardware version for the specific Engine type */
  gnss_ClockStructType          z_MeasClock;          /* GNSS clock information for the GPS or GLONASS measurements */
  uint32                        e_MeasBlkSrc;         /* Source of the measurements in the GNSS measurement block */
  gnss_L2L5MeasStructType       z_Gnss[N_ACTIVE_CHAN];

} gnss_L2L5MeasBlkStructType;

typedef struct
{
  uint8                         u_NumSvs;             /* Num SV for which measurements are valid */
  uint8                         u_SeqNum;             /* Sequence Number */
  uint8                         u_MaxMessageNum;      /* Maximum Number of message */
  uint32                        q_EngineType;         /* See the enumeration above, use gnss_EngineEnumType */
  uint8                         u_HwVersion;          /* Hardware version for the specific Engine type */
  gnss_ClockStructType          z_MeasClock;          /* GNSS clock information for the GPS or GLONASS measurements */
  uint32                        e_MeasBlkSrc;         /* Source of the measurements in the GNSS measurement block */
  gnss_L2CMeasStructType        z_Gnss[N_ACTIVE_CHAN];

} gnss_L2CMeasBlkStructType;

typedef enum
{
  GNSS_MEAS_RPT_PERIODIC,
  GNSS_MEAS_RPT_MEAS_DONE,
  GNSS_GPS_MEAS_RPT_PERIODIC,
  GNSS_GPS_MEAS_RPT_MEAS_DONE,
  GNSS_MEAS_RPT_MAX

} gnss_MeasRptReasonEnumTypeVal;
typedef uint8 gnss_MeasRptReasonEnumType;

#define CPE_MEAS_BLK_VERSION (0)

/* BitMask
  Bit 0 = Measurement Validity (0 = invalid, 1 = valid)
  Bit 1 = Carrier Phase Validity (0 = invalid, 1 = valid)
  Bit 2 = Lock Point (0 = unknown, 1 = known)
  Bit 3 = Lock Point Polarity (0 = negative, 1 = positive)
*/
#define CPE_MEAS_STATUS_NULL              (0) /* The don't know anything state */
#define CPE_MEAS_STATUS_VALID             (1<<0)
#define CPE_MEAS_CARRIER_PHASE_VALID      (1<<1)
#define CPE_MEAS_LOCK_POINT_VALID         (1<<2)
#define CPE_MEAS_LOCK_POLARITY_POSITIVE   (1<<3)

/* GnssObservablesData from Measurement Report*/
typedef struct
{
  uint8 e_GNSSType; /*  0 = GPS,  1 = GLO, All else = Invalid */
  uint8 u_Sv; /*  GPS Satellite PRN (1~32) if e_GNSSType is GPS,
                  Glonass slot number (1~24) if e_GNSSType is GLO
                  Invalid otherwise
              */
  int8  b_FreqChannelNum;  /* -7 to +6 for GLONASS, valid only if e_GNSSType is GLO */
  float f_CnoDB; /* Signal to noise in 1 Hz BW. [dB Hz] 
                    referenced to the Matched Filter Output */
  double d_CodePhase; /* Code Phase [meter] */
  float f_CodePhaseUnc; /* Code Phase Unc [meter] */
  float f_Doppler; /* Doppler estimate (Hz) */
  float f_DopplerUnc; /* Doppler uncertainty estimate (Hz) */
  uint32 q_MeasStatus; /* BitMask
                          Bit 0 = Measurement Validity (0 = invalid, 1 = valid)
                          Bit 1 = Carrier Phase Validity (0 = invalid, 1 = valid)
                          Bit 2 = Lock Point (0 = unknown, 1 = known)
                          Bit 3 = Lock Point Polarity (0 = negative, 1 = positive)
                          Bits 4 to 31 = Reserved
                       */
  double d_CarrPhaseCycles; /* Carrier phase cycles */
  uint8 u_CycleSlipCount; /* Cycle slip count */
} gnss_CPEMeasStructType;

typedef struct
{
  uint8 u_Version; /* Version Number of Structure */

  /* Clock information */
  uint32 q_FCount; /* Receiver Ms count for debugging purpose */
  uint16  w_GpsWeek; /* GPS week number [Unknown = 65535] */
  uint32 q_GpsMsec; /* GPS msec [msec] */
  float f_ClkTimeBias; /* Clock bias [msec] where 
                          GPS Time (msec) = w_GpsWeek*WEEK_MSEC + q_GpsMsec - f_ClkTimeBias
                       */
  float f_ClkTimeUncMs; /* Clock Time bias Uncertainty [msec] */
  float f_ClkDrift; /* Clock Drift [s/s] */
  float f_ClkDriftUnc; /* 1 sided max Clock Drift uncertainty [s/s] */

  uint8 u_NumValidReports; /* Number of valid reports in z_Meas */

  gnss_CPEMeasStructType z_Meas[N_CPE_MAX_SV]; /* Measurment array */
} gnss_CPEMeasBlockStructType;


/* These are packed versions of above to allow for logging */
typedef PACKED struct PACKED_POST
{
  uint8 e_GNSSType; /*  0 = GPS,  1 = GLO, All else = Invalid */
  uint8 u_Sv; /*  GPS Satellite PRN, valid only if e_GNSSType is GPS */
  int8  b_FreqChannelNum;  /* -7 to +6 for GLONASS, valid only if e_GNSSType is GLO */
  float f_CnoDB; /* Signal to noise in 1 Hz. [dB Hz] 
                    referenced to the Matched Filter Output */
  double d_CodePhase; /* Code Phase [meter] */
  float f_CodePhaseUnc; /* Code Phase Unc [meter] */
  float f_Doppler; /* Doppler estimate (Hz) */
  float f_DopplerUnc; /* Doppler uncertainty estimate (Hz) */
  uint32 q_MeasStatus; /* BitMask
                          Bit 0 = Measurement Validity (0 = invalid, 1 = valid)
                          Bit 1 = Carrier Phase Validity (0 = invalid, 1 = valid)
                          Bit 2 = Lock Point (0 = unknown, 1 = known)
                          Bit 3 = Lock Point Polarity (0 = negative, 1 = positive)

                       */
  double d_CarrPhaseCycles; /* Carrier phase */
  uint8 u_CycleSlipCount; /* Cycle slip count */
} gnss_CPEMeasStructTypeDM;

/* CPE Measurement Block Log Structure */
typedef PACKED struct PACKED_POST
{
  log_hdr_type z_hdr;        /* Standard DM header */
  uint8 u_Version; /* Version Number of Structure */

  /* Clock information */
  uint32 q_FCount; /* Receiver Ms count for debugging purpose */
  uint16  w_GpsWeek; /* GPS week number [Unknown = 65535] */
  uint32 q_GpsMsec; /* GPS msec [msec] */
  float f_ClkTimeBias; /* Clock bias [msec] where 
                          GPS Time (msec) = w_GpsWeek*WEEK_MSEC + q_GpsMsec - f_ClkTimeBias
                       */
  float f_ClkTimeUncMs; /* Clock Time bias Uncertainty [msec] */
  float f_ClkDrift; /* Clock Drift [s/s] */
  float f_ClkDriftUnc; /* 1 sided max Clock Drift uncertainty [s/s] */

  uint8 u_NumValidReports; /* Number of valid reports in z_Meas */

  gnss_CPEMeasStructTypeDM z_Meas[N_CPE_MAX_SV]; /* Measurment array */
} gnss_CPEMeasBlockStructTypeDM;

typedef enum
{
  GNSS_GLO_TYPE_0 = 0, /* GLONASS type SV */
  GNSS_GLO_TYPE_M = 1,  /* GLONASS M type SV */
  GNSS_GLO_TYPE_K = 2,  /* GLONASS K type SV */
} gnss_GloSvTypeVal;

typedef struct
{
  uint32              q_RefFCount;      /*!< FCount value at reference tick */
  uint32              q_NumClockResets; /*!< Cumulative # of Clock resets. */
  uint64              t_SystemRtcMs;    /*!< System RTC value at clock validity */
  gps_TimeStructType  z_GpsTime; /*!< Time at q_RefFCount */
} gnss_NHzClockStructType;

typedef struct
{
  uint8   u_CnoDB;             /* CNo. Units of 1 dB */
  int8    b_FreqNum;           /* GLONASS frequency number + 7 (0 - 13)  */
  uint16  w_Sv;                /* GPS/BDS/GAL SVPRN or GLONASS SV Slot Id */
  int16   x_LatencyMs;         /* Age of the measurement in msecs (+ve meas Meas precedes ref time) */
  uint32  q_MeasStatus;        /* Bit mask indicating measurement status */
  FLT     f_FineSpeed;         /* Carrier phase derived speed */
  FLT     f_FineSpeedUnc;      /* Carrier phase derived speed UNC */
  FLT     f_DopplerAccel;      /* Acceleration in Hz/Sec */
  gnss_SvTime z_SvTime;        /* SV Time information */
  FLT     f_SvTimeUncMs;       /* Time uncertainty (msec) */
  uint8   u_NHzCSlipCount;     /* CSlip count */
} gnss_NHzMeasStructType;

typedef struct
{
  uint8                         u_NumGpsSvs; /* Num GPS SV for which measurements are valid */
  uint8                         u_NumGloSvs; /* Num GLO SV for which measurements are valid */
  uint8                         u_NumBdsSvs; /* Num BDS SV for which measurements are valid */
  uint8                         u_NumGalSvs; /* Num BDS SV for which measurements are valid */
  uint8                         u_NumQzssSvs; /* Num BDS SV for which measurements are valid */
  gnss_NHzClockStructType       z_MeasClock; /* GPS clock information */
  gnss_NHzMeasStructType        z_Gnss[N_ACTIVE_GNSS_CHAN_NHZ];
} gnss_NHzMeasBlkStructType;
/*MGP data strucutres*/

/*************************************************************************
  Acquisition Assistance (Steering) information                         
*************************************************************************/
typedef enum
{
  MGP_GNSS_SV_STEER_SRC_NONE = 0,
  MGP_GNSS_SV_STEER_SRC_GPS,
  MGP_GNSS_SV_STEER_SRC_SBAS,
  MGP_GNSS_SV_STEER_SRC_GLO,
  MGP_GNSS_SV_STEER_SRC_BDS,
  MGP_GNSS_SV_STEER_SRC_GAL,
  MGP_GNSS_SV_STEER_SRC_MAX

} gnss_SvSteerSrcEnumTypeVal;
typedef uint8 gnss_SvSteerSrcEnumType;


typedef enum
{
  GNSS_ACQ_SRC_NONE = 0,
  GNSS_ACQ_SRC_ALM,
  GNSS_ACQ_SRC_EPH,
  GNSS_ACQ_SRC_XTRA,
  GNSS_ACQ_SRC_NWSERV,
  GNSS_ACQ_SRC_LAST,
}gnss_SteeringSrcTypeVal;
typedef uint8 gnss_SteeringSrcType;

/* Common struct for Steer info */
typedef struct
{
  uint16  w_Sv;                   /* GPS, BDS, GAL SV PRN */
  int8    b_GloFreqIndex;         /* GLO Frequency Index -7..+6 */
  gnss_SteeringSrcType e_GnssSteeringSrc; /* Steering src */
  boolean u_Modulo80ms;           /* Indicate whether q_Ms is a value modulo 80ms */
  FLT     f_SubMsec;              /* Gnss Code Phase (0-0.999999ms) */
  uint32  q_Ms;                   /* Gnss milliseconds of the week (msec) */
  FLT     f_SvTimeUnc;            /* Code Search window (msec) */
  FLT     f_SvSpeed;              /* Satellite Speed (meters/sec) */
  boolean u_SvAccelValid;         /* if TRUE, f_SvAccel has valid value */
  FLT     f_SvAccel;              /* Satellite Acceleration (meters/sec/sec) */
  boolean u_SvSpeedUncValid;      /* if TRUE, u_SvSpeedUncValid has valid value */
  FLT     f_SvSpeedUnc;           /* Speed Uncertainty (meters/sec) */
}gnss_SvSteerStructType;

typedef struct
{
  boolean                u_WeekValid;         /* if TRUE: GPS, BDS week number in this struture is valid */
  boolean                u_TimeRefIsGnss;     /* TRUE if the steering data is referenced to specified GNSS time */
  boolean                u_RefPnPhaseValid;   /* TRUE if Ref PN phase is valid, GPS and BDS only */
  boolean                u_OptimisticPos;     /* AA using optimisitic posiiton - Only valid if u_TimeRefIsGps = True */
  boolean                u_GloCalDayValid;    /* if TRUE: GLO Day is valid , GPS and BDS only */
  boolean                u_GloCycleValid;     /* if TRUE: GLO Four year cycle is valid */
  uint8                  u_GloNumCycles;      /* Glonass Four year cycles since 1996 */
  uint8                  u_NumSvs;            /* Number of SVs for which steering is provided */
  uint16                 w_Week;              /* GPS, BDS week number */
  uint16                 w_GloCalDay;         /* Glonass Calendar Day */
  uint16                 w_RefPn;             /* CDMA Reference PN sequence offset, GPS and BDS only */
  uint32                 q_Msec;              /* GPS, BDS milliseconds of the week */
  uint32                 q_SvSteerSrc;        /* Enumeration of SV steering source, use gnss_SvSteerSrcEnumType */
  uint32                 q_TimeOfApp;         /* Time of applicability in CDMA units of 1.28sec (modulo 320ms), GPS and BDS only */
  int32                  l_RefPnPhase;        /* Ref PN position (CDMARTC_CCx8 % 80ms), GPS and BDS only */
  gnss_SvSteerStructType z_SvSteerInfo[N_MAX_VISIBLE_GNSS_SV]; /*!< Steering information per SV */
} gnss_SvAcqAssistCommonStructType;

typedef enum
{
  MGP_GNSS_AA_REF_TIME_SRC_NONE = 0,
  MGP_GNSS_AA_REF_TIME_SRC_CDMA,
  MGP_GNSS_AA_REF_TIME_SRC_GPS,
  MGP_GNSS_AA_REF_TIME_SRC_GLO,
  MGP_GNSS_AA_REF_TIME_SRC_BDS,
  MGP_GNSS_AA_REF_TIME_SRC_MAX,
} aa_RefTimeSrcEnumTypeVal;
typedef uint8 aa_RefTimeSrcEnumType;

/* 
 * code phase in steering is relative to time indicated by 
 * codePhaseOrigin in gnss_AcqAssistDataClkInfo
 *
 * codePhaseOrigin (0-127)ms itself is relative to referenceTime in 
 * gnss_AcqAssistDataClkInfo.
 *
 *    codePhase                           codePhaseOrigin    referenceTime
 *  PRN 1 <-------------------------------|                  |   (Msec mod
 *  2<------  -64 to 64 -2^(-10) ms-------|<-- 0 to 127ms -->| Msec in Week)
 *           3<---------------------------|                  |
 *                                        |                  |
 */

typedef struct
{
  aa_RefTimeSrcEnumType     e_RefTimeSrc;       /* time reference source. a value of MGP_GNSS_AA_REF_TIME_SRC_NONE 
                                                   implies no data present in structure, use cdma time at MS       */
  uint32                    q_RefTime;          /* set to t mod MSEC_IN_WEEK, t is reference time un units of 1 ms */
  FLT                       f_RefTimeUnc;       /* Single sided Unc. of reference time field. units of 1 ms        */
  FLT                       f_ClkBias;          /* estimated MS clock bias. units of 1 ms                          */
  FLT                       f_StdDevClkBiasErrMin; /* units of 1 ms                                                   */
  FLT                       f_StdDevClkBiasErrMax; /* units of 1 ms                                                   */

  FLT                       f_CodePhaseOrigin;  /* units of 1 ms. Value 0 - 127 ms. The origin of all code phase 
                                                   values included in the accompanying SV steering information.    */
} gnss_AcqAssistDataClkInfo;

typedef struct
{
  uint32                    q_GnssType; /* GNSS, use mgp_GnssType */
  gnss_AcqAssistDataClkInfo z_AADataClkInfo;
  gnss_SvAcqAssistCommonStructType z_GnssSvAcq; /* GNSS Sv Acquisition  Type */

} gnss_SvAcqAssistStructType;



/*************************************************************************
  SV Direction information                                              
*************************************************************************/

typedef struct
{
  uint16  w_Sv;                    /* GNSS SV PRN GPS:1-32, GLO 65-88, BDS 201-237, GAL 301-336 */
  int8    b_GloFreqIndex;          /* Glo Freq Index -7..+6 */
  FLT     f_Azimuth;               /* Azimuth (radians) */
  FLT     f_Elevation;             /* Elevation (radians) */
  boolean u_ElevUncValid;          /* if TRUE, u_ElevUnc has valid value */
  FLT     f_ElevUnc;               /* Elevation Uncertainty (radians) */

} gnss_AzElInfoStructType;


typedef struct
{
  uint32                  q_GnssType; /* GNSS, use mgp_GnssType */
  uint8                   u_NumSvs; /* Numbers of GNSS  SVs for which direction information is provided */
  gnss_AzElInfoStructType z_SvAzElInfo[N_MAX_GNSS_SV];

} gnss_SvDirStructType;


/*************************************************************************
  GNSS Time Model Reference Data strcuture

  gnss-TO-ID to Indication relation
  Value of gnss-TO-ID Indication
  1 GPS
  2 Galileo
  3 QZSS
  4 GLONASS
  5-15  reserved
*************************************************************************/

#define C_GNSS_TO_ID_GPS (1)
#define C_GNSS_TO_ID_GLO (4)
#define C_GNSS_GANSST_A1_UNKNOWN (0x8000)
#define C_GNSS_GANSST_A2_UNKNOWN (0x8000)
#define C_GNSS_GANSST_WEEK_UNKNOWN C_GPS_WEEK_UNKNOWN
#define C_GNSS_GANSST_DELTAT_UNKNOWN (0xFFFF)

#define   TWO_TO_10   1024
#define   TWO_TO_21   2097152.0
#define   TWO_TO_24   16777216.0
#define   TWO_TO_35   34359738368.0
#define   TWO_TO_51   2.251799814E+15
#define   TWO_TO_68   2.951479052E+20

#define   TWO_TO_MIN6   (1.0/TWO_TO_6)
#define   TWO_TO_MIN10  (1.0/TWO_TO_10)
#define   TWO_TO_MIN21  (1.0/TWO_TO_21)
#define   TWO_TO_MIN24  (1.0/TWO_TO_24)
#define   TWO_TO_MIN35  (1.0/TWO_TO_35)
#define   TWO_TO_MIN51  (1.0/TWO_TO_51)
#define   TWO_TO_MIN68  (1.0/TWO_TO_68)

typedef struct
{
  uint32 q_GanssTimeModelRefTime; /* Reference time scaled at 2^4 seconds */
  int32  l_A0;                    /* TauGPS scaled at 2^-35 seconds */
  int16  x_A1;                    /* TauGPS drift scaled at 2^-51 seconds */
  int16  x_A2;                    /* TauGPS drift scaled at 2^-68 seconds */
  uint8  u_GnssToID;              /* See definition above */
  uint16 w_WeekNo;                /* Reference week number */
  uint16 w_DeltaT;                /* Leap second value */
}gnss_TimeModelType;

/*************************************************************************
  GLO Reference Time Data strcuture
*************************************************************************/
typedef struct
{
  /* Glo 4 Year number from 1996: C_GLO_DAY_UNKNOWN if invalid */
  uint8   u_GloFourYear;

  /* Glo day number in 4 years: C_GLO_FOUR_YEAR_UNKNOWN  if invalid */
  uint16  w_GloDays;

  uint32  q_GloMsec;     /* Glo msec in the day at reference tick */

  FLT     f_GloTimeUncMs; /* Glo Clock uncertainty in msecs*/

} glo_RefTimeStructType;

/*************************************************************************
  GLO-UTC Information
  This structure is shared between PE and MC to pass the notice of GLONASS
  leap sec change and Tao_Gps
*************************************************************************/
typedef struct
{
  boolean   b_Filled;
  uint8     u_N4;
  uint8     u_Kp; /* Kp could be invalid. This structure is re-used to push 
                  TaoGPS, N4, Nt to ME with Kp set to 0. Kp should not be 
                  used by ME without further CD changes. */
  uint16    w_Nt;
  FLT       f_TaoGps;
} glo_TimeParaStructType;


/*************************************************************************
  GLO-UTC Information
  This structure is shared between PE and SM, for SM to inject GLONASS
  UTC related info to CD
  All the data fields are unscaled.
*************************************************************************/
typedef struct
{
  union               /* Flags for optional data */
  {
    uint8 u_FilledMask;
    struct {
      uint8 u_N4Flag : 1;
      uint8 u_KpFlag : 1;
      uint8 u_NtFlag : 1;
      uint8 u_TaoGpsFlag : 1;
      uint8 u_FreqNumFlag : 1;
      uint8 : 3;
    } z_FilledMask;   /* Set corresponding flag to 1 if that data is filled. */
  } z_Mask;
  uint8     u_N4;     /* Number of 4-year cycle since 1996, starting from 1 */
  uint8     u_Kp;     /* Leap sec change notice, 1: will be changed */
  uint16    w_Nt;     /* Number of days in a 4-year cycle, starting from 1 */
  uint32    q_TaoGps; /* TaoGps: unscaled, 22-bit */
  int8      b_FreqNum; /* should be ignored if the type is NOT OTA */
  uint8     u_Src;    /* C_GLOUTC_SRC_INVALID,C_GLOUTC_SRC_OTA,C_GLOUTC_SRC_XTRA,
                         C_GLOUTC_SRC_ASSIST */
} glo_UnscaledUtcStructType;


/*************************************************************************
  BDS Time Offset Information
  This structure keeps time offset information between BDS and other GNSS time
*************************************************************************/
typedef struct
{
  /* GPS-related offsets */
  int16   x_A0Gps;        /* BDT clock bias relative to GPS time */
  int16   x_A1Gps;        /* BDT clock rate relative to GPS time */
  /*GLO-related offsets */
  int16   x_A0Glo;        /* BDT clock bias relative to GLO time */
  int16   x_A1Glo;        /* BDT clock rate relative to GLO time */

  uint8   u_TimeOffsetValidMask; /* bits: 0 -- GPS-related offset is valid 
                                          1 -- GLO-related offset is valid 
                                          2-7 -- unused */
  uint16  w_BdsWeek;      /* BDS week number */
  uint32  q_BdsMsec;      /* BDS milliseconds of the week */
} bds_UnscaledTimeOffsetStructType;

/* GPS UTC message from ME to other blocks */
typedef struct
{
  uint16 w_ExtendedWeekNumber; /* Full weeks since Jan/06/1980, extension of u_WNt */
  uint8  u_Valid;              /* Is UTC data valid or not */
  gps_UtcInfoRptStructType z_UtcInfo;
} gnss_RawUtcStructType;

typedef struct
{
  mgp_GnssType e_gnss_id;
  uint16 w_gnss_day;
  uint32 q_gnss_tod_sec;
  uint32 q_gnss_tod_unc_ms;
} gnss_RefTimeStructType;

/*************************************************************************
  BDS UTC Time Offset Information
  This structure keeps time offset information between BDS and UTC
*************************************************************************/
typedef struct
{
  /* GPS-related offsets */
  int32   l_A0;           /* BDT clock bias relative to UTC */
  int32   l_A1;           /* BDT clock rate relative to UTC */
  int8    b_DeltaTls;     /* Delta time due to leap seconds BEFORE the new LS effective */
  int8    b_DeltaTlsf;    /* Delta time due to leap seconds AFTER the new LS effective */
  uint8   u_WNlsf;        /* Week number of the new leap second */
  uint8   u_DN;           /* Day number of the week of the new leap second */
} bds_UnscaledUtcOffsetStructType;

/*************************************************************************
  GAL Time Offset Information
  This structure keeps time offset information between GAL and GPS time
  (i.e. GAL_time = GPS_time + offset)
*************************************************************************/
typedef struct
{
  uint8  u_Valid;      /* Validity flag */
  uint8  u_Wn0G;       /* Reftime for GAL GPS time offset, week number */
  uint8  u_T0G;        /* Reftime for GAL GPS time offset, milliseconds of the week */
  int16  x_A0G;        /* Constant term of the polynomial describing the offset */
  int16  x_A1G;        /* Rate of change of the offset */
  uint16  w_GalWeek;   /* GAL week number ToA*/
  uint32  q_GalMsec;   /* GAL milliseconds of the week ToA*/
} gal_GstGpsStructType;

/* Alias for compatibility with ME. ToDo_GAL: remove once API is stable. */
typedef gal_GstGpsStructType gal_UnscaledTimeOffsetStructType;

/*******************************************************************************
  GAL UTC Offset Information
  This structure keeps time offset information between GAL and UTC time
*******************************************************************************/
typedef struct
{
  int32    l_A0;        /* GAL clock bias relative to UTC, constant term */
  int32    l_A1;        /* GAL clock rate relative to UTC, 1st order term */
  int8     b_DeltaTls;  /* Delta time due to leap seconds before the new LS
                           effective */
  uint8    u_Tot;       /* UTC data reference time of week */
  uint8    u_WNot;      /* UTC data reference week */
  uint8    u_WNlsf;     /* Week number of the new leap second */
  uint8    u_DN;        /* Day number of the week of the new leap second */
  int8     b_DeltaTlsf; /* Delta time due to leap seconds after the new LS
                           effective */
} gal_UnscaledUtcOffsetStructType;

/*************************************************************************
  GLO eph structure
  This structure is shared between PE and SM, for SM to inject GLONASS
  ephemeris to CD.
  All the day fields are unscaled.
  Refer each field definition to GLONASS-ICD 02
  http://qshare.qualcomm.com/qshare/drl/objectId/09010025804b8e1a/v/c
*************************************************************************/
typedef struct
{
  uint8  u_SvId;      /* The GLONASS satellite ID = slot ID + 64
                         valid range: 65-88 */
  union {               /* Flags for optional data */
    uint8 u_FilledMask;
    struct {
      uint8  u_GloTypeFlag : 1;
      uint8  u_TimeSrcPFlag : 1;
      uint8  u_FtFlag : 1;
      uint8  u_NtFlag : 1;
      uint8  u_P4Flag : 1;
      uint8  u_LnFlag : 1;
      uint8  : 2;
    } z_FilledMask;   /* Set corresponding flag to 1 if that data is filled. */
  } z_Mask;

  uint8  u_Tb;        /* GLONASS eph reference time tb: 7-bit LSBs*/
  uint8  u_GloType;   /* GLONASS-Type: 00-GLONASS, 01-GLONASS-M, 02-GLONASS-K */
  uint8  u_TimeSrcP;  /* Indication the satellite operation mode in respect of 
                         time parameters P, 2-bit LSBs*/
  uint8  u_Bn;        /* Health flag Bn: 3-bit LSBs, only check bit 2 */
  uint8  u_L1L2;      /* Time diff between the L2 and L1 transmitters: 5-bit,
                         LSB0-LSB3 is data bit, scale factor 2^-30, LSB4 is 
                         the sign bit, Delat_Tao_n */
  uint8  u_P1;        /* Indication of time interval between adjacent eph P1,
                         2-bit LSBs*/
  uint8  u_P2;        /* interval indicator P2: 1 bit, LSB */
  uint8  u_Ln;        /* Additional health flag only available on GLONASS-M 
                         satellite, 1 bit LSB*/
  uint8  u_FreqNum;   /* Frequency index: 0 ~ 13 */
  uint8  u_Ft;        /* FT is a parameter shown the URE at time tb: 4-bit 
                         LSBs */
  uint8  u_Acc[3];    /* acceleration due to lunar-solar gravitational 
                         perturbation, 5-bit LSBs, X/Y/ZDotDot_n */
  uint16 w_Nt;        /* calendar number of day within four-year interval 
                         starting from the 1st of January in a leap year, 
                         11-bit LSBs */
  uint16 w_FreqOff;   /* (f(pre)-f(nom))/f(nom), unscaled, 11-bit LSBs,
                         Gamma_n */
  uint32 q_ClkCorr;   /* Satellite clock correction: 22-bit LSBs, Tao_n */
  uint32 q_SatPos[3]; /* Satellite position-xyz, 27-bit LSBs each, X/Y/Zn; */
  uint32 q_SatVel[3]; /* Satellite velocity, 24-bit LSBs each, X/Y/ZDot_n */
  uint8  u_En;        /* Age of Eph in days, 5-bit LSBs */
  uint8  u_P4;        /* Flag P4,1-bit LSB */
} glo_EphStructType;


/*************************************************************************
  BDS eph structure
  This structure is shared between PE and SM, for SM to inject BeiDou
  ephemeris to CD. All the data fields are unscaled
  Refer for each field definition to BeiDou-ICD
*************************************************************************/

typedef struct
{
  uint8   u_Sv;                /* SV ID: 201 - 237 */
  uint16  w_BdsWeek;           /* Unambiguous BDS week number */
  uint32  q_Toe;               /* Reference time of Ephemeris (17-bits) */
  uint32  q_Toc;               /* Time of clock (17-bits) */
  uint8   u_Iode;              /* Issue of data ephemeris tag (5-bits) */
  uint8   u_Iodc;              /* Issue of Data clock (5-bits) */
  uint8   u_Health;            /* SV Health (1-bit): SatH1 */
  uint8   u_URAI;              /* User Range Accuracy Index (4-bits) */
  uint32  q_SqrtA;             /* Square-root of the Semi-Major axis (32-bits) */
  uint32  q_E;                 /* Orbital Eccentricity (32-bits) */
  uint32  q_Omega;             /* Argument of Perigee (32-bits) */
  uint16  w_DeltaN;            /* Mean motion difference from computed value (16-bits) */
  uint32  q_M0;                /* Mean anomaly at reference time (32-bits) */
  uint32  q_Omega0;            /* Longitude of the ascending node of orbit plane computed according to ref. time (32-bits) */
  uint32  q_OmegaDot;          /* Rate of Right Ascension (24-bits) */
  uint32  q_I0;                /* Inclination angle at reference time (32-bits) */
  uint16  w_Idot;              /* Rate of inclination angle (14-bits) */
  uint32  q_Cuc;               /* Amplitude of the cosine harmonic correction term to argument of latitude (18-bits) */
  uint32  q_Cus;               /* Amplitude of the sine harmonic correction term to argument of latitude (18-bits) */
  uint32  q_Crc;               /* Amplitude of the cosine harmonic correction term to Orbit Radius (18-bits) */
  uint32  q_Crs;               /* Amplitude of the sine harmonic correction term to Orbit Radius (18-bits) */
  uint32  q_Cic;               /* Amplitude of the cosine harmonic correction term to angle of inclination (18-bits) */
  uint32  q_Cis;               /* Amplitude of the sine harmonic correction term to angle of inclination (18-bits) */
  uint32  q_A0;                /* Clock bias coefficient (24-bits) */
  uint32  q_A1;                /* Clock drift coefficient (22-bits) */
  uint16  w_A2;                /* Clock acceleration coefficient (11-bits) */
  uint16  w_Tgd;               /* Time of Group Delay (10-bits) */
} bds_EphStructType;

/*************************************************************************
  GAL eph structure
  This structure is shared between PE and SM, for SM to inject Galileo
  ephemeris to CD. All the data fields are unscaled
  Refer for each field definition to Galileo OS SIS ICD.
*************************************************************************/
typedef struct
{
  uint16 w_SvId;         /* SV ID: 301 – 336 */
  uint16 w_Iode;         /* Issue of data ephemeris tag (10 bits) */
  uint16 w_Toe;          /* Reference time of Ephemeris (14 bits) [minutes] */
  uint32 q_M0;           /* Mean anomaly at reference time (32 bits) */
  uint16 w_DeltaN;       /* Mean motion difference from computed value 16 bits */
  uint32 q_E;            /* Orbital Eccentricity (32 bits) */
  uint32 q_SqrtA;        /* 32 bits */
  uint32 q_Omega0;       /* Longitude of the ascending node of orbit plane
                         computed according to ref. time (32 bits) */
  uint32 q_OmegaDot;     /* Rate of Right Ascension (24 bits) */
  uint32 q_Omega;        /* Argument of Perigee (32 bits) */
  uint32 q_I0;           /* Inclination angle at reference time (32 bits) */
  uint16 w_IDot;         /* Rate of inclination angle (14 bits) */
  uint16 w_Cic;          /* Amplitude of the cosine harmonic correction term
                         to angle of inclination (16 bits) */
  uint16 w_Cis;          /* Amplitude of the sine harmonic correction term to
                         angle of inclination (16 bits) */
  uint16 w_Crc;          /* Amplitude of the cosine harmonic correction term
                         to Orbit Radius ( 16 bits) */
  uint16 w_Crs;          /* Amplitude of the sine harmonic correction term to
                         Orbit Radius (16 bits) */
  uint16 w_Cuc;          /* Amplitude of the cosine harmonic correction term
                         to argument of latitude (16 bits) */
  uint16 w_Cus;          /* Amplitude of the sine harmonic correction term to
                         argument of latitude (16 bits) */
  uint16 w_Week;         /* Week number (derived from GST containing word) */
  uint8  u_Sisa;         /* Signal In Space Accuracy; not defined in the current
                            Galileo ICD */
  uint8  u_Health;       /* SV Health (2-bit): E5bHS and E5bDVS. Taken from Word5 */
  uint16 w_Bgd;          /* Broadcast Group Delay E1/E5b only (10 bits). Taken from Word5 */
  uint16 w_Toc;          /* Time of clock (14 bits) [minutes] */
  uint8  u_Af2;          /* Clock acceleration coefficient (6  bits) */
  uint32 q_Af1;          /* Clock drift coefficient (21 bits) */
  uint32 q_Af0;          /* Clock bias coefficient (31 bits) */
} gal_EphStructType;

/*************************************************************************
  GLO alm structure
  This structure is shared between PE and SM, for SM to inject GLONASS
  almanac to CD
  All the data fields are unscaled
  Refer each field definition to GLONASS-ICD 02
  http://qshare.qualcomm.com/qshare/drl/objectId/09010025804b8e1a/v/c
*************************************************************************/
typedef struct
{
  uint8  u_SvId;      /* = GLONASS SV slot number (1..24) + 
                           64 (i.e. GLO_SLOTID_OFFSET) */

  union {               /* Flags for optional data */
    uint8 u_FilledMask;
    struct {
      uint8  u_SvTypeFlag : 1;
      uint8  u_N4Flag : 1;
      uint8  : 6;
    } z_FilledMask;   /* Set corresponding flag to 1 if that data is filled. */
  } z_Mask;

  uint8  u_Health;    /* Generalized unhealthy flag, Cn 
                         0: non operable 
                         1: operable  */
  uint8  u_SvType;    /* Satellite type2 (0~3), Mn
                         00: GLONASS
                         01: GLONASS-M */
  uint16 w_TauN;      /* Satellite time coarse correction (-1.9E-3~1.9E-3)
                         10-bit LSBs, scale factor: 2^-18 second */
  uint32 q_LongAscend; /* Longitude of the first ascending node within the NA day
                          21-bit LSBs, Lamda_nA
                          scale factor: 2^-20, valid range:(-1..1)semi-circle */
  uint32 q_InclCorrect; /*Correction to the mean value of inclination
                          18-bit LSBs, Delta_i_n_A
                          scale factor: 2^-20 
                          Range: (-0.067 .. 0.067)semi-circle */
  uint16 w_OrbitEcc;  /* Eccentricity of orbit, scale factor: 2^-20, 
                         15-bit LSBs, Epsilon_n_A
                         Range:(0~0.03) */
  uint16 w_Perigee;   /* Argument of perigee, scale factor: 2^-15 
                         16-bit LSBs, Omega_n_A
                         Range: (-1~1) semi-circle */
  uint32 q_TimeFirstAscend; /*Time in seconds of the first ascending node passage 
                          within NA day, scale factor: 2^-5, t_Lamda_n_A
                          21-bit LSBs
                          Range: (0~4100) second */
  uint32 q_DracCorrect; /* Correction to the mean value of Draconian period.  
                          22-bit LSBs, Delat_T_n_A
                          scale factor: 2^-9, 
                          Range: (-3.6E3 ~ 3.6E3) s/orbital period */
  uint8  u_DractRate; /* Rate of change of Draconian period
                         7-bit LSBs, Delta_TDot_n_A
                         scale factor: 2^-14
                         Range: (-2E-8~2E-8) s/orbital period2 */
  uint8  u_FreqNum;   /* HnA : Carrier frequency number 
                         5-bit LSBs, */
  uint8  u_N4;        /* Number of 4-year cycle since 1996/1/1, starting from 1
                         optional parameter */
  uint16 w_Na;        /* Number of days in a 4-year cycle */
  uint32 q_FullToa;   /* When injecting, DO NOT fill this field.
                         Number of days since 1996/1/1, starting from 1 
                         Combination of N4 and NA
                         FullToA = (N4-1)*FOURYEAR_DAYS+(NA-1) */
} glo_AlmStructType;

/*************************************************************************
  BDS alm structure
  This structure is shared between PE and SM, for SM to inject Beidou
  almanac to CD
  All the data fields are unscaled
  Refer for each field definition to BeiDou-IC
*************************************************************************/

typedef struct
{
  uint32  q_SqrtA;             /* Square-root of the Semi-Major axis (24-bits) */
  uint32  q_E;                 /* Orbital Eccentricity (17-bits) */
  uint32  q_Omega;             /* Argument of Perigee (24-bits) */
  uint32  q_M0;                /* Mean anomaly at reference time (24-bits) */
  uint32  q_Omega0;            /* Longitude of the ascending node of orbit plane at weekly epoch (24-bits) */
  uint32  q_OmegaDot;          /* Rate of Right Ascension (17-bits) */
  uint16  w_DeltaI;            /* Correction of orbit reference iclination at reference angle (16-bits) */
  uint16  w_A0;                /* Clock bias coefficient (11-bits) */
  uint16  w_A1;                /* Clock drift coefficient (11-bits) */
  uint16  w_BdsWeek;           /* BDS week number, can be ambiguous depending on the src */
  uint8   u_Toa;               /* Reference time of Almanac (8-bits) */
  uint8   u_Sv;                /* SV ID: 201 - 237 */
} bds_AlmStructType;

/*************************************************************************
  GAL alm structure
  This structure is shared between PE and SM, for SM to inject Galileo
  almanac to CD
  All the data fields are unscaled
  Refer for each field definition to Galileo OS SIS ICD
*************************************************************************/
typedef struct
{
  uint16  w_SvId;       /* SV ID: 301 - 336 */
  uint16  w_E;          /* Orbital Eccentricity (11-bits) */
  uint16  w_Toa;        /* Reference time of Almanac (10-bits) */
  uint16  w_DeltaI;     /* Correction of orbit reference iclination at 
                           reference angle (11-bits) */
  uint16  w_OmegaDot;   /* Rate of Right Ascension (11-bits) */
  uint16  w_DeltaSqrtA; /* Difference with respect to the square root of the 
                           nominal semi-major axis (13-bits) */
  uint16  w_Omega0;     /* Longitude of the ascending node of orbit plane at 
                           weekly epoch (16-bits) */
  uint16  w_Omega;      /* Argument of Perigee (16-bits) */
  uint16  w_M0;         /* Mean anomaly at reference time (16-bits) */
  uint16  w_A0;         /* Clock bias coefficient (16-bits) */
  uint16  w_A1;         /* Clock drift coefficient (13-bits) */
  uint16  w_Week;       /* GAL week number, can be ambiguous depending on src */
  uint8   u_HS;         /* Contains both signal health status SVBstatus
                           (E5bHS) and SVBstatus(E1BHS) coded on 2 bits each */
} gal_AlmStructType;

/*************************************************************************/
/* BDS Ionospheric corrections - Klobuchar model                         */
/*************************************************************************/
typedef gps_IonoStructType bds_IonoStructType; /* BDS Klobuchar model */

/*************************************************************************/
/* BDS Ionospheric corrections - Grid model                            */
/*************************************************************************/
typedef struct
{
  uint32 q_VertDelay  : 9; /* Magnitude of the ionospher vertical delay at the grid point */
  uint32 q_GiveInd    : 4; /* 4-bit Give Indicator */
  uint32 q_GridNum    : 9; /* Grid number, range - 1 to 320 */
  uint32              : 10; /* padding to align to 4-byte boundary */
}bds_GridModelStructType;

typedef struct
{
  uint16 w_RefBdsSec;      /* Ref BDS Sec. for the the grid based model, modulo 3600s */
  uint16 w_NumOfGridPts;   /* Total number of valid grid points in the payload, range 1-320*/
  bds_GridModelStructType  *pz_GridData; /* pointer to BDS Grid model parameters */
} bds_IonoGridStructType;

/*************************************************************************
  GNSS SV non-existence information                                      
*************************************************************************/
typedef struct
{
  mgp_GnssType  e_GnssType;
  union               {
    uint8 u_FilledMask;
    struct {
      uint8  u_SvFilledFlag : 1;   /* q_SvNoExistMask and/or 
                                      q_SvNoExistMask2 is set to non-zero */
      uint8  u_FreqFilledFlag : 1; /* w_FreqNoExistMask is set to non-zero*/
      uint8  : 6;
    } z_FilledMask;
  } z_Mask;
  uint64 t_SvNoExistMask;   /*!< Bit Mask specifying the SVs which do not exist
                               ('1' => No exist)
                               GPS  bit 0-31  => SV ID 1-32,
                               QZSS bit 32-36 => SV ID 193-197,
                               GLO  bit 0-23  => Slot ID:1-24 */
  uint16 w_FreqNoExistMask; /* GPS: N/A
                               GLO:bit 0-13 => freq number -7..+6 */

} gnss_SvNoExistStructType;


/*************************************************************************
  GNSS SV Adv health information                                      
*************************************************************************/
typedef struct
{
  mgp_GnssType  e_GnssType;

  uint32 q_BadSvMask;   /* Bit Mask specifying the SVs which have bad health */
  /* GPS bit 0-31 => SV ID 1-32, 
     GLO bit 0-23 => Slot ID:1-24) */
  uint32 q_BadSvMask2;  /* to support any GNSS which has more than 32 satellites 
                           bit 0-31 => GNSS SV ID 33-64 */
} gnss_AdvHealthStructType;

typedef struct
{
  U64     t_FilledSvMask; /* mask to indicate which element in w_SVHealth has 
                           valid health info:
                           bit 0: sv 201
                           bit 1: sv 202
                           bit 29: sv 230, and etc */
  uint32  q_BdsMsec;    /* Msecs in BDS week */
  uint16  w_BdsWeek;    /* BDS week number since beginning of Jan 1, 2006 */
  uint16  w_SVHealth[N_BDS_SV];
} bds_HealthStructType;

typedef struct
{
  uint16  w_Week;    /* GAL week number since 1999 */
  uint32  q_Msec;    /* Msecs of GAL week */
  uint8   u_SvHealth[N_GAL_SV];
} gal_HealthStructType;

typedef gps_IonoStructType bds_IonoStructType;

/*************************************************************************/
/* Ionospheric Corrections for Galileo                                   */
/*************************************************************************/
typedef struct
{
  uint16 w_Ai0;        /* Effective Ionisation Level 1st order parameter (11 bits) */
  uint16 w_Ai1;        /* Effective Ionisation Level 2nd order parameter (11 bits) */
  uint16 w_Ai2;        /* Effective Ionisation Level 3rd order parameter (14 bits) */
  uint8  u_SF;         /* Ionospheric Disturbance Flags (5  bits) */
} gal_IonoStructType;

typedef struct
{
  uint8   b_fieldsPresent;
#define   GGTO_DRIFT_PRESENT        ((uint8) 0x1)
#define   GGTO_DRIFT_RATE_PRESENT   ((uint8) 0x2)
  /* bias coeff. of GPS Time Scale relative to GNSS TimeScale (sec) */
  FLT     f_TimeBias;
  /* Drift coeff. of GPS Time Scale relative to GNSS TimeSclae (sec/sec) */
  FLT     f_Drift;
  /* Drift rate coeff. of GPS Time Scale relative to GNSS TimeSclae (sec/sec) */
  FLT     f_DriftRate;
  int8    b_GGTO;
  uint16  w_TimeGGTO;
  uint16  w_WeekNumGGTO;
} gnssGPSTimeOffset;

typedef struct
{
  boolean            gpsGloTimeOffsetPresent;
  gnssGPSTimeOffset  gpsGloTimeOffset;
  /* Add other GPS-GNSS Time offsets below */
} gnssGPSTimeOffsets;

/*************************************************************************/
/* RF Dev Tracking Types                                                 */
/*************************************************************************/
/* Enums below for Multichannel Track */
typedef enum
{
  MultiChan_Track_RESERVED_0             = 0,
  MultiChan_Track_RESERVED_1,
  MultiChan_Track_L1_G1,
  MultiChan_Track_RESERVED_3,
  MultiChan_Track_L1,
  MultiChan_Track_G1,
  MultiChan_Track_L1_SBAS,
  MultiChan_L1_DPO, /* internal */
  MultiChan_Track_RESERVED_8,
  MultiChan_Track_RESERVED_9,
  MultiChan_Track_RESERVED_10,
  MultiChan_Track_RESERVED_11,
  MultiChan_Track_B1,
  MultiChan_Track_L1_B1,
  MultiChan_Track_L1_G1_B1,
  MultiChan_Track_L1_G1_B1_E1,
  MultiChan_Track_E1,
  MultiChan_Track_L1_E1,
  MultiChan_Track_L1_B1_E1,
  MultiChan_Track_L1_G1_E1,
  MultiChan_Track_L1_G1_B1_E1_L5E5,
  MultiChan_Track_L1_G1_B1_E1_L5E5_L2,
  MultiChan_Track_L1_G1_B1_E1_L5E5_L2_G2,
  MultiChan_Track_L1_B1_E1_L5E5_L2_B2,
  MultiChan_Track_L1_G1_E1_L5E5
} gnss_RfDevTrackEnumTypeVal;
typedef uint8 gnss_RfDevTrackEnumType;

typedef enum
{
  MultiChan_Track_L1_ENABLED = 0,
  MultiChan_Track_G1_ENABLED,
  MultiChan_Track_B1_ENABLED,
  MultiChan_Track_E1_ENABLED,
  MultiChan_Track_L5_ENABLED,
  MultiChan_Track_E5a_ENABLED,
  MultiChan_Track_L2_ENABLED,
  MultiChan_Track_B2_ENABLED,
  MultiChan_Track_G2_ENABLED
} gnss_RfDevTrackConstellationMaskVal;
typedef uint32 gnss_RfDevTrackConstellationMaskType;

typedef enum
{
  MultiChan_Track_Cmd_Start = 0,
  MultiChan_Track_Cmd_Stop,
  MultiChan_Track_Cmd_MAX
} gnss_RfDevTrackCmdTypeVal;
typedef uint8 gnss_RfDevTrackCmdType;

/*************************************************************************/
/* RF Dev MultiChan Track Test Command Information                                            */
/*************************************************************************/
typedef struct
{
  mgp_GnssType               e_GnssType;
  gnss_RfDevTrackEnumType    e_RfDevTrackInfo;
  uint16                     u_GloHwChanNum;
  uint16                     u_GpsSbasSvId;
  int8                       l_GloFreqId;
  boolean                    u_ClearFlag;
  uint8                      u_BdsSvId;
  uint8                      u_GalSvId;
  gnss_RfDevTrackCmdType     e_RfDevTrackCmd;
} gnss_RfDevCmdInfoStructType;

#if 0 /* for future use if needed */
/**************************************************************/
/*                                                            */
/*  CellGlobalIdGERAN                                         */
/*                                                            */
/**************************************************************/

typedef struct gnss_FTA_CellGlobalIdGERAN {
  gnss_ECGI cellIdentityGERANi;
  uint32 locationAreaCode;
} gnss_FTA_CellGlobalIdGERAN;

/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_gSM                                    */
/*                                                            */
/**************************************************************/

typedef struct gnss_FTA_NetworkTime_cellID_gSM {
  struct {
    unsigned cellGlobalIdGERANPresent : 1;
   } m;
  uint16 bcchCarrier;
  uint8 bsic;
  gnss_FTA_CellGlobalIdGERAN cellGlobalIdGERAN;
} gnss_FTA_NetworkTime_cellID_gSM;

/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_uTRA_mode_fdd                          */
/*                                                            */
/**************************************************************/

typedef struct gnss_FTA_NetworkTime_cellID_uTRA_mode_fdd {
  uint16 primary_CPICH_Info;
} gnss_FTA_NetworkTime_cellID_uTRA_mode_fdd;

/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_uTRA_mode_tdd                          */
/*                                                            */
/**************************************************************/

typedef struct gnss_FTA_NetworkTime_cellID_uTRA_mode_tdd {
  uint8 cellParameters;
} gnss_FTA_NetworkTime_cellID_uTRA_mode_tdd;


/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_uTRA_mode                              */
/*                                                            */
/**************************************************************/

/* Choice tag constants */

#define GNSS_FTA_NETWORKTIME_CELL_ID_UTRA_MODE_FDD 1
#define GNSS_FTA_NETWORKTIME_CELL_ID_UTRA_MODE_TDD 2

typedef struct NetworkTime_cellID_uTRA_mode {
  int t;
  union
  {
    /* t = 1 */
    gnss_FTA_NetworkTime_cellID_uTRA_mode_fdd fdd;
    /* t = 2 */
    gnss_FTA_NetworkTime_cellID_uTRA_mode_tdd tdd;
   } u;
} gnss_FTA_NetworkTime_cellID_uTRA_mode;

/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_uTRA                                   */
/*                                                            */
/**************************************************************/

typedef struct gnss_FTA_NetworkTime_cellID_uTRA {
  struct {
    unsigned cellGlobalIdUTRAPresent : 1;
   } m;
  gnss_FTA_NetworkTime_cellID_uTRA_mode mode;
  gnss_ECGI cellGlobalIdUTRA;
  uint16 uarfcn;
} gnss_FTA_NetworkTime_cellID_uTRA;
#endif


/**************************************************************/
/*                                                            */
/*  ECGI                                                      */
/*                                                            */
/**************************************************************/

typedef struct {
  uint8 mcc[3];
  uint8 mnc[3];
  uint32 cellidentity;
  uint8 num_mnc_digits;
  uint8 cell_id_numbits;
} gnss_ECGI;

/**************************************************************/
/*                                                            */
/*  NetworkTime_cellID_eUTRA                                  */
/*                                                            */
/**************************************************************/

typedef struct gnss_FTA_NetworkTime_cellID_eUTRA {
  struct {
    unsigned cellGlobalIdEUTRAPresent : 1;
  } m;
  uint16 physCellId;
  gnss_ECGI cellGlobalIdEUTRA;
  lte_earfcn_t earfcn; /* (0..262143) */
} gnss_FTA_NetworkTime_cellID_eUTRA;

#define GNSS_FTA_NETWORKTIME_CELLID_EUTRA      1
#if 0
#define GNSS_FTA_NETWORKTIME_CELLID_UTRA       2
#define GNSS_FTA_NETWORKTIME_CELLID_GSM        3
#endif
typedef struct gnss_NetworkTime_cellID {
  int t;
  union {
    /* t = 1 */
    gnss_FTA_NetworkTime_cellID_eUTRA eUTRA;
    /* t = 2 */
#if 0 /* Only eUTRA for now */
    gnss_FTA_NetworkTime_cellID_uTRA uTRA;
    /* t = 3 */
    gnss_FTA_NetworkTime_cellID_gSM gSM;
#endif
  } u;
} gnss_FTA_NetworkTime_cellID;
/**************************************************************/
/*                                                            */
/*  NetworkTime                                               */
/*                                                            */
/**************************************************************/

typedef struct gnss_FTA_NetworkTime {
  struct {
    unsigned frameDriftPresent : 1;
  } m;
  uint16 secondsFromFrameStructureStart;
  uint32 fractionalSecondsFromFrameStructureStart;
  uint8 frameDrift;
  gnss_FTA_NetworkTime_cellID cellID;
} gnss_FTA_NetworkTime;

/**************************************************************/
/*                                                            */
/*  GNSS_ReferenceTimeForOneCell_bsAlign                      */
/*                                                            */
/**************************************************************/

typedef enum {
  gnss_FTA_ReferenceTimeForOneCell_bsAlign_true_ = 0
} gnss_FTA_ReferenceTimeForOneCell_bsAlign_RootVal;
typedef uint8 gnss_FTA_ReferenceTimeForOneCell_bsAlign_Root;

/**************************************************************/
/*                                                            */
/*  GNSS_ReferenceTimeForOneCell                              */
/*                                                            */
/**************************************************************/

typedef struct gnss_FTA_ReferenceTimeForOneCell {
  struct {
    unsigned bsAlignPresent : 1;
  } m;
  gnss_FTA_NetworkTime networkTime;
  uint8 referenceTimeUnc;
  uint32 bsAlign;
} gnss_FTA_ReferenceTimeForOneCell;


/**************************************************************/
/*                                                            */
/*  GNSS_ReferenceTime_gnss_ReferenceTimeForCells             */
/*                                                            */
/**************************************************************/

#define GNSS_LOC_LTE_FTA_MAX_CELLS               16
typedef struct gnss_FTA_FTA_Information {
  uint32 n;
  gnss_FTA_ReferenceTimeForOneCell elem[GNSS_LOC_LTE_FTA_MAX_CELLS];
} gnss_FTA_ReferenceTimeForCells;

typedef struct gnss_FTA_Information
{
  struct {
    unsigned referenceTimeUncPresent : 1;
    unsigned gnss_ReferenceTimeForCellsPresent : 1;
  } m;
  gps_RefTimeStructType      z_RefTimeInfo; /* reference time provided in the AD*/
  uint8 referenceTimeUnc; /* reference time unc */
  gnss_FTA_ReferenceTimeForCells fta_ReferenceTimeForCells; /* FTA information */
}gnss_FTA_Information;

/*************************************************************************
  Union of Glonass assistance data structures                            
*************************************************************************/
typedef union
{
  /* Common */
  gnss_TimeModelType         z_TimeModelInfo;
  gnss_SvAcqAssistCommonStructType z_SvAcqAssistInfo;

  /* GPS session */
  gps_RefTimeStructType      z_RefTimeInfo;
  gps_RefLocStructType       z_RefLocInfo;
  gps_EphStructType          z_EphInfo;
  gps_AlmStructType          z_AlmInfo;
  gps_IonoStructType         z_IonoInfo;
  gps_RtiInfoStructType      z_RtiInfo;
  gps_SvNoExistStructType    z_SvNoExistInfo;
  gps_PreFixPpmSructType     z_PreFixPpmInfo;
  gps_UtcInfoRptStructType   z_UtcInfo;
  gps_SvUnhealthyStructType  z_SvUnhealthyInfo;
  gnss_AdvHealthStructType   z_GpsHealthInfo;
  gnssGPSTimeOffsets         z_GGTO;

  /* GLONASS session */
  glo_RefTimeStructType      z_GloRefTimeInfo;
  gnss_SvDirStructType       z_GnssSvDirInfo;
  glo_EphStructType          z_GloEphInfo;
  glo_AlmStructType          z_GloAlmInfo;
  gnss_SvNoExistStructType   z_GloNonExistenceInfo;
  glo_UnscaledUtcStructType  z_GloUtcInfo;
  gnss_AdvHealthStructType   z_GloHealthInfo;

  /* BeiDou session */
  bds_TimeStructType         z_BdsRefTime;
  bds_EphStructType          z_BdsEphInfo;
  bds_AlmStructType          z_BdsAlmInfo;
  bds_HealthStructType       z_BdsHealthInfo;
  bds_IonoStructType         z_BdsGlobalIonoInfo;  /* BeiDou Klobuchar iono */
  bds_IonoGridStructType     *pz_BdsIonoGridInfo; /* BeiDou Grid-based iono */
  gnss_AdvHealthStructType   z_BdsAdvHealthInfo; /* BeiDou unhealthy SV info */

  /* Galileo session */
  gal_EphStructType          z_GalEphInfo;
  gal_AlmStructType          z_GalAlmInfo;
  gal_HealthStructType       z_GalHealthInfo;
  gal_IonoStructType         z_GalIonoInfo;

  /* RF Dev Test session */
  gnss_RfDevCmdInfoStructType z_RfDevCmdInfo;
  gnss_FTA_Information        z_FTAInfo;
} gnss_AssistDataUnionType;


/*************************************************************************
  MGP GNSS Assistance data type                                        
*************************************************************************/
typedef enum
{
  MGP_GNSS_ASSIST_DATA_NONE = 0,
  /* Common to GPS + GLONASS system */
  MGP_GNSS_ASSIST_DATA_REF_TIME,
  MGP_GNSS_ASSIST_DATA_REF_LOC,
  MGP_GNSS_ASSIST_DATA_ACQ_ASSIST,
  MGP_GNSS_ASSIST_DATA_SV_DIR,
  MGP_GNSS_ASSIST_DATA_SA_DATA,
  MGP_GNSS_ASSIST_DATA_EPH,
  MGP_GNSS_ASSIST_DATA_ALM,
  MGP_GNSS_ASSIST_DATA_IONO,
  MGP_GNSS_ASSIST_DATA_RTI,
  MGP_GNSS_ASSIST_DATA_SV_NO_EXIST,
  MGP_GNSS_ASSIST_DATA_PRE_FIX_PPM,
  MGP_GNSS_ASSIST_DATA_UTC,
  MGP_GNSS_ASSIST_DATA_ALM_XTRA,
  MGP_GNSS_ASSIST_DATA_ADV_HEALTH,          /* GPS or GLONASS RealTimeIntegrity (RTI) */
  MGP_GNSS_ASSIST_DATA_RF_DEV_CMD_INFO,
  MGP_GNSS_ASSIST_DATA_GGTO,
  MGP_GNSS_ASSIST_DATA_SV_UNHEALTHY,        /* GPS Only */
  MGP_GNSS_ASSIST_DATA_FTA_DATA,
  MGP_GNSS_ASSIST_DATA_GNSS_TIME_MODEL,     /* GPS or GLONASS TimeModel */
  MGP_GNSS_ASSIST_DATA_BDS_GRID_MODEL,      /* BDS Iono grid model */
  MGP_GNSS_ASSIST_DATA_MAX
} gnss_AssistDataEnumTypeVal;
typedef uint8 gnss_AssistDataEnumType;

/*************************************************************************
  MGP GNSS Assistance data source                                        
*************************************************************************/
typedef enum
{
  GNSS_ASSIST_DATA_SOURCE_UNKNOWN,
  GNSS_ASSIST_DATA_SOURCE_XTRA,
  GNSS_ASSIST_DATA_SOURCE_AGNSS,
  GNSS_ASSIST_DATA_SOURCE_MAX
} gnss_AssistDataSourceEnumType;


/*************************************************************************
  GNSS assistance data                                                
*************************************************************************/
typedef struct
{
  mgp_GnssType             e_GnssType;
  gnss_AssistDataEnumType  e_AssistanceType;
  boolean                  u_Force; /* When true, the injected data will be accepted un-conditionally */
  gnss_AssistDataUnionType z_AssistData;
  gnss_AssistDataSourceEnumType e_AssistDataSource;

} gnss_AssistDataInfoStructType;

/*************************************************************************
  External position injection data type for ellipse position uncertanties
*************************************************************************/

/* Fields for horizontal position injection in the ellipse form.  */
typedef struct {

  DBL semiMajorUnc; /* Uncertainty: Non-zero positive number (meters).  */

  DBL semiMinorUnc; /* Uncertainty: Non-zero positive number (meters).  */

  DBL semiMajorAzimuthDegrees; /* Angle of semimajor from North: -90 to 90 (degrees).  */

} gnss_HorizontalUncertaintyEllipseType;


/*************************************************************************
  Navigation solution data type                                         
*************************************************************************/

/** This struct is for External Coarse Position Injection 
    To initialize safely, memset to 0 is okay, 
    le_InitExternCoarsePosInjection() is better. */
typedef struct
{
  /* indicating the presence/absence of the optional fields */
  gps_extern_coarse_pos_opt_field          opt_field_mask;

  /* info. of the time associated with the position. */
  gps_extern_coarse_pos_time_stamp_e_type  time_info_type;
  uint32                                   timestamp_sec; /** time of week or 
                                                              age, in sec. */

  gps_extern_coarse_pos_alt_def_e_type alt_def; /* def. of altitude */

  gps_extern_coarse_pos_source_e_type   source;

  gps_extern_coarse_pos_network_source_e_type network_source;

  DBL   latitude;  /* Lat.: in degrees -90.0 to 90.0, positive for northern */
  DBL   longitude; /* Long.: in degrees -180.0 to 180.0, positive for eastern */

  /* When opt_field_mask has the EXTERN_COARSE_POS_CIRCULAR bit set, this field 
     should contain a valid horizontal radius defining a circle.  

     On receipt of a coarse position injection in PE, the horizontal circular 
     position uncertainty is by definition 99% confidence (radial). All 
     necessary scaling to adhere to this definition is performed in SM. */
  FLT  loc_unc_hor;   /* horizontal location unc.: meters */

  FLT  loc_unc_hor_raw;   /*Non-optimized horizontal location unc.: meters */

  FLT   altitude; /* alt: meters, positive height, negative depth  */

  /* vertical unc */
  FLT   loc_unc_vert;   /* loc unc vertical: meters */

  U64     t_TimeTickMsec;  /* TimeTickMsec of current state (0 means unknown) */
  pdsm_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
  pdsm_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/

  /* source of altitude in injected position */
  pdsm_pd_altitude_source_e_type             e_AltitudeSource;
  /* linkage / dependence between injected horizontal position and altitude */
  pdsm_pd_altitude_horizontal_linkage_e_type e_AltHorLinkage;
  /* uncertainty coverage of the injected altitude */
  pdsm_pd_altitude_unc_coverage_e_type       e_AltUncCoverage;

  /* This field can be used instead of loc_unc_hor.  When opt_field_mask has the 
     EXTERN_COARSE_POS_ELLIPTICAL bit set, this struct should contain valid 
     horizontal ellipse data.  

     On receipt of a coarse position injection in PE, the horizontal circular 
     position uncertainty is by definition 99% confidence. All 
     necessary scaling to adhere to this definition is performed in SM. */
  gnss_HorizontalUncertaintyEllipseType horizontal_unc_elliptical;

} gnss_ExternCoarsePosInjectionType;

/* A generic GNSS bitmask type has one bit per GNSS SV,
   which is indexed by the SV ID */
typedef struct
{
  uint32 q_GpsPrnMask;    /*Bits[0-31]: GPS PRNs*/
  uint32 q_GloSlotIdMask; /*Bits[0-23]: GLO Slot IDs*/
  uint64 t_BdsPrnMask;    /*Bits[0-36]: BDS PRNs*/
  uint64 t_GalPrnMask;    /*Bits[0-35]: GAL PRNs*/
  uint8 u_QzssPrnMask;    /*Bits[0-4]: QZSS PRNs*/
} gnss_SvIdMaskStructType;

/* A structure containing per-SV usage info. The data is valid in the case of a
  GLE fix or no-fix. An SV is considered "used" in the fix if the pseudo-range 
  measuement was used to compute position. An SV is considered "usable" in the 
  fix if the measurement and SV polynomial validity criteria were satisfied. */
typedef struct
{
  boolean b_Valid;                   /* SV usage info validity */
  gnss_SvIdMaskStructType z_UsedGnssSvMask; /* Mask of GNSS SVs used in the fix */
  gnss_SvIdMaskStructType z_UsableGnssSvMask; /* Mask of GNSS SVs usable in the fix */
} gnss_SvUsageInfoStructType;

/* Inter-constellation Time Bias data as generated by PE */
typedef struct
{
  FLT f_UnfiltMeters;        /* Raw TB in meters */
  FLT f_UnfiltUncMeters;     /* Uncertianty of raw TB in
                                   meters */
  U32 q_FiltAgeMs;           /* Msec since last update */
  FLT f_FiltMeters;          /* Filtered TB in meters */
  FLT f_FiltUncMeters;       /* Uncertainty of filtered TB in meters */
  boolean b_IsPeTbConfident; /* TRUE: PE computed TB is confident, FALSE: otherwise. */
} gnss_PeTimeBiasStructType;

/* Navigation solution position flags. Each field is binary. The TRUE/FALSE 
  state is described below. */
typedef struct
{
  uint64 b_IsValid : 1;       /* Solution is/is not valid */
  uint64 b_IsPropagated : 1;  /* Solution is/is not propagated */
  uint64 b_IsWls : 1;         /* Solution is/is not WLS */
  uint64 b_IsKf : 1;          /* Solution is/is not KF */
  uint64 b_IsItarVln : 1;     /* Solution is/is not in violation of ITAR */
  uint64 b_IsOverDetrm : 1;   /* Solution is/is not over determined */
  uint64 b_IsExternal : 1;    /* Solution is/is not an external update */
  uint64 b_IsRefLoc : 1;      /* Solution is/is not a ref loc update */
  uint64 b_IsCoarsePos : 1;   /* Solution is/is not a coarse pos update */
  uint64 b_IsInternal : 1;    /* Solution is/is not an internal update */
  uint64 b_IsGpsUsed : 1;     /* GPS is/is not used in solution */
  uint64 b_IsGloUsed : 1;     /* GLO is/is not used in solution */
  uint64 b_IsBdsUsed : 1;     /* BDS is/is not used in solution */
  uint64 b_IsGalUsed : 1;     /* GAL is/is not used in solution */
  uint64 b_IsSft : 1;         /* Solution is/is not SFT */
  uint64 b_Is3D : 1;          /* Solution is/is not 3D */
  uint64 b_IsAltConstr : 1;   /* Solution is/is not alt constrained */
  uint64 b_IsGpsGloTBConstr : 1;  /* Solution is/is not GpsGloTB constrained */
  uint64 b_IsGpsBdsTBConstr : 1;  /* Solution is/is not GpsBdsTB constrained */
  uint64 b_IsBdsGloTBConstr : 1;  /* Solution is/is not BdsGloTB constrained */
  uint64 b_IsMagDevValid : 1; /* Magnetic deviation is/is not valid */
  uint64 b_IsFinal : 1;       /* Solution is/is not final */
  uint64 b_IsBackground : 1;  /* Solution is/is not background */
  uint64 b_IsPeGenerated : 1; /* Solution is/is not PE generated */
  uint64 b_IsMeGenerated : 1; /* Solution is/is not ME generated */
  uint64 b_IsGpsXtraSln : 1;     /* Solution is/is not using GPS XTRA */
  uint64 b_IsGpsXtraEphSln : 1;  /* Solution is/is not using GPS XTRA and 
                                  ephemeris */
  uint64 b_IsGloXtraSln : 1;     /* Solution is/is not using Glonass XTRA */
  uint64 b_IsGloXtraEphSln : 1;  /* Solution is/is not using Glonass XTRA and 
                                  ephemeris */
  uint64 b_IsBdsXtraSln : 1;     /* Solution is/is not using Beidou XTRA */
  uint64 b_IsBdsXtraEphSln : 1;  /* Solution is/is not using Beidou XTRA and 
                                  ephemeris */
  uint64 b_IsWifiGenerated : 1;  /* Solution is/is not WIFI generated */
  uint64 b_IsHeadingValid : 1;   /* Heading is/is not valid */
  uint64 b_IsVelValid : 1;    /* Velocity is/is not valid */
  uint64 b_IsEpiHzConstr : 1;   /* Solution does/does not use an EPI as a 
                                  horizontal constraint */
  uint64 b_IsEpiAltConstr : 1;  /* Solution does/does not use an EPI as an 
                                  altitude constraint */
  uint64 b_IsStatic : 1;      /* Solution is/is not in KF static mode */
  uint64 b_IsSpiValid : 1;    /* SPI is/is not valid */
  uint64 b_IsSpiStationary : 1;  /* SPI is/is not indicating stationary. Only
                                    valid if b_XspiValid == TRUE */
  uint64 b_IsPosAssistValid : 1;  /* Initial position is/is not valid for this 
                                    second. */
  uint64 b_IsGile : 1;          /* Solution source is/is not GILE */
  uint64 b_IsGnssInsSln : 1;   /* Solution used GNSS and INS measurements */
  uint64 b_IsInsOnlySln : 1;   /* Solution used Only INS measurements */
  uint64 b_IsKfInitFromEpi : 1;   /* KF is/is not initialized from an EPI */
  uint64 b_IsKfPvFiltered : 1;    /* KF is/is not a PV filtered result */
  uint64 b_IsXmsiKnown : 1;    /* XMSI is known = true, false otherwise */
  uint64 b_IsXmsiMounted : 1;  /* XMSI is mounted = true, false otherwise */
  uint64 b_IsMsiKnown : 1;     /*  MSI is known = true, false otherwise */
  uint64 b_IsMsiMounted : 1;  /*  MSI is mounted = true, false otherwise */
  uint64 b_IsCrdDetected : 1;  /*  Cradle Rotation is = true, false otherwise */
  uint64 b_IsNhcUsedForHeading : 1; /* NHC used for GILE heading = TRUE */
  uint64 b_IsEcid : 1;         /* Solution is an E-CID position = TRUE */
  uint64 b_IsPle: 1;          /* Solution is/is not a PLE */
  uint64 b_IsCellid : 1;         /* Solution is an Cell ID position = TRUE */
  uint64 b_IsZppFinal : 1;     /* Solution is for ZPP reported position = TRUE */
  uint64 b_IsVehOnlySln : 1;   /* Solution used only vehicle sensor measurements */
  uint64 b_IsPedestrianDrSln : 1; /* Solution used only pedestrian sensors */
  uint64 b_IsGpsGcf : 1; /* Solution derived while GPS was in GCF */
  uint64 b_IsGloGcf : 1; /* Solution derived while GLO was in GCF */
  uint64 b_IsBdsGcf : 1; /* Solution derived while BDS was in GCF */
  uint64 b_IsGalGcf : 1; /* Solution derived while GAL was in GCF */
  uint64 b_IsPedWithoutGnss : 1; /* flag to indicate pedestrian in w/o GNSS,
                                      used for time-out check */
  uint64 b_IsPedWithoutGnssTimeout : 1; /* flag to indicate pedestrian w/o GNSS timeout */
  uint64 b_IsInPedMode : 1;  /* flag to indicate pedestrian mode */
  uint64 b_IsCogUsed : 1;       /* Solution used COG data */
  uint64 b_IsSpdInputUsed : 1;  /* Solution used Speed input data */
  uint64 b_IsQzssUsed : 1;     /* QZSS is/is not used in solution */
  uint64 b_IsQzssXtraSln : 1;     /* Solution is/is not using QZSS XTRA */
  uint64 b_IsQzssXtraEphSln : 1;  /* Solution is/is not using QZSS XTRA and
                                  ephemeris */
  uint64 b_IsQzssGcf : 1; /* Solution derived while QZSS was in GCF */

  uint64 b_IsGpsGalTBConstr:1;   /* Solution is/is not GpsGalTB constrained */
  uint64 b_IsGalGloTBConstr:1;   /* Solution is/is not GalGloTB constrained */
  uint64 b_IsGalBdsTBConstr:1;   /* Solution is/is not GalBdsTB constrained */
  uint64 b_IsGalXtraSln:1;    /* Solution is/is not using Galileo XTRA */
  uint64 b_IsGalXtraEphSln:1; /* Solution is/is not using Galileo XTRA and 
                                  ephemeris */
  uint64 b_IsSbasIonoCorr:1;    /* Solution is/is not using SBAS ionospheric correction */
  uint64 b_IsSbasFastCorr:1;    /* Solution is/is not using SBAS fast correction */
  uint64 b_IsSbasLongCorr:1;    /* Solution is/is not using SBAS long-term correction */
  uint64 b_IsSbasIntegrity:1; /* Solution is/is not using SBAS integrity information */
  uint64 b_PVAPropagationUsedFixRpt:1; /* acceleration derived from fix history used */
  uint64 b_IsTdpUsed:1;       /* TDP Meas are/are not used in the solution */
  uint64 b_IsINP:1; /* Position is a Internal (Qualcomm) network provider position */
  uint64 b_IsExtNP:1; /* Position is an External Network provider position */
  uint64 b_FullGnssUsedFixRpt:1;    /* Fix used full GNSS meas information */
  uint64 b_NHzGnssUsedFixRpt:1;     /* Fix used Fine speed only GNSS meas information */
  uint64 b_IsGnssLePos:1; /* The fix is qualified as GNSS-LE fix */
  uint64 b_IsSmoother:1; /* Ped smoother is used */
  uint64 b_IsSmootherPlusClipping:1; /* Ped smoother + clipping is used */
  uint64 b_IsSpeedCorrected:1; /* Speed correction is applied */
  uint64 b_IsGnssIndoor:1;   /* device is indoor as per gnss signal conditions */
  uint64 b_IsGnssChallenged:1;  /* device is in challenged gnss signal conditions */
  uint64 b_IsOemDreFix:1; /* If this fix is to be shipped to OEM DRE client */
  uint64 v_IsDgpsModeEngaged: 1;     /* Flag to indicate if D-GPS mode engaged or not */

  uint64 : 35;  /* Padding to align with 8 byte boundary */
} gnss_NavSlnFlagsStructType;

/* Fields to indicate if SV is corrected with SBAS information */
typedef struct
{
  uint32 v_SbasIonoCorrApplied:1;    /* SBAS ionospheric correction applied/not applied to SV */
  uint32 v_SbasFastCorrApplied:1;    /* SBAS Fast correction applied/not applied to SV  */
  uint32 v_SbasLongCorrApplied:1;    /* SBAS Long-term correction applied/not applied to SV  */
  uint32 v_SbasIntergrityFail:1;    /* SV is/not using SBAS integrity information */
  uint32 :28;    /* Padding to align with 4 byte boundary */
} gnss_SbasCorrStructType;

/* Indices of particular constellation data inside unified arrays. 
   The order of the defintitions shouldn't be changed.
   Check before making changes to this type, because it's used in NF DM LOGs.
  */
typedef enum
{
  BCN_IDX_GPS = 0,
  BCN_IDX_GLO,
  BCN_IDX_BDS,
  BCN_IDX_GAL,
  BCN_IDX_QZSS,
  BCN_IDX_TDP,
  BCN_IDX_GNSS_TOTAL = BCN_IDX_TDP,
  BCN_IDX_TOTAL,
  BCN_IDX_INVALID = BCN_IDX_TOTAL,
} bcn_IdxEnumType;

/*enum for all the supported GNSS single frequency measurement types
  The order of the defintitions shouldn't be changed.
  Check before making changes to this type, because it's used in NF DM LOGs.
*/
typedef enum
{
  C_GNSS_SFMT_GPS_L1CA = 0,
  C_GNSS_SFMT_GLO_G1,
  C_GNSS_SFMT_BDS_B1,
  C_GNSS_SFMT_GAL_E1,
  C_GNSS_SFMT_QZSS_L1CA,
  C_GNSS_SFMT_LEGACY_TOTAL,
  C_GNSS_SFMT_GPS_L2C = C_GNSS_SFMT_LEGACY_TOTAL,
  C_GNSS_SFMT_GPS_L5Q,  
  C_GNSS_SFMT_GLO_G2,  
  C_GNSS_SFMT_BDS_B2,  
  C_GNSS_SFMT_GAL_E5A,  
  C_GNSS_SFMT_QZSS_L2C,
  C_GNSS_SFMT_QZSS_L5Q,
  C_GNSS_SFMT_MAX,
} gnss_SfmtEnumTypeVal;
typedef uint8 gnss_SfmtEnumType;

/* Type of inter-system time difference/bias estimated 
 Check before making changes to this type, because it's used in NF DM LOGs.
*/
typedef enum
{
  GNSS_TB_GPSGLO = 0,
  GNSS_TB_GPSBDS,
  GNSS_TB_BDSGLO,
  GNSS_TB_GPSGAL,
  GNSS_TB_GALGLO,
  GNSS_TB_GALBDS,
  GNSS_TB_TOTNUM
} gnss_TBEnumType;

typedef struct
{
  boolean b_Process; /*This field is used internally only in NF, deprecated by NV item*/
  U8   u_GcfLogic[BCN_IDX_GNSS_TOTAL];
  U8   u_GcfStatus[BCN_IDX_GNSS_TOTAL];     /* When TRUE: GCF is on ON */
  boolean b_PendingGcf[BCN_IDX_GNSS_TOTAL]; /* When TRUE: pending GCF setting 
                                            need to be carried in next track message  */
  gnss_SvIdMaskStructType z_GnssSvFailure;
} gnss_GcfStructType;

typedef enum
{
  TECHNOLOGIES_UNSPECIFIED_INTERNAL = 0,
  TECHNOLOGIES_UNSPECIFIED_EXTERNAL,
  TECHNOLOGIES_GPS,
  TECHNOLOGIES_GLONASS,
  TECHNOLOGIES_BEIDOU,
  TECHNOLOGIES_QZSS,
  TECHNOLOGIES_GALILEO,
  TECHNOLOGIES_SENSORS_ON_DEVICE,
  TECHNOLOGIES_SENSORS_ON_VEHICLE,
  TECHNOLOGIES_WIFI_COVERAGE,
  TECHNOLOGIES_WWAN_COVERAGE,
  TECHNOLOGIES_WIFI_RANGING,
  TECHNOLOGIES_WWAN_RANGING,
  TECHNOLOGIES_MAX,
  TECHNOLOGIES_INVALID = TECHNOLOGIES_MAX
} gnss_TechnologiesIdxType;

#define GET_GNSS_TECH_CONTRIBUTION(x)  \
  ((x)[TECHNOLOGIES_GPS] + (x)[TECHNOLOGIES_GLONASS] + \
   (x)[TECHNOLOGIES_BEIDOU] + (x)[TECHNOLOGIES_QZSS] + \
   (x)[TECHNOLOGIES_GALILEO])

typedef enum
{
  GNSS_PQWP_TYPE_1 = 1, /* EPI information                    (from ALE) */
  GNSS_PQWP_TYPE_2,     /* Best Position information          (from ALE) */
  GNSS_PQWP_TYPE_3,     /* XTRA related information           (from CD)  */
  GNSS_PQWP_TYPE_4,     /* Ephemeris availability information (from CD)  */
  GNSS_PQWP_TYPE_5,     /* Health information                 (from CD)  */
  GNSS_PQWP_TYPE_6,     /* SAP information                    (from ALE) */
  GNSS_PQWP_TYPE_7      /* SV data related information        (from CD)  */
}gnss_PQWEnumType;
/* Navigation solution data at a specific time */
typedef struct
{
  /* Flags that describe the navigation solution */
  gnss_NavSlnFlagsStructType z_NavSolutionFlags;
  FLT f_TechContributions[TECHNOLOGIES_MAX]; /* Technology contributions.  */

  uint16 w_GpsWeek;       /* GPS week number */
  uint32 q_GpsTimeMs;     /* GPS time in week (milliseconds) */
  uint8 u_GloNumFourYear; /* Glonass four-year cycle number */
  uint16 w_GloNumDaysInFourYear;  /* Glonass calendar day within the current 
                                    four-year cycle */
  uint32 q_GloTimeMs;     /* Glonass time in day (milliseconds) */
  uint32 q_UtcTimeMs;     /* UTC time (milliseconds) */
  uint8 u_DeltaLeapSec;   /* Leap second offset between GPS time and Glonass 
                            time/ UTC time */
  uint16 w_BdsWeek;       /* BDS week number */
  uint32 q_BdsTimeMs;     /* BDS time in week (milliseconds) */
  uint16 w_GalWeek;       /* GAL week number */
  uint32 q_GalTimeMs;     /* GAL time in week (milliseconds) */
  uint32 q_RefFcount;     /* Receiver's ms counter value. */
  uint64 t_TimeTickMsec;  /* Receiver's time tick ms counter value */
  FLT f_InitGpsTuncMs;    /* Initial GPS time uncertainty in milliseconds */
  U8 u_InitGpsTimeSrc;    /* Initial source of GPS time */
  DBL d_PosEcef[3];       /* ECEF position (m) */
  DBL d_PosLla[3];        /* LLA position (rad, m) */
  FLT f_Punc3dMeters;     /* Uncertainty of position (m) */
  FLT f_PuncLatLonMeters[2];  /** Latitude, longitude uncertianty (m), 
                                  68% conf. within +/- this 1-sigma value */
  FLT f_PuncVertMeters;         /* Vertical uncertainty (m) */
  FLT f_ErrorEllipse[3];  /** Unc angle (deg), unc a (m), unc b (m)
                              39% conf. within this one-sigma ellipse */
  pdsm_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
  pdsm_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/
  DBL d_VelEcef[3];       /* Velocity in ECEF (m/s) */
  DBL d_VelEnu[3];        /* Velocity in ENU (m/s) */
  DBL d_CorrVelEcef[3];       /* Corrected Velocity in ECEF (m/s) */
  DBL d_CorrVelEnu[3];        /* Corrected Velocity in ENU (m/s) */
  FLT f_HeadingRad;       /* Heading angle, [0,2*pi) (rad). Referenced to North
                            (0 rad) and positive clock-wise. This is the "best" estimate
                            of heading and uses sensor data (if available and enabled).
                            See also: f_GnssHeadingRad below */
  FLT f_HeadingUncRad;    /* Heading uncertainty in Radians */
  FLT f_Vunc3dMps;              /* Velocity uncertainty (m/s) */
  FLT f_VuncEastNorthMps[2];    /* East, North uncertianty (m/s) */
  FLT f_VuncVertMps;      /* Vertical uncertainty (m/s) */
  FLT f_ClockBiasMs;      /* Receiver's clock bias (m). */
  mgp_GnssType e_ClockBiasAlignGnss;
  FLT f_ClockBiasUncMs;   /* Receiver's clock uncertainty (m). 3-sigma value */
  FLT f_ClockFreqBiasMps;     /* Receiver's clock frequency bias (m/s). */
  FLT f_ClockFreqBiasUncMps;  /* Receiver's clock frequency bias uncertainty 
                                (m/s). */

  gnss_PeTimeBiasStructType z_InterGnssTb[GNSS_TB_TOTNUM]; /* Inter-constellation Time Biases */

  FLT f_UnfilteredAlt;      /* Raw altitude (m). */
  FLT f_UnfilteredAltUnc;   /* Raw altitude uncertainty, one-sided (m). */
  FLT f_FilteredAlt;      /* Filtered altitude (m). */
  FLT f_FilteredAltUnc;   /* Filtered altitude uncertainty, one-sided (m). */
  FLT f_SftOffsetSec;     /* SFT offset solution (seconds). */
  FLT f_SftOffsetUncSec;  /* SFT offset solution uncertainty (seconds). */
  FLT f_MagDevDegrees;    /* Magnetic deviation from true north (degrees). */
  FLT f_PDOP;             /* Position Dilution of Precision (unitless). */
  FLT f_HDOP;             /* Horizontal Dilution of Precision (unitless). */
  FLT f_VDOP;             /* Vertical Dilution of Precision (unitless). */
  FLT f_TDOP;             /* Time Dilution of Precision (unitless). */
  FLT f_GDOP;             /* Geometrical Dilution of Precision (unitless). */
  boolean b_HaveFirstFix; /* Set to TRUE if the first fix was computed. */

  FLT f_GnssHeadingRad;       /* Heading angle, [0,2*pi) (rad). Referenced to North
                                (0 rad), increasing clock-wise. This is the heading 
                                estimate that is based on GNSS measurements only. 
                                Sensor aiding, if enabled does NOT affect this member,
                                See also f_HeadingRad above. */
  FLT f_GnssHeadingUncRad;   /* Heading uncertainty in radians */
  uint32 q_SensorDataUsageMask;    /* Denotes which additional sensor data
                                           were used to compute this position 
                                           fix.
                                        BIT[0] 0x00000001 – Accelerometer
                                        BIT[1] 0x00000002 – Gyro
                                        BIT[0] 0x00000004 – Vehicle odometer
                                        BIT[1] 0x00000008 – Vehicle gyro
                                        BIT[0] 0x00000010 – Pedometer
                                        BIT[1] 0x00000020 – Pedestrian alignment
                                        0x0000FFFC - Reserved
                                        A bit set to 1 indicates that certain 
                                        fields as defined by the 
                                        SENSOR_AIDING_MASK were aided with 
                                        sensor data*/
  uint32 q_SensorAidMask;         /* Denotes which component of the 
                                          position report was assisted with 
                                          additional sensors defined in 
                                          SENSOR_DATA_USAGE_MASK
                                       BIT[0] 0x00000001 – 
                                         Heading aided with sensor data
                                       BIT[1] 0x00000002 – 
                                         Speed aided with sensor data
                                       BIT[2] 0x00000004 – 
                                         Position aided with sensor data
                                       BIT[3] 0x00000008 – 
                                         Velocity aided with sensor data
                                       0xFFFFFFF0 – Reserved */

  gnss_SvUsageInfoStructType z_SvUsageInfo; /* Information describing the usage 
                                         of GNSS SV's by the GNSS engine */

  /*The following measurement data in size of  N_ACTIVE_GNSS_CHANis filled in the order of GPS, GLO and BDS.
    The is no gap or overlap between the data from any two GNSS constellations.*/

  /* position fix data */
  U8 u_NumPrMeas[BCN_IDX_TOTAL]; /* Number of Bcns used in the position fix. */
  U16 w_PrMeasBcn[N_ACTIVE_MEAS_CHAN]; /* PRNs of the GPS/BDS/GAL SVs, or Slot
                                         IDs of the GLO Svs or PhyId of TDP meas
                                         used in the position fix. */

  U16 w_PrMeasIode[N_ACTIVE_MEAS_CHAN]; /* IODEs of the GPS/BDS/GAL SVs or Tbs of the Glonass SVs used in the 
                                           position fix. */

  S8 b_PrMeasFreqNum[N_ACTIVE_MEAS_CHAN]; /* Glonass frequency number 
                                                 (-7 to +6) of SVs used in the
                                                 position fix */
  FLT f_PrMeasResMeters[N_ACTIVE_MEAS_CHAN]; /* Pseudo-range measurement 
                                                residuals of the Bcns 
                                                used in the position fix. */
  FLT f_PrMeasUncMeters[N_ACTIVE_MEAS_CHAN]; /* Pseudo-range uncertainties of 
                                                the Bcns used in the 
                                                position fix. */
  /* velocity fix data */
  U8 u_NumPrrMeas[BCN_IDX_TOTAL]; /* Number of Bcn used in the velocity fix. */
  U16 w_PrrMeasBcn[N_ACTIVE_MEAS_CHAN];  /* PRNs of the GPS/BDS/GAL SVs, or Slot
                                           IDs of the GLO Svs or PhyIds of TDP meas
                                           used in the velocity fix. */
  FLT f_PrrMeasResMps[N_ACTIVE_MEAS_CHAN]; /* Pseudo-range rate measurement 
                                              residuals of the Bcn used 
                                              in the velocity fix. */
  FLT f_PrrMeasUncMps[N_ACTIVE_MEAS_CHAN]; /* Pseudo-range rate uncertainties 
                                              of the Bcn used in the 
                                              velocity fix. */
  /* measurement block data */
  U8 u_NumMeas[BCN_IDX_TOTAL]; /* Number of Bcn received in the measurement block. */
  U16 w_MeasBcn[N_ACTIVE_MEAS_CHAN]; /* PRNs of the GPS/BDS/GAL SVs, or Slot
                                       IDs of the GLO Svs or PhyId of the TDP meas
                                       received in the measurement block. */
  S8 b_MeasFreqNum[N_ACTIVE_MEAS_CHAN]; /* Frequency numbers of the 
                                          Glonass SVs received in the 
                                          the measurement block.  */

  pdsm_pd_sensor_param_report   z_OnDeviceSensorParam;  /* On-device sensor bias data */

  gnss_SbasCorrStructType z_SbasCorrAvail[N_ACTIVE_GNSS_CHAN]; /* SBAS info usage */

} gnss_NavSolutionStructType;


/*************************************************************************
  GNSS delete info bit mask                                              
*************************************************************************/
typedef struct
{
  mgp_GnssType e_GnssType;
  boolean u_DeleteAll;  /* Delete specific GNSS system, indicated by GnssType*/
  boolean u_DeleteUtc;
  boolean u_DeleteXtra;
  boolean v_DeleteIono;
  boolean u_DeleteFreqBiasEst;
  boolean v_DeleteXtraIntegrity; /*flags whether XTRA integrity data is to be deleted*/

  uint32 q_DeleteClockMask; /* Refer C_DELETE_INFO_CLOCK_TIME_EST, etc.
                               in gps_common.h. from Gen 8 */
  uint32 q_DeletePosMask;   /* Refer C_DELETE_INFO_POS_ALT_INFO, etc in
                               gps_common.h. from Gen 8 */
  uint32 q_DeleteGpsEphMask;   /*!< GPS 0-31 */
  uint32 q_DeleteGpsAlmMask;   /*!< GPS 0-31 */
  uint32 q_DeleteQzssEphMask;  /*!< QZSS 0-4 */
  uint32 q_DeleteQzssAlmMask;  /*!< QZSS 0-4 */
  uint32 q_DeleteGloEphMask;   /*!< GLO 0-13 */
  uint32 q_DeleteGloAlmMask;   /*!< GLO 0-13 */
  uint32 q_DeleteSbasEphMask; /* bit 0-18 -> SBAS ID: 120-138 */
  uint32 q_DeleteSbasAlmMask; /* bit 0-18 -> SBAS ID: 120-138 */
  uint64 t_DeleteBdsEphMask; /* bit 0-36 -> BDS ID: 201-237 */
  uint64 t_DeleteBdsAlmMask; /* bit 0-36 -> BDS ID: 201-237 */

  uint64 t_DeleteSvDirMask;   /*!< GPS 0-31, QZSS 32-36, Glo Frequency Index :bit0-13 ->-7..+6*/
  uint64 t_DeleteSvSteerMask; /*!< GPS 0-31, QZSS 32-36, Glo Frequency Index: bit0-13 ->-7..+6*/

  uint64 t_DeleteBdsSvDirMask;     /* bit 0-36 -> BDS ID: 201-237 */
  uint64 t_DeleteBdsSvSteerMask;   /* bit 0-36 -> BDS ID: 201-237 */
  uint64 t_DeleteBdsSvHealthMask;  /* bit 0-36 -> BDS ID: 201-237 */

  uint32 q_DeleteGpsSvHealthMask;  /* GPS 0-31 */
  uint32 q_DeleteQzssSvHealthMask; /* QZSS 0-4 */
  uint32 q_DeleteGloSvHealthMask;  /* Glo Frequency Index: bit0-13 */

  uint32 q_DeleteSaDataMask;
  uint32 q_DeleteRtiMask;
  uint32 q_DeleteCellDBMask;
  uint32 q_DeleteSbasSvDirMask; /* bit 0-18 -> SBAS ID: 120-138 */
  uint32 q_DeleteSbasSvSteerMask; /* bit 0-18 -> SBAS ID: 120-138 */
  uint32 q_DeleteSbasAcqMask; /* bit 0-18 -> SBAS ID: 120-138 */
  uint64 t_DeleteSvNoExistMask;  /*!< GPS 0-31, QZSS 32-36 */

  uint64 t_DeleteBlacklistGpsMask; /*GPS: bit 0-31 + 32-36 = PRN 1-32  + QZSS 193-197*/
  uint64 t_DeleteBlacklistGloMask; /*GLO: bit 0-23 = SlotId 1-24 */
  uint64 t_DeleteBlacklistBdsMask; /*BDS: bit 0-36 = PRN 1-37*/
  uint64 t_DeleteBlacklistGalMask; /*BDS: bit 0-35 = PRN 1-36*/
  uint64 t_DeleteGalSvDirMask;     /* bit 0-35 -> GAL SVID: 301-336 */
  uint64 t_DeleteGalSvSteerMask;   /* bit 0-35 -> GAL SVID: 301-336 */
  uint64 t_DeleteGalSvHealthMask;  /* bit 0-35 -> GAL SVID: 301-336 */
  uint64 t_DeleteGalEphMask; /* bit 0-35 -> GAL SVID: 301-336 */
  uint64 t_DeleteGalAlmMask; /* bit 0-35 -> GAL SVID: 301-336 */  
  boolean v_DeleteTgd;       /*TGD for all the constillations*/
} gnss_DeleteInfoStructType;

/*************************************************************************
  Defines for LOG_GNSS_POSITION_REPORT_C, 0x1476
*************************************************************************/
#ifdef FEATURE_CGPS_PLAYBACK
#pragma pack(push)
#pragma pack(1)
#endif

typedef PACKED struct PACKED_POST
{
  uint32      q_FltTBMeters;          /* inter-GNSS time bias in 
                                        meters */
  uint32      q_FltTBSigmaMeters;     /* Gaussian 1-sigma value for inter-GNSS
                                      time bias uncertainty in meters */
  uint32      q_FltFiltMeters;      /* Filtered inter-GNSS time bias in meters */
  uint32      q_FltFiltTBSigmaMeters; /* Filtered Gaussian 1-sigma value for 
                                          inter-GNSS time bias uncertainty 
                                          in meters */
} gnss_PositionRptPerInterGnssTbDataDmLog;

typedef PACKED struct PACKED_POST
{
  uint8       u_NumBcnsUsed;          /* The number of GNSS SVs used in 
                                          the fix */
  uint8       u_TotalBcns;            /* Total number of GNSS SVs detected 
                                          by searcher, including ones not used 
                                          in position calculation */
} gnss_PositionRptNumBcnDataDmLog;

typedef PACKED struct PACKED_POST
{
  uint8       u_BcnType;                /* BCN type bit field: 
                                          (see DM log 0x1476 documentation) */
  uint16      w_BcnId;                  /* BCN ID, GNSS SV PRN or Slot ID
                                          if the SV is Glonass. */
  uint8       u_IntFrequencyNum;        /* Frequency number (-7 to +6) of the
                                          Glonass SV. Only valid for Glonass 
                                          SVs. */
  uint16      w_Iode;                   /* IODE for GNSS SV, Tb if the SV 
                                          is Glonass. */
  uint32      q_FltPrResidual;          /* A posteriori pseudo-range 
                                          measurement residual from the 
                                          position fix. */
  uint32      q_FltPrrResidual;         /* A posteriori pseudo-range rate
                                          measurement residual from the 
                                          velocity fix. */
  uint32      q_FltPrUnc;               /* Pseudo-range measurement 
                                          uncertainty */
  uint32      q_FltPrrUnc;              /* Pseudo-range rate measurement 
                                          uncertainty */
} gnss_PositionRptPerBcnDataDmLog;


typedef enum
{
  DMLOG_GNSS_POSRPT_GNSSTB_GPSGLO = 0,
  DMLOG_GNSS_POSRPT_GNSSTB_GPSBDS,
  DMLOG_GNSS_POSRPT_GNSSTB_BDSGLO,
  DMLOG_GNSS_POSRPT_GNSSTB_GPSGAL,
  DMLOG_GNSS_POSRPT_GNSSTB_GALGLO,
  DMLOG_GNSS_POSRPT_GNSSTB_GALBDS,
  DMLOG_GNSS_POSRPT_GNSSTB_TOTAL
} gnss_GnssPosRptGnssTbIdxEnumType;

typedef enum
{
  DMLOG_GNSS_POSRPT_BCN_IDX_GPS = 0,
  DMLOG_GNSS_POSRPT_BCN_IDX_GLO,
  DMLOG_GNSS_POSRPT_BCN_IDX_BDS,
  DMLOG_GNSS_POSRPT_BCN_IDX_GAL,
  DMLOG_GNSS_POSRPT_BCN_IDX_TDP,
  DMLOG_GNSS_POSRPT_BCN_IDX_QZSS,
  DMLOG_GNSS_POSRPT_BCN_IDX_TOTAL,
} gnss_GnssPosRptBcnIdxEnumType;

#define DMLOG_GNSS_POSRPT_TECHNOLOGIES_MAX (13)

#define DMLOG_GNSS_POSRPT_N_ACTIVE_MEAS (N_ACTIVE_GPS_CHAN + \
  N_ACTIVE_GLO_CHAN + N_ACTIVE_BDS_CHAN +  N_ACTIVE_GAL_CHAN + \
  N_ACTIVE_TDP_CHAN + N_QZSS_SV)


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint8       u_PosSource;              /* Source of position information */
  /*  0: None
      1: Weighted least-squares
      2: Kalman filter
      3: Externally injected
      4: Internal database    */
  uint32      q_Reserved1;              /* Reserved memory field */
  uint16      w_PosVelFlag;             /* Position velocity bit field: 
                                          (see DM log 0x1476 documentation) */
  uint64      t_PosVelFlag2;            /* Position velocity 2 bit field:
                                          (see DM log 0x1476 documentation) */
  uint8       u_FailureCode;            /* Failure code:
                                          (see DM log 0x1476 documentation) */
  uint16      w_FixEvents;              /* Fix events bit field:
                                          (see DM log 0x1476 documentation) */
  uint16      w_GpsWeekNumber;          /* GPS week number of position */
  uint32      q_GpsFixTimeMs;           /* GPS fix time of week of in 
                                          milliseconds */
  uint8       u_GloNumFourYear;         /* Number of Glonass four year 
                                          cycles */
  uint16      w_GloNumDaysInFourYear;   /* Glonass calendar day in four year 
                                          cycle */
  uint32      q_GloFixTimeMs;           /* Glonass fix time of day in 
                                          milliseconds */
  uint32      q_PosCount;               /* Integer count of the number of 
                                          unique positions reported */
  uint64      t_DblFinalPosLatLon[2];   /* Final latitude and longitude
                                          of position in radians */
  uint32      q_FltFinalPosAlt;         /* Final height-above-ellipsoid 
                                          altitude of position */
  uint32      q_FltHeadingRad;          /* User heading in radians */
  uint32      q_FltHeadingUncRad;       /* User heading uncertainty in radians */
  uint32      q_FltVelEnuMps[3];        /* User velocity in east, north, up
                                          coordinate frame. In meters per 
                                          second. */
  uint32      q_FltVelSigmaMps[3];      /* Gaussian 1-sigma value for east, 
                                          north, up components of user 
                                          velocity */
  uint32      q_FltClockBiasMeters;     /* Receiver clock bias in meters */
  uint32      q_FltClockBiasSigmaMeters;  /* Gaussian 1-sigma value for 
                                            receiver clock bias in meters */

  /*inter-GNSS TBs*/
  gnss_PositionRptPerInterGnssTbDataDmLog z_InterGnssTb[DMLOG_GNSS_POSRPT_GNSSTB_TOTAL];

  uint32      q_FltSftOffsetSec;        /* SFT offset as computed by WLS in 
                                          seconds */
  uint32      q_FltSftOffsetSigmaSec;   /* Gaussian 1-sigma value for SFT 
                                          offset in seconds */
  uint32      q_FltClockDriftMps;       /* Clock drift (clock frequency bias) 
                                          in meters per second */
  uint32      q_FltClockDriftSigmaMps;  /* Gaussian 1-sigma value for clock 
                                          drift in meters per second */
  uint32      q_FltFilteredAlt;         /* Filtered height-above-ellipsoid
                                          altitude in meters as computed by 
                                          WLS */
  uint32      q_FltFilteredAltSigma;    /* Gaussian 1-sigma value for filtered 
                                          height-above-ellipsoid altitude in 
                                          meters */
  uint32      q_FltRawAlt;              /* Raw height-above-ellipsoid altitude 
                                          in meters as computed by WLS */
  uint32      q_FltRawAltSigma;         /* Gaussian 1-sigma value for raw 
                                          height-above-ellipsoid altitude in 
                                          meters */
  uint32      q_FltPdop;                /* 3D position dilution of precision as 
                                          computed from the unweighted 
                                          least-squares covariance matrix */
  uint32      q_FltHdop;                /* Horizontal position dilution of 
                                          precision as computed from the 
                                          unweighted least-squares covariance 
                                          matrix */
  uint32      q_FltVdop;                /* Vertical position dilution of 
                                          precision as computed from the 
                                          unweighted least-squares covariance 
                                          matrix */
  uint32      q_FltTdop;                /* Time position dilution of 
                                          precision as computed from the 
                                          unweighted least-squares covariance 
                                          matrix */
  uint32      q_FltGdop;                /* Geometrical position dilution of 
                                          precision as computed from the 
                                          unweighted least-squares covariance 
                                          matrix */
  uint8       u_EllipseConfidence;      /* Statistical measure of the 
                                          confidence (percentage) associated 
                                          with the uncertainty ellipse 
                                          values */
  uint32      q_FltEllipseAngle;        /* Angle of semimajor axis with respect 
                                          to true North, with increasing angles 
                                          moving clockwise from North. In units 
                                          of degrees. */
  uint32      q_FltEllipseSemimajorAxis;  /* Semimajor axis of final horizontal 
                                            position uncertainty error ellipse. 
                                            In units of meters. */
  uint32      q_FltEllipseSemiminorAxis;  /* Semiminor axis of final horizontal 
                                            position uncertainty error ellipse. 
                                            In units of meters. */
  uint32      q_FltPosSigmaVertical;    /* Gaussian 1-sigma value for final 
                                          position height-above-ellipsoid 
                                          altitude in meters */
  uint8       u_HorizontalReliability;  /* Horizontal position reliability 
                                          0: Not set
                                          1: Very Low
                                          2: Low
                                          3: Medium
                                          4: High    */
  uint8       u_VerticalReliability;    /* Vertical position reliability */
  uint16      w_Reserved2;              /* Reserved memory field */
  uint32      q_FltGnssHeadingRad;      /* User heading in radians derived 
                                           from GNSS only solution  */
  uint32      q_FltGnssHeadingUncRad;   /* User heading uncertainty in radians 
                                           derived from GNSS only solution  */

  uint32      q_SensorDataUsageMask;    /* Denotes which additional sensor data
                                           were used to compute this position 
                                           fix.
                                        BIT[0] 0x00000001 – Accelerometer
                                        BIT[1] 0x00000002 – Gyro
                                        BIT[2] 0x00000004 – Vehicle odometer
                                        BIT[3] 0x00000008 – Vehicle gyro
                                        BIT[4] 0x00000010 – Pedometer
                                        BIT[5] 0x00000020 – Pedestrian alignment
                                        0x0000FFFC - Reserved
                                        A bit set to 1 indicates that certain 
                                        fields as defined by the 
                                        SENSOR_AIDING_MASK were aided with 
                                        sensor data*/
  uint32      q_SensorAidMask;         /* Denotes which component of the 
                                          position report was assisted with 
                                          additional sensors defined in 
                                          SENSOR_DATA_USAGE_MASK
                                       BIT[0] 0x00000001 – 
                                         Heading aided with sensor data
                                       BIT[1] 0x00000002 – 
                                         Speed aided with sensor data
                                       BIT[2] 0x00000004 – 
                                         Position aided with sensor data
                                       BIT[3] 0x00000008 – 
                                         Velocity aided with sensor data
                                       BIT[4] 0x00000010 – 
                                         Aided by vehicle sensors
                                       BIT[5] 0x00000020 – 
                                         Aided by virgo
                                       BIT[6] 0x00000040 – 
                                         Aided by pedometer
                                       BIT[7] 0x00000080 – 
                                         Aided by inertial sensors
                                       BIT[8] 0x00000100 – 
                                         Aided by pedestrian alignment
                                       0xFFFFFFF0 – Reserved */
  /*numbers of used beacons and total beacons for each system */
  gnss_PositionRptNumBcnDataDmLog z_NumBcnInfo[DMLOG_GNSS_POSRPT_BCN_IDX_TOTAL];
  uint16      w_TechContributions[DMLOG_GNSS_POSRPT_TECHNOLOGIES_MAX];
  /* Numerical representation for each 
     technology's contribution to 
     positioning accuracy.  */
  /* Beacon position report per SV data. Data is filled in the order of GPS, GLO, BDS, GAL and TDP. 
     There is no overlap or gap between two consecutive constellations.*/
  gnss_PositionRptPerBcnDataDmLog  z_BcnData[DMLOG_GNSS_POSRPT_N_ACTIVE_MEAS];
} gnss_PositionRptDmLog;

/*! @brief Technology & Event enum type for LOG_GNSS_PDR_TECH_EVENT_REPORT_C */
typedef enum
{
  PDR_TECH_EVENT_NONE = 0,      /**< Invalid Event */
  PDR_TECH_EVENT_LPPM_RCVR_OFF, /**< LPPM Receiver Off */
  PDR_TECH_EVENT_LPPM_RCVR_ON,  /**< LPPM Receiver On */
  PDR_TECH_EVENT_DEV_ACCEL_OFF, /**< Device Accel Off */
  PDR_TECH_EVENT_DEV_ACCEL_ON,  /**< Device Accel On */
  PDR_TECH_EVENT_DEV_GYRO_OFF,  /**< Device Gyro Off */
  PDR_TECH_EVENT_DEV_GYRO_ON,   /**< Device Gyro On */
  PDR_TECH_EVENT_DEV_MAG_OFF,   /**< Device Mag Off */
  PDR_TECH_EVENT_DEV_MAG_ON,    /**< Device Mag On */
  PDR_TECH_EVENT_MAX = PDR_TECH_EVENT_DEV_MAG_ON
}gnss_PdrTechEventEnumTypeVal;
typedef uint32 gnss_PdrTechEventEnumType;

/*! @brief LOG_GNSS_PDR_TECH_EVENT_REPORT_C (0x19A9) */
typedef PACKED struct PACKED_POST
{
  log_hdr_type   xx_hdr;          /**< log header */
  uint8          u_Version;       /**< Version number of DM log */
  uint64         t_TimeTickMsec;  /**< Timestamp at which this log is generated, in time-tick-msec */
  uint32         q_TechEventId;   /**< Technology & Event that triggered this log, see gnss_PdrTechEventEnumTypeVal */
} gnss_PdrTechEventReportDmLog;


#ifdef FEATURE_CGPS_PLAYBACK
#pragma pack(pop)
#endif

/* This struct is used for PE to report gnss polynomial to SM and then SM reports
   it to OEM */
typedef struct
{
  U16   w_SvId;         /* Sv ID */
  /* GPS: 1-32, GLO: 65-96, 0: Invalid
     All others are reserved */
  S8    b_FreqNum;      /* Freq index, only valid if u_SysInd is GLO */
  U8    u_SvPolyFlags;  /* Indicates the validity of data components, see those
                           C_SV_POLY_... defintions above */
  U16   w_Iode;         /* Ephemeris reference time 
                           GPS:Issue of Data Ephemeris used [unitless].
                           GLO: Tb 7-bit, refer to ICD02 
                           GAL: 10-bit */
  DBL   d_T0;           /* Reference time for polynominal calculations 
                           GPS: Secs in week.
                           GLO: Full secs since Jan/01/96 */
  DBL   pd_xyz0[3];     /* C0X, C0Y, C0Z */
  DBL   pd_xyzN[9];     /* C1X, C2X ... C2Z, C3Z */
  FLT   pf_other[4];    /* C0T, C1T, C2T, C3T */
  FLT   f_PosUnc;       /* SV position uncertainty [m]. */
  FLT   f_IonoDelay;    /* Ionospheric delay at d_T0 [m]. */
  FLT   f_IonoDot;      /* Iono delay rate [m/s].  */
  FLT   f_sbasIonoDelay; /* SBAS Ionospheric delay at d_T0 [m]. */
  FLT   f_sbasIonoDot;  /* SBAS Iono delay rate [m/s].  */
  FLT   f_TropoDelay;   /* Tropospheric delay [m]. */
  FLT   f_Elevation;    /* Elevation [rad] at d_T0 */
  FLT   f_ElevDot;      /* Elevation rate [rad/s] */
  FLT   f_ElevUnc;      /* SV elevation [rad] uncertainty */
  DBL   d_VelCoef[12];  /* Coefficients of velocity poly */
} gnss_SvPoly;


/*************************************************************************
  GNSS Dead Reckoning Sync (DRSync)                                            
*************************************************************************/
typedef enum {
  GNSS_DRSYNC_TIMED_OUTPUT_OFF = 0, /* No timed pulse output */
  GNSS_DRSYNC_TIMED_OUTPUT_WO_REARM, /* Timed pulses will be generated without 
                                        re-arm messages.*/
  GNSS_DRSYNC_TIMED_OUTPUT_W_REARM, /* After the initial timed pulse, 
                                       every generation of a timed 
                                       pulse is conditioned on 
                                       receiving of a "Re-Arm" message 
                                       from the client application. */
  GNSS_DRSYNC_TIMED_OUTPUT_ON_DEMAND, /* Timed pulses will be generated at the 
                                         fastest rate permitted by software and 
                                         a re-arm message is required for each 
                                         pulse. Currently, the fastest pulse rate 
                                         in this mode shall be 1 HZ*/
  GNSS_DRSYNC_TIMED_HIGH_ACCURACY_TRIGGER   /* Timed pulses will be generated but the 
                                               trigger will be high accracy. Used
                                               for lab based calibrations */
} gnss_DRSyncModeTypeVal;
typedef uint8 gnss_DRSyncModeType;


typedef enum {
  GNSS_DRSYNC_POLARITY_POSITIVE = 0,
  GNSS_DRSYNC_POLARITY_NEGATIVE
} gnss_DRSyncPolarityTypeVal;
typedef uint8 gnss_DRSyncPolarityType;

typedef struct {
  gnss_DRSyncModeType e_Mode;         /* DR Sync mode */

  gnss_DRSyncPolarityType e_Polarity; /* Polarity of the sync pulse */

  FLT f_BiasNs;                     /* Bias of the pulse in nano sec */

  int32 l_PhaseSec;                 /* The phase in seconds of the pulse 
                                       relative to GPS week */
  int32 l_PeriodSec;                /* The period of the pulse in seconds */

  uint32 q_WidthMs;                  /* Pulse Width in millisec  */

  FLT f_TuncStartThresholdMs;       /* The time uncertainty below which 
                                       the pulse generation will start */
  FLT f_TuncStopThresholdMs;        /* The time uncertainty above which 
                                       the pulse generation will stop */
  boolean b_TuncIgnore;             /* Flag to ignore TUNC */

  boolean b_Armed;                  /* Flag to arm the next pulse */
} gnss_DRSyncConfigType;

typedef struct {
  FLT    f_GpsMsec;     /* GPS msec at reference tick [msecs] */

  uint16 w_GpsWeek;     /* GPS week number at reference tick [weeks] */

  FLT    f_ClkTimeUncMs; /* 1 sided max time bias uncertainty [msecs] */

  uint32 q_Count;       /* Number of pulses generated */
} gnss_DRSyncPulseDescType;


typedef struct
{
  /* indicating the presence/absence of the optional fields */
  gps_extern_coarse_pos_opt_field z_OptionMask;

  boolean u_PosInfoValid; /* Validity indicator for the Position Information */
  boolean u_GpsTimeValid; /* GPS Time validity indicator for the Position */
  uint16  w_GpsWeek;      /* GPS Week time reference for the Position */
  uint32  q_GpsMsec;      /* GPS Msec time reference for the Position */

  gps_extern_coarse_pos_alt_def_e_type e_AltDef; /* def. of altitude */
  gps_extern_coarse_pos_source_e_type  e_PosSrc;

  int32  l_Lat;     /* Lat: rad. LSB = pi/(2^32), 2's comp. */
  int32  l_Lon;     /* Lon: rad, LSB = pi /(2^31), 2's comp. */
  int16  x_Alt;     /* Alt: meters, positive height, negative depth  */

  uint32 q_Punc;    /* Horizontal location unc.: meters */
  uint16 w_AltUnc;  /* Vertical: meters */
}gnss_TerrestrialPositionType;

/*
  Function pointer for the callback to be used by a module that is requesting
  slow clock time(gps/gnss time) from mgp.  The requesting module must
  implement this callback for mc to call to return the current slow clock time.
  The implementation for this callback SHOULD package the time data into an IPC
  to be sent to the requesting module.
 
  The gps_TimeStructType * parameter is a pointer to the current time data as
  provided by mc.
 
  The boolean parameter is an indication if the gps_TimeStructType * parameter
  is valid.  If the boolean parameter is TRUE, the data pointed to by
  gps_TimeStructType * is valid.
*/
typedef void (*gnss_SlowClkCbType)(const gnss_ClockStructType *, boolean, gnss_SlowClkReqReasonEnumType);


/* Filtered BPAmp information */
typedef struct
{
  boolean u_Valid;   /* indicates if this data is valid or not */
  uint32 q_BP1AmpI;
  uint32 q_BP1AmpQ;

} gnss_BpAmpInfoType;

/*************************************************************************
  GNSS Measurement Error Detection                                            
*************************************************************************/
typedef enum
{
  GNSS_MEAS_ERR_NONE = 0,
  GNSS_MEAS_ERR_DETECTED,
  GNSS_MEAS_ERR_MAX
} gnss_MeasErrDetectionEnumTypeVal;
typedef uint8 gnss_MeasErrDetectionEnumType;

typedef struct
{
  gnss_MeasErrDetectionEnumType   e_MeasError;  /* Type of measurement error 
                                                  detected */
  mgp_GnssType                    e_GnssType;   /* Associated GNSS type */
  uint16                          w_Sv;         /* GNSS SVID: GPS 1-32, GLO 65-88 or 255
                                                              BDS 201:237, GAL 301:336, QZSS 193:197*/
  int8                            b_FreqNum;    /* Frequency number */
  uint16                          w_CnoIncrease;  /* Amount of C/N0 increase in 
                                                    units of 0.1 dB */
  uint64                          t_TimeTickMsec; /* TimeTickMsec at time of 
                                                    detection */
  FLT                             f_AbsPrErrMeters; /* Magnitude of 
                                                      pseudo-range error of 
                                                      previous measurement */
  FLT                             f_AbsPrrErrMps;  /* Magnitude of pseudo-range
                                                    rate error of previous 
                                                    measurement */
} gnss_MeasErrDetectionStructType;

/* GNSS-MGP Emergency modes*/

#define GNSS_EMERGENCY_EVENT_NONE              0x0
#define GNSS_EMERGENCY_EVENT_CALL_ORIGINATION  0x1   /* Emergency call origination */
#define GNSS_EMERGENCY_EVENT_CALL_END          0x2   /* Emergency call end */
#define GNSS_EMERGENCY_EVENT_ECBM_ACTIVE       0x4   /* Emergency call back mechanism active */
#define GNSS_EMERGENCY_EVENT_ECBM_NONE         0x8   /* Emergency call back mechanism is not active */

typedef uint32 MgpEmergencyEventType;

typedef enum
{
  GNSS_NAV_DATA_TYPE_EPH,
  GNSS_NAV_DATA_TYPE_ALM
} gnss_NavDataEnumTypeVal;
typedef uint8 gnss_NavDataEnumType;

#define SFRAME_WORDS  10    /* 10 x 30 bit words in a 6 second subframe */

typedef struct
{
  uint8  u_Ch;                      /* Channel number. 0 thru N_LCHAN-1 */
  uint8  u_Sv;                      /* GPS/BDS SV */
  uint8  u_LinearityMode;           /* Gnss linearity mode */
  uint16 w_ConfidenceLow;           /* 10 lsb's -> T indicates low SNR associated with word */
  uint32 q_GnssType;                /* Should be populated as an mgp_GnssType */
  uint32 q_ParityErrors;            /* 1. 10 LSB for 10 GPS words in a GPS sub-frame 
                                       2. BDS sub-frame: Preamble error in bit 0 + 19 bits 
                                          (from bit 1 through 19)for 19 BCH codes */
  uint32 q_TowCount;                /* Subframe count in week. 6 secs/bit */
  uint32 q_Demod[SFRAME_WORDS];   /* (D29* D30* d1...d24 D25 ... D30) */
  uint32 q_CycleSlipBitmask[10];    /* This is to record the cycle slip information 
                                       for each raw nav bit. For each U32, the 30 
                                       LSB are used to indicate if cycle slip 
                                       happens on 30 raw nav bits in one GPS(or BDS) word. 
                                       The 2 MSB are not used. */
  uint64 t_LinearityStampmsDiff;    /* Msecs since linearity change */
  uint64 t_Timetickms;              /* real-time clock tick */
} gnss_SvSubframeStruct;


/* GLONASS raw string injection structure */
typedef struct
{
  uint8   u_SvId;           /* GLONASS Sv ID: 65-88 */
  int8    b_FreqNum;        /* freq index: -7 ~ +6 */
  boolean b_HcError;        /* TRUE: if Hamming code shows err in string */
  boolean b_LowConf;        /* TRUE: if the string is low conf */
  uint64  t_SlowClkMs;      /* Slow clock counter in ms */
  uint8   u_StringData[11];
  /* The GLONASS string bits are arranged as
                      MSB                           LSB
  u_StringData[0]  -- 0     0     0     Bit85 Bit84 Bit83 Bit82 Bit81 
  u_StringData[1]  -- Bit80 Bit79 Bit78 Bit77 Bit76 Bit75 Bit74 Bit73
  .......
  u_StringData[10] -- Bit08 Bit07 Bit06 Bit05 Bit04 Bit03 Bit02 Bit01 */
} gnss_GloStringStruct;

typedef enum
{
  C_GNSS_GCF_CLEAR = 0, /* GCF not triggered */
  C_GNSS_GCF_NF, /* GCF triggered by NF */
  C_GNSS_GCF_CD, /* GCF triggered by CD */
  C_GNSS_GCF_EX, /* GCF triggered by external source */
} gnss_GCFTriggerEnumType;

typedef enum
{
  C_GNSS_GCF_NF_TEST_TRIGGERED = 0,   /* GCF test in NF was triggered */
  C_GNSS_GCF_NF_SET,                  /* GCF was detected in NF */
  C_GNSS_GCF_NF_NOT_SET_MEASEXCLUDE,  /* GCF was not detected by NF but 
                                      measurements were excluded */
  C_GNSS_GCF_NF_AMBIGOUS,             /* GCF was not detected by NF because of 
                                      ambiguous situation */
  C_GNSS_GCF_CD_SET,                  /* GCF was detected in CD */
  C_GNSS_GCF_NF_SET_NOTALLOWED,
  C_GNSS_GCF_CD_SET_NOTALLOWED
} gnss_GCFLogicEnumType;


typedef struct
{
  /* Below two GIT variables dont change after being set from NV at bootup */
  uint16 w_GitCpuBusyThresholdMs;
  uint16 w_GitIpcSleepMs;

  /* Below GIT var can change at run time based on CFCM cmds */
  boolean b_GitEnableThrottle;
  /* Attempt to enable GIT depending on WWAN state */
  boolean b_GitCFCMAttemptThrottleEnable;

  /* GIT must be disabled when emergency call is active */
  boolean b_EmergencyActive;

  /* GIT enabled only when CPU set to Resource max */
  boolean b_CpuAtResourceMax;

  /* NPA handle to query CPU freq */
  npa_query_handle          p_ClkCpuQueryHandle;
} gnss_GitControlsStruct;

extern gnss_GitControlsStruct gnssz_GitControls;

/*************************************************************************
  GNSS PDR Feature Control
*************************************************************************/
typedef enum
{
  GNSS_PDR_CONTROL_SRC_NONE = 0,
  GNSS_PDR_CONTROL_SRC_BUILD_CONFIG,
  GNSS_PDR_CONTROL_SRC_IZAT,
  GNSS_PDR_CONTROL_SRC_NV,
  GNSS_PDR_CONTROL_SRC_LAST
} gnss_PdrCtrlSrcEnumType;

/*! Struct for PDR control IPC */
typedef struct
{
  uint64 t_ModemTimeTickMsec;
  boolean b_PdrControl;
  gnss_PdrCtrlSrcEnumType e_PdrCtrlSrc;
} gnss_PdrControlMsgStructType;

/*! Struct for PDR Control Table */
typedef struct
{
  boolean b_DisableFlag; /* Flag to indicate if the PDR feature is disabled (TRUE) or enabled (FALSE).*/
  gnss_PdrCtrlSrcEnumType e_DisableSource; /* Source of the PDR Disable flag */
  /* Flag to indicate if the PDR feature is disabled (TRUE) or enabled (FALSE).*/
  boolean b_DisableFlagTable[GNSS_PDR_CONTROL_SRC_LAST];
} gnss_PdrControlDbStructType;

/* GAL nominal page injection structure */
typedef struct
{
  uint16  w_SvId;
  boolean b_CrcFailed;
  boolean b_TowCountValid;
  uint32  q_TowCountS;  // Refers beginnig of first bit on page
  uint32  q_Data[8];    // Full page content 240 bits
  uint64  t_Timetickms; // Refers TOW

  // For nominal pages q_Data is filled as:
  //
  // q_Data[0] bit 31     even/odd     even part
  // q_Data[0] bit 30     page type    even part
  // q_Data[0] bit 29..0  Data         even part
  // q_Data[1] bit 31..0  Data         even part
  // q_Data[2] bit 31..0  Data         even part
  // q_Data[3] bit 31..14 Data         even part
  // q_Data[3] bit 13..8  Tail         even part
  // q_Data[3] bit 7..0   Unused, no navigation data
  //
  // q_Data[4] bit 31     even/odd     odd part
  // q_Data[4] bit 30     page type    odd part
  // q_Data[4] bit 29..14 Data         odd part
  // q_Data[4] bit 13..0  Reserved1    odd part
  // q_Data[5] bit 31..6  Reserved1    odd part
  // q_Data[5] bit 5..0   SAR          odd part
  // q_Data[6] bit 31..16 SAR          odd part
  // q_Data[6] bit 15..14 Spare        odd part
  // q_Data[6] bit 13..0  CRC          odd part
  // q_Data[7] bit 31..22 CRC          odd part
  // q_Data[7] bit 21..14 Reserved2    odd part
  // q_Data[7] bit 13..8  Tail         odd part
  // q_Data[7] bit 7..0   Unused, no navigation data
  //
  // Alert pages are filled in the same order but larger
  // Reserved1 field replaces Data, SAR, and spare fields

} gnss_GalPageStruct;

/*************************************************************************
  GNSS PDR3.0 Data-Structures
*************************************************************************/

/* Minimum Time-Between-Measurement (TBM) expected for LPPM mechanism */
#define GNSS_RCVR_LPPM_MIN_TBM_MSEC    (1000)

/* Receiver Sub-State when engaged in LPPM */
typedef enum
{
  GNSS_RCVR_STATE_LPPM_NONE = 1, /* disabled */
  GNSS_RCVR_STATE_LPPM_WAITING_CLOCK, /* enabled but waiting for clock */
  GNSS_RCVR_STATE_LPPM_INIT, /* enabled and engaged in init state */
  GNSS_RCVR_STATE_LPPM_HIGH_UNC, /* enabled and engaged with high-unc */
  GNSS_RCVR_STATE_LPPM_LOW_UNC, /* enabled and engaged with low-unc */

  GNSS_RCVR_STATE_LPPM_MAX = GNSS_RCVR_STATE_LPPM_LOW_UNC,
  GNSS_RCVR_STATE_LPPM_FORCE_32BIT = 2147483647 /* Force 32bit */
} gnss_RcvrLppmStateEnumType;

/* Receiver Sub State Flags */
typedef struct
{
  uint32 b_InLppm : 1; /*Reciever in Lppm State*/
  uint32 : 31; /*For Padding*/
} gnss_RcvrSubStateFlagsStructType;

/* GPS Receiver Sub State Flags */
typedef struct
{
  gnss_RcvrSubStateFlagsStructType z_RcvrSubStateFlags;
  gnss_RcvrLppmStateEnumType e_LppmState;
} gnss_RcvrSubStateStructType;

/* MGP Pedestrian status type */
typedef enum
{
  GNSS_MGP_PED_STATUS_NONE,     /* unknown */
  GNSS_MGP_PED_STATUS_NON_PED,  /* non-pedestrian, e.g., vehicle */
  GNSS_MGP_PED_STATUS_PED,      /* pedestrian */
  GNSS_MGP_PED_STATUS_PED_EXIT_EVAL,  /* ped-exit evaluation period */

  GNSS_MGP_PED_STATUS_MAX = GNSS_MGP_PED_STATUS_PED_EXIT_EVAL,
  GNSS_MGP_PED_STATUS_FORCE_32BIT = 2147483647 /* Force 32bit */
} gnss_MgpPedStatusEnumType;

/* MGP GNSS environment quality indicator type */
typedef enum
{
  GNSS_QUALITY_IND_NONE,  /* unknown */
  GNSS_QUALITY_IND_OUTDOORS,  /* outdoors based on strong signal conditions */
  GNSS_QUALITY_IND_CHALLENGED,  /* challenged based on strong signal conditions */
  GNSS_QUALITY_IND_INDOORS,  /* indoors based on strong signal conditions */

  GNSS_QUALITY_IND_MAX = GNSS_QUALITY_IND_INDOORS,
  GNSS_QUALITY_IND_FORCE_32BIT = 2147483647 /* Force 32bit */
} gnss_QualIndEnumType;

/* MGP sensor-based pedestrian device context indicator type */
typedef enum
{
  GNSS_PED_DEV_CTX_UNKNOWN,  /* unknown */
  GNSS_PED_DEV_CTX_IN_HAND_STABLE,  /* device is stable position */
  GNSS_PED_DEV_CTX_IN_HAND_SWING,  /* device is in swing motion */
  GNSS_PED_DEV_CTX_IN_PANT_POCKET,  /* device is in pocket */
  GNSS_PED_DEV_CTX_IN_HAND_JOG,  /* device is in jog motion */
  GNSS_PED_DEV_CTX_MAX = GNSS_PED_DEV_CTX_IN_HAND_JOG,
  GNSS_PED_DEV_CTX_FORCE_32BIT = 2147483647 /* Force 32bit */
} gnss_PedDevCxtEnumType;


/*32bit bitmask to specify the enabled/acceptd 
  GNSS constellations in an XTRA injection*/
typedef enum
{
  GNSS_XTRA_INJ_MASK_NONE = 0,
  GNSS_XTRA_INJ_MASK_GPS = 1UL << 0,
  GNSS_XTRA_INJ_MASK_GLO = 1UL << 1,
  GNSS_XTRA_INJ_MASK_BDS = 1UL << 2,
  GNSS_XTRA_INJ_MASK_GAL = 1UL << 3,
  GNSS_XTRA_INJ_MASK_QZSS = 1UL << 4,
  GNSS_XTRA_INJ_MASK_ALL = UINT32_MAX,
} gnss_XtraInjMaskTypeVal;

typedef uint32 gnss_XtraInjMaskType;


typedef struct
{
  boolean v_Enable;
  uint32  q_FilterMask;
  FLT     f_PucThreshold;
} gnss_GnssLeFilterConfigType;

typedef struct
{
  uint8 u_IntegrityFileVersionNum;   /* File Version Number */ 
  uint8 u_WmId[6];                   /* Warn Manager ID */
  uint8 u_TimeWindowOfAnalysisHours; /* Time duration in hours over which the
                                        analysis was done to create integrity 
                                        file*/
  uint8 u_ServiceProviderId;  	     /* Service provider ID */
  uint8 u_Spare;                     /* reserved for future use*/
  uint16 w_GnssMask;                 /* Bit mask of the constellations analyzed in
                                        this injection*/
  uint32 q_IntWmVersionNum;          /* Warn Manager Version Number */
  uint32 q_FileSequenceNum;          /* File sequence Number */ 
  uint32 q_FileSizeBytes;            /* File size in bytes */
  gps_GpsTimeStructType z_TimeOfGeneration;    /* Time of file generation */
  gps_GpsTimeStructType z_TimeOfApplicability; /* Time of Applicability */
} gnss_IntegrityDataHeader;

typedef enum
{
  XID_GPS_INTEGRITY_RECORD = 101,
  XID_GLO_INTEGRITY_RECORD = 102,
  XID_BDS_INTEGRITY_RECORD = 103,
  XID_GAL_INTEGRITY_RECORD = 104,
  XID_QZSS1_INTEGRITY_RECORD= 105,
  XID_QZSS2_INTEGRITY_RECORD = 106,
  XID_GPS_L2C_INTEGRITY_RECORD = 107,
  XID_GPS_L5_INTEGRITY_RECORD = 108,
  XID_GLO_G2_INTEGRITY_RECORD = 109,
  XID_BDS_B2_INTEGRITY_RECORD = 110,
  XID_GAL_E5A_INTEGRITY_RECORD = 111,
  XID_QZSS_1_L2C_INTEGRITY_RECORD = 112,
  XID_QZSS_2_L2C_INTEGRITY_RECORD = 113,
  XID_QZSS_1_L5_INTEGRITY_RECORD = 114,
  XID_QZSS_2_L5_INTEGRITY_RECORD = 115
}gnss_RecordIdEnumType;

typedef struct
{
  uint8  u_IntegritySrc;  /* Source of integrity information */
  int8   b_FreqNum;       /* Glonass freq number */
  uint16 w_SvId;          /* SV ID  */
  int32  l_DeltaTimeSec;  /* Delta-time wrt. to TOA provided */
                          /* in header file in sec           */
} gnss_IntegrityRecordType;

typedef struct
{
  gnss_IntegrityDataHeader z_IntegrityHeader;     
  gnss_SvIdMaskStructType  z_UnusableSvBitMask[3]; /* for L1=0, L2=1 and L5=2, E5a uses L2 */
  uint8 u_NumIntegrityEvents[C_GNSS_SFMT_MAX]; /* Number of integrity events for each 
                                               signal. */
  uint16 w_EventsStartIndex[C_GNSS_SFMT_MAX]; /* start index of integrity event for
                                              each signal */
  gnss_IntegrityRecordType z_IntegrityRecord[1];  /* Pointer to first integrity record.       */
                                                  /* The actual buffer size is variable       */
                                                  /* depending on the number of integrity     */
                                                  /* events captured by u_NumIntegrityEvents. */
} gnss_XtraIntegrityIpcStructType;

/* XTRA Integrity structure contains the Xtra Integrity records */
/* within the XTRA Integrity file.                              */
typedef struct
{
  gnss_IntegrityDataHeader z_IntegrityHeader;
  gnss_SvIdMaskStructType  z_UnusableSvBitMask[3]; /* for L1=0, L2=1 and L5=2, 
                                                   E5a uses L2;
                                                   B2a uses L5*/
  uint8 u_NumIntegrityEvents[C_GNSS_SFMT_MAX]; /* # of integrity events for 
                                               each constellation */
  uint16 w_EventsStartIndex[C_GNSS_SFMT_MAX]; /* start index of integrity events
                                              for each signal */
  gnss_IntegrityRecordType z_IntegrityRecord[XTRAINT_MAX_MB_EVENT_RECORDS];
} gnss_XtraIntTmStructType;

/* Xtra injection type used to confirm Xtra or Xtra Integrity has been */
/* injected.                                                           */
typedef struct
{
  gnss_XtraInjMaskType  q_AcceptMask;
  uint8                 u_XtraInjType;
} gnss_XtraInjectType;

typedef enum
{
  XTRA_INT_EVENT_SRC_INVALID = 0,
  XTRA_INT_EVENT_SRC_BRD_EPHE = 1,
  XTRA_INT_EVENT_SRC_BRD_ALM = 2,
  XTRA_INT_EVENT_SRC_BRD_PAGE = 3,
  XTRA_INT_EVENT_SRC_IER6 = 4,
  XTRA_INT_EVENT_SRC_IER7 = 5,
  XTRA_INT_EVENT_SRC_IER8 = 6,
  XTRA_INT_EVENT_SRC_IER9 = 7,
  XTRA_INT_EVENT_SRC_SRV_DISABLED = 8,
  XTRA_INT_EVENT_SRC_RTI = 9, /* Real-time integrity */
  XTRA_INT_EVENT_SRC_PREDICTED_OUTRAGE_INFO = 10, /* Predicted outage information (NANU, NAGU etc.) */
  XTRA_INT_EVENT_SRC_UNUSABLE_BY_SERVER = 11, /* SV marked unusable by server due 
                                              to consistent sanity check failures */
  XTRA_INT_EVENT_SRC_COMPUTED_AS_UNHEALTHY = 12, /* Computed as unhealthy */
  XTRA_INT_EVENT_SRC_MANUAL_OVERRIDE = 255 /* Manual override for a selected PRN or 
                                           an entire constellation when PRN is set to 0 */
}gnss_XtraIntegritySrcEnumType;


typedef union
{
  int16 w_GloEphExpTime[N_GLO_SV];
  int16 w_BdsEphExpTime[N_BDS_SV];
  int16 w_GalEphExpTime[N_GAL_SV];
  int16 w_QzssEphExpTime[N_QZSS_SV];
} gnss_NonGpsEphExpTimeUnionType;


typedef enum
{
  GNSS_EPHEXPTIME_MASK_UNKNOWN = 0,
  GNSS_EPHEXPTIME_MASK_GLONASS,
  GNSS_EPHEXPTIME_MASK_BDS,
  GNSS_EPHEXPTIME_MASK_GALILEO,
  GNSS_EPHEXPTIME_MASK_QZSS
} gnss_NonGpsEphExpTimeMask;


typedef struct 
{
  gps_GpsTimeStructType z_CurrGpsTime;                /* Current GPS time when calculating the ExpTime */
  int16 w_GpsEphExpTime[N_GPS_SV];                    /* Time to GPS Eph expire in second referenced to w_CurrTime */
  gnss_NonGpsEphExpTimeUnionType y_NonGpsEphExpTime;  /* Time to Non-GPS Eph expire in second referenced to w_CurrTime */
  gnss_NonGpsEphExpTimeMask e_NonGpsIndex;            /* Indicate which array is in use in the union y_NonGpsEphExpTime */
} gnss_EphExpTimeStructType;


/*=============================================================================

       Function prototypes

=============================================================================*/

/*=============================================================================

FUNCTION 
  gps_MeasBlkPropagate

DESCRIPTION
  gps_MeasBlkPropagate propagates the GPS Measurements to the MeasClock
  reference point. It moves both the filtered and unfiltered measurements.

DEPENDENCIES
  None.

PARAMETERS
  p_MeasBlke - pointer to the MeasBlock

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void gps_MeasBlkPropagate(gnss_MeasBlkStructType *p_MeasBlk);

/*=============================================================================

  FUNCTION      gnss_ConvertGpsTime2GloTime()

  DESCRIPTION   Converts the GPS milliseconds in week and unambiguous week 
                number into the GLONASS 4 year interval, calendar day, and 
                milliseconds in day.

  DEPENDENCIES  None

  PARAMETERS    p_GpsTime - input, GPS time information to be converted.
                u_LeapSecOffset - input, leap second offset used in conversion
                p_GloTime - output, converted GLONASS time.
  
  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_ConvertGpsTime2GloTime(const gps_TimeStructType *p_GpsTime,
                                 uint8 u_LeapSecOffset,
                                 glo_TimeStructType *p_GloTime);


/*=============================================================================

  FUNCTION      ConvertGpsTime2BdsTime()

  DESCRIPTION   Converts GPS time to BDS time. If GPS time is older than 
                BDS week 0 (1/2006) set BDS time to invalid. Note that leap 
                second offset is always constant

  DEPENDENCIES  None

  PARAMETERS    p_GpsTime - input, GPS time information to be converted.
                p_BdsTime - output, converted BeiDou time.
  
  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_ConvertGpsTime2BdsTime(const gps_TimeStructType *p_GpsTime,
                                 bds_TimeStructType *p_BdsTime);

/*=============================================================================

  FUNCTION      gnss_ConvertGpsTime2GalTime()

  DESCRIPTION   Converts the GPS milliseconds in week and week 
                number into the GAL milliseconds in week and week 
                number.

  DEPENDENCIES  None

  PARAMETERS    pz_GpsTime - input, GPS time information to be converted.
                pz_GalTime - output, converted GAL time.
  
  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_ConvertGpsTime2GalTime(const gps_TimeStructType *pz_GpsTime,
                                 gal_TimeStructType *pz_GalTime);

/*=============================================================================

FUNCTION 
  gnss_MeasBlkPropagate

DESCRIPTION
  gnss_MeasBlkPropagate propagates the GNSS Measurements to the MeasClock
  reference point. It moves both the filtered and unfiltered measurements.

DEPENDENCIES
  None

PARAMETERS
  p_MeasBlk - pointer to the MeasBlock

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void gnss_MeasBlkPropagate(gnss_MeasBlkStructType *p_MeasBlk);


/*=============================================================================

FUNCTION 
  gnss_GloMeasBlkGGRfgdAdjust

DESCRIPTION
  gnss_GloMeasBlkGGRfgdAdjust adjusts the Glonass measurements with
  GPS/GLO RF group delay, to align the measurements to a GPS time-scale.
  This is necessary for AGNSS MSA, when we send the GPS/GLO measurements
  with the same time-stamp to the SMLC. Adjusting with GG RFGD will
  help the SMLC combine the GPS/GLO measurements seamlessly 
  (Note: that SMLC still needs to compensate for TauGPS offset)

DEPENDENCIES
  RF group delay should have been calibrated and the NV item should
  be correctly populated.

PARAMETERS
  p_MeasBlk - pointer to the MeasBlock

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void gnss_GloMeasBlkGGRfgdAdjust(gnss_MeasBlkStructType *p_MeasBlk);


/*============================================================================
FUNCTION gnss_meas_status_bits_check

DESCRIPTION
    Function checking SV measurement status bits. 

RETURN VALUE
    TRUE/FALSE: if TRUE, status is good enough to be included in a measurement
    report.

DEPENDENCIES

LIMITATIONS

=============================================================================*/

boolean gnss_meas_status_bits_check(const uint32 q_sv_meas_status, const boolean stringent_check_needed);


/*=============================================================================

  FUNCTION      gnss_TechContribFltToU16()

  DESCRIPTION   Convert the tech contributions from float to U16,  with the 
          components normalized and scaled to fit the unsigned integer 
        field completely.

  DEPENDENCIES  None

  PARAMETERS    pf_TechContributions: The array of float TechContributions 
                                
                pw_TechContributions: The array of U16 TechContributions

                w_MaxTechContributions: The max number of TechContributions
  
  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_TechContribFltToU16(const FLT pf_TechContributions[], U16 pw_TechContributions[],
                              U16 w_MaxTechContributions);


/*=============================================================================

  FUNCTION      gnss_LogPositionReport()

  DESCRIPTION   Generate the GNSS position report DM log 
                (LOG_GNSS_POSITION_REPORT_C, 0x1476)

  DEPENDENCIES  None

  PARAMETERS    pz_NavSolution: The GNSS navigation solution data to be logged 
                                as a GNSS position report DM log.
                pq_GnssPosRepDmLogCnt: The counter that represents the integer 
                                       number GNSS position report DM logs 
                                       transmitted so far.
  
  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_LogPositionReport(const gnss_NavSolutionStructType *pz_NavSolution,
                            uint32 *pq_GnssPosRepDmLogCnt);



/*=============================================================================

FUNCTION 
  gnss_TimeSpeedProp

DESCRIPTION
  TimeSpeedProp propagates the GNSS time speed measurement according to the
  reference time input.

DEPENDENCIES
  None

PARAMETERS
  p_SvStruct - pointer to the Filtered/Unfiltered measurement
  d_PropMs - msecs to propagate
  f_AbsPropMs - absolute value of msecs to propagate
  e_MeasBlkSrc - measurement block source

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void gnss_TimeSpeedProp(gnss_TimeSpeedStructType *p_SvStruct,
                        DBL d_PropMs,
                        FLT f_AbsPropMs,
                        gnss_MeasBlkSrcEnumType e_MeasBlkSrc);


void gnss_GpsPropTimeSpeedCarPhase(gnss_MeasStructType *p_GpsMeas,
                                   FLT f_PropMs, FLT f_AbsPropMs);


/*=============================================================================

  FUNCTION      gnss_GetDpoStatus()

  DESCRIPTION   For a given a GNSS measurement block, determine if DPO is 
                engaged.

  DEPENDENCIES  None

  PARAMETERS    pz_MeasBlk: Pointer to a GNSS measurement block
  
  RETURN VALUE  TRUE: DPO is engaged
                FALSE: DPO is disengaged

  SIDE EFFECTS  None

=============================================================================*/
boolean gnss_GetDpoStatus(const gnss_MeasBlkStructType *pz_MeasBlk);

/*=============================================================================

  FUNCTION      gnss_ProprietaryNmeaTerminate()

  DESCRIPTION   Append the NMEA sentence with the standard terminator. This 
                consists of a "*0x2.2X\r\n" fragment.

                'The checksum is the 8-bit exclusive OR (no start or stop bits) 
                of all the characters in the sentence, including the "," 
                delimiters, between but not including the "$" and the "*" 
                delimiters.'

  DEPENDENCIES  None

  PARAMETERS    pu_String: Pointer to the buffer containing the string
  
  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_ProprietaryNmeaTerminate(unsigned char *pu_String);

/*=============================================================================
  FUNCTION      gnss_LogPositionMeasReport()

  DESCRIPTION   Generate the GNSS and/or TDP meas report of GNSS position report 
                DM log (LOG_GNSS_POSITION_REPORT_C, 0x1476)

  DEPENDENCIES  None

  PARAMETERS    z_BcnData[]: Array of per Bcn meas report
                pz_NavSolution: The GNSS navigation solution data to be logged 
                                as a GNSS position report DM log.
                u_TotalBcns[]: total SVs[GPS,GLO,BDS,GAL]
                u_NumBcnsUsed[]: Svs used[[GPS,GLO,BDS,GAL] 
  
  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/

void gnss_LogPositionMeasReport(gnss_PositionRptPerBcnDataDmLog z_BcnData[],
                                const gnss_NavSolutionStructType *pz_NavSolution,
                                gnss_PositionRptNumBcnDataDmLog *pz_NumBcnInfo);

#if defined(WIN32) && defined(ADS_120_COMPILER_4_BYTE_ALIGNMENT)
#pragma pack(pop, GNSS_COMMON_H)
#endif

/*
 ******************************************************************************
 * Function gnss_InitTimeTickGetMsecMutex
 *
 * Description:
 *
 *  This function initiates Mutex which is used to get TimeTick Msec.
 *
 * Parameters: 
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value: 
 *
 *  TRUE if it initilized Mutex Sucessfully, Otherwise FALSE
 *
 ******************************************************************************
*/
boolean gnss_InitTimeTickGetMsecMutex(void);

/*
 ******************************************************************************
 * gnss_InitDal
 *
 * Function description:
 *
 *  This function initializes the DAL interface for GNSS for reading 
 *  System Timer values
 *
 * Parameters:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
void gnss_InitDal(void);

/*
 ******************************************************************************
 * gnss_GpsWeekExtend
 *
 * Function description:
 *
 *  gnss_GpsWeekExtend is a helper function used to locate a propsective
 *  week number (10 bits) in the valid week number range. In essence this
 *  handles all of the week number rollover effects within the receiver.
 *
 *  The return value from this function is constrained to be in the range 
 *  of w_MinGpsWeekNumber read from NV through (w_MinGpsWeekNumber+1023)
 *
 * Parameters: 
 *
 *  w_GpsWeek
 *
 * Return value: 
 *
 *  Bounded w_GpsWeek
 *
 ******************************************************************************
*/
uint16 gnss_GpsWeekExtend(U16 w_GpsWeek);

/*
 ******************************************************************************
 * Function gnss_SumU8Array
 *
 * Description:
 *
 *  This function sums up the value in the array. The typical use is to sum the 
 *  counts of sv measurements. It is the caller's responsibility to avoid overflow.
 *
 * Parameters: 
 *
 *  pu_Array: pointer to the array to sum
 *  u_Num: length of the array
 *
 * Dependencies:
 *
 *  None
 *
 * Return value: 
 *
 *  the sum of u_Num values in the array
 *
 ******************************************************************************
*/
U8 gnss_SumU8Array(const U8 *pu_Array, U8 u_Num);

/*=============================================================================

  FUNCTION      gnss_WriteFltToU32()

  DESCRIPTION   Write floating-point value to U32 preserve the binary 
                representation. This is usually needed for logging floating-point
                value.                

  DEPENDENCIES  None

  PARAMETERS    p_Dst: point to the address for writing
                f_Val: floating point value
  
  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_WriteFltToU32(void *p_Dst, FLT f_Val);

/*=============================================================================

  FUNCTION      gnss_WriteU8ToU16()

  DESCRIPTION   Write U8 value to U16 adress. 
                This is usually needed for logging value in to packed structure.                

  DEPENDENCIES  None

  PARAMETERS    p_Dst: point to the address for writing
                u_Val: U8 value
  
  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_WriteU8ToU16(void *p_Dst, U8 u_Val);

/*=============================================================================

  FUNCTION      gnss_FindValInU8Array()

  DESCRIPTION   Find the value in an U8 array

  DEPENDENCIES  None

  PARAMETERS    u_Val: value to find
                pu_Array: point to the first element of the array 
                u_Len: length of the array
  
  RETURN VALUE  index of the found element that has the same value
                u_Len if not found

  SIDE EFFECTS  None

=============================================================================*/
U8 gnss_FindValInU8Array(U8 u_Val, const U8 *pu_Array, U8 u_Len);

/*=============================================================================

  FUNCTION      gnss_FindValInU16Array()

  DESCRIPTION   Find the value in an U16 array

  DEPENDENCIES  None

  PARAMETERS    w_Val: value to find
                pw_Array: point to the first element of the array 
                u_Len: length of the array
  
  RETURN VALUE  index of the found element that has the same value
                u_Len if not found

  SIDE EFFECTS  None

=============================================================================*/
U8 gnss_FindValInU16Array(U16 w_Val, const U16 *pw_Array, U8 u_Len);

/*=============================================================================

  FUNCTION      gnss_GetNumBitsSet()

  DESCRIPTION   Get the number of bits set to 1 in U64 bit mask

  DEPENDENCIES  None

  PARAMETERS    t_Mask: bit mask 
                  
  RETURN VALUE  u_SumBits: number of bits set to 1

  SIDE EFFECTS  None

=============================================================================*/
U8 gnss_GetNumBitsSet(U64 t_Mask);

/*=============================================================================

  FUNCTION      gnss_ConvertFlt2U32()

  DESCRIPTION   Extract the integer part of the single precision floating-point 
                number. Saturation is applied if the input is out of the range
                of U32. Using this function instead of (U32)f_In avoids out of
                range exception.

  DEPENDENCIES  None

  PARAMETERS    f_In: input FLT 
                  
  RETURN VALUE  output U32

  SIDE EFFECTS  None

=============================================================================*/
U32 gnss_ConvertFlt2U32(FLT f_In);

/*=============================================================================

  FUNCTION      gnss_ConvertFlt2S32()

  DESCRIPTION   Extract the integer part of the single precision floating-point 
                number. Saturation is applied if the input is out of the range
                of S32. Using this function instead of (S32)f_In avoids out of
                range exception.

  DEPENDENCIES  None

  PARAMETERS    f_In: input FLT 
                  
  RETURN VALUE  output S32

  SIDE EFFECTS  None

=============================================================================*/
S32 gnss_ConvertFlt2S32(FLT f_In);

/*=============================================================================

  FUNCTION      gnss_SendPdrControl()

  DESCRIPTION   Send the PDR disable control to all the invested modules. This 
                function shall be called from following 3 places, 
                1. By development only NV item AAGPS Development Test Control5
                   (injected from CD)
                2. By QMI message for control of commercially deployed feature
                   (specific details are still under discussion w/ marketing)
                3. By feature define for PLs without the feature as POR

  DEPENDENCIES  None

  PARAMETERS    b_PdrDisableFlag: TRUE if feature is disabled, FALSE otherwise

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_SendPdrControl
(
   boolean b_PdrDisableFlag,
   gnss_PdrCtrlSrcEnumType e_PdrCtrlSrc
   );

/*=============================================================================

  FUNCTION      gnss_InitPdrControlDb()

  DESCRIPTION   Initialize the PDR control data-struct with defaults. 

  DEPENDENCIES  None

  PARAMETERS    pz_PdrFeatureCtrlDb: pointer to the calling module's 
                  PDR data-struct

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_InitPdrControlDb
(
   gnss_PdrControlDbStructType *pz_PdrFeatureCtrlDb
   );

/*=============================================================================

  FUNCTION      gnss_UpdatePdrControlDb()

  DESCRIPTION   Update the PDR control data-struct with the input PDR control message. 

  DEPENDENCIES  None

  PARAMETERS    pz_PdrFeatureCtrlDb: pointer to the calling module's 
                  PDR data-struct
                pz_PdrControlMsg: pointer to input PDR control struct

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_UpdatePdrControlDb
(
   gnss_PdrControlDbStructType *pz_PdrFeatureCtrlDb,
   const gnss_PdrControlMsgStructType *pz_PdrControlMsg
   );

/*=============================================================================

FUNCTION 
  gps_SvTimeDelta

DESCRIPTION
  gps_SvTimeDelta is a helper function used to apply an arbritrary time delta
  to a gps_SvTime structure. To accomodate large time deltas without loss of
  precision DBL arithmetic in used. The ability to operate only on the
  sub millisecond portion of the time is afforded by the u_EvalMs flag.

DEPENDENCIES
  None.

PARAMETERS
  p_SvTime - pointer to the gps_SvTime structure that is to be adjusted
  d_TimeDelta - Value of the time delta [msecs]
  u_EvalMs - TRUE, forces the msec time delta to be evaluated

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

void gps_SvTimeDelta(gnss_SvTime *p_SvTime, DBL d_TimeDelta, uint8 u_EvalMs);

/*=============================================================================

  FUNCTION      gnss_LogPdrTechEventReport()

  DESCRIPTION   Generate the GNSS PDR Technology Event report DM log
                (LOG_GNSS_PDR_TECH_EVENT_REPORT_C, 0x19A9)

  DEPENDENCIES  None

  PARAMETERS    e_TechEventId: Technology & Event that triggered this log.
  
  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_LogPdrTechEventReport(gnss_PdrTechEventEnumType e_TechEventId);

/*=============================================================================

FUNCTION 
  gnss_Uax3IsEnabled

DESCRIPTION
  gnss_Uax3IsEnabled parses the UAX3 configuration information and checks if
  the UAX3 feature is enabled

DEPENDENCIES
  None.

PARAMETERS
  q_Uax3CfgInfo - UAX3 configuration information

RETURN VALUE
  TRUE if Enabled. FALSE Otherwise

SIDE EFFECTS
  None.

=============================================================================*/
boolean gnss_Uax3IsEnabled(uint32 q_Uax3CfgInfo);

/*=============================================================================

  FUNCTION      gnss_UpdateQmiPosRptTime()

  DESCRIPTION   This function provides an indication of the QMI Position
                report time, logged when QMI is sending out the report
                over QMI pipe. The message is sent to Measurement Engine
                and is used to measure processing latency from ME->PE->SM->QMI
                This is used both for diagnostics and possibly for MIPS management
 

  DEPENDENCIES  None

  PARAMETERS    w_GpsWeek --- GPS Week corresponding to the Pos Report
                q_GpsMsec --- GPS Msec of the week corresponding to the Pos Report
                t_QmiPosRptTimeMsec --- Time Tick msec corresponding to QMI
                                        Pos report time
 
  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void gnss_UpdateQmiPosRptTime
(
   uint16 w_GpsWeek,
   uint32 q_GpsMsec,
   uint64 t_QmiPosRptTimeMsec
   );

/*=============================================================================

  FUNCTION      gnss_ApplyGnssLeFilter

  DESCRIPTION
    Verifies if a position fix is qualified to be reported as a GNSS-LE 
    fix. 

  DEPENDENCIES    

  PARAMETERS
     f_Hepe - position fix HEPE
   e_Quality - GNSS Quality indication

  RETURN VALUE
     TRUE  - The fix is a qualified GNSS-LE fix
     FALSE - Otherwise

  SIDE EFFECTS

=============================================================================*/
boolean gnss_ApplyGnssLeFilter(FLT f_Hepe, gnss_QualIndEnumType e_Quality,
                               const gnss_GnssLeFilterConfigType *cpz_Filter);

/*=============================================================================

FUNCTION      gnss_Sfmt2SigIdx

DESCRIPTION This function mapps the single frequency measurement type to the
index of the signal under the corrsponding constellation.

0: GPS L1 C/A, GLO G1, BDS B1, GAL E1, QZSS L1 C/A
1: GPS L2C, GLO G2, BDS B2, GAL E5a, QZSS L2C
2: GPS L5Q, QZSS L5Q

DEPENDENCIES None

PARAMETERS  e_Smft - single frequency measurement type

RETURN VALUE index of the signal under the corresponding constellation
-1 if the single frequency measurement type is not supported

SIDE EFFECTS None

=============================================================================*/
int32 gnss_Sfmt2SigIdx(gnss_SfmtEnumType e_Smft);

/*=============================================================================

FUNCTION		gnss_GetEphAvailStatusToSetSessionTime()
	
DESCRIPTION Check if the GNSS ephemeris timeout is valid and enough for position fix
		
DEPENDENCIES	None
		
PARAMETERS
		
  cpz_GnssEphAvailInfo: points to the GNSS ephemeris timeout information
		  
  cpz_CurrentGpsTime: points to current GPS time
		  
		
RETURN VALUE
	TRUE if valid for fix, FALSE otherwise
		
SIDE EFFECTS	None
		
=============================================================================*/
boolean gnss_GetEphAvailStatusToSetSessionTime(const gnss_EphExpTimeStructType* cpz_GnssEphAvailInfo,
											 const gps_TimeStructType* cpz_CurrentGpsTime);
	
	
/*=============================================================================
		
FUNCTION gnss_DeleteEphAvailStatus()
		
DESCRIPTION Delete the GNSS ephemeris timeout data
		
DEPENDENCIES None
		
PARAMETERS
	
	pz_GnssEphAvailInfo: points to the GNSS ephemeris timeout information
		
RETURN VALUE
	TRUE if delete success, FALSE otherwise
		
SIDE EFFECTS	None
		
=============================================================================*/
boolean gnss_DeleteEphAvailStatus(gnss_EphExpTimeStructType* pz_GnssEphAvailInfo);

#ifdef __cplusplus
}
#endif

#endif /* GNSS_COMMON_H */


