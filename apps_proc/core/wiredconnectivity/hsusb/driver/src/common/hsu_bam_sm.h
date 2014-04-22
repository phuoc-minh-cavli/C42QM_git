/*==============================================================================

  HSU BAM State Machine

  GENERAL DESCRIPTION

  EXTERNALIZED FUNCTIONS

  INITALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_bam_sm.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  08/15/13  ac      Initial version
==============================================================================*/


#ifndef _HSU_BAM_SM__H_
#define _HSU_BAM_SM__H_

//#include "rex.h"
#include "comdef.h"

#define HSU_BAM_SIG_SHFT 16

#define HSU_BAM_GET_SIG(sig_evt) \
  ((uint32)sig_evt >> HSU_BAM_SIG_SHFT)

#define HSU_BAM_GET_EVT(sig_evt) \
  ((uint32)sig_evt & 0xFFFF)

#define HSU_BAM_SET_SIG_EVT(sig, evt) \
  (((uint32)sig << HSU_BAM_SIG_SHFT) | (uint32)evt)

typedef enum 
{
  HSU_AP_SIDEBAND_SIG = 1,
  HSU_USB_BUS_SIG,
  HSU_IPA_SIG,
  HSU_FRAMEWORK_SIG,

  HSU_BAM_SM_NUM_SIGS = HSU_FRAMEWORK_SIG, /* must be assigned to last signal */
} hsu_bam_sm_sig_enum;

typedef enum
{
  /* HSU_AP_SIDEBAND_SIG */
  HSU_HOST_READY_RISE_EVT = 1,
  HSU_HOST_READY_FALL_EVT,
  
  /* HSU_USB_BUS_SIG */
  HSU_BUS_SUSPEND_EVT,
  HSU_BUS_RESUME_EVT,
  HSU_BUS_RESET_EVT,
  HSU_BUS_CONFIGURED_EVT,

  /* HSU_IPA_SIG */
  HSU_IPA_SUSPEND_COMPLETE_IND_EVT,
  HSU_IPA_DATA_ACTIVITY_EVT,

  /* HSU_FRAMEWORK_SIG */
  HSU_TIMER_EVT,

  HSU_BAM_SM_NUM_EVTS = HSU_TIMER_EVT, /* must be assigned to last event */

} hsu_bam_sm_evt_enum;

/* higher 16 bit is signal, lower 16 bit is event */
typedef uint32 hsu_bam_sig_evt_type;

/* callback type for device ready signal */
typedef void (*hsu_bam_sm_device_ready_cb)(boolean ready);
typedef void (*hsu_bam_sm_wakeup_host_cb)(void);

// +++++++ For primary state machine with ECM
/* External APIs */
// Main API to initialize the state machine
void hsu_bam_sm_init(void);

// For customer to register callbacks for MDM to AP signals
void hsu_bam_sm_register_cbs(hsu_bam_sm_device_ready_cb device_ready_cb, hsu_bam_sm_wakeup_host_cb wakeup_host_cb);

// For customer to invoke when AP raises host ready
void hsu_bam_sm_notify_host_ready(boolean ready);

// Main API to send signal and event pair to the state machine
void hsu_bam_sm_send_sig_evt(hsu_bam_sig_evt_type sig_evt);

// Specific HW initializations invoked by the ECM or OBEX layer
void hsu_bam_sm_hw_initial_configuration(void);

// Prevent sending data packets to client unless state machine is in flow enabled state
boolean hsu_bam_sm_is_rx_forward_to_app_allowed(void);
// ------- For primary state machine with ECM

// +++++++ For secondary state machine without ECM
// Main API to initialize the state machine
void hsu_no_ecm_bam_sm_init(void);

// For customer to register callbacks for MDM to AP signals
void hsu_no_ecm_bam_sm_register_cbs(hsu_bam_sm_device_ready_cb device_ready_cb, hsu_bam_sm_wakeup_host_cb wakeup_host_cb);

// For customer to invoke when AP raises host ready
void hsu_no_ecm_bam_sm_notify_host_ready(boolean ready);

// Main API to send signal and event pair to the state machine
void hsu_no_ecm_bam_sm_send_sig_evt(hsu_bam_sig_evt_type sig_evt);

// Specific HW initializations invoked by the ECM or OBEX layer
void hsu_no_ecm_bam_sm_hw_initial_configuration(void);

// Prevent sending data packets to client unless state machine is in flow enabled state
boolean hsu_no_ecm_bam_sm_is_rx_forward_to_app_allowed(void);
// ------- For secondary state machine without ECM
#endif /* _HSU_BAM_SM__H_ */

