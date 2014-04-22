/*==============================================================================
  FILE:         uim_hotswap.c

  OVERVIEW:     FIle conatins stub functions for UIM HOTSWAP feature.

  DEPENDENCIES: N/A

                Copyright (c) 2016 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uim_hotswap_stub.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when        who        what, where, why
------      ----       ---------------------------------------------------------
06/23/17    ks         Compilation error fix when remote SIM is enabled.
06/03/16    sam        Stub out Hotswap related functionality
04/25/16    dd         Disable Hotswap functionality for IoE flavor
==============================================================================*/
#include "uimglobals.h"
#include "uimdrv_main.h"

/**
 * DEFINITIONS OF EXTERNAL FUNTIONS
 */

/** 
* Stubs function for UIM_READ_HOTSWAP_DEBOUNCE_CONFIG
*/
void uim_read_hotswap_debounce_config(uim_instance_global_type *uim_ptr)
{
  (void)uim_ptr;
  return;
}/*uim_read_hotswap_debounce_config*/


/**
 * This procedure initializes the  queues and timers for UIM
 * HOTSWAP. It should be called only once, at powerup time.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_init(uim_instance_global_type *uim_ptr)
{
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);
  uim_ptr->hotswap.hotswap_first_ins_class = uim_ptr->state.first_inst_class;
  uim_ptr->hotswap.card_status = UIM_CARD_INSERTED;
  uim_ptr->hotswap.hotswap_card_status = UIM_CARD_INSERTED;
  UIMDRV_MSG_HIGH_3(uim_ptr->id,
  	 "uim_hotswap_init  m_hs_enable  0x%x, hotswap_card_removed_suspicious  =0x%x, hotswap_flaky_switch_support =0x%x",
         uim_hw_if.intctrl[uim_ptr->id].m_hs_enable,
         uim_ptr->hotswap.hotswap_card_removed_suspicious,
         uim_ptr->hotswap.hotswap_flaky_switch_support);
} /* uim_hotswap_init */

/**
*  Stubs function for UIM_SUSPICIOUS_CARD_REM_SIG signal.
*/
boolean uim_handle_card_removed_suspicious_sig(rex_sigs_type *sig_mask_ptr,
                                            rex_sigs_type *imask_ptr,
                                            uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  (void)uim_ptr;
  return FALSE;
}/*uim_handle_card_removed_suspicious_sig*/

/**
*  Stubs function for UIM_HOTSWAP_CMD_CARD_REM_SIG signal.
*/
boolean uim_handle_card_removed_sig(rex_sigs_type *sig_mask_ptr,
                                           rex_sigs_type *imask_ptr,
                                           uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  (void)uim_ptr;
  return FALSE;
}/*uim_handle_card_removed_sig*/


/**
*  Stubs function for UIM_HOTSWAP_CMD_CARD_INS_SIG signal.
*/
boolean uim_handle_card_inserted_sig(rex_sigs_type *sig_mask_ptr,
                                            rex_sigs_type *imask_ptr,
                                            uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  (void)uim_ptr;
  return FALSE;
}/*uim_handle_card_inserted_sig*/

/**
 * Reset the UIM global to its intial value caused by HOTSWAP
 * and re-configure the UARTDM for HOTSWAP.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_reset_slot(uim_instance_global_type *uim_ptr)
{
  /* Reset the necessary UIM globals */
  uim_reset_globals(uim_ptr);

  /* Re-configure the UARTDM */
  uim_reconfig_uartdm(uim_ptr);
}/* uim_hotswap_reset_slot */


/**
 * Process card inserted signal in UIM task.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_hotswap_process_card_inserted_sig(
  uim_instance_global_type *uim_ptr)
{
  (void) uim_ptr;
  return FALSE;
} /* uim_hotswap_process_card_inserted_sig */


/**
 * Process card removed signal in UIM task.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_hotswap_process_card_removed_sig(
  uim_instance_global_type  *uim_ptr)
{
  (void) uim_ptr;
  return FALSE;
} /* uim_hotswap_process_card_removed_sig */


/**
 * HOTSWAP ISR for GPIO interrupt.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_gpio_isr(uim_instance_global_type *uim_ptr)
{
  (void) uim_ptr;
  return;
} /* uim_hotswap_gpio_isr */


/**
 * Retrieves the real time card status of given slot.
 *
 * @param uim_slot Slot number.
 *
 * @return uim_rt_status_enum Real time HOTSWAP status.
 */
uim_rt_status_enum  uim_get_rt_status(uim_slot_type  uim_slot)
{
  (void) uim_slot;
  return UIM_RT_STATUS_NOT_SUPPORTED;
}/* uim_get_rt_status */


/**
 * Disable the hotswap interrupt for given slot
 *
 * @param uim_slot Slot number
 */
void uim_disable_hotswap_interrupt(uim_slot_type uim_slot)
{
  (void) uim_slot;
  return;
}/* uim_disable_hotswap_interrupt */


/**
 * Query the real-time HOTSWAP status
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return uim_card_swap_enum_type
 */
uim_card_swap_enum_type uim_hotswap_query_card_status(
  uim_instance_global_type *uim_ptr)
{
  (void) uim_ptr;
  return UIM_CARD_INSERTED;
} /* uim_hotswap_query_card_status */


/**
 * Mask the hotswap interrupt.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_mask_interrupt(uim_instance_global_type *uim_ptr)
{
  (void) uim_ptr;
  return;
} /* uim_hotswap_mask_interrupt */


/**
 * Clear the hotswap interrupt.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_clear_interrupt(uim_instance_global_type *uim_ptr)
{
  (void) uim_ptr;
  return;
} /* uim_hotswap_clear_interrupt  */


/**
 * Unmask the hotswap interrupt.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_unmask_interrupt(uim_instance_global_type *uim_ptr)
{
  (void) uim_ptr;
  return;
} /* uim_hotswap_unmask_interrupt */


/**
 * Query the hotswap capability.
 *
 * @param uim_slot Slot number.
 *
 * @return boolean True if slot have the hotswap capability
 */
boolean uim_is_hotswap_enabled_slot( uim_slot_type uim_slot )
{
  (void) uim_slot;
  return  FALSE;
} /* uim_is_hotswap_enabled_slot */


/**
 * DEFINITIONS OF INTERNAL FUNTIONS
 *
 */
/**
 * Send the card removed cmd to uim task.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_hotswap_send_card_removed_cmd(
  uim_instance_global_type *uim_ptr)
{
  (void) uim_ptr;
  return FALSE;
}/* uim_hotswap_send_card_removed_cmd */

/**
 * HOTSWAP ISR for the PMIC\GPIO used to communicate with the
 * UIM.
 *
 * @param uim_ptr Pointer to the global data common.
 * @param hos_need_to_query Request the current status.
 * @param hos_new_card_status New HOTSWAP status.
 */
void uim_hotswap_isr(uim_instance_global_type *uim_ptr,
                     boolean                   hos_need_to_query,
                     uim_card_swap_enum_type   hos_new_card_status)

{
  (void)uim_ptr;
  (void)hos_need_to_query;
  (void)hos_new_card_status;
  return;
}

/**
 * Disable the Hotswap functionality in UIM sw.
 * @return boolean 
 */
boolean uim_hotswap_disabled_in_sw (void)
{
  /* True - Means that Hotswap feature is disabled in SW*/
  /* False - Means that Hotswap feature is enabled in SW*/
  return TRUE;
} /* uim_hotswap_disabled_in_sw */

/**
 * Set hotswap signal to UIM task.
 * @return boolean 
 */
boolean  uim_hotswap_set_signal(
  uim_card_swap_enum_type   hos_new_card_status,
  uim_instance_global_type *uim_ptr)
{
  (void)uim_ptr;
  return FALSE;
}/* uim_hotswap_set_signal */


