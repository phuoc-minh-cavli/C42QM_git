/*===========================================================================
  FILE: qdss_qmi_ctrl_client.c

  OVERVIEW:     Qualcomm Debug Subsystem (QDSS) Qualcomm Messaging
                Interface (QMI) Control Client

  DEPENDENCIES: None.

===========================================================================*/
/*===========================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/qmi/ctrl_client/src/qdss_qmi_ctrl_client.c#1 $

  when       who     what, where, why
  --------   ---    ---------------------------------------------------------
  02/26/13   lht    Initial revision.

===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "qdss_qmi_ctrl_client.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

#define QDSS_QMI_CTRL_MAX_SERVICES 11         // MPSS, ADSP, WCNSS,
#define QDSS_QMI_CTRL_CLIENT_NOTIFY_SIG  0x0001
#define QDSS_QMI_CTRL_CLIENT_TIMER_SIG   0x0002
#define QDSS_QMI_CTRL_CLIENT_NOTIFY_TIMEOUT_MS 2
#define QDSS_QMI_CTRL_CLIENT_SEND_TIMEOUT_MS 10

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

typedef struct qdss_qmi_ctrl_client_info_s
{
   int isConnected;            // 0 = not ready, userHandle invalid;
                               // 1 = ready, userHandle valid.
   qmi_client_type userHandle; // handle for connection to server.
//   qmi_service_info service_info;

} qdss_qmi_ctrl_client_info_t;

/*--------------- ------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/

int qdss_qmi_ctrl_client_indication_cb_data[10];

// One connection per service on the different subsystems.
qdss_qmi_ctrl_client_info_t qdss_qmi_ctrl_client_info[QDSS_QMI_CTRL_MAX_SERVICES];



/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/
void qdss_qmi_ctrl_client_indication_cb(
   qmi_client_type user_handle,
   unsigned int    msg_id,
   void            *ind_buf,
   unsigned int    ind_buf_len,
   void            *ind_cb_data
)
{
   return;
}

/* ----------------------------------------------------------------------- */
// QMI_NO_ERR = user_handle valid; ow service unavailable/failed.
qmi_client_error_type
qdss_qmi_ctrl_client_connect(
   qdssc_svc_instance_id_enum_type_v01 svc_inst_id,    // subsystem or instance id
   uint32 timeout_ms,                    // may block for this amt of time.
   qmi_client_type *pUser_handle)        // [out] - connection handle.
{
   qmi_client_error_type err;
   qmi_idl_service_object_type service_obj;
   qmi_client_os_params os_params;

   *pUser_handle = NULL;

   if (0 > svc_inst_id || QDSS_QMI_CTRL_MAX_SERVICES <= svc_inst_id)
   {  // Out of resources.
      return QMI_INTERNAL_ERR;
   }
   if (1 == qdss_qmi_ctrl_client_info[svc_inst_id].isConnected)
   {  // Connection already initialized.
      *pUser_handle = qdss_qmi_ctrl_client_info[svc_inst_id].userHandle;
      err = QMI_NO_ERR;
   }
   else
   {  // Need to initialize a connection.
      // Get the service object
      service_obj = qdssc_get_service_object_v01();
      if (NULL == service_obj)
      {  // Should never fail.
         return QMI_INTERNAL_ERR;
      }

      // Initialize OS parameters.
      //os_params.tcb = rex_self();
      os_params.sig = QDSS_QMI_CTRL_CLIENT_NOTIFY_SIG;
      os_params.timer_sig = QDSS_QMI_CTRL_CLIENT_TIMER_SIG;

      // Find and initialize a connection to the service of the
      // subsystem specified by svc_inst_id. Get a user handle.
      err = qmi_client_init_instance(service_obj, svc_inst_id,
                                     qdss_qmi_ctrl_client_indication_cb,
                                     qdss_qmi_ctrl_client_indication_cb_data,
                                     &os_params, timeout_ms,
                                     pUser_handle);
      if (QMI_NO_ERR == err)
      {
         // Save the info.
         qdss_qmi_ctrl_client_info[svc_inst_id].isConnected = 1;
      }
      else
      {
         *pUser_handle = NULL;
      }
      qdss_qmi_ctrl_client_info[svc_inst_id].userHandle = *pUser_handle;
   }
   return err;
}

/* ----------------------------------------------------------------------- */
// Main call to issue a command over QMI to QDSSC service on the
// different subsystems.
qmi_client_error_type qdss_qmi_ctrl_client_send(
   qdssc_svc_instance_id_enum_type_v01  svc_inst_id,
   unsigned int msg_id,
   void *pRqst,
   unsigned int rqst_len,
   void *pResp,
   unsigned int resp_len
)
{
   qmi_client_error_type err;
   uint32 timeout_ms;
   qmi_client_type user_handle;

   timeout_ms = QDSS_QMI_CTRL_CLIENT_NOTIFY_TIMEOUT_MS;
   memset (pResp, 0, resp_len);  // Clear the response.

   // Make connection to subsystem's QDSS control service.
   // Keep user handle, will be released later.
   err = qdss_qmi_ctrl_client_connect(svc_inst_id, timeout_ms, &user_handle);

   if (QMI_NO_ERR == err && NULL != user_handle)  // Service is available.
   {

      // Send the request.
      timeout_ms = QDSS_QMI_CTRL_CLIENT_SEND_TIMEOUT_MS;
      err = qmi_client_send_msg_sync(user_handle, msg_id,
                                     pRqst,rqst_len,
                                     pResp,resp_len,
                                     timeout_ms);   // timeout_ms
   }
   // Keep user handle.
   return err;
}
/* ----------------------------------------------------------------------- */
int qdss_qmi_ctrl_client_init(void)
{
   memset(qdss_qmi_ctrl_client_indication_cb_data, 0,
          sizeof(qdss_qmi_ctrl_client_indication_cb_data));
   memset(qdss_qmi_ctrl_client_info, 0, sizeof(qdss_qmi_ctrl_client_info));

   return 0;
}


