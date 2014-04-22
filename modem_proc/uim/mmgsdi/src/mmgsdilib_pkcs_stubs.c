/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                   M M G S D I   P K C S   S T U B   L I B R A R Y


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

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdilib_pkcs_stubs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/16   sp      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmgsdilib_pkcs.h"
#include "mmgsdilib_common.h"

/*============================================================================
   FUNCTION:      MMGSDI_PKCS_QUEUE_START_POTENTIAL_SILENT_FILE_CHANGE

   DESCRIPTION:
     This is stub function.
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
     NONE.

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
  (void)client_id;
  (void)slot;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_start_potential_silent_file_change */

