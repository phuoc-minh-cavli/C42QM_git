/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      Session Manager NV Items Module

GENERAL DESCRIPTION
  This module contains the SM interface routines for the Non-Volatile Memory
  module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015-2021 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/src/sm_nv_efs.c#2 $
  $DateTime: 2021/03/09 23:29:53 $
  $Author: pwbldsvc $

=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/27/21   sai     Added NV for Controlling Fix related operations, currently controls cache fix propogation and its reporting to client
10/19/17   rn      Add NVs for Xtra Integrity Auto Download (DL) enable and DL Interval.
07/19/17   ak      Added NV for HEPE threshold config for DR/INS fixes
08/30/16   skm     Added NV for default LPP12 2 timer feature config 
04/15/16   ld      Added NHz NV support
02/17/16   mj      Added NV support to enable/disable CPI injections and early exit logic 
02/17/16   mj      Remove extra space in NV name for CPI/Early Exit feature 
02/11/16   gk      Added LPPe support
08/13/15   sjk     Added default configuration of GM from NV for AON
06/26/15   sjk      Adding NV for configuring Location Batching 
06/09/15   ssu      Adding NV for GM filtering of WLAN freq. 
03/14/15   rh      Added support for MultiSIM-EFS NV API 
02/26/15   ssu      Adding new NV items for GM LOWI integration.
01/14/15   rk      Added LPP >24 AD NV support.
11/13/14   rh      Added protocol transmission delay EFS NV  
02/21/14   ssu   Adding NV for configuring motion sensing distance in case of challenging GPS env. 
02/20/14   ssu     Adding a NV for modifying the medium responsiveness backoff value and gps challenging env backoff config. 
02/10/14   ssu     Adding a NV for modifying the rate at which GM requests CPI injection. 
01/10/14   ss       Supporting High Confidence Geofence config
11/08/12   rh      Added FEATURE_GNSS_NO_NV_WRITES support for Mav 
07/17/12   mjohn   Added NV GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES for Premium Services (Enhanced Cell ID)
08/09/12   rk      Rename "gnss_lte_lpp_enable" EFS NV item to "gnss_lpp_enable" for 
                   tools to set/get this NV on all targets.
06/11/12   rk      Added support to get GNSS_NV_EFS_SM_TM_ASSISTED_GLO_PROTOCOL_SELECT NV item value.
11/24/10   rh      Added NV item for sending NO_FIX as Intermediate Report
06/10/09   gk      Added Get UTC offset from NV
01/15/09    atien  XTRA-T support
01/25/09   ns      Added support for QWiP NV item
08/22/08   jlp     Added support of emergency call simulation for simulating
                   E911 A-GPS
09/14/07    gk     NMEA CONFIG set parameters support
08/01/07    rw     Add support for XTRA feature
07/31/07    ank    Added Dynamic QoS and Override logic.
05/01/07    lt     Restore the default for cgps_mo_method back to MO_CP.
04/17/07    lt     Changed default for cgps_mo_method to MO_CP_LOC_EST.
04/12/07    ah     Added default read values for all SM-related NV items.
12/22/06    ank    Added default values for various NV items.
11/14/06    ah     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*
 * Include files
*/
#include "comdef.h"
//#include "customer.h"
#include "msg.h"
#include "sm_nv_efs.h"
#include "sm_nv.h"
#include "stringl.h"

/* ensure the order of the items in this table is same as the item definitions */
static const cgps_nv_efs_reg_item_struct sm_nv_reg_items[] =
{
  /* data location, item file name, size */

  /*CGPS_NV_EFS_SM_XTRA_PREFERRED_MAX_VALID_AGE , */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "xtra/cgps_xtra_preferred_max_valid_age", sizeof(uint16), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},
  /*CGPS_NV_EFS_SM_SEND_NO_FIX_REPORT , */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gnss_send_no_fix_report", sizeof(uint8), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},  
  /*GNSS_NV_EFS_CONFIG_QMI_SERVICE_EXTERNAL, */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "qmi/gnss_qmi_config", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},  

   /* GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gnss_lm_hepe_threshold", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},
  

  /* GNSS_NV_EFS_SM_GM_GNSS_UNAVAIL_IND_TIMEOUT */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_gnss_unavail_ind_timeout", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},
   
   /* GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESSION_TIMEOUT */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_gnss_position_qos_session_timeout", sizeof(uint8), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

   /* GNSS_NV_EFS_SM_GM_POSITION_SOURCES */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_position_sources", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_XTRA_LAST_XTRA3_KEY_INDEX */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "xtra/gnss_xtra3_last_key_index", sizeof(uint16), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

 
  /* GNSS_NV_EFS_SM_GM_GNSS_MAX_POS_UNC_ACCEPTED */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_gnss_max_pos_unc_accepted", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},  

  /* GNSS_NV_EFS_SM_GM_HIGH_RESPONSIVENESS_CONFIG */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_high_responsiveness_config", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

/* GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_cpi_request_rate", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_GM_MED_RESP_BACKOFF */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_med_resp_backoff", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},
  
  /* GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_CONFIG */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_challenging_gps_env_backoff_config", sizeof(gm_chal_gps_env_backoff_config_s_type), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CONFIG */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_challenging_gps_env_motion_sensing_dist_config", sizeof(gm_chal_gps_env_ms_dist_config_s_type), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},
  
  /* GNSS_NV_EFS_SM_GM_MS_SPEED_CONFIG */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_motion_state_speed_config", sizeof(gm_ms_speed_config_s_type), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_TM_MEAS_QMI_REPORT_CONTROL */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gnss_meas_qmi_reporting_enable_control", sizeof(uint8), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_GM_COMP_LOGGING_RATE */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_comp_logging_rate_secs", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_GM_IOD_CONFIG_SWITCHES_MASK */
  { CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/iod_configuration_switches", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL }, 

   /* GNSS_NV_EFS_SM_BATCHING_GF_RADIUS */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/batching_gf_radius", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_BATCHING_CONFIG_MASK */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/batching_config_mask", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_AON_DEFAULT_GNSS_POS_QOS_SESSION_TIMEOUT */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/aon_default_gnss_position_qos_session_timeout", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_AON_DEFAULT_CPI_REQUEST_RATE */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/aon_default_cpi_request_rate", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_BO_CONFIG */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/aon_default_challenging_gps_env_backoff_config", sizeof(gm_chal_gps_env_backoff_config_s_type), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_MS_DIST_CONFIG  */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/aon_default_challenge_gps_env_motion_sensing_dist_config",sizeof(aon_chal_gps_env_ms_dist_config_s_type), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_TM_UAX3_CONFIG */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "xtra/gnss_uax3_config", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_NHZ_CONFIG  */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gnss_nhz_config", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_FOR_DR */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gnss_lm_hepe_threshold_dr_fixes",sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_GTP_WWAN_FIX_TIMER_DURATION */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gtp_wwan_fix_timer_duration",sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_GTP_WWAN_MIN_INTERVAL_BTWN_REQ */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gtp_wwan_min_interval_btwn_req",sizeof(uint16), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_BYPASS_GNSS_AIDING_CHECK */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/bypass_gnss_aiding_check",sizeof(uint8), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_BYPASS_XTRA_VALIDITY_CHECK */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/bypass_xtra_validity_check",sizeof(uint8), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},

  /* GNSS_NV_EFS_SM_GM_MOTION_DETECTION_SOURCES */
  { CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gm_motion_detection_sources", sizeof(uint32), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL },

  /* GNSS_NV_EFS_SM_GNSS_CONTROL_FIX_OPERATION */
  { CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "sm/gnss_control_fix_operation", sizeof(uint8), 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL },
  /* add entries below this line and make sure you update the CGPS_NV_EFS_SM_NUM_ITEMS */
};


#ifdef FEATURE_GNSS_NO_NV_WRITES
#error code not present
#endif /* FEATURE_GNSS_NO_NV_WRITES */


/*===========================================================================

FUNCTION sm_nv_efs_map_sub  //  TODO  [an][del] see if this can be removed since sub removed

DESCRIPTION
  This function is used to map Subscription ID to proper value (if it's
  subscription-indepdendent NV, it'll always map to Subs 0)
 
  
RETURN VALUE
  cgps_nv_sub_id_type 
  
DEPENDENCIES
 
===========================================================================*/
static sys_modem_as_id_e_type sm_nv_efs_map_sub(cgps_nv_efs_reg_item_type const item)
{


    return SM_CONFIG_MIN_SUB_ID;  /* subscription-independent EFS NV, return 0 */
}

/*==============================================================================
  FUNCTION
    sm_nv_efs_reg_get_default

  DESCRIPTION
    Retrieve the default value for specified EFS NV item and store it in 'dest'.
    Note that the size of 'dest' is implicit via the item type.
 
    NB: This function may block if the data source resides in another task.
    Returns True if successful, False on error. This function should be expanded
    to include all default values.
 
  RETURN VALUE
    TRUE if no ERROR
    FALSE if error

  
==============================================================================*/
boolean sm_nv_efs_reg_get_default(cgps_nv_efs_reg_item_type const item, uint8 data_size,
                                  void *data_ptr) 
{
  if (data_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer");
    return FALSE;
  }

  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "CGPS_NV_EFS_REG: Registry Get %d Default", item); 

  switch (item)
  {
    case CGPS_NV_EFS_SM_XTRA_PREFERRED_MAX_VALID_AGE: 
      if (data_size == sizeof(uint16))
      {
        *(uint16 *)data_ptr = CGPS_NV_EFS_SM_XTRA_PREFERRED_MAX_VALID_AGE_DEFAULT;
      }
      break;
    case GNSS_NV_EFS_SM_SEND_NO_FIX_REPORT: 
      if (data_size == sizeof(uint8))
      {
        *(uint8 *)data_ptr = GNSS_NV_EFS_SM_SEND_NO_FIX_REPORT_DEFAULT; 
      }
      break;

    case GNSS_NV_EFS_CONFIG_QMI_SERVICE_EXTERNAL:
      if ( data_size == sizeof(uint32) )
      {
        *(uint32 *)data_ptr = GNSS_NV_EFS_CONFIG_QMI_SERVICE_EXTERNAL_DEFAULT;
      }
      break;
      
    case GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD:
      if ( data_size == (sizeof(uint32)))
      {
        *(uint32 *)data_ptr = GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_DEFAULT;
      }
      break;
    
    case GNSS_NV_EFS_SM_GM_GNSS_UNAVAIL_IND_TIMEOUT:
      if ( data_size == (sizeof(uint32)))
      {
         *(uint32 *)data_ptr = GNSS_NV_EFS_SM_GM_GNSS_UNAVAIL_IND_TIMEOUT_DEFAULT;
      }
      break;  

    case GNSS_NV_EFS_SM_GM_MOTION_DETECTION_SOURCES:
      if (data_size == (sizeof(uint32)))
      {
        *(uint32 *)data_ptr = GNSS_NV_EFS_SM_GM_MOTION_DETECTION_SOURCES_DEFAULT;
      }
      break;

  case GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT:
    if ( data_size == (sizeof(uint8)))
    {
      *(uint8 *)data_ptr = GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT_DEFAULT;
    }
    break;    

  case GNSS_NV_EFS_SM_GM_POSITION_SOURCES:
    if ( data_size == (sizeof(uint32)))
    {
      *(uint32 *)data_ptr = GNSS_NV_EFS_SM_GM_POSITION_SOURCES_DEFAULT;
    }
    break;
  
    case GNSS_NV_EFS_SM_XTRA_LAST_XTRA3_KEY_INDEX:
      if (data_size == sizeof(uint16))
      {
        *(uint16 *)data_ptr = GNSS_NV_EFS_SM_XTRA_LAST_XTRA3_KEY_INDEX_DEFAULT;
      }
      break;

  case GNSS_NV_EFS_SM_GM_GNSS_MAX_POS_UNC_ACCEPTED:
    if ( data_size == (sizeof(uint32)))
    {
      *(uint32 *)data_ptr = GNSS_NV_EFS_SM_GM_GNSS_MAX_POS_UNC_ACCEPTED_DEFAULT;
    }
    break;

  case GNSS_NV_EFS_SM_GM_HIGH_RESPONSIVENESS_CONFIG:
    if ( data_size == (sizeof(uint32)))
    {
      *(uint32 *)data_ptr = GNSS_NV_EFS_SM_GM_HIGH_RESPONSIVENESS_CONFIG_DEFAULT;
    }
    break;
     
    case GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE:
      if ( data_size == (sizeof(uint32)))
      {
        *(uint32 *)data_ptr = GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE_DEFAULT;
      }
      break;
 
      case GNSS_NV_EFS_SM_GM_MED_RESP_BACKOFF:
      if ( data_size == (sizeof(uint32)))
      {
        *(uint32 *)data_ptr = GNSS_NV_EFS_SM_GM_MED_RESP_BACKOFF_DEFAULT;
      }
      break;

      case GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_CONFIG:
      if ( data_size == (sizeof(gm_chal_gps_env_backoff_config_s_type)))
      {
        gm_chal_gps_env_backoff_config_s_type *backoff_config = (gm_chal_gps_env_backoff_config_s_type*)data_ptr;
        backoff_config->q_backoff_min = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MIN_DEFAULT;
        backoff_config->q_backoff_max = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MAX_DEFAULT;
      }
      break;

      case GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CONFIG:
      if ( data_size == (sizeof(gm_chal_gps_env_ms_dist_config_s_type)))
      {
        gm_chal_gps_env_ms_dist_config_s_type* dist_config = (gm_chal_gps_env_ms_dist_config_s_type*)data_ptr;
        dist_config->q_dist_used_for_gps = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_GPS_DEFAULT;
        dist_config->q_dist_used_for_cpi = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CPI_DEFAULT;
      }
      break;

    case GNSS_NV_EFS_SM_GM_MS_SPEED_CONFIG:
    if ( data_size == (sizeof(gm_ms_speed_config_s_type)))
    {
      gm_ms_speed_config_s_type* speed_config = (gm_ms_speed_config_s_type*)data_ptr;
      speed_config->gm_ms_walk_speed = GNSS_NV_EFS_SM_GM_MS_WALK_SPEED_DEFAULT;
      speed_config->gm_ms_run_speed = GNSS_NV_EFS_SM_GM_MS_RUN_SPEED_DEFAULT;
    }
    break;


    case GNSS_NV_EFS_SM_BATCHING_GF_RADIUS:
      if ( data_size == (sizeof(uint32)))
      {
        *(uint32 *)data_ptr = GNSS_NV_EFS_SM_BATCHING_GF_RADIUS_DEFAULT;
      }
      break;

    case GNSS_NV_EFS_SM_BATCHING_CONFIG_MASK:
      if ( data_size == (sizeof(uint32)))
      {
        *(uint32 *)data_ptr = GNSS_NV_EFS_SM_BATCHING_CONFIG_MASK_DEFAULT;
      }
      break;
    case GNSS_NV_EFS_SM_GM_IOD_CONFIG_SWITCHES_MASK:
      if ( data_size == sizeof(uint32) )
      {
        *(uint32 *)data_ptr = GNSS_NV_EFS_SM_GM_IOD_CONFIG_SWITCHES_MASK_DEFAULT;
      }
      break;       
      
    case GNSS_NV_EFS_SM_GM_COMP_LOGGING_RATE:
    if ( data_size == (sizeof(uint32)))
    {
      *(uint32 *)data_ptr = GNSS_NV_EFS_SM_GM_COMP_LOGGING_RATE_DEFAULT;
    }
    break;

    case GNSS_NV_EFS_SM_AON_GNSS_POS_QOS_SESSION_TIMEOUT:
      if ( data_size == (sizeof(uint32)))
      {
        *(uint32 *)data_ptr = GNSS_NV_EFS_SM_AON_DEFAULT_GNSS_POS_QOS_SESSION_TIMEOUT;
      }
    break;

    case GNSS_NV_EFS_SM_AON_CPI_REQUEST_RATE:
      if ( data_size == (sizeof(uint32)))
      {
        *(uint32 *)data_ptr = GNSS_NV_EFS_SM_AON_DEFAULT_CPI_REQUEST_RATE;
      }
    break;

    case GNSS_NV_EFS_SM_AON_CHAL_GPS_ENV_BO_CONFIG:
      if ( data_size == (sizeof(gm_chal_gps_env_backoff_config_s_type)))
      {
        gm_chal_gps_env_backoff_config_s_type *backoff_config = (gm_chal_gps_env_backoff_config_s_type*)data_ptr;
        backoff_config->q_backoff_min = GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_BO_MIN;
        backoff_config->q_backoff_max = GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_BO_MAX;
      }
    break;

    case GNSS_NV_EFS_SM_AON_CHAL_GPS_ENV_MS_DIST_CONFIG:
      if ( data_size == (sizeof(aon_chal_gps_env_ms_dist_config_s_type)))
      {
        aon_chal_gps_env_ms_dist_config_s_type* dist_config = (aon_chal_gps_env_ms_dist_config_s_type*)data_ptr;
        dist_config->q_dist_used_for_gps = GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_MS_DIST_GPS;
        dist_config->q_dist_used_for_cpi = GNSS_NV_EFS_SM_AON_DEFAULT_CHAL_GPS_ENV_MS_DIST_CPI;
      }
    break;

    case GNSS_NV_EFS_SM_TM_UAX3_CONFIG:
    if ( data_size == (sizeof(uint32)))
    {
      *(uint32 *)data_ptr = GNSS_NV_EFS_SM_TM_UAX3_CONFIG_DEFAULT;  /* UAX3 is enabled */
    }
    break;

    case GNSS_NV_EFS_SM_NHZ_CONFIG:
      if ( data_size == (sizeof(uint32)))
      {
        *(uint32 *)data_ptr = GNSS_NV_EFS_SM_NHZ_CONFIG_DEFAULT;
      }
      break;

   

    case GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_FOR_DR:
      if ( data_size == (sizeof(uint32)))
      {
         *(uint32 *)data_ptr = GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_FOR_DR_DEFAULT;  /* Set default HEPE THRESHOLD for DR/INS fixes */
          return TRUE;
      }
    break;

    case GNSS_NV_EFS_SM_GTP_WWAN_FIX_TIMER_DURATION:
      if ( data_size == (sizeof(uint32)))
      {
         *(uint32 *)data_ptr = GNSS_NV_EFS_SM_GTP_WWAN_FIX_TIMER_DURATION_VAL_DEFAULT;  /* Set timer duration for GTP wwan fix */
          return TRUE;
      }
    break;
    case GNSS_NV_EFS_SM_GTP_WWAN_MIN_INTERVAL_BTWN_REQ:
      if ( data_size == (sizeof(uint16)))
      {
         *(uint16 *)data_ptr = GNSS_NV_EFS_SM_GTP_WWAN_MIN_INTERVAL_BTWN_REQ_VAL_DEFAULT;  /* Set the duration between two req for fixes */
          return TRUE;
      }
    break;
    case GNSS_NV_EFS_SM_BYPASS_GNSS_AIDING_CHECK:
     if ( data_size == (sizeof(uint8)))
     {
       *(uint8 *)data_ptr = GNSS_NV_EFS_SM_BYPASS_SYS_UNC_CHECK_VAL_DEFAULT;  /* Set default HEPE THRESHOLD for DR/INS fixes */
        return TRUE;
     }
    break;
    #if 0
    case GNSS_NV_EFS_SM_XTRAINT_AUTO_DOWNLOAD_ENABLE:
      if ( data_size == (sizeof(uint8)))
      {
        *(uint8 *)data_ptr = GNSS_NV_EFS_SM_XTRAINT_AUTO_DOWNLOAD_ENABLE_DEFAULT;
      }
      break;    
    
    case GNSS_NV_EFS_SM_XTRAINT_DOWNLOAD_INTERVAL:
      if ( data_size == (sizeof(uint16)))
      {
        *(uint16 *)data_ptr = GNSS_NV_EFS_SM_XTRAINT_DOWNLOAD_INTERVAL_DEFAULT;  
      }
      break;
#endif  /* #if 0 */
    case GNSS_NV_EFS_SM_BYPASS_XTRA_VALIDITY_CHECK:
     if ( data_size == (sizeof(uint8)))
     {
       *(uint8 *)data_ptr = GNSS_NV_EFS_SM_BYPASS_XTRA_VALIDITY_CHECK_DEFAULT;
        return TRUE;
     }
    break;
	case GNSS_NV_EFS_SM_GNSS_CONTROL_FIX_OPERATION:
     if ( data_size == (sizeof(uint8)))
     {
       *(uint8 *)data_ptr = GNSS_NV_EFS_SM_GNSS_CONTROL_FIX_OPERATION_DEFAULT;
        return TRUE;
     }
    break;
    default:
      MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "CGPS_NV_EFS_REG: Invalid item id: %d", item);
      return FALSE;
  }

  return TRUE;
}


#ifdef FEATURE_GNSS_NO_NV_WRITES
#error code not present
#endif /* FEATURE_GNSS_NO_NV_WRITES */


/*===========================================================================

FUNCTION sm_nv_efs_init

DESCRIPTION
  This function is used to initialize internal SM-NV data efs structure.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
void sm_nv_efs_init (void)
{
  #ifndef FEATURE_GNSS_LEAN_BUILD

  boolean ret_val = FALSE;
  ret_val =  cgps_nv_efs_init (CGPS_NV_EFS_SM, CGPS_NV_EFS_SM_MIN_ITEM,
                               CGPS_NV_EFS_SM_MAX_ITEM,
                               sm_nv_reg_items);

#ifdef FEATURE_GNSS_NO_NV_WRITES
  #error code not present
#endif /* FEATURE_GNSS_NO_NV_WRITES */

  if (ret_val)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM_NV_EFS: NV EFS init successful");
  }
  else
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM_NV_EFS: NV EFS init failed");
  }

  #endif
}


/*===========================================================================

FUNCTION sm_nv_efs_reg_item_read

DESCRIPTION
  This function is used to read EFS NV item. If the read fails, it sets it to
  default value. If FEATURE_GNSS_NO_NV_WRITES feature is turned on, it'll
  read from SM EFS NV cache.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_efs_reg_item_read(cgps_nv_efs_reg_item_type const item, 
                                uint8 data_size,
                                void *data_ptr 
                               )
{
#ifndef FEATURE_GNSS_LEAN_BUILD
  if (data_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer");
    return FALSE;
  }

  /* Using current DD-sub for carrier-specific NV if No Sub is specificed */
  return sm_nv_efs_reg_item_read_from_sub(item, data_size, data_ptr,SYS_MODEM_AS_ID_1);
  #else
return TRUE;
#endif
}

/*===========================================================================

FUNCTION sm_nv_efs_reg_item_read_ex

DESCRIPTION
  This function is used to read EFS NV item. If the read fails, it sets it to
  default value. If cached value is present, will return cached value instead
  of reading from EFS.
  GPS SHALL CALL THIS API INSTEAD OF sm_nv_read AFTER BOOT UP INITIALIZATION
  
  Using current DD-sub for carrier-specific EFS NVs
 
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_efs_reg_item_read_ex(cgps_nv_efs_reg_item_type const item, 
                                uint8 data_size,
                                void *data_ptr
                                )
{


#ifndef FEATURE_GNSS_LEAN_BUILD

  cgps_nv_efs_reg_rw_stat_enum_type status = CGPS_NV_EFS_REG_RW_STAT_INVALID;    
  boolean return_value = FALSE;

  if (data_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer");
    return FALSE;
  }

#ifdef FEATURE_GNSS_NO_NV_WRITES
#error code not present
#else

  status =  cgps_nv_efs_reg_item_read_subsid(item, data_size, data_ptr, 
                                      CGPS_NV_EFS_SM_MIN_ITEM, CGPS_NV_EFS_SM_MAX_ITEM, sm_nv_reg_items, 
                                      (cgps_nv_efs_sub_id_type)sm_nv_efs_map_sub(item));

#endif /* FEATURE_GNSS_NO_NV_WRITES */

  if (status == CGPS_NV_EFS_REG_RW_STAT_OK )
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM_NV_EFS: NV EFS file read succeeded for item=%d (sub=%d)", 
           item, 0);
    return_value = TRUE;
  }
  else
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM_NV_EFS: NV EFS file read failed for item=%d(sub=%d), setting defaults", 
            item, 0);
    return_value = sm_nv_efs_reg_get_default(item, data_size, data_ptr);
  }

  return return_value;


#else
  return TRUE;
#endif

}


/*===========================================================================

FUNCTION sm_nv_efs_reg_item_write

DESCRIPTION
  This function is used to write an NV item. 

  
RETURN VALUE
  TRUE for success, otherwise FALSE
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_efs_reg_item_write(cgps_nv_efs_reg_item_type const item, 
                                 uint8 data_size,
                                 void *data_ptr
                                )
{
  if (data_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer");
    return FALSE;
  }

  /* Using current DD-sub for carrier-specific NV if No Sub is specificed */
  return sm_nv_efs_reg_item_write_to_sub(item, data_size, data_ptr,SYS_MODEM_AS_ID_1);
  }


/*===========================================================================

FUNCTION sm_nv_efs_reg_item_read_sub

DESCRIPTION
  This function is used to read EFS NV item for a specific subscription.
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
                                )
{
  cgps_nv_efs_reg_rw_stat_enum_type status = CGPS_NV_EFS_REG_RW_STAT_INVALID;    
  boolean return_value = FALSE;

  if (data_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer");
    return FALSE;
  }

  if ((item >= CGPS_NV_EFS_SM_MIN_ITEM) ||
      (item <= CGPS_NV_EFS_SM_MAX_ITEM))
  {
     status =  cgps_nv_efs_reg_item_read_subsid(item, data_size, data_ptr, 
                                         CGPS_NV_EFS_SM_MIN_ITEM,
                                         CGPS_NV_EFS_SM_MAX_ITEM, sm_nv_reg_items,
                                         (cgps_nv_efs_sub_id_type)sub
                                         );
  }
  else
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NV item index %d, out of range", (int)item);
    return FALSE;
  }

  if (status == CGPS_NV_EFS_REG_RW_STAT_OK )
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM_NV_EFS: NV EFS file read succeeded for item=%d(sub=%d)", item, 0);
    return_value = TRUE;
  }
  else
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM_NV_EFS: NV EFS file read failed for item=%d(sub=%d), setting defaults", item, 0);
    return_value = sm_nv_efs_reg_get_default(item, data_size, data_ptr);
  }

#ifdef FEATURE_GNSS_NO_NV_WRITES
  #error code not present
#endif /* FEATURE_GNSS_NO_NV_WRITES */

  return return_value;
}


/*===========================================================================

FUNCTION sm_nv_efs_reg_item_write

DESCRIPTION
  This function is used to write an EFS NV item to a specific subscription
 
  
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_efs_reg_item_write_to_sub(cgps_nv_efs_reg_item_type const item, 
                                uint8 data_size,
                                void *data_ptr,
                                sys_modem_as_id_e_type sub
                                )
{
#ifndef FEATURE_GNSS_LEAN_BUILD
  cgps_nv_efs_reg_rw_stat_enum_type status = CGPS_NV_EFS_REG_RW_STAT_INVALID;    
  boolean return_value = FALSE;

  if (data_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer");
    return FALSE;
  }  

#ifdef FEATURE_GNSS_NO_NV_WRITES
   #error code not present
#else

  status =  cgps_nv_efs_reg_item_write_subsid(item, data_size, data_ptr, 
                                       CGPS_NV_EFS_SM_MIN_ITEM,
                                       CGPS_NV_EFS_SM_MAX_ITEM, sm_nv_reg_items,
                                       (cgps_nv_efs_sub_id_type)sub
                                       );

#endif /* FEATURE_GNSS_NO_NV_WRITES */

  if (status == CGPS_NV_EFS_REG_RW_STAT_OK )
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM_NV_EFS: NV EFS file write succeeded for item=%d(sub=%d)",item,0);
    return_value = TRUE;
  }
  else
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM_NV_EFS: NV EFS file write failed for item=%d(sub=%d)",item,0);
  }

  return return_value;


#else
  return TRUE;
#endif


}



