/*===========================================================================
                      DSMSGRSEND.C
DESCRIPTION
This file defines 3GPP MSGR APIs to external clients outside DS.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "amssassert.h"
#include "msgr.h"
#include "dsmsgr.h"
#include "data_msg.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

boolean ds_msgr_snd_msg_send
(
  msgr_umid_type         msgtype,
  msgr_hdr_struct_type   *msg,
  uint32                 msglen
)
{
  return ds_msgr_snd_msg_send_ext(msgtype, MSGR_DS_MSGRRECV, msg, msglen);
} /* dsmsgrsnd_msg_send */

boolean ds_msgr_snd_msg_send_ext
(
  msgr_umid_type         msgtype,
  msgr_tech_module_type  tech_mod_type,
  msgr_hdr_struct_type   *msg,
  uint32                 msglen 
)
{
  errno_enum_type  ret_val = E_FAILURE;

  DS_ASSERT(msg != NULL);

  /* Initialize the header info */
  msgr_init_hdr_inst(msg, tech_mod_type, msgtype, 0);
  ret_val = msgr_send(msg, msglen);
  if ( ret_val != E_SUCCESS)
  {
    DS_ERROR_LOG_2("Unable to send msg to tech =d err:=d",
                    tech_mod_type,ret_val);
    return FALSE;
  }
  return TRUE;
} /* dsmsgrsnd_per_subs_msg_send_ext */


