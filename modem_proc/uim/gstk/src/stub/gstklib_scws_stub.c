/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   L I B R A R Y


GENERAL DESCRIPTION

  This source file contains the stub functions for SCWS that are exposed to GSTK
  client as well as internal GSTK clients.


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstklib_scws_stub.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   gs      Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION   gstk_scws_ch_status_req

DESCRIPTION
  Dummy Functionality

PARAMETERS
  client_id  : client id
  ch id     :  Channel id
  ch status :  Channel status

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  GSTK_UNSUPPORTED_COMMAND

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type  gstk_scws_ch_status_req(
  gstk_client_id_type           client_id,
  uint32                        ch_id,
  gstk_scws_ch_state_enum_type  ch_status
)
{
  (void) client_id;
  (void) ch_id;
  (void) ch_status;
  
  return GSTK_UNSUPPORTED_COMMAND;
}/* gstk_scws_ch_status_req*/

/*===========================================================================
FUNCTION   gstk_scws_data_avail_req_type

DESCRIPTION
 Dummy Functionality

PARAMETERS
  client_id      : client id
  ch id          : Channel status
  data           : Data from the scws socket
  data_remaining : How much data is left

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  GSTK_UNSUPPORTED_COMMAND

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type  gstk_scws_data_avail_req_type(
  gstk_client_id_type      client_id,
  uint32                   ch_id,
  gstk_generic_data_type   data,
  uint32                   data_remaining

)
{
  (void)client_id;
  (void)ch_id;
  (void)data;
  (void)data_remaining;

  return GSTK_UNSUPPORTED_COMMAND;
}/* estk_scws_data_avail_req_type */

/*===========================================================================
FUNCTION   gstk_scws_open_ch_rsp

DESCRIPTION
  Library function called by client to report SCWS Open Ch response.

PARAMETERS
  client_id  : client id
  cmd_ref_id : This is the gstk command reference id in the
               gstk header information sent to the client in the callback
  ch id      : Channel id
  ch status  :  Channel status

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  GSTK_UNSUPPORTED_COMMAND

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type  gstk_scws_open_ch_rsp(
  gstk_client_id_type           client_id,
  uint32                        cmd_ref_id,
  uint32                        ch_id,
  gstk_scws_ch_state_enum_type  ch_status
)
{
  (void)client_id;
  (void)cmd_ref_id;
  (void)ch_id;
  (void)ch_status;

  return GSTK_UNSUPPORTED_COMMAND;
}/* gstk_scws_open_ch_rsp*/

/*===========================================================================
FUNCTION   gstk_scws_close_ch_rsp

DESCRIPTION
  Dummy Functionality

PARAMETERS
  client_id  : client id
  cmd_ref_id : This is the gstk command reference id in the
               gstk header information sent to the client in the callback
  ch id      : Channel id
  ch status  :  Channel status

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  GSTK_UNSUPPORTED_COMMAND
  
COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type  gstk_scws_close_ch_rsp(
  gstk_client_id_type           client_id,
  uint32                        cmd_ref_id,
  uint32                        ch_id,
  gstk_scws_ch_state_enum_type  ch_status
)
{
  (void)client_id;
  (void)cmd_ref_id;
  (void)ch_id;
  (void)ch_status;
  
  return GSTK_UNSUPPORTED_COMMAND;
}/* gstk_scws_close_ch_rsp*/

/*===========================================================================
FUNCTION   gstk_scws_send_data_rsp

DESCRIPTION
  Dummy Functionality

PARAMETERS
  client_id     : client id
  cmd_ref_id    : This is the gstk command reference id in the
                  gstk header information sent to the client in the callback
  ch id         : Channel status
  write_success : Whether the write was a success

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
    GSTK_UNSUPPORTED_COMMAND

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type  gstk_scws_send_data_rsp(
  gstk_client_id_type      client_id,
  uint32                   cmd_ref_id,
  uint32                   ch_id,
  boolean                  write_success
)
{
  (void)client_id;
  (void)cmd_ref_id;
  (void)ch_id;
  (void)write_success;
  
  return GSTK_UNSUPPORTED_COMMAND;
}/* gstk_scws_send_data_rsp*/

