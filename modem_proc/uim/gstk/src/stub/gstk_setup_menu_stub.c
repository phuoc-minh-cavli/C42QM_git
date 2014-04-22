/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   S E T U P   M E N U  S T U B


GENERAL DESCRIPTION
    This source file contains dummy implmentation of functions handled in gstk_setup_menu.c

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All  Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_setup_menu_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   gs     Initial Version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_proactive_cmd.h"

/*===========================================================================
FUNCTION GSTK_SETUP_MENU_REQ

DESCRIPTION
  Dummy Functionality

PARAMETERS
  command_ptr: [Input] Pointer to message parsed and sent to clients

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_setup_menu_req (
  gstk_proactive_cmd_type* command_ptr
)
{
  (void) command_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
}

/*===========================================================================
FUNCTION gstk_setup_menu_cnf

DESCRIPTION
  Dummy Functionality

PARAMETERS
  setup_menu_cnf_ptr: [Input] Pointer to message that is required to
                            be processed and sent to UIM

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_setup_menu_cnf (
  gstk_setup_menu_cnf_type* setup_menu_cnf_ptr
)
{
  (void) setup_menu_cnf_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_setup_menu_cnf */
