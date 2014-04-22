/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C A L L   M A N A G E R   C L I E N T   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Client Interface Block

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmclient_list_init() must be call to initialize the client list object
  before any other cmclient_list_xxx() function declared in this module is
  being called.


Copyright (c) 1991 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.CT.1.0/Main/modem_proc/stubs/src/cmclient.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#include "cm.h"
#include "cmdiag.h"

/*===========================================================================

FUNCTION cm_client_act

DESCRIPTION
  Activate the client object, so that the client will be notified
  of call manager events (for which it is registered).

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  Add the client object to the client list.

===========================================================================*/
cm_client_status_e_type cm_client_act(

    cm_client_id_type          client
    /* Client ID */
)
{
  return CM_CLIENT_OK;
}

/*===========================================================================

FUNCTION cm_client_init

DESCRIPTION
  Initializing a client object.

  This function can be called more than once with the same client type. Each
  call will result in an ADDITIONAL client id being allocated for the client
  type. It's upto the caller to keep track of the several client ids that
  they own and use them appropriately.

  Note that in order to get notified of CM events a client still needs
  to do the following:
  1. Register its callback functions and specific events with each functional group
  2. Activate its client object with cm_client_act()

DEPENDENCIES
  none

RETURN VALUE
  request staus

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_client_init(
    cm_client_type_e_type           client_type,
            /* Client type */
    cm_client_id_type               *client_id_ptr
            /* Client ID pointer*/
)
{
  /* This is really goofy, but don't have anywhere else to put this since
     there is no CM task running. */
  cmdiag_init();

  return CM_CLIENT_OK;
}

/*===========================================================================

FUNCTION cm_client_ph_reg

DESCRIPTION
  This function allows a client to register the following phone-related
  callbacks and events with CM:

   1. Callback function to receive registered phone events
   2. Register and de-register phone events
   3. Callback function to receive other clients' phone-command errors.

   If a client registers a callback function to receive phone event
   notification, it also needs to register one or more phone events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of call events, a client needs to call on this API twice- once for each
   range of call events.

  If a client registers a callback function to receive other clients'
  phone-command errors, it will receive notifications on all possible
  phone-command errors caused by other clients. A client cannot register to
  receive only a subset of phone-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' phone-command errors, it can set
  ph_cmd_err_func = NULL. However, if the ph_event_func is set to NULL,
  all registered phone events are also de-registered and the event_reg_type,
  from_ph_event, and to_ph_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_client_ph_reg(

    cm_client_id_type           client,
        /* client id */

    cm_ph_event_f_type          *ph_event_func,
        /* Pointer to a callback function to notify the client of phone
        ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_ph_event_e_type          from_ph_event,
        /* register from this event (including) */

    cm_ph_event_e_type          to_ph_event,
          /* to this event (including) */

    cm_ph_cmd_err_f_type        *ph_cmd_err_func
      /* pointer to a callback function to notify the client of phone
      ** command errors of OTHER clients */
)
{
  return CM_CLIENT_OK;
}

/*===========================================================================

FUNCTION cm_mm_client_call_reg

DESCRIPTION
  This function allows a client to register the following call-related
  callbacks and events with CM:

   1. Callback function to receive registered call events
   2. Register and de-register call events
   3. Callback function to receive other clients' call-command errors.

   If a client registers a callback function to receive call event
   notification, it also needs to register one or more call events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of call events, a client needs to call on this API twice- once for each
   range of call events.

  If a client registers a callback function to receive other clients'
  call-command errors, it will receive notifications on all possible
  call-command errors caused by other clients. A client cannot register to
  receive only a subset of call-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' call-command errors, it can set
  call_cmd_err_func = NULL. However, if the call_event_func is set to NULL,
  all registered call events are also de-registered and the event_reg_type,
  from_call_event, and to_call_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_call_reg(

    cm_client_id_type           client,
        /* client id */

    cm_mm_call_event_f_type     *call_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_call_event_e_type        from_call_event,
        /* register from this event (including) */

    cm_call_event_e_type        to_call_event,
          /* to this event (including) */

    cm_mm_call_cmd_err_f_type   *call_cmd_err_func
      /* pointer to a callback function to notify the client of call
      ** command errors of OTHER clients */
)
{
  return CM_CLIENT_OK;
}

/*===========================================================================

FUNCTION cm_mm_client_ss_reg

DESCRIPTION
  This function allows a client to register the following serving system-related
  callbacks and events with CM:

   1. Callback function to receive registered serving system events
   2. Register and de-register serving system events
   3. Callback function to receive other clients' serving system-command errors.

   If a client registers a callback function to receive serving system event
   notification, it also needs to register one or more serving system events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of serving system events, a client needs to call on this API twice- once for each
   range of serving system events.

  If a client registers a callback function to receive other clients'
  serving system-command errors, it will receive notifications on all possible
  serving system-command errors caused by other clients. A client cannot register to
  receive only a subset of serving system-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' serving system-command errors, it can set
  ss_cmd_err_func = NULL. However, if the ss_event_func is set to NULL,
  all registered serving system events are also de-registered and the event_reg_type,
  from_ss_event, and to_ss_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_ss_reg(

    cm_client_id_type           client,
        /* client id */

    cm_mm_ss_event_f_type       *ss_event_func,
        /* Pointer to a callback function to notify the client of serving system
           ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_ss_event_e_type          from_ss_event,
        /* register from this event (including) */

    cm_ss_event_e_type          to_ss_event,
          /* to this event (including) */

    cm_ss_cmd_err_f_type        *ss_cmd_err_func
      /* pointer to a callback function to notify the client of serving system
      ** command errors of OTHER clients */
)

{
  return CM_CLIENT_OK;
}

/*===========================================================================

FUNCTION cm_ph_cmd_oprt_mode

DESCRIPTION
  Command CM to put phone in a specified operating mode, such as offline,
  online, LPM, etc.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_oprt_mode(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  sys_oprt_mode_e_type    oprt_mode
    /* Switch to this operating mode */

)
{
    return FALSE;
}