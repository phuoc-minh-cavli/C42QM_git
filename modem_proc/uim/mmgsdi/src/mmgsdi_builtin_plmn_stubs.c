/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   B U I L T I N   P L M N   S T U B S   D E F I N I T I O N S

                      A N D   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains stubs functions for built in PLMN list functionality.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016, 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_builtin_plmn_stubs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/18   tq      UIM interface optimization
06/22/16   bcho    initial version


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "mmgsdi_builtin_plmn.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
FUNCTION MMGSDI_PROCESS_SET_BUILTIN_PLMN_LIST

DESCRIPTION
  This function queues a response for handling set builtin plmn list in
  the cnf

PARAMETERS:
  mmgsdi_set_builtin_plmn_list_req_type req_ptr: request pointer

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_set_builtin_plmn_list(
  mmgsdi_set_builtin_plmn_list_req_type * req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_process_set_builtin_plmn_list */


/*================================================================
FUNCTION  MMGSDI_GET_BUILTIN_PLMN_LIST

DESCRIPTION:
  This function is called to read builtin PLMN list stored in EFS.

  INPUT PARAMETERS:
    uint32 file_size_ptr    : file size pointer
    uint8  file_buffer_pptr : address of file buffer pointer

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_get_builtin_plmn_list(
  uint32 * file_size_ptr,
  uint8 ** file_buffer_pptr)
{
  (void)file_size_ptr;
  (void)file_buffer_pptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_get_builtin_plmn_list */


/*================================================================
FUNCTION  MMGSDI_STORE_BUILTIN_PLMN_LIST

DESCRIPTION:
  This function is called to store builtin plmn list in EFS provided by QMI NAS
  interface.

  INPUT PARAMETERS:
    mmgsdi_set_builtin_plmn_list_req_type set_builtin_plmn_list_req_ptr: request pointer

DEPENDENCIES:
  It triggers Refresh FCN on OPLMNwACT after storing builtin PLMN list only if
  USIM/SIM app has reached SUB READY.

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_store_builtin_plmn_list(
  const mmgsdi_builtin_plmn_list_type      *set_builtin_plmn_list_req_ptr
)
{
  (void)set_builtin_plmn_list_req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_store_builtin_plmn_list */


/*================================================================
FUNCTION  MMGSDI_BUILTIN_PLMN_LIST_IS_CONFIGURED

DESCRIPTION:
  This function is called to check biltin PLMN list is configured in EFS.

INPUT PARAMETERS:
  None

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_builtin_plmn_list_is_configured(
  void
)
{
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_builtin_plmn_list_is_configured */
