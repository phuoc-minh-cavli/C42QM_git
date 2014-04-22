/*===========================================================================

                         C S D_ Q M I _ S E R V I C E S. C

DESCRIPTION

  The Qualcomm Core Sound Driver Services MSM Interface source file.

EXTERNAL FUNCTIONS

  qmi_csi_register()
    Register the Core Sound Driver Service with QCSI for all applicable
    QMI links.
  
  qmi_sap_register
    Register the Core Sound Driver Service with QSAP for all applicable
    QMI links.

Copyright (c) 2011-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/src/csd_qmi_services.c#1 $
$DateTime: 2018/12/25 22:39:07 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/15/16    rk     Abstracting OSAL layer
09/12/14    vk     Added VoUSB feature support
09/01/14    rp     replaced the qmi debug buffer with uniform csd debug buffer
07/03/13    vk     Changes to remove compiler warnings
05/24/13    rp     Added support for asynchronized qmi csd clients apis
06/26/12    jk     Added support for cleanup when client is disconnected 
04/03/12    rp     Added init debug variable and init varification code
02/22/12    jp/rp  Added debug message for close handler
12/06/11    jp     Added messaging for feature like music delivery , Device connect
09/12/11    akj    Added QMI CSD Design changes.
09/09/11    jp     Added CSD registration to QSAP
08/16/11    jp     Incorporated review comment
06/13/11    jg     Created file (initial version)

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "csd_handle.h"
#include "csd_qmi_services_common.h"
#include "csd_qmi_services.h"
#include "csdlite.h"
#include "csd_os_dependencies.h"
#include "apr_errcodes.h"
#include "npa.h"
#include "npa_resource.h"
#include "csd_os_dependencies.h"
#ifdef FEATURE_AUDIO_RTPD_SERVICE
#include "rtpd_services.h"
#include "rtpd_status.h"
#include "rtpd_debug_msg.h"
#endif
#include "csd_debug_info.h"
/*===========================================================================

                               DEFINITIONS

===========================================================================*/

#define QMI_CLNT_WAIT_SIG  0x00010000
#define QMI_CLNT_TIMER_SIG  0x00000001

/*===========================================================================

                                DATA TYPES

===========================================================================*/
csd_event_t qmi_csd_sync_event = NULL;      /* qmi csd sync event  */
npa_client_handle clock_voting_handle = NULL;

boolean csd_debug_call_continue =FALSE;

/* QMI CSD message IDs start from 0x20
   This constant has to match the actual Minimum message ID for QMI CSD.
   Refer to core_sound_driver_v01.idl/h */

#define QMI_CSD_MSG_ID_MIN    0x20
/*Stores the QMI CSD initialisation status */
enum qmi_csd_init_status_t
{
  QMI_CSD_INIT_DONE  = 1,
  QMI_CSD_DINIT_DONE,
  QMI_CSD_INIT_FAILED,
  QMI_CSD_DINIT_FAILED
};

uint32_t  qmi_csd_init   = 0xFFFFFFFF;  /* default boot up value */
         /* 1  - CSD INIT is done */ 
         /* 2  - CSD DE INIT is done */ 
         /* 3  - QMI CSD INIT is called and csd_init failed */ 
         /* 4  - QMI CSD De Init called and csd_deinit failed */ 
         /* 0xFFFFFFFF  - QMI CSD INIT Never called */ 

bool_t csd_lite_init = 0;
         
service_context_type service_cookie;
qmi_sap_client_handle qsap_handle = NULL;
/*=============================================================================
                    REQUEST HANDLER Functions
=============================================================================*/

/*=============================================================================
  FUNCTION csd_query_driver_version_req_handler
=============================================================================*/
/*!
@brief
  This function is called when a QMI_CSD_QUERY_DRIVER_VERSION_REQ message is
  received by the service, sending a response message with the version.

@param[in]   req_c_struct        Decoded request message structure

@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
static qmi_csi_cb_error csd_query_driver_version_req_handler
(
  client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
)
{
  qmi_csi_cb_error qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  qmi_csi_error resp_err;
  // service_context_type *context = (service_context_type*)service_cookie;
  // response msg is small, declared on stack.
  qmi_csd_query_driver_version_resp_msg_v01 resp;

  csd_memset( &resp, 0, sizeof(qmi_csd_query_driver_version_resp_msg_v01) );
  resp.csd_version = csdlite_get_version();
  resp.csd_version_valid = TRUE;
  resp_err = qmi_csi_send_resp( req_handle, msg_id, &resp, sizeof(resp) );
  if( resp_err != QMI_CSI_NO_ERR )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_query_driver_version_req_handler: qmi_csi_send_resp failed for msg_id[0x%x], rc[0x%x]", msg_id, resp_err );
  }
  else
  {
    CSD_MSG( CSD_PRIO_HIGH, "csd_query_driver_version_req_handler: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
    QMI_CSD_RESP_SET_BUFFER(msg_id, 0 );
    qmi_rc = QMI_CSI_CB_NO_ERR;
  }
  return qmi_rc;
}

/*=============================================================================
  FUNCTION csd_init_req_handler
=============================================================================*/
/*!
@brief
  This function is called when a QMI_CSD_INIT_REQ message is
  received by the service, sending a response message after CSD init.

@param[in]   req_c_struct        Decoded request message structure

@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
static qmi_csi_cb_error csd_init_req_handler
(
  client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
  )
{
  qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
  qmi_csi_error resp_err;
  qmi_csd_init_resp_msg_v01 resp;
  uint32_t  csd_rc = CSD_EOK;
  int32_t apr_rc = APR_EOK;
#ifdef FEATURE_AUDIO_RTPD_SERVICE
  uint32_t rtpd_rc =  RTPD_EOK;
#endif

  csd_memset( &resp, 0, sizeof(qmi_csd_init_resp_msg_v01) );
  
  if( QMI_CSD_INIT_DONE != qmi_csd_init )
  {
    
    csd_rc = qmi_csd_client_info_list_init();
    csd_rc = csd_init();    

#ifdef FEATURE_AUDIO_RTPD_SERVICE
    rtpd_rc = rtpd_service_init();
#endif
  }
  else
  {
    CSD_MSG( CSD_PRIO_LOW, "csd_init_req_handler: QMI CSD already initialized" );
  }

#ifdef FEATURE_AUDIO_RTPD_SERVICE
  if(rtpd_rc != RTPD_EOK)
  {
    RTPD_MSG( RTPD_PRIO_ERROR, "csd_init_req_handler: rtpd_service_init failed rc = 0x%x", rtpd_rc );
  } 
#endif

  if( csd_rc != CSD_EOK )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_init_req_handler: csd_init failed, rc = 0x%x", csd_rc );
    
    resp.qmi_csd_status_code_valid = TRUE;
    resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
    /* set response error code to QMI_ERR_GENERAL_V01 */
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    resp.resp.error = QMI_ERR_GENERAL_V01;
    /*csd init falied*/
    qmi_csd_init  = QMI_CSD_INIT_FAILED;
  }
  else
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_init_req_handler: csd_init success" );
    /* Updating the CSD QMI INIT variable */
    qmi_csd_init  = QMI_CSD_INIT_DONE;
    /*Create the Sync event */
    if(!qmi_csd_sync_event)
    {
      apr_rc = csd_event_create( &qmi_csd_sync_event );
      if ( apr_rc != APR_EOK )
      {
        CSD_MSG(CSD_PRIO_ERROR,"csd_init_req_handler: Failed to create event  rc = 0x%x", apr_rc );
        csd_rc = CSD_ENORESOURCE;
        resp.qmi_csd_status_code_valid = TRUE;
        resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
        /* set response error code to QMI_ERR_GENERAL_V01 */
        resp.resp.result = QMI_RESULT_FAILURE_V01;
        resp.resp.error = QMI_ERR_GENERAL_V01;
        /*csd init falied*/
        qmi_csd_init  = QMI_CSD_INIT_FAILED; /* CSD init failed */
      }
      else
      {
        CSD_MSG(CSD_PRIO_HIGH,"csd_init_req_handler: QMI Sync Event is created  rc = 0x%x", apr_rc );
      }
    }
  }

  resp_err = qmi_csi_send_resp( req_handle, msg_id, &resp, sizeof(resp) );
  if(resp_err != QMI_CSI_NO_ERR)
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_init_req_handler: qmi_csi_send_resp failed for msg_id[0x%x], rc[0x%x]", msg_id, resp_err );
    qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  }
  else
  {
    CSD_MSG( CSD_PRIO_HIGH, "csd_init_req_handler: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
    QMI_CSD_RESP_SET_BUFFER(msg_id, csd_rc );
  }

  if( clock_voting_handle == NULL )
  {
    clock_voting_handle = npa_create_sync_client( "/clk/cpu", 
                                                  "csd_service", 
                                                  NPA_CLIENT_SUPPRESSIBLE ); /* cpu requests are suppressible */
    CSD_MSG( CSD_PRIO_MEDIUM, "Query clock npa handle[0x%x]", clock_voting_handle );
    /* Note:  npa_issue_scalar_request() and npa_complete_request() should be
              used to vote and devote for the frequency required. */
    /* TODO: "/clk/cpu" is not the best node to vote for, better to vote for
             "core/cpu" for MIPS and DCVS will determine the correct clock voting */
  }
  
  return qmi_rc;
}

/*=============================================================================
  FUNCTION csd_deinit_req_handler
=============================================================================*/
/*!
@brief
  This function is called when a QMI_CSD_DEINIT_REQ message is
  received by the service, sending a response message after CSD deinit.

@param[in]   req_c_struct        Decoded request message structure

@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
static qmi_csi_cb_error csd_deinit_req_handler
(
  client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
)
{
  qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
  qmi_csi_error resp_err;
  qmi_csd_deinit_resp_msg_v01 resp;
  uint32_t  csd_rc = CSD_EOK;
#ifdef FEATURE_AUDIO_RTPD_SERVICE
  uint32_t rtpd_rc = RTPD_EOK;
#endif

  memset( &resp, 0, sizeof(qmi_csd_deinit_resp_msg_v01) );
  
  if( QMI_CSD_INIT_DONE == qmi_csd_init )
  {
    csd_rc = csd_deinit();
    qmi_csd_client_info_list_dinit();
#ifdef FEATURE_AUDIO_RTPD_SERVICE
    rtpd_rc = rtpd_service_deinit();
#endif
  }
  else
  {
    CSD_MSG( CSD_PRIO_LOW, "csd_deinit_req_handler: QMI CSD not initialized yet" );
  }
  
#ifdef FEATURE_AUDIO_RTPD_SERVICE 
  if(rtpd_rc != RTPD_EOK)
  {
    RTPD_MSG( RTPD_PRIO_ERROR, "csd_init_req_handler: RTPD_service_deinit failed rc = 0x%x", rtpd_rc );
  } 
#endif

  /* For error case  */
  if( csd_rc != CSD_EOK ) 
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_deinit_req_handler: csd_deinit failed, rc[0x%x]", csd_rc );
    resp.qmi_csd_status_code_valid = TRUE;
    resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
    // set response error code to QMI_ERR_GENERAL_V01
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    resp.resp.error = QMI_ERR_GENERAL_V01;
    qmi_csd_init  = QMI_CSD_DINIT_FAILED;  
  }
  else
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_req_handler: csd_deinit success" );
    /*Updating the CSD QMI INIT variable*/
    qmi_csd_init  = QMI_CSD_DINIT_DONE; 
  }
  resp_err = qmi_csi_send_resp( req_handle, msg_id, &resp, sizeof(resp) );

  if( resp_err != QMI_CSI_NO_ERR )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_deinit_req_handler: qmi_csi_send_resp failed for msg_id[0x%x], rc[0x%x]", msg_id, resp_err );
    qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  }
  else
  {
    CSD_MSG( CSD_PRIO_HIGH, "csd_deinit_req_handler: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
    QMI_CSD_RESP_SET_BUFFER(msg_id, csd_rc );
  }
  
  if( clock_voting_handle != NULL )
  {
    npa_destroy_client( clock_voting_handle );
    clock_voting_handle = NULL;
  }
  
  return qmi_rc;
}

/*=============================================================================
  FUNCTION csd_close_req_handler
=============================================================================*/
/*!
@brief
  This function is called when a QMI_CSD_CLOSE_REQ message is received by
  the service.

@param[in]   req_c_struct        Decoded request message structure

@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
static qmi_csi_cb_error csd_close_req_handler
(
  client_info_type         *clnt_info,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
)
{
  qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
  qmi_csi_error resp_err;
  qmi_csd_close_resp_msg_v01 resp;
  uint32_t handle;
  uint32_t csd_rc;

  handle = ( (qmi_csd_close_req_msg_v01*)(req_c_struct) )->handle;
  CSD_MSG( CSD_PRIO_LOW, "csd_close_req_handler: Start closing handle[0x%x]", handle );
  csd_rc = csd_close( handle );

  memset( &resp, 0, sizeof(qmi_csd_close_resp_msg_v01) );

  if( csd_rc != CSD_EOK )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_close_req_handler: csd_close failed, rc[0x%x]", csd_rc );
    resp.resp.result = QMI_RESULT_FAILURE_V01;
    resp.resp.error = QMI_ERR_GENERAL_V01;
    resp.qmi_csd_status_code_valid = TRUE;
    resp.qmi_csd_status_code = (qmi_csd_status_v01) csd_rc;
    resp.handle_valid = FALSE;
  }
  else
  {
    CSD_MSG( CSD_PRIO_HIGH, "csd_close_req_handler: csd_close handle[0x%x] success", handle );
    resp.qmi_csd_status_code_valid = FALSE;
    resp.handle_valid = TRUE;
    resp.handle = handle;
  }

  resp_err = qmi_csi_send_resp( req_handle, msg_id, &resp, sizeof(resp) );
  if( resp_err != QMI_CSI_NO_ERR )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_close_req_handler: qmi_csi_send_resp failed for msg_id[0x%x], rc[0x%x]", msg_id, resp_err );
    qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  }
  else
  {
    CSD_MSG( CSD_PRIO_HIGH, "csd_close_req_handler: qmi_csi_send_resp success, msg_id[0x%x]", msg_id );
    QMI_CSD_RESP_SET_BUFFER(msg_id, csd_rc );
  }
  return qmi_rc;
}


/*=============================================================================
                    JUMP TABLE for Request Handler Functions
=============================================================================*/

/*=============================================================================
  FUNCTION csd_req_handle_table_get_ioctl
=============================================================================*/
/*!
@brief
  This function is called to get the respective CSD IOCTL from the QMI 
  message ID.

@param[in]   qmi_msg_id         QMI message ID

@retval    rc              Success, CSD IOCTL values. 
@retval    NULL            Error, No CSD IOCTL found for QMI message.

NOTE
    The table entry for request handler has to match corresponding
    request message here.
    QMI message ID starts from 0x20 for a service except for special required
    requests.
*/
/*=========================================================================*/
static uint32_t csd_req_handle_table_get_ioctl (uint32_t qmi_msg_id)
{
  uint32_t rc = CSD_EOK;

  qmi_msg_id = qmi_msg_id - QMI_CSD_MSG_ID_MIN;

  if( qmi_msg_id < ( sizeof(csd_req_handle_table) / sizeof(*csd_req_handle_table) ) )
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_req_handle_table_get_ioctl: qmi_msg_id[0x%x]", qmi_msg_id );
    rc = csd_req_handle_table[qmi_msg_id];
  }
  else
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_req_handle_table_get_ioctl: wrong input parameter, qmi_msg_id[0x%x]", qmi_msg_id);
    rc = NULL;
  }

  return rc;
}

/*=============================================================================
  FUNCTION qmi_csd_ioctl_call
=============================================================================*/
/*!
@brief
  This function calls the respective CSD IOCTL command handler function for 
  specific module depending upon the handle passed to the function.

@param[in]   ioctl_id            CSD IOCTL
@param[in]   connection_handle   connection handle used by infrastructure
                                 to communicate with each client-service
                                 connection.
@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.
@param[in]  msg_id               Message Id pertaining to a particular
                                 message.
@param[in]  req_c_struct         C struct with the decoded data.
@param[in]  req_c_struct_len     Length of the c struct.
@param[in]  service_cookie       Service specific data. Service cookie is
                                 registered with the infrastructure during
                                 service registration(qmi_csi_register).

@retval    QMI_CSI_CB_NO_ERR     Success
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
static qmi_csi_cb_error qmi_csd_ioctl_call
(
  uint32_t                 ioctl_id,
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  int32_t                  msg_id,
  void                     *req_c_struct,
  int32_t                  req_c_struct_len,
  void                     *service_cookie
)
{
  uint32_t hdl_type;
  qmi_csi_cb_error qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  client_info_type *clnt_info = (client_info_type*)connection_handle;
  uint32_t csd_handle;

  /* CSD handle is obtained from the QMI msg payload. The first 4 bytes of
     QMI msg will always be a handle for all the CSD IOCTL calls */
  csd_handle = *( (uint32_t*)req_c_struct );

  hdl_type = csd_handle_get_type( csd_handle );

  CSD_MSG(CSD_PRIO_MEDIUM, "qmi_csd_ioctl_call: handle[0x%x] handle type[0x%x]", csd_handle, hdl_type);

  switch(hdl_type)
  {
    case QMI_CSD_VOICE_STREAM :
      qmi_rc = qmi_csd_ioctl_vs_cmd_req_handler( csd_handle,
                                                 ioctl_id, 
                                                 clnt_info,
                                                 req_handle, 
                                                 msg_id, 
                                                 req_c_struct, 
                                                 req_c_struct_len, 
                                                 service_cookie );
    break;

    case QMI_CSD_VOICE_CONTEXT:
      qmi_rc = qmi_csd_ioctl_vc_cmd_req_handler( csd_handle, 
                                                 ioctl_id, 
                                                 clnt_info,
                                                 req_handle, 
                                                 msg_id, 
                                                 req_c_struct, 
                                                 req_c_struct_len,
                                                 service_cookie );
    break;

    case QMI_CSD_VOICE_MANAGER:
      qmi_rc = qmi_csd_ioctl_vm_cmd_req_handler( csd_handle, 
                                                 ioctl_id, 
                                                 clnt_info,
                                                 req_handle, 
                                                 msg_id,
                                                 req_c_struct, 
                                                 req_c_struct_len,
                                                 service_cookie );
    break;

    case QMI_CSD_DEVICE_CONTROL:
      qmi_rc = qmi_csd_ioctl_dev_cmd_req_handler( csd_handle,
                                                  ioctl_id, 
                                                  clnt_info,
                                                  req_handle, 
                                                  msg_id,
                                                  req_c_struct, 
                                                  req_c_struct_len,
                                                  service_cookie );
    break;

    default:
    break;
  }
  
  if( qmi_rc != QMI_CSI_CB_NO_ERR )
  {
    CSD_MSG( CSD_PRIO_ERROR, "qmi_csd_ioctl_call: ioctl_id(0x%x) returned failure, qmi_rc[0x%x]", ioctl_id, qmi_rc );
  }
  return qmi_rc;
}


/*=============================================================================
                    CALLBACK FUNCTIONS for handling request
=============================================================================*/

/*===========================================================================
  FUNCTION CSD_HANDLE_REQ_CB()

  DESCRIPTION
    This callback is invoked when the infrastructure receives an incoming
    request.  The infrastructure decodes the data and gives it to the services

  PARAMETERS
    @param[in]  connection_handle  connection handle used by infrastructure
                                   to communicate with each client-service
                                   connection.
    @param[in]  req_handle         Opaque handle provided by the infrastructure
                                   to specify a particular transaction and
                                   message.
    @param[in]  msg_id             Message Id pertaining to a particular
                                   message.
    @param[in]  req_c_struct       C struct with the decoded data.
    @param[in]  req_c_struct_len   Length of the c struct.
    @param[in]  service_cookie     Service specific data. Service cookie is
                                   registered with the infrastructure during
                                   service registration(qmi_csi_register).

  RETURN VALUE
    @retval    QMI_CSI_NO_ERR    Success
    @retval    QMI_IDL_...         Error, see error codes defined in qmi.h

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/

static qmi_csi_cb_error csd_handle_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  uint32_t                  msg_id,
  void                     *req_c_struct,
  uint32_t                  req_c_struct_len,
  void                     *service_cookie
)
{
  qmi_csi_cb_error qmi_rc = QMI_CSI_CB_INTERNAL_ERR;
  client_info_type *clnt_info = (client_info_type*)connection_handle;

  int32_t csd_ioctl_qmi_msg_id;

  /* A request is being handled, increment the service_cookie num_requests */
  ((service_context_type*)service_cookie)->num_requests++;

  /* Note that QMI message ID starts from QMI_CSD_MSG_ID_MIN */
  //mapped_msg_id = msg_id - QMI_CSD_MSG_ID_MIN;

  QMI_CSD_REQ_SET_BUFFER(msg_id, req_c_struct );
  CSD_MSG( CSD_PRIO_HIGH, "csd_handle_req_cb: msg_id[0x%x] received, req_handle[0x%x]", msg_id, req_handle );
  
  switch( msg_id )
  {
    case QMI_CSD_QUERY_DRIVER_VERSION_REQ_V01:
    {
      CSD_MSG( CSD_PRIO_HIGH, "csd_handle_req_cb: QMI_CSD_QUERY_DRIVER_VERSION_REQ_V01 called" );
      qmi_rc = csd_query_driver_version_req_handler( clnt_info, 
                                                     req_handle, 
                                                     msg_id, 
                                                     req_c_struct, 
                                                     req_c_struct_len, 
                                                     service_cookie );
    }
    break;

    case QMI_CSD_INIT_REQ_V01:
    {
      CSD_MSG( CSD_PRIO_HIGH, "csd_handle_req_cb: QMI_CSD_INIT_REQ_V01 called" );
      qmi_rc = csd_init_req_handler( clnt_info,
                                     req_handle,
                                     msg_id, 
                                     req_c_struct,
                                     req_c_struct_len, 
                                     service_cookie );
    }
    break;

    case QMI_CSD_DEINIT_REQ_V01:
    {
      CSD_MSG( CSD_PRIO_HIGH, "csd_handle_req_cb: QMI_CSD_DEINIT_REQ_V01 called");                 
      qmi_rc = csd_deinit_req_handler( clnt_info,
                                       req_handle,
                                       msg_id, 
                                       req_c_struct,
                                       req_c_struct_len, 
                                       service_cookie );
    }
    break;

    case QMI_CSD_CLOSE_REQ_V01:
    {
      CSD_MSG( CSD_PRIO_HIGH, "csd_handle_req_cb: QMI_CSD_CLOSE_REQ_V01 called");   
      if( QMI_CSD_INIT_DONE == qmi_csd_init )
      {
        qmi_rc = csd_close_req_handler( clnt_info,
                                        req_handle,
                                        msg_id, 
                                        req_c_struct,
                                        req_c_struct_len, 
                                        service_cookie );
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR,"csd_handle_req_cb: QMI_CSD_CLOSE_REQ_V01 error, CSD QMI Not Initialized!!");
      }
    }
    break;

    case QMI_CSD_OPEN_PASSIVE_CONTROL_VOICE_STREAM_REQ_V01:
    {
      if( QMI_CSD_INIT_DONE == qmi_csd_init )
      {
        qmi_rc = csd_open_passive_control_voice_stream_req_handler( clnt_info,
                                                                    req_handle,
                                                                    msg_id,
                                                                    req_c_struct,
                                                                    req_c_struct_len,
                                                                    service_cookie );
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_handle_req_cb: QMI_CSD_OPEN_PASSIVE_CONTROL_VOICE_STREAM_REQ_V01 error, CSD QMI Not Initialized!!" );
      }
    }
    break;

    case QMI_CSD_OPEN_FULL_CONTROL_VOICE_STREAM_REQ_V01:
    { 
      if( QMI_CSD_INIT_DONE == qmi_csd_init )
      {
        qmi_rc = csd_open_full_control_voice_stream_req_handler( clnt_info,
                                                                 req_handle,
                                                                 msg_id,
                                                                 req_c_struct,
                                                                 req_c_struct_len,
                                                                 service_cookie );
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_handle_req_cb: QMI_CSD_OPEN_FULL_CONTROL_VOICE_STREAM_REQ_V01 error, CSD QMI Not Initialized!!" );
      }
    }
    break;


    case QMI_CSD_OPEN_VOICE_CONTEXT_REQ_V01:
    {
      if( QMI_CSD_INIT_DONE == qmi_csd_init )
      {
        qmi_rc = csd_open_voice_context_req_handler( clnt_info, 
                                                     req_handle, 
                                                     msg_id,
                                                     req_c_struct, 
                                                     req_c_struct_len,
                                                     service_cookie );
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_handle_req_cb: QMI_CSD_OPEN_VOICE_CONTEXT_REQ_V01 error, CSD QMI Not Initialized!!" );
      }
    }
    break;


    case QMI_CSD_OPEN_VOICE_MANAGER_REQ_V01:
    {
      if( QMI_CSD_INIT_DONE == qmi_csd_init )
      {
        qmi_rc = csd_open_voice_manager_req_handler( clnt_info,
                                                     req_handle,
                                                     msg_id,
                                                     req_c_struct, 
                                                     req_c_struct_len,
                                                     service_cookie );
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_handle_req_cb: QMI_CSD_OPEN_VOICE_MANAGER_REQ_V01 error, CSD QMI Not Initialized!!" );
      }
    }
    break;

    case QMI_CSD_OPEN_DEVICE_CONTROL_REQ_V01:
    {
      CSD_MSG( CSD_PRIO_HIGH, "csd_handle_req_cb: QMI_CSD_OPEN_DEVICE_CONTROL_REQ_V01 called");  
      if( QMI_CSD_INIT_DONE == qmi_csd_init )
      {
        qmi_rc = csd_open_device_control_req_handler( clnt_info,
                                                      req_handle,
                                                      msg_id, 
                                                      req_c_struct, 
                                                      req_c_struct_len,
                                                      service_cookie );
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_handle_req_cb: QMI_CSD_OPEN_DEVICE_CONTROL_REQ_V01 error, CSD QMI Not Initialized!!" );
      }
    }
    break;

    case QMI_CSD_VOICE_CONFIG_REQ_V01:
    case QMI_CSD_VOICE_START_REQ_V01:
    case QMI_CSD_VOICE_END_REQ_V01:
    case QMI_CSD_VOICE_DEVICE_SWITCH_REQ_V01:
    case QMI_CSD_AFE_LOOPBACK_REQ_V01:
    case QMI_CSD_VOICE_VOLUME_CONTROL_REQ_V01:
    case QMI_CSD_VOICE_MUTE_REQ_V01:
    case QMI_CSD_DTMF_GENERATION_REQ_V01:
    case QMI_CSD_VOICE_DTMF_DETECTION_REQ_V01:
    case QMI_CSD_SET_VOICE_FEATURE_REQ_V01:
    case QMI_CSD_VOICE_RECORD_START_REQ_V01:
    case QMI_CSD_VOICE_RECORD_END_REQ_V01:
    case QMI_CSD_VOICE_PLAYBACK_START_REQ_V01:
    case QMI_CSD_VOICE_PLAYBACK_END_REQ_V01:
      csd_lite_init = TRUE;
      qmi_csdlite_handle_req( clnt_info, req_handle, msg_id, req_c_struct, req_c_struct_len, service_cookie );
    break;
    
    default:
      if( QMI_CSD_INIT_DONE == qmi_csd_init )
      {
        csd_ioctl_qmi_msg_id = csd_req_handle_table_get_ioctl( msg_id );
    
        if( csd_ioctl_qmi_msg_id != NULL )
        {
          CSD_MSG( CSD_PRIO_MEDIUM, "csd_handle_req_cb: Translated CSD IOCTL ID[0x%x] from msg_id[0x%x]", csd_ioctl_qmi_msg_id, msg_id );
          qmi_rc = qmi_csd_ioctl_call( csd_ioctl_qmi_msg_id, 
                                       clnt_info, 
                                       req_handle, 
                                       msg_id, 
                                       req_c_struct,
                                       req_c_struct_len, 
                                       service_cookie );
        }
        else
        {
          CSD_MSG( CSD_PRIO_ERROR, "csd_handle_req_cb: Unable to retrive CSD IOCTL ID from msg_id[0x%x]", qmi_rc );
        }
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_handle_req_cb: CSD QMI Not Initialized!!" );
      }
    break;
  }

#if 0  
  /* Verify that the msg_id received is not greater than the largest known
     entry in jump table.*/
  if((mapped_msg_id>=0) &&
     (mapped_msg_id < (sizeof(csd_req_handle_table) / sizeof(*csd_req_handle_table))))
  {
    /* If the jump table has a valid entry at this mapped message ID,
       call the handler function */
    if(csd_req_handle_table[mapped_msg_id])
    {
      rc = csd_req_handle_table[mapped_msg_id] (clnt_info, req_handle,
                                                msg_id, req_c_struct,
                                                req_c_struct_len, service_cookie);
    }else{
      //printf("No function defined to handle request for message ID: %d",msg_id);
    }
  }else{
    //printf("Message ID: %d greater than maximum known message ID.",msg_id);
  }
#endif

  return qmi_rc;
}/* csd_handle_req_cb */


/*=============================================================================
                    CALLBACK FUNCTIONS for Connect/Disconnect
=============================================================================*/

/*===========================================================================
  FUNCTION CSD_CONNECT_CB()

  DESCRIPTION
    The QCSI framework calls this callback function when it receives connect
    from each client.

  PARAMETERS
    @param[in]   client_handle   Opaque handle used by the infrastructure to
                                 identify different clients.
    @param[in]   service_cookie  Service specific data.  Service cookie is
                                 registered with the infrastructure during
                                 service registration(qmi_csi_register).
    @param[out]  connection_handle
                                 Services return this handle to facilitate
                                 the infrastructure about the client
                                 connection/communication.

  RETURN VALUE
    @retval    QMI_CSI_NO_ERR    Success
    @retval    QMI_CSI_CONN_REFUSED
                                 This error occurs when  limit on MAX number
                                 of clients a service can support is reached.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error csd_connect_cb
(
  qmi_client_handle         client_handle,
  void                      *service_cookie,
  void                      **connection_handle
  )
{
  /* This is where the client handle can be stored if the service wants
     to keep some state information about clients. The connection handle can
     also be modified to facilitate communication with the infrastructure */
  client_info_type *clnt_info;

  CSD_MSG( CSD_PRIO_MEDIUM, "csd_connect_cb: CSD QMI client connected!!!" );

  QMI_CSD_REQ_SET_BUFFER(CSD_CLIENT_CONNECT_REQ, service_cookie );

  clnt_info = (client_info_type*)malloc(sizeof(client_info_type));
  /* Will be freed in csd_disconnect_cb */
  if(!clnt_info)
  {
    QMI_CSD_RESP_SET_BUFFER(CSD_CLIENT_CONNECT_REQ, CSD_EFAILED );
    return QMI_CSI_CB_CONN_REFUSED;
  }    
  
  clnt_info->clnt = client_handle;
  *connection_handle = clnt_info;
  
  QMI_CSD_RESP_SET_BUFFER(CSD_CLIENT_CONNECT_REQ, CSD_EOK );
  
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION CSD_DISCONNECT_CB()

  DESCRIPTION
    The QCSI framework calls this callback function when each client is
    disconnected from the service.

  PARAMETERS
    @param[in]  connection_handle  connection handle used by infrastructure
                                   to communicate with each client-service
                                   connection.
    @param[in]  service_cookie     Service specific data. Service cookie is
                                   registered with the infrastructure during
                                   service registration(qmi_csi_register).

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void csd_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_cookie
 )
{
  CSD_MSG( CSD_PRIO_MEDIUM, "csd_disconnect_cb: CSD QMI client disconnected!!!"
           " call_continue = %d", csd_debug_call_continue );
  
  QMI_CSD_REQ_SET_BUFFER(CSD_CLIENT_DISCONNECT_REQ, service_cookie );

  if ( FALSE == csd_debug_call_continue )
  {
    if( csd_lite_init )
    {
      csd_lite_init = 0;
      csdlite_cleanup();
    }

    /* call csd_deinit to clean up session */
    if( QMI_CSD_INIT_DONE == qmi_csd_init )
    {
      csd_deinit();
      qmi_csd_client_info_list_dinit();
#ifdef FEATURE_AUDIO_RTPD_SERVICE
      rtpd_service_deinit();
#endif
      qmi_csd_init = QMI_CSD_DINIT_DONE;
    }
  
  }
  free( connection_handle ); /* Malloc in csd_connect_cb */
  
  QMI_CSD_RESP_SET_BUFFER(CSD_CLIENT_DISCONNECT_REQ, CSD_EOK );
  
  return;
}

/*=============================================================================
               Registration APIs
=============================================================================*/

/*=============================================================================
  FUNCTION qmi_csd_register_service

  DESCRIPTION
    Register the Core Sound Driver Service with QCSI framework.

  PARAMETERS
    OS specific parameters for signalling

  RETURN VALUE
    Handle to service provider (when registration succeed).

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
=============================================================================*/
void *qmi_csd_register_service(qmi_csi_os_params *os_params)
{
  qmi_idl_service_object_type csd_service_object = csd_get_service_object_v01();
  qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;

  qmi_client_os_params qsap_os_params;

  qsap_os_params.sig = QMI_CLNT_WAIT_SIG;
  qsap_os_params.timer_sig = QMI_CLNT_TIMER_SIG;

  rc = qmi_csi_register( csd_service_object, 
                         csd_connect_cb,
                         csd_disconnect_cb, 
                         csd_handle_req_cb, 
                         &service_cookie, 
                         os_params,
                         &service_cookie.service_handle );
                         
  rc |= qmi_sap_register( csd_service_object, &qsap_os_params, &qsap_handle );
  if( rc != QMI_CSI_NO_ERR )
  {
    return NULL;
  }
  return service_cookie.service_handle;
}
