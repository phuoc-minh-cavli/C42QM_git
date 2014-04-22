/*===========================================================================

                         Q M I _ U I M _ G B A. C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016, 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_gba.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
12/05/16    bcho   Remove compilation error
07/26/16    sp     Move qmi_uimi_ functions to corresponding files
05/19/16    vdc    Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "qmi_uim_gba.h"
#include "mmgsdilib_common.h"
#include "uim_msg.h"
#include "qmi_uim_util.h"
#include "qmi_uim.h"
#include "amssassert.h"
#include "qmi_uim_encryption.h"
#include "mmgsdisessionlib_v.h"
#include <stringl/stringl.h>
#include "fs_lib.h"
#include "fs_public.h"
#include "fs_diag_access.h"
#include "fs_stdlib.h"

/* Maximum APN length */
#define QMI_UIM_APN_NAME_MAX                   100

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_UIM_GBA_CONVERT_UIM_SESSION_TO_GBA_SESSION

  DESCRIPTION
    Convert UIM session type to GBA session type.

  PARAMETERS
    mmgsdi_session_type  : MMGSDI session type
    gba_session_type_ptr : GBA session type

  RETURN VALUE
    errval               : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_uim_gba_convert_uim_session_to_gba_session
(
  uim_session_type_enum_v01       uim_session_type,
  gba_session_type               *gba_session_type_ptr
)
{
  if(gba_session_type_ptr == NULL)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  switch(uim_session_type)
  {
    case UIM_SESSION_TYPE_PRIMARY_GW_V01:
      *gba_session_type_ptr = GBA_3GPP_PROV_SESSION_PRI;
      break;

    case UIM_SESSION_TYPE_SECONDARY_GW_V01:
      *gba_session_type_ptr = GBA_3GPP_PROV_SESSION_SEC;
      break;

    case UIM_SESSION_TYPE_NONPROVISIONING_SLOT_1_V01:
      *gba_session_type_ptr = GBA_NON_PROV_SESSION_SLOT_1;
      break;

    case UIM_SESSION_TYPE_NONPROVISIONING_SLOT_2_V01:
      *gba_session_type_ptr = GBA_NON_PROV_SESSION_SLOT_2;
      break;

    default:
      UIM_MSG_ERR_1("session type not supported: 0x%x", uim_session_type);
      return QMI_ERR_INVALID_ARG_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_uim_gba_convert_uim_session_to_gba_session */


/*===========================================================================
  FUNCTION QMI_UIM_GBA_CONVERT_GBA_STATUS_TO_ERRVAL

  DESCRIPTION
    To obtain QMI client-specific error message from GBA error message

  PARAMETERS
    gba_status   : GBA return value

  RETURN VALUE
    errval       : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_gba_convert_gba_status_to_errval
(
  gba_result_enum_type gba_status
)
{
  qmi_error_type_v01 errval  = QMI_ERR_INTERNAL;

  UIM_MSG_HIGH_1("Received gba_status 0x%x", gba_status);

  switch(gba_status)
  {
    case GBA_SUCCESS:
      errval = QMI_ERR_NONE_V01;
      break;

    case GBA_GENERIC_ERROR:
    case GBA_SIM_ERROR:
      errval = QMI_ERR_INTERNAL_V01;
      break;

    case GBA_INCORRECT_PARAMS:
      errval = QMI_ERR_INVALID_ARG_V01;
      break;

    case GBA_TIMEOUT:
      errval = QMI_ERR_ABORTED_V01;
      break;

    case GBA_AUTH_FAILURE:
    case GBA_SERVER_ERROR:
    case GBA_NETWORK_ERROR:
    case GBA_NETWORK_NOT_READY:
      errval = QMI_ERR_AUTHENTICATION_FAILED_V01;
      break;

    case GBA_MEMORY_ERROR_HEAP_EXHAUSTED:
      errval = QMI_ERR_NO_MEMORY_V01;
      break;

    case GBA_SIM_NOT_READY:
      errval = QMI_ERR_DEVICE_NOT_READY_V01;
      break;

    case GBA_UNSUPPORTED:
      errval = QMI_ERR_NOT_SUPPORTED_V01;
      break;

    default:
      errval = QMI_ERR_INTERNAL_V01;
      break;
  }

  return errval;
} /* qmi_uim_gba_convert_gba_status_to_errval() */


/*===========================================================================
  FUNCTION QMI_UIMI_GBA_CALLBACK()

  DESCRIPTION
    This function is executed as callback of GBA API. This function makes a
    deep copy of the results and post a command to the QMI task to be
    processed.

  PARAMETERS

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uimi_gba_callback
(
  gba_result_enum_type          status,
  uint32                        request_id,
  const gba_response_data_type *cnf_ptr,
  const void                   *user_data_ptr
)
{
  qmi_uim_message_type * msg_ptr = NULL;

  (void)request_id;

  /* Do not remove log print */
  UIM_MSG_MED_1("Got response from GBA with status: 0x%x", status);

  if(status == GBA_SUCCESS && cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("NULL response from GBA module");
    return;
  }

  /* Copy content into a buffer and post a message to QMI task */
  msg_ptr = qmi_uim_util_create_message(QMI_UIM_MESSAGE_GBA_CB);
  if (msg_ptr != NULL)
  {
    msg_ptr->data.gba_cnf.status    = status;

    if(cnf_ptr != NULL)
    {
      (void)memscpy(&msg_ptr->data.gba_cnf.cnf_value,
                    sizeof(msg_ptr->data.gba_cnf.cnf_value),
                    cnf_ptr,
                    sizeof(gba_response_data_type));
    }
    msg_ptr->data.gba_cnf.user_data_ptr = user_data_ptr;

    /* Send command */
    qmi_uim_util_post_message(msg_ptr);
    msg_ptr = NULL;
  }
} /* qmi_uimi_gba_callback */


/*===========================================================================
  FUNCTION QMI_UIM_EFS_READ_FILE

  DESCRIPTION
    Generic function called by QMI UIM to read a file from EFS. This function
    will validate the input parameters and return the EFS data requested.

  PARAMETERS
    file_name_ptr: Name of the file that needs to be read.
    data_len_ptr : Data length of the read file.

  DEPENDENCIES
    None

  RETURN VALUE
    pointer of the buffer where the EFS data is read. Otherwise, return  NULL
    if any failure in opening or reading the file from EFS.

  SIDE EFFECTS
    None
===========================================================================*/
static char * qmi_uim_efs_read_file
(
  const char  * file_name_ptr,
  uint32      * data_len_ptr
)
{
  struct fs_stat   file_stat       = {0};
  int              file_descriptor = 0;
  char           * file_buffer_ptr = NULL;

  if(file_name_ptr == NULL || data_len_ptr == NULL)
  {
    return NULL;
  }

  if(efs_stat(file_name_ptr, &file_stat) != 0 || file_stat.st_size == 0)
  {
    UIM_MSG_LOW_0("efs_stat procedure failed");
    return NULL;
  }

  /* Allocate memory for file buffer */
  file_buffer_ptr = (char *)uimqmi_malloc(file_stat.st_size);
  if(file_buffer_ptr == NULL)
  {
    return NULL;
  }

  /* Open the file in EFS for Read Only */
  file_descriptor = efs_open(file_name_ptr, O_RDONLY);

  if(file_descriptor < 0)
  {
    UIM_MSG_LOW_0("efs_open procedure failed ");
    uimqmi_free(file_buffer_ptr);
    return NULL;
  }

  /* Read the file into buffer provided */
  if (efs_read(file_descriptor, file_buffer_ptr, file_stat.st_size) !=
       file_stat.st_size)
  {
    UIM_MSG_LOW_0("efs_read procedure failed ");
    uimqmi_free(file_buffer_ptr);
    (void)efs_close(file_descriptor);
    return NULL;
  }

  *data_len_ptr = (uint32)file_stat.st_size;

  /* Close the file */
  (void)efs_close(file_descriptor);

  return file_buffer_ptr;
}  /* qmi_uim_efs_read_file */


/*===========================================================================
  FUNCTION QMI_UIM_GBA_ENCRYPT_KS_NAF_REQUIRED()

  DESCRIPTION
    Reads the FQDN entries from the white / black list in the EFS. If white
    list is not available then we fallback to read FQDN entries in the black
    list. This list (white / black) of FQDN entries are checked against the
    input FQDN data and if it matches with any of the entry then encryption of
    ks_naf is done in the response.

  PARAMETERS
    req_fqdn_data_ptr: FQDN data pointer

  RETURN VALUE
    Boolean that indicates whether encryption is required for ks_naf
      TRUE: Encryption is required for ks_naf.
      FALSE: No encryption is done.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_gba_encrypt_ks_naf_required
(
  gba_naf_fqdn_type  * req_fqdn_data_ptr
)
{
  uint16   fqdn_buf_index                  = 0;
  char   * fqdn_list_file_data_ptr         = NULL;
  uint32   efs_data_len                    = 0;
  boolean  result                          = TRUE;
  boolean  is_white_list                   = TRUE;

  if(req_fqdn_data_ptr == NULL)
  {
    return FALSE;
  }

  /* Read the FQDN list file as follows:
     1) Check if white list FQDN file is present in the EFS.
     2) If the white list FQDN file is not present then try to read the black
        list FQDN from the EFS.
     3) If the black list FQDN read also fails then return FALSE as no FQDN
        list is available & encryption of ks_naf is not required */
  fqdn_list_file_data_ptr = qmi_uim_efs_read_file(QMI_UIM_CONFIG_GBA_FQDN_WHITE_LIST,
                                                  &efs_data_len);
  if(fqdn_list_file_data_ptr == NULL)
  {
    UIM_MSG_MED_0("White list FQDN is not available. Falling back to Black list FQDN");

    fqdn_list_file_data_ptr = qmi_uim_efs_read_file(QMI_UIM_CONFIG_GBA_FQDN_BLACK_LIST,
                                                    &efs_data_len);
    if(fqdn_list_file_data_ptr == NULL)
    {
      UIM_MSG_HIGH_0("No FQDN list is available");
      return FALSE;
    }

    /* As white list is not available and we are successful in reading black
       list we make is_white_list to FALSE  & result re-initialized to FALSE.
       If FQDN entry does not match with any FQDN in black list then we return
       this result (FALSE) and we don't encrypt ks_naf */
    is_white_list = FALSE;
    result = FALSE;
  }

  /* Convert the requested FQDN to lower case */
  for (fqdn_buf_index = 0; fqdn_buf_index < req_fqdn_data_ptr->data_len; fqdn_buf_index++)
  {
    req_fqdn_data_ptr->data[fqdn_buf_index] =
      QMI_UIM_LOWER_CASE(req_fqdn_data_ptr->data[fqdn_buf_index]);
  }

  /* Convert the FQDN buffer to lower case */
  for(fqdn_buf_index = 0; fqdn_buf_index < efs_data_len; fqdn_buf_index++)
  {
    fqdn_list_file_data_ptr[fqdn_buf_index] =
      QMI_UIM_LOWER_CASE(fqdn_list_file_data_ptr[fqdn_buf_index]);
  }

  /* Parse the FQDN data */
  fqdn_buf_index = 0;
  while(fqdn_buf_index < efs_data_len)
  {
    uint8   fqdn_start_index = fqdn_buf_index;
    uint8   fqdn_end_index   = fqdn_buf_index;
    uint16  fqdn_len         = 0;

    while(fqdn_end_index < efs_data_len &&
          fqdn_list_file_data_ptr[fqdn_end_index] != ';')
    {
      fqdn_end_index++;
    }

    /* If there is no FQDN entry between two delimiters(;;) then we continue
       to next FQDN entry */
    if(fqdn_end_index == fqdn_buf_index)
    {
      fqdn_buf_index++;
      continue;
    }

    /* update the fqdn_buf_index to point to next FQDN entry in the EFS buffer */
    fqdn_buf_index = fqdn_end_index + 1;

    /* Update the fqdn_end_index to last character of the FQDN entry */
    fqdn_end_index--;

    /* Skip leading spaces if any */
    while(fqdn_list_file_data_ptr[fqdn_start_index] == ' ' &&
          fqdn_start_index < fqdn_end_index)
    {
      fqdn_start_index++;
    }

    /* Skip trailing spaces if any */
    while(fqdn_list_file_data_ptr[fqdn_end_index] == ' ' &&
          fqdn_end_index > fqdn_start_index)
    {
      fqdn_end_index--;
    }

    /* When there are only spaces in FQDN entry then goto next FQDN entry*/
    if(fqdn_start_index == fqdn_end_index &&
       fqdn_list_file_data_ptr[fqdn_start_index] == ' ')
    {
      continue;
    }

    fqdn_len = fqdn_end_index - fqdn_start_index + 1;

    if(req_fqdn_data_ptr->data_len == fqdn_len &&
       memcmp(req_fqdn_data_ptr->data, &fqdn_list_file_data_ptr[fqdn_start_index], fqdn_len) == 0)
    {
      /* FQDN has matched. If the list is white list then return FALSE and
         if it is black list then send TRUE to send encrypted ks_naf */
      if(is_white_list)
      {
        result = FALSE;
      }
      else
      {
        result = TRUE;
      }
      break;
    }
  }

  uimqmi_free(fqdn_list_file_data_ptr);

  return result;
} /* qmi_uim_gba_encrypt_ks_naf_required */


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
  qmi_uim_cmd_buf_s       *cmd_buf_ptr          = NULL;
  qmi_uimi_userdata_type  *cb_userdata_ptr      = NULL;
  qmi_error_e_type         errval               = QMI_ERR_NONE;
  gba_naf_fqdn_type        fqdn_data            = {0};
  void*                   *ind_resp_ptr         = NULL;
  uint32                   ind_resp_len         = 0;
  uint16                   client_index         = 0;
  mmgsdi_data_type         naf_enc_data         = {0, NULL};
  mmgsdi_data_type         ks_naf_data          = {0, NULL};
  boolean                  encrypt_ks_naf       = FALSE;

  UIM_MSG_HIGH_0("Sending response for GBA");

  ASSERT( uim_message_ptr );

  /* Retrieve userdata */
  cb_userdata_ptr = (qmi_uimi_userdata_type*)uim_message_ptr->data.gba_cnf.user_data_ptr;
  if (cb_userdata_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Null userdata ptr in response");
    return;
  }

  /* Retrieve info */
  fqdn_data        = cb_userdata_ptr->data.gba.fqdn_data;

  /* First check if the requested client is still valid */
  if (FALSE == qmi_uim_util_check_clid(cb_userdata_ptr->client_handle, cb_userdata_ptr->client_reg_count, &client_index))
  {
    goto free_cmd_data;
  }

  cmd_buf_ptr = qmi_uim_util_get_cmd_buf_ptr(cb_userdata_ptr->req_handle, cb_userdata_ptr->client_handle);
  if(!cb_userdata_ptr->ind_token.is_valid &&
     cmd_buf_ptr == NULL)
  {
    goto free_cmd_data;
  }

  if(uim_message_ptr->data.gba_cnf.status == GBA_SUCCESS)
  {
    mmgsdi_data_type             gba_resp_data = {0, NULL};

    /* If NAF id is empty then do not send Ks_NAF TLV in the response */
    if(fqdn_data.data_len > 0)
    {
      /* Check whether ks_naf should be encrypted */
      if(qmi_uim_gba_encrypt_ks_naf_required(&fqdn_data))
      {
        encrypt_ks_naf = TRUE;
        ks_naf_data.data_ptr = uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.ks_naf;
        ks_naf_data.data_len = sizeof(uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.ks_naf);
        if(!qmi_uim_encrypt_payload(ks_naf_data, &naf_enc_data))
        {
          errval = QMI_ERR_INTERNAL;
        }
      }
    }
  }
  if (cb_userdata_ptr->ind_token.is_valid)
  {
    uim_gba_ind_msg_v01 *gba_ind_ptr = (uim_gba_ind_msg_v01*) uimqmi_malloc(sizeof(uim_gba_ind_msg_v01));
    ind_resp_len = sizeof(uim_gba_ind_msg_v01);
    ind_resp_ptr = gba_ind_ptr;
    if(ind_resp_ptr == NULL)
    {
      goto free_cmd_data;
    }
    gba_ind_ptr->indication_token = cb_userdata_ptr->ind_token.token;
    if(encrypt_ks_naf)
    {
      if(naf_enc_data.data_ptr != NULL && naf_enc_data.data_len > 0 && errval == QMI_ERR_NONE_V01)
      {
        /* We are ignoring the return value bcoz it is optional TLV 
           and we should send resp/ind even if population of optional TLV got failed */
        (void)qmi_uim_util_populate_opaque(&gba_ind_ptr->encrypted_ks_naf_valid,
                                           gba_ind_ptr->encrypted_ks_naf,
                                           &gba_ind_ptr->encrypted_ks_naf_len,
                                           sizeof(gba_ind_ptr->encrypted_ks_naf),
                                           naf_enc_data.data_ptr,
                                           naf_enc_data.data_len,
                                           FALSE);
      }
      if(naf_enc_data.data_ptr)
      {
        uimqmi_free(naf_enc_data.data_ptr);
      }
    }
    else
    {
        /* We are ignoring the return value bcoz it is optional TLV 
           and we should send resp/ind even if population of optional TLV got failed */
        (void)qmi_uim_util_populate_opaque(&gba_ind_ptr->ks_naf_valid,
                                           gba_ind_ptr->ks_naf,
                                           NULL,
                                           sizeof(gba_ind_ptr->encrypted_ks_naf),
                                           uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.ks_naf,
                                           sizeof(uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.ks_naf),
                                           FALSE);
    }

    if(errval == QMI_ERR_NONE)
    {
      /* We are ignoring the return value bcoz it is optional TLV 
         and we should send resp/ind even if population of optional TLV got failed */
      (void)qmi_uim_util_populate_opaque(&gba_ind_ptr->btid_valid,
                                         (uint8*)gba_ind_ptr->btid,
                                         &gba_ind_ptr->btid_len,
                                         sizeof(gba_ind_ptr->btid),
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.btid.data,
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.btid.data_len,
                                         FALSE);
    
      (void)qmi_uim_util_populate_opaque(&gba_ind_ptr->lifetime_valid,
                                         (uint8*)gba_ind_ptr->lifetime,
                                         &gba_ind_ptr->lifetime_len,
                                         sizeof(gba_ind_ptr->lifetime),
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.lifetime.data,
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.lifetime.data_len,
                                        FALSE);

      (void)qmi_uim_util_populate_opaque(&gba_ind_ptr->impi_valid,
                                         gba_ind_ptr->impi,
                                         &gba_ind_ptr->impi_len,
                                         sizeof(gba_ind_ptr->impi),
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.impi.data,
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.impi.data_len,
                                         FALSE);
    }

    qmi_uim_util_populate_resp(errval, &gba_ind_ptr->resp);
  }
  else 
  {
    uim_gba_resp_msg_v01 *gba_resp_ptr = (uim_gba_resp_msg_v01*) uimqmi_malloc(sizeof(uim_gba_resp_msg_v01));
    ind_resp_len = sizeof(uim_gba_resp_msg_v01);
    ind_resp_ptr = gba_resp_ptr;
    if(ind_resp_ptr == NULL)
    {
      goto free_cmd_data;
    }
    if(encrypt_ks_naf)
    {
      if(naf_enc_data.data_ptr != NULL && naf_enc_data.data_len > 0 && errval == QMI_ERR_NONE_V01)
      {
        /* We are ignoring the return value bcoz it is optional TLV 
           and we should send resp/ind even if population of optional TLV got failed */
        (void)qmi_uim_util_populate_opaque(&gba_resp_ptr->encrypted_ks_naf_valid,
                                           gba_resp_ptr->encrypted_ks_naf,
                                           &gba_resp_ptr->encrypted_ks_naf_len,
                                           sizeof(gba_resp_ptr->encrypted_ks_naf),
                                           naf_enc_data.data_ptr,
                                           naf_enc_data.data_len,
                                           FALSE);
      }
      if(naf_enc_data.data_ptr)
      {
        uimqmi_free(naf_enc_data.data_ptr);
      }
    }
    else
    {
      /* We are ignoring the return value bcoz it is optional TLV 
         and we should send resp/ind even if population of optional TLV got failed */
      (void)qmi_uim_util_populate_opaque(&gba_resp_ptr->ks_naf_valid,
                                         gba_resp_ptr->ks_naf,
                                         NULL,
                                         sizeof(gba_resp_ptr->ks_naf),
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.ks_naf,
                                         sizeof(uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.ks_naf),
                                         FALSE);
    }

    if(errval == QMI_ERR_NONE_V01)
    {
      /* We are ignoring the return value bcoz it is optional TLV 
         and we should send resp/ind even if population of optional TLV got failed */
      (void)qmi_uim_util_populate_opaque(&gba_resp_ptr->btid_valid,
                                         (uint8*)gba_resp_ptr->btid,
                                         &gba_resp_ptr->btid_len,
                                         sizeof(gba_resp_ptr->btid),
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.btid.data,
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.btid.data_len,
                                        FALSE);
    
      (void)qmi_uim_util_populate_opaque(&gba_resp_ptr->lifetime_valid,
                                         (uint8*)gba_resp_ptr->lifetime,
                                         &gba_resp_ptr->lifetime_len,
                                         sizeof(gba_resp_ptr->lifetime),
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.lifetime.data,
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.lifetime.data_len,
                                        FALSE);

      (void)qmi_uim_util_populate_opaque(&gba_resp_ptr->impi_valid,
                                         (uint8*)gba_resp_ptr->impi,
                                         &gba_resp_ptr->impi_len,
                                         sizeof(gba_resp_ptr->impi),
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.impi.data,
                                         uim_message_ptr->data.gba_cnf.cnf_value.resp_data.success.impi.data_len,
                                        FALSE);
    }
    qmi_uim_util_populate_resp(errval, &gba_resp_ptr->resp);
  }

  qmi_uim_send_response_ind( 
    (cb_userdata_ptr->ind_token.is_valid) ? NULL : cmd_buf_ptr,
    ind_resp_ptr,
    cb_userdata_ptr->request_id,
    ind_resp_len,
    cb_userdata_ptr->ind_token.is_valid,
    cb_userdata_ptr->client_handle);

  uimqmi_free(ind_resp_ptr);

free_cmd_data:
  if(cmd_buf_ptr)
  {
    qmi_uim_util_remove_qmi_cmd(
      cb_userdata_ptr->req_handle,
      cb_userdata_ptr->client_handle,
      &qmi_uim_state_ptr->client[client_index]->cmd_q);
  }

  /* Free userdata */
  qmi_uim_util_free_cb_userdata(cb_userdata_ptr);
  cb_userdata_ptr = NULL;
} /* qmi_uim_gba_process_gba_cb */


/*===========================================================================
  FUNCTION QMI_UIM_GBA_PREPARE_IMPI_FROM_IMSI

  DESCRIPTION
    Prepare IMPI From USIM IMSI

  PARAMETERS
    imsi_digit_ptr : value of imsi in a gba_imsi_data_type digit format
    impi_data_ptr : IMPI after conversion from IMSI
    mnc_length: MNC length

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_uim_gba_prepare_impi_from_imsi(
  const qmi_uim_imsi_digits_data_type  *imsi_digit_ptr,
  qmi_uim_impi_type                    *impi_data_ptr,
  uint8                                 mnc_length
)
{
  const char  impi_suffix[]    = "@ims.mnc000.mcc000.3gppnetwork.org";

  if(imsi_digit_ptr == NULL ||
     impi_data_ptr == NULL ||
     imsi_digit_ptr->imsi_digits_len > QMI_UIM_IMSI_DIGIT_LEN)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  /* Process IMSI */
  /* For example if the IMSI is 234150999999999 (MCC = 234, MNC = 15), the
     private user identity then takes the form
     234150999999999@ims.mnc015.mcc234.3gppnetwork.org as specified in TS.23.003
     section 13. */
  (void)memscpy(impi_data_ptr->data,
                sizeof(impi_data_ptr->data),
                imsi_digit_ptr->imsi_digits,
                imsi_digit_ptr->imsi_digits_len);

  impi_data_ptr->data_len = imsi_digit_ptr->imsi_digits_len;

  (void)memscpy(impi_data_ptr->data + impi_data_ptr->data_len,
                QMI_UIM_IMPI_MAX_V01 - impi_data_ptr->data_len,
                impi_suffix,
                strlen(impi_suffix));

  /*Update MNC */
  if(mnc_length == 3)
  {
    impi_data_ptr->data[impi_data_ptr->data_len + QMI_UIM_MNC_START_POS]     =
      impi_data_ptr->data[3];
    impi_data_ptr->data[impi_data_ptr->data_len + QMI_UIM_MNC_START_POS + 1] =
      impi_data_ptr->data[4];
    impi_data_ptr->data[impi_data_ptr->data_len + QMI_UIM_MNC_START_POS + 2] =
      impi_data_ptr->data[5];
  }
  else
  {
    impi_data_ptr->data[impi_data_ptr->data_len + QMI_UIM_MNC_START_POS]     = '0';
    impi_data_ptr->data[impi_data_ptr->data_len + QMI_UIM_MNC_START_POS + 1] =
      impi_data_ptr->data[3];
    impi_data_ptr->data[impi_data_ptr->data_len + QMI_UIM_MNC_START_POS + 2] =
      impi_data_ptr->data[4];
  }

  /* Update MCC */
  impi_data_ptr->data[impi_data_ptr->data_len + QMI_UIM_MCC_START_POS]     =
    impi_data_ptr->data[0];
  impi_data_ptr->data[impi_data_ptr->data_len + QMI_UIM_MCC_START_POS + 1] =
    impi_data_ptr->data[1];
  impi_data_ptr->data[impi_data_ptr->data_len + QMI_UIM_MCC_START_POS + 2] =
    impi_data_ptr->data[2];

  impi_data_ptr->data_len += strlen(impi_suffix);

  return QMI_ERR_NONE_V01;
}/* qmi_uim_gba_prepare_impi_from_imsi */


/*===========================================================================
  FUNCTION QMI_UIM_GBA_CONVERT_IMSI_TO_DIGITS()

  DESCRIPTION
    Convert IMSI from qmi_uim_imsi_type to qmi_uim_imsi_digits_data_type

  PARAMETERS
    imsi_data_ptr : value of IMSI in qmi_uim_imsi_type format
    imsi_digit_ptr: value of IMSI in qmi_uim_imsi_digits_data_type format

  RETURN VALUE
    errval        : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_gba_convert_imsi_to_digits
(
  const qmi_uim_imsi_type        *imsi_data_ptr,
  qmi_uim_imsi_digits_data_type  *imsi_digit_ptr
)
{
  uint8    i = 0;

  if(imsi_data_ptr == NULL ||
     imsi_digit_ptr == NULL ||
     ((imsi_data_ptr->imsi[0] + 1) != imsi_data_ptr->len) ||
    ((imsi_data_ptr->imsi[0] * 2) - 1 > sizeof(imsi_digit_ptr->imsi_digits)))
  {
    return QMI_ERR_INTERNAL;
  }

  imsi_digit_ptr->imsi_digits_len = 0;
  memset(&imsi_digit_ptr->imsi_digits[0], 0xF, sizeof(imsi_digit_ptr->imsi_digits));

  /* -----------------------------------------------------------
    Parse MCC from the IMSI:
    B0     B1    B2   B3   B4    B5    B6     B7      B8
    LEN  D1PB   D3D2 D5D4 D7D6  D9D8 D11D10 D13D12  D15D14
  -------------------------------------------------------------*/
  for(i = 0; i < imsi_data_ptr->imsi[0] - 1; i++)
  {
    imsi_digit_ptr->imsi_digits[i * 2] = imsi_data_ptr->imsi[i + 1] & QMI_UIM_IMSI_HIGH_BYTE_MASK;
    imsi_digit_ptr->imsi_digits[i * 2] >>= 4;
    imsi_digit_ptr->imsi_digits_len++;
    imsi_digit_ptr->imsi_digits[i * 2 + 1] = imsi_data_ptr->imsi[i + 2] & QMI_UIM_IMSI_LOW_BYTE_MASK;
    imsi_digit_ptr->imsi_digits_len++;
  }

  /* Check if last digit of IMSI is valid */
  if((imsi_data_ptr->imsi[imsi_data_ptr->imsi[0]] & QMI_UIM_IMSI_HIGH_BYTE_MASK) !=
       QMI_UIM_IMSI_HIGH_BYTE_MASK)
  {
    imsi_digit_ptr->imsi_digits[i * 2] =
      imsi_data_ptr->imsi[imsi_data_ptr->imsi[0]] & QMI_UIM_IMSI_HIGH_BYTE_MASK;
    imsi_digit_ptr->imsi_digits[i * 2] >>= 4;
    imsi_digit_ptr->imsi_digits_len++;
  }

  /* Convert the IMSI into digits format */
  for(i = 0; i < imsi_digit_ptr->imsi_digits_len; i++)
  {
    imsi_digit_ptr->imsi_digits[i] += '0';
  }

  return QMI_ERR_NONE;
} /* qmi_uim_gba_convert_imsi_to_digits */


/*===========================================================================
  FUNCTION QMI_UIM_GBA_GET_IMPI_FROM_USIM()

  DESCRIPTION
    Provides the IMPI data from the USIM application

  PARAMETERS
    session_info_ptr    : Session Information TLV pointer
    impi_data_pptr      : Pointer to a pointer of IMPI data

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_uim_gba_get_impi_from_usim
(
  qmi_uim_session_information_type       *session_info_ptr,
  qmi_uim_impi_type                     **impi_data_pptr
)
{
  qmi_error_type_v01              errval           = QMI_ERR_NONE_V01;
  mmgsdi_slot_id_enum_type        slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_protocol_enum_type       card_protocol    = MMGSDI_NO_PROTOCOL;
  qmi_uim_imsi_type               imsi_data        = {0, };
  qmi_uim_imsi_digits_data_type   imsi_digits_data = {0, };
  mmgsdi_read_data_type           read_data        = {0, NULL};
  uint8                           ad_data[4]       = {0};
  mmgsdi_return_enum_type         mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_session_id_type          session_id       = UIM_INVALID_SESSION_ID;

  if(impi_data_pptr == NULL || session_info_ptr == NULL)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  /* Retrieve session id */
  errval = qmi_uim_util_session_type_to_session_id(&session_id,
                                                   session_info_ptr->session_type,
                                                   session_info_ptr->aid);
  if (errval != QMI_ERR_NONE_V01)
  {
    return errval;
  }

  errval = qmi_uim_util_session_id_to_slot(&slot_id,
                                           session_id);
  if(errval != QMI_ERR_NONE_V01)
  {
    return errval;
  }

  card_protocol = qmi_uim_util_card_protocol(slot_id);
  if(card_protocol != MMGSDI_UICC)
  {
    UIM_MSG_ERR_1("QMI_UIM_GET_GBA_IMPI is not supported for protocol 0x%x",
                  card_protocol);
    return QMI_ERR_NOT_SUPPORTED_V01;
  }

  /* Read the IMSI from MMGSDI */
  errval = qmi_uim_util_read_imsi_for_session_id(session_id, &imsi_data);
  if(errval != QMI_ERR_NONE_V01)
  {
    UIM_MSG_ERR_0("Could not read IMSI");
    return errval;
  }

  /* Convert IMSI data in to IMSI digits format */
  errval = qmi_uim_gba_convert_imsi_to_digits(&imsi_data, &imsi_digits_data);
  if (errval != QMI_ERR_NONE_V01)
  {
    return errval;
  }

  /* Read USIM AD to get the number of digits in MNC */
  read_data.data_ptr = ad_data;
  read_data.data_len = 0x04;

  mmgsdi_status = mmgsdi_session_read_cache(session_id,
                                            MMGSDI_USIM_AD,
                                            read_data);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      (ad_data[3] != 0x02 && ad_data[3] != 0x03))
  {
    return QMI_ERR_INTERNAL;
  }

  *impi_data_pptr = (qmi_uim_impi_type *)uimqmi_malloc(sizeof(qmi_uim_impi_type));
  if(*impi_data_pptr == NULL)
  {
    return QMI_ERR_NO_MEMORY_V01;
  }

  /* Prepare the IMPI from IMSI according to section 13.3 in 3GPP TS.23.003 */
  errval = qmi_uim_gba_prepare_impi_from_imsi(&imsi_digits_data, *impi_data_pptr, ad_data[3]);
  if(errval != QMI_ERR_NONE_V01)
  {
    uimqmi_free(*impi_data_pptr);
    *impi_data_pptr = NULL;
  }

  return errval;
} /* qmi_uim_gba_get_impi_from_usim */


/*===========================================================================
  FUNCTION QMI_UIM_GBA_GET_IMPI_FROM_ISIM()

  DESCRIPTION
    Provides the IMPI data from the ISIM application

  PARAMETERS
    get_gba_impi_req_ptr   : Input gba request ptr
    cmd_buf_ptr            : Pointer to command buffer
    cl_sp                  : Client data pointer

  RETURN VALUE
    QMI_ERR_NONE_V01 on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_uim_gba_get_impi_from_isim
(
  uim_get_gba_impi_req_msg_v01        *get_gba_impi_req_ptr,
  qmi_uim_cmd_buf_s                   *cmd_buf_ptr,
  qmi_uimi_client_state_type          *cl_sp
)
{
  qmi_error_type_v01                    errval     = QMI_ERR_NONE_V01;
  mmgsdi_session_id_type                session_id = UIM_INVALID_SESSION_ID;
  qmi_uim_session_information_type      session_info;

  memset(&session_info, 0x00, sizeof(session_info));

  if(cmd_buf_ptr == NULL || cl_sp == NULL || get_gba_impi_req_ptr == NULL)
  {
    return QMI_ERR_INTERNAL_V01;
  }
  
  errval = qmi_uim_util_map_to_mmgsdi_session_type(get_gba_impi_req_ptr->session_information.session_type, &session_info.session_type);
  if(errval != QMI_ERR_NONE_V01)
  {
    return errval;
  }

  if(get_gba_impi_req_ptr->session_information.aid_len <= MMGSDI_MAX_AID_LEN)
  {
    session_info.aid.data_len = get_gba_impi_req_ptr->session_information.aid_len;
    memscpy(session_info.aid.data_ptr,
            sizeof(session_info.aid.data_ptr),
            get_gba_impi_req_ptr->session_information.aid,
            sizeof(get_gba_impi_req_ptr->session_information.aid));
  } 
  else 
  {
    return QMI_ERR_ARG_TOO_LONG_V01;
  }

  /* Retrieve session id */
  errval = qmi_uim_util_session_type_to_session_id(&session_id,
                                                   session_info.session_type,
                                                   session_info.aid);
  if (errval != QMI_ERR_NONE_V01)
  {
    if(errval == QMI_ERR_DEVICE_NOT_READY_V01)
    {
      return errval;
    }

    /* Try to open session */
    errval = qmi_uim_open_session(QMI_UIM_GET_GBA_IMPI_REQ_V01,
                                  &session_info,
                                  get_gba_impi_req_ptr,
                                  sizeof(uim_get_gba_impi_req_msg_v01),
                                  cmd_buf_ptr,
                                  NULL,
                                  cl_sp);
  }
  else
  {
    qmi_uim_requested_encrytion_type encryption_status = QMI_UIM_NO_ENCRYPTION;
    mmgsdi_access_type               mmgsdi_access;

    memset(&mmgsdi_access, 0x00, sizeof(mmgsdi_access));

    if(get_gba_impi_req_ptr->encrypt_data_valid && get_gba_impi_req_ptr->encrypt_data)
    {
      encryption_status = QMI_UIM_DATA_ONLY_ENCRYPTION;
    }

    /* Read EF IMPI */
    mmgsdi_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    mmgsdi_access.file.file_enum = MMGSDI_ISIM_IMPI;

    errval = qmi_uim_execute_read_transparent(
                            QMI_UIM_GET_GBA_IMPI_REQ_V01,
                            session_id,
                            mmgsdi_access,
                            0,
                            0,
                            encryption_status,
                            NULL,
                            cmd_buf_ptr,
                            cl_sp->client_handle);

    /* Associate the Client with the requested Session (if it is a
       non-prov. Session) */
    if(errval == QMI_ERR_NONE_V01)
    {
      (void)qmi_uim_util_update_non_prov_session_cl_sp(&session_info,
                                                       cl_sp);
    }
  }

  return errval;
} /* qmi_uim_gba_get_impi_from_isim */



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
  qmi_uimi_userdata_type*               cb_userdata_ptr     = NULL;
  gba_result_enum_type                  gba_result          = GBA_SUCCESS;
  boolean                               gba_force_bootstrap = FALSE;
  gba_session_type                      gba_session_type    = GBA_SESSION_MAX;
  gba_aid_type                          app_id_data;
  gba_bootstrap_ext_info_type           gba_ext_info;
  gba_bsf_info_type                     bsf_info;
  qmi_error_type_v01                    errval              = QMI_ERR_NONE_V01;
  uim_gba_req_msg_v01                  *gba_req_ptr         = NULL;
  uim_gba_resp_msg_v01                 *gba_resp_ptr        = NULL;
  uint16                                client_index        = 0;
  qmi_uim_cmd_buf_s                    *cmd_buf_p           = NULL;
  qmi_uim_session_information_type      session_info;
  qmi_csi_error                         csi_errval          = QMI_CSI_NO_ERR;
  gba_naf_id_type                       naf_data;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  memset(&naf_data, 0, sizeof(naf_data));
  memset(&bsf_info, 0, sizeof(bsf_info));
  memset(&app_id_data, 0, sizeof(gba_aid_type));
  memset(&gba_ext_info, 0, sizeof(gba_bootstrap_ext_info_type));
  memset(&session_info, 0x00, sizeof(session_info));

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  gba_req_ptr = (uim_gba_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  if(gba_req_ptr->session_information.aid_len > MMGSDI_MAX_AID_LEN)
  {
    errval = QMI_ERR_ARG_TOO_LONG_V01;
    goto send_result; 
  } 

  errval = qmi_uim_gba_convert_uim_session_to_gba_session(
             gba_req_ptr->session_information.session_type,
             &gba_session_type);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  if(gba_req_ptr->session_information.aid_len > 0 &&
     gba_req_ptr->session_information.aid_len <= sizeof(app_id_data.data))
  {
    app_id_data.data_len = gba_req_ptr->session_information.aid_len;
    (void)memscpy(app_id_data.data,
                  sizeof(app_id_data.data),
                  gba_req_ptr->session_information.aid,
                  gba_req_ptr->session_information.aid_len);
  }

  if(gba_req_ptr->naf_id.fqdn_len > GBA_MAX_NAF_FQDN_LEN)
  {
    errval = QMI_ERR_ARG_TOO_LONG_V01;
    goto send_result;
  }

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_GBA_REQ_V01,
                                                 gba_req_ptr->indication_token_valid ? NULL : cmd_buf_p,
                                                 gba_req_ptr->indication_token_valid ? &gba_req_ptr->indication_token : NULL,
                                                 cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  /* Store FQDN to check against the list of white list / black list FQDNs to
     encrypt ks_naf */
  cb_userdata_ptr->data.gba.fqdn_data = naf_data.fqdn;

  gba_result = gba_bootstrap(&naf_data,
                             gba_session_type,
                             &app_id_data,
                             gba_force_bootstrap,
                             NULL,
                             (gba_req_ptr->apn_name_valid) ?
                               &gba_req_ptr->apn_name : NULL,
                             qmi_uimi_gba_callback,
                             cb_userdata_ptr);

  errval = qmi_uim_gba_convert_gba_status_to_errval(gba_result);

send_result:
  if(errval == QMI_ERR_NONE_V01 && !gba_req_ptr->indication_token_valid)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

  gba_resp_ptr =   uimqmi_malloc(sizeof(uim_gba_resp_msg_v01));
  if(!gba_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }

  if(errval == QMI_ERR_NONE_V01)
  {
    /* Update token info */
    gba_resp_ptr->indication_token_valid = gba_req_ptr->indication_token_valid;
    gba_resp_ptr->indication_token = gba_req_ptr->indication_token;
  }
  
  qmi_uim_util_populate_resp(errval, &gba_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           gba_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_gba_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(gba_resp_ptr);
 
  return csi_errval;
} /* qmi_uimi_gba */


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
qmi_csi_error qmi_uimi_get_gba_impi
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                    errval                = QMI_ERR_NONE_V01;
  boolean                               retval                = TRUE;
  qmi_result_e_type                     result                = QMI_RESULT_SUCCESS;
  qmi_uim_impi_type                    *impi_data_ptr         = NULL;
  uim_get_gba_impi_req_msg_v01         *get_gba_impi_req_ptr  = NULL;
  uim_get_gba_impi_resp_msg_v01        *get_gba_impi_resp_ptr = NULL;
  uint16                                client_index          = 0;
  qmi_uim_cmd_buf_s                    *cmd_buf_p             = NULL;
  qmi_uim_session_information_type      session_info;
  qmi_csi_error                         csi_errval            = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  get_gba_impi_req_ptr = (uim_get_gba_impi_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  errval = qmi_uim_util_map_to_mmgsdi_session_type(get_gba_impi_req_ptr->session_information.session_type, &session_info.session_type);
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  if(get_gba_impi_req_ptr->session_information.aid_len <= MMGSDI_MAX_AID_LEN)
  {
    session_info.aid.data_len = get_gba_impi_req_ptr->session_information.aid_len;
    memscpy(session_info.aid.data_ptr,
            sizeof(session_info.aid.data_ptr),
            get_gba_impi_req_ptr->session_information.aid,
            sizeof(get_gba_impi_req_ptr->session_information.aid));
  } 
  else 
  {
    errval = QMI_ERR_ARG_TOO_LONG_V01;
    goto send_result; 
  }

  switch(session_info.session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_GW_PROV_SEC_SESSION:
      errval = qmi_uim_gba_get_impi_from_usim(&session_info,
                                              &impi_data_ptr);
      if(errval != QMI_ERR_NONE || impi_data_ptr == NULL)
      {
        goto send_result;
      }
      break;

    case MMGSDI_NON_PROV_SESSION_SLOT_1:
    case MMGSDI_NON_PROV_SESSION_SLOT_2:
      if(qmi_uim_is_isim_aid(&session_info.aid))
      {
        errval = qmi_uim_gba_get_impi_from_isim(get_gba_impi_req_ptr,
                                                cmd_buf_p,
                                                cl_sp);
        if(errval != QMI_ERR_NONE_V01)
        {
          goto send_result;
        }
      }
      else
      {
        errval = QMI_ERR_INTERNAL_V01;
        goto send_result;
      }
      cmd_buf_p->resp_pending = TRUE;
      return csi_errval;

    default:
      UIM_MSG_ERR_1("QMI_UIM_GET_GBA_IMPI is not supported, session: 0x%x",
                     session_info.session_type);
      errval = QMI_ERR_NOT_SUPPORTED;
      goto send_result;
  }

  if (impi_data_ptr == NULL)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  
  get_gba_impi_resp_ptr = uimqmi_malloc(sizeof(uim_get_gba_impi_resp_msg_v01));
  if(!get_gba_impi_resp_ptr)
  {
    uimqmi_free(impi_data_ptr);
    return QMI_CSI_NO_MEM;
  }

  /* Check whether IMPI data should be encrypted */
  if(get_gba_impi_req_ptr->encrypt_data && get_gba_impi_req_ptr->encrypt_data_valid)
  {
    mmgsdi_data_type    impi_enc_data        = {0, NULL};
    mmgsdi_data_type    impi_data            = {0, NULL};

    impi_data.data_ptr = impi_data_ptr->data;
    impi_data.data_len = impi_data_ptr->data_len;

    if(qmi_uim_encrypt_payload(impi_data, &impi_enc_data))
    {

      if(!qmi_uim_util_populate_opaque(&get_gba_impi_resp_ptr->encrypted_impi_valid,
                                       get_gba_impi_resp_ptr->encrypted_impi,
                                       &get_gba_impi_resp_ptr->encrypted_impi_len,
                                       sizeof(get_gba_impi_resp_ptr->encrypted_impi),
                                       impi_enc_data.data_ptr,
                                       impi_enc_data.data_len,
                                       TRUE))
      {
        errval = QMI_ERR_NO_MEMORY_V01;
      }
    }
    else
    {
      errval = QMI_ERR_INTERNAL_V01;
    }
  }
  else
  {
    mmgsdi_data_type impi_data = {0, NULL};

    impi_data.data_ptr = (uint8 *)impi_data_ptr->data;
    impi_data.data_len = impi_data_ptr->data_len;      
    if(!qmi_uim_util_populate_opaque(&get_gba_impi_resp_ptr->impi_valid,
                                     (uint8*)get_gba_impi_resp_ptr->impi,
                                     &get_gba_impi_resp_ptr->impi_len,
                                     sizeof(get_gba_impi_resp_ptr->impi),
                                     impi_data.data_ptr,
                                     impi_data.data_len,
                                     TRUE))
      {
        errval = QMI_ERR_NO_MEMORY_V01;
      }
  }

  /* Free IMPI data pointer */
  uimqmi_free(impi_data_ptr);
  impi_data_ptr = NULL;

send_result:
  if(!get_gba_impi_resp_ptr)
  {
    get_gba_impi_resp_ptr =   uimqmi_malloc(sizeof(uim_get_gba_impi_resp_msg_v01));
    if(!get_gba_impi_resp_ptr)
    {
      return QMI_CSI_NO_MEM;
    }
  }

  qmi_uim_util_populate_resp(errval, &get_gba_impi_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           get_gba_impi_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_get_gba_impi_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(get_gba_impi_resp_ptr);
 
  return  csi_errval;
} /* qmi_uimi_get_gba_impi */
