/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   L I B R A R Y  S C W S


GENERAL DESCRIPTION

  This source file contains the library functions for SCWS that are exposed to GSTK
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

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstklib_scws.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/16   gs      Changes in TP based on RAT and App Type
06/23/16   gs      Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk.h"
#include "uim_msg.h"
#include "gstkutil.h"
#ifdef FEATURE_ESTK
#include "estk.h"
#endif /* FEATURE_ESTK */

/*===========================================================================
FUNCTION   gstk_scws_ch_status_req

DESCRIPTION
  Library function called by client for SCWS Ch status change.

PARAMETERS
  client_id  : client id
  ch id     :  Channel id
  ch status :  Channel status

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

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
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;
  gstk_slot_id_enum_type sim_slot_id    = GSTK_SLOT_AUTO;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  sim_slot_id = gstk_util_get_sim_slot_id(client_id);

  if(!GSTK_IS_VALID_SLOT_ID(sim_slot_id))
  {
     return GSTK_ERROR;
  }

  UIM_MSG_HIGH_3("gstk_scws_ch_status_req: ch_id=0x%x, ch_status=0x%x, RECOVERY=0x%x",
                 ch_id, ch_status, GSTK_IDLE_S_RECOVERY(sim_slot_id));

  if (GSTK_IDLE_S_RECOVERY(sim_slot_id))
  {
    return GSTK_ERROR;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id                        = 0; /* Not used */
  task_cmd_ptr->hdr.cli_user_data                     = 0;
  task_cmd_ptr->hdr.cmd                               = ESTK_SCWS_CH_STATUS_REQ_CMD;
  task_cmd_ptr->hdr.sim_slot_id                       = gstk_util_get_sim_slot_id(client_id);
  task_cmd_ptr->result                                = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.scws_ch_status_req.ch_id     = ch_id;
  task_cmd_ptr->cmd_data.scws_ch_status_req.ch_status = ch_status;
  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)ch_id;
  (void)ch_status;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
}/* gstk_scws_ch_status_req*/

/*===========================================================================
FUNCTION   gstk_scws_data_avail_req_type

DESCRIPTION
  Library function called by client to say if data is available on SCWS socket.

PARAMETERS
  client_id      : client id
  ch id          : Channel status
  data           : Data from the scws socket
  data_remaining : How much data is left
DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

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
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;
  gstk_slot_id_enum_type sim_slot_id    = GSTK_SLOT_AUTO;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  if (data.data_len <= 0)
  {
    UIM_MSG_ERR_1("invalid data len! 0x%x", data.data_len);
    return GSTK_BAD_PARAM;
  }

  if (data.data_buffer_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null Ptr!");
    return GSTK_BAD_PARAM;
  }

  sim_slot_id = gstk_util_get_sim_slot_id(client_id);

  if(!GSTK_IS_VALID_SLOT_ID(sim_slot_id))
  {
     return GSTK_ERROR;
  }

  UIM_MSG_HIGH_1("gstk_scws_data_avail_req: GSTK_IDLE_S_RECOVERY=0x%x",
                 GSTK_IDLE_S_RECOVERY(sim_slot_id));

  if (GSTK_IDLE_S_RECOVERY(sim_slot_id))
  {
    return GSTK_ERROR;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id                        = 0; /* Not used */
  task_cmd_ptr->hdr.cli_user_data                     = 0;
  task_cmd_ptr->hdr.cmd                               = ESTK_SCWS_DATA_AVAIL_REQ_CMD;
  task_cmd_ptr->hdr.sim_slot_id                       = gstk_util_get_sim_slot_id(client_id);
  task_cmd_ptr->result                                = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.scws_data_avail_req.ch_id    = ch_id;
  task_cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len = (uint32)data.data_len;
  task_cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = (uint8*)GSTK_CALLOC((dword)data.data_len);
  (void)gstk_memcpy((void*)task_cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr,
                    (void*)data.data_buffer_ptr,
                    (size_t)data.data_len,
                    (size_t)data.data_len,
                    (size_t)data.data_len);
  /* ESTK will free the pointer after copying the data to the receive buffer */

  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)ch_id;
  (void)data;
  (void)data_remaining;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
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
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

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
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;
  gstk_slot_id_enum_type    slot_id       = GSTK_SLOT_1;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  slot_id = gstk_util_get_sim_slot_id(client_id);

  UIM_MSG_HIGH_3("gstk_scws_open_ch_rsp: ch_id=0x%x, ch_status=0x%x, RECOVERY=0x%x",
                 ch_id,
                 ch_status,
                 GSTK_IDLE_S_RECOVERY(slot_id));

  if (GSTK_IDLE_S_RECOVERY(slot_id))
  {
    return GSTK_ERROR;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id                        = cmd_ref_id;
  task_cmd_ptr->hdr.cli_user_data                     = 0;
  task_cmd_ptr->hdr.cmd                               = ESTK_SCWS_OPEN_CH_RSP_CMD;
  task_cmd_ptr->hdr.sim_slot_id                       = slot_id;
  task_cmd_ptr->result                                = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.scws_open_ch_rsp.ch_id       = ch_id;
  task_cmd_ptr->cmd_data.scws_open_ch_rsp.ch_status   = ch_status;
  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)cmd_ref_id;
  (void)ch_id;
  (void)ch_status;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
}/* gstk_scws_open_ch_rsp*/

/*===========================================================================
FUNCTION   gstk_scws_close_ch_rsp

DESCRIPTION
  Library function called by client to report SCWS Close Channel response.

PARAMETERS
  client_id  : client id
  cmd_ref_id : This is the gstk command reference id in the
               gstk header information sent to the client in the callback
  ch id      : Channel id
  ch status  :  Channel status

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

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
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;
  gstk_slot_id_enum_type    slot_id       = GSTK_SLOT_1;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  slot_id = gstk_util_get_sim_slot_id(client_id);

  UIM_MSG_HIGH_3("gstk_scws_close_ch_rsp: ch_id=0x%x, ch_status=0x%x, RECOVERY=0x%x",
                 ch_id,
                 ch_status,
                 GSTK_IDLE_S_RECOVERY(slot_id));

  if (GSTK_IDLE_S_RECOVERY(slot_id))
  {
    return GSTK_ERROR;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id                        = cmd_ref_id;
  task_cmd_ptr->hdr.cli_user_data                     = 0;
  task_cmd_ptr->hdr.cmd                               = ESTK_SCWS_CLOSE_CH_RSP_CMD;
  task_cmd_ptr->hdr.sim_slot_id                       = slot_id;
  task_cmd_ptr->result                                = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.scws_close_ch_rsp.ch_id      = ch_id;
  task_cmd_ptr->cmd_data.scws_close_ch_rsp.ch_status  = ch_status;
  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)cmd_ref_id;
  (void)ch_id;
  (void)ch_status;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
#endif /* FEATURE_ESTK */
}/* gstk_scws_close_ch_rsp*/

/*===========================================================================
FUNCTION   gstk_scws_send_data_rsp

DESCRIPTION
  Library function called by client to report SCWS Send Data respose.

PARAMETERS
  client_id     : client id
  cmd_ref_id    : This is the gstk command reference id in the
                  gstk header information sent to the client in the callback
  ch id         : Channel status
  write_success : Whether the write was a success

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

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
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;
  gstk_slot_id_enum_type    slot_id       = GSTK_SLOT_1;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  slot_id = gstk_util_get_sim_slot_id(client_id);

  UIM_MSG_HIGH_3("gstk_scws_send_data_rsp: ch_id=0x%x, write_success=0x%x, RECOVERY=0x%x",
                 ch_id,
                 write_success,
                 GSTK_IDLE_S_RECOVERY(slot_id));

  if (GSTK_IDLE_S_RECOVERY(slot_id))
  {
    return GSTK_ERROR;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id                        = cmd_ref_id;
  task_cmd_ptr->hdr.cli_user_data                     = 0;
  task_cmd_ptr->hdr.cmd                               = ESTK_SCWS_SEND_DATA_RSP_CMD;
  task_cmd_ptr->hdr.sim_slot_id                       = slot_id;
  task_cmd_ptr->result                                = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.scws_send_data_rsp.ch_id      = ch_id;
  task_cmd_ptr->cmd_data.scws_send_data_rsp.is_success  = write_success;
  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)cmd_ref_id;
  (void)ch_id;
  (void)write_success;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
}/* gstk_scws_send_data_rsp*/

