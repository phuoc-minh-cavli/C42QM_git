/*===========================================================================


              G S T K    T E R M I N A L    R E S P O N S E



GENERAL DESCRIPTION
  This source file contains the dummy functionality of 
  gstk_process_tr.c
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_process_tr_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/16   gs      Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_terminal_rsp.h"
#include "gstkutil.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_process_raw_terminal_response

DESCRIPTION
  This function packs raw terminal response and sends response to the card
  through the UIM server.

PARAMETERS
  raw_term_resp_ptr: [Input] Pointer to raw terminal response that is required
                             to be processed and sent to UIM

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
gstk_status_enum_type gstk_process_raw_terminal_response (
  gstk_client_raw_term_resp_type   *raw_term_resp_ptr )
{
  (void) raw_term_resp_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
}