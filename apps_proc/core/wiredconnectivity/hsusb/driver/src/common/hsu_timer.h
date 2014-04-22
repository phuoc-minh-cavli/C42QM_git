/*==============================================================================

  HSU Timer Abstraction
  
  GENERAL DESCRIPTION

  EXTERNALIZED FUNCTIONS

  INITALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_timer.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  08/01/12  ac      Initial version
==============================================================================*/
#ifndef __HSU_TIMER_H___
#define __HSU_TIMER_H___

#include <stdio.h>
#include "timer.h"

#ifdef FEATURE_HS_USB_PRE_SILICON_BU
// For convenience in adjusting a divisible time factor if necessary
#define HSU_EMULATION_TIME_FACTOR 1
#else // ~FEATURE_HS_USB_PRE_SILICON_BU
#define HSU_EMULATION_TIME_FACTOR 1
#endif // ~FEATURE_HS_USB_PRE_SILICON_BU

#define HSU_TOTAL_NUM_TIMER 72

#define HSU_TIMER_UNINITIALIZED_HANDLE 0

typedef uint32 hsu_timer_feature_bit_field;

/* most typical end-user can leave this data structure as all zeros */
/* to use multiple features, use '|' to pass multiple parameters */
#define HSU_TIMER_DEFAULT_TIMER                          (hsu_timer_feature_bit_field)(0) /* non-deferrable timer */
#define HSU_TIMER_IS_TIMER_DEFERRABLE                    (hsu_timer_feature_bit_field)(1 << 0)   /* set to 1 for deferrable tmer */
#define HSU_TIMER_DISALLOW_REPRIME_WHILE_TIMER_ACTIVE    (hsu_timer_feature_bit_field)(1 << 1)   /* set to 1 for error fatal when prime API is called while timer is running */
/* Below two flags are both set, "CB in HSU_AL_TASK with giant mutex" will take priority */
#define HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX     (hsu_timer_feature_bit_field)(1 << 2)   /* registered callback will be invoked in HSU_AL_TASK with jsafe_enter (giant mutex) */
#define HSU_TIMER_CB_IN_HSU_AL_TASK_WITHOUT_GIANT_MUTEX  (hsu_timer_feature_bit_field)(1 << 3)   /* registered callback will be invoked in HSU_AL_TASK without jsafe_enter (giant mutex) */

typedef enum
{
  HSU_TIMER_SUCCESS                            = 0,

  HSU_TIMER_INVALID_TIMER_ID_INPUT             = 0xFFFC,
  HSU_TIMER_NO_MORE_EMPTY_SLOT_TO_CREATE_TIMER = 0xFFFD,
  HSU_TIMER_ALREADY_EXIST                      = 0xFFFE,
  HSU_TIMER_FAIL                               = 0xFFFF,
} hsu_timer_result_e;

typedef enum
{
  HSU_TIMER_CID__UNKNOWN,
  HSU_TIMER_CID__ENCAP_TX_ZLP,
  HSU_TIMER_CID__ENCAP_RX_ZLP,
  HSU_TIMER_CID__ECM_TX_HANDLE_COMPLETE,
  HSU_TIMER_CID__RAWDATA_MODE_ESC_SEQ,
  HSU_TIMER_CID__SER_FLUSH_TX,
  HSU_TIMER_CID__SER_TX_COMPLETE,
  HSU_TIMER_CID__LOG_DBG_1,
  HSU_TIMER_CID__LOG_DBG_2,
  HSU_TIMER_CID__GPIO_REMOTE_WAKEUP_DEBUG,
  HSU_TIMER_CID__RM_WAKEUP,
  HSU_TIMER_CID__LPM_ENTRY,
  HSU_TIMER_CID__CABLE_STATUS_CHECK,
  HSU_TIMER_CID__RESUME_PERIPHERALS,
  HSU_TIMER_CID__THREAD_SLEEP,
  HSU_TIMER_CID__SPS_SM_DEBUG,
  HSU_TIMER_CID__HOST_RDY_HI_DELAY,
  HSU_TIMER_CID__IS_UL_BAM_EMPTY,
  HSU_TIMER_CID__SPS_SM_DISCONNECT_DELAY,
  HSU_TIMER_CID__BAM_SM_FLOW_DISABLING_FROM_SUSPEND,
  HSU_TIMER_CID__BAM_SM_FORCE_SHUTDOWN_TMR,
  HSU_TIMER_CID__AUTO_HOST_RDY_AFTER_HOST_RDY_FALL,
  HSU_TIMER_CID__FUNCTION_WAKE_TNOTIFICATION,
#ifdef CI_MERGE
  HSU_TIMER_CID__AUDIO_TX,
  HSU_TIMER_CID__AUDIO_RX,
#endif
  HSU_TIMER_CID__CPU_PROFILE,
#ifdef FEATURE_HS_USB_TEST_SPOOF_DISCONNECT
  HSU_TIMER_CID__SPOOF_DISCONNECT,
#endif // FEATURE_HS_USB_TEST_SPOOF_DISCONNECT
  HSU_TIMER_CID_BOOTUP_DELAY_CONNECT,
#ifdef CI_MERGE
  HSU_TIMER_CID_CDC_GSI_DBG_TIMER_1,
  HSU_TIMER_CID_CDC_GSI_DBG_TIMER_2,
  HSU_TIMER_CID_CDC_GSI_DBG_TIMER_3,
#endif
  HSU_TIMER_CID__SER_RX_THROTTLE,
} hsu_timer_client_id_type;

typedef struct
{
  hsu_timer_client_id_type client_id;
  
  timer_type timer;
  timer_group_type timer_group;
  timetick_type timer_val;

  uint32  feature_mask;
  boolean is_allocated; /* indicates whether the timer slot is in allocated or in use */

  timer_t1_cb_type usr_cb;
  timer_cb_data_type usr_data;

  uint32 duplicate_entry_count;
} hsu_timer_type;

typedef struct
{
  hsu_timer_type timers[HSU_TOTAL_NUM_TIMER]; /* timer ID is implicitly equal to the index of the timer */
} hsu_timer_ctx_type;

typedef uint32 hsu_timer_handle_type;

hsu_timer_handle_type hsu_timer_init(hsu_timer_client_id_type client_id, hsu_timer_feature_bit_field feature_mask, timer_t1_cb_type usr_cb, timer_cb_data_type usr_data);
void hsu_timer_set(hsu_timer_handle_type *timer_hnd, timetick_type expiry_time, timer_unit_type time_unit);
void hsu_timer_cancel(hsu_timer_handle_type *timer_hnd);
timetick_type hsu_timer_get_time(hsu_timer_handle_type *timer_hnd, timer_unit_type time_unit);
void hsu_timer_deinit(hsu_timer_handle_type *timer_id);

#endif /* __HSU_TIMER_H___ */
