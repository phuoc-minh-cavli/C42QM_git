/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   H I D D E N   K E Y   S T U B   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains stubs for hidden key related support and processing for MMGSDI.

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

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_hidden_key_stubs.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/12/16   vdc     Initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "uim_msg.h"
#include "mmgsdi.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
  FUNCTION:      MMGSDI_HIDDENKEY_OPERATION

  DESCRIPTION:
    This function will handle the Hidden Key requests

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_hidden_key_operation (
  const mmgsdi_pin_operation_req_type * req_ptr
)
{
  (void)req_ptr;

  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_hidden_key_operation */


/* ============================================================================
   FUNCTION:      MMGSDI_HIDDEN_KEY_RSP_OPERATION

   DESCRIPTION:
     This function will build the response to the Hiddenkey Command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_hidden_key_rsp_operation (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  (void)uim_rsp_ptr;

  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_hidden_key_rsp_operation */


/*===========================================================================
   FUNCTION MMGSDI_HIDDEN_KEY_CHECK_VALIDNESS

   DESCRIPTION:
     This function checks if the Hidden Key data is valid
     (This is the ASCII format input Hiddenkey provided by clients)

   PARAMS:
    hkey_data_ptr: Contains the Hidden Key length and data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE
      boolean:  TRUE  Hidden Key data is valid
                FALSE Hidden Key data is not valid

   SIDE EFFECTS:
     None
 ===========================================================================*/
boolean mmgsdi_hidden_key_check_validness(
  const mmgsdi_data_type * hkey_data_ptr
)
{
  (void)hkey_data_ptr;

  return FALSE;
} /* mmgsdi_hidden_key_check_validness */


/*===========================================================================
  FUNCTION MMGSDI_HIDDEN_KEY_REFRESH_PROCESS_FCN

  DESCRIPTION
    This function will handle the Refresh FCN of USIM/CSIM Hiddenkey

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    Hiddenkey Event will be sent to clients
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_hidden_key_refresh_process_fcn(
  const mmgsdi_event_data_type *event_ptr,
  mmgsdi_file_enum_type         file
)
{
  (void)event_ptr;
  (void)file;

  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_hidden_key_refresh_process_fcn */


/*=============================================================================
FUNCTION:      MMGSDI_HIDDEN_KEY_VALIDATE_HKEY_DATA

DESCRIPTION
  This is the validation function for checking the Hidden Key data received
  from the card

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
============================================================================*/
mmgsdi_return_enum_type mmgsdi_hidden_key_validate_hkey_data (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type status,
  const mmgsdi_data_type *data_ptr
)
{
  (void)session_id;
  (void)status;
  (void)data_ptr;

  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_hidden_key_validate_hkey_data */


/*===========================================================================
   FUNCTION MMGSDI_HIDDEN_KEY_IS_VALID

   DESCRIPTION:
     This function checks if the Hidden Key data is valid
     (This is the ASCII format input Hiddenkey provided by clients)

   PARAMS:
    hkey_data_ptr: Contains the Hidden Key length and data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE
      boolean:  TRUE  Hidden Key data is valid
                FALSE Hidden Key data is not valid

   SIDE EFFECTS:
     None
 ===========================================================================*/
boolean mmgsdi_hidden_key_is_valid(
  const mmgsdi_data_type * hkey_data_ptr
)
{
  (void)hkey_data_ptr;

  return FALSE;
} /* mmgsdi_hidden_key_is_valid */