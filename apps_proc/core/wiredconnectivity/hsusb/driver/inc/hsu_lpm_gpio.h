#ifndef HSU_LPM_GPIO_H
#define HSU_LPM_GPIO_H

/*=============================================================================

                                H E A D E R   F I L E

==============================================================================*/
/*
@file hsu_lpm_gpio.h
*/

/*=============================================================================
Copyright (c) 2013 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/
/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
  
$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_lpm_gpio.h#1 $
$DateTime: 2020/01/30 22:49:35 $
    
when     who  what, where, why
-------- ---  ---------------------------------------------------------
02/08/13 ag  Initial version
=============================================================================*/
/*==============================================================================
                        INCLUDE FILES
==============================================================================*/

#include "hsu_platform.h"
#include "hsu_os_abstract_bus.h"
/*=============================================================================
  Forward Declarations
=============================================================================*/

/*=============================================================================
  Constants
=============================================================================*/

/*=============================================================================
  Macros
=============================================================================*/

/*=============================================================================
  Typedefs
=============================================================================*/
typedef struct
{
  /* Is remote wakeup used */               boolean is_remote_wakeup_gpio_used;
  /* Is remote wakeup timeout used */       boolean is_remote_wakeup_timeout_tmr_used;

  /* Counter for remote wakeups driven */   uint32 hsu_remote_wakeup_counter;
  /* Counter for clear feature recieved */  uint32 hsu_clear_feature_counter;

  hsu_timer_handle_type remote_wakeup_debug_timer;
  timetick_type remote_wakeup_timeout_tmr_value;
  hsu_gpio_handle_ctx_type hsu_gpio_remote_wakeup_ctx;
}hsu_lpm_gpio_remote_wakeup_ctx_type;

/*=============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/
#ifdef FEATURE_HS_USB_F3

void hsu_lpm_gpio_remote_wakeup_gpio_init(void);
boolean hsu_lpm_gpio_is_gpio_remote_wakeup_required(void);

/*===========================================================================
FUNCTION HSU_LPM_HSIC_REMOTE_WAKEUP_GPIO_LOW

DESCRIPTION
  The function lowers the HSIC_REMOTE_WAKEUP_GPIO after we get a Resume due to Remote Wake up from AP

DEPENDENCIES
  GPIO should be properly initialized before use.

RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void hsu_lpm_gpio_remote_wakeup_gpio_low(void);

/*===========================================================================
FUNCTION HSU_LPM_HSIC_REMOTE_WAKEUP_GPIO_HIGH

DESCRIPTION
  The function raises the HSIC_REMOTE_WAKEUP_GPIO to signal Remote Wakeup to AP
DEPENDENCIES
  GPIO should be properly initialized before use.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void hsu_lpm_gpio_remote_wakeup_gpio_high(void);

/*===========================================================================
FUNCTION HSU_LPM_LOG_CLEAR_FEATURE_FOR_REMOTE_WAKEUP

DESCRIPTION
  Increment the Clear_Feature counter and cancel the gpio_remote_wakeup timeout timer if armed.

DEPENDENCIES
  Timer should be initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_lpm_gpio_log_clear_feature_for_remote_wakeup(void);

#endif /* FEATURE_HS_USB_F3 */

#endif /* HSU_LPM_GPIO_H */
