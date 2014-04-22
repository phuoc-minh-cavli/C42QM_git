/*==============================================================================

                H S U _ O S _ A B S T R A C T _ B U S _ E X T . H
                                

GENERAL DESCRIPTION
  High Speed USB OS abstraction layer bus externalized API declarations.
  The APIs declared in this file are intended for the layers above the OS
  abstraction layer, and not for the HS-USB core driver.

EXTERNALIZED FUNCTIONS
  Externalized to outside of the OS abstraction layer:
    hsu_os_bus_read_from_phy
    hsu_os_bus_write_to_phy
    hsu_os_bus_set_lpm_feature_is_active

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2008-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/


/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_os_abstract_bus_ext_int.h#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
08/25/09  rg      Initial version

==============================================================================*/
#ifndef _HSU_OS_ABSTRACT_BUS_EXT_INT_H
#define _HSU_OS_ABSTRACT_BUS_EXT_INT_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/
#include "customer.h"
#include "hsu_os_abstract_bus_ext.h"
#include "port_int.h"
#include "timetick.h"
#include "hsu_timer.h"

/*==============================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
    
/* Events : Add bit mapped events below.
** Code for USB disconnect event.
*/
typedef enum {
  HSU_OS_BUS_DISCONNECT_EVT = 0x00000001,
  HSU_OS_BUS_CONNECT_EVT    = 0x00000002,
} hsu_os_bus_ext_isr_evt_type;

typedef enum
{
  // Link Power Events
  PWR_EVNT_SW,
  PWR_EVNT_PHY_L2,
  PWR_EVNT_PHY_P3,

  // QMP PHY Events
  QMP_PHY_LFPS_IRQ,

  // QUSB2 PHY Events
  QUSB2_PHY_IRQ,

  // Generic wakeup event
  GENERIC_WAKEUP_EVNT,

  PWR_EVNT_MAX = 0x7FFFFFFF,
} pwr_evnt_t;
/*==============================================================================
  Typedefs
==============================================================================*/

/* Interrupt resource info type. */
typedef struct 
{
  /* The index of the core which owns this interrupt. */
  uint32            core_index;

  /* User supplied interrupt handler.                 */
  interrupt_handler isr_func;

  /* Argument to be passed to the ISR function.       */
  void*             isr_arg;

  /* User supplied deferred service routine handler.  */
  jthread_func      dsr_func;

  /* Argument to be passed to the DSR function.       */
  void*             dsr_arg;

  /* events posted  */
  uint32            event;
} interrupt_resource_info_type;

typedef struct  
{
  boolean
    lpm_is_active;

  boolean 
    is_in_low_power_mode;

  /* This flag reflects the most recent state of the HS-USB port
  ** before entering LPM (low-power mode). It is set to True if the HS-USB 
  ** port is attached when we start entering LPM. Otherwise, it is set to FALSE.
  ** When the HS-USB core is not in LPM, this flag has no meaning.
  */
  boolean
    cable_is_connected_during_lpm;

  /* This flag is TRUE when the LPM entry timer is armed or when
  ** it has been expired, but its callback (the LPM entry function)
  ** has not been executed yet.
  */
  boolean
    lpm_entry_is_pending;

  /* This is the value which the lpm_entry_timer is initialized to. */
  uint32
    lpm_entry_timer_value_ms;

  /* This timer is armed when the core enters into suspend state.
  ** When the timer expires, the core switches to low power mode.
  ** The timer is canceled if the core exits the suspend state
  ** before it expires.
  **
  ** The goal of this timer is to avoid unnecessary entry into low power
  ** mode if the core enters into suspend state only for a short time.
  */
  hsu_timer_handle_type
    lpm_entry_timer;

  /* This timer is used for USBCV remote-wakeup tests.
  ** It's initialized with the explicit_remote_wakeup_delay
  ** that is read from the NV.
  **
  ** This timer is armed when the core enters into suspend state.
  ** When the timer expires, the core issues a remote wakeup.
  ** The timer is canceled if the core exits the suspend state
  ** before it expires.
  **
  ** When the explicit remote wakeup delay NV value is greater than 0, this 
  ** timer is used for ensuring that at least NV value of
  ** milliseconds pass between each suspend and remote wakeup events.
  **
  ** A regular REX timer is not appropriate for this timer, because
  ** REX timers are not supported during power collapse and TCXO
  ** shutdown modes.
  */
  hsu_timer_handle_type
    remote_wakeup_timer;

  /* This timer is armed when the core exits from low-power mode
  ** due to an asynchronous interrupt. When this happens, a USB cable
  ** connection status check is scheduled, in order to decide whether
  ** to return to low-power mode or not.
  */
  hsu_timer_handle_type
    cable_status_check_timer;

  /* exit_lpm is different between remote wakeup and resume, so we want
  ** to know what cause the exiting from LPM 
  */
  boolean
    was_lpm_exit_due_to_remote_wakeup;

  /* if remote wakeup is pending due to exit lpm, this flag will be set to true
  ** and this flag should be cleared in exit lpm.
  */
  boolean
    is_remote_wakeup_pending;

  /* remote wakeup suspend event time stamp in milliseconds */
  timetick_type 
    remote_wakeup_suspend_time_stamp_ms;

  /* Enter LPM callback */
  void (*enter_lpm_handler)(uint32 core_index, 
                            boolean is_called_by_lpm_timer,
                            pwr_evnt_t power_event);

  /* Exit LPM callback*/
  void (*exit_lpm_handler)(uint32 core_index, pwr_evnt_t power_event);

  /* Port is attached callback */
  boolean (*port_is_attached)(uint32 core_index);

  /* Should cable status check needs to be performed? */
  boolean enable_cable_status_check;
} hsu_lpm_info_type;

/*===========================================================================
FUNCTION HSU_OS_BUS_SET_LPM_FEATURE_IS_ACTIVE

DESCRIPTION
This function is called during config_selector_init() and sets the value of
lpm_feature_is_active according to the NV parameter hs_usb_disable_lpm.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_os_bus_set_lpm_feature_is_active(boolean enable_lpm);

/*===========================================================================
FUNCTION HSU_OS_BUS_SEND_EVENT_TO_CORE

DESCRIPTION
  This function sets events to event variable in interrupt resources 
  and calls the USB interrupt handler.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_send_event_to_core
(
  uint32                      core_index, 
  hsu_os_bus_ext_isr_evt_type event
);

#endif
