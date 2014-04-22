/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        M M G S D I   P K C S   L I B R A R Y


GENERAL DESCRIPTION

  This file contains Library function that MMGSDI client can call to access
  various PKCS related functions


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

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdilib_pkcs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/16   sp      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "mmgsdilib_pkcs.h"
#include "mmgsdiutil.h"
#include "mmgsdilib.h"
#include "mmgsdi.h"
#include "intconv.h"
#include "mmgsdilib_common.h"
#include "uim_msg.h"

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*============================================================================
   FUNCTION:      MMGSDI_PKCS_QUEUE_START_POTENTIAL_SILENT_FILE_CHANGE

   DESCRIPTION:
     In some specific cards available in the field when BIP session or SMS PP
     download is triggered, there is a potential for certain files to be update
     without any indication to MMGSDI, so this function will notify MMGSDI when
     any BIP session or SMS PP download starts, so that MMGSDI can perform the
     appropriate action. This will potentially save on redundant SELECTs when
     accessing the EFs.

   PARAMETERS:
     client_id:         Client ID of the requesting client.
     slot:              Slot on which potential file change occurs.

   DEPENDENCIES:
     None

   LIMITATIONS:
     NONE

   RETURN VALUE:
     mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
    MMGSDI_ERROR:            Generic error.

   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_pkcs_queue_start_potential_silent_file_change(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot
)
{
  mmgsdi_task_cmd_type                      *task_cmd_ptr   = NULL;
  mmgsdi_start_silent_file_change_req_type  *msg_ptr        = NULL;
  int32                                      total_mem_len  = 0;
  mmgsdi_return_enum_type                    mmgsdi_status  = MMGSDI_ERROR;

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Validate the slot */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(slot,
                                    MMGSDI_SLOT_1,
                                    MMGSDI_SLOT_3);

  /* Memory allocation for msg_ptr and check for allocation */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr,
                                     int32touint32(total_mem_len));
  if(task_cmd_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.start_silent_file_change_req;

  /* Populating mmgsdi_map_sessions_to_slots header */
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.session_id        = 0;
  msg_ptr->request_header.slot_id           = slot;

  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_start_potential_silent_file_change */

