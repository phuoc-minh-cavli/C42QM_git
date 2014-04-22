/*============================================================================
  FILE:         uim_supply_voltage_inidcation.c

  OVERVIEW:
   This file consist the apis to notify voltage change indications to QMI

  EXTERNALIZED FUNCTIONS:

  DEPENDENCIES: N/A

                Copyright (c) 2016 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uim_supply_voltage_indication.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when          who        what, where, why
--------      ----       ---------------------------------------------------------
05/19/16      sam        Typo correction
05/16/16      sam        Initial Revision
============================================================================*/

#include "uimdrv_msg.h"
#include "uim_p.h"
#include "qmi_uim.h"
#include "uimglobals.h"


/*===========================================================================
FUNCTION UIMDRV_WAIT_FOR_QMI_ACKNOWLEDGEMENT

DESCRIPTION
  Waits on the required acknowledgement from QMI_UIM that is inturn waiting on
  the REQ to come in from the AP.
===========================================================================*/
static void uimdrv_wait_for_qmi_acknowledgement
(
  uim_instance_global_type     *uim_ptr,
  uimdrv_qmi_acknowledgements_type  required_ack
)
{
  rex_sigs_type  sigs_received = 0;
  uint16         time_in_ms = 1000; /* NV driven approach for future use */

  if ( required_ack == UIMDRV_QMI_INVALID_IND_ACK ||
       required_ack == UIMDRV_QMI_IND_ACK_NOT_REQUIRED )
  {
    return;
  }

  UIMDRV_MSG_HIGH_1(uim_ptr->id,"UIMDRV waiting on QMI for ACK - 0x%x",required_ack);

  /* ACK awaited from QMI.
     The supplied callback to QMI while sending the Indication would match
     the awaited ack and received indication and sets the
     UIM_QMI_ACK_RECEIVED_SIG accordingly. */
  uim_ptr->uim_qmi_interaction.required_ack_from_qmi = required_ack;

  if(uim_ptr->uim_qmi_interaction.uimdrv_qmi_sanity_timer_ptr == NULL)
  {
    timer_def( &uim_ptr->uim_qmi_interaction.uimdrv_qmi_sanity_timer,
               &uim_ptr->task.always_on_timer_group,
               uim_ptr->tcb_ptr, UIM_QMI_SANITY_TIMER_EXP_SIG, NULL, 0);
    uim_ptr->uim_qmi_interaction.uimdrv_qmi_sanity_timer_ptr = &uim_ptr->uim_qmi_interaction.uimdrv_qmi_sanity_timer;
  }

  (void)rex_set_timer(&uim_ptr->uim_qmi_interaction.uimdrv_qmi_sanity_timer, time_in_ms);

  sigs_received = uim_pet_and_wait( UIM_QMI_SANITY_TIMER_EXP_SIG|UIM_QMI_ACK_RECEIVED_SIG, uim_ptr);

  UIMDRV_MSG_HIGH_1(uim_ptr->id,"Sigs received while waiting for ACK from QMI - 0x%x",sigs_received);

  /* Clearing the required ACK for
     We could be here due to
     a. The ACK being received.
     b. We are done waiting on the ACK due to sanity timer expiry of task stop sig */
  uim_ptr->uim_qmi_interaction.required_ack_from_qmi = UIMDRV_QMI_IND_ACK_NOT_REQUIRED;

  (void)rex_clr_timer(&uim_ptr->uim_qmi_interaction.uimdrv_qmi_sanity_timer);
  (void)rex_clr_sigs(uim_ptr->tcb_ptr, UIM_QMI_SANITY_TIMER_EXP_SIG);
  (void)rex_clr_sigs(uim_ptr->tcb_ptr, UIM_QMI_ACK_RECEIVED_SIG);
} /* uimdrv_wait_for_qmi_acknowledgement */


/*===========================================================================
FUNCTION UIMDRV_QMI_POWER_MANAGEMENT_CALLBACK

DESCRIPTION
  Callback to manage the acknowledgements from QMI that are related to UIM LDO.
  The acknowledgements are for the ldo indications that we sent to QMI_UIM.
===========================================================================*/
static void uimdrv_qmi_power_management_callback
(
  uim_slot_type  uim_slot,
  uimdrv_qmi_acknowledgements_type received_ack
)
{
  uim_instance_global_type *uim_ptr = NULL;
  uim_instance_enum_type uim_instance = (uim_instance_enum_type) uim_slot - 1;

  RETURN_IF_INSTANCE_INVALID(uim_instance);

  uim_ptr = uim_get_instance_ptr(uim_instance);

  if (received_ack == UIMDRV_QMI_INVALID_IND_ACK || NULL == uim_ptr)
  {
    return;
  }

  UIMDRV_MSG_HIGH_1(uim_ptr->id,"UIMDRV received ACK from QMI - 0x%x",received_ack);

  /* ACK received from QMI for an Indication that was sent.
     The supplied callback would match the required ack and received ack and
     sets the UIM_QMI_ACK_RECEIVED_SIG accordingly. */

  uim_ptr->uim_qmi_interaction.received_ack_from_qmi = received_ack;

  if ( uim_ptr->uim_qmi_interaction.required_ack_from_qmi == uim_ptr->uim_qmi_interaction.received_ack_from_qmi )
  {
    rex_set_sigs(uim_ptr->tcb_ptr, UIM_QMI_ACK_RECEIVED_SIG);
  }

} /* uimdrv_qmi_power_management_callback */


/*===========================================================================
FUNCTION UIM_NOTIFY_VOLTAGE_CHANGE_INDICATION

DESCRIPTION
  Notifies QMI of the voltage change indication
===========================================================================*/
void uim_notify_voltage_change_indication 
(
  uim_instance_global_type     *uim_ptr, 
  uimdrv_qmi_indications_type  indication
)
{
  UIMDRV_MSG_HIGH_1(uim_ptr->id,"UIM_POWER_OFF/ON_SLOT notified QMI with Indication - 0x%x",indication);
  if(UIMDRV_LDO_AWAITING_DEACTIVATION_IND == indication)
  {
    qmi_uim_supply_voltage_ind((uim_ptr->id + 1),
                               indication,
                               uimdrv_qmi_power_management_callback);
    uimdrv_wait_for_qmi_acknowledgement( uim_ptr, UIMDRV_LDO_AWAITING_DEACTIVATION_IND_ACK );
  }
  else
  {
    qmi_uim_supply_voltage_ind((uim_ptr->id + 1),
                                indication,
                                NULL);
  }
}/*uim_notify_voltage_change_indication*/

