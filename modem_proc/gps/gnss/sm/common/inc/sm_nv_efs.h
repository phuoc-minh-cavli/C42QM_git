/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Session Manager NV Items using EFA API Interface Header File

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for 
  the SM Non-Volatile Memory interface.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2015 - 2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/inc/sm_nv_efs.h#3 $
  $DateTime: 2021/03/09 23:29:53 $
  $Author: pwbldsvc $ 

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/27/21   sai     Added NV for Controlling Fix related operations, currently controls cache fix propogation and its reporting to client
10/20/17   rn      Add NVs for XTRA Integrity.
07/19/17   ak      Added NV for HEPE threshold config for DR/INS fixes
11/08/16   gurdar  Add the NV item for UAX3 configuration.
08/30/16   skm     Added NV for default LPP12 2 timer feature config 
05/01/16   muk     Change NV73542, to Enable Gnss Measurement by Default 
04/15/16   ld      Added NHz NV support
02/17/16   mj      Added NV support to enable/disable CPI injections and early exit logic 
02/11/16   gk      LPPe enablement
10/22/15   rk      By default choose LPP CP as position protocol for LPP profile   
09/13/15   gk      Changed default retransmission timer value to 2sec
08/13/15   sjk     Added default configuration of GM from NV for AON
06/26/15   sjk     Added support for Location Batching 2.0 NV items 
06/21/15   gk      change default for network transmission delay to 2 seconds
06/09/15   ssu      Adding NV for GM filtering of WLAN freq. 
03/14/15   rh      Added support for MultiSIM-EFS NV
02/26/15   ssu     Adding new NV items for GM LOWI integration.
01/14/15   rk      LPP >24 AD NV default added.
11/13/14   rh      Added protocol transmission delay EFS NV 
04/28/14   ah      Added support for handling premium service SAP disabled
02/21/14   ssu     Adding NV for configuring motion sensing distance in case of challenging GPS env. 
02/20/14   ssu     Adding a NV for modifying the medium responsiveness backoff value and gps challenging env backoff config. 
02/10/14   ssu     Adding a NV for modifying the rate at which GM requests CPI injection. 
01/10/14   ss      Supporting High Confidence Geofence config
10/22/12   mjohn   Reverting back to default DISABLE option for premium services
10/08/12   mjohn   Added default ENABLE option for premium services
07/17/12   mjohn   Added default DISABLE option for premium services
06/11/12   rk      Added default A-glonass protocol choice value.
5/14/10    gk      Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef SM_NV_EFS_H
#define SM_NV_EFS_H

/*
 * Include files
*/

#include "comdef.h"
#include "pdapi_lite.h"
#include "gnss_common.h"
#include "gps_nv_efs.h"
#include "gnss_cqsvc.h"
#include "sys.h"


/* All the NV EFS values are initialzed here */

/* add all the default values for NV items here */
#define CGPS_NV_EFS_SM_XTRA_PREFERRED_MAX_VALID_AGE_DEFAULT (168)   /* in hours; 7 days */
#define GNSS_NV_EFS_SM_SEND_NO_FIX_REPORT_DEFAULT           (0)     /* don't send NO_FIX report */
#define GNSS_NV_EFS_SM_TM_1X_UP_SUPL_ENABLE_DEFAULT         (0)     /* SUPL not enabled as 1x UP protocol by default*/
#define GNSS_NV_EFS_SM_TM_LPP_ENABLE_DEFAULT                PDSM_PA_LPP_IN_LTE_CP_ENABLE     /* LPP CP is enabled as default protocol in LTE*/
#define GNSS_NV_EFS_SM_AGPS_RAT_PREF_UMTS_LTE_PREFERED      (1)     /* UMTS/LTE is prefered over other RAT's by default*/
#define GNSS_NV_EFS_SM_AGPS_RAT_ENABLE_BIT_MASK             (0xFFFF)/* ALL RAT's are enabled for AGPS by default*/
#define GNSS_NV_EFS_SM_TM_ASST_GLO_PROTOCOL_DEFAULT         (0)     /* By default no protocol selected for A-Glonass */
#define GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES_ENABLE_DEFAULT (0)   /* Disable Premium services by default */
#define GNSS_NV_EFS_SM_TM_LPP_CP_TRANSPORT_TIMEOUT_SECS_DEFAULT (2)  /* LPP CP transport TX timeout in seconds */
//#ifdef FEATURE_CGPS_LOC_MIDDLEWARE /* Enable Loc, Disable PDS if Loc Middleware is present, then LOC is enabled Rework needed */
  #define GNSS_NV_EFS_CONFIG_QMI_SERVICE_EXTERNAL_DEFAULT     (PDSM_LITE_QMI_CONFIG_ENABLE_LOC)   
//#endif
#define GNSS_NV_EFS_SM_TM_EMERGENCY_SUPPORT_CP_DEFAULT PDSM_PA_EMERGENCY_PROTOCOL_CONFIG_WCMDA_CP  /* 0 CP is default for e911 session on W/G/1x, 1 for user plane */
#define GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_DEFAULT            (250) /* 250 meters */
#define GNSS_NV_EFS_SM_LM_MAX_HEPE_THRESHOLD                (250)
#define GNSS_NV_EFS_SM_LM_MIN_HEPE_THRESHOLD                (50)
#define GNSS_NV_EFS_SM_GM_MOTION_DETECTION_SOURCES_DEFAULT (1)   /* Motion sensing enabled by default */
#define GNSS_NV_EFS_SM_GM_GNSS_UNAVAIL_IND_TIMEOUT_DEFAULT (40)  /* 40 in seconds  */
#define GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT_DEFAULT  (30) /*30 seconds*/
#define GNSS_NV_EFS_SM_GM_POSITION_SOURCES_DEFAULT            (1) /*Usage of CPI enabled, GTP2.0 disabled by default*/
#define GNSS_NV_EFS_SM_XTRA_LAST_XTRA3_KEY_INDEX_DEFAULT      (0)  /* last key index used in XTRA3 */
#define GNSS_NV_EFS_SM_TM_WIFI_WAIT_TIMEOUT_SEC_DEFAULT       (0)  /* default to 0, which disables the feature */
#define GNSS_NV_EFS_SM_GM_GNSS_MAX_POS_UNC_ACCEPTED_DEFAULT  (100) /*100 meters*/
#define GNSS_NV_EFS_SM_GM_USE_NW_ASST_FIXES_DEFAULT           (0)   /*FALSE: GM wont use NW assisted fixes by default*/
#define GNSS_NV_EFS_SM_GM_HIGH_RESPONSIVENESS_CONFIG_DEFAULT  1 /*Enable High Responsiveness Dynamic Responsiveness*/
#define GNSS_NV_EFS_SM_E911_CONFIG_DEFAULT (0x0)  /* All MO Sessions Abort on E911 */
#define GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE_DEFAULT  5*60 /*Rate at which GM requests CPI is by default 5 mins*/
#define GNSS_NV_EFS_SM_GM_MED_RESP_BACKOFF_DEFAULT  120 /*Medium Responsiveness Backoff in seconds */
#define GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MIN_DEFAULT 30/* Minimum backoff when in Challenging GPS Env in seconds */
#define GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MAX_DEFAULT 16 * 60/* Maximum backoff when in Challenging GPS Env in seconds */
#define GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_GPS_DEFAULT 50 /* The motion sensing distance for GPS in case of challenging GPS env ( meters ) */
#define GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CPI_DEFAULT 50 /* The motion sensing distance for CPI in case of challenging GPS env ( meters ) */
#define GNSS_NV_EFS_SM_GM_MS_WALK_SPEED_DEFAULT  3 /*3 m\sec*/
#define GNSS_NV_EFS_SM_GM_MS_RUN_SPEED_DEFAULT   8 /*8 m\sec*/
#define GNSS_NV_EFS_SM_GM_LOWI_REG_ENABLE_DEFAULT                  1    /*By default GM LOWI registration is enabled*/
#define GNSS_NV_EFS_SM_GM_LOWI_SUB_MASK_DEFAULT                    0x1  /*By default GM registers to SNOOP scans only from LOWI*/
#define GNSS_NV_EFS_SM_GM_ENABLE_TIMER_MASK_DEFAULT                0x3  /*By default both short term and long term timers are enabled for LOWI registration from GM */
#define GNSS_NV_EFS_SM_GM_SHORT_TERM_TIMER_MAX_RETRY_CNT_DEFAULT   12   /*By default there are 12 attempts by the short term timer for registration retries by GM */
#define GNSS_NV_EFS_SM_GM_COMP_LOGGING_RATE_DEFAULT   (5*60) /*5 minutes*/
#define GNSS_NV_EFS_SM_GM_IOD_CONFIG_SWITCHES_MASK_DEFAULT   0 /* ALS IOD Disabled */
#define GNSS_NV_EFS_SM_GM_FILTER_WLAN_FREQ_MASK_DEFAULT            0x3  /*By default GM filters and accepts 2.4Ghz and 5Ghz band from the WLAN frequencies */


#ifdef FEATURE_CGPS_FW_NON_STANDARD_SUPPORT /* currently this NV is only used for LPP Control Plane */
#define GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY_DEFAULT (2000) /*2000ms == 2s */
#else
#define GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY_DEFAULT (1000) /*1000ms == 1s */
#endif
#define GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY_MAX     (10000) /*10000ms == 10s, max delay allow */
#define GNSS_NV_EFS_SM_TM_CONFIG_RRC_TRANSMISSION_DELAY_DEFAULT    (2000) /*2000ms == 2s RRC default is 2s */

#define GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES_ENABLE_BASIC   (0)
#define GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES_ENABLE_PREMIUM (1)
#define GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES_DISABLE        (2)
#define GNSS_NV_EFS_SM_LPP_GRT_24_OTDOA_AD_DEFAULT            (0)    /* By default >24 OTDOA AD/Meas processing isn't supported */

#ifdef FEATURE_RD_SUPL_HSLP_N_CERTS_FRM_USIM
  #error code not present
#endif /* FEATURE_RD_SUPL_HSLP_N_CERTS_FRM_USIM */

#define GNSS_NV_EFS_SM_BATCHING_GF_RADIUS_DEFAULT   (100) /* 100 meters */
#define GNSS_NV_EFS_SM_BATCHING_CONFIG_MASK_DEFAULT (0)
 
#define GNSS_NV_EFS_SM_AON_DEFAULT_GNSS_POS_QOS_SESSION_TIMEOUT (20) /*Default initial session timeout 20 seconds*/
#define GNSS_NV_EFS_SM_AON_DEFAULT_CPI_REQUEST_RATE             (5*60) /*Rate at which AON requests CPI is by default 5 mins when registrating with GM*/
#define GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_BO_MIN          (30) /* Default Minimum backoff AON requests when in Challenging GPS Env in seconds */
#define GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_BO_MAX          (8 * 60) /*Default Maximum backoff AON requests when in Challenging GPS Env in seconds */
#define GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_MS_DIST_GPS     (50) /* Default motion sensing distance for GPS in case of challenging GPS env ( meters ) */
#define GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_MS_DIST_CPI     (50) /* Default motion sensing distance for CPI in case of challenging GPS env ( meters ) */

#define GNSS_NV_EFS_SM_TM_LPPE_UP_ENABLE_DEFAULT  (0) /* Default LPP UP LPPe support is disabled. To enabled use the
                                                         corresponding macros PDSM_PA_LPPE_ENABLE, PDSM_PA_LPPE_DBH_ENABLE,
                                                         PDSM_PA_LPPE_WLAN_ENABLE */
#define GNSS_NV_EFS_SM_TM_LPPE_CP_DEFAULT         (0) /* Default LPP CP LPPe support is disabled. To enabled use the
                                                         corresponding macros PDSM_PA_LPPE_ENABLE, PDSM_PA_LPPE_DBH_ENABLE,
                                                         PDSM_PA_LPPE_WLAN_ENABLE */

#ifdef FEATURE_CGPS_FW_NON_STANDARD_SUPPORT /* currently this NV is only used for LPP Control Plane */
#define GNSS_NV_EFS_SM_TM_EMERGENCY_SUPPORT_CONFIG_DEFAULT (0)       /* CPI injections for E911 default: bit 0 - disabled ==0, 
                                                                        Early Exit logic for E911: bit 1 -disabled == 0     */
#else
/* For LPPe support enable E911 CPI injections */
#define GNSS_NV_EFS_SM_TM_EMERGENCY_SUPPORT_CONFIG_DEFAULT (1)       /* CPI injections for E911 default: bit 0 - disabled ==0, 
                                                                        Early Exit logic for E911: bit 1 -disabled == 0     */
#endif

#ifdef FEATURE_GNSS_GEN9_AUTOMOTIVE_ENABLED
  /* Automotive should always have LPPM disabled. */
  #define GNSS_NV_EFS_SM_TECH_SEL_LPPM_CONFIG_DEFAULT (0) /*Bit0 Enabled: Lppm Enabled*/
#else
  #define GNSS_NV_EFS_SM_TECH_SEL_LPPM_CONFIG_DEFAULT (0) /*Bit0 Enabled: Lppm Enabled*/
#endif /* FEATURE_GNSS_GEN9_AUTOMOTIVE_ENABLED */

#define GNSS_NV_EFS_SM_TECH_SEL_WIFI_ASSIST_CONFIG_DEFAULT (1) /*Bit0 Enabled: Wifi Assistance Enabled*/
#define GNSS_NV_EFS_SM_TM_UAX3_CONFIG_DEFAULT  (PDSM_PA_UAX3_MASK_ENABLE)

#define GNSS_NV_EFS_SM_LPP_2TIMER_SUPPORT_DEFAULT         (0) /*Default LPP Release 12 2 Timer feature is OFF*/
#define GNSS_NV_EFS_SM_ASSISTED_BDS_PROTOCOL_ENABLE_MASK_DEFAULT         (0) /*Default ABDS Enable Mask is OFF*/

#ifdef FEATURE_GNSS_GEN9_AUTOMOTIVE_ENABLED
  #define GNSS_NV_EFS_SM_CONCURRENT_SESSION_SUPPORT_CONFIG_DEFAULT         (0x01) /*Automotive Default is Concurreny feature is enabled*/
#else
  #define GNSS_NV_EFS_SM_CONCURRENT_SESSION_SUPPORT_CONFIG_DEFAULT         (0x00) /*Default Concurreny feature is disabled*/
#endif /* FEATURE_GNSS_GEN9_AUTOMOTIVE_ENABLED */

/* Default Mask for NHz Config is enabled/Automotive. 
This is for testing only on the final submitt it needs to be disabled */
#define GNSS_NV_EFS_SM_NHZ_CONFIG_DEFAULT ( GNSS_NHZ_ENABLED | \
                                            GNSS_NHZ_MODE_AUTOMOTIVE )

#define GNSS_NV_EFS_SM_FUSION_TEST_CONTROL_ENHANCED_DEBUG_MODE (0x00000001)  /* Generates more debugging F3 for fusion CS */
#define GNSS_NV_EFS_SM_FUSION_TEST_CONTROL_STORE_BARO_TO_EFS   (0x00000002)  /* Store baro samples into EFS file system */

#define GNSS_NV_EFS_SM_FUSION_CSM_DATA_UPLOAD_THRESHOLD_MINS_DEFAULT (30) /*Default threshold for Fusion CSM upload*/
#define GNSS_NV_EFS_SM_FUSION_CSM_SB_BUFFER_SIZE_DEFAULT            (120) /*Default size for Fusion CSM SB buffer*/
#define GNSS_NV_EFS_SM_FUSION_CSM_MAX_SSID_SUPPORTED_DEFAULT        (200) /*Default maximum number of SSID supported for Fusion CSM*/
#define GNSS_NV_EFS_SM_FUSION_CSM_VENUE_TRACKING_RESP_SEC_DEFAULT   (120) /*Default venue tracking resp in sec for Fusion CSM*/
#define GNSS_NV_EFS_SM_FUSION_CSM_OPERATION_MODE_DEFAULT            (1) 
#define GNSS_NV_EFS_SM_FUSION_CSM_BARO_FILTER_WEIGHT_DEFAULT        (10) /* Default weight factor for current Barometer sample */
#define GNSS_NV_EFS_SM_FUSION_BARO_SAMPLE_RATE_HZ_DEFAULT           (10)  /* Default sampling rate for Barometer data */
       
#define GNSS_NV_EFS_SM_TEST_CONTROL_DEFAULT_MASK                    (0)  /* Default test control */

#define GNSS_NV_EFS_SM_GM_DRIVE_TO_PED_PREDICTOR_ENABLE_DEFAULT     (1)  /* Default enable predictor in drive to ped detector*/

#define GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_FOR_DR_DEFAULT             (250)  /* Default HEPE threshold for DR/INS fixes is 250m */

#define GNSS_NV_EFS_SM_XTRAINT_AUTO_DOWNLOAD_ENABLE_DEFAULT         (0)  /* Default for Xtra Integrity auto download feature */
                                                                          /* 0 - disabled, 1 - enabled                        */
#define GNSS_NV_EFS_SM_XTRAINT_DOWNLOAD_INTERVAL_DEFAULT            (12)  /* Default for Xtra Integrity download interval (hours) */
#define GNSS_NV_EFS_SM_GTP_WWAN_FIX_TIMER_DURATION_VAL_DEFAULT      (0x00050C14) /*Wait duration for each RAT in one byet 0x00|LTE-MI(5s)|LTE-NB(12s)|GSM(20s)*/
#define GNSS_NV_EFS_SM_GTP_WWAN_MIN_INTERVAL_BTWN_REQ_VAL_DEFAULT   (0x012C) /*Default min time between two calls to GTP WWAN server*/
#define GNSS_NV_EFS_SM_BYPASS_SYS_UNC_CHECK_VAL_DEFAULT             (0) /*Flag to check if the Sys Uncertainity check can be skipped. Default val 0, Set 1 to skip check*/
#define GNSS_NV_EFS_SM_BYPASS_XTRA_VALIDITY_CHECK_DEFAULT           (0) /*Flag to check if the XTRA validity can be skipped. Default val 0, Set 1 to skip check*/
#define GNSS_NV_EFS_SM_GTP_WWAN_FIX_TIMER_VAL_DEFAULT               (20000) /*default timer value for GTP WWAN sever*/
#define GNSS_NV_EFS_SM_GNSS_CONTROL_FIX_OPERATION_DEFAULT           (0) /*Flag to control default fix operation. Default value is 0.
																		  Bit 0 corresponds to cache fix control, set it to 1 to disable cache fix propogation and reporting to client*/

/*
 * Structure definitions
*/

/*Struct for AGPS RAT preference NV*/
typedef PACKED struct PACKED_POST
{
  uint8 umts_lte_prefered; /*If UMTS/LTE to be prefered over other RAT when both are camped*/
  uint16 agps_rat_enable_bit_mask; /*Bit mask to enable/disable AGPS on RAT's*/
} cgps_nv_agps_rat_pref_config; 

/* Structure for configuring the motion sensing distance for GM in case of challenging GPS env*/
typedef PACKED struct PACKED_POST
{
  uint8 u_version;
  uint32 q_dist_used_for_cpi; /* Motion sensing distance used for CPI  */
  uint32 q_dist_used_for_gps; /* Motion sensing distance used for GPS fixes */
  uint32 q_reserved[10];
}gm_chal_gps_env_ms_dist_config_s_type;


/* Structure for configuring the backoff time in GM in case of challenging GPS env*/
typedef PACKED struct PACKED_POST
{  
  uint32 q_backoff_min; /* Minimum backoff when in Challenging GPS Env in seconds */
  uint32 q_backoff_max; /* Maximum backoff when in Challenging GPS Env in seconds  */
}gm_chal_gps_env_backoff_config_s_type;

typedef PACKED struct PACKED_POST
{
  uint32 q_dist_used_for_cpi; /* Motion sensing distance used for CPI  */
  uint32 q_dist_used_for_gps; /* Motion sensing distance used for GPS fixes */
}aon_chal_gps_env_ms_dist_config_s_type;

/*structure for configuring motion state speeds used by Geofencing*/
typedef PACKED struct PACKED_POST
{
  float gm_ms_walk_speed ;
  float gm_ms_run_speed;
} gm_ms_speed_config_s_type;


/* Structure for configuring the GM LOWI registration */
typedef PACKED struct PACKED_POST
{  
  uint32 q_enable_reg_timer_mask; /* Mask to identify which timer to enable for LOWI registration retry */
  uint32 q_short_term_timer_max_retry_cnt; /* Maximum retry count for the short term timer  */
}gm_lowi_reg_config_s_type;


#define GNSS_NV_TM_TRANS_DELAY_MAX_PLANE 2
#define GNSS_NV_TM_TRANS_DELAY_CP        0
#define GNSS_NV_TM_TRANS_DELAY_UP        1

#define GNSS_NV_TM_TRANS_DELAY_MAX_INDEX 6
#define GNSS_NV_TM_TRANS_DELAY_GSM       0
#define GNSS_NV_TM_TRANS_DELAY_WCDMA     1
#define GNSS_NV_TM_TRANS_DELAY_LTELPP    2

/*Structure used to configure the transmission delay in milliseconds. 
   Used to compensate the timeout value for protocols */
typedef PACKED struct PACKED_POST
{
  uint32 trans_delay[GNSS_NV_TM_TRANS_DELAY_MAX_INDEX][GNSS_NV_TM_TRANS_DELAY_MAX_PLANE]; /*delay stored in milliseconds */
  /* Following is the layout of the above array:
    -----------------------------------------------------
     Protocol/Session Type          CP(ms)  |   UP(ms)
     -----------------------------------------------------
     GSM            |           RRLP    |   RRLP
     -----------------------------------------------------
     WCDMA          |           RRC     |   RRLP
     -----------------------------------------------------
     LTE-LPP            |           LPP     |   LPP
     -----------------------------------------------------
     LTE-RRLP       |           LPP     |   RRLP
     -----------------------------------------------------
     Reserved_0          |          Reserved |     Reserved
     -----------------------------------------------------
     Reserved_1     |           Reserved |  Reserved
     -----------------------------------------------------
   */    
}tm_config_agnss_transmission_delay_s_type;


/* these are the SM related NV items. Add other NV items below this. All SM items are in
   SM/ directory, this is a subset of cgps_nv_efs_reg_item_type pertaining to SM */

/* This should be max number of SM NV items in the table below */
extern cgps_nv_efs_reg_item_struct cgps_nv_efs_sm_reg_items[];


/*===========================================================================

FUNCTION sm_nv_efs_init

DESCRIPTION
  This function is used to initialize internal SM-NV data efs structure.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
void sm_nv_efs_init (void);

/*===========================================================================

FUNCTION sm_nv_efs_reg_item_read

DESCRIPTION
  This function is used to read EFS NV item from EFS. If the read fails, it sets
  it to default value.
  GPS SHALL CALL THIS ONLY DURING BOOTUP INITIALIZATION
  This function will read carrier-specific EFS NV from current DD-sub
  
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_efs_reg_item_read(cgps_nv_efs_reg_item_type const item, 
                                uint8 data_size,
                                void *data_ptr
                                );

/*===========================================================================

FUNCTION sm_nv_efs_reg_item_write

DESCRIPTION
  This function is used to write an EFS NV item. 
  This function will write carrier-specific EFS NV to current DD-sub
  
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_efs_reg_item_write(cgps_nv_efs_reg_item_type const item, 
                                uint8 data_size,
                                void *data_ptr
                                );

/*===========================================================================

FUNCTION sm_nv_efs_reg_item_read_ex

DESCRIPTION
  This function is used to read EFS NV item. If the read fails, it sets it to
  default value. If cached value is present, will return cached value instead
  of reading from EFS.
  GPS SHALL CALL THIS API INSTEAD OF sm_nv_read AFTER BOOT UP INITIALIZATION
  
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_efs_reg_item_read_ex(cgps_nv_efs_reg_item_type const item, 
                                uint8 data_size,
                                void *data_ptr
                                );


/*===========================================================================

FUNCTION sm_nv_efs_reg_item_read_from_sub

DESCRIPTION
  This function is used to read EFS NV item for a specific Subs ID.
  If the read fails, it sets it to default value. It'll also set the
  cache values if available. This function should be called at bootup
  and during SIM swap (always read from EFS not cache).
  
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_efs_reg_item_read_from_sub(cgps_nv_efs_reg_item_type const item, 
                                uint8 data_size,
                                void *data_ptr,
                                sys_modem_as_id_e_type sub
                                );

/*===========================================================================

FUNCTION sm_nv_efs_reg_item_write_to_sub

DESCRIPTION
  This function is used to write an EFS NV item to a specific Subs ID
 
  
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_efs_reg_item_write_to_sub(cgps_nv_efs_reg_item_type const item, 
                                uint8 data_size,
                                void *data_ptr,
                                sys_modem_as_id_e_type sub
                                );


#endif /* SM_NV_EFS_H */
