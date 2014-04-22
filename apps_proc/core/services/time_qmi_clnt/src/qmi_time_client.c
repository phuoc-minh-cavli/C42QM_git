/*=============================================================================
  Copyright (c) 2003-2016,2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/**
@file qmi_time_client.c
@brief

Implements time client for QMI remoting.  

INITIALIZATION AND SEQUENCING REQUIREMENTS

time server should be started in modem

*/ 
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/services/time_qmi_clnt/src/qmi_time_client.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
04/12/11   gparikh File created.

=============================================================================*/

/*includes*/
#include "qmi_time_client.h"
#include "time_remote.h"
#include "qmi_cci_target.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_common.h"
#include "time_service_v01.h"
#include "time_genoff.h"
#include "time_genoff_v.h"
#include "time_ipc.h"
#include "timer.h"
/*end includes*/

/*globals*/
qmi_client_type time_qmi_client_handle=NULL;
/*end globals*/

static qmi_idl_service_object_type time_server_object=NULL;
static qmi_cci_os_signal_type os_params;

boolean qmi_time_connection_done = FALSE;
int time_client_status = TIME_NO_ERR;
/** 
@ingroup QMI Time Client Functions
         
@brief 
This fn implements callback fn that will called 
by QMI FW

@return
None

@param[in]    
  
qmi_client_type user handle
unsigned long   msg id
unsigned char*  ptr to indication buf
int             buffer len,
void*           ptr to cb data
  

@param[out]
 void
 
@dependencies  
time client should be initialized before this fn is called
 
@sideeffects 

*/
void qmi_time_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  time_update_indication_message_v01 ind_resp;
  int rc=-1;
  time_genoff_ptr                 ptime_genoff; 

  MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Indication: msg_id=0x%x buf_len=%d cb_data=0x%x\n", msg_id,
      ind_buf_len, (int)(ind_cb_data)); 

  memset(&ind_resp,0,sizeof(time_update_indication_message_v01));

  switch (msg_id) 
  {
    case QMI_TIME_ATS_TOD_UPDATE_IND_V01:
    case QMI_TIME_ATS_USER_UPDATE_IND_V01:
    case QMI_TIME_ATS_1X_UPDATE_IND_V01:
    case QMI_TIME_ATS_GPS_UPDATE_IND_V01:
    case QMI_TIME_ATS_PSM_UPDATE_IND_V01:
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Decoding the indication into the c struct......\n");
      rc = qmi_client_message_decode(user_handle,
                                     QMI_IDL_INDICATION,
                                     msg_id,
                                     ind_buf,
                                     ind_buf_len,
                                     &ind_resp,
                                     sizeof(time_update_indication_message_v01));

      MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"size of ind_resp=%d ind_resp.base = %d, ind_resp.offset=%d ", 
             sizeof(ind_resp), sizeof(ind_resp.base), sizeof(ind_resp.offset));

      if (rc == QMI_NO_ERR )
      {
        MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Successfully got and parsed an indication");
        MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"msg_id=%d, Base = %d , Offset= 0x%x %x\n",msg_id, ind_resp.base);
        MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Offset= 0x%x %x\n",((ind_resp.offset>>32)&0xFFFFFFFF),ind_resp.offset&0xFFFFFFFF);

        /*Update base here if the base is of valid value*/
        if(ind_resp.base >= ATS_RTC && ind_resp.base < ATS_MAX) 
        {
          /* Get the pointer to genoff in consideration */
           ptime_genoff = time_genoff_get_pointer( (time_bases_type)ind_resp.base );

          /* Sets Generic offset */
          time_genoff_set_generic_offset(ptime_genoff, ind_resp.offset );

          /*signal the time ipc task so that leap sec info
           can be queried from modem*/
          if(ATS_TOD == ind_resp.base) 
          {
			qurt_signal_set(time_ipc_qurt_sig_ptr,TIME_IPC_QMI_REMOTE_COMM2);
          }
        }
        else
        {
          MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Invalid base");
        }
      }

      break;

    default:
      MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Decoding of msg_id =%d not supported\n",msg_id);
    break;
  }  
}

/** 
@ingroup QMI Time Client Functions
         
@brief 
This fn implements connection to the timer server

@return
boolean

@param[in]    
None 

@param[out]
void
 
@dependencies  
time ipc  should be initialized before this fn is called
 
@sideeffects 
None
*/
int qmi_time_client_connect(
           void
)
{
	uint32_t num_services=0, num_entries=1;
	qmi_service_info info[10];
	time_get_genoff_offset_req_msg_v01 time_req;
    time_get_genoff_offset_resp_msg_v01 time_resp;
  
    if(qmi_time_connection_done == TRUE)
		return TIME_NO_ERR;
	
	qmi_time_connection_done = TRUE;
	
    /*initialize the variables before using them*/
    time_req.base = (time_service_base_enum_v01)ATS_TOD;
    time_resp.base = (time_service_base_enum_v01)ATS_RTC;
    time_resp.generic_offset = 0;
  
	  /* The server has come up, store the information in info variable */
    time_client_status = qmi_client_get_service_list( time_server_object, info, &num_entries, &num_services);
    MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"time_qmi_client: qmi_client_get_service_list() returned %d num_entries ="
           " %d num_services = %d\n", time_client_status, num_entries, num_services);
    
	if(time_client_status != QMI_NO_ERR)
		return TIME_SERVICE_ERR;
	
      /*initialize qmi_client*/
    time_client_status = qmi_client_init(&info[0],time_server_object,
                       qmi_time_ind_cb, NULL,
                       &os_params ,&time_qmi_client_handle);

	if(time_client_status != QMI_NO_ERR)
		return TIME_CLIENT_ERR;
    
	/*send the data over to modem. For indication messages to flow 
     from QMI FW, some of the FW variables will be initialized
     by sending a message*/
    time_client_status = qmi_client_send_msg_sync(time_qmi_client_handle,
                                QMI_TIME_GENOFF_GET_REQ_V01,
                                &time_req,
                                sizeof(time_req),
                                &time_resp,
                                sizeof(time_resp),
                                 1000);

	if(time_client_status != QMI_NO_ERR)
		return TIME_SYNC_ERR;

return TIME_NO_ERR;
}

/** 
@ingroup QMI Time Client Functions
         
@brief 
This fn implements initializes QMI client

@return
boolean 

@param[in]
qurt_signal_t* object to signal,
uint32 signal to be set
  
@dependencies  
time client should be initialized before this fn is called
 
@sideeffects 

*/
boolean qmi_time_client_init
(
  qurt_signal_t * signal_ptr,
  uint32  signal_to_be_set,
  uint32  timeout_signal_to_be_set
)
{
  uint32_t num_services=0;
  qmi_client_type notifier;

  /*populate the os_param field*/
  os_params.ext_signal = time_ipc_qurt_sig_ptr;
  os_params.sig = TIME_IPC_QMI_CLIENT_INIT;
  os_params.timer_sig= 0;



  MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"time_qmi_client: Started qmi client...\n");

 /*get the time server object*/
  time_server_object =  time_get_service_object_v01();

  /* Verify that time_server_object did not return NULL */
  if (!time_server_object)
  {
    MSG(MSG_SSID_ATS, MSG_LEGACY_ERROR,"time_qmi_client: time_server_object failed, "
              "verify time_server_object.h and .c match.\n");
    return FALSE;
  }

  time_client_status = qmi_client_notifier_init(time_server_object, &os_params, &notifier);
  
  if(time_client_status != QMI_NO_ERR)
 	return FALSE;

  /* Check if the service is up, if not wait on a signal */
  time_client_status = qmi_client_get_service_list( time_server_object, NULL, NULL, &num_services);
  MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"time_qmi_client: qmi_client_get_service_list() returned"
             " %d num_services = %d\n", time_client_status, num_services);
  
  if(time_client_status != QMI_NO_ERR)
 	return FALSE;

  /*Trying to connect to the server*/
  time_client_status = qmi_time_client_connect();

  return (time_client_status == QMI_NO_ERR);
}


