/*============================================================================
  @file qmi_ssctl_ser.c

  Sub System Control(SSCTL) QMI service implementation
  
  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: 
$DateTime: 2020/01/30 22:49:35 $
$Author: pwbldsvc $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/



#include "comdef.h"
#include "qmi_csi.h"
#include "qmi_sap.h"
#include "subsystem_control_v01.h"
#include "qmi_csi_target.h"
#include "msg.h" 
#include "qmi_sap.h"
#include "sys_m.h"

typedef struct 
{
   qmi_client_handle  qmi_client_handle;
   boolean            connected;
} ssctl_ser_client_type;

ssctl_ser_client_type qmi_qsc_sys_m_client;
static qmi_csi_service_handle ssctl_qmi_ser_handler = NULL;

static qmi_sap_client_handle qsap_handle;
static qmi_response_type_v01 sys_m_qmi_resp;

extern char sys_m_sfr_buffer[SYS_M_SFR_LENGTH];

/**
  @brief ssctl_ser_validate_connection_handle.

  This function validates a connection handle.

  @param  : connection_handle: Handle to validate.
   
  @return : TRUE if valid, FALSE otherwise
*/
static boolean ssctl_ser_validate_connection_handle(void *connection_handle)
{
   /* Validate handle. */
   if (connection_handle == NULL)
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl::Connection handle NULL");
      return FALSE;
   }
   if (&qmi_qsc_sys_m_client == 
       (ssctl_ser_client_type *)connection_handle)
   {
      return TRUE;
   }
   else{
      return FALSE;
  }
}

/**
  @brief ssctl_ser_process_req

  This function processes incoming requests for the QMI sys_m
  services handler. Sanitizing function. 
  req_c_struct and req_c_struct_len are validated in handling 
  functions. 

  @param  : connection_handle: Client connection making request. 
            req_handle: QMI request handle
            msg_id: QMI msg request ID
            req_c_struct: QMI request structure
            req_c_struct_len: QMI request structure length
            service_cookie: Ignored

  @return : qmi_csi_cb_error.
*/

static qmi_csi_cb_error ssctl_ser_process_req(void *connection_handle,
                                          qmi_req_handle  req_handle,
                                          unsigned int    msg_id,
                                          void           *req_c_struct,
                                          unsigned int    req_c_struct_len,
                                          void           *service_cookie)
{
   qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
   qmi_response_type_v01 *response = (qmi_response_type_v01 *)&sys_m_qmi_resp;
   if ((ssctl_ser_validate_connection_handle(connection_handle) == FALSE) ||
      (req_handle == NULL))
   {
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl::Invalid request arguments request id: 0x%x", msg_id);
      response->result = QMI_RESULT_FAILURE_V01;
      response->error  = QMI_ERR_INVALID_MESSAGE_ID_V01;
      return retval;
  }


   switch(msg_id)
   {
      case QMI_SSCTL_RESTART_REQ_V01:
      {
         qmi_ssctl_restart_resp_msg_v01 *response = (qmi_ssctl_restart_resp_msg_v01 *)&sys_m_qmi_resp;

         response->resp.result = QMI_RESULT_SUCCESS_V01;;
         response->resp.error  = QMI_ERR_NONE_V01;
         qmi_csi_send_resp(req_handle, msg_id, response, sizeof(qmi_ssctl_restart_resp_msg_v01));
         
        sys_m_reset();
        qmi_csi_send_ind((qmi_qsc_sys_m_client.qmi_client_handle),
                      QMI_SSCTL_RESTART_READY_IND_V01,
                      (void*)NULL,0);
      }
      break;

      case QMI_SSCTL_SHUTDOWN_REQ_V01:
      {
         qmi_ssctl_shutdown_resp_msg_v01 *response = (qmi_ssctl_shutdown_resp_msg_v01 *)&sys_m_qmi_resp;
         
         response->resp.result = QMI_RESULT_SUCCESS_V01;;
         response->resp.error  = QMI_ERR_NONE_V01;
         qmi_csi_send_resp(req_handle, msg_id, response, sizeof(qmi_ssctl_shutdown_resp_msg_v01));

         sys_m_shutdown();
         
         // sending dummy indication in case of QSC since there is no EFS
         qmi_csi_send_ind((qmi_qsc_sys_m_client.qmi_client_handle),
                           QMI_SSCTL_SHUTDOWN_READY_IND_V01,
                           (void*)NULL,0);
                           
         

      }
      break;

      case QMI_SSCTL_GET_FAILURE_REASON_REQ_V01:
      {
         qmi_ssctl_get_failure_reason_resp_msg_v01 sfr_resp;
         sfr_resp.error_message_valid = TRUE;
         sfr_resp.error_message_len = SYS_M_SFR_LENGTH;
         sfr_resp.resp.result = QMI_RESULT_SUCCESS_V01;;
         sfr_resp.resp.error  = QMI_ERR_NONE_V01;
         
         //copy SFR 
         strlcpy(sfr_resp.error_message, sys_m_sfr_buffer, SYS_M_SFR_LENGTH);
         
         qmi_csi_send_resp(req_handle, msg_id, &sfr_resp, sizeof(qmi_ssctl_get_failure_reason_resp_msg_v01));
      }
      break;

      default:
      {
         MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl::Unhandled QMI request id: 0x%x",msg_id);

         response->result = QMI_RESULT_FAILURE_V01;
         response->error  = QMI_ERR_INVALID_MESSAGE_ID_V01;
      }
      qmi_csi_send_resp(req_handle, msg_id, response, sizeof(qmi_response_type_v01));
      break;
   }
   return retval;
}

/**
  @brief ssctl_ser_connect

  This function processes incoming requests for the sys_m
  QMI services to connect a client.

  @param  : client_handle: Handle used by the infrastructure 
                           to identify different services.
            service_cookie: Ignored
            connection_handle: Client connection to created
                               internally.

  @return : None.
*/
static qmi_csi_cb_error ssctl_ser_connect(qmi_client_handle  client_handle,
                                           void              *service_cookie,
                                           void             **connection_handle)
{
   qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
   ssctl_ser_client_type *client = (ssctl_ser_client_type*)(&qmi_qsc_sys_m_client);
  
   /* Find empty client slot */
   if (client->connected == FALSE)
   {
      /* Found empty slot */
      client->qmi_client_handle = client_handle;
      client->connected = TRUE;
      *connection_handle = client;
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl::Connect client");
   }
   else
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl::No client connections available");
      retval = QMI_CSI_CB_NO_MEM;
   }
   return retval;
}

/**
  @brief ssctl_ser_disconnect

  This function processes incoming requests for the QMI sys_m
   services to disconnect a client. Sanitizing 
  function. 

  @param  : connection_handle: Client connection to 
                               disconnect.
            service_cookie: Ignored

  @return : None.
*/
static void ssctl_ser_disconnect(void  *connection_handle,
                                 void  *service_cookie)
{
   ssctl_ser_client_type *client = connection_handle;

   if (ssctl_ser_validate_connection_handle(connection_handle) == FALSE)
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl::Disconnect invalid client");
      return;
   }

   if (client->connected== FALSE)
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl:Disconnect invalid client");
      return;
   }

   /* Free client slot from being connected */
   client->connected = FALSE;
   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl:Disconnect client");
}

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS
                     
===========================================================================*/

/**
  @brief ssctl_qmi_ser_init

  This function initializes the QMI sys_m 
  services handler. 

  @param  : os_params: OS parameters.

  @return : qmi_csi_service_handle.
*/
qmi_csi_service_handle ssctl_qmi_ser_init(qmi_csi_os_params *os_params)
{
   qmi_idl_service_object_type ssctl_qmi_ser_object = ssctl_get_service_object_v01();

   if (qmi_csi_register(ssctl_qmi_ser_object, ssctl_ser_connect, 
                                 ssctl_ser_disconnect, ssctl_ser_process_req,
                                 NULL, os_params, &ssctl_qmi_ser_handler) != QMI_CSI_NO_ERR)
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl QMI service error");
   } 
   else
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl QMI service register okay");

      if(ssctl_qmi_ser_object)
      {
         if (QMI_SAP_NO_ERR != qmi_sap_register(ssctl_qmi_ser_object, NULL, &qsap_handle))
         {
           MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl QMI service QSAP register error");
         }
         else{
            MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl QMI service QSAP register okay");
         }
      }
      else
      {
         MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"ssctl_qmi_ser_object is NULL");
      } 
   }
      
  return ssctl_qmi_ser_handler;
}

