/*================================================================================

DESCRIPTION


Copyright (c) 2013 - 2014, 2016 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_cat_common.c#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/15/18   tq      Migration of QMI UIM service to QCCI-QCSI
08/10/16   ar      Support for setting terminal capability from AP
05/26/16   sp      F3 logging improvements
03/08/16   vr      Common function for result tlv and send response
03/08/16   vr      Remove duplicated code between QMI CAT and QMI UIM
03/27/14   at      Usage of common QMI APIs from DS header
02/12/14   at      Additional check in the free transaction API
01/10/14   df      Use v2.0 diag macros
10/08/13   gm      Removed unused comments
10/04/13   gm      F3 message reduction
06/12/13   kb      Added QMI malloc and free wrapper functions
04/24/13   tkl     Hardening qmi_svc_new_cmd_buf against overflow
09/07/12   at      Initial version
=============================================================================*/

#include "uim_variation.h"
#include "customer.h"
#include "comdef.h"
#include "uim_msg.h"
#include "err.h"
#include "amssassert.h"
#include "qmi_uim_cat_common.h"
#include "modem_mem.h"
#include "ds_qmi_fw_common.h"
#include "ps_in.h"
#include <stringl/stringl.h>
#include "qmi_uim.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*---------------------------------------------------------------------------
  QMI common client state detail for QMI UIM or CAT message
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_common_client_state_type       common; // must be first since we alias
  int16                              instance;
  int16                              service_id;
} uimqmi_common_client_state_type;


/*===========================================================================
  FUNCTION UIMQMI_MALLOC

  DESCRIPTION
    The uimqmi_malloc calls modem_mem_calloc to request memory from the Modem Heap.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void * uimqmi_malloc
(
  uint32 size
)
{
  return modem_mem_calloc(1, (size), MODEM_MEM_CLIENT_UIM);
}/* uimqmi_malloc */


/*===========================================================================
  FUNCTION UIMQMI_FREE

  DESCRIPTION
    The uimqmi_free frees the pointer from the Modem Heap.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_free
(
  void * ptr
)
{
  modem_mem_free((void*)(ptr), MODEM_MEM_CLIENT_UIM);
}/* uimqmi_free */


/*===========================================================================
  FUNCTION UIMQMI_DEEP_COPY()

  DESCRIPTION
    This function allocates memory and duplicates the content of a buffer.
    This is a utility function to make deep copies.

  PARAMETERS
    dest : pointer to the destination buffer. Output value.
    src  : pointer to the source buffer
    size : size of the source buffer

  RETURN VALUE
    None. Errors are ignored.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_deep_copy
(
  void       ** dest,
  const void  * src,
  uint32        size
)
{
  if (dest != NULL)
  {
    *dest = NULL;
    if (src != NULL && size > 0)
    {
      *dest = uimqmi_malloc(size);
      if (*dest != NULL)
      {
        (void)memscpy(*dest, size, src, size);
      }
    }
  }
} /* uimqmi_deep_copy */


/*===========================================================================
  FUNCTION UIMQMI_MMGSDI_SLOT_TO_SLOT_INDEX()

  DESCRIPTION
    Determines the slot index from the MMGSDI slot id

  PARAMETERS
    mmgsdi_slot_id      : MMGSDI slot id
    slot_index_ptr      : output parameter containing slot index

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 uimqmi_mmgsdi_slot_to_slot_index
(
  mmgsdi_slot_id_enum_type       mmgsdi_slot_id,
  uint8                        * slot_index_ptr
)
{
  ASSERT(slot_index_ptr);

  switch(mmgsdi_slot_id)
  {
    case MMGSDI_SLOT_1:
      *slot_index_ptr = 0;
      break;

    case MMGSDI_SLOT_2:
      *slot_index_ptr = 1;
      break;

    case MMGSDI_SLOT_3:
      *slot_index_ptr = 2;
      break;

    default:
      UIM_MSG_ERR_1("Invalid slot: 0x%x", mmgsdi_slot_id);
      return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
}/* uimqmi_mmgsdi_slot_to_slot_index */


/*===========================================================================
  FUNCTION UIMQMI_CONVERT_MMGSDI_SLOT_TO_EFS_CONTEXT()

  DESCRIPTION
    Determines the uim common EFS context from mmgsdi slot id

  PARAMETERS
    mmgsdi_slot_id      : MMGSDI slot id

  RETURN VALUE
    uim_common_efs_context_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uim_common_efs_context_type uimqmi_convert_mmgsdi_slot_to_efs_context
(
  mmgsdi_slot_id_enum_type                   mmgsdi_slot_id
)
{
  uim_common_efs_context_type      efs_context = UIM_COMMON_EFS_CONTEXT_MAX;

  switch(mmgsdi_slot_id)
  {
    case MMGSDI_SLOT_1:
      efs_context = UIM_COMMON_EFS_CONTEXT_0;
      break;

    case MMGSDI_SLOT_2:
      efs_context = UIM_COMMON_EFS_CONTEXT_1;
      break;

    case MMGSDI_SLOT_3:
      efs_context = UIM_COMMON_EFS_CONTEXT_2;
      break;

    default:
      break;
  }

  return efs_context;
}/* uimqmi_convert_mmgsdi_slot_to_efs_context */


/*===========================================================================
FUNCTION UIMQMI_SEND_RESPONSE()

DESCRIPTION
  This function calls QMI Framework API to send a QMI UIM or CAT Service
  response to a client. It always frees cmd_buf_p and when successful frees
  the transaction.

PARAMETERS
  cmd_buf_p   : command buffer
  msg_ptr     : dsm item
  service_type: service type

RETURN VALUE
  TRUE: If responses send to Framework
  FALSE: otherwise

DEPENDENCIES
  QMI UIM or CAT service must be initialized and registered with Framework

SIDE EFFECTS
  Frees cmd_buf_p

===========================================================================*/
static boolean uimqmi_send_response
(
  qmi_cmd_buf_type           * cmd_buf_p,
  dsm_item_type              * msg_ptr,
  qmux_service_e_type          service_type
)
{
  qmi_transaction_type             * x_p          = NULL;
  uimqmi_common_client_state_type  * common_cl_sp = NULL;
  qmi_framework_msg_hdr_type         msg_hdr;

  ASSERT (msg_ptr);
  ASSERT (cmd_buf_p);
  ASSERT (cmd_buf_p->x_p);
  ASSERT (cmd_buf_p->x_p->cl_sp);

  memset(&msg_hdr,0, sizeof(msg_hdr));
  common_cl_sp = (uimqmi_common_client_state_type*)cmd_buf_p->x_p->cl_sp;

  if(FALSE == qmi_svc_prepend_msg_hdr(&msg_ptr, cmd_buf_p->cmd_type))
  {
    UIM_MSG_ERR_1("Unable to send Response for cmd %d", cmd_buf_p->cmd_type);
    /* Freeing msg_ptr is done in qmi_svc_prepend_msg_hdr for failure */
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
    return FALSE;
  }

  /* Get the transaction from the command buffer*/
  x_p = cmd_buf_p->x_p;

  /* We do not support bundled commands only check the first command in list
     make sure we only have one command and be sure it's expecting a resp.
     Validate service_id as well*/
  if( (x_p->n_cmds != 1) ||
      (cmd_buf_p->cmd_no != 0) ||
      (cmd_buf_p != x_p->cmd_list[cmd_buf_p->cmd_no]) ||
      (x_p->resp_list[cmd_buf_p->cmd_no] != QMI_SVC_RESPONSE_PENDING) ||
      (common_cl_sp->service_id != (uint8)service_type) ||
      (common_cl_sp->common.clid == QMI_SVC_CLID_UNUSED) ||
      (common_cl_sp->common.clid > UIMQMI_SVC_MAX_CLIDS))
  {
    UIM_MSG_ERR_0("Command is linked to transaction it doesn't belong to! discarding");
    dsm_free_packet(&msg_ptr);
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
    return FALSE;
  }

  /* Freeup the transaction and the command buffer as it no longer used */
  ds_qmi_fw_free_cmd_buf(&cmd_buf_p);

  msg_hdr.common_hdr.client_id      = common_cl_sp->common.clid;
  msg_hdr.common_hdr.qmi_instance   = common_cl_sp->instance;
  msg_hdr.common_hdr.service        = (qmux_service_e_type)common_cl_sp->service_id;
  msg_hdr.common_hdr.transaction_id = x_p->x_id;

  msg_hdr.msg_ctl_flag = QMI_FLAG_MSGTYPE_RESP;

  msg_hdr.msg_len  = (uint16) dsm_length_packet(msg_ptr);

  /* qmi_framework_svc_send_response takes care of freeing msg_ptr */
  (void)qmi_framework_svc_send_response(&msg_hdr, msg_ptr);

  /* free the transaction buffer */
  ds_qmi_fw_free_transaction(&x_p);
  return TRUE;
} /* uimqmi_send_response */


/*===========================================================================
  FUNCTION UIMQMI_SEND_INDICATION()

  DESCRIPTION
    This function calls the QMI Framework API to send out the UIM or CAT
    service indication to client.

  PARAMETERS
    client_id   : client ID
    cmd_type    : type of indication
    ind_ptr     : dsm item
    service_type: service type

  RETURN VALUE
    TRUE  - For success
    FALSE - For qmi svc failure

  DEPENDENCIES
    QMI UIM or CAT service must be initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
boolean uimqmi_send_indication
(
  uint8                        client_id,
  uint16                       cmd_type,
  dsm_item_type              * ind_ptr,
  qmux_service_e_type          service_type
)
{
  qmi_framework_msg_hdr_type framework_msg_hdr;

  ASSERT(ind_ptr);

  if (client_id == QMI_SVC_CLID_UNUSED || client_id > UIMQMI_SVC_MAX_CLIDS)
  {
    UIM_MSG_ERR_1("Invalid Client ID: 0x%x", client_id);
    return FALSE;
  }

  /* For indication the transaction ID can be ignored */
  framework_msg_hdr.common_hdr.transaction_id = 0;
  /* If unicast indication, qmi framework map the instance and no need to fill*/
  framework_msg_hdr.common_hdr.qmi_instance   = UIMQMI_SVC_INVALID_INSTANCE;
  framework_msg_hdr.common_hdr.client_id      = client_id;
  framework_msg_hdr.common_hdr.service        = service_type;
  framework_msg_hdr.msg_ctl_flag              = QMI_FLAG_MSGTYPE_IND;
  framework_msg_hdr.msg_len                   = (uint16)dsm_length_packet(ind_ptr);

  UIM_MSG_HIGH_2("Sending QMI Indication to QMI Framework: clid:0x%x, ind_id:0x%x",
                 client_id, cmd_type);

  if(FALSE == qmi_svc_prepend_msg_hdr(&ind_ptr, cmd_type))
  {
    UIM_MSG_ERR_1("Unable to build indication for cmd 0x%x", cmd_type);
    return FALSE;
  }

  if(FALSE == qmi_framework_svc_send_ind(&framework_msg_hdr, ind_ptr))
  {
    UIM_MSG_ERR_0("Unable to send QMI indication to QMI Framework" );
    return FALSE;
  }
  return TRUE;
} /* uimqmi_send_indication */


/*===========================================================================
  FUNCTION  UIMQMI_PROCESS_FRAMEWORK_CMD()

  DESCRIPTION
    Generic QMI processing for QMI UIM/CAT commands

  PARAMETERS
    framework_cmd_ptr       : Pointer to the framework command details
    svc_sp                  : QMI UIM or CAT service state pointer
    cl_sp                   : Client state pointer
    qmi_callback_table_ptr  : Pointer to the service's callback table
    qmi_callback_table_size : Number of entries in the callback table
    is_command_disabled_fptr: Function pointer to check if command is disabled

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM/CAT must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_process_framework_cmd
(
  uimqmi_framework_cmd_type        * framework_cmd_ptr,
  qmi_common_svc_state_type        * svc_sp,
  qmi_common_client_state_type     * cl_sp,
  qmi_svc_cmd_hdlr_type            * qmi_callback_table_ptr,
  uint16                             qmi_callback_table_size,
  uimqmi_command_check_ftype         is_command_disabled_fptr
)
{
  qmi_framework_msg_hdr_type * fw_msg_hdr_ptr  = NULL;
  dsm_item_type              * sdu             = NULL;
  qmi_svc_cmd_hdlr_type      * cmd_hdlr_ptr    = NULL;
  qmi_transaction_type       * x_p             = NULL;
  qmi_cmd_buf_type           * cmd_buf_p       = NULL;
  dsm_item_type              * response_ptr    = NULL;
  uint16                       cmd             = 0;
  int32                        temp            = 0;
  uint16                       cmd_type        = 0;
  uint16                       sdu_len         = 0;
  uint16                       remaining_bytes = 0;
  qmi_error_e_type             errval          = QMI_ERR_NONE;

  ASSERT(framework_cmd_ptr      &&
         framework_cmd_ptr->sdu &&
         svc_sp                 &&
         qmi_callback_table_ptr &&
         is_command_disabled_fptr);

  fw_msg_hdr_ptr = &framework_cmd_ptr->cmd_hdr;
  sdu            = framework_cmd_ptr->sdu;

  if(fw_msg_hdr_ptr->common_hdr.client_id >  UIMQMI_SVC_MAX_CLIDS ||
     fw_msg_hdr_ptr->common_hdr.client_id == 0)
  {
    UIM_MSG_ERR_0("Invalid qmi_framework message header");
    dsm_free_packet(&sdu);
    return;
  }

  if(cl_sp == NULL)
  {
    UIM_MSG_ERR_0("Invalid client indicated by qmi_framework");
    dsm_free_packet(&sdu);
    return;
  }

  /* Get a transaction container */
  x_p = ds_qmi_fw_get_transaction(((qmi_common_svc_state_type*)svc_sp),
                                  ((qmi_common_client_state_type*)cl_sp));
  if (x_p == NULL)
  {
    UIM_MSG_ERR_0("No transaction container!  Discarding QMI framework message");
    dsm_free_packet(&sdu);
    return;
  }

  /* Get a QMI command buffer to hold the command */
  cmd_buf_p = ds_qmi_fw_new_cmd_buf(x_p);
  if (cmd_buf_p == NULL || cmd_buf_p->x_p == NULL || cmd_buf_p->x_p->cl_sp == NULL)
  {
    UIM_MSG_ERR_1("No QMI command buffer. Discarding service transaction: %d",
                  fw_msg_hdr_ptr->common_hdr.transaction_id);
    dsm_free_packet(&sdu);
    ds_qmi_fw_free_transaction(&x_p);
    return;
  }

  x_p->ctl  = fw_msg_hdr_ptr->msg_ctl_flag;
  x_p->x_id = fw_msg_hdr_ptr->common_hdr.transaction_id;

  /* Parse the incoming service message */
  remaining_bytes = (uint16) dsm_length_packet(sdu);

  /* Pull out the command ID */
  temp = dsm_pull16(&sdu);
  if (temp == -1)
  {
    UIM_MSG_ERR_1("Short QMI svc header!  Discarding transaction %d",
                  fw_msg_hdr_ptr->common_hdr.transaction_id);
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  cmd_type = ps_ntohs(temp);

  /* Pull out the length of the input sdu */
  temp = dsm_pull16(&sdu);
  if (temp == -1)
  {
    UIM_MSG_ERR_1("Short QMI svc header!  Discarding transaction %d",
                  fw_msg_hdr_ptr->common_hdr.transaction_id);
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  sdu_len = ps_ntohs(temp);

  /* Framework input sdus have the command and length
     nibbles prepended to them.  Remove these bytes and
     insure we have one and only one input sdu
     remaining bytes must be >= 4 adding the check for lint */
  if(remaining_bytes >=4)
  {
    remaining_bytes -= 4;
  }

  if (sdu_len != remaining_bytes)
  {
    UIM_MSG_ERR_2("Invalid payload length - sdu len:0x%x svc payload len:0x%x",
                  sdu_len, remaining_bytes);
    errval = QMI_ERR_MALFORMED_MSG;
    goto send_result;
  }

  cmd_buf_p->cmd_type = cmd_type;

  /* Search through the UIM or CAT command callback table for
     the command specified in the incoming sdu */
  cmd_hdlr_ptr = qmi_callback_table_ptr;
  for(cmd = 0; cmd < qmi_callback_table_size; cmd++, cmd_hdlr_ptr++)
  {
    if (cmd_buf_p->cmd_type == cmd_hdlr_ptr->cmd_type)
    {
      break;
    }
  }

  if((cmd == qmi_callback_table_size) ||
     (cmd_hdlr_ptr->request_hdlr == NULL) ||
     (is_command_disabled_fptr(cmd_hdlr_ptr->cmd_type) != FALSE))
  {
    /* We couldn't find this command in the UIM or CAT command callbacks array */
    UIM_MSG_ERR_1("Unrecognized type 0x%x for QMI CMD HDLR",
                  cmd_buf_p->cmd_type );
    errval = QMI_ERR_INVALID_QMI_CMD;
    goto send_result;
  }
  else
  {
    /* We found the specified command in the UIM  or CAT command callbacks array */
    cmd_buf_p->in_progress = TRUE;
    if((cmd_buf_p->x_p->ctl & QMI_FLAG_MASK_MSGTYPE) == QMI_FLAG_MSGTYPE_CMD)
    {
      response_ptr = cmd_hdlr_ptr->request_hdlr(svc_sp, cmd_buf_p, cl_sp, &sdu);
      UIM_MSG_HIGH_2("Handling cmd:0x%x Response ptr = 0x%x",
                      cmd_hdlr_ptr->cmd_type, response_ptr);
    }
    else
    {
      errval = QMI_ERR_MALFORMED_MSG;
    }
  }
  cmd_buf_p->in_progress = FALSE;

  if (response_ptr == QMI_SVC_RESPONSE_PENDING)
  {
    dsm_free_packet(&sdu);
    return;
  }

send_result:
  UIM_MSG_HIGH_0("No response pending - execute send_response");
  /* If there was some error from the TLV to send. Otherwise
     the response_ptr will be populated by the cmd_hdlr. */
  if(response_ptr == NULL)
  {
    (void)qmi_svc_put_result_tlv(&response_ptr,
                                 QMI_RESULT_FAILURE,
                                 errval);
  }
  if((response_ptr == NULL) ||
     (FALSE == uimqmi_send_response(cmd_buf_p,
                                    response_ptr,
                                    fw_msg_hdr_ptr->common_hdr.service)))
  {
    ds_qmi_fw_free_transaction(&x_p);
  }
  dsm_free_packet(&sdu);
} /* uimqmi_process_framework_cmd */


/*===========================================================================
  FUNCTION UIMQMI_PUT_RESULT_SEND_RESPONSE_IND

  DESCRIPTION
    This function copies the result TLV and send response or indication to
    the client.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_put_result_send_response_ind
(
  qmi_cmd_buf_type *       cmd_buf_ptr,
  dsm_item_type *          response_ptr,
  qmi_error_e_type         errval,
  qmux_service_e_type      service_type,
  boolean                  ind_required,
  uint8                    indication_clid,
  uint16                   indication_id
)
{
  qmi_result_e_type        result           = QMI_RESULT_FAILURE;
  boolean                  retval           = TRUE;

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response_ptr, result, errval);
  if(FALSE == retval)
  {
    dsm_free_packet(&response_ptr);
    if(cmd_buf_ptr != NULL)
    {
      ds_qmi_fw_free_cmd_buf(&cmd_buf_ptr);
    }	
    return;
  }

  /* Send response or indication */
  if (ind_required)
  {
    retval = uimqmi_send_indication(indication_clid,
                                    indication_id,
                                    response_ptr,
                                    service_type);
  }
  else
  {
    retval = uimqmi_send_response(cmd_buf_ptr, response_ptr, service_type);
  }
  if (FALSE == retval)
  {
    dsm_free_packet(&response_ptr);
    if(cmd_buf_ptr != NULL)
    {
      ds_qmi_fw_free_cmd_buf(&cmd_buf_ptr);
    }
  }
}/* uimqmi_put_result_send_response_ind */

