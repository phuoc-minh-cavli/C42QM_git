/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  SETUP MENU STUB


GENERAL DESCRIPTION :
  This source file contains dummy implmentation of functions handled in estk_setup_menu.c

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights
Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/estk/src/stub/estk_setup_menu_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   gs      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
#include "estk.h"
#include "estk_priv.h"

/*===========================================================================

FUNCTION: estk_process_setup_menu_req

DESCRIPTION:
  Dummy Functionality

PARAMETERS:
  void

DEPENDENCIES:
  None

RETURN VALUE:
  gstk_status_enum_type

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
estk_result_enum_type estk_process_setup_menu_req(
  const estk_cmd_type *cmd_ptr
)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
} /*estk_process_setup_menu_req*/

/*===========================================================================

FUNCTION: estk_process_menu_sel_req

DESCRIPTION:
  Dummy Functionality

PARAMETERS:
  void

DEPENDENCIES:
  None

RETURN VALUE:
  gstk_status_enum_type

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
estk_result_enum_type estk_process_menu_sel_req(
  const estk_cmd_type *cmd_ptr
)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
} /*estk_process_menu_sel_req*/
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/
