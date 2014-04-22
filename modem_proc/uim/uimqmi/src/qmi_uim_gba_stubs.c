/*===========================================================================

                         Q M I _ U I M _ G B A._ S T U B S C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016, 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_gba_stubs.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/06/18    tq     Migration of QMI UIM service to QCCI-QCSI
07/26/16    sp     Move qmi_uimi_ functions to corresponding files
06/16/16    vdc    Split QMI UIM parsing files for supporting IOE
05/19/16    vdc    Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "qmi_uim_gba.h"
#include "mmgsdilib_common.h"
#include "qmi_uim_util.h"
#include "mmgsdisessionlib_v.h"
#include "qmi_uim_parsing.h"
#include "qmi_framework.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*===========================================================================

                                DATA TYPES

===========================================================================*/


/*===========================================================================

                               INTERNAL DATA

===========================================================================*/


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_UIMI_GBA()

  DESCRIPTION
    Performs GBA authentication

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure
    req_c_struct_len : structure len

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_uimi_gba
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  uim_gba_resp_msg_v01            *resp_ptr             = NULL;
  uint16                           client_index         = 0;
  qmi_uim_cmd_buf_s               *cmd_buf_p            = NULL;
  qmi_csi_error                    csi_errval           = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  cmd_buf_p->req_handle = req_handle;  

  resp_ptr =   uimqmi_malloc(sizeof(uim_gba_resp_msg_v01));
  if(!resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }

  qmi_uim_util_populate_resp(QMI_ERR_NOT_SUPPORTED_V01, &resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_gba_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(resp_ptr);

  return csi_errval;
}/* qmi_uimi_gba */


/*===========================================================================
  FUNCTION QMI_UIMI_GET_GBA_IMPI()

  DESCRIPTION
    Retrieves the IMPI used for GBA corresponding to the session

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure
    req_c_struct_len : structure len

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error  qmi_uimi_get_gba_impi
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  uim_get_gba_impi_resp_msg_v01   *resp_ptr             = NULL;
  uint16                           client_index         = 0;
  qmi_uim_cmd_buf_s               *cmd_buf_p            = NULL;
  qmi_csi_error                    csi_errval           = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);
  
  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  cmd_buf_p->req_handle = req_handle;  

  resp_ptr =   uimqmi_malloc(sizeof(uim_get_gba_impi_resp_msg_v01));
  if(!resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(QMI_ERR_NOT_SUPPORTED_V01, &resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_get_gba_impi_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(resp_ptr);

  return csi_errval;
}/* qmi_uimi_get_gba_impi */


/*===========================================================================
  FUNCTION  QMI_UIM_GBA_PROCESS_GBA_CB()

  DESCRIPTION
    Generic QMI processing for external commands

  PARAMETERS
    uim_message_ptr : message to the internal qmi_uim_message
                      containing an external command

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_gba_process_gba_cb
(
  qmi_uim_message_type * uim_message_ptr
)
{
  (void)uim_message_ptr;
} /* qmi_uim_gba_process_gba_cb */

