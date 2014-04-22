/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        M M G S D I   S A P   S T U B   L I B R A R Y


GENERAL DESCRIPTION

  This file contains Library function that MMGSDI client can call to access
  various SAP functions


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdilib_sap_stubs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/16   bcho    Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "mmgsdilib.h"
#include "mmgsdi.h"


/*===========================================================================
FUNCTION MMGSDI_SAP_CONNECT

DESCRIPTION
  This function attempts to perform a SAP connect.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_connect(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  (void)client_id;
  (void)card_slot;
  (void)sap_mode;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_sap_connect */


/*===========================================================================
FUNCTION MMGSDI_SAP_CONNECT_EXT

DESCRIPTION
  This function attempts to perform a SAP connect. It also provides an option
  for the client to specify a condition for the SAP connection request.
  SAP connection request with proceed based on this condition.

PARAMS:
  client_id:           Client Id of the caller
  card_slot:           Physical slot for multiple slot targets.
  sap_mode:            Client/Server.
  condition:           Condtion to check for active call status.
  response_cb_ptr:     Response callback.
  client_ref:          User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_connect_ext(
    mmgsdi_client_id_type                 client_id,
    mmgsdi_slot_id_enum_type              card_slot,
    mmgsdi_sap_mode_enum_type             sap_mode,
    mmgsdi_sap_conn_condition_enum_type   condition,
    mmgsdi_callback_type                  response_cb_ptr,
    mmgsdi_client_data_type               client_ref)
{
  (void)client_id;
  (void)card_slot;
  (void)sap_mode;
  (void)condition;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_sap_connect_ext */


/*===========================================================================
FUNCTION MMGSDI_SAP_DISCONNECT

DESCRIPTION
  This function attempts to perform a SAP disconnect.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  disconnect_mode: Graceful/Immediate.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_disconnect(
    mmgsdi_client_id_type               client_id,
    mmgsdi_slot_id_enum_type            card_slot,
    mmgsdi_sap_mode_enum_type           sap_mode,
    mmgsdi_disconnect_mode_enum_type    disconnect_mode,
    mmgsdi_callback_type                response_cb_ptr,
    mmgsdi_client_data_type             client_ref)
{
  (void)client_id;
  (void)card_slot;
  (void)sap_mode;
  (void)disconnect_mode;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_disconnect */


/*===========================================================================
FUNCTION MMGSDI_SAP_GET_ATR

DESCRIPTION:
  This is the API used to retrieve the ATR Information which pertains to the
  card(s) currently inserted in the ME.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_get_atr(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref)
{
  (void)client_id;
  (void)card_slot;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_get_atr */


/*===========================================================================
FUNCTION MMGSDI_SAP_SEND_APDU

DESCRIPTION
  This function will send an APDU to the SAP Client.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  channel_id:      The channel id.
  apdu_data:       The APDU to send to the card.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_send_apdu(
  mmgsdi_client_id_type          client_id,
  mmgsdi_slot_id_enum_type       card_slot,
  int32                          channel_id,
  mmgsdi_sap_send_apdu_data_type apdu_data,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref)
{
  (void)client_id;
  (void)card_slot;
  (void)channel_id;
  (void)apdu_data;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_send_apdu */


/*===========================================================================
FUNCTION MMGSDI_SAP_POWER_ON

DESCRIPTION
  This function attempts to perform a SAP power on

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_power_on(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  (void)client_id;
  (void)card_slot;
  (void)sap_mode;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_power_on */


/*===========================================================================
FUNCTION MMGSDI_SAP_POWER_OFF

DESCRIPTION
  This function attempts to perform a SAP power off

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_power_off(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  (void)client_id;
  (void)card_slot;
  (void)sap_mode;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_power_off */


/*===========================================================================
FUNCTION MMGSDI_SAP_CARD_READER_STATUS

DESCRIPTION
  This function attempts to perform a SAP card reader status

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_card_reader_status(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  (void)client_id;
  (void)card_slot;
  (void)sap_mode;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_card_reader_status */


/*===========================================================================
FUNCTION MMGSDI_SAP_RESET

DESCRIPTION
  This function attempts to perform a SAP RESET

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_reset(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  (void)client_id;
  (void)card_slot;
  (void)sap_mode;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_reset */

