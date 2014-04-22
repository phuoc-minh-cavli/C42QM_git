/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   M O D U L E
                        Task related functions

GENERAL DESCRIPTION
  This module is created for handling the PSM related functionality.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/cm/src/cmpsm.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/15   SKK      Initial release.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
#include "cm.h"       /* Interface to CM client service.                   */
#include "cmph.h"
#include "cmidbg.h"
#include "cmtaski.h"
#include "cmmmgsdi.h"
#include "pm_rtc.h"
#include "pmapp_rtc.h"
#include "pm_pon.h"
#include "cmpsm.h"
#include "mmoc_v.h"
#include "sys_m_reset.h"
#include "pm_clk.h" 
#include "DALSys.h"
#include "fs_public.h" 
#ifdef FEATURE_REF_TRACKER
#include "cm_psm_uart.h"
#endif

#ifdef CM_DEBUG
#error code not present
#endif
#if !defined(FEATURE_BUILD_PAGEONLY_IMAGE)
#include "mcpm_api.h"                 /*For MCPM_GPIO_TOGGLE*/
#endif
#if !defined FEATURE_BUILD_SENSORONLY_IMAGE && !defined FEATURE_BUILD_PAGEONLY_IMAGE
#include "baseband_config.h"
#endif
/* If this bit is set, next boot image type will be Full */
#define  FULL_BOOT_BIT 0x0

/* If this bit is set, next boot image type will be Page_only */
#define  PAGE_BOOT_BIT  0x1

/* If this bit is set, Apps will be available in next boot */
#define APPS_BIT  0x10

/* If this bit is set, next boot image type will be Sensor_only */
#define SNS_BOOT_BIT 0x03 /* it is tentative can change */

/* This bit is set,logs can be downloaded and crashdumps can be collected */ 
#define DWNLD_BIT 0x40 /* 6th bit is going to be set as DWNLDBIT */

#define ALL_PSM_BIT (PAGE_BOOT_BIT|APPS_BIT|SNS_BOOT_BIT|DWNLD_BIT)


#define PRIMARY_DEVICE 0
#define PON_DISABLE   0
#define CHIP_CODE 0

static timer_type         cmpsm_deep_sleep_timer;
static timer_type         cmpsm_rtc_timer;
static timer_type         cmpsm_inactivity_timer;
static timer_type         cmpsm_high_activity_timer;

/* Timer group which will not be disabled during sleep.
** This group ensures MMOC can wake the mobile up from deep sleep.
*/
extern timer_group_type   mmoc_timer_group;

/* Definition for cmpsm_local.
*/
cm_psm_info_s_type        cmpsm_local;

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/
void cmpsm_wait_for_cond(uint8 mask,uint8 cond);
  static void cmpsm_process_psm_enter_req(uint64 apps_time);
static void cmpsm_start_psm_enter( void );
static void cmpsm_configure_rtc_clk(uint64      psm_time);
#if defined (FEATURE_BUILD_PAGEONLY_IMAGE) || defined(FEATURE_BUILD_SENSORONLY_IMAGE)
extern void qmi_send_psm_status_ind
(
  cm_psm_status_e_type              status,  
  cm_psm_reject_reason_e_type       reject_reason,
  uint32                       periodic_update_timer
);
#endif

#ifdef FEATURE_REF_TRACKER
extern void cmsnsmgr_process_lpm_oos(void);
#endif

/*===========================================================================
FUNCTION cmpsm_ptr

DESCRIPTION
  To get the pointer to psm object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_psm_info_s_type *cmpsm_ptr(void)
{
  return &cmpsm_local;
}

/*===========================================================================
FUNCTION cmpsm_get_baseband_config_param

DESCRIPTION
  Reading baseband_config_param.

DEPENDENCIES
  none

RETURN VALUE
  TRUE -> framework responded with SUCCESS
  FALSE > framework didnt respond with SUCCESS

SIDE EFFECTS
  none

===========================================================================*/
#if !defined FEATURE_BUILD_SENSORONLY_IMAGE && !defined FEATURE_BUILD_PAGEONLY_IMAGE

boolean cmpsm_get_baseband_config_param(bb_config_param_id_e_type param, boolean* param_value)
{
                       
  bb_config_param_status_e_type  status = BB_CONFIG_PARAM_STATUS_ERROR;                                
                                   
  status = baseband_config_param_read(param,param_value,sizeof(boolean));                               
                                   
  if(status == BB_CONFIG_PARAM_STATUS_OK )
  {
    return TRUE;
  }                               
  return FALSE;  
}
#endif
/*===========================================================================
FUNCTION cmpsm_nv_read

DESCRIPTION
  Readin PSM related NV items.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_nv_read(void)
{
  cm_psm_info_s_type    *psm_ptr  =  cmpsm_ptr();
  /* get the pointer to the psm object to be initialized */

  int32 read_size = 0;
  boolean param_value = 0;
  cm_psm_config_ext_2_s_type   psm_config_ext_2;  
  boolean poi_profile_status = FALSE;
#if !defined FEATURE_BUILD_SENSORONLY_IMAGE && !defined FEATURE_BUILD_PAGEONLY_IMAGE
  poi_profile_status = cmpsm_get_baseband_config_param(BB_CONFIG_PARAM_POI,&param_value);
  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_2(cm_get_ulog_handle(), "=CM= BASEBAND_CONFIG_PARAMS status:%d, param_value:%d",
                             poi_profile_status, param_value);
  }
#endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                       READ NV ITEMS*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Read the PSM config */
  read_size = sizeof (cm_psm_config_s_type);

  if ( cmefs_read ( CMEFS_PSM_CONFIG,
                   (byte* )&psm_ptr->psm_cfg_info, &read_size) )
  {
    if((psm_ptr->psm_cfg_info.is_enabled) &&(psm_ptr->psm_cfg_info.threshold < CM_PSM_MIN_THRESHOLD ))
    {
      psm_ptr->psm_cfg_info.threshold = CM_PSM_DEFAULT_THRESHOLD;
    }

    if(psm_ptr->psm_cfg_info.version < 2)
    {
      psm_ptr->psm_cfg_info.tiny_image_config |= CM_PSM_DEV_DWNLD_ENABLE;
    }
    CM_MSG_HIGH_5 ("cmpsm: psm conf(EFS version = %d, PSM_version:%d enabled = %d, threshold = %d,.tiny_image_config:%d",
        psm_ptr->psm_cfg_info.version,psm_ptr->psm_cfg_info.psm_version,
        psm_ptr->psm_cfg_info.is_enabled, psm_ptr->psm_cfg_info.threshold,psm_ptr->psm_cfg_info.tiny_image_config);
  }
  else
  {
    /* set default values */
	/* set PSM enable as default from PDM recommendation */
    psm_ptr->psm_cfg_info.is_enabled = TRUE;
    psm_ptr->psm_cfg_info.threshold = CM_PSM_DEFAULT_THRESHOLD;
    psm_ptr->psm_cfg_info.psm_version = CM_PSM_VERSION_MASK_ALL;
    psm_ptr->psm_cfg_info.tiny_image_config |= CM_PSM_DEV_DWNLD_ENABLE;
  }

    /* Read the PSM config Ext*/
    read_size = sizeof (cm_psm_config_ext_s_type);

    if ( cmefs_read ( CMEFS_PSM_CONFIG_EXT,
               (byte* )&psm_ptr->psm_cfg_ext_info, &read_size) )
    {
       /* If duration_due_to_oos is less than “2*psm threshold time”, UE will enter PSM for “2*psm threshold time” */
       if(psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos < 2 * psm_ptr->psm_cfg_info.threshold)
       {
          psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos = 2 * psm_ptr->psm_cfg_info.threshold;  
       }
       
       CM_MSG_MED_2 ("cmpsm: psm conf ext(version = %d, psm_opt_mask = %u ) ",
           psm_ptr->psm_cfg_ext_info.version, psm_ptr->psm_cfg_ext_info.psm_opt_mask );
       CM_MSG_MED_4 ("cmpsm: psm conf ext(max_oos_full_scans = %d, psm_duration_due_to_oos = %u, randomization_window = %d, max_oos_time = %d) ",
           psm_ptr->psm_cfg_ext_info.max_oos_full_scans, 
           psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos, 
           psm_ptr->psm_cfg_ext_info.psm_randomization_window,
           psm_ptr->psm_cfg_ext_info.max_oos_time );

    if(psm_ptr->psm_cfg_ext_info.max_oos_time < CM_PSM_DEFAULT_MIN_OOS_TIME)
    {
      psm_ptr->psm_cfg_ext_info.max_oos_time = CM_PSM_DEFAULT_MIN_OOS_TIME;
    }

      if(psm_ptr->psm_cfg_ext_info.version < 2)
      {
        psm_ptr->psm_cfg_ext_info.early_wakeup_time = 4;
      }
    }
    else
    {
    /* set default values */
    psm_ptr->psm_cfg_ext_info.psm_opt_mask = CM_PSM_OPT_MASK_DEEP_SLEEP | CM_PSM_OPT_MASK_OOS;
      psm_ptr->psm_cfg_ext_info.max_oos_full_scans       = CM_PSM_DEFAULT_MAX_OOS_SCANS;
      psm_ptr->psm_cfg_ext_info.max_oos_time       = CM_PSM_DEFAULT_MAX_OOS_TIME;
    psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos  = (24 * 60 * 60); //(24 hrs in secs). 
      psm_ptr->psm_cfg_ext_info.psm_randomization_window = CM_PSM_DEFAULT_RANDOMIZATION_WINDOW;
      psm_ptr->psm_cfg_ext_info.early_wakeup_time = 4;
    }

    psm_ptr->psm_rtc_cal = FALSE;
    /* Read the PSM config Ext*/
    read_size = sizeof (cm_psm_config_ext_2_s_type);

    if ( cmefs_read ( CMEFS_PSM_CONFIG_EXT2,
               (byte* )&psm_config_ext_2, &read_size) )
    {
      /* Only these fields are in version 0*/
      psm_ptr->psm_inactivity_cfg.psm_inactivity_timeout = psm_config_ext_2.psm_inactivity_timeout;
      psm_ptr->psm_inactivity_cfg.psm_duration_due_to_inactivity = 
                                                   psm_config_ext_2.psm_duration_due_to_inactivity;
      if(psm_config_ext_2.version<1)
      {
        /* These fields added with version 1*/
        psm_ptr->catm1_early_wakeup_time_poi = 2;
        psm_ptr->nb1_early_wakeup_time = 5;
        psm_ptr->nb1_early_wakeup_time_poi = 4;
        if(poi_profile_status == TRUE)
        {
          /*If version <1 and profile setting read is SUCCESS*/
          psm_ptr->poi_m1_enabled = param_value;
   	      psm_ptr->poi_nb1_enabled = param_value;
        }
        else
        {
          /*If version <1 and profile setting read is FAILURE*/
	        psm_ptr->poi_m1_enabled = TRUE;
	        psm_ptr->poi_nb1_enabled = TRUE;
        }
       }
#ifdef FEATURE_REF_TRACKER
      else if (psm_config_ext_2.version < 2)
      {
         psm_ptr->sns_early_wakeup_time = 0;
         psm_ptr->sns_awake_time =0;
         if(poi_profile_status == TRUE)
         {
           psm_ptr->poi_m1_enabled = psm_ptr->poi_m1_enabled & param_value;
           psm_ptr->poi_nb1_enabled = psm_ptr->poi_nb1_enabled & param_value;
         }
      }
#endif
      else
      {
        psm_ptr->catm1_early_wakeup_time_poi = psm_config_ext_2.catm1_early_wakeup_time_poi;
        psm_ptr->nb1_early_wakeup_time_poi = psm_config_ext_2.nb1_early_wakeup_time_poi;
        psm_ptr->nb1_early_wakeup_time = psm_config_ext_2.nb1_early_wakeup_time;
#ifdef FEATURE_REF_TRACKER
        psm_ptr->sns_early_wakeup_time = psm_config_ext_2.sns_early_wakeup_time;		
        psm_ptr->sns_awake_time = psm_config_ext_2.sns_awake_time;

#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
	     cmsns_set_force_motion_enable(psm_config_ext_2.force_motion_sns_ind);
#endif
        	
#endif
        if(psm_config_ext_2.poi_image_enabled & CM_PSM_POI_NB1_ENABLE)
        {
          psm_ptr->poi_nb1_enabled = TRUE;
        }
	if(psm_config_ext_2.poi_image_enabled & CM_PSM_POI_M1_ENABLE)
        {
          psm_ptr->poi_m1_enabled = TRUE;
        }
        if(poi_profile_status == TRUE)
        {
          psm_ptr->poi_m1_enabled = psm_ptr->poi_m1_enabled & param_value;
          psm_ptr->poi_nb1_enabled = psm_ptr->poi_nb1_enabled & param_value;
        }

        if (psm_config_ext_2.version == 3)
        {
          psm_ptr->psm_rtc_cal = psm_config_ext_2.psm_rtc_cal;
        }
     }
    }  
    else
    {
      /* Disable the timer for now ,default value was 10 */
      psm_ptr->psm_inactivity_cfg.psm_inactivity_timeout = 0;
      psm_ptr->psm_inactivity_cfg.psm_duration_due_to_inactivity = psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos;
      psm_ptr->catm1_early_wakeup_time_poi = 2;
      psm_ptr->nb1_early_wakeup_time = 5;
      psm_ptr->nb1_early_wakeup_time_poi = 4;
#ifdef FEATURE_REF_TRACKER
      psm_ptr->sns_early_wakeup_time = 0;
      psm_ptr->sns_awake_time = 0;
#endif
      if(poi_profile_status == TRUE)
      {
        /*If NV read fails and profile setting read is SUCCESS*/
        psm_ptr->poi_m1_enabled = param_value;
	      psm_ptr->poi_nb1_enabled = param_value;
      }
      else
      {
        /*If NV read fails and profile setting read is FAILURE*/
        psm_ptr->poi_m1_enabled = TRUE;
        psm_ptr->poi_nb1_enabled = TRUE;
      }
    }
#ifdef FEATURE_REF_TRACKER
    cmsns_set_boot_up_time(psm_ptr->sns_early_wakeup_time );
#ifndef FEATURE_MMODE_TX_21
    cmsns_set_smd_time_out_value((uint8)psm_ptr->sns_awake_time);
#endif

#endif
    //Reading PSM_UIM_OPT
    read_size = sizeof(uint8);
    if ( cmefs_read ( CMEFS_PSM_UIM_OPT,
               (byte* )&psm_ptr->psm_uim_opt_enabled, &read_size) )
    {
       CM_MSG_MED_1 ("cmpsm: : psm_uim_opt_enabled-%d ", psm_ptr->psm_uim_opt_enabled );
    }
    else
    {
      /* set default value to FALSE */
      psm_ptr->psm_uim_opt_enabled = 0;
    }

  /* Read the PSM config Ext*/
  read_size = sizeof (cm_psm_rtc_config_s_type);
  if ( cmefs_read ( CMEFS_PSM_RTC_CONFIG,
               (byte* )&psm_ptr->psm_rtc_info, &read_size) )
  {
     CM_MSG_MED_2 ("cmpsm: : rtc_enabled-%d, time-out:%d ", psm_ptr->psm_rtc_info.rtc_enabled,psm_ptr->psm_rtc_info.timer_info_timeout );
  }
  else
  {
    /* set default value to TRUE */
    psm_ptr->psm_rtc_info.rtc_enabled = TRUE;
    psm_ptr->psm_rtc_info.timer_info_timeout = 5;
  }
#if defined(FEATURE_BUILD_PAGEONLY_IMAGE)
/*  call only when Tiny apps is not available */
  if( !((psm_ptr->psm_cfg_info.tiny_image_config & CM_PSM_TINY_IMAGE_APP_ENABLE)!= 0) )
  {
#ifndef CM_DEBUG
    cm_psm_sio_uart_init(psm_ptr->uart_baud_rate);
#endif
  }
#endif
  /* last_psm_cfg_info with default values */
  psm_ptr->last_psm_cfg_info.psm_enabled                  = psm_ptr->psm_cfg_info.is_enabled;
  psm_ptr->last_psm_cfg_info.active_rat                   = SYS_RAT_NONE;
  psm_ptr->last_psm_cfg_info.active_timer_value           = CM_PSM_ACTIVE_TIMER_INVALID;
  psm_ptr->last_psm_cfg_info.periodic_update_timer_value  = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID;
  psm_ptr->last_psm_cfg_info.gprs_ready_timer_value       = CM_PSM_GPRS_READY_TIMER_INVALID;
  
}
/*===========================================================================
FUNCTION cmpsm_reset

DESCRIPTION
  Initialize the PSM related data structures.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_reset(void)
{
  cm_psm_info_s_type    *psm_ptr  =  cmpsm_ptr();
  /* get the pointer to the psm object to be initialized */

  cm_psm_module_e_type   psm_module;


  for(psm_module = CM_PSM_MODULE_BEGIN;psm_module <= CM_PSM_MODULE_END;psm_module++ )
  {
    psm_ptr->module_info[psm_module].psm_enter_prot_status = SYS_PSM_STATUS_NONE;
  }

  psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_IDLE;
  psm_ptr->psm_periodic_update_uptime = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID_64;

  //reset high activity timer
  (void) timer_clr(&cmpsm_high_activity_timer,T_SEC);
}
#if !defined(FEATURE_BUILD_PAGEONLY_IMAGE) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)
/*===========================================================================
FUNCTION cmpsm_any_nw_psm_supported

DESCRIPTION
  Check if Rel 12 PSM is supported from UE & network

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmpsm_any_nw_psm_supported(void)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if((psm_ptr->psm_cfg_info.psm_version & CM_PSM_VERSION_MASK_WITH_CONTEXT_RETENTION
       && psm_ptr->last_psm_cfg_info.active_timer_value != CM_PSM_ACTIVE_TIMER_INVALID)
      ||(psm_ptr->psm_cfg_info.psm_version & CM_PSM_VERSION_MASK_PSM_BTW_EDRX_CYCLE
         && cmph_ptr()->edrx_enabled == TRUE)
     )
  {
    return TRUE;
  }

  return FALSE;

}
/*===========================================================================
FUNCTION cmpsm_inactivity_timer_start

DESCRIPTION
 Start/Stop inactivity timer based on nw PSM support

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_inactivity_timer_start(void)
{
  boolean timer_active = (boolean)timer_get(&cmpsm_inactivity_timer,T_SEC);
  cm_psm_info_s_type *psm_ptr = cmpsm_ptr();
   /*Start inactivity timer even though PSM_with_nw_coordination is enabled, thus no need to check for nw_coord if REF_TRACKER is defined*/

  if(psm_ptr->psm_cfg_info.is_enabled == FALSE || psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_IDLE
    || psm_ptr->psm_inactivity_cfg.psm_inactivity_timeout == CM_PSM_MAX_UPTIME
    || psm_ptr->psm_inactivity_cfg.psm_inactivity_timeout == 0)
  {
    return;
  }

  if(!timer_active && psm_ptr->psm_full_boot_uptime != CM_PSM_MAX_UPTIME_64 && (time_get_ms_native()+psm_ptr->psm_inactivity_cfg.psm_inactivity_timeout*1000) < psm_ptr->psm_full_boot_uptime
   )
   {
     CM_MSG_HIGH_1("Starting inactivity timer in ref_tracker with timeout:%d",psm_ptr->psm_inactivity_cfg.psm_inactivity_timeout);
     timer_set(&cmpsm_inactivity_timer,psm_ptr->psm_inactivity_cfg.psm_inactivity_timeout, 0, T_SEC);
   }
}
#endif
/*===========================================================================
FUNCTION cmpsm_btw_edrx_active

DESCRIPTION
  Check if PSM btw eDRX is active

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmpsm_btw_edrx_active(void)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  boolean                     ack_needed = TRUE;

  #if !defined (FEATURE_BUILD_PAGEONLY_IMAGE) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)
  /*L-RRC needs to be queried only if Rel12 PSM is not active*/
  if(!(psm_ptr->psm_cfg_info.psm_version & CM_PSM_VERSION_MASK_PSM_BTW_EDRX_CYCLE)
    ||!cmph_ptr()->edrx_enabled  /* Indicates no network support for eDRX*/
    ||(psm_ptr->psm_cfg_info.psm_version & CM_PSM_VERSION_MASK_WITH_CONTEXT_RETENTION
       && psm_ptr->last_psm_cfg_info.active_timer_value != CM_PSM_ACTIVE_TIMER_INVALID)
        /* Indicates Rel12 PSM is not active*/
    )
  {
    ack_needed = FALSE;
  }

  CM_MSG_HIGH_3("psm_version:%x, active_timer:%u ack_needed:%d",psm_ptr->psm_cfg_info.psm_version,
                                                    psm_ptr->last_psm_cfg_info.active_timer_value,
                                                    ack_needed);
  
  #endif

  return ack_needed;
}

/*===========================================================================
FUNCTION cmpsm_send_psm_ind

DESCRIPTION
 Send PSM indication to clients through msgr.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_send_psm_ind( cm_psm_status_e_type psm_status, cm_psm_reject_reason_e_type reason)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  cm_psm_status_ind_s_type   psm_status_ind;
  uint64                      uptime = time_get_ms_native();  /* Get the current time */
#ifdef FEATURE_APPS_ONLY_BOOT
  msgr_init_hdr( (msgr_hdr_struct_type*)&psm_status_ind, MSGR_MM_CM, MM_CM_PSM_STATUS_IND );
  psm_status_ind.status = psm_status;
  psm_status_ind.reject_reason = reason;
  psm_status_ind.periodic_update_timer_value = (uint32)((psm_ptr->psm_periodic_update_uptime - uptime)/1000); 
  psm_status_ind.modem_only_timer_value = (uint32)((psm_ptr->psm_full_boot_uptime - uptime)/ 1000);

  if( psm_status_ind.periodic_update_timer_value  < psm_ptr->psm_cfg_info.threshold )
  {
     psm_status_ind.periodic_update_timer_value   = psm_ptr->psm_cfg_info.threshold ;
  }
  CM_MSG_HIGH_4 ( "MM_CM_PSM_STATUS_IND - status:%d - reason:%d, min all module time = %lu, modem only time = %lu",
                     psm_status, reason,  psm_status_ind.periodic_update_timer_value,psm_status_ind.modem_only_timer_value);

  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_4(cm_get_ulog_handle(), "=CM= MM_CM_PSM_STATUS_IND - status:%d - reason:%d,  all_time = %lu, modemonly = %d",
                             psm_status, reason,psm_status_ind.periodic_update_timer_value,psm_status_ind.modem_only_timer_value);
  }
  
  (void)msgr_send( (msgr_hdr_struct_type*)&psm_status_ind, sizeof(psm_status_ind) );

#else
  cm_psm_image_boot_type_ind_s_type psm_boot_type_ind;
  errno_enum_type msgr_error = E_SUCCESS;
  uint8                      mask = (uint8)ALL_PSM_BIT;
  uint8                      data = FULL_BOOT_BIT;
#ifdef FEATURE_BUILD_PAGEONLY_IMAGE
  pm_err_flag_type           pmic_err = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
#endif
  uint64         early_wakeup_time = (uint64)psm_ptr->psm_cfg_ext_info.early_wakeup_time * 1000;/* to convert in msec */
  uint64         psm_status_periodic_update_val = 0;
  
  msgr_init_hdr( (msgr_hdr_struct_type*)&psm_status_ind, MSGR_MM_CM, MM_CM_PSM_STATUS_IND );
  msgr_init_hdr( (msgr_hdr_struct_type*)&psm_boot_type_ind, MSGR_MM_CM, MM_CM_PSM_IMAGE_BOOT_TYPE_IND );
  
  psm_status_ind.status = psm_status;
  psm_status_ind.reject_reason = reason;
  psm_status_periodic_update_val = (psm_ptr->psm_periodic_update_uptime-uptime);
  
  psm_boot_type_ind.psm_boot_type = CM_PSM_IMAGE_BOOT_FULL;

  psm_status_ind.modem_only_timer_value = 0xFFFFFFFF;
  
  if(psm_status == CM_PSM_STATUS_COMPLETED)
  {
#ifdef FEATURE_REF_TRACKER 
#if defined(FEATURE_BUILD_PAGEONLY_IMAGE)	  
    if(psm_ptr->psm_entry_type == CM_PSM_TYPE_SHUTDOWN)      
     
#elif  defined(FEATURE_BUILD_SENSORONLY_IMAGE)
    if (psm_ptr->psm_entry_type == CM_PSM_TYPE_SHUTDOWN) 
       
#endif        
    {

    /* it check if sensor boot time is minimum  */


       if(
#if !defined(FEATURE_BUILD_PAGEONLY_IMAGE) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)   	
	   	  (psm_ptr->psm_periodic_update_uptime == psm_ptr->psm_sns_boot_time) &&
#endif
          (MIN(psm_ptr->psm_full_boot_uptime,psm_ptr->psm_page_only_boot_time)  >  /*awake time feed after power number   */
               (psm_ptr->psm_periodic_update_uptime + ((uint64)psm_ptr->psm_cfg_info.threshold*1000)+ ((uint64)psm_ptr->sns_awake_time * 1000)))

	   )

       {
 
         /* check if  full boot is sufficiently greater than 
             sns boot time els move to full boot*/

   	      {
            /* setting with respect to SNS image only  */ 
   		     data |= SNS_BOOT_BIT;
       
             /* Other bits matter only for tiny image*/
             if((psm_ptr->psm_cfg_info.tiny_image_config & CM_PSM_TINY_IMAGE_APP_ENABLE) !=0 )
             {
               data |= APPS_BIT;
             }   			 
			 early_wakeup_time = (uint64)((uint64)psm_ptr->sns_early_wakeup_time * 1000);
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifdef FEATURE_SERIAL_LOGGING_ENABLE
             CM_SERIAL_LOG_1("Going to SOI with time %ld \n\r",psm_ptr->psm_periodic_update_uptime);
#endif
#endif
   	      }

       }
	      
      else if((cmpsm_config_rat_is_active(psm_ptr->psm_rat) == TRUE) 
	  	
#if !defined(FEATURE_BUILD_PAGEONLY_IMAGE) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)

	  &&  (psm_ptr->psm_periodic_update_uptime == psm_ptr->psm_page_only_boot_time)
#endif

	    )
	   {
		  if (( MIN(psm_ptr->psm_full_boot_uptime,psm_ptr->psm_sns_boot_time) > 
                 (psm_ptr->psm_periodic_update_uptime + (uint64)((uint64)psm_ptr->edrx_ptw * 1000)+ (uint64)((uint64)psm_ptr->psm_cfg_info.threshold * 1000)))
                  && psm_ptr->is_next_full_image == FALSE)
         {
            /* Setting boot bit to TRUE since we want to wake up in page only mode*/
           data |= PAGE_BOOT_BIT;

           /* Other bits matter only for tiny image*/
           if((psm_ptr->psm_cfg_info.tiny_image_config & CM_PSM_TINY_IMAGE_APP_ENABLE) !=0 )
           {
             data |= APPS_BIT;
           }
      
           if(psm_ptr->psm_rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
           {
             early_wakeup_time =  (uint64)psm_ptr->catm1_early_wakeup_time_poi * 1000;
           }
           else
           {
             early_wakeup_time =  (uint64)psm_ptr->nb1_early_wakeup_time_poi * 1000;
           }
         }
         else if(psm_ptr->psm_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
         {
           early_wakeup_time = (uint64)psm_ptr->nb1_early_wakeup_time * 1000;
         }
	   }
	  else if(psm_ptr->psm_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
      {
        early_wakeup_time = (uint64)psm_ptr->nb1_early_wakeup_time*1000;
      }
    }
 #if defined(FEATURE_BUILD_PAGEONLY_IMAGE)|| defined(FEATURE_BUILD_SENSORONLY_IMAGE)	
    else if(psm_ptr->psm_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
    {
      early_wakeup_time = (uint64)psm_ptr->nb1_early_wakeup_time*1000;
    }
#endif

#else
   
    if(
#ifdef FEATURE_BUILD_PAGEONLY_IMAGE
        (psm_ptr->psm_entry_type == CM_PSM_TYPE_SHUTDOWN)
        && 
#endif        
        (cmpsm_config_rat_is_active(psm_ptr->psm_rat) == TRUE)
        &&
       (psm_ptr->psm_full_boot_uptime > 
         psm_ptr->psm_periodic_update_uptime + (uint64)((uint64)psm_ptr->edrx_ptw * 1000)+ (uint64)((uint64)psm_ptr->psm_cfg_info.threshold * 1000))
         && psm_ptr->is_next_full_image == FALSE)
    {
      /* Setting boot bit to TRUE since we want to wake up in page only mode*/
      data |= PAGE_BOOT_BIT;

      /* Other bits matter only for tiny image*/
      if((psm_ptr->psm_cfg_info.tiny_image_config & CM_PSM_TINY_IMAGE_APP_ENABLE) !=0 )
      {
        data |= APPS_BIT;
      }
      
      if(psm_ptr->psm_rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
      {
        early_wakeup_time = (uint64)psm_ptr->catm1_early_wakeup_time_poi * 1000;
      }
      else
      {
        early_wakeup_time = (uint64)psm_ptr->nb1_early_wakeup_time_poi * 1000;
      }
	  psm_boot_type_ind.psm_boot_type = CM_PSM_IMAGE_BOOT_PAGE;
    }
    else if(psm_ptr->psm_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
    {
      early_wakeup_time = (uint64)psm_ptr->nb1_early_wakeup_time * 1000;
    }


#endif
    
#if defined(FEATURE_BUILD_PAGEONLY_IMAGE)|| defined(FEATURE_BUILD_SENSORONLY_IMAGE)	
   if( psm_ptr->psm_full_boot_uptime > uptime && 
       psm_ptr->psm_full_boot_uptime < psm_ptr->psm_periodic_update_uptime && data == 0)
   {
       psm_status_periodic_update_val = (psm_ptr->psm_full_boot_uptime-uptime);
   }
#endif
	
#ifdef FEATURE_REF_TRACKER 
/*  if apps in tiny image is not enable and device download is enable, in that 
     case set the bit in register*/
    if(((psm_ptr->psm_cfg_info.tiny_image_config & CM_PSM_TINY_IMAGE_APP_ENABLE) == 0 &&
	    (psm_ptr->psm_cfg_info.tiny_image_config & CM_PSM_DEV_DWNLD_ENABLE) != 0))
    {	 
      data |= DWNLD_BIT;
    } 
#else
	if(((psm_ptr->psm_cfg_info.tiny_image_config & CM_PSM_TINY_IMAGE_APP_ENABLE) == 0 &&
	   (psm_ptr->psm_cfg_info.tiny_image_config & CM_PSM_DEV_DWNLD_ENABLE) != 0) &&
       (data & PAGE_BOOT_BIT) != 0 )
    {	 
      data |= DWNLD_BIT;
    } 

#endif

	/*This will configure the boot register with boot option */
    pm_app_misc_set_cfg( PM_APP_MISC_CFG_BOOT_OPTION, mask, data);
#if defined(FEATURE_BUILD_PAGEONLY_IMAGE)|| defined(FEATURE_BUILD_SENSORONLY_IMAGE)	
    if(psm_ptr->psm_entry_type == CM_PSM_TYPE_SHUTDOWN)
#endif 
   {
	 /*  This will configure the PSM register here */
	 pm_app_misc_set_cfg( PM_APP_MISC_CFG_PSM,0x01,0x01);
   }


    cmpsm_wait_for_cond(ALL_PSM_BIT,data);

    if(( psm_ptr->psm_enter_uptime > uptime &&
        psm_ptr->psm_enter_uptime < psm_ptr->psm_periodic_update_uptime)
        || (psm_ptr->psm_context_retained == FALSE
#ifdef FEATURE_REF_TRACKER 
          &&(psm_ptr->psm_enter_uptime > uptime &&
		     psm_ptr->psm_enter_uptime < psm_ptr->psm_sns_boot_time )
#endif		
		)
        )
    {
      /* If apps timer is shorter than modem's, apps timer to be used for PSM*/
        CM_MSG_HIGH_3 ( "PSM complete- enter_time:%lu, modem_time:%lu, uptime=%lu",
        psm_ptr->psm_enter_uptime, psm_ptr->psm_periodic_update_uptime, uptime);
        psm_status_periodic_update_val = (psm_ptr->psm_enter_uptime -uptime);
    }

    /* Adjusting early wake-up time*/

    if(psm_status_periodic_update_val < (uint64)((uint64)psm_ptr->psm_cfg_info.threshold * 1000))
    {
      /*In complete indication modem timer should never be less than threshold
          ** As RIL can reject it, in case if it falls short set == threshold*/
      psm_status_periodic_update_val  = (uint64)psm_ptr->psm_cfg_info.threshold * 1000;
    }
    
	if(psm_status_periodic_update_val <= early_wakeup_time)
	{
	   psm_status_periodic_update_val = early_wakeup_time;
	}
	else 
	{
	   psm_status_periodic_update_val = psm_status_periodic_update_val - early_wakeup_time;
	}


#if defined(FEATURE_BUILD_PAGEONLY_IMAGE)|| defined(FEATURE_BUILD_SENSORONLY_IMAGE)	
    if(psm_ptr->psm_entry_type == CM_PSM_TYPE_SHUTDOWN)
#endif 
    {

#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifdef FEATURE_SERIAL_LOGGING_ENABLE
      CM_SERIAL_LOG_1("CMPSM RTC programmed value 0x %x \n\r",(uint32)psm_status_periodic_update_val);
#endif
#endif
      if(psm_ptr->psm_rtc_cal == TRUE)
      {
        /* Caliberation API for CAL_RC */
        pm_clk_sleep_cal_en(CHIP_CODE,PM_CLK_CALRC3,TRUE);
      }
	/* This function will configure the RTC clock for PSM duration */
	  cmpsm_configure_rtc_clk(psm_status_periodic_update_val);
	}

  }

  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_4(cm_get_ulog_handle(), "=CM= MM_CM_PSM_STATUS_IND - status:%d - reason:%d,  modem_timer = %lu, PMIC register = %d",
                                                      psm_status, reason, psm_status_periodic_update_val,data);
  }
  
  CM_MSG_HIGH_4 ( "MM_CM_PSM_STATUS_IND - status:%d - reason:%d, modem_timer = %lu, data:%d",
    psm_status, reason,  psm_status_periodic_update_val,data);

#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  CM_SERIAL_LOG_1(" =CM= MM_CM_PSM_STATUS_IND : uptime =%d \n\r ",(uint32) time_get_ms_native());
  CM_SERIAL_LOG_3("=CM= CMPSM psm_status=%d reason =%d data = %d \n\r",psm_status,reason,data);
#endif

  psm_status_ind.periodic_update_timer_value = (uint32)(psm_status_periodic_update_val/1000); /* Convert to sec for app */ 

  #if defined (FEATURE_BUILD_PAGEONLY_IMAGE) || defined(FEATURE_BUILD_SENSORONLY_IMAGE)

  if((psm_ptr->psm_cfg_info.tiny_image_config & CM_PSM_TINY_IMAGE_APP_ENABLE) != 0)
  {
    cm_psm_reject_reason_e_type psm_shutdown_type;
    
    if(psm_ptr->psm_entry_type == CM_PSM_TYPE_RESET)
    {
      /* RIL will use this cause to trigger reset*/
      psm_shutdown_type = CM_PSM_REJECT_REASON_PSM_DURATION_CHANGED;
    }
    else
    {
      /* This for shutdown*/
      psm_shutdown_type = CM_PSM_REJECT_REASON_DURATION_TOO_SHORT;
    }
    
    qmi_send_psm_status_ind
    (
     CM_PSM_STATUS_COMPLETED,  
     psm_shutdown_type,
     psm_status_ind.periodic_update_timer_value 
    );
    #ifdef CM_DEBUG
    #error code not present
#endif
  }
  else
  {
    sys_m_reset_config_type reset_cfg = {0};
#ifdef FEATURE_BUILD_PAGEONLY_IMAGE
    pmic_err = pm_pon_trigger_enable(PRIMARY_DEVICE,PM_PON_TRIGGER_PON1,PON_DISABLE);
    CM_MSG_HIGH_1 ( "pm_pon_trigger_enable - status:%d",pmic_err);
	if(cm_get_ulog_handle() != NULL)
    {
      ULOG_RT_PRINTF_1(cm_get_ulog_handle(), "=CM= pm_pon_trigger_enable - status:%d ",pmic_err);
    }
#endif

    reset_cfg.reset_type = (sys_m_reset_type)psm_ptr->psm_entry_type;
    sys_m_reset(&reset_cfg);
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
    CM_SERIAL_LOG_1("=CM= CMPSM RESET call with entry type %d \n\r",psm_ptr->psm_entry_type);
#endif
  }
  #else
  {
	msgr_error = msgr_send( (msgr_hdr_struct_type*)&psm_boot_type_ind, sizeof(psm_boot_type_ind) );
	if(cm_get_ulog_handle() != NULL)
    {
      ULOG_RT_PRINTF_2(cm_get_ulog_handle(), " =CM= the image boot %d and error = %d ",psm_boot_type_ind.psm_boot_type,msgr_error);
    }

    (void)msgr_send( (msgr_hdr_struct_type*)&psm_status_ind, sizeof(psm_status_ind) );
 
	
  }
  #endif /* Tiny image end flag   */
#endif /* Apps only boot flag */
}
/*===========================================================================
FUNCTION cmpsm_check_and_proceed_with_psm_mode

DESCRIPTION
write cumulative context from all the clients to EFS

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
static void cmpsm_write_psm_context( void)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  uint32                       write_size = 0, write_committed =0;          /* Total context size*/
  uint8                      *total_context,*current_block;
  cm_psm_module_e_type        psm_module = CM_PSM_MODULE_BEGIN;
  uint32                      file_size=0;

  for(psm_module = CM_PSM_MODULE_BEGIN;psm_module <=CM_PSM_MODULE_END; psm_module++)
  {
    write_size  = write_size + psm_ptr->module_info[psm_module].context_size+ (uint32)4;
  }
    
  /* Append CM context as well*/
#ifdef FEATURE_REF_TRACKER
  /* total size(uint32), psm_full_boot_uptime(uint64), psm_page_only_boot_time(uint64), psm_sns_only_boot_time(uint64), 
  sys_radio_access_tech_e_type, psm_status_periodic_update_val_dbg (unit64), edrx_ptw (unit8)*/
  write_size = write_size +sizeof(uint32)+sizeof(uint64)+sizeof(uint64)+ sizeof(uint64)+sizeof(sys_radio_access_tech_e_type)+sizeof(uint64) +sizeof(uint8);
#else  
  /*total size(uint32), psm_full_boot_uptime(uint64), sys_radio_access_tech_e_type,uart_baud_rate(uint16), psm_status_periodic_update_val_dbg (unit64), edrx_ptw(uint8)*/
  write_size = write_size +sizeof(uint32)+sizeof(uint64)+sizeof(sys_radio_access_tech_e_type)+sizeof(uint16)+sizeof(uint64)+sizeof(uint8);
#endif



  total_context = (uint8*)cm_mem_malloc(write_size);
        
  current_block = total_context;

  /*First 4 bytes of context be total context size since it is longer fitting in single EFS*/
  memscpy(current_block,sizeof(uint32),&write_size,sizeof(uint32));
  current_block = (uint8*)(current_block + sizeof(uint32));

  /* PSM full boot time in next context */
  memscpy(current_block,sizeof(uint64),&psm_ptr->psm_full_boot_uptime,sizeof(uint64));
  current_block = (uint8*)(current_block + sizeof(uint64));
#ifdef FEATURE_REF_TRACKER  
  /* page only boot time  */
  memscpy(current_block,sizeof(uint64),&psm_ptr->psm_page_only_boot_time,sizeof(uint64));
  current_block = (uint8*)(current_block + sizeof(uint64));
  /* sensor only boot time */
  memscpy(current_block,sizeof(uint64),&psm_ptr->psm_sns_boot_time,sizeof(uint64));
  current_block = (uint8*)(current_block + sizeof(uint64));
#endif  
  memscpy(current_block,sizeof(sys_radio_access_tech_e_type),&psm_ptr->psm_rat,sizeof(sys_radio_access_tech_e_type));
  current_block = (uint8*)(current_block + sizeof(sys_radio_access_tech_e_type));
  
#ifndef FEATURE_REF_TRACKER

  memscpy(current_block,sizeof(uint16),&psm_ptr->uart_baud_rate,sizeof(uint16));
  current_block = (uint8*)(current_block + sizeof(uint16));
#endif

  psm_ptr->psm_status_periodic_update_val_dbg = psm_ptr->psm_periodic_update_uptime - time_get_ms_native();

  memscpy(current_block,sizeof(uint64),&psm_ptr->psm_status_periodic_update_val_dbg,sizeof(uint64));
  current_block = (uint8*)(current_block + sizeof(uint64));

  memscpy(current_block,sizeof(uint8),&psm_ptr->edrx_ptw,sizeof(uint8));
  current_block = (uint8*)(current_block + sizeof(uint8));

  for(psm_module = CM_PSM_MODULE_BEGIN;psm_module <=CM_PSM_MODULE_END; psm_module++)
  {
    memscpy(current_block,sizeof(uint32),&psm_ptr->module_info[psm_module].context_size,sizeof(uint32));
    current_block = (uint8*)(current_block + sizeof(uint32));
          
    if(psm_ptr->module_info[psm_module].context_size != 0)
    {
      memscpy(current_block,psm_ptr->module_info[psm_module].context_size,
               psm_ptr->module_info[psm_module].context,psm_ptr->module_info[psm_module].context_size);
      current_block = (uint8*)(current_block + psm_ptr->module_info[psm_module].context_size);
            
      cm_mem_free(psm_ptr->module_info[psm_module].context);
      psm_ptr->module_info[psm_module].context = NULL;
    }
    
  }    

        
  if(psm_ptr->psm_context_changed)
  {
     write_committed = cmefs_write( CMEFS_PSM_CONTEXT, (byte* )total_context, (int32)write_size);
     psm_ptr->psm_context_retained = (write_committed>0);
     if(write_committed < write_size)
     {
       uint32 extWrite = 0;
       current_block = (uint8*)(total_context + write_committed);
       extWrite = write_size - write_committed;
       extWrite = cmefs_write( CMEFS_PSM_CONTEXT_EXT, (byte* )current_block, extWrite);
       if(write_committed + extWrite < write_size)
       {
         CM_ERR_FATAL_1("Unable to write context %d",write_size);
       }
        
     }
  }
  else
  {
     psm_ptr->psm_context_retained = TRUE;
  }
    
    cm_mem_free(total_context);
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  CM_SERIAL_LOG_0("=CM= PSM write done \n\r");
#endif
}

#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA)) && !defined(FEATURE_BUILD_PAGEONLY_IMAGE) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)
/*===========================================================================
FUNCTION cmpsm_send_psm_ready_req_to_reg

DESCRIPTION
  Send PSM Ready request to the NAS.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmpsm_send_psm_ready_req_to_reg(void)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  reg_cmd_type               *reg_ptr;

  /* Get reg cmd. buffer and populate params.*/
  reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_PSM_READY_REQ );

  /* Fill in parameters, deliberately using ready_req as it is union. */
  reg_ptr->cmd.cm_psm_ready_req.as_id = SYS_MODEM_AS_ID_1;
  CM_MSG_HIGH_0("Send CM_PSM_READY_REQ to NAS");

  /* Send the request to NAS */
  cm_reg_send_cmd( reg_ptr );
  return TRUE;
}

/*===========================================================================
FUNCTION cmpsm_send_psm_enter_req_to_reg

DESCRIPTION
  Send PSM enter request to the NAS.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmpsm_send_psm_enter_req_to_reg(void)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  reg_cmd_type               *reg_ptr;

  /* Get reg cmd. buffer and populate params.*/
  reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_ENTER_PSM_MODE_REQ );

  CM_MSG_HIGH_0("CM_ENTER_PSM_MODE_REQ to NAS");

  /* Send the request to NAS */
  cm_reg_send_cmd( reg_ptr );
  return TRUE;
}


/*===========================================================================
FUNCTION cmpsm_start_psm_enter

DESCRIPTION
 All modules are ready to enter PSM. This function starts the PSM enter procedure.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_start_context_save( void )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  cm_psm_module_e_type		  psm_module = CM_PSM_MODULE_BEGIN;

  if(((psm_ptr->psm_cfg_info.psm_version & CM_PSM_VERSION_MASK_WITH_CONTEXT_RETENTION
      && psm_ptr->last_psm_cfg_info.active_timer_value != CM_PSM_ACTIVE_TIMER_INVALID)
    || (psm_ptr->psm_cfg_info.psm_version & CM_PSM_VERSION_MASK_PSM_BTW_EDRX_CYCLE
      && cmpsm_btw_edrx_active())) && (psm_ptr->psm_due_to_oos_allowed == FALSE)
    )
  {
    reg_cmd_type               *reg_ptr;
    
    /* Waiting for MMGSDI write in NAS*/
    reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_PSM_SAVE_MMGSDI_EF_REQ );
    cm_reg_send_cmd( reg_ptr );
    psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_WAIT_MMGSDI_RSP;
  }
  else
  {
    CM_MSG_HIGH_2("active_timer:%d, PSM due to OOS:%d",
            psm_ptr->last_psm_cfg_info.active_timer_value,psm_ptr->psm_due_to_oos_allowed);

    //Sending PSM
    (void) cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS, SYS_OPRT_MODE_PSM);
    //Mark psm enter request state to enter in progress
	psm_ptr->psm_context_changed = TRUE;
	for(psm_module = CM_PSM_MODULE_BEGIN;psm_module <=CM_PSM_MODULE_END; psm_module++)
    {
      psm_ptr->module_info[psm_module].context_size = 0;
    }
    cmpsm_write_psm_context();
    psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_ENTER_IN_PROGRESS;
#ifdef FEATURE_REF_TRACKER
	cmsnsmgr_process_lpm_oos();
#endif
  }
}
#endif

/*===========================================================================
  FUNCTION cmpsm_start_psm_enter

DESCRIPTION
 All modules are ready to enter PSM. This function starts the PSM enter procedure.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_start_psm_enter( void )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  cm_psm_module_e_type        psm_module;
  
#if !defined(FEATURE_BUILD_PAGEONLY_IMAGE) && !defined (FEATURE_BUILD_SENSORONLY_IMAGE)
  /*GPIO Toggling for power measurment breakdown context save start*/
  MCPM_GPIO_TOGGLE(RCM_TRIGGER_1)
#endif

  for(psm_module = CM_PSM_MODULE_BEGIN;psm_module <= CM_PSM_MODULE_END;psm_module++ )
  {
    if(psm_ptr->module_info[psm_module].psm_enter_req_func_ptr)
    {
      psm_ptr->module_info[psm_module].psm_enter_req_func_ptr();
      CM_MSG_HIGH_1("Sending enter req to %d",psm_module);
    }
    else
    {
      /* Consider module done if it doesn't register call-back for enter req*/
      psm_ptr->module_info[psm_module].psm_enter_prot_status = SYS_PSM_STATUS_DONE;
    }
  }
  psm_ptr->psm_context_changed  = FALSE;
  psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_WAIT_ENTER_RSP;
}

/*===========================================================================
FUNCTION cmpsm_check_psm_prot_status

DESCRIPTION
 Checks if all stakeholders are ready to enter PSM

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
boolean cmpsm_check_psm_prot_status( sys_psm_status_e_type psm_status )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  cm_psm_module_e_type        psm_module;

  for(psm_module = CM_PSM_MODULE_BEGIN;psm_module <= CM_PSM_MODULE_END;psm_module++ )
  {
    if(psm_ptr->module_info[psm_module].psm_enter_prot_status != psm_status)
    {
      return FALSE;
    }
  }
  
  return TRUE;
}
/*===========================================================================

FUNCTION cmpsm_check_not_ready_psm_prot_status

DESCRIPTION
    Checks if all stakeholders are ready to enter PSM/psm_status


DEPENDENCIES
   None

RETURN VALUE
  TRUE  - if all modules are ready to enter PSM/ psm_status 
  
SIDE EFFECTS
  none

===========================================================================*/
boolean  cmpsm_check_not_ready_psm_prot_status( sys_psm_status_e_type psm_status )
{
  cm_psm_info_s_type    *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  cm_psm_module_e_type   psm_module;
  boolean                status = FALSE;

  for(psm_module = CM_PSM_MODULE_BEGIN;psm_module <= CM_PSM_MODULE_END;psm_module++ )
  {
    if(psm_ptr->module_info[psm_module].psm_enter_prot_status == psm_status)
    {
      status = TRUE;
    }
    else if(psm_ptr->module_info[psm_module].psm_enter_prot_status != SYS_PSM_STATUS_READY)
    {
      status = FALSE;
      break;
    }
  }
  return status;
}
/*===========================================================================

FUNCTION cmpsm_configure_rtc_clk

DESCRIPTION
    Configure RTC alarm for PSM time

DEPENDENCIES
   None

RETURN VALUE
  None
  
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_configure_rtc_clk(uint64      psm_time)
{
  cm_psm_info_s_type    *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  pm_rtc_time_type      rtc_time={0};

  rtc_time.sec = (uint32)(psm_time/1000);
  rtc_time.msec = (uint32)(psm_time%1000);

  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_2(cm_get_ulog_handle(), "=CM= cmpsm_configure_rtc_clk  modem_timer_sec = %lu modem_timer_msec= %lu",
                                                      rtc_time.sec, rtc_time.msec);
  }

  (void)pmapp_rtc_alarm_set_time(&rtc_time);
}
#if !defined (FEATURE_BUILD_PAGEONLY_IMAGE) && ! defined(FEATURE_BUILD_SENSORONLY_IMAGE)
/*===========================================================================
FUNCTION cmpsm_handle_deep_sleep

DESCRIPTION
 All modules are ready to enter PSM but PSM duration less than threshold. 
 Handle deep-sleep by entering or exiting

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_handle_deep_sleep( boolean is_enter )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  uint64                       uptime  = time_get_ms_native();  /* Get the current time */
  uint64              deep_sleep_time  = 0;
  
  deep_sleep_time = MIN(psm_ptr->psm_enter_uptime,psm_ptr->psm_periodic_update_uptime) - uptime;
  
  if((is_enter == TRUE) &&
     (deep_sleep_time <= CM_PSM_DEEP_SLEEP_EARLY_WAKEUP_TIME *1000))
  {
	CM_MSG_HIGH_1("PSM deep sleep time %lu secs, less than equal threshold",deep_sleep_time);
    return;	
  }

  mmoc_cmd_suspend_ss_new(is_enter, FALSE, PROT_DEACT_DEEP_SLEEP);

  if(is_enter)
  {
  
    cm_psm_deep_sleep_enter_ind_s_type         *psm_cmd 
         = (cm_psm_deep_sleep_enter_ind_s_type*)cm_mem_malloc(sizeof(cm_psm_deep_sleep_enter_ind_s_type));

    deep_sleep_time = deep_sleep_time - CM_PSM_DEEP_SLEEP_EARLY_WAKEUP_TIME*1000;

    /*Set the timer now*/
    timer_set(&cmpsm_deep_sleep_timer,(uint32) (deep_sleep_time/1000), 0, T_SEC );
    
    psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_DEEP_SLEEP;
    CM_MSG_HIGH_1 ("Enter PSM deep sleep for %lu milisecs",deep_sleep_time);

    
    if(psm_cmd != NULL)
    {
      memset(psm_cmd, 0, sizeof(cm_psm_deep_sleep_enter_ind_s_type));
      cm_msgr_send(MM_CM_PSM_DEEP_SLEEP_ENTER_IND,MSGR_MM_CM_PSM,
                    (msgr_hdr_s*)psm_cmd,sizeof(cm_psm_deep_sleep_enter_ind_s_type));
    }
    
    cm_mem_free(psm_cmd);
  }
  else
  {
    /* Exiting deep sleep should move psm_state back to IDLE*/
    psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_IDLE;

    /*Clear the timer */
    (void) timer_clr(&cmpsm_deep_sleep_timer,T_SEC);
  }
}
#else
/*===========================================================================

FUNCTION cmpsm_process_auto_ready_ext_ind

DESCRIPTION
    Handle auto ready ext for page only 

DEPENDENCIES
   None

RETURN VALUE
  None
  
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_auto_ready_ext_ind(const void* cmd_ptr)
{  

  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  cm_client_psm_ready_ind_ext_s_type*  psm_auto_ready_ext_ptr = &(((cm_psm_u_type*) cmd_ptr)->cm_client_psm_auto_ready_ext_ind);
  uint64                       uptime =  time_get_ms_native();  /* Get the current time */
  
  psm_ptr->psm_entry_type = psm_auto_ready_ext_ptr->psm_entry_type;
#ifdef FEATURE_BUILD_PAGEONLY_IMAGE 
  if(psm_ptr->psm_entry_type == CM_PSM_TYPE_SHUTDOWN)
  {
    if(psm_auto_ready_ext_ptr->activate_timer == CM_PSM_MAX_UPTIME || psm_auto_ready_ext_ptr->active_timer_msec ==  CM_PSM_MAX_UPTIME )
    {
      CM_MSG_HIGH_2("the activate_timer is = %lu and active_timer is = %lu which is invalid ",psm_auto_ready_ext_ptr->activate_timer,psm_auto_ready_ext_ptr->active_timer_msec);
      return ;
    }
	if(psm_auto_ready_ext_ptr->activate_timer == 0 && psm_auto_ready_ext_ptr->active_timer_msec == 0 )
    {
      CM_MSG_HIGH_0("the auto_ready_ext_ind duration is 0");
	  return ;
    }
			 
  }
#endif
  /* convert time to msec */
  psm_ptr->psm_enter_uptime = ((uint64)psm_auto_ready_ext_ptr->activate_timer * 1000 + 
                              psm_auto_ready_ext_ptr->active_timer_msec+ uptime);
							  
  
  psm_ptr->psm_periodic_update_uptime = psm_ptr->psm_enter_uptime;
  
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  CM_SERIAL_LOG_2("=CM= cmpsm auto ready ext enter uptime = %d, fullboot time =%d  \n\r",(uint32)psm_ptr->psm_enter_uptime, (uint32)psm_ptr->psm_full_boot_uptime);
#endif
  
#ifdef FEATURE_BUILD_PAGEONLY_IMAGE 
  if( (psm_ptr->psm_entry_type == CM_PSM_TYPE_SHUTDOWN) &&
      ((psm_ptr->psm_enter_uptime  - uptime) <= (uint64)((uint64)psm_ptr->psm_cfg_info.threshold * 1000)))
  {
	CM_MSG_HIGH_1("ignoring the auto ready ext ind duration %lu ",(psm_ptr->psm_enter_uptime - uptime));
	return;
  }	
#endif
#ifdef FEATURE_BUILD_PAGEONLY_IMAGE 
      if( psm_ptr->psm_entry_type == CM_PSM_TYPE_RESET)
      {
           CM_MSG_HIGH_0("Updating psm full boot uptime in RESET");
           psm_ptr->psm_full_boot_uptime = uptime;
      }
#endif

  cmpsm_start_psm_enter();
}
#endif

/*===========================================================================

FUNCTION cm_client_psm_enter_reg

DESCRIPTION
  registration function for client to register callback. To enter PSM and save context, CM would send this request to Client

DEPENDENCIES
  none

RETURN VALUE
  results of the registration

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_client_psm_enter_reg(

cm_client_psm_enter_req_f_type  * ntfy_cb_func,
    /*  pointer to Client callback function */

cm_psm_module_e_type              psm_module
)
{
  cm_psm_info_s_type    *psm_ptr  =  cmpsm_ptr();

  if(!INRANGE (psm_module, CM_PSM_MODULE_BEGIN, CM_PSM_MODULE_END))
  {
    return FALSE;
  }

  psm_ptr->module_info[psm_module].psm_enter_req_func_ptr = ntfy_cb_func;
  CM_MSG_HIGH_1("PSM enter cb func registered by %d",psm_module);

  return TRUE;
}


/*===========================================================================
FUNCTION cmpsm_check_and_proceed_with_psm_mode

DESCRIPTION
 1. In case if any one of the Module rejected before and ready now then 
     send READY_IND to clients.
 2. If all the protocals responded with READY then post LPM, once LPM is 
    complete then COMPLETE_IND to clients.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_check_and_proceed_with_psm_mode( void)
{
  if(cmpsm_check_psm_prot_status(SYS_PSM_STATUS_DONE))
  {
    cmpsm_write_psm_context();

#if !defined(FEATURE_BUILD_PAGEONLY_IMAGE)  && !defined(FEATURE_BUILD_SENSORONLY_IMAGE) 
    /*GPIO Toggling for power measurment breakdown context save end*/
    MCPM_GPIO_TOGGLE(RCM_TRIGGER_1)
#endif

    cmpsm_ptr()->psm_enter_req_status = CM_PSM_ENTER_STATUS_ENTER_IN_PROGRESS;

    #if defined (FEATURE_BUILD_PAGEONLY_IMAGE) || defined(FEATURE_BUILD_SENSORONLY_IMAGE)
    /* Temporarily till all core dependencies are resolved*/
    cmpsm_send_psm_ind( CM_PSM_STATUS_COMPLETED, CM_PSM_REJECT_REASON_PSM_NO_ERROR);
    #else
    cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS, SYS_OPRT_MODE_PSM);
    #endif
  }
}

#if  !defined (FEATURE_BUILD_PAGEONLY_IMAGE) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)
/*===========================================================================
FUNCTION cmpsm_check_all_prot_status_and_proceed

DESCRIPTION
 1. In case if any one of the Module rejected before and ready now then 
     send READY_IND to clients.
 2. If all the protocals responded with READY then move to psm enter in progress

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_check_all_prot_status_and_proceed( void)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  uint64                       uptime   =  time_get_ms_native();  /* Get the current time */
  uint64                   periodic_update_timer_value = 0;
  uint64                       psm_enter_duration = 0;
  
  if(psm_ptr->psm_periodic_update_uptime > uptime)
  {
    periodic_update_timer_value = psm_ptr->psm_periodic_update_uptime - uptime;
  }

  if(psm_ptr->psm_enter_uptime > uptime)
  {
    psm_enter_duration = psm_ptr->psm_enter_uptime - uptime;
  }
  #ifndef  FEATURE_BUILD_PAGEONLY_IMAGE
  if(psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_DEEP_SLEEP
     && cmpsm_check_not_ready_psm_prot_status(SYS_PSM_STATUS_ENTER_DEEP_SLEEP))
  {
    cmpsm_handle_deep_sleep(TRUE);
  }
  else 
  #endif
  if(cmpsm_check_psm_prot_status(SYS_PSM_STATUS_READY))
  {
    //if we entered deep sleep due to client before then send READY_IND to clients
    //In case if any one of the Module rejected before and ready now then send READY_IND to clients.
    if(psm_ptr->psm_enter_req_status == CM_PSM_ENTER_STATUS_WAIT_READY_IND ||
       psm_ptr->psm_enter_req_status == CM_PSM_ENTER_STATUS_DEEP_SLEEP)
    {
      /*Starting PSM entry now*/
      cmpsm_reset();
      
      /* Fresh PSM now, start with timer in ready ind if nw supported PSM, else inactivity duration*/
      if(cmpsm_any_nw_psm_supported())
      {
        /* Starting with nw given timer*/
        psm_enter_duration = periodic_update_timer_value;
      }
      else if(psm_ptr->psm_due_to_oos_allowed)
      {
        /* PSM due to OOS, so start with OOS duration*/
        psm_enter_duration = (uint64)psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos * 1000;
      }
      else
      {
        /* Only other case is inactivity*/
        psm_enter_duration = (uint64)psm_ptr->psm_inactivity_cfg.psm_duration_due_to_inactivity * 1000;
      }
     
      cmpsm_process_psm_enter_req(psm_enter_duration);
    }
    else if(psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_ENTER_IN_PROGRESS)
    {
      /* PSM duration should not be greater than next modem update timer 
      ** as it could miss the update procedure with network*/
          
      if(psm_enter_duration > (uint64)((uint64)psm_ptr->psm_cfg_info.threshold * 1000)
         && periodic_update_timer_value >= (uint64)((uint64)psm_ptr->psm_cfg_info.threshold * 1000))
      {
       /* CM going to start PSM context save now*/
       boolean timer_active = (boolean)timer_get(&cmpsm_high_activity_timer,T_SEC);
       if(!timer_active)
       {
          cmpsm_start_context_save();      
       }	  
       else
       {
          CM_MSG_HIGH_0("Not initiating PSM since high activity timer is running");
       }
      }
      /* if psm duration is less than the PSM mode threshold */
      else
      {
        CM_MSG_HIGH_4 ( "TOO_SHORT, threshold:%d, Remaining(Tm:%lu, modem_time:%lu), psm_opt_mask:%u ",
            psm_ptr->psm_cfg_info.threshold, psm_enter_duration, periodic_update_timer_value,psm_ptr->psm_cfg_ext_info.psm_opt_mask);

        cmpsm_reset();
        #ifndef FEATURE_BUILD_PAGEONLY_IMAGE
        if(psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_DEEP_SLEEP)
        {
          if(periodic_update_timer_value != 0)
          {
            psm_ptr->psm_periodic_update_uptime = periodic_update_timer_value + uptime;
          }
           
          /* Enter deep-sleep only if rejecting due to threshold*/
          cmpsm_handle_deep_sleep(TRUE);
        }
        #endif
      }
    }
  }
}

/*===========================================================================

FUNCTION cm_client_psm_ready_reg

DESCRIPTION
  registration function for Client to register callback. To check PSM ready CM would send this request to Client

DEPENDENCIES
  none

RETURN VALUE
  results of the registration

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_client_psm_ready_reg(

cm_client_psm_ready_req_f_type  * ntfy_cb_func,

cm_psm_module_e_type              psm_module


    /*  pointer to DS callback function */
)
{
  cm_psm_info_s_type    *psm_ptr  =  cmpsm_ptr();

  if(!INRANGE (psm_module, CM_PSM_MODULE_BEGIN, CM_PSM_MODULE_END))
  {
    return FALSE;
  }

  psm_ptr->module_info[psm_module].psm_ready_req_func_ptr = ntfy_cb_func;
  CM_MSG_HIGH_1("PSM ready cb func registered by %d",psm_module);

  return TRUE;
}
/*===========================================================================
FUNCTION cmpsm_send_psm_ready_req

DESCRIPTION
  Send PSM ready request to modules.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_send_psm_ready_req(void)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  cm_psm_module_e_type        psm_module;

  for(psm_module = CM_PSM_MODULE_BEGIN;psm_module <= CM_PSM_MODULE_END;psm_module++ )
  {
    if(psm_ptr->module_info[psm_module].psm_ready_req_func_ptr
      && (psm_module != CM_PSM_MODULE_LRRC || cmpsm_btw_edrx_active()))
    {
      psm_ptr->module_info[psm_module].psm_ready_req_func_ptr();
    }
    else
    {
      /* Consider module ready if it doesn't register call-back for ready req*/
      psm_ptr->module_info[psm_module].psm_enter_prot_status = SYS_PSM_STATUS_READY;
    }
  }
}

/*===========================================================================
FUNCTION cmpsm_process_ready_rsp

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_ready_rsp( cm_psm_module_e_type module, const void   *rpt_ptr )

{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  sys_psm_status_e_type     psm_status = SYS_PSM_STATUS_NONE;
  uint64  periodic_update_timer_value = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID_64;
  uint64                    uptime = time_get_ms_native();  /* Get the current time */
  cmss_s_type          *ss_ptr        =  cmss_ptr();

  if((psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_WAIT_READY_RSP
    && psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_WAIT_READY_IND)
    ||!INRANGE (module, CM_PSM_MODULE_BEGIN, CM_PSM_MODULE_END))
  {
    CM_MSG_HIGH_1("Ignoring READY_RSP from Module:%d", module);
    return;
  }

  switch( module)
  {
    case CM_PSM_MODULE_NAS:
    {
        /* Pointer to LL report message.  */
        cm_rpt_type           *cm_rpt_ptr        = (cm_rpt_type *) rpt_ptr;

        psm_status = cm_rpt_ptr->cmd.psm_ready_rsp.status;
        periodic_update_timer_value = (uint64)cm_rpt_ptr->cmd.psm_ready_rsp.periodic_update_timer_value * 1000;
    }
    break;
   
    default:
    {
      cm_psm_u_type         *psm_rpt = (cm_psm_u_type*) rpt_ptr;

      psm_status = psm_rpt->cm_client_psm_ready_rsp.status;
	  periodic_update_timer_value = (uint64)psm_rpt->cm_client_psm_ready_rsp.activate_timer * 1000 + 
		                          psm_rpt->cm_client_psm_ready_rsp.active_timer_msec;
      //Update only when activate_timer is valid  
      if(module == CM_PSM_MODULE_QMI && psm_rpt->cm_client_psm_ready_rsp.activate_timer != 0xFFFFFFFF)
      {
        //update the uptime with apps_time, this would need special handling if PSM due to OOS
        if(((uint64)psm_rpt->cm_client_psm_ready_ind.activate_timer * 1000)+uptime >= uptime)
        {
          psm_ptr->psm_enter_uptime = uptime + (uint64)psm_rpt->cm_client_psm_ready_rsp.activate_timer * 1000;
        }
      }
      if(module == CM_PSM_MODULE_LRRC)
      {
          psm_ptr->is_next_full_image = psm_rpt->cm_client_psm_ready_rsp.is_next_full_image;
      }    
   
    }
    break;
  }

  CM_MSG_HIGH_4("Ready RSP, status:%d, module:%d, update_timer:%lu is_next_full_image:%d",psm_status, module, periodic_update_timer_value, psm_ptr->is_next_full_image);

  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_3(cm_get_ulog_handle(), " =CM= Ready RSP, status:%d, module:%d, update_timer:%lu",psm_status, module, periodic_update_timer_value);
  }

  switch( psm_status)
  {
    case SYS_PSM_STATUS_NOT_READY_ACTIVE_TIMER_RUNNING:
    case SYS_PSM_STATUS_NOT_READY_NOT_IDLE:
    case SYS_PSM_STATUS_NOT_READY_EMC_CALL:
    {
      psm_ptr->module_info[module].psm_enter_prot_status = psm_status;
      psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_WAIT_READY_IND; 
    }
    break;

   case SYS_PSM_STATUS_ENTER_DEEP_SLEEP:
    {
      psm_ptr->module_info[module].psm_enter_prot_status = psm_status;
      psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_WAIT_READY_IND;
      cmpsm_check_all_prot_status_and_proceed();
    }
    break;

    case SYS_PSM_STATUS_NOT_READY_ACTIVE_OOS:
    {
      if(psm_ptr->psm_due_to_oos_allowed == FALSE)
      {
        psm_ptr->module_info[module].psm_enter_prot_status = psm_status;
        psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_WAIT_READY_IND;
        break;
      }
    }
  /*break is intentionally removed to execute SYS_PSM_STATUS_READY case as well*/

    case SYS_PSM_STATUS_READY:
    {
      /*Periodic update timer value �0� indicates that NW doesn�t wants UE to do periodic TAU at all*/
      if( periodic_update_timer_value != 0 && ((uptime+periodic_update_timer_value) > uptime))
      /*(uptime+periodic_update_timer_value) > uptime check will take care of overflow */
      {
        if(psm_ptr->psm_periodic_update_uptime > (uptime+periodic_update_timer_value))
        {
          psm_ptr->psm_periodic_update_uptime = uptime+periodic_update_timer_value;
        }

#ifdef FEATURE_APPS_ONLY_BOOT
        /* psm_periodic_update_uptime for mintime all contain QMI
           psm_full_boot_uptime for mintime excluding QMI*/
        if(module != CM_PSM_MODULE_QMI  &&
          (psm_ptr->psm_full_boot_uptime > (uptime+periodic_update_timer_value)) )
        {
           psm_ptr->psm_full_boot_uptime = uptime + periodic_update_timer_value;
        }
#else

        if(module != CM_PSM_MODULE_LRRC &&
#ifdef FEATURE_REF_TRACKER
           module != CM_PSM_MODULE_SNS_MGR &&
#endif
          (psm_ptr->psm_full_boot_uptime > (uptime+periodic_update_timer_value)))
        {
          psm_ptr->psm_full_boot_uptime = uptime + periodic_update_timer_value;
        }
#ifdef FEATURE_REF_TRACKER
	    if( module == CM_PSM_MODULE_LRRC )
	    {
          psm_ptr->psm_page_only_boot_time = uptime + periodic_update_timer_value;
	    }
	    else if( module == CM_PSM_MODULE_SNS_MGR )
	    {
          psm_ptr->psm_sns_boot_time = uptime + periodic_update_timer_value;
	    }
#endif
#endif
      }

     /*If poi is disabled in release13 PSM, update full_boot_uptime to be periodic_update_timer_value*/
      if(cmpsm_btw_edrx_active() && ss_ptr->info.sys_mode == SYS_SYS_MODE_LTE &&
        ((ss_ptr->info.ciot_lte_mode == SYS_SYS_MODE_LTE_M1 && psm_ptr->poi_m1_enabled == FALSE) ||
        (ss_ptr->info.ciot_lte_mode == SYS_SYS_MODE_LTE_NB1 && psm_ptr->poi_nb1_enabled == FALSE)))
        {
             psm_ptr->psm_full_boot_uptime = psm_ptr->psm_periodic_update_uptime;
             psm_ptr->psm_page_only_boot_time = CM_PSM_MAX_UPTIME_64;
        }
      psm_ptr->module_info[module].psm_enter_prot_status = SYS_PSM_STATUS_READY;
      cmpsm_check_all_prot_status_and_proceed();
    }
    break;
  default:
    break;
  }
}

/*===========================================================================
FUNCTION cmpsm_process_ready_ind

DESCRIPTION
  Process READY_IND from NAS/MC/HDR. Move to LPM if all the modules are 
  ready to enter PSM.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_ready_ind( cm_psm_module_e_type module, const void *rpt_ptr )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  uint64   periodic_update_timer_value = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID_64;
  uint64                         uptime = time_get_ms_native();  /* Get the current time */


  if((psm_ptr->psm_enter_req_status  != CM_PSM_ENTER_STATUS_WAIT_READY_IND
    && psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_DEEP_SLEEP)
    || !INRANGE (module, CM_PSM_MODULE_BEGIN, CM_PSM_MODULE_END))
  {
    CM_MSG_HIGH_1("Ignoring READY_IND from REG, module:%d",module);
    return;
  }
  psm_ptr->module_info[module].psm_enter_prot_status = SYS_PSM_STATUS_READY;

  if(module == CM_PSM_MODULE_NAS)
  {
     cm_rpt_type           *cm_rpt_ptr        = (cm_rpt_type *) rpt_ptr;

    periodic_update_timer_value = (uint64)cm_rpt_ptr->cmd.psm_ready_ind.periodic_update_timer_value*1000;
  }
  else if(module == CM_PSM_MODULE_DS || module == CM_PSM_MODULE_LRRC || module == CM_PSM_MODULE_UIM     ||   module == CM_PSM_MODULE_SNS_MGR)
  {
    cm_psm_u_type         *psm_rpt_ptr = (cm_psm_u_type*) rpt_ptr;
    periodic_update_timer_value = (uint64)psm_rpt_ptr->cm_client_psm_ready_ind.activate_timer * 1000 + 
		                          psm_rpt_ptr->cm_client_psm_ready_ind.active_timer_msec;
  }

  CM_MSG_HIGH_2("Ready IND,  module:%d, update_timer:%lu", module, periodic_update_timer_value);
  
  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_2(cm_get_ulog_handle(), " =CM= Ready IND,  module:%d, update_timer:%lu", module, periodic_update_timer_value);
  }
  

  /*Periodic update timer value �0� indicates that NW doesn�t wants UE to do periodic TAU at all*/

  if((periodic_update_timer_value != 0) && ((uptime+periodic_update_timer_value) > uptime))
  /*(uptime+periodic_update_timer_value) > uptime check will take care of overflow */
  {
    psm_ptr->psm_periodic_update_uptime = uptime+periodic_update_timer_value;
  }

  cmpsm_check_all_prot_status_and_proceed();

}

/*===========================================================================
FUNCTION cmpsm_process_psm_enter_req

DESCRIPTION
  processing PSM_ENTER_REQ and send psm enter request to all the protocals/modules.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpsm_process_psm_enter_req(uint64 psm_time)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  uint64                      clk_time  = time_get_ms_native();
  cmph_s_type                *ph_ptr   = NULL;



#ifndef FEATURE_BUILD_PAGEONLY_IMAGE

  #ifndef FEATURE_FMD_SPEED_INFO
  ph_ptr   = cmph_ptr();
  //Send completed indication if already in PSM/LPM
  if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
  {
    cmpsm_send_psm_ind(CM_PSM_STATUS_COMPLETED, CM_PSM_REJECT_REASON_PSM_NO_ERROR);
    CM_MSG_HIGH_1 ( "CM_PSM_STATUS_COMPLETED - curr_oprt_mode:%d", ph_ptr->oprt_mode);
    psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_COMPLETE;
#ifdef FEATURE_REF_TRACKER
	cmsnsmgr_process_lpm_oos();
#endif
    return;
  }
  #endif

  if(( psm_ptr->last_psm_cfg_info.psm_enabled == TRUE ) && ( psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_QUICK ))
  {
      CM_MSG_HIGH_0 ( "Quick PSM is enabled so sending LPM rightaway, no need to check with NAS/MC/HDR" );
     //Sending PSM
     (void) cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS, SYS_OPRT_MODE_PSM);
     //Mark psm enter request state to lpm in progress
      psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_ENTER_IN_PROGRESS;
     return;
  }
#endif

  /* Reset if any stale data present*/
  cmpsm_reset();

  //update the uptime with apps_time
  psm_ptr->psm_enter_uptime = clk_time + psm_time;

  /* If PSMD comes with larger_apps_time and if psm_enter_uptime overflows after adding uptime(clk_time+apps_time), set it to MAX*/
  if(psm_ptr->psm_enter_uptime <= clk_time )
  {
    psm_ptr->psm_enter_uptime = CM_PSM_MAX_UPTIME_64;
  }

  cmpsm_send_psm_ready_req();

  if(cmpsm_check_psm_prot_status(SYS_PSM_STATUS_READY))
  {
    psm_ptr->psm_periodic_update_uptime = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID_64;
  }
  else
  {
    CM_MSG_HIGH_0("Waiting for Ready rsp");
    psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_WAIT_READY_RSP;
  }

  cmpsm_check_all_prot_status_and_proceed();

}

/*===========================================================================
FUNCTION cmpsm_process_auto_ready_ind

DESCRIPTION
  This function should be called to process the PSM reports from NAS like
  get_timer_info_rsp.


DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
void cmpsm_process_auto_ready_ind(cm_psm_module_e_type module, const void *rpt_ptr)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  uint64                     periodic_update_timer_value = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID_64;
  uint64                      clk_time  = time_get_ms_native();

  //Reseting the fullboot time as we get auto ready indication.
  #if !defined (FEATURE_BUILD_PAGEONLY_IMAGE)  && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)  
 	psm_ptr->psm_full_boot_uptime = CM_PSM_MAX_UPTIME_64;
#endif


  if(module == CM_PSM_MODULE_LRRC )
  {
    cm_psm_u_type         *psm_rpt_ptr = (cm_psm_u_type*) rpt_ptr;
    boolean timer_active = (boolean)timer_get(&cmpsm_inactivity_timer,T_SEC);
    
    if(timer_active)
    { 
        CM_MSG_HIGH_0 ( "Ignoring AUTO_READY from LRRC due to inactivity timer running");
        return;
    }

    if(cmpsm_btw_edrx_active())
    {
      periodic_update_timer_value = (uint64)((uint64)psm_rpt_ptr->cm_client_psm_auto_ready_ind.activate_timer * 1000 + 
	                                psm_rpt_ptr->cm_client_psm_auto_ready_ind.active_timer_msec );
    }
    else
    {
      /* LRRC auto ready needs to be ignored if edrx psm is not active*/
      return;
    }
  }
  else if(module == CM_PSM_MODULE_NAS)
  {
    cm_rpt_type           *cm_rpt_ptr        = (cm_rpt_type *) rpt_ptr;
    periodic_update_timer_value = (uint64)cm_rpt_ptr->cmd.cm_psm_auto_ready_ind.periodic_update_timer_value * 1000;
  }
#ifdef FEATURE_REF_TRACKER
  else if (module == CM_PSM_MODULE_SNS_MGR)
  {
    cm_psm_u_type         *psm_rpt_ptr = (cm_psm_u_type*) rpt_ptr;
	periodic_update_timer_value = (uint64)((uint64)psm_rpt_ptr->cm_client_psm_auto_ready_ind.activate_timer*1000 + 
                                  psm_rpt_ptr->cm_client_psm_auto_ready_ind.active_timer_msec);
  }
#endif

  CM_MSG_HIGH_4 ( "AUTO_READY received from module:%d timer:%u, curr uptime:%lu fullboot from NV:%lu ", 
                                  module,periodic_update_timer_value, clk_time,psm_ptr->psm_full_boot_uptime);

  
  CM_MSG_HIGH_6 ("cmpsm: psm conf(version=%d, enabled=%d, threshold=%d, psm_version=%d) conf_ext(opt_mask = %u), enter_req_status%d",
                 psm_ptr->psm_cfg_info.version, psm_ptr->psm_cfg_info.is_enabled, psm_ptr->psm_cfg_info.threshold,
                 psm_ptr->psm_cfg_info.psm_version, psm_ptr->psm_cfg_ext_info.psm_opt_mask, psm_ptr->psm_enter_req_status );

  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_1(cm_get_ulog_handle(), "=CM= Received PSM enter req, PSM status: %d",
                                                       psm_ptr->psm_enter_req_status);
  }

  //Reject if PSM feature is disabled
  if( (!psm_ptr->last_psm_cfg_info.psm_enabled && !psm_ptr->psm_due_to_oos_allowed) || 
       (psm_ptr->psm_cfg_info.psm_version == CM_PSM_VERSION_MASK_NONE ))
  {
    return;
  }

  //Ignore if already in process of PSM LPM
  if( psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_IDLE 
      && psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_WAIT_READY_IND
      && psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_DEEP_SLEEP)
  {
    return;
  }

  cmpsm_process_psm_enter_req(periodic_update_timer_value);
  
}
#endif
/*===========================================================================
FUNCTION cmpsm_process_enter_rsp

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_enter_rsp( cm_psm_module_e_type module, const void  *rpt_ptr )

{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  boolean                    context_saved = TRUE;

  if(psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_WAIT_ENTER_RSP
   || !INRANGE (module, CM_PSM_MODULE_BEGIN, CM_PSM_MODULE_END))
  {
    CM_MSG_HIGH_1("Ignoring ENTER_RSP from Module:%d", module);
    return;
  }

  switch( module)
  {
    case CM_PSM_MODULE_NAS:
    {
      /* Pointer to LL report message.  */
      cm_rpt_type           *cm_rpt_ptr          = (cm_rpt_type *) rpt_ptr;
      if(cm_rpt_ptr->cmd.cm_enter_psm_mode_rsp.is_context_changed)
      {
        psm_ptr->psm_context_changed = TRUE;
      }
      psm_ptr->module_info[module].context = cm_rpt_ptr->cmd.cm_enter_psm_mode_rsp.context;
      psm_ptr->module_info[module].context_size = cm_rpt_ptr->cmd.cm_enter_psm_mode_rsp.context_size;
    }
    break;
   
    default:
    {
      cm_psm_u_type         *psm_rpt_ptr = (cm_psm_u_type*) rpt_ptr;
      
      if(psm_rpt_ptr->cm_client_psm_enter_rsp.is_context_changed)
      {
        psm_ptr->psm_context_changed = TRUE;
      }
      psm_ptr->module_info[module].context = psm_rpt_ptr->cm_client_psm_enter_rsp.context;
      psm_ptr->module_info[module].context_size = psm_rpt_ptr->cm_client_psm_enter_rsp.context_size;
    }
    break;
  }

  CM_MSG_HIGH_2("Enter_RSP: module:%d, context_size:%d",module,psm_ptr->module_info[module].context_size);

  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_2(cm_get_ulog_handle(), " =CM= Enter_RSP:module:%d, context_size:%d", \
        module,psm_ptr->module_info[module].context_size);
  }

  {
    psm_ptr->module_info[module].psm_enter_prot_status = SYS_PSM_STATUS_DONE;
  }

  cmpsm_check_and_proceed_with_psm_mode();

}

#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA)) &&!defined(FEATURE_BUILD_PAGEONLY_IMAGE) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)
/*===========================================================================
FUNCTION cmpsm_send_get_timer_info_req

DESCRIPTION
  Send PSM enter request to the NAS.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_send_get_timer_info_req(boolean user_config)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  reg_cmd_type               *reg_ptr;

  /* Get reg cmd. buffer and populate params.*/
  reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_PSM_GET_TIMER_INFO_REQ );

  /* Fill in parameters. */
  reg_ptr->cmd.cm_psm_get_timer_info_req.as_id = SYS_MODEM_AS_ID_1;
  reg_ptr->cmd.cm_psm_get_timer_info_req.user_config = user_config;
  CM_MSG_HIGH_2("as_id=%d, Send CM_PSM_GET_TIMER_INFO_REQ to NAS user_config=%d", reg_ptr->cmd.cm_psm_ready_req.as_id, user_config );

  /* Send the request to NAS */
  cm_reg_send_cmd( reg_ptr );
  //psm_ptr->nas_get_config_resp_pending = TRUE;

}
/*===========================================================================
FUNCTION cmpsm_process_get_cfg_params_req

DESCRIPTION
  processing get_cfg_params_req and send get_cfg_params_rsp.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpsm_process_get_cfg_params_req(boolean user_config)
{
#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
  cmpsm_send_get_timer_info_req(user_config);
#else
  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  errno_enum_type msgr_error;
  cm_psm_get_cfg_params_rsp_s_type resp_msg;

  msgr_init_hdr( (msgr_hdr_struct_type*)&resp_msg, MSGR_MM_CM, MM_CM_PSM_GET_CFG_PARAMS_RSP );

  resp_msg.enabled    = psm_ptr->last_psm_cfg_info.psm_enabled;
  resp_msg.threshold = psm_ptr->psm_cfg_info.threshold;

  resp_msg.psm_duration_due_to_oos = psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos;
  resp_msg.psm_randomization_window = psm_ptr->psm_cfg_ext_info.psm_randomization_window;

  resp_msg.active_timer_value = CM_PSM_ACTIVE_TIMER_INVALID;
  resp_msg.periodic_tau_timer_value    = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID; 

  resp_msg.periodic_rau_timer_value    = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID;
  resp_msg.gprs_ready_timer_value      = CM_PSM_GPRS_READY_TIMER_INVALID;

  msgr_error = msgr_send( (msgr_hdr_struct_type*)&resp_msg, sizeof(resp_msg) );
  CM_MSG_HIGH_1 ( "MM_CM_PSM_GET_CFG_PARAMS_RSP, msgr_error=%d", msgr_error );
#endif

}
/*===========================================================================
FUNCTION cmpsm_process_set_cfg_params_req

DESCRIPTION
  processing set_cfg_params_req.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpsm_process_set_cfg_params_req( cm_psm_u_type    *cm_cmd_ptr)
{
  cm_psm_info_s_type                                 *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  cm_mm_psm_set_cfg_params_req_s_type  *set_cfg_params_req_ptr = &cm_cmd_ptr->cm_mm_psm_set_cfg_params_req;
  uint16         change_mask = CM_PSM_CONFIG_INFO_MASK_NONE;
    
  CM_MSG_HIGH_6(" Send CM_PSM_SET_CONFIG_PARAMS_REQ, change_mask=%d, enabled=%d,active_timer=%u,periodic_tau_timer=%u,periodic_rau_timer_value=%u,gprs_ready_timer_value=%u",
                               set_cfg_params_req_ptr->change_mask,  set_cfg_params_req_ptr->psm_enabled,
                               set_cfg_params_req_ptr->active_timer_value,  set_cfg_params_req_ptr->periodic_tau_timer_value,
                                set_cfg_params_req_ptr->periodic_rau_timer_value, set_cfg_params_req_ptr->gprs_ready_timer_value);

  if(set_cfg_params_req_ptr->change_mask != 0 )
  {
      if(psm_ptr->psm_cfg_info.is_enabled == TRUE && 
            cm_cmd_ptr->cm_mm_psm_set_cfg_params_req.psm_enabled == FALSE &&
            psm_ptr->psm_enter_req_status == CM_PSM_ENTER_STATUS_DEEP_SLEEP) 
    {
      cmpsm_process_deep_sleep_timer_expiry();
    } 
    if( ( set_cfg_params_req_ptr->change_mask & CM_PSM_CONFIG_INFO_MASK_PSM_ENABLED ) && 
         ( psm_ptr->psm_cfg_info.is_enabled != cm_cmd_ptr->cm_mm_psm_set_cfg_params_req.psm_enabled ))
    {

      psm_ptr->last_psm_cfg_info.psm_enabled    = cm_cmd_ptr->cm_mm_psm_set_cfg_params_req.psm_enabled;
      change_mask |= CM_PSM_CONFIG_INFO_MASK_PSM_ENABLED;

      //Update the persistent cfg info
      psm_ptr->psm_cfg_info.is_enabled = cm_cmd_ptr->cm_mm_psm_set_cfg_params_req.psm_enabled;
      //Write into NV
      if( !cmefs_write(CMEFS_PSM_CONFIG, (byte*)&psm_ptr->psm_cfg_info, sizeof(psm_ptr->psm_cfg_info)) )
      {
         CM_MSG_HIGH_0 ( "write to CMEFS_PSM_CONFIG is failed");
      }
    }
    
    #if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
    {
      reg_cmd_type               *reg_ptr;

      /* Get reg cmd. buffer and populate params.*/
      reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_PSM_SET_CONFIG_PARAMS_REQ );
      /* Fill in parameters. */
      reg_ptr->cmd.cm_psm_set_cfg_params_req.as_id                      = SYS_MODEM_AS_ID_1;
      reg_ptr->cmd.cm_psm_set_cfg_params_req.psm_enabled          = set_cfg_params_req_ptr->psm_enabled;
      reg_ptr->cmd.cm_psm_set_cfg_params_req.active_timer_value  = set_cfg_params_req_ptr->active_timer_value;
      reg_ptr->cmd.cm_psm_set_cfg_params_req.periodic_tau_timer_value = set_cfg_params_req_ptr->periodic_tau_timer_value;
      reg_ptr->cmd.cm_psm_set_cfg_params_req.periodic_rau_timer_value = set_cfg_params_req_ptr->periodic_rau_timer_value;
      reg_ptr->cmd.cm_psm_set_cfg_params_req.gprs_ready_timer_value   = set_cfg_params_req_ptr->gprs_ready_timer_value;
      reg_ptr->cmd.cm_psm_set_cfg_params_req.change_mask           = set_cfg_params_req_ptr->change_mask;

       /* Send the request to NAS */
      cm_reg_send_cmd( reg_ptr );
    }
    #endif
     
    //Send MM_CM_PSM_CFG_CHANGE_IND so that other clients also will get to know of these new settings
      {
        cm_mm_psm_cfg_change_ind_s_type psm_cfg_change_ind = {0};
          errno_enum_type            msgr_error;

          msgr_init_hdr( (msgr_hdr_struct_type*)&psm_cfg_change_ind, MSGR_MM_CM, MM_CM_PSM_CFG_CHANGE_IND );

        if(change_mask & CM_PSM_CONFIG_INFO_MASK_PSM_ENABLED )
        {
          psm_cfg_change_ind.psm_enabled                             = set_cfg_params_req_ptr->psm_enabled;
        }

        psm_cfg_change_ind.change_mask                             = change_mask;
        psm_cfg_change_ind.active_rat   = psm_ptr->last_psm_cfg_info.active_rat;

        if(change_mask  != CM_PSM_CONFIG_INFO_MASK_NONE)
        {
          msgr_error = msgr_send( (msgr_hdr_struct_type*)&psm_cfg_change_ind, sizeof(psm_cfg_change_ind) );
          CM_MSG_HIGH_1( "MM_CM_PSM_CFG_CHANGE_IND - change_mask = %d " ,change_mask );
        }

      }
  }
}

/*===========================================================================
FUNCTION cmpsm_process_config_change_ind

DESCRIPTION
  This function should be called to process the PSM reports from NAS like
  get_timer_info_rsp.


DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
void cmpsm_process_config_change_ind(cm_rpt_type *cm_rpt_ptr)
{
  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  errno_enum_type msgr_error;
  cm_mm_psm_cfg_change_ind_s_type psm_cfg_change_ind;
  uint16         change_mask  = CM_PSM_CONFIG_INFO_MASK_NONE;

  msgr_init_hdr( (msgr_hdr_struct_type*)&psm_cfg_change_ind, MSGR_MM_CM, MM_CM_PSM_CFG_CHANGE_IND );

  CM_MSG_HIGH_2("CM_PSM_TIMER_CHANGE_IND - active_timer = %u, active_rat = %d ",
                     cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value,cm_rpt_ptr->cmd.psm_timer_change_ind.active_rat);

  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_2(cm_get_ulog_handle(), " =CM= CM_PSM_TIMER_CHANGE_IND - active_timer = %u, active_rat = %d ",
		               cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value,cm_rpt_ptr->cmd.psm_timer_change_ind.active_rat);
  }

  if( !( psm_ptr->psm_cfg_info.psm_version & CM_PSM_VERSION_MASK_WO_NW_COORDINATION 
       || psm_ptr->psm_cfg_info.psm_version & CM_PSM_VERSION_MASK_PSM_BTW_EDRX_CYCLE)  )
  {
      //NW doesn't support Rel12
      if ( cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value == CM_PSM_ACTIVE_TIMER_INVALID )
      {
        if( psm_ptr->last_psm_cfg_info.psm_enabled == TRUE )
        {
          psm_cfg_change_ind.psm_enabled                    = FALSE;
          change_mask |= CM_PSM_CONFIG_INFO_MASK_PSM_ENABLED;
        
          /* This is temp transition state due to NW not supporting Rel12, 
               Update only local cfg info and not required to update persistent cfg info( so not required to NV)  */
          psm_ptr->last_psm_cfg_info.psm_enabled = FALSE;
        }
      }
      else
      {
          /* Rel12 is supported hence fall back to persistent/NV*/
          if( psm_ptr->last_psm_cfg_info.psm_enabled != psm_ptr->psm_cfg_info.is_enabled )
          {
            psm_cfg_change_ind.psm_enabled                     = psm_ptr->psm_cfg_info.is_enabled;
            change_mask |= CM_PSM_CONFIG_INFO_MASK_PSM_ENABLED;
            psm_ptr->last_psm_cfg_info.psm_enabled = psm_ptr->psm_cfg_info.is_enabled;
          }
      }
	  psm_cfg_change_ind.psm_enabled                     = psm_ptr->psm_cfg_info.is_enabled;
	  change_mask |= CM_PSM_CONFIG_INFO_MASK_PSM_ENABLED;
  }

  if(cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value != CM_PSM_ACTIVE_TIMER_INVALID)
  {
    psm_ptr->psm_rat = cm_rpt_ptr->cmd.psm_timer_change_ind.active_rat;
  }
  else
  {
    psm_ptr->psm_rat = SYS_RAT_NONE;
  }
  if(psm_ptr->last_psm_cfg_info.active_rat != cm_rpt_ptr->cmd.psm_timer_change_ind.active_rat)
  {
    psm_cfg_change_ind.periodic_update_timer_value         = cm_rpt_ptr->cmd.psm_timer_change_ind.periodic_update_timer_value;
    psm_ptr->last_psm_cfg_info.periodic_update_timer_value    = cm_rpt_ptr->cmd.psm_timer_change_ind.periodic_update_timer_value;

    psm_cfg_change_ind.active_timer_value           = cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value;
    psm_ptr->last_psm_cfg_info.active_timer_value   = cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value;

    if(cm_rpt_ptr->cmd.psm_timer_change_ind.active_rat == SYS_RAT_GSM_RADIO_ACCESS)
    {
      psm_cfg_change_ind.gprs_ready_timer_value            = cm_rpt_ptr->cmd.psm_timer_change_ind.gprs_ready_timer_value;
      psm_ptr->last_psm_cfg_info.gprs_ready_timer_value    = cm_rpt_ptr->cmd.psm_timer_change_ind.gprs_ready_timer_value;
      change_mask |= (CM_PSM_CONFIG_INFO_MASK_PERIODIC_RAU_TIMER | CM_PSM_CONFIG_INFO_MASK_ACTIVE_TIMER | CM_PSM_CONFIG_INFO_MASK_GPRS_READY_TIMER);
    }
    else
    {
      change_mask |= (CM_PSM_CONFIG_INFO_MASK_PERIODIC_UPDATE_TIMER | CM_PSM_CONFIG_INFO_MASK_ACTIVE_TIMER);
    }

  }
  else if(cm_rpt_ptr->cmd.psm_timer_change_ind.active_rat == SYS_RAT_GSM_RADIO_ACCESS)
  {
    if ( (psm_ptr->last_psm_cfg_info.periodic_update_timer_value != cm_rpt_ptr->cmd.psm_timer_change_ind.periodic_update_timer_value ) )
    {
       psm_cfg_change_ind.periodic_update_timer_value       = cm_rpt_ptr->cmd.psm_timer_change_ind.periodic_update_timer_value;
         psm_ptr->last_psm_cfg_info.periodic_update_timer_value    = cm_rpt_ptr->cmd.psm_timer_change_ind.periodic_update_timer_value;
         change_mask |= CM_PSM_CONFIG_INFO_MASK_PERIODIC_RAU_TIMER;
    }
  
    if (  (psm_ptr->last_psm_cfg_info.active_timer_value != cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value ) )
    {
       psm_cfg_change_ind.active_timer_value                = cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value;
       psm_ptr->last_psm_cfg_info.active_timer_value        = cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value;
       change_mask |= CM_PSM_CONFIG_INFO_MASK_ACTIVE_TIMER;
    }
  
    if (psm_ptr->last_psm_cfg_info.gprs_ready_timer_value != cm_rpt_ptr->cmd.psm_timer_change_ind.gprs_ready_timer_value)
    {
       psm_cfg_change_ind.gprs_ready_timer_value            = cm_rpt_ptr->cmd.psm_timer_change_ind.gprs_ready_timer_value;
       psm_ptr->last_psm_cfg_info.gprs_ready_timer_value    = cm_rpt_ptr->cmd.psm_timer_change_ind.gprs_ready_timer_value;
       change_mask |= CM_PSM_CONFIG_INFO_MASK_GPRS_READY_TIMER;
    }
  }
  else
  {
    if ( (psm_ptr->last_psm_cfg_info.periodic_update_timer_value != cm_rpt_ptr->cmd.psm_timer_change_ind.periodic_update_timer_value ) )
    {
       psm_cfg_change_ind.periodic_update_timer_value         = cm_rpt_ptr->cmd.psm_timer_change_ind.periodic_update_timer_value;
       psm_ptr->last_psm_cfg_info.periodic_update_timer_value    = cm_rpt_ptr->cmd.psm_timer_change_ind.periodic_update_timer_value;
       change_mask |= CM_PSM_CONFIG_INFO_MASK_PERIODIC_UPDATE_TIMER;
    }
    
    if (  (psm_ptr->last_psm_cfg_info.active_timer_value != cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value ) )
    {
       psm_cfg_change_ind.active_timer_value                = cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value;
       psm_ptr->last_psm_cfg_info.active_timer_value        = cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value;
       change_mask |= CM_PSM_CONFIG_INFO_MASK_ACTIVE_TIMER;
    }
  }

  psm_ptr->last_psm_cfg_info.active_rat = cm_rpt_ptr->cmd.psm_timer_change_ind.active_rat;
  
  psm_cfg_change_ind.active_rat   = cm_rpt_ptr->cmd.psm_timer_change_ind.active_rat;
  psm_cfg_change_ind.change_mask                       = change_mask;

  if(change_mask  != CM_PSM_CONFIG_INFO_MASK_NONE)
  {
    msgr_error = msgr_send( (msgr_hdr_struct_type*)&psm_cfg_change_ind, sizeof(psm_cfg_change_ind) );
    CM_MSG_HIGH_1( "MM_CM_PSM_CFG_CHANGE_IND - change_mask = %d",change_mask);
  }

  cmpsm_inactivity_timer_start();
}


/*===========================================================================
FUNCTION cmpsm_process_get_cfg_params_rsp

DESCRIPTION
  This function should be called to process the PSM reports from NAS like
  get_timer_info_rsp.


DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
void cmpsm_process_get_cfg_params_rsp(cm_rpt_type *cm_rpt_ptr)
{
  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  errno_enum_type msgr_error;
  cm_psm_get_cfg_params_rsp_s_type resp_msg;

  msgr_init_hdr( (msgr_hdr_struct_type*)&resp_msg, MSGR_MM_CM, MM_CM_PSM_GET_CFG_PARAMS_RSP );

  resp_msg.enabled    = psm_ptr->psm_cfg_info.is_enabled;

  //if psm_verion mask is having only Rel12 and net work doesn't support Rel12 then send psm enabled as false
  if(  psm_ptr->psm_cfg_info.is_enabled && 
       ( psm_ptr->psm_cfg_info.psm_version != CM_PSM_VERSION_MASK_WO_NW_COORDINATION 
        && psm_ptr->psm_cfg_info.psm_version != CM_PSM_VERSION_MASK_PSM_BTW_EDRX_CYCLE ) && 
       ( cm_rpt_ptr->cmd.psm_timer_change_ind.active_timer_value == CM_PSM_ACTIVE_TIMER_INVALID )
    )
  {
    resp_msg.enabled    = FALSE;
    psm_ptr->last_psm_cfg_info.psm_enabled = FALSE;
  }
  
  resp_msg.threshold = psm_ptr->psm_cfg_info.threshold;

  resp_msg.psm_duration_due_to_oos     = psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos;
  resp_msg.psm_randomization_window  = psm_ptr->psm_cfg_ext_info.psm_randomization_window;
  resp_msg.active_timer_value               = cm_rpt_ptr->cmd.psm_get_timer_info_rsp.active_timer_value;
  resp_msg.periodic_tau_timer_value    = cm_rpt_ptr->cmd.psm_get_timer_info_rsp.periodic_tau_timer_value;
  resp_msg.periodic_rau_timer_value    = cm_rpt_ptr->cmd.psm_get_timer_info_rsp.periodic_rau_timer_value;
  resp_msg.gprs_ready_timer_value      = cm_rpt_ptr->cmd.psm_get_timer_info_rsp.gprs_ready_timer_value;
  
#ifdef FEATURE_APPS_ONLY_BOOT
  resp_msg.early_wakeup_time          = MAX(psm_ptr->psm_cfg_ext_info.early_wakeup_time,psm_ptr->nb1_early_wakeup_time);
#else
  resp_msg.early_wakeup_time          = psm_ptr->psm_cfg_ext_info.early_wakeup_time;
#endif

  msgr_error = msgr_send( (msgr_hdr_struct_type*)&resp_msg, sizeof(resp_msg) );
  CM_MSG_HIGH_6( "MM_CM_PSM_GET_CFG_PARAMS_RSP, msgr_error=%d, enabled=(%d -> %d), threshold=%d, psm_version=%d , duration_due_to_oos=%u", 
                                 msgr_error, psm_ptr->psm_cfg_info.is_enabled, resp_msg.enabled, 
                                 resp_msg.threshold, psm_ptr->psm_cfg_info.psm_version,resp_msg.psm_duration_due_to_oos);
  CM_MSG_HIGH_6( "MM_CM_PSM_GET_CFG_PARAMS_RSP, randomization_window=%d, active_timer=%u,periodic_update_timer=%u, periodic_rau_timer_value=%u,gprs_ready_timer_value=%u, early_wakeup_time:%u", 
                                 resp_msg.psm_randomization_window,
                                 resp_msg.active_timer_value, resp_msg.periodic_tau_timer_value,
                                 resp_msg.periodic_rau_timer_value,resp_msg.gprs_ready_timer_value,
                                 resp_msg.early_wakeup_time);

}
/*===========================================================================
FUNCTION cmpsm_process_srv_cnf

DESCRIPTION
  This function should be called to process the srv cnf.


DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
void cmpsm_process_srv_cnf(cm_rpt_type *cm_rpt_ptr)
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  cm_psm_deep_sleep_exit_ind_s_type         *psm_cmd 
         = (cm_psm_deep_sleep_exit_ind_s_type*)cm_mem_malloc(sizeof(cm_psm_deep_sleep_exit_ind_s_type));
    
  if(psm_cmd != NULL)
  {
    memset(psm_cmd, 0, sizeof(cm_psm_deep_sleep_exit_ind_s_type));
    cm_msgr_send(MM_CM_PSM_DEEP_SLEEP_EXIT_IND,MSGR_MM_CM_PSM,
                    (msgr_hdr_s*)psm_cmd,sizeof(cm_psm_deep_sleep_exit_ind_s_type));
  }
  
  cm_mem_free(psm_cmd);

}

/*===========================================================================
FUNCTION cm_psm_wrpt_handle

DESCRIPTION
  This function should be called to process the PSM reports from NAS like
  psm_ready_rsp, psm_ready_ind.


DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean                        cm_psm_wrpt_handle
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
)
{
  /* Pointer to LL report message.
  */
  cm_rpt_type           *cm_rpt_ptr        = NULL;

  /* Flag to indicate if the report was processed.
  */
  boolean is_rpt_processed                 = TRUE;
 #ifdef FEATURE_FMD_SPEED_INFO
  cmph_s_type				 *ph_ptr   = NULL;
 #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( rpt_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_rpt_ptr = (cm_rpt_type *) rpt_ptr;

  /* Invoke the appropriate event report handler.
  */
  switch ( cm_rpt_ptr->hdr.cmd )
  {
    case CM_PSM_READY_RSP:
    {
      cmpsm_process_ready_rsp(CM_PSM_MODULE_NAS, (void*)rpt_ptr );
      break;
    }
    case CM_PSM_READY_IND:
    {
      cmpsm_process_ready_ind(CM_PSM_MODULE_NAS, (void*)rpt_ptr);
      break;
    }
    case CM_ENTER_PSM_MODE_RSP:
    {
      cmpsm_process_enter_rsp(CM_PSM_MODULE_NAS, (void*)rpt_ptr);
      break;
    }

    case CM_PSM_GET_TIMER_INFO_RSP:
    {
      cmpsm_process_get_cfg_params_rsp(cm_rpt_ptr);
      break;
    }
    case CM_PSM_TIMER_CHANGE_IND:
    {
   #ifdef FEATURE_FMD_SPEED_INFO
      ph_ptr   = cmph_ptr();
      if(ph_ptr != NULL && ph_ptr->oprt_mode != SYS_OPRT_MODE_LPM)
	#endif
	  {    
		  cmpsm_process_config_change_ind(cm_rpt_ptr);
      }
	  break;
    }
      
    case CM_PSM_AUTO_READY_IND:
    {
      cmpsm_process_auto_ready_ind(CM_PSM_MODULE_NAS, (void*)rpt_ptr);
      break;
    }
 
    case CM_SERVICE_CNF:
    {
      cmpsm_process_srv_cnf(cm_rpt_ptr);
    }
    break;

    case CM_PSM_SAVE_MMGSDI_EF_RSP:
    {
      if(cmpsm_ptr()->psm_enter_req_status == CM_PSM_ENTER_STATUS_WAIT_MMGSDI_RSP)
      {
        cmpsm_start_psm_enter();
      }
    }
    break;
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      is_rpt_processed = FALSE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* switch ( ) */
  return is_rpt_processed;
}
#endif

/*===========================================================================
FUNCTION cmpsm_msgr_cmd_proc

DESCRIPTION
  Process PSM msgr reports.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_msgr_cmd_proc(

  const void   *cmd_ptr
    /* Pointer to the report sent by Message Router */
)
{
  cm_psm_u_type            *cm_cmd_ptr    = (cm_psm_u_type*)cmd_ptr;

  CM_ASSERT( cm_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_MSG_HIGH_1("cmpsm_msgr_cmd_proc, id %d",cm_cmd_ptr->msg_hdr.id);
  switch( cm_cmd_ptr->msg_hdr.id )
  {
  
#if   !defined( FEATURE_BUILD_PAGEONLY_IMAGE) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)
    case MM_CM_PSM_GET_CFG_PARAMS_REQ:
      cmpsm_process_get_cfg_params_req(cm_cmd_ptr->cm_psm_get_cfg_params_req.user_config);
      break;
  
    case MM_CM_PSM_SET_CFG_PARAMS_REQ:
      cmpsm_process_set_cfg_params_req(cm_cmd_ptr);
      break;
    
    case MM_CM_PSM_ENTER_REQ:
      cmpsm_process_psm_enter_req((uint64)cm_cmd_ptr->cm_psm_enter_req.apps_time * 1000);
      break;
        
    case MM_CM_PSM_DEEP_SLEEP_EXIT_REQ:
    case MM_CM_PSM_DEEP_SLEEP_TIMER_EXPIRY_IND:
      cmpsm_process_deep_sleep_timer_expiry();
      break;

    case MM_CM_PSM_READY_RSP:
      cmpsm_process_ready_rsp(cm_cmd_ptr->cm_client_psm_ready_rsp.module, cmd_ptr );
      break;
    
    case MM_CM_PSM_READY_IND:
      cmpsm_process_ready_ind(cm_cmd_ptr->cm_client_psm_ready_ind.module, cmd_ptr );
      break;
      
    case MM_CM_PSM_AUTO_READY_IND:
      cmpsm_process_auto_ready_ind(cm_cmd_ptr->cm_client_psm_auto_ready_ind.module, cmd_ptr );
      break;
#ifndef FEATURE_REF_TRACKER
    case MM_CM_PSM_UART_BAUD_RATE_IND:
      cmpsm_process_uart_baud_rate_ind(cm_cmd_ptr->cm_psm_baud_rate_ind.uart_baud_rate);
      break;
#endif
#else
    case MM_CM_PSM_AUTO_READY_EXT_IND:
      cmpsm_process_auto_ready_ext_ind(cmd_ptr);
      break;
#endif

    case MM_CM_PSM_ENTER_RSP:
      cmpsm_process_enter_rsp(cm_cmd_ptr->cm_client_psm_enter_rsp.module, cmd_ptr );
      break;

    default:
      break;
  }
}
/*===========================================================================
FUNCTION cmpsm_process_lpm

DESCRIPTION
 Send PSM_STATUS_COMPLETE Indication to clients.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_lpm( void )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if( psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_IDLE 
      && psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_COMPLETE
      && psm_ptr->psm_enter_req_status != CM_PSM_ENTER_STATUS_WAIT_READY_IND)
  {
    cmpsm_send_psm_ind( CM_PSM_STATUS_COMPLETED, CM_PSM_REJECT_REASON_PSM_NO_ERROR);
    psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_COMPLETE;
  }

  psm_ptr->full_service_lost_count = CM_PSM_MAX_OOS_SCANS;
  psm_ptr->psm_oos_uptime = CM_PSM_MAX_UPTIME;
  psm_ptr->psm_lmtd_service_ind_uptime = CM_PSM_MAX_UPTIME;
  psm_ptr->psm_due_to_oos_allowed  = FALSE;
  (void) timer_clr(&cmpsm_inactivity_timer,T_SEC);
}

#if !defined (FEATURE_BUILD_PAGEONLY_IMAGE) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)
/*===========================================================================

FUNCTION cmpsm_due_to_oos_allowed

DESCRIPTION
   Checks protocol status of any of the module is ACTIVE_OOS.


DEPENDENCIES
   None

RETURN VALUE
  TRUE  - if protocol status of any of the module is ACTIVE_OOS.
  FALSE - if protocol status of none of the module is ACTIVE_OOS.

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmpsm_due_to_oos_allowed( void )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  cm_psm_module_e_type        psm_module;

  for(psm_module = CM_PSM_MODULE_BEGIN;psm_module <= CM_PSM_MODULE_END;psm_module++ )
  {
    if(psm_ptr->module_info[psm_module].psm_enter_prot_status == SYS_PSM_STATUS_NOT_READY_ACTIVE_OOS)
    {
      return TRUE;
    }
  }
  
  return FALSE;

}

/*===========================================================================

FUNCTION cmpsm_proc_full_srv_lost

DESCRIPTION
   Process the full service lost indication from SD.


DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmpsm_proc_full_srv_lost 
(  
  sd_ss_e_type ss,
  boolean      is_scan_attempted
)
{
  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if(( psm_ptr->psm_cfg_info.is_enabled != TRUE ) || !( psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_OOS )
    || is_scan_attempted != TRUE)
  {
    return;
  }

  if( psm_ptr->full_service_lost_count == CM_PSM_MAX_OOS_SCANS )
  {
    psm_ptr->full_service_lost_count = 0;
  }

  psm_ptr->full_service_lost_count ++;

  CM_MSG_HIGH_2 ( "cmpsm_proc_full_srv_lost - (max:%d,  count:%d) Triggers PSM if count >= max" , 
               psm_ptr->psm_cfg_ext_info.max_oos_full_scans, psm_ptr->full_service_lost_count);

  if(psm_ptr->full_service_lost_count  >= psm_ptr->psm_cfg_ext_info.max_oos_full_scans )
  {
    psm_ptr->full_service_lost_count = CM_PSM_MAX_OOS_SCANS;
    psm_ptr->psm_oos_uptime = CM_PSM_MAX_UPTIME;
    psm_ptr->psm_due_to_oos_allowed  = TRUE;
    if(cmpsm_due_to_oos_allowed())
    {
      cmpsm_reset();
    }
      #if !defined (FEATURE_BUILD_PAGEONLY_IMAGE)  && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)  
 	psm_ptr->psm_full_boot_uptime = CM_PSM_MAX_UPTIME_64;
#endif
    cmpsm_process_psm_enter_req((uint64)psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos * 1000);
  }

}

/*===========================================================================
FUNCTION cmpsm_proc_srv_ind

DESCRIPTION
   Process service indication from SD. Reset the psm full service lost counter.

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern  void  cmpsm_proc_srv_ind 
( 
  const sd_si_info_s_type   *si_info_ptr 
)
{
  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if(( psm_ptr->psm_cfg_info.is_enabled != TRUE ) || 
     !( psm_ptr->psm_cfg_ext_info.psm_opt_mask & (CM_PSM_OPT_MASK_OOS | CM_PSM_OPT_MASK_LMTD_SRV) )
    )
  {
    return;
  }

  // Reset full_service_lost_count on reciving full service
  if(psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_OOS)
  {
    if (si_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
    {
      if ((psm_ptr->full_service_lost_count != CM_PSM_MAX_OOS_SCANS) || (psm_ptr->psm_oos_uptime != CM_PSM_MAX_UPTIME))
      {
        CM_MSG_HIGH_2 ( "cmpsm_proc_srv_ind - resetting oos counter = %d psm_oos_uptime = %lu", psm_ptr->full_service_lost_count, psm_ptr->psm_oos_uptime );
        psm_ptr->full_service_lost_count = CM_PSM_MAX_OOS_SCANS;
        psm_ptr->psm_oos_uptime          = CM_PSM_MAX_UPTIME;
        psm_ptr->psm_due_to_oos_allowed  = FALSE;
      }
    }
    else
    {
      //Restart OOS timer if not running already
      if(psm_ptr->psm_oos_uptime == CM_PSM_MAX_UPTIME) 
      {
        dword   uptime = time_get_secs();  /* Get the current time */

        psm_ptr->psm_oos_uptime = uptime + psm_ptr->psm_cfg_ext_info.max_oos_time;
        CM_MSG_HIGH_2 ( "set psm_oos_uptime - (uptime:%lu,  psm_oos_uptime:%lu)" , 
                         uptime, psm_ptr->psm_oos_uptime);
      }
    }
  }

  if(psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_LMTD_SRV)
  {
    // Reset lmtd srv uptimer if already running and got full service
    if( (psm_ptr->psm_lmtd_service_ind_uptime != CM_PSM_MAX_UPTIME) && 
        (si_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
      )
    {
      CM_MSG_HIGH_1 ( "cmpsm_proc_srv_ind - resetting psm lmtd srv uptime = %lu", psm_ptr->psm_lmtd_service_ind_uptime );
      psm_ptr->psm_lmtd_service_ind_uptime = CM_PSM_MAX_UPTIME;
    }
    else if( (sys_intl_srv_status_to_actl_srv_status(si_info_ptr->srv_status) == SYS_SRV_STATUS_LIMITED )&&
        /* To make sure limited service is not due to CAMPED_IND*/
        (si_info_ptr->srv_domain != SYS_SRV_DOMAIN_CAMPED)
      )
    {
        cmpsm_set_psm_lmtd_srv_ind_timer();
    }
  }
}

#ifndef FEATURE_REF_TRACKER
/*===========================================================================

FUNCTION cmpsm_timer_proc

DESCRIPTION
  Process timer for PSM.

DEPENDENCIES
  PSM object must have already been initialized with cmpsm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_timer_proc
(  
  cm_timer_event_type    timer_event   /* Indicate specific timer event */
)
{
  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();         /* get the pointer to the psm object */
  dword                    uptime = time_get_secs();  /* Get the current uptime */
  cmss_s_type *ss_ptr = cmss_ptr();

  //PSM is not enabled
  if(psm_ptr->psm_cfg_info.is_enabled != TRUE )
  {
    return;
  }
  if( (psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_OOS) && 
       (psm_ptr->psm_oos_uptime != CM_PSM_MAX_UPTIME) &&
       (uptime >= psm_ptr->psm_oos_uptime) &&
       (psm_ptr->full_service_lost_count != CM_PSM_MAX_OOS_SCANS &&
        psm_ptr->full_service_lost_count > 0)
     )
  {
	  CM_MSG_HIGH_1 ( "Triggering PSM due to OOS max time-out: %d",psm_ptr->psm_oos_uptime );
    psm_ptr->psm_oos_uptime = CM_PSM_MAX_UPTIME;
    psm_ptr->full_service_lost_count = CM_PSM_MAX_OOS_SCANS;
    psm_ptr->psm_due_to_oos_allowed  = TRUE;
    if(cmpsm_due_to_oos_allowed())
    {
      cmpsm_reset();
    }
    cmpsm_process_psm_enter_req((uint64)psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos* 1000);

  }
       
  /* PSM Limited service indication timer is expired 
  */
  if( (psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_LMTD_SRV) &&
      (psm_ptr->psm_lmtd_service_ind_uptime != CM_PSM_MAX_UPTIME) &&
      (uptime >= psm_ptr->psm_lmtd_service_ind_uptime)  
   )
  {
    CM_MSG_HIGH_0 ( "MM_CM_PSM_STATUS_IND - LIMITERE SRV" );
    psm_ptr->psm_lmtd_service_ind_uptime = CM_PSM_MAX_UPTIME;
	cmpsm_reset();
	cmpsm_process_psm_enter_req((uint64)psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos * 1000);
  }
} /* cmpsm_timer_proc */
#endif
/*===========================================================================

FUNCTION cmpsm_set_psm_lmtd_srv_ind_timer

DESCRIPTION
  Set limited service indication timer.

DEPENDENCIES
  PSM object must have already been initialized with cmpsm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_set_psm_lmtd_srv_ind_timer( )
{

  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if ( (psm_ptr->psm_cfg_info.is_enabled != TRUE) || 
       !( psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_LMTD_SRV )
     )
  {
    return;
  }
  //Set psm lmtd srv uptime if timer is not running
  if ( (psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_LMTD_SRV) && 
       (psm_ptr->psm_lmtd_service_ind_uptime == CM_PSM_MAX_UPTIME) 
     )
  {
    dword                    uptime = time_get_secs();  /* Get the current time */

    psm_ptr->psm_lmtd_service_ind_uptime =  uptime + psm_ptr->psm_cfg_ext_info.max_oos_time;
    CM_MSG_HIGH_2 ( "cmpsm_set_psm_lmtd_srv_ind_timer - (uptime:%lu,  ind_uptime:%d)" , 
                     uptime, psm_ptr->psm_lmtd_service_ind_uptime);

  }
}

/*===========================================================================
FUNCTION cmpsm_process_online

DESCRIPTION
 start OOS timer if needed.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_online( void )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  dword                    uptime = time_get_secs();  /* Get the current time */


  if(psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_OOS)
  {
    psm_ptr->psm_oos_uptime = uptime + psm_ptr->psm_cfg_ext_info.max_oos_time;
    CM_MSG_HIGH_2 ( "cmpsm_process_online(uptime:%lu, max_oos_time:%d)", uptime, psm_ptr->psm_cfg_ext_info.max_oos_time );

  }

  if(psm_ptr->psm_rtc_info.rtc_enabled == FALSE)
  {
    CM_MSG_HIGH_1 ( "Setting RTC time-out for %u", psm_ptr->psm_rtc_info.timer_info_timeout );
    timer_set(&cmpsm_rtc_timer,psm_ptr->psm_rtc_info.timer_info_timeout,0,T_SEC);
  }
}

/*===========================================================================
FUNCTION cmpsm_handle_subs_not_avail

DESCRIPTION
 Reset ongoing PSM transaction on detach from NW

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/

void cmpsm_handle_subs_not_avail(void)
{
   cm_psm_info_s_type    *psm_ptr  =  cmpsm_ptr();
   psm_ptr->psm_full_boot_uptime = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID_64;
   #ifdef FEATURE_REF_TRACKER
   psm_ptr->psm_page_only_boot_time = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID_64;
   #endif
   cmpsm_reset();     
}

/*===========================================================================
FUNCTION cmpsm_rtc_timer_expiry_cb

DESCRIPTION
  Timer callback function used to process deep sleep timer expiry.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cmpsm_rtc_timer_expiry_cb(timer_cb_data_type data)
{
  /* Pushing timer ready on timer expiry*/
  CM_MSG_HIGH_1(" RTC timer expiry, current rtc_enabled: %d", cmpsm_ptr()->psm_rtc_info.timer_info_timeout);
  
  if(cmpsm_ptr()->psm_rtc_info.timer_info_timeout)
  {
    cm_ph_cmd_timer_info_ready(NULL,NULL,CM_CLIENT_ID_ANONYMOUS);
  }
}
/*===========================================================================
FUNCTION cmpsm_inactivity_timeout_cb

DESCRIPTION
  Timer callback function used to process inactivity timer expiry.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cmpsm_inactivity_timeout_cb(timer_cb_data_type data)
{
  cm_psm_enter_req_s_type         *psm_cmd = (cm_psm_enter_req_s_type*)cm_mem_malloc(sizeof(cm_psm_enter_req_s_type));
  cm_psm_info_s_type *psm_ptr = cmpsm_ptr();
  CM_MSG_HIGH_0 ( "Inactivity timeout expiry process start" );
  if(psm_ptr->psm_cfg_info.is_enabled == TRUE && psm_cmd != NULL)
    {
      memset(psm_cmd, 0, sizeof(cm_psm_enter_req_s_type));
      
      psm_cmd->apps_time = cmpsm_ptr()->psm_inactivity_cfg.psm_duration_due_to_inactivity;
      
      cm_msgr_send(MM_CM_PSM_ENTER_REQ,MSGR_MM_CM_PSM,
                    (msgr_hdr_s*)psm_cmd,sizeof(cm_psm_enter_req_s_type));
    }
  cm_mem_free(psm_cmd);
}

/*===========================================================================
FUNCTION cmpsm_high_activity_timeout_cb

DESCRIPTION
  Timer callback function used to process high activity timer expiry.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cmpsm_high_activity_timeout_cb(timer_cb_data_type data)
{
  cm_psm_enter_req_s_type		  *psm_cmd = (cm_psm_enter_req_s_type*)cm_mem_malloc(sizeof(cm_psm_enter_req_s_type));
  cm_psm_info_s_type *psm_ptr = cmpsm_ptr();
  uint64  uptime = time_get_ms_native();
  CM_MSG_HIGH_0("Send ready req to all modules upon high_activity timer expiry");
  memset(psm_cmd, 0, sizeof(cm_psm_enter_req_s_type));
  psm_cmd->apps_time = (uint32)(psm_ptr->psm_enter_uptime - uptime);
      
  cm_msgr_send(MM_CM_PSM_ENTER_REQ,MSGR_MM_CM_PSM,
                    (msgr_hdr_s*)psm_cmd,sizeof(cm_psm_enter_req_s_type));
  cm_mem_free(psm_cmd);
  
}

/*===========================================================================
FUNCTION cmpsm_pwr_up_uim_opt_enabled_and_no_subs_avail

DESCRIPTION
  To check whether PSM UIM Optimization is enabled and no subscription available.

DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmpsm_pwr_up_uim_opt_enabled_and_no_subs_avail()
{

  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if( psm_ptr->psm_cfg_info.is_enabled &&  psm_ptr->psm_uim_opt_enabled &&
      !cmmmgsdi_count_given_state(CMMMGSDI_STATE_SUBSCRIPTION_READY)
     )
  {
    CM_MSG_HIGH_0 ( "uim_opt_enabled_and_no_subs_avail" );
    if(cm_get_ulog_handle() != NULL)
    {
       ULOG_RT_PRINTF_0(cm_get_ulog_handle(), " =CM= uim_opt_enabled_and_no_subs_avail");
    }
    return TRUE;
  }

  return FALSE;


} /* cmpsm_pwr_up_uim_opt_enabled_and_no_subs_avail */
 
/*===========================================================================
FUNCTION cmpsm_deep_sleep_timer_expiry_cb

DESCRIPTION
  Timer callback function used to process deep sleep timer expiry.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cmpsm_deep_sleep_timer_expiry_cb(timer_cb_data_type data)
{
  cm_generic_rpt_u_type     gen_rpt;

  memset(&gen_rpt, 0x00, sizeof(cm_generic_rpt_u_type));
  
  CM_MSG_HIGH_0("cmpsm_deep_sleep_timer_expiry_cb");

  cm_client_send_psm_rpt(CM_PSM_DEEP_SLEEP_TIMER_EXPIRY_IND, &gen_rpt);
  
  SYS_ARG_NOT_USED (data);
}
/*===========================================================================

FUNCTION cmpsm_process_deep_sleep_timer_expiry

DESCRIPTION
   Process PSM deep sleep timer expiry.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void cmpsm_process_deep_sleep_timer_expiry()
{
  cm_psm_info_s_type       *psm_ptr = cmpsm_ptr();             /* get the pointer to the psm object */
  
  if(psm_ptr->psm_enter_req_status == CM_PSM_ENTER_STATUS_DEEP_SLEEP)
  {
    CM_MSG_HIGH_0 ( "Exit deep-sleep mode ");

    cmpsm_handle_deep_sleep(FALSE);

    if(cmpsm_check_not_ready_psm_prot_status(SYS_PSM_STATUS_ENTER_DEEP_SLEEP))
    {
       uint64  uptime = time_get_ms_native();  /* Get the current time */
      
       psm_ptr->psm_periodic_update_uptime = (uint64)psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos * 1000 + uptime;
       cmpsm_send_psm_ind(CM_PSM_STATUS_READY, CM_PSM_REJECT_REASON_PSM_NO_ERROR);
       cmpsm_reset();
    }
  }
}
/*===========================================================================

FUNCTION cmpsm_get_rtc_enabled

DESCRIPTION
   Returns if RTC is available

DEPENDENCIES
  none

RETURN VALUE
  Returns if RTC is available
  
SIDE EFFECTS
  none
===========================================================================*/
boolean cmpsm_get_rtc_enabled()
{
  
  return cmpsm_ptr()->psm_rtc_info.rtc_enabled;
}
/*===========================================================================

FUNCTION cmpsm_update_rtc_enabled

DESCRIPTION
    Updates RTC enabled

DEPENDENCIES
  none

RETURN VALUE
  none
  
SIDE EFFECTS
  none
===========================================================================*/
void cmpsm_update_rtc_enabled(boolean rtc_enabled)
{
  cmpsm_ptr()->psm_rtc_info.rtc_enabled = rtc_enabled;
}
uint8 cm_get_psm_sw_ver(void)
{
  return 2;
}
/*===========================================================================
FUNCTION cmpsm_process_non_online_deep_sleep

DESCRIPTION
 Reset the psm enter request status

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_non_online_deep_sleep( void )
{
  cm_psm_info_s_type         *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */

  if( psm_ptr->psm_enter_req_status == CM_PSM_ENTER_STATUS_DEEP_SLEEP )
  {
     psm_ptr->psm_enter_req_status = CM_PSM_ENTER_STATUS_IDLE;
     psm_ptr->psm_periodic_update_uptime = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID_64;
  }

}
/*===========================================================================

FUNCTION cm_is_psm_context_saved

DESCRIPTION
  Check if all modules saved context successfully
  
DEPENDENCIES
  To be invoked only when device is entering PSM oprt mode

RETURN VALUE
  if context is saved

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_is_psm_context_saved(sys_modem_as_id_e_type         asub_id)
{
  cm_psm_info_s_type           *psm_ptr  =  cmpsm_ptr();  /* get the pointer to the psm object */
  boolean                       psm_context_saved = FALSE;
  
  if(psm_ptr->psm_enter_req_status == CM_PSM_ENTER_STATUS_ENTER_IN_PROGRESS)
  {
    psm_context_saved = psm_ptr->psm_context_retained;
  }

  CM_MSG_HIGH_2("sub:%d, PSM context saved:%d, Enter status:%d",
                      psm_context_saved,psm_ptr->psm_enter_req_status);
  
  return psm_context_saved;
}

#endif
#ifdef FEATURE_REF_TRACKER
/*===========================================================================

FUNCTION cmpsm_timer_proc

DESCRIPTION
  Process timer for PSM.

DEPENDENCIES
  PSM object must have already been initialized with cmpsm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_timer_proc
(  
  cm_timer_event_type    timer_event   /* Indicate specific timer event */
)
{
  cm_psm_info_s_type       *psm_ptr  =  cmpsm_ptr();         /* get the pointer to the psm object */
  dword                    uptime = time_get_secs();  /* Get the current uptime */
#if !defined( FEATURE_BUILD_PAGEONLY_IMAGE ) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)
  cmss_s_type *ss_ptr = cmss_ptr();

  //PSM is not enabled
  if(psm_ptr->psm_cfg_info.is_enabled != TRUE )
  {
    return;
  }
  if( (psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_OOS) && 
       (psm_ptr->psm_oos_uptime != CM_PSM_MAX_UPTIME) &&
       (uptime >= psm_ptr->psm_oos_uptime) &&
       (psm_ptr->full_service_lost_count != CM_PSM_MAX_OOS_SCANS &&
        psm_ptr->full_service_lost_count > 0)
     )
  {
	  CM_MSG_HIGH_1 ( "Triggering PSM due to OOS max time-out: %d",psm_ptr->psm_oos_uptime );
    psm_ptr->psm_oos_uptime = CM_PSM_MAX_UPTIME;
    psm_ptr->full_service_lost_count = CM_PSM_MAX_OOS_SCANS;
    psm_ptr->psm_due_to_oos_allowed  = TRUE;
    if(cmpsm_due_to_oos_allowed())
    {
      cmpsm_reset();
    }
    cmpsm_process_psm_enter_req((uint64)psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos* 1000);

  }
       
  /* PSM Limited service indication timer is expired 
  */
  if( (psm_ptr->psm_cfg_ext_info.psm_opt_mask & CM_PSM_OPT_MASK_LMTD_SRV) &&
      (psm_ptr->psm_lmtd_service_ind_uptime != CM_PSM_MAX_UPTIME) &&
      (uptime >= psm_ptr->psm_lmtd_service_ind_uptime)  
   )
  {
    CM_MSG_HIGH_0 ( "MM_CM_PSM_STATUS_IND - LIMITERE SRV" );
    psm_ptr->psm_lmtd_service_ind_uptime = CM_PSM_MAX_UPTIME;
	cmpsm_reset();
	cmpsm_process_psm_enter_req((uint64)psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos * 1000);
  }
#endif
  #ifdef FEATURE_BUILD_PAGEONLY_IMAGE
/* following should be in page only image  */
  if((psm_ptr->psm_sns_boot_time != 0) && 
	  (uptime >= psm_ptr->psm_sns_boot_time))
   {

     psm_ptr->psm_enter_uptime = 0;
	 psm_ptr->psm_entry_type = CM_PSM_TYPE_RESET;
	 psm_ptr->psm_periodic_update_uptime = 0;
     /* Need to boot in full image */
	  cmpsm_start_psm_enter();

   }
 #endif
 #ifdef FEATURE_BUILD_SENSORONLY_IMAGE
 /* following should be in sensor only image  */
   if((psm_ptr->psm_page_only_boot_time !=0 )  &&
	   (uptime >= psm_ptr->psm_page_only_boot_time))
   {
             /* Need to boot in full image */
	 psm_ptr->psm_enter_uptime = 0;
	 psm_ptr->psm_entry_type = CM_PSM_TYPE_RESET;
	 psm_ptr->psm_periodic_update_uptime = 0;
	 cmpsm_start_psm_enter();
			 
   }
#endif
} /* cmpsm_timer_proc */
#endif

/*===========================================================================
FUNCTION cmpsm_init

DESCRIPTION
  PSM initialization. Read the NV items related to PSM and reset internal 
  data structures.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_init(void)
{
  cm_psm_info_s_type *    psm_ptr = cmpsm_ptr();
  uint8 data = 0;
  uint8 mask =ALL_PSM_BIT;
  
#if !defined (FEATURE_BUILD_PAGEONLY_IMAGE) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)
  errno_enum_type msgr_error;
  cm_psm_get_cfg_params_rsp_s_type resp_msg;
  msgr_init_hdr( (msgr_hdr_struct_type*)&resp_msg, MSGR_MM_CM, MM_CM_PSM_GET_CFG_PARAMS_RSP );
#endif

  psm_ptr->psm_context_restore_needed             = TRUE;

  pm_app_misc_get_cfg(PM_APP_MISC_CFG_BOOT_OPTION, &data);

  if(cm_get_ulog_handle() != NULL)
  {
      ULOG_RT_PRINTF_1(cm_get_ulog_handle(), " PMIC Register contents after Boot_up = %d", data);
  }

  psm_ptr->full_service_lost_count                = CM_PSM_MAX_OOS_SCANS;
  psm_ptr->psm_lmtd_service_ind_uptime            = CM_PSM_MAX_UPTIME;
  psm_ptr->psm_oos_uptime                         = CM_PSM_MAX_UPTIME;
  psm_ptr->psm_due_to_oos_allowed                 = FALSE;
  psm_ptr->psm_periodic_update_uptime             = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID_64;
  psm_ptr->psm_context_retained                   = FALSE;
  psm_ptr->psm_full_boot_uptime = CM_PSM_MAX_UPTIME_64;
  psm_ptr->psm_status_periodic_update_val_dbg = 0;
  psm_ptr->is_next_full_image = FALSE;
#ifdef FEATURE_REF_TRACKER
  psm_ptr->psm_sns_boot_time = CM_PSM_MAX_UPTIME_64;
  psm_ptr->psm_page_only_boot_time = CM_PSM_MAX_UPTIME_64;
#endif

  //RESET high_activity timer  
  (void) timer_clr(&cmpsm_high_activity_timer,T_SEC);

#ifdef FEATURE_BUILD_PAGEONLY_IMAGE
  psm_ptr->psm_entry_type = CM_PSM_TYPE_SHUTDOWN;
  cmpsm_read_context();
  
  /*reseting PMIC register in POI boot-up after taking RAT info from it. 
  So that in any unforeseen event it would come back in full image.*/
  pm_app_misc_set_cfg( PM_APP_MISC_CFG_BOOT_OPTION, mask, 0);  
#endif
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE  
  pm_app_misc_get_cfg(PM_APP_MISC_CFG_BOOT_OPTION, &data);  
  psm_ptr->sensor_image_boot = (data & SNS_BOOT_BIT)?TRUE:FALSE;
  psm_ptr->psm_entry_type = CM_PSM_TYPE_SHUTDOWN;
  cmpsm_read_context();
  cmsns_initiate_snsmgr();
#endif
  //Read the NV items related to PSM
  cmpsm_nv_read();
  cmpsm_reset();

  
  

  #if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA)) && !defined(FEATURE_BUILD_PAGEONLY_IMAGE) && !defined(FEATURE_BUILD_SENSORONLY_IMAGE)
  cm_client_psm_ready_reg(cmpsm_send_psm_ready_req_to_reg,CM_PSM_MODULE_NAS);
  cm_client_psm_enter_reg(cmpsm_send_psm_enter_req_to_reg,CM_PSM_MODULE_NAS);

  /* Setup a timer to receive indication after timer expiry. */
  timer_def(&cmpsm_deep_sleep_timer, &mmoc_timer_group, NULL, 0, cmpsm_deep_sleep_timer_expiry_cb, (timer_cb_data_type)0);
  timer_def(&cmpsm_rtc_timer, &mmoc_timer_group, NULL, 0, cmpsm_rtc_timer_expiry_cb, (timer_cb_data_type)0);
  timer_def(&cmpsm_inactivity_timer, &mmoc_timer_group, NULL, 0, cmpsm_inactivity_timeout_cb, (timer_cb_data_type)0);
  timer_def(&cmpsm_high_activity_timer, &mmoc_timer_group, NULL, 0, cmpsm_high_activity_timeout_cb, (timer_cb_data_type)0);
  


  resp_msg.enabled                                = psm_ptr->psm_cfg_info.is_enabled;
  resp_msg.threshold                              = psm_ptr->psm_cfg_info.threshold;
  resp_msg.psm_duration_due_to_oos                = psm_ptr->psm_cfg_ext_info.psm_duration_due_to_oos;
  resp_msg.psm_randomization_window               = psm_ptr->psm_cfg_ext_info.psm_randomization_window;
  resp_msg.active_timer_value                     = psm_ptr->last_psm_cfg_info.active_timer_value;
  resp_msg.periodic_tau_timer_value               = psm_ptr->last_psm_cfg_info.periodic_update_timer_value;
  resp_msg.periodic_rau_timer_value               = psm_ptr->last_psm_cfg_info.periodic_update_timer_value;
  resp_msg.gprs_ready_timer_value                 = psm_ptr->last_psm_cfg_info.gprs_ready_timer_value;
  
  msgr_error = msgr_send( (msgr_hdr_struct_type*)&resp_msg, sizeof(resp_msg) );
#endif

}
/*===========================================================================

FUNCTION cm_client_send_psm_rpt

DESCRIPTION
  Function for clients to notify CM PSM status.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cm_client_send_psm_rpt(cm_name_type    rpt, cm_generic_rpt_u_type*       psm_rpt)
{
  cm_psm_u_type         *psm_cmd = (cm_psm_u_type*)cm_mem_malloc(sizeof(cm_psm_u_type));
  uint32                 msgr_id =0;

  if(psm_cmd == NULL)
  {
    return;
  }
  
  switch ( rpt)
  {
    case CM_CLIENT_PSM_READY_RSP:
    {
      msgr_id = MM_CM_PSM_READY_RSP;
      psm_cmd->cm_client_psm_ready_rsp = psm_rpt->psm_ready_rsp;
      break;
    }
    case CM_CLIENT_PSM_READY_IND:
    {
      msgr_id = MM_CM_PSM_READY_IND;
      psm_cmd->cm_client_psm_ready_ind = psm_rpt->psm_ready_ind;
      break;
    }
    
    case CM_CLIENT_PSM_ENTER_RSP:
    {
      msgr_id = MM_CM_PSM_ENTER_RSP;
      psm_cmd->cm_client_psm_enter_rsp = psm_rpt->psm_enter_rsp;
      break;
    }

    case CM_PSM_DEEP_SLEEP_TIMER_EXPIRY_IND:
    {
      msgr_id = MM_CM_PSM_DEEP_SLEEP_TIMER_EXPIRY_IND;
      psm_cmd->cm_psm_deep_sleep_timer_expiry_ind = psm_rpt->psm_deep_sleep_timer_expiry_ind;
      break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      break;
  } /* switch ( ) */
  
  /* Queue the PSM status report to CM
  */
  if(msgr_id !=0)
  {
    cm_msgr_send(msgr_id,MSGR_MM_CM_PSM,(msgr_hdr_s*)psm_cmd,sizeof(cm_psm_u_type));
  }

  cm_mem_free(psm_cmd);
}


/*===========================================================================

FUNCTION cmpsm_read_context

DESCRIPTION
    Read PSM context and store into module

DEPENDENCIES
  none

RETURN VALUE
  none
  
SIDE EFFECTS
  none
===========================================================================*/
boolean cmpsm_read_context(void)
{
  cm_psm_info_s_type       *psm_ptr = cmpsm_ptr();             /* get the pointer to the psm object */
  uint8      *psm_context_ptr,*current_block;
  cm_psm_module_e_type psm_module;
  int32      file_size = 0,temp_size = 0;
  const char* file;
  uint64  uptime = time_get_ms_native(); 

  if(psm_ptr->psm_context_restore_needed == FALSE)
  {
    /* Already restored so just exit*/
    CM_MSG_HIGH_0("Context already restored, so skipping now");
    return FALSE;
  }

  /* Restoration needs to be attempted only at first sub-ready*/
  psm_ptr->psm_context_restore_needed = FALSE;

  temp_size = sizeof(uint32);

  cmefs_read(CMEFS_PSM_CONTEXT,(byte*)&file_size,&temp_size);

  if(file_size == 0)
  {
    return FALSE;
  }
  psm_context_ptr = (uint8*)cm_mem_malloc(file_size);

  temp_size = file_size;
  cmefs_read(CMEFS_PSM_CONTEXT,psm_context_ptr,&temp_size);
  
  if(temp_size < file_size)
  {
    current_block = (uint8*)(psm_context_ptr + temp_size);
    
    temp_size = file_size - temp_size;
    
    cmefs_read(CMEFS_PSM_CONTEXT_EXT,current_block,&temp_size);
    
  }

  /* Moving by context size*/
  current_block = (uint8*)(psm_context_ptr + sizeof(uint32));
  
  memscpy(&psm_ptr->psm_full_boot_uptime,sizeof(uint64),current_block,sizeof(uint64));
  /* Moving by full boot time*/
  current_block = (uint8*)(current_block + sizeof(uint64));

#ifdef FEATURE_REF_TRACKER
  memscpy(&psm_ptr->psm_page_only_boot_time,sizeof(uint64),current_block,sizeof(uint64));
  /* Moving by page only boot time*/
  current_block = (uint8*)(current_block + sizeof(uint64));

  memscpy(&psm_ptr->psm_sns_boot_time,sizeof(uint64),current_block,sizeof(uint64));
  /* Moving by sensor boot time*/
  current_block = (uint8*)(current_block + sizeof(uint64));
#endif
  memscpy(&psm_ptr->psm_rat,sizeof(sys_radio_access_tech_e_type),current_block,sizeof(sys_radio_access_tech_e_type));
  current_block = (uint8*)(current_block + sizeof(sys_radio_access_tech_e_type));

#ifndef FEATURE_REF_TRACKER

  memscpy(&psm_ptr->uart_baud_rate,sizeof(uint16),current_block,sizeof(uint16));
  current_block = (uint8*)(current_block + sizeof(uint16));

#endif

   memscpy(&psm_ptr->psm_status_periodic_update_val_dbg,sizeof(uint64),current_block,sizeof(uint64));
  /* Moving by dbg time value*/
   current_block = (uint8*)(current_block + sizeof(uint64));

  memscpy(&psm_ptr->edrx_ptw,sizeof(uint8),current_block,sizeof(uint8));
  current_block = (uint8*)(current_block + sizeof(uint8));

  for(psm_module = CM_PSM_MODULE_BEGIN; psm_module<= CM_PSM_MODULE_END; psm_module++)
  {
    memscpy(&psm_ptr->module_info[psm_module].context_size,sizeof(uint32),current_block,sizeof(uint32));
    current_block = current_block + sizeof(uint32);
    
    if(cm_get_ulog_handle() != NULL)
    {
      ULOG_RT_PRINTF_2(cm_get_ulog_handle(), " =CM= psm_module:%d, context_size:%d", \
                 psm_module,psm_ptr->module_info[psm_module].context_size);
    }
    
    if(psm_ptr->module_info[psm_module].context_size != 0)
    {
      psm_ptr->module_info[psm_module].context =
              (uint8*)cm_mem_malloc(psm_ptr->module_info[psm_module].context_size);
      memscpy(psm_ptr->module_info[psm_module].context,psm_ptr->module_info[psm_module].context_size,
              current_block,psm_ptr->module_info[psm_module].context_size);
    }
    
    current_block += psm_ptr->module_info[psm_module].context_size;
  }



  cm_mem_free(psm_context_ptr);


  file = cmefs_map_efs_id_to_filename(CMEFS_PSM_CONTEXT);

  if(file == NULL)
  {
    return FALSE;
  }

  /* Deleting so that so that it needs to be available only for PSM entry*/
#if defined (FEATURE_BUILD_PAGEONLY_IMAGE) || defined(FEATURE_BUILD_SENSORONLY_IMAGE)
  efs_unlink(file);
#else
  (void)mcfg_fs_delete(file,
                       MCFG_FS_TYPE_EFS,
                       MCFG_FS_SUBID_0);   
#endif

  file = cmefs_map_efs_id_to_filename(CMEFS_PSM_CONTEXT_EXT);

  if(file == NULL)
  {
    return FALSE;
  }

    /* Deleting so that so that it needs to be available only for PSM entry*/
#if defined (FEATURE_BUILD_PAGEONLY_IMAGE) || defined(FEATURE_BUILD_SENSORONLY_IMAGE) 
  efs_unlink(file);
#else
  (void)mcfg_fs_delete(file,
                       MCFG_FS_TYPE_EFS,
                       MCFG_FS_SUBID_0);
#endif  
  return TRUE;
}

/*===========================================================================

FUNCTION cmpsm_retreive_saved_context

DESCRIPTION
   Retrieve saved context
   context free to be taken care by module
   CM passes its buffer to module

DEPENDENCIES
  none

RETURN VALUE
  size of context,can be used for validation on client side
  
SIDE EFFECTS
  none
===========================================================================*/
uint32 cmpsm_retreive_saved_context(
cm_psm_module_e_type psm_module,
uint8** context
)
{
  cm_psm_info_s_type       *psm_ptr = cmpsm_ptr();             /* get the pointer to the psm object */
  uint32                    context_size = psm_ptr->module_info[psm_module].context_size;
  
  if(psm_ptr->module_info[psm_module].context_size != 0)
  {
    *context = psm_ptr->module_info[psm_module].context;
    psm_ptr->module_info[psm_module].context = NULL;
    psm_ptr->module_info[psm_module].context_size = 0;
  }
  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_2(cm_get_ulog_handle(), " Retrieve PSM context module:%d, context_size:%d",\
           psm_module,context_size);
  }
  
  return context_size;
}

sys_sys_mode_e_type  cm_get_edrx_active_rat(void)
{
  if(cmpsm_ptr()->psm_rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
  {
    return SYS_SYS_MODE_LTE_M1;
  }
  else
  {
    return SYS_SYS_MODE_LTE_NB1;
  }
}

/*===========================================================================

FUNCTION cmpsm_config_rat_is_active

DESCRIPTION
  if rat poi image from NV is same is 
  current camped rat

DEPENDENCIES
  none

RETURN VALUE
  boolean TRUE if camped and config rat is same
  
SIDE EFFECTS
  none
===========================================================================*/


boolean cmpsm_config_rat_is_active(sys_radio_access_tech_e_type rat)
{
  boolean res = FALSE;
  if( cmpsm_ptr()->poi_nb1_enabled == TRUE &&
  	  rat == SYS_RAT_LTE_NB1_RADIO_ACCESS )
  {
    res = TRUE;
  }
  else if(cmpsm_ptr()->poi_m1_enabled == TRUE &&
  	       rat == SYS_RAT_LTE_M1_RADIO_ACCESS )
  {
    res = TRUE;
  }

  return res;
}

/*===========================================================================

FUNCTION cmpsm_wait_for_cond

DESCRIPTION
   condition check for data

DEPENDENCIES
  none

RETURN VALUE
 wait for the time before crash
  
SIDE EFFECTS
  none
===========================================================================*/

void cmpsm_wait_for_cond(uint8 mask,uint8 cond)
{
  uint32 count = 0;
  uint8 data = ALL_PSM_BIT;
  do
  {
    pm_app_misc_get_cfg(PM_APP_MISC_CFG_BOOT_OPTION, &data);
	
	if(((data & mask) != cond )&& (count < 10))
	{
	  DALSYS_BusyWait(100);  /* it should be 1 us sec  */
	  count++;
	  continue;
	}
	else if (((data & mask) != cond ))
	{
	  CM_ERR_FATAL_0("cmpsm_wait_for_cond() crash the reg is not set!!");
	}
	CM_MSG_HIGH_2( "[Debug]cmpsm_wait_for_cond fig cond = %d and data= %d ",cond,data); 
    break;	
		
  }while(1);
	
}

#ifndef FEATURE_REF_TRACKER
#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
/*===========================================================================
FUNCTION cmpsm_process_uart_baud_rate_ind

DESCRIPTION
 Set the baud rate

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

void cmpsm_process_uart_baud_rate_ind(uint16 uart_baud_rate)
{
  cmpsm_ptr()->uart_baud_rate = uart_baud_rate;
  CM_MSG_HIGH_1("Received baud rate = %d ",uart_baud_rate);
}

#endif
#endif

/*===========================================================================

FUNCTION cmpsm_wait_for_msec

DESCRIPTION
   wait for msec

DEPENDENCIES
  none

RETURN VALUE
 none
  
SIDE EFFECTS
  none
===========================================================================*/

void cmpsm_wait_for_msec(uint32 msec)
{
	uint32 i =0;
	uint32 j =0;
	
	for(j=0;j<msec;j++)
	{
	  for(i=0;i<10;i++)
	  {
	    DALSYS_BusyWait(100);  /* it should be 1 msec sec  */
		cm_kick_dog();
	  }
	}
	
}

/*===========================================================================

FUNCTION cmpsm_get_psm_status

DESCRIPTION
   get_psm_status

DEPENDENCIES
  none

RETURN VALUE
 none
  
SIDE EFFECTS
  none
===========================================================================*/

cm_psm_enter_req_status_e_type cmpsm_get_psm_status(void)
{ 
  return cmpsm_ptr()->psm_enter_req_status;
}

/*===========================================================================

FUNCTION cmpsm_set_high_activity_timer

DESCRIPTION
   set_high_activity_timer

DEPENDENCIES
  none

RETURN VALUE
 none
  
SIDE EFFECTS
  none
===========================================================================*/

void cmpsm_set_high_activity_timer (uint32 time)
{
  CM_MSG_HIGH_0("Starting high activity timer");  
  (void) timer_clr(&cmpsm_high_activity_timer,T_SEC);
  timer_set(&cmpsm_high_activity_timer,time,0,T_SEC);
}

