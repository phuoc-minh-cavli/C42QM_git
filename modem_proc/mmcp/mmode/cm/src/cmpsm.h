#ifndef CMPSM_H
#define CMPSM_H

/*===========================================================================

          PSM    H E A D E R    F I L E

DESCRIPTION
  This file contains all the definitions necessary for the psm module.

Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/cm/src/cmpsm.h#1 $

when       who     what, where, why
--------   ---    -------------------------------------------------------
09/19/15   skk     Initial release.

===========================================================================*/



#include "sys.h"
#include "cmi.h"
#include "cmidbg.h"
#include "cm_msgr_msg.h"
#include "fs_public.h"
#include "cmefs.h"
#include "comdef.h"

#include "sd.h"
#include "cmss.h"
#include "timer.h"

/* PSM OPTIMIZATION MASKS
*/
#define CM_PSM_OPT_MASK_NONE            0       /*No optimizations enabled*/
#define CM_PSM_OPT_MASK_QUICK           BM(0)   /*Enter PSM directly, no need to check modem state*/
#define CM_PSM_OPT_MASK_OOS             BM(1)   /*Indicate OOS state to PSM daemon so that it can trigger PSM*/
#define CM_PSM_OPT_MASK_LMTD_SRV        BM(2)   /*Indicate Ltd srv to PSM daemon so that it can trigger PSM*/
#define CM_PSM_OPT_MASK_DEEP_SLEEP      BM(3)   /*Enter modem to deep-sleep mode if PSM duration less than threshold*/

/* Default Values */
#define CM_PSM_MAX_OOS_SCANS                     0xffff
#define CM_PSM_DEFAULT_MAX_OOS_SCANS                        (2)
#define CM_PSM_DEFAULT_MIN_OOS_TIME                          (60)         /* 60 seconds */
#define CM_PSM_DEFAULT_MAX_OOS_TIME                          (200)        /* 200 seconds */

#define CM_PSM_DEFAULT_RANDOMIZATION_WINDOW 5
#define CM_PSM_LMTD_SRV_IND_WAIT_TIME       5

#define CM_PSM_MAX_UPTIME                      0xFFFFFFFF
#define CM_PSM_MAX_UPTIME_64                      0xffffffffffffffff 


#define CM_PSM_DEEP_SLEEP_EARLY_WAKEUP_TIME    3 /* 3 seconds */

#define CM_PSM_POI_NB1_ENABLE        BM(0)
#define CM_PSM_POI_M1_ENABLE         BM(1)

#define CM_PSM_TINY_IMAGE_APP_ENABLE     BM(0)
#define CM_PSM_DEV_DWNLD_ENABLE      BM(1)

#define CMPSM_HIGH_ACT_TIMER_VAL 2

typedef PACKED struct PACKED_POST{
  uint8          version;

  uint32         psm_opt_mask;
  /* PSM Optimization mask, CM_PSM_OPT_MASK_xxx */

  uint16        max_oos_full_scans;
  /* Max full scans to wait before declaring SYS_PSM_STATUS_OOS to clients */

  uint32         psm_duration_due_to_oos;
  /* PSM duration due to outage, in seconds */

  uint16        psm_randomization_window;
  /* PSM wakeup randomization window,in seconds */

  uint16        max_oos_time;
  /* Max time in seconds to wait before declaring SYS_PSM_STATUS_OOS to clients */

  uint16         early_wakeup_time;
  /* Device to wake up this time early from PSM to account for boot-up and acquisition delay*/

}cm_psm_config_ext_s_type;

typedef PACKED struct PACKED_POST{
  uint8          version;

  uint8          rtc_enabled;
  /* Real Time Clock enabled, this could be enabled by default 
  ** or could be enabled through client interface once timer info ready*/

  uint8          timer_info_timeout;
  /* time-out to handle subscription change */

  uint8          reserved;
  /* Reserved */

}cm_psm_rtc_config_s_type;

typedef PACKED struct PACKED_POST{
    
  uint8          version;

  uint32         psm_inactivity_timeout;
  /* PSM inactivity timeout */

  uint32         psm_duration_due_to_inactivity;
  /* PSM duration due to inactivity, in seconds */

  uint16         catm1_early_wakeup_time_poi;
  /* Early wake-up time for catm1 page only*/
  
  uint16         nb1_early_wakeup_time_poi;
  /* Early wake-up time for nb1 page only*/
  
  uint16          nb1_early_wakeup_time;
  /* Early wake-up time for nb1*/

  uint8           poi_image_enabled;


  uint16          sns_early_wakeup_time;
  /*early wake up time for sensor */
  uint16          sns_awake_time;
  /* forcefully send the motion ind to app for debug purpose */
  uint8          force_motion_sns_ind;
  
  /* Denotes Caliberation of RTC timer */
  uint8          psm_rtc_cal;
 
  uint8          reserved[78];

}cm_psm_config_ext_2_s_type;

typedef struct{
    
    uint32         psm_inactivity_timeout;
    /* PSM inactivity timeout */
    
    uint32         psm_duration_due_to_inactivity;
    /* PSM duration due to inactivity, in seconds */

}cm_psm_inactivity_s_type;
/* Enum of Protocals/Modules to which CM sends request for PSM enter
*/
typedef enum
{
  /** @cond
  */
  CM_PSM_ENTER_REQ_STATUS_NONE=-1, /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

  CM_PSM_ENTER_STATUS_IDLE,
  /**< Request not yet posted to protocals.
    *  Indicates no psm enter request is in progress.
   */

  CM_PSM_ENTER_STATUS_WAIT_READY_RSP,
  /**< Request posted to protocals, psm enter request is in progress */

  CM_PSM_ENTER_STATUS_WAIT_READY_IND,
  /**< Some module failed request, waiting for ready ind */

  CM_PSM_ENTER_STATUS_WAIT_ENTER_RSP,
  /**< Waiting for PSM ENTER response */

  CM_PSM_ENTER_STATUS_DEEP_SLEEP,
  /**< Device entering deep-sleep mode(kind of PSM w/o shutdown)*/
  
  CM_PSM_ENTER_STATUS_ENTER_IN_PROGRESS,
  /**< All protocals voted OK, Moving to PSM is in progress */

  CM_PSM_ENTER_STATUS_COMPLETE,
  /**< All protocals/modules responded READY and moved to PSM */

  CM_PSM_ENTER_STATUS_WAIT_MMGSDI_RSP,
  /**< Waiting for MMGSDI write response */

  /** @cond
  */
  CM_PSM_ENTER_REQ_STATUS_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

} cm_psm_enter_req_status_e_type;

/* Type for PSM configuration notified to clients
*/

typedef  struct {

  boolean        psm_enabled;
    /* Indicate if PSM is enabled or not */

  sys_radio_access_tech_e_type  active_rat;

  uint32         active_timer_value;

  uint32         periodic_update_timer_value;

  uint32         gprs_ready_timer_value;

  boolean        edrx_supported;
}cm_psm_cfg_info_s_type;

typedef  struct {

  sys_psm_status_e_type                   psm_enter_prot_status;
  /* psm enter state info at protocal/module level */

  cm_client_psm_ready_req_f_type*         psm_ready_req_func_ptr;
  /* PSM ready callback to be registered*/

  cm_client_psm_enter_req_f_type*         psm_enter_req_func_ptr;
  /* PSM enter callback to be registered*/

  uint8*                                  context;

  uint32                                  context_size;
  
}cm_module_s_type;

typedef struct
{

  cm_psm_cfg_info_s_type            last_psm_cfg_info;

  cm_psm_config_ext_s_type         psm_cfg_ext_info;
  /* psm cfg info */


  cm_module_s_type                 module_info[CM_PSM_MODULE_END+1];
  /* psm state info at protocal/module level */

  cm_psm_config_s_type             psm_cfg_info;
  /* psm cfg info */

  cm_psm_rtc_config_s_type         psm_rtc_info;
  /* psm rtc info */

  cm_psm_inactivity_s_type         psm_inactivity_cfg;

  uint64                            psm_enter_uptime;
  /* PSM enter duration/uptime*/
  uint64                            psm_periodic_update_uptime;
  /* Next periodic update time/uptime*/

  uint64                          psm_status_periodic_update_val_dbg;
  /* psm_periodic_update_uptime - uptime, used for debugging purposes , doesnt not take into account early_wakeup_time*/
  
  dword                           psm_lmtd_service_ind_uptime; 

  uint64                            psm_full_boot_uptime;
  /* Next full boot uptime*/

  dword                            psm_oos_uptime; 
#ifdef FEATURE_REF_TRACKER
  uint64                            psm_sns_boot_time;
#endif
  uint64                            psm_page_only_boot_time;

  uint16                           catm1_early_wakeup_time_poi;

  uint16                           nb1_early_wakeup_time_poi;

  uint16                           nb1_early_wakeup_time;

  uint16                           full_service_lost_count; 

  uint8                            psm_uim_opt_enabled;

  boolean                          psm_due_to_oos_allowed;

  boolean                          psm_context_changed;

  boolean                          poi_nb1_enabled;
  boolean                          poi_m1_enabled;

  cm_psm_enter_req_status_e_type     psm_enter_req_status;
      /* Status of psm enter request */
  boolean                          psm_context_restore_needed;

  boolean                          psm_context_retained;

  sys_radio_access_tech_e_type     psm_rat;

  uint8                            edrx_ptw;
#ifdef FEATURE_REF_TRACKER
  boolean                          sensor_image_boot;
  uint16                           sns_early_wakeup_time;
  uint16                           sns_awake_time;
#endif
#if defined (FEATURE_BUILD_PAGEONLY_IMAGE ) || defined(FEATURE_BUILD_SENSORONLY_IMAGE)
  /* Indicates if shutdown or reboot needed*/
  uint8                            psm_entry_type;

#endif  

uint16 uart_baud_rate;

boolean is_next_full_image;
/* Denotes Caliberation of RTC timer */
uint8                            psm_rtc_cal;


}cm_psm_info_s_type;


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
void cmpsm_init(void);

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
cm_psm_info_s_type *cmpsm_ptr(void);

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
void cmpsm_msgr_cmd_proc( const void   *cmd_ptr );

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
void cmpsm_process_ready_rsp( cm_psm_module_e_type module, const void   *rpt_ptr);

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
void cmpsm_process_ready_ind( cm_psm_module_e_type module, const void   *rpt_ptr);

#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
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
);
#endif


#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/*===========================================================================
FUNCTION cm_psm_xrpt_handle

DESCRIPTION
  This function should be called to process the PSM reports from MC & HDR like
  psm_ready_rsp, psm_ready_ind.

DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean                        cm_psm_xrpt_handle
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
);
#endif

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
void cmpsm_process_lpm( void );


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
void cmpsm_process_non_online_deep_sleep( void );


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
extern  void  cmpsm_proc_full_srv_lost
(
        sd_ss_e_type            ss,
        boolean                 is_scan_attempted
);

/*===========================================================================

FUNCTION cmpsm_proc_srv_ind

DESCRIPTION
   Process service indication from SD.


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
 );

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
void cmpsm_timer_proc(
  cm_timer_event_type    timer_event   /* Indicate specific timer event */
);

/*===========================================================================

FUNCTION cmpsm_set_lmtd_srv_ind_timer_on_card_error

DESCRIPTION
  Set limited service indication timer if no subscription is available.

DEPENDENCIES
  PSM object must have already been initialized with cmpsm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_set_psm_lmtd_srv_ind_timer( );

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
void cmpsm_process_online( void );

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

void cmpsm_handle_subs_not_avail(void);


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
boolean cmpsm_pwr_up_uim_opt_enabled_and_no_subs_avail( void );
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
boolean  cmpsm_due_to_oos_allowed( void );

/*===========================================================================
FUNCTION cmpsm_send_context_save_req

DESCRIPTION
  Send PSM context save request to mmgsdi.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmpsm_send_context_save_req(void);

/*===========================================================================
FUNCTION cmpsm_deep_sleep_timer_expiry_cb

DESCRIPTION
  Timer callback function used to process deep sleep timer expiry.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cmpsm_deep_sleep_timer_expiry_cb(timer_cb_data_type data);

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
void cmpsm_process_deep_sleep_timer_expiry();
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
boolean cmpsm_get_rtc_enabled();

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
void cmpsm_update_rtc_enabled(boolean rtc_enabled);

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
boolean cmpsm_read_context(void);

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
void cmpsm_inactivity_timer_start(void);

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
boolean cmpsm_config_rat_is_active(sys_radio_access_tech_e_type rat);


/*===========================================================================
FUNCTION cmsns_set_boot_up_time

DESCRIPTION
  it will set up the boot time for sensors

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_set_boot_up_time( uint32 boot_up_time);


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
void cmpsm_process_uart_baud_rate_ind(uint16 uart_baud_rate);


/*===========================================================================

FUNCTION cm_psm_sio_uart_init

DESCRIPTION
    It will init the uart 

DEPENDENCIES
   None

RETURN VALUE
  None
  
SIDE EFFECTS
  none

===========================================================================*/

void cm_psm_sio_uart_init(uint16 uart_baud);


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

void cmpsm_wait_for_msec(uint32 msec);

#ifndef FEATURE_MMODE_TX_21
/*===========================================================================
FUNCTION cmsns_set_smd_time_out_value

DESCRIPTION
  It will set up the SMD time out value, this value can be used 
  to decide incase of false motion,how much time UE should 
  wait before moving to PSM

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_set_smd_time_out_value(uint8 time_in_sec);
#endif

/*===========================================================================
FUNCTION cmsns_set_force_motion_enable

DESCRIPTION
  This should be used for testing purpose only, it simulate if APPS has 
  configured to enable motion or not, without load app
  should not be enable for commercialize build

  arguement --> motion enable 
                    TRUE --> enable motion in SOI and MD interrupt configure
                    FALSE --> disable motion in SOI and MD in

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_set_force_motion_enable(uint8 motion_enable);

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

cm_psm_enter_req_status_e_type cmpsm_get_psm_status(void);

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

void cmpsm_set_high_activity_timer(uint32 time);

#endif  /* CMPSM_H */


