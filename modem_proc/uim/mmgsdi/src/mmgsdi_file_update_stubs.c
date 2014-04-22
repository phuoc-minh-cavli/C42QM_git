/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


             M M G S D I   F I L E   U P D A T E   S T U B   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the stub for all file update related commands

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

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_file_update_stubs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/16   sp      Fix low level compiler warnings
01/20/16   ar      Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "intconv.h"
#include "mmgsdi_file_update.h"
#include "mmgsdi.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*=============================================================================

                       FUNCTION DEFINATIONS

=============================================================================*/


/* =============================================================================
  FUNCTION:      MMGSDI_PROCESS_FILE_UPDATE_REGISTRATION

  DESCRIPTION:
    This function is stub for mmgsdi_process_file_update_registration

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_NOT_SUPPORTED:          The function is not supported on this PL

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_process_file_update_registration
(
  mmgsdi_session_register_file_update_req_type * req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_process_file_update_registration */


/* ============================================================================
  FUNCTION:      MMGSDI_PROCESS_FILE_UPDATE_REGISTRATION_RSP

  DESCRIPTION:
    This function is stub for mmgsdi_process_file_update_registration_rsp

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_NOT_SUPPORTED:          The function is not supported on this PL

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_process_file_update_registration_rsp
(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_process_file_update_registration_rsp */


/* ============================================================================
  FUNCTION:      MMGSDI_BUILD_FILE_UPDATE_EVENT

  DESCRIPTION:
    This function is stub for mmgsdi_build_file_update_event

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
void mmgsdi_build_file_update_event(
  mmgsdi_return_enum_type              status,
  const mmgsdi_write_extra_info_type * write_extra_ptr,
  mmgsdi_session_id_type               session_id,
  mmgsdi_slot_id_enum_type             slot
)
{
  (void)status;
  (void)write_extra_ptr;
  (void)session_id;
  (void)slot;
} /* mmgsdi_build_file_update_event */


/* ============================================================================
  FUNCTION MMGSDI_FILE_UPDATE_BUILD_AND_SEND_EVT

  DESCRIPTION
    This function is stub for mmgsdi_file_update_build_and_send_evt

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void mmgsdi_file_update_build_and_send_evt(
  mmgsdi_evt_session_notify_type       notify_type,
  mmgsdi_access_type                   access_type,
  const mmgsdi_write_extra_info_type * write_extra_ptr)
{
  (void)notify_type;
  (void)access_type;
  (void)write_extra_ptr;
} /* mmgsdi_file_update_build_and_send_evt */