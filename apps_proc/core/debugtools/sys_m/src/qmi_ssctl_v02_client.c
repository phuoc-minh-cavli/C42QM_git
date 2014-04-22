/*============================================================================
  @file qmi_ssctl_client.c

  Sub System Control(SSCTL) QMI cleint implementation

  Copyright (c) 2015-17 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/qmi_ssctl_v02_client.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/06/14   rks      Initial Version

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "subsystem_control_v02.h"
#include "msg.h"
#include "sys_m_internal.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_ssctl_client.h"
#include "timer.h"
#include "rcevt.h"
#include "task.h"
#include "tms_utils_msg.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define QMI_CLNT_WAIT_SIG      (0x1 << 5)
#define QMI_CLNT_TIMER_SIG     (0x1 << 6)
#define QMI_TIMEOUT_MS 5000

#ifdef SYSM_SPI_NOR_TIMEOUTS
#define SSCTL_REQ_TIMEOUT_MS 40000
#else
#define SSCTL_REQ_TIMEOUT_MS 9000
#endif

static qmi_idl_service_object_type      ssctl_service_object;
static qmi_cci_os_signal_type           os_params;
static qmi_client_type                  ssctl_mpss_client_handle = NULL;
static qmi_client_type                  ssctl_adsp_client_handle = NULL;

static timer_type                ssctl_mpss_req_ind_timer;
static timer_group_type          ssctl_mpss_req_timer_group = {0};
static timer_type                ssctl_adsp_req_ind_timer;
static timer_group_type          ssctl_adsp_req_timer_group = {0};

static boolean ssctl_mpss_req_in_progress = FALSE;
static boolean ssctl_adsp_req_in_progress = FALSE;

/*===========================================================================

  FUNCTION:  ssctl_v02_mpss_ind_cb

===========================================================================*/
/*!
    @brief
    Helper function for the client indication callback

    @detail

    @return
    None

*/
/*=========================================================================*/

static void ssctl_v02_mpss_ind_cb
(
 qmi_client_type                client_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
   time_timetick_type responce_time ;
   qmi_csi_error ret = QMI_CSI_NO_ERR;
   ssctl_cmd_type cmd = (ssctl_cmd_type)((unsigned int)ind_cb_data);

   TMS_MSG_HIGH_1("ssctl_v02_mpss_ind_cb, cmd = %d", cmd);

   responce_time = timer_clr(&ssctl_mpss_req_ind_timer,T_MSEC);

   switch(cmd)
   {
      case SSCTL_SHUTDOWN:
      case SSCTL_RESTART:
         //sys_m_complete_reset();
         break;

      case SSCTL_HIBERNATION:
         sys_m_complete_hibernation();
         break;
        
      case SSCTL_SUBSYS_EVENT:
         ret = QMI_CSI_NO_ERR;/*It would send the indication to HLOS at ssctl service*/
         break;

      default:
         TMS_MSG_ERROR_1("SSCTL client unhandled ssctl_cmd = %d to send indication", cmd);
         break;
   }

   TMS_MSG_HIGH_3("ssctl:qmi_csi_send_ind ret =%d, msg_id = %d, responce_time =%d", ret, msg_id, responce_time);

   ssctl_mpss_req_in_progress = FALSE;
   return;
}

/*===========================================================================

  FUNCTION:  ssctl_v02_adsp_ind_cb

===========================================================================*/
/*!
    @brief
    Helper function for the client indication callback

    @detail

    @return
    None

*/
/*=========================================================================*/
static void ssctl_v02_adsp_ind_cb
(
 qmi_client_type                client_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
   time_timetick_type responce_time ;

   responce_time = timer_clr(&ssctl_adsp_req_ind_timer,T_MSEC);
   ssctl_adsp_req_in_progress = FALSE;
   MSG_3(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"SSCTL:ssctl_v02_adsp_ind_cb request client_handle = %d, msg_id =%d, responce_time = 0x%x",client_handle,msg_id,responce_time);
   return;
}
/*===========================================================================

  FUNCTION:  ssctl_v02_mpss_request_timout_fn

===========================================================================*/
/*!
    @brief
    Callback function called once the request timeout occure

    @detail
    Set ssctl_req_in_process to FALSE so that next requests can be processed
    And notify to the user via its registered callback.
    
    @return
    None

*/
/*=========================================================================*/
static void ssctl_v02_mpss_request_timout_fn(timer_cb_data_type data)
{
   MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02_mpss_request_timout_fn ");
   ssctl_mpss_req_in_progress = FALSE;
}

/*===========================================================================

  FUNCTION:  ssctl_adsp_request_timout_fn

===========================================================================*/
/*!
    @brief
    Callback function called once the request timeout occure

    @detail
    Set SSCTL_req_in_process to FALSE so that next requests can be processed
    And notify to the user via its registered callback.
    
    @return
    None

*/
/*=========================================================================*/
static void ssctl_v02_adsp_request_timout_fn(timer_cb_data_type data)
{
   MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ssctl_v02_adsp_request_timout_fn ");
   ssctl_adsp_req_in_progress = FALSE;
}
/*===========================================================================

  FUNCTION:  ssctl_send_shutdown_req
 
===========================================================================*/
/*!
    @brief
    Send a request to the server to shutdown system

    @detail
    

    @return
    SSCTL_SUCCESS : request for memory was succesful
    SSCTL_FAILURE: request for memory failed

*/
/*=========================================================================*/
static ssctl_status_t ssctl_v02_send_shutdown_req(qmi_client_type client_handle)
{
   int ret = 0;
   qmi_ssctl_shutdown_req_msg_v02 ssctl_shutdown_req;
   qmi_ssctl_shutdown_resp_msg_v02 ssctl_shutdown_resp;

   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"ssctl_v02_send_shutdown_req");

   /* QMI call to send shutdown request */
   ret = qmi_client_send_msg_sync(client_handle, SSCTL_SHUTDOWN,
                                  (void*)&ssctl_shutdown_req,sizeof(ssctl_shutdown_req),
                                  (void*)&ssctl_shutdown_resp,sizeof(ssctl_shutdown_resp), SSCTL_REQ_TIMEOUT_MS );
   if(QMI_NO_ERR != ret)
   {
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID" ERROR ret= %d\n",ret);
      return SSCTL_FAILURE;
   }

   if(ssctl_shutdown_resp.resp.result != QMI_RESULT_SUCCESS_V01)
   {
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID" ERROR , responce = %d\n",ssctl_shutdown_resp.resp.error);
      return SSCTL_FAILURE;
   }

   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID" SSCTL Shutdown Success");

   return SSCTL_SUCCESS;
}
/*===========================================================================

  FUNCTION:  ssctl_v02_send_restart_req

===========================================================================*/
/*!
    @brief
    Send a request to the server to restart system

    @detail

    @return
    SSCTL_SUCCESS : request for memory was succesful
    SSCTL_FAILURE: request for memory failed

*/
/*=========================================================================*/
static ssctl_status_t ssctl_v02_send_restart_req(qmi_client_type client_handle)
{
   int ret = 0;
   qmi_ssctl_restart_req_msg_v02 ssctl_restart_req;
   qmi_ssctl_restart_resp_msg_v02 ssctl_restart_resp;

   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID" SSCTL SSCTL_restart_system");

   /* QMI call to send restart request */
   ret = qmi_client_send_msg_sync(client_handle, SSCTL_RESTART,
                                  (void*)&ssctl_restart_req,sizeof(ssctl_restart_req),
                                  (void*)&ssctl_restart_resp,sizeof(ssctl_restart_resp), SSCTL_REQ_TIMEOUT_MS );
   if(QMI_NO_ERR != ret)
   {
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID" ERROR ret= %d\n",ret);
      return SSCTL_FAILURE;
   }

   if(ssctl_restart_resp.resp.result != QMI_RESULT_SUCCESS_V01)
   {
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ERROR , ssctl_restart_resp.resp = %d\n",ssctl_restart_resp.resp.error);
      return SSCTL_FAILURE;
   }

   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID" SSCTL restart success");
   return SSCTL_SUCCESS;
}
/*===========================================================================

  FUNCTION:  ssctl_v02_send_subsys_event_req
 
===========================================================================*/
/*!
    @brief
    Send SSR events to the subsystem

    @detail
    

    @return
    SSCTL_SUCCESS : request for memory was succesful
    SSCTL_FAILURE: request for memory failed

*/
/*=========================================================================*/
static ssctl_status_t ssctl_v02_send_subsys_event_req(qmi_client_type client_handle,
                                             qmi_ssctl_subsys_event_req_msg_v02 * ss_evt)
{
   int ret = 0;
   qmi_ssctl_subsys_event_resp_msg_v02 ssctl_ss_evt_resp;

   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"enter ssctl_v02_send_subsys_event_req");

   /* QMI call to send shutdown request */
   ret = qmi_client_send_msg_sync(client_handle, SSCTL_SUBSYS_EVENT,
                                  (void*)ss_evt,sizeof(qmi_ssctl_subsys_event_req_msg_v02),
                                  (void*)&ssctl_ss_evt_resp,sizeof(qmi_ssctl_subsys_event_resp_msg_v02), SSCTL_REQ_TIMEOUT_MS );
   if(QMI_NO_ERR != ret)
   {
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"SSCTL: ERROR ret= %d\n",ret);
      return SSCTL_FAILURE;
   }

   if(ssctl_ss_evt_resp.resp.result != QMI_RESULT_SUCCESS_V01)
   {
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"SSCTL: ERROR , responce = %d\n", ssctl_ss_evt_resp.resp.error);
      return SSCTL_FAILURE;
   }

   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"SSCTL ssctl_v02_send_subsys_event_req Success");
   return SSCTL_SUCCESS;
}

/*===========================================================================

  FUNCTION:  ssctl_v02_send_hibernation_req
 
===========================================================================*/
/*!
    @brief
    Send a request to the server for hibernation

    @detail
    

    @return
    SSCTL_SUCCESS : request for memory was succesful
    SSCTL_FAILURE: request for memory failed

*/
/*=========================================================================*/
static ssctl_status_t ssctl_v02_send_hibernation_req(qmi_client_type client_handle)
{
   int ret = 0;
   qmi_ssctl_hibernation_req_msg_v02 ssctl_hibernation_req;
   qmi_ssctl_hibernation_resp_msg_v02 ssctl_hibernation_resp;

   TMS_MSG_HIGH("ssctl_v02_send_hibernation_req");

   /* QMI call to send shutdown request */
   ret = qmi_client_send_msg_sync(client_handle, SSCTL_HIBERNATION,
                                  (void*)&ssctl_hibernation_req,sizeof(ssctl_hibernation_req),
                                  (void*)&ssctl_hibernation_resp,sizeof(ssctl_hibernation_resp), SSCTL_REQ_TIMEOUT_MS );
   if(QMI_NO_ERR != ret)
   {
      TMS_MSG_ERROR_1("ERROR ret= %d\n", ret);
      return SSCTL_FAILURE;
   }

   if(ssctl_hibernation_resp.resp.result != QMI_RESULT_SUCCESS_V01)
   {
      TMS_MSG_ERROR_1("ERROR, responce = %d\n", ssctl_hibernation_resp.resp.error);
      return SSCTL_FAILURE;
   }

   TMS_MSG_HIGH("SSCTL Hibernation Success");

   return SSCTL_SUCCESS;
}

/*===========================================================================

  FUNCTION:  ssctl_v02_send_req

===========================================================================*/
/*!
    @brief
    Initialize the SSCTL QMI client and send request to the SSSCTL service
    at the peripheral requested
    @detail

    @return
    SSCTL_SUCCESS : Request sent successfully 
    SSCTL_FAILURE : Request failed
    SSCTL_IN_PROCESS: already a request is in process
*/
/*=========================================================================*/
ssctl_status_t ssctl_v02_send_req(ssctl_svc_type ssctl_service, 
                              ssctl_cmd_type cmd,
                              qmi_ssctl_subsys_event_req_msg_v02 * ss_evt)
{
   qmi_client_error_type qmi_err = QMI_INTERNAL_ERR;
   ssctl_status_t ssctl_result = SSCTL_FAILURE;
   qmi_client_type client_handle = NULL;
   uint32 ser_intsance = SYSM_CHIP_ID*QMI_SSCTL_SERVICE_INSTANCE_OFFSET_V02 + ssctl_service;
   ssctl_service_object = ssctl_get_service_object_v02();

   qurt_signal_init(&os_params.signal);
   os_params.sig = QMI_CLNT_WAIT_SIG;
   os_params.timer_sig = QMI_CLNT_TIMER_SIG;
   
   if(((MPSS_SSCTL_SVC == ssctl_service)&&(ssctl_mpss_req_in_progress))
      ||((ADSP_SSCTL_SVC == ssctl_service)&&(ssctl_adsp_req_in_progress)))
   {
          return SSCTL_IN_PROCESS;
   }

   if((MPSS_SSCTL_SVC == ssctl_service) && (ssctl_mpss_client_handle == NULL))
   {
       
       qmi_err = qmi_client_init_instance
                 (
                   ssctl_service_object,
                   ser_intsance,
                   ssctl_v02_mpss_ind_cb,
                   (void*)cmd,
                   &os_params,
                   QMI_TIMEOUT_MS,
                   &ssctl_mpss_client_handle
                );
       if(qmi_err!= QMI_NO_ERR)
       {
          MSG_2(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID" SSCTLv02 service not found qmi_err = %d, ser_intsance = %d", qmi_err, ser_intsance);
          return SSCTL_FAILURE;
       }
       MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID" SSCTL v02 ser found instance = %d", ser_intsance);
	   timer_def_osal(&ssctl_mpss_req_ind_timer, 
	   					&ssctl_mpss_req_timer_group,
	   					TIMER_FUNC1_CB_TYPE,
	   					&ssctl_v02_mpss_request_timout_fn,
	   					0);
   }
   else if((ADSP_SSCTL_SVC == ssctl_service) && ( ssctl_adsp_client_handle == NULL))
   {
       qmi_err = qmi_client_init_instance
                 (
                   ssctl_service_object,
                   ser_intsance,
                   ssctl_v02_adsp_ind_cb,
                   (void*)cmd,
                   &os_params,
                   QMI_TIMEOUT_MS,
                   &ssctl_adsp_client_handle
                );
       if(qmi_err!= QMI_NO_ERR)
       {
          MSG_2(MSG_SSID_TMS, MSG_LEGACY_ERROR,PROC_ID"ADSP SSCTL service not found qmi_err = %d, ser_intsance = %d", qmi_err, ser_intsance);
          return SSCTL_FAILURE;
       }
	   timer_def_osal(&ssctl_adsp_req_ind_timer, 
	   					&ssctl_adsp_req_timer_group,
	   					TIMER_FUNC1_CB_TYPE,
	   					&ssctl_v02_adsp_request_timout_fn,
	   					0);	   
   }

   if(MPSS_SSCTL_SVC == ssctl_service)
   {
      timer_set(&ssctl_mpss_req_ind_timer, (timetick_type)SSCTL_REQ_TIMEOUT_MS, 0, T_MSEC);
      client_handle = ssctl_mpss_client_handle;
      ssctl_mpss_req_in_progress = TRUE;
   }
   else if(ADSP_SSCTL_SVC == ssctl_service)
   {
      timer_set(&ssctl_adsp_req_ind_timer, (timetick_type)SSCTL_REQ_TIMEOUT_MS, 0, T_MSEC);
      client_handle = ssctl_adsp_client_handle;
      ssctl_adsp_req_in_progress = TRUE;
   }

   switch(cmd)
   {
      case SSCTL_SHUTDOWN:
         ssctl_result = ssctl_v02_send_shutdown_req(client_handle);
         break;
      case SSCTL_RESTART:
         ssctl_result = ssctl_v02_send_restart_req(client_handle);
         break;
      case SSCTL_SUBSYS_EVENT:
         ssctl_result = ssctl_v02_send_subsys_event_req(client_handle, ss_evt);
         break;
      case SSCTL_HIBERNATION:
         ssctl_result = ssctl_v02_send_hibernation_req(client_handle);
         break;
      default:
         TMS_MSG_HIGH_1("SSCTL v02 client unhandled cmd = %d", cmd);
         break;
   }
   
   if(SSCTL_FAILURE == ssctl_result && MPSS_SSCTL_SVC == ssctl_service)
   {
      timer_clr(&ssctl_mpss_req_ind_timer, T_MSEC);
      ssctl_mpss_req_in_progress = FALSE;
   }
   else if(SSCTL_FAILURE == ssctl_result && ADSP_SSCTL_SVC == ssctl_service)
   {
      timer_clr(&ssctl_adsp_req_ind_timer, T_MSEC);
      ssctl_adsp_req_in_progress = FALSE;
   }

   return ssctl_result;
}

