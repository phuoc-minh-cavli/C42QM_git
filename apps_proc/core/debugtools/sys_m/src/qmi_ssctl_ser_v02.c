/*============================================================================
  @file qmi_ssctl_v02_ser.c

  Sub System Control(SSCTL_v02) QMI service implementation
  
  Copyright (c) 2014-2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================
                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/qmi_ssctl_ser_v02.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/16   nk      Changes to Update the notification id to correct value.
09/10/14   rks     initial SSCTL v02 service implementation

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/



#include "comdef.h"
#include "qmi_csi.h"
#include "subsystem_control_v02.h"
#include "qmi_csi_target.h"
#include "stringl.h"
#include "msg.h"
#include "sys_m_internal.h"

#define MSG_SEP_CHAR ':'
#define SSR_STRING "ssr:"
#define EXTRA_SSR_CHAR_ADDN 6
#define MAX_EVENT_SIZE 20

#define BEFORE_POWERUP "before_powerup"
#define AFTER_POWERUP "after_powerup"
#define BEFORE_SHUTDOWN "before_shutdown"
#define AFTER_SHUTDOWN "after_shutdown"

#define SUBSYS_EVT_SIZE (QMI_SSCTL_SUB_SYS_NAME_LENGTH_V02 + MAX_EVENT_SIZE + EXTRA_SSR_CHAR_ADDN)
extern char sys_m_sfr_buffer[];
typedef struct
{
   qmi_client_handle  qmi_client_handle;
   boolean            connected;
} ssctl_v02_ser_client_type;

static ssctl_v02_ser_client_type qmi_sys_m_client;
static qmi_csi_service_handle ssctl_v02_qmi_ser_handler = NULL;
extern void sys_m_signal_modules(qmi_ssctl_subsys_event_req_msg_v02 * ss_evt);
static int ssctl_v02_cmd_in_progress = 0;
extern void sys_m_update_notification_id(char *evt_str);
/**
  @brief ssctl_v02_ser_validate_connection_handle.

  This function validates a connection handle.

  @param  : connection_handle: Handle to validate.

  @return : TRUE if valid, FALSE otherwise
*/
static boolean ssctl_v02_ser_validate_connection_handle(void *connection_handle)
{
   /* Validate handle. */
   if (connection_handle == NULL)
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02::Connection handle NULL");
      return FALSE;
   }
   if (&qmi_sys_m_client ==
       (ssctl_v02_ser_client_type *)connection_handle)
   {
      return TRUE;
   }
   else{
      return FALSE;
  }
}
/**
  @brief ssctl_v02_send_ind

  This function sends indication back to ssctl_vo2 client


  @return : none.
*/

void ssctl_v02_send_ind(void)
{
   qmi_csi_error err = QMI_CSI_INTERNAL_ERR;
   switch(ssctl_v02_cmd_in_progress)
   {
      case QMI_SSCTL_RESTART_REQ_V02:
      {
         qmi_ssctl_restart_ind_msg_v02 ind;
         err = qmi_csi_send_ind((qmi_sys_m_client.qmi_client_handle),
                      QMI_SSCTL_RESTART_READY_IND_V02,
                      &ind,sizeof(qmi_ssctl_restart_ind_msg_v02));
      }
      break;

      case QMI_SSCTL_SHUTDOWN_REQ_V02:
      {
         qmi_ssctl_shutdown_ind_msg_v02 ind;
         err = qmi_csi_send_ind((qmi_sys_m_client.qmi_client_handle),
                           QMI_SSCTL_SHUTDOWN_READY_IND_V02,
                           &ind,sizeof(qmi_ssctl_shutdown_ind_msg_v02));

      }
      break;
      default:
         MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02:ssctl_v02_cmd_in_progress 0x%x ",ssctl_v02_cmd_in_progress);
      break;
    }
    MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"ssctl_v02:qmi_csi_send_ind err= %d" ,err);
    ssctl_v02_cmd_in_progress = 0;
}


static const char* ssctl_v02_get_ssr_event(ssctl_ssr_event_enum_type_v02 event)
{
   switch(event)
   {
   case SSCTL_SSR_EVENT_BEFORE_POWERUP_V02:
      return BEFORE_POWERUP;
   case SSCTL_SSR_EVENT_AFTER_POWERUP_V02:
      return AFTER_POWERUP;
   case SSCTL_SSR_EVENT_BEFORE_SHUTDOWN_V02:
      return BEFORE_SHUTDOWN;
   case SSCTL_SSR_EVENT_AFTER_SHUTDOWN_V02:
      return AFTER_SHUTDOWN;
   default:
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02:  invalid SSR event: 0x%x ", event);
      break;
   }
   return NULL;
}
/**
  @brief ssctl_v02_ser_process_req

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

static qmi_csi_cb_error ssctl_v02_ser_process_req(void *connection_handle,
                                          qmi_req_handle  req_handle,
                                          unsigned int    msg_id,
                                          void           *req_c_struct,
                                          unsigned int    req_c_struct_len,
                                          void           *service_cookie)
{
   qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
   qmi_response_type_v01 response;
   if ((ssctl_v02_ser_validate_connection_handle(connection_handle) == FALSE) ||
      (req_handle == NULL) || 
      (ssctl_v02_cmd_in_progress!=0)/*concurrency protection */)
   {
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02::Invalid request arguments request id: 0x%x", msg_id);
      response.result = QMI_RESULT_FAILURE_V01;
      response.error  = QMI_ERR_INVALID_MESSAGE_ID_V01;
      return retval;
  }

   MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"ssctl_v02: ssctl_v02_ser_process_req msg_id: 0x%x", msg_id);

   switch(msg_id)
   {
      case QMI_SSCTL_RESTART_REQ_V02:
      {
         qmi_ssctl_restart_resp_msg_v02 response;

         response.resp.result = QMI_RESULT_SUCCESS_V01;;
         response.resp.error  = QMI_ERR_NONE_V01;
         ssctl_v02_cmd_in_progress = QMI_SSCTL_RESTART_REQ_V02;
         qmi_csi_send_resp(req_handle, msg_id, (void*)(&response), sizeof(qmi_ssctl_restart_resp_msg_v02));

         sys_m_reset();

         /*indication will be send after SYS_M_COMPLETE_RESET event  at sys_m_task*/
      }
      break;

      case QMI_SSCTL_SHUTDOWN_REQ_V02:
      {
         qmi_ssctl_shutdown_resp_msg_v02 response;

         response.resp.result = QMI_RESULT_SUCCESS_V01;;
         response.resp.error  = QMI_ERR_NONE_V01;
         ssctl_v02_cmd_in_progress = QMI_SSCTL_SHUTDOWN_REQ_V02;
         qmi_csi_send_resp(req_handle, msg_id, (void*)(&response), sizeof(qmi_ssctl_shutdown_resp_msg_v02));

         sys_m_shutdown();

         /*indication will be send after SYS_M_COMPLETE_RESET event  at sys_m_task*/

      }
      break;

      case QMI_SSCTL_GET_FAILURE_REASON_REQ_V02:
      {
         qmi_csi_error ret;
         qmi_ssctl_get_failure_reason_resp_msg_v02 sfr_resp;
         sfr_resp.error_message_valid = TRUE;
         sfr_resp.resp.result = QMI_RESULT_SUCCESS_V01;
         sfr_resp.resp.error  = QMI_ERR_NONE_V01;
         strlcpy(sfr_resp.error_message, sys_m_sfr_buffer, QMI_SSCTL_ERROR_MSG_LENGTH_V02);
         MSG_SPRINTF_1(MSG_SSID_LEGACY, MSG_LEGACY_HIGH,PROC_ID"sys_m_sfr_buffer = %s", sys_m_sfr_buffer);
         sfr_resp.error_message_len = strlen(sys_m_sfr_buffer);
         ret = qmi_csi_send_resp(req_handle, msg_id, &sfr_resp, sizeof(qmi_ssctl_get_failure_reason_resp_msg_v02));
         MSG_2(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"ssctl_v02::qmi_csi_send_resp ret =%d,length =  %d", ret,sfr_resp.error_message_len);
      }
      break;

      case QMI_SSCTL_SUBSYS_EVENT_REQ_V02:
      {
        qmi_ssctl_subsys_event_resp_msg_v02 ss_evt_rsp;
        qmi_ssctl_subsys_event_comp_ind_msg_v02 ind;
        qmi_csi_error err;
        size_t copy_size = 0;
        qmi_ssctl_subsys_event_req_msg_v02 *ss_evt_req_msg = (qmi_ssctl_subsys_event_req_msg_v02*)(req_c_struct);
        char ss_msg_str[SUBSYS_EVT_SIZE];
        int ss_msg_str_rem_sz = SUBSYS_EVT_SIZE;
        char *temp = (char *)(&ss_msg_str);
        const char* event_str = NULL;
        ss_evt_rsp.resp.result = QMI_RESULT_SUCCESS_V01;
        ss_evt_rsp.resp.error  = QMI_ERR_NONE_V01;


        /*need to make event case insensitive*/
        qmi_csi_send_resp(req_handle, msg_id, &ss_evt_rsp, sizeof(qmi_ssctl_subsys_event_resp_msg_v02));

        copy_size = memscpy(temp, EXTRA_SSR_CHAR_ADDN, SSR_STRING, sizeof(SSR_STRING));

        temp = (char*)temp + copy_size - 1;
        ss_msg_str_rem_sz -= copy_size;
        copy_size = memscpy(temp, QMI_SSCTL_SUB_SYS_NAME_LENGTH_V02,
                            ss_evt_req_msg->subsys_name, strlen(ss_evt_req_msg->subsys_name) +1);

        if(copy_size >= 1){
            temp = (char*)temp + copy_size - 1; /* -1 is to overwrite the NULL char*/
            *temp = MSG_SEP_CHAR;
        }
        ss_msg_str_rem_sz -= copy_size;

        event_str = ssctl_v02_get_ssr_event(ss_evt_req_msg->event);

        if (event_str != NULL){
           copy_size = memscpy(temp, ss_msg_str_rem_sz,
                            event_str, MAX_EVENT_SIZE);
        }

        MSG_SPRINTF_1(MSG_SSID_LEGACY, MSG_LEGACY_HIGH,PROC_ID"ssctl_v02:: SSUBSYS_EVENT = %s", ss_msg_str);
		sys_m_update_notification_id(ss_msg_str);
        sys_m_signal_modules(ss_evt_req_msg);
        err = qmi_csi_send_ind((qmi_sys_m_client.qmi_client_handle),
                  QMI_SSCTL_SUBSYS_EVENT_COMP_IND_V02,
                  &ind,sizeof(qmi_ssctl_subsys_event_comp_ind_msg_v02));
        MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"ssctl_v02: qmi_csi_send_ind err= %d" ,err);
      }
      break;

      default:
      {
         MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02:Unhandled QMI request id: 0x%x",msg_id);

         response.result = QMI_RESULT_FAILURE_V01;
         response.error  = QMI_ERR_INVALID_MESSAGE_ID_V01;
      }
      qmi_csi_send_resp(req_handle, msg_id, (void*)(&response), sizeof(qmi_response_type_v01));
      break;
   }
   return retval;
}

/**
  @brief ssctl_v02_ser_connect

  This function processes incoming requests for the sys_m
  QMI services to connect a client.

  @param  : client_handle: Handle used by the infrastructure
                           to identify different services.
            service_cookie: Ignored
            connection_handle: Client connection to created
                               internally.

  @return : None.
*/
static qmi_csi_cb_error ssctl_v02_ser_connect(qmi_client_handle  client_handle,
                                           void              *service_cookie,
                                           void             **connection_handle)
{
   qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
   ssctl_v02_ser_client_type *client = (ssctl_v02_ser_client_type*)(&qmi_sys_m_client);

   /* Find empty client slot */
   if (client->connected == FALSE)
   {
      /* Found empty slot */
      client->qmi_client_handle = client_handle;
      client->connected = TRUE;
      *connection_handle = client;
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"ssctl_v02:Connect client");
   }
   else
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02:No client connections available");
      retval = QMI_CSI_CB_NO_MEM;
   }
   return retval;
}

/**
  @brief ssctl_v02_ser_disconnect

  This function processes incoming requests for the QMI sys_m
   services to disconnect a client. Sanitizing
  function.

  @param  : connection_handle: Client connection to
                               disconnect.
            service_cookie: Ignored

  @return : None.
*/
static void ssctl_v02_ser_disconnect(void  *connection_handle,
                                 void  *service_cookie)
{
   ssctl_v02_ser_client_type *client = connection_handle;

   if (ssctl_v02_ser_validate_connection_handle(connection_handle) == FALSE)
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02:Disconnect invalid client");
      return;
   }

   if (client->connected== FALSE)
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02:Disconnect invalid client");
      return;
   }

   /* Free client slot from being connected */
   client->connected = FALSE;
   MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02:Disconnect client");
}

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/**
  @brief ssctl_v02_qmi_ser_init

  This function initializes the QMI sys_m
  services handler.

  @param  : os_params: OS parameters.

  @return : qmi_csi_service_handle.
*/
qmi_csi_service_handle ssctl_v02_qmi_ser_init(qmi_csi_os_params *os_params)
{
   qmi_idl_service_object_type ssctl_v02_qmi_ser_object = ssctl_get_service_object_v02();
   qmi_csi_options options;
   unsigned int ser_instance_id = SSCTL_QMI_SERVICE_INSTANCE_MDM_V02;

   QMI_CSI_OPTIONS_INIT(options);

   QMI_CSI_OPTIONS_SET_INSTANCE_ID(options,ser_instance_id);

   if (qmi_csi_register_with_options(ssctl_v02_qmi_ser_object, ssctl_v02_ser_connect,
                                 ssctl_v02_ser_disconnect, ssctl_v02_ser_process_req,
                                 NULL, os_params,&options, &ssctl_v02_qmi_ser_handler) != QMI_CSI_NO_ERR)
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02: QMI service error");
   }
   else
   {
      MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02: QMI service register okay");
   }

  return ssctl_v02_qmi_ser_handler;
}

