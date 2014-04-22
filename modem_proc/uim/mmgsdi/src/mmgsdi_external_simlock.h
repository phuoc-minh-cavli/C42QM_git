#ifndef MMGSDI_EXTERNAL_SIMLOCK_H
#define MMGSDI_EXTERNAL_SIMLOCK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I  E X T E R N A L  S I M L O C K  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains functions to perform MMGSDI specific external SIM
  Lock tasks.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_external_simlock.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/14/16   vdc     Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"

/*===========================================================================
                  F U N C T I O N   P R O T O T Y P E S
============================================================================*/

/* ==========================================================================
FUNCTION:      MMGSDI_EXTERNAL_SIMLOCK_SUB_OK

DESCRIPTION:
  This function can be called by a client to inform MMGSDI that the subscription
  is OK and it can proceed with publishing the subscription to all clients thus
  allowing the modem to move to full service

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE:
  mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_external_simlock_sub_ok(
  mmgsdi_session_id_type         session_id,
  boolean                        ok_for_subscription,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref
);

/*===========================================================================
FUNCTION MMGSDI_EXTERNAL_SIMLOCK_PROCESS_SUB_OK_REQ

DESCRIPTION
  This function, called in mmgsdi_process_command.
  It will determine if a subscription that is initialized successfully will
  get published to the rest of the modem.

DEPENDENCIES
  None

LIMITATIONS
  This command is only supported on provisioning subscriptions

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_NOT_SUPPORTED    - Subscription state doesn't support this command
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_external_simlock_process_sub_ok_req (
  const mmgsdi_session_sub_ok_req_type *sub_ok_req_ptr
);

/*===========================================================================
FUNCTION MMGSDI_EXTERNAL_SIMLOCK_PROCESS_SUB_OK_RSP

DESCRIPTION
  This function is called in mmgsdi_process_response.
  It serves mainly to pack the response and queue the confirmation

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_external_simlock_process_sub_ok_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr
);
#endif /* MMGSDI_EXTERNAL_SIMLOCK_H */

