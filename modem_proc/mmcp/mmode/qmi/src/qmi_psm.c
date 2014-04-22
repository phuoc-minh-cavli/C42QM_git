/*===========================================================================

                         Q M I _ PSM . C

DESCRIPTION

 The Qualcomm Network Access Services MSM Interface source file.

EXTERNALIZED FUNCTIONS

  qmi_psm_init()
    Register the Power Save Mode Service with QMUX for all applicable QMI links.

Copyright (c) 2004-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================
    
                      EDIT HISTORY FOR FILE

  $Header: //components/rel/mmcp.mpss/6.1.5/mmode/qmi/src/qmi_psm.c

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/22/20    rdprasad     Created file
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stringl/stringl.h>
#include "ps_byte.h"

#include "sys.h"
#include "cm.h"
#include "nv.h"
#include "rex.h"
#include "cmll_v.h"

#include "qmi_svc_utils.h"
#include "qmi_psm.h"
#include "modem_mem.h"
#include "qmi_mmode_taski.h"
#ifdef FEATURE_MMODE_TX_21
#include "cmsnsmgr.h"
#endif

#define PSM_MASK_MAX PSM_MASK_SENSOR_ID_TILT_DETECT_V01 | PSM_MASK_SENSOR_ID_FMD_V01 | PSM_MASK_SENSOR_ID_PRESSURE_V01 |PSM_MASK_SENSOR_ID_HUMIDITY_V01 | PSM_MASK_SENSOR_ID_TEMPERATURE_V01 | PSM_MASK_SENSOR_ID_ALS_V01
        
#define smd_perceptron_weight 20
#define smd_perceptron_min_feature_weight 4
#define smd_perceptron_max_feature_weight 4

/*===========================================================================

                               DEFINITIONS

===========================================================================*/



/*===========================================================================

                                DATA TYPES

===========================================================================*/



/*---------------------------------------------------------------------------
  PSM Command enum type - not equal to the actual command values!
  mapping is in qmi_psm_cmd_callbacks table

  DO NOT REORDER THIS ENUM!  (make sure the command value here = index into
  qmi_psmi_cmd_callbacks table for corresponding command)
---------------------------------------------------------------------------*/
typedef enum
{
  PSMI_CMD_MIN                                = 0x00,
    
  PSMI_CMD_INDICATION_REGISTER                = PSMI_CMD_MIN,
  PSMI_GET_CFG_PARAMS,
  PSMI_ENTER,
  PSMI_SET_CFG_PARAMS,
  PSMI_GET_CAPABILITY,
#ifdef FEATURE_REF_TRACKER
  PSMI_SET_SENSOR_CONFIGURATION,
  PSMI_GET_SENSOR_DATA,
  PSMI_RESET_SENSOR_CONFIG_DATA,
  PSMI_GET_LAST_SENSOR_POLICY_MET_INFO,
#ifdef FEATURE_MMODE_TX_21
  PSMI_SET_SENSOR_CONFIGURATION_EXT,
  PSMI_GET_SENSOR_CONFIGURATION,
  PSMI_GET_FMD_SNS_SAMPLE,
  PSMI_PHY_SNS_TEST,
  PSMI_PHY_SNS_TEST_GET_DATA,
  PSMI_SNS_SET_LOGGING_LEVEL,
  PSMI_GET_SNS_SERIAL_NUMBER,
#endif
#endif

  PSMI_CMD_MAX,
  PSMI_CMD_WIDTH                              = 0xFFFF

} qmi_psmi_cmd_e_type;



/*===========================================================================*/

/*---------------------------------------------------------------------------
  QMI_PSM service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

#ifdef _WIN32
#define PSM_HDLR_PROTOTYPE(x)   qmi_csi_cb_error x( qmi_mmode_svc_info_type*, qmi_mmode_qcsi_transaction_type*, void*, void*, unsigned int )
#else
#define PSM_HDLR_PROTOTYPE(x)   qmi_csi_cb_error x( qmi_mmode_svc_info_type*, qmi_mmode_qcsi_transaction_type*, void*, void*, unsigned int )__attribute__((unused))
#endif

static PSM_HDLR_PROTOTYPE(qmi_psm_indication_register);
static PSM_HDLR_PROTOTYPE(qmi_psm_get_cfg_params);
static PSM_HDLR_PROTOTYPE(qmi_psm_enter);
static PSM_HDLR_PROTOTYPE(qmi_psm_set_cfg_params);
static PSM_HDLR_PROTOTYPE(qmi_psm_get_capability);
#ifdef FEATURE_REF_TRACKER
static PSM_HDLR_PROTOTYPE(qmi_psm_set_sensor_configuration);
#ifdef FEATURE_MMODE_TX_21
static PSM_HDLR_PROTOTYPE(qmi_psm_get_sensor_configuration);
#endif
static PSM_HDLR_PROTOTYPE(qmi_psm_get_sensor_data);
static PSM_HDLR_PROTOTYPE(qmi_psm_reset_sensor_config_data);
static PSM_HDLR_PROTOTYPE(qmi_psm_get_last_policy_met_info);
#ifdef FEATURE_MMODE_TX_21
static PSM_HDLR_PROTOTYPE(qmi_psm_set_sensor_configuration_ext);
static PSM_HDLR_PROTOTYPE(qmi_psm_get_fmd_sns_sample_req);
static PSM_HDLR_PROTOTYPE(qmi_psm_phy_sns_test_req);
static PSM_HDLR_PROTOTYPE(qmi_psm_phy_sns_test_get_data_req);
static PSM_HDLR_PROTOTYPE(qmi_psm_set_sensor_logging_level);
static PSM_HDLR_PROTOTYPE(qmi_psm_get_sns_serial_num_req);
#endif
#endif

#define PSM_HDLR(a,b)  QMI_MMODE_SVC_MSG_HDLR( (uint16)a, (qmi_mmode_svc_msg_hdlr_ftype) b )

static qmi_mmode_svc_cmd_hdlr_type  qmi_psmi_cmd_callbacks[PSMI_CMD_MAX] =
{
    #ifndef REMOVE_QMI_PSM_INDICATION_REGISTER_V01
    PSM_HDLR( QMI_PSM_INDICATION_REGISTER_REQ_V01,                       qmi_psm_indication_register ),
    #endif
    #ifndef REMOVE_QMI_PSM_GET_CFG_PARAMS_V01
    PSM_HDLR( QMI_PSM_GET_CFG_PARAMS_REQ_V01,                       qmi_psm_get_cfg_params ),
    #endif
    #ifndef REMOVE_QMI_PSM_ENTER_V01
    PSM_HDLR( QMI_PSM_ENTER_REQ_V01,                       qmi_psm_enter ),
    #endif
    #ifndef REMOVE_QMI_PSM_SET_CFG_PARAMS_V01
    PSM_HDLR( QMI_PSM_SET_CFG_PARAMS_REQ_V01,                       qmi_psm_set_cfg_params ),
    #endif
    #ifndef REMOVE_QMI_PSM_GET_CAPABILITY_V01
    PSM_HDLR( QMI_PSM_GET_CAPABILITY_REQ_V01,                       qmi_psm_get_capability ),
    #endif
#ifdef FEATURE_REF_TRACKER
    #ifndef REMOVE_QMI_PSM_SET_SENSOR_CONFIGURATION_V01
    PSM_HDLR( QMI_PSM_SET_SENSOR_CONFIGURATION_REQ_V01,                       qmi_psm_set_sensor_configuration ),
    #endif
    #ifndef REMOVE_QMI_PSM_GET_SENSOR_DATA_V01
    PSM_HDLR( QMI_PSM_GET_SENSOR_DATA_REQ_V01,                       qmi_psm_get_sensor_data ),
    #endif
    #ifndef REMOVE_QMI_PSM_RESET_SENSOR_CONFIG_DATA_V01
    PSM_HDLR( QMI_PSM_RESET_SENSOR_CONFIG_DATA_REQ_V01,                       qmi_psm_reset_sensor_config_data ),
    #endif
    #ifndef REMOVE_QMI_PSM_GET_LAST_POLICY_MET_INFO_V01
    PSM_HDLR( QMI_PSM_GET_LAST_SENSOR_POLICY_MET_INFO_REQ_V01,                       qmi_psm_get_last_policy_met_info ),
    #endif
#ifdef FEATURE_MMODE_TX_21
    #ifndef REMOVE_QMI_PSM_SET_SENSOR_CONFIGURATION_EXT_V01
    PSM_HDLR( QMI_PSM_SET_SENSOR_CONFIGURATION_EXT_REQ_V01,                       qmi_psm_set_sensor_configuration_ext ),
    #endif
    #ifndef REMOVE_QMI_PSM_GET_SENSOR_CONFIGURATION_V01
    PSM_HDLR( QMI_PSM_GET_SENSOR_CONFIGURATION_REQ_V01,                       qmi_psm_get_sensor_configuration ),
    #endif
	#ifndef REMOVE_QMI_PSM_GET_FMD_SNS_SAMPLE_V01
    PSM_HDLR( QMI_PSM_GET_FMD_SNS_SAMPLE_REQ_V01,                       qmi_psm_get_fmd_sns_sample_req ),
    #endif
	#ifndef REMOVE_QMI_PSM_PHY_SNS_TEST_REQ_V01
    PSM_HDLR( QMI_PSM_PHY_SNS_TEST_REQ_V01,                       qmi_psm_phy_sns_test_req ),
    #endif
	#ifndef REMOVE_QMI_PSM_PHY_SNS_TEST_GET_DATA_REQ_V01
    PSM_HDLR( QMI_PSM_PHY_SNS_TEST_GET_DATA_REQ_V01,                       qmi_psm_phy_sns_test_get_data_req ),
    #endif
	#ifndef REMOVE_QMI_PSM_SENSOR_LOGGING_REQ_V01
    PSM_HDLR( QMI_PSM_SENSOR_LOGGING_REQ_V01,                       qmi_psm_set_sensor_logging_level ),
    #endif  
	#ifndef REMOVE_QMI_PSM_GET_SNS_SERIAL_NUMBER_REQ_V01
    PSM_HDLR( QMI_PSM_GET_SNS_SERIAL_NUMBER_REQ_V01,                       qmi_psm_get_sns_serial_num_req ),
    #endif
  
#endif
#endif
    
};

//===========================================================================
// GLOBALS
//===========================================================================
qmi_psmi_global_s_type qmi_psmi_global;

qmi_mmode_svc_info_type  qmi_psm_state;
static qmi_mmode_svc_config_type  qmi_psmi_cfg;


/*===========================================================================

                   NEW FW FUNCTIONS

===========================================================================*/




/*===========================================================================
  FUNCTION qmi_psmi_get_cl_sp_by_cmd_buf()

  DESCRIPTION
    Retrieve client sp from the command buffer

  PARAMETERS
    cmd_buf_p_in  :  command buffer

  RETURN VALUE
    cl_sp   :  client sp

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void* qmi_psmi_get_cl_sp_by_cmd_buf( qmi_mmode_qcsi_transaction_type* cmd_buf_p )
{

  return cmd_buf_p->clnt_info_ptr;
}

/*===========================================================================
  FUNCTION qmi_psmi_send_response()

  DESCRIPTION
    Send the response to framework

  PARAMETERS
    clid: uint8

  RETURN VALUE
    TRUE if sent successfully, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_psmi_send_response
(
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void*                            resp_msg,
  unsigned int                     resp_len
)
{

  qmi_csi_error                  err    = QMI_CSI_NO_ERR;

  if ( cmd_buf_p == NULL )
  {
    return FALSE;
  }

  /* Send the response */
  err = qmi_mmode_qcsi_send_resp(&qmi_psm_state, &cmd_buf_p, resp_msg, resp_len);

  return ((err == QMI_CSI_NO_ERR)? TRUE: FALSE);
}

/*===========================================================================
  FUNCTION qmi_psmi_send_ind()

  DESCRIPTION
    This function is calls the QMI Framework API to send out the PSM service
    indication to client.
===========================================================================*/
qmi_csi_error qmi_psmi_send_ind
(
  qmi_psmi_client_state_type*         cl_sp, 
  unsigned int                        msg_id, 
  void*                               ind_msg,
  unsigned int                        ind_len
)
{

  qmi_csi_error                       err    = QMI_CSI_NO_ERR;

  if ( cl_sp )
  {
    err = qmi_mmode_qcsi_send_ind(cl_sp->clnt, msg_id, ind_msg, ind_len);
  }
  return err;
}

/*===========================================================================
  FUNCTION qmi_psm_get_cm_client_id()

  DESCRIPTION
    Returns the client id

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static cm_client_id_type qmi_psm_get_cm_client_id(void)
{
  return qmi_psmi_global.cm_client_id;
}

/*===========================================================================
  FUNCTION qmi_psm_cm_if_init()

  DESCRIPTION
    Initialize CM interface for QMI PSM service

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_psm_cm_if_init(void)
{

  /* Initialize the CM client */
  if( CM_CLIENT_OK != cm_client_init( CM_CLIENT_TYPE_RM_NETWORK_INTERFACE,
                                      &qmi_psmi_global.cm_client_id) )
  {
    QM_MSG_ERROR("CM client initialization failed!");
    return;
  }

  QM_MSG_MED_1("client_id = %d", qmi_psm_get_cm_client_id());

  /* Activate QMI CM client */
  if ( cm_client_act( qmi_psm_get_cm_client_id()) != CM_CLIENT_OK )
  {
    QM_MSG_HIGH_1("Fail to activate CM client %d", qmi_psm_get_cm_client_id());
    return;
  }
}

/*===========================================================================
  FUNCTION qmi_psmi_global_init()

  DESCRIPTION
    Process init from Framework
===========================================================================*/
void qmi_psmi_global_init( void )
{
  
  /*-------------------------------------------------------------------------
  Initialize the global PSM service parameters
  -------------------------------------------------------------------------*/
  if(qmi_psmi_global.inited == FALSE)
  {
    
    memset(&qmi_psmi_global, 0, sizeof(qmi_psmi_global));
    qmi_psmi_global.inited = TRUE;
    qmi_psm_cm_if_init();

    q_init(&qmi_psmi_global.get_cfg_params_req_buf_p);
    #ifdef FEATURE_REF_TRACKER
    q_init(&qmi_psmi_global.sns_get_config_cmd_buf_p);
    q_init(&qmi_psmi_global.sns_get_data_cmd_buf_p);
    q_init(&qmi_psmi_global.sns_reset_config_data_cmd_buf_p);
    #endif
    
    qmi_psmi_global.psm_capability = PSM_VERSION_2_V01;
    
  }

}

static int qmi_psm_qcsi_compare_pending_req_for_client
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_psmi_client_state_type *clnt_info_ptr;
  qmi_mmode_qcsi_transaction_type *req_node;
  qmi_client_handle clnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((item_ptr == NULL) || (compare_val == NULL))
  {
    return FALSE;
  }
  req_node = (qmi_mmode_qcsi_transaction_type *)item_ptr;
  clnt_info_ptr = (qmi_psmi_client_state_type *)req_node->clnt_info_ptr;
  clnt = (qmi_client_handle)compare_val;

  return ( (clnt_info_ptr->clnt == (clnt))? 1 : 0 );
} /* qmi_psm_qcsi_compare_pending_req_for_client */


static int qmi_psm_compare_pending_req_addr_for_client
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_psmi_client_state_type *clnt_info_ptr;
  qmi_psm_pending_cmd_buf_addr_s_type *req_node;
  qmi_mmode_qcsi_transaction_type     *pend_req;
  qmi_client_handle clnt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if((item_ptr == NULL) || (compare_val == NULL))
  {
    return FALSE;
  }
  req_node = (qmi_psm_pending_cmd_buf_addr_s_type *)item_ptr;
  pend_req = (qmi_mmode_qcsi_transaction_type *)req_node->cmd_buf_addr;
  clnt_info_ptr = (qmi_psmi_client_state_type *)pend_req->clnt_info_ptr;
  clnt = (qmi_client_handle)compare_val;

  return ( (clnt_info_ptr->clnt == (clnt))? 1 : 0 );
} /* qmi_psm_compare_pending_req_addr_for_client */

/*===========================================================================
  FUNCTION qmi_psmi_reset_client()

  DESCRIPTION
    Resets the state for the given client
    
  PARAMETERS
    cl_sp_in :  client info handle
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_psmi_reset_client
(
  qmi_mmode_svc_info_type      *svc_ptr,
  qmi_client_handle             clnt
)
{

  qmi_mmode_qcsi_transaction_type  *pending_req = NULL;
  qmi_mmode_qcsi_transaction_type  *pending_req_buf_p = NULL;
  int num_deleted_pending_txs = 0;

  if(q_cnt(&qmi_psmi_global.get_cfg_params_req_buf_p) > 0)
  {
    do
    {
      pending_req_buf_p = q_linear_delete_new(&qmi_psmi_global.get_cfg_params_req_buf_p,
                                        qmi_psm_qcsi_compare_pending_req_for_client,
                                        (void*)&clnt,
                                         NULL,
                                         NULL );
      if (pending_req_buf_p != NULL)
      {
        modem_mem_free( pending_req_buf_p, MODEM_MEM_CLIENT_QMI );
      }
    }while(pending_req_buf_p != NULL);
  }

  #ifdef FEATURE_REF_TRACKER
  if(q_cnt(&qmi_psmi_global.sns_get_data_cmd_buf_p) > 0)
  {
    do
    {
      pending_req_buf_p = q_linear_delete_new(&qmi_psmi_global.sns_get_data_cmd_buf_p,
                                        qmi_psm_qcsi_compare_pending_req_for_client,
                                        (void*)&clnt,
                                         NULL,
                                         NULL );
      if (pending_req_buf_p != NULL)
      {
        modem_mem_free( pending_req_buf_p, MODEM_MEM_CLIENT_QMI );
      }
    }while(pending_req_buf_p != NULL);
  }

  if(q_cnt(&qmi_psmi_global.sns_get_config_cmd_buf_p) > 0)
  {
    do
    {
      pending_req_buf_p = q_linear_delete_new(&qmi_psmi_global.sns_get_config_cmd_buf_p,
                                        qmi_psm_qcsi_compare_pending_req_for_client,
                                        (void*)&clnt,
                                         NULL,
                                         NULL );
      if (pending_req_buf_p != NULL)
      {
        modem_mem_free( pending_req_buf_p, MODEM_MEM_CLIENT_QMI );
      }
    }while(pending_req_buf_p != NULL);
  }

  if(q_cnt(&qmi_psmi_global.sns_reset_config_data_cmd_buf_p) > 0)
  {
    do
    {
      pending_req_buf_p = q_linear_delete_new(&qmi_psmi_global.sns_reset_config_data_cmd_buf_p,
                                        qmi_psm_qcsi_compare_pending_req_for_client,
                                        (void*)&clnt,
                                         NULL,
                                         NULL );
      if (pending_req_buf_p != NULL)
      {
        modem_mem_free( pending_req_buf_p, MODEM_MEM_CLIENT_QMI );
      }
    }while(pending_req_buf_p != NULL);
  }
  #endif
  
  /* Delete any pending transactions for this client */
  if(q_cnt(&svc_ptr->pending_req) > 0)
  {
    do
    {
      pending_req = q_linear_delete_new(&svc_ptr->pending_req,
                                        qmi_psm_qcsi_compare_pending_req_for_client,
                                        (void*)&clnt,
                                         NULL,
                                         NULL );
      if (pending_req != NULL)
      {
        modem_mem_free( pending_req, MODEM_MEM_CLIENT_QMI );
        num_deleted_pending_txs++;
      }
    }while(pending_req != NULL);
  }
  
  if( num_deleted_pending_txs > 0 )
  {
    QM_MSG_HIGH_2("qmi_psmi_reset_client: Deleted pending txs %d, of clnt 0x%p",
                   num_deleted_pending_txs,
                   clnt);
  }

} /* qmi_psmi_reset_client() */

/*===========================================================================
  FUNCTION qmi_psm_qcsi_svc_disconnect_cb()

  DESCRIPTION
    CallBack function to be registered with QCSI for disconnecting a client

  PARAMETERS
    connection_handle : QCSI client connection handle
    service_handle : QCSI service handle

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_psm_qcsi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_psmi_client_state_type       *clnt_info_ptr;
  qmi_mmode_svc_info_type          *svc_ptr;
  qmi_csi_error                     err = QMI_CSI_NO_ERR;
  qmi_mmode_qcsi_transaction_type  *pending_req = NULL;
  void                             *temp_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(connection_handle && service_handle);
  svc_ptr = (qmi_mmode_svc_info_type *)service_handle;
  clnt_info_ptr = (qmi_psmi_client_state_type *)connection_handle;

  do
  {
    if( svc_ptr == NULL || svc_ptr->service_type != QMI_MMODE_SERVICE_PSM ||
        svc_ptr->service_handle != qmi_psm_state.service_handle )
    {
      err = QMI_CSI_INVALID_HANDLE;
      QM_MSG_HIGH("Invalid service received");
      break;
    }
    
    if ( TRUE != q_delete_ext( &svc_ptr->client_q,
                               &clnt_info_ptr->link ) )
    {
      err = QMI_CSI_CONN_REFUSED;
      break;
    }

    qmi_psmi_reset_client(svc_ptr, clnt_info_ptr->clnt);
    
  } while(0);

  if( err == QMI_CSI_NO_ERR )
  {
    modem_mem_free( clnt_info_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
  }
  
  QM_MSG_HIGH_2("psm_qcsi_disconnect_cb: Releasing client 0x%p err %d",
                  connection_handle, err);
} /* qmi_psm_qcsi_svc_disconnect_cb() */

/*===========================================================================
  FUNCTION psm_alloc_cl_sp()

  DESCRIPTION
    Allocate the client data buffer

  PARAMETERS
    clid: uint8

  RETURN VALUE
    ptr to malloc'ed client pointer in case of success. NULL otherwise.
===========================================================================*/
static qmi_psmi_client_state_type* psm_alloc_cl_sp
( 
  qmi_client_handle          client_handle 
)
{
  qmi_psmi_client_state_type *cl_sp = NULL;

  QM_MSG_MED_1("In psm_alloc_cl_sp(): client_handle = %p", client_handle);

  cl_sp = QMI_PSM_MEM_ALLOC(sizeof(qmi_psmi_client_state_type));
  
  if ( cl_sp )
  {
    memset( cl_sp, 0x00, sizeof(qmi_psmi_client_state_type) );
    (void) q_link ( cl_sp, &cl_sp->link );
    cl_sp->clnt = client_handle;
  }
  else
  {
   QM_MSG_ERROR("Can't allocate memory");
  }

  return cl_sp;
  }

int qmi_psm_qcsi_compare_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_psmi_client_state_type *clnt_info = (qmi_psmi_client_state_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
} /* qmi_psm_qcsi_compare_client_handle */

/*===========================================================================
  FUNCTION qmi_psm_qcsi_svc_connect_cb()

  DESCRIPTION
    CallBack function to be registered with QCSI for connecting a client

  PARAMETERS
    Client Handle
    Service Handle
    Connection Handle

  RETURN VALUE
    QMI_ERR_NONE - Success
    QMI_CSI_CB_CONN_REFUSED - Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_qcsi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{

  qmi_psmi_client_state_type        *cl_sp = NULL;
  qmi_csi_cb_error                   err = QMI_CSI_CB_NO_ERR;
  qmi_mmode_svc_info_type           *svc_ptr;

  ASSERT(connection_handle && service_handle);

  svc_ptr = (qmi_mmode_svc_info_type *)service_handle;

  do
  {
    if( svc_ptr == NULL || svc_ptr->service_type != QMI_MMODE_SERVICE_PSM ||
        svc_ptr->service_handle != qmi_psm_state.service_handle )
    {
      err = QMI_CSI_CB_UNSUPPORTED_ERR;

      break;
    }
    if ( NULL != q_linear_search( &svc_ptr->client_q,
                                    qmi_psm_qcsi_compare_client_handle,
                                    client_handle ) )
    {
      QM_MSG_ERROR_1("psm_qcsi_connect_cb: client 0x%p present", client_handle);
      err = QMI_CSI_CB_CONN_REFUSED;
      break;
    }

    if( q_cnt(&svc_ptr->client_q) == PSMI_MAX_CLIDS )
    {
      QM_MSG_ERROR_1("psm_qcsi_connect_cb: max num clients reached rejected client_handle %p", 
                        client_handle);
      err = QMI_CSI_CB_CONN_REFUSED;
      break;
    }
  
    cl_sp = psm_alloc_cl_sp( client_handle );
    
    if ( cl_sp )
    {
      /* Add client node to the queue of clients */
      q_put( &(svc_ptr->client_q), &(cl_sp->link) );
      *connection_handle = cl_sp;
      QM_MSG_HIGH_2("psm_qcsi_connect_cb: Alloc client 0x%p client_handle %p", 
                      cl_sp, client_handle);
  
    }
    else
    {
      err = QMI_CSI_CB_NO_MEM;
    }
  } while(0);

  if (err != QMI_CSI_CB_NO_ERR)
  {  
    QM_MSG_ERROR_3("psm_qcsi_connect_cb: Error processing err = %d client_handle %p svc handle %p", 
                    err, client_handle, service_handle);
  }
  else
  {
    QM_MSG_HIGH_2("psm_qcsi_connect_cb: client connect client_handle %p connection_handle %p",
                    client_handle, *connection_handle);
  }

  return err;
  
}/* qmi_psm_qcsi_svc_connect_cb */

/*===========================================================================
  FUNCTION qmi_psmi_process_cmd()

  DESCRIPTION
    This function processes a QMI PSM command or event.

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI PSM command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI PSM must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_psmi_process_cmd
(
  qmi_mmode_cmd_enum_type  cmd,
  void                    *user_data_ptr
)
{
  qmi_psmi_cmd_buf_type *cmd_ptr = NULL;
/*-------------------------------------------------------------------------*/

  ASSERT(user_data_ptr);
  cmd_ptr = (qmi_psmi_cmd_buf_type *)user_data_ptr;

  switch(cmd_ptr->cmd_id)
  {

    case QMI_PSM_CMD_STATUS_IND:
    {
      psm_status_s_type status_ind;
      
      QM_MSG_HIGH_1("PSM_STATUS_IND, status:%d", cmd_ptr->data.psm_status.status);

      status_ind.status = cmd_ptr->data.psm_status.status;
      status_ind.reject_reason = cmd_ptr->data.psm_status.reject_reason;
      status_ind.periodic_update_timer = cmd_ptr->data.psm_status.periodic_update_timer;
      
      qmi_psm_generate_status_ind(status_ind);
      break;
    }
#ifdef FEATURE_REF_TRACKER
    case QMI_PSM_CMD_SNS_SET_CONFIG:
    {
    #ifndef FEATURE_MMODE_TX_21
      qmi_psm_set_sensor_configuration_resp(cmd_ptr->data.sensor_cm_cmd_cb_err); 
    #else
      qmi_psm_set_sensor_configuration_ext_resp(cmd_ptr->data.sensor_cm_cmd_cb_err); 
    #endif
      break;
    }
    case QMI_PSM_CMD_SNS_DATA_RESP:
    {
      if( cmd_ptr->data.sensor_cm_cmd_cb_err != QMI_ERR_NONE_V01 )
      {
        qmi_psm_get_sensor_data_resp_err(cmd_ptr->data.sensor_cm_cmd_cb_err);      
      }
      break;
    }
    case QMI_PSM_CMD_SNS_RESET_CONFIG:
    {
      qmi_psm_reset_sensor_config_data_resp(cmd_ptr->data.sensor_cm_cmd_cb_err);      
      break;
    }

	case QMI_PSM_CMD_SNS_OD_SNS_RESP:
    {            
      break;
    }
#ifdef FEATURE_MMODE_TX_21	
	case QMI_PSM_CMD_SNS_PHY_TEST:
	{
	  qmi_psm_phy_sns_test_resp(cmd_ptr->data.sensor_cm_cmd_cb_err);
	  break;
    }  
	case QMI_PSM_CMD_SNS_PHY_TEST_GET_DATA:
	{
      qmi_psm_phy_sns_test_get_data_resp(cmd_ptr->data.sensor_cm_cmd_cb_err);
	  break;
    }	
    case QMI_PSM_CMD_SNS_SET_LOGGING_LEVEL:
    {
      qmi_psm_set_sensor_logging_level_resp(cmd_ptr->data.sensor_cm_cmd_cb_err);
      break;
    }   
#endif
#endif
    default:
      QM_MSG_ERROR_1("Unexpected cmd(%d) recd in QMI PSM cmd handler",
                      cmd_ptr->cmd_id);
  }
  return;
} /* qmi_psmi_process_cmd() */

/*===========================================================================
  FUNCTION qmi_psm_init()

  DESCRIPTION
    Register the Power save mode Service with QCSI for all applicable QMI links
===========================================================================*/
void qmi_psm_init( void )
{
  qmi_csi_error                 err;
  qmi_sap_error                 serr;

  /* Set the cmd handlers in QMI MMODE task */  
  qmi_mmode_set_cmd_handler( QMI_MMODE_CMD_PSM_CMD_CB,    qmi_psmi_process_cmd);
  
  memset( &qmi_psm_state, 0x00, sizeof(qmi_mmode_svc_info_type) );
  memset( &qmi_psmi_cfg, 0x00, sizeof(qmi_psmi_cfg) );

  /*-----------------------------------------------------------------------
    Populate QMI PSM Config
  -----------------------------------------------------------------------*/

  qmi_psmi_cfg.cmd_hdlr_array                = qmi_psmi_cmd_callbacks;
  qmi_psmi_cfg.cmd_num_entries               = PSMI_CMD_MAX;

  /* Initialize QMI PSM state */
  qmi_psmi_global_init();
  qmi_psm_state.svc_config                   = (void *)&qmi_psmi_cfg;
  qmi_psm_state.service_type                 = QMI_MMODE_SERVICE_PSM;
  qmi_psm_state.uses_raw_msgs                = FALSE;
  
  err = qmi_mmode_svc_qcsi_register(QMI_MMODE_SERVICE_PSM,
                                  qmi_psm_qcsi_svc_connect_cb,
                                  qmi_psm_qcsi_svc_disconnect_cb,
                                  qmi_mmode_qcsi_svc_handle_req_cb,
                                  &qmi_psm_state);

  //register with SAP
  serr = qmi_sap_register(psm_get_service_object_v01(), 
                         NULL, 
                         &qmi_psm_state.sap_client_handle);

  if ( serr != QMI_SAP_NO_ERR || err != QMI_CSI_NO_ERR )
  {
    QM_MSG_ERROR_2("qmi_psm_init() qmi_csi_register/sap_register failed %d", err, serr);
    return;
  }
} /* qmi_psm_init() */


/*========================================================================
  FUNCTION qmi_psm_msgr_register

  DESCRIPTION
    register to msgr messages

  PARAMETERS
    msgr client object pointer

  RETURN VALUE
    None
===========================================================================*/
void qmi_psm_msgr_register( msgr_client_t *msgr_client )
{
  errno_enum_type err;

  err = msgr_register( MSGR_QMI_NAS, msgr_client, MSGR_ID_REX, MM_CM_PSM_GET_CFG_PARAMS_RSP );
  ASSERT( err == E_SUCCESS );
  err = msgr_register( MSGR_QMI_NAS, msgr_client, MSGR_ID_REX, MM_CM_PSM_STATUS_IND );
  ASSERT( err == E_SUCCESS );
  err = msgr_register( MSGR_QMI_NAS, msgr_client, MSGR_ID_REX, MM_CM_PSM_CFG_CHANGE_IND );
  ASSERT( err == E_SUCCESS );

#ifdef FEATURE_REF_TRACKER
  err = msgr_register( MSGR_QMI_NAS, msgr_client, MSGR_ID_REX, MM_CM_SNS_MGR_SNS_APP_EVENT_IND );
  ASSERT( err == E_SUCCESS );
  err = msgr_register( MSGR_QMI_NAS, msgr_client, MSGR_ID_REX, MM_CM_SNS_MGR_DATA_RSP );
  ASSERT( err == E_SUCCESS );

  err = msgr_register( MSGR_QMI_NAS, msgr_client, MSGR_ID_REX,MM_CM_SNS_MGR_SNS_TEST_PHY_IND);
  ASSERT( err == E_SUCCESS );
  
  err = msgr_register( MSGR_QMI_NAS, msgr_client, MSGR_ID_REX,MM_CM_SNS_MGR_SNS_TEST_PHY_GET_DATA_IND);
  ASSERT( err == E_SUCCESS );
  
  err = msgr_register( MSGR_QMI_NAS, msgr_client, MSGR_ID_REX,MM_CM_SNS_MGR_ATTRIBUTE_IND);
  ASSERT( err == E_SUCCESS );
    err = msgr_register( MSGR_QMI_NAS, msgr_client, MSGR_ID_REX, MM_CM_SNS_MGR_REGISTRY_IND );
  ASSERT( err == E_SUCCESS );
   err = msgr_register( MSGR_QMI_NAS, msgr_client, MSGR_ID_REX, MM_CM_SNS_MGR_READ_CONFIG_RSP );
  ASSERT( err == E_SUCCESS );
#endif
  
}

/*========================================================================
  FUNCTION qmi_psm_msgr_handler

  DESCRIPTION
    process messages received from msgr

  PARAMETERS
    msg pointer in *void. cast to qmi_mmodei_msgr_type before use.
    *void type is to avoid circular reference of headers

  RETURN VALUE
    None
===========================================================================*/
void qmi_psm_msgr_handler( void *param )
{

  qmi_mmodei_msgr_type       *msg;
  
  ASSERT( param );

  msg = (qmi_mmodei_msgr_type*)param;

  QM_MSG_HIGH_1("qmi_psm_msgr_handler rcvd %d", msg->cmd.hdr.id);
  switch ( msg->cmd.hdr.id )
  {
    
    case MM_CM_PSM_GET_CFG_PARAMS_RSP:
      QM_MSG_HIGH_1("PSM_GET_CFG_PARAMS_RSP, enabled:%d", msg->cmd.qmi_psm.psm_cfg_params_rsp.enabled);
      qmi_psm_get_cfg_params_resp( msg->cmd.qmi_psm.psm_cfg_params_rsp);
      break;
    
    case MM_CM_PSM_CFG_CHANGE_IND:
      QM_MSG_HIGH_1("PSM_CFG_CHANGE_IND, enabled:%d", msg->cmd.qmi_psm.psm_cfg_params_change_ind.psm_enabled);
      qmi_psm_generate_cfg_change_ind ( msg->cmd.qmi_nas.psm_cfg_params_change_ind);
      break;

    case MM_CM_PSM_STATUS_IND:
      {
        psm_status_s_type status_ind;
        
        QM_MSG_HIGH_1("PSM_STATUS_IND, status:%d", msg->cmd.qmi_psm.psm_status_ind.status);
  
        status_ind.status = msg->cmd.qmi_psm.psm_status_ind.status;
        status_ind.reject_reason = msg->cmd.qmi_psm.psm_status_ind.reject_reason;
        status_ind.periodic_update_timer = msg->cmd.qmi_psm.psm_status_ind.periodic_update_timer_value;
        
        qmi_psm_generate_status_ind ( status_ind );
      }
      break;

#ifdef FEATURE_REF_TRACKER
    case MM_CM_SNS_MGR_SNS_APP_EVENT_IND:
      QM_MSG_HIGH("MM_CM_SNS_MGR_SNS_APP_EVENT_IND");
      qmi_psm_sensor_policy_met_indication ( msg->cmd.qmi_psm.sensor_event_ind);
      break;

    case MM_CM_SNS_MGR_DATA_RSP:
      QM_MSG_HIGH_1("MM_CM_SNS_MGR_DATA_RSP, sns_len %d",
                     msg->cmd.qmi_psm.sensor_data_collection_rsp.sns_len);
      qmi_psm_get_sensor_data_resp( msg->cmd.qmi_psm.sensor_data_collection_rsp);
      break;
#ifdef FEATURE_MMODE_TX_21
    case MM_CM_SNS_MGR_SNS_TEST_PHY_IND:
		 QM_MSG_HIGH_1("MM_CM_SNS_MGR_SNS_TEST_PHY_IND, test_type %d",
                     msg->cmd.qmi_psm.sensor_phy_test_rsp.physical_sns_test);
    	qmi_psm_phy_sns_test_ind(msg->cmd.qmi_psm.sensor_phy_test_rsp);
	break;
	
	case MM_CM_SNS_MGR_SNS_TEST_PHY_GET_DATA_IND:
		 QM_MSG_HIGH_1("MM_CM_SNS_MGR_SNS_TEST_PHY_GET_DATA_IND, sensor_id %d",
                     msg->cmd.qmi_psm.sensor_phy_test_get_data_rsp.sensor_id);
		qmi_psm_phy_sns_test_get_data_ind(msg->cmd.qmi_psm.sensor_phy_test_get_data_rsp);
	break;
	case MM_CM_SNS_MGR_ATTRIBUTE_IND:
		 QM_MSG_HIGH("MM_CM_SNS_MGR_ATTRIBUTE_IND");
		 qmi_psm_get_sns_serial_num_resp(msg->cmd.qmi_psm.sensor_attribute_rsp);
		 break;
  case MM_CM_SNS_MGR_READ_CONFIG_RSP:
	 	qmi_psm_get_sensor_config_resp(msg->cmd.qmi_psm.sensor_registry_rsp);
     break;
      case MM_CM_SNS_MGR_REGISTRY_IND:
   	qmi_psm_set_sensor_configuration_ext_resp(QMI_ERR_NONE_V01);
  break;
#endif	
#endif
    default:
      QM_MSG_HIGH_1("not a QMI_PSM handled msg %d", msg->cmd.hdr.id);
      break;
  }
}

/*===========================================================================
FUNCTION      qmi_psm_cm_ready_req_cb

DESCRIPTION   This callback function is registered with CM to notify QMI of
              PSM READY REQUEST. It posts cmd to QMI PSM.

DEPENDENCIES  None

RETURN VALUE 
       True if command is posted successfully.
       False if command posting fails

SIDE EFFECTS  None
===========================================================================*/
boolean qmi_psm_cm_ready_req_cb( void )
{  
  qmi_psmi_cmd_buf_type             *cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_ptr = QMI_PSM_MEM_ALLOC(sizeof(qmi_psmi_cmd_buf_type));
  if ( cmd_ptr == NULL )
  {
    QM_MSG_ERROR("Out of memory");
    return FALSE;
  }

  QM_MSG_HIGH("PSM Ready Command Posted");
  
  cmd_ptr->cmd_id = QMI_PSM_CMD_STATUS_IND;
  cmd_ptr->data.psm_status.status = CM_PSM_STATUS_READY;
  cmd_ptr->data.psm_status.reject_reason = CM_PSM_REJECT_REASON_PSM_NO_ERROR;
  cmd_ptr->data.psm_status.periodic_update_timer  = 0xFFFF;
  
  qmi_mmode_send_cmd( QMI_MMODE_CMD_PSM_CMD_CB, cmd_ptr);

  return TRUE;

} 

/*===========================================================================
  FUNCTION qmi_psm_indication_register()

  DESCRIPTION
    Registers the requesting control point for various indications. 

  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message
    

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_indication_register
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void                            *req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_indication_register_req_msg_v01   *req_msg;
  psm_indication_register_resp_msg_v01   resp_msg;
  boolean                                got_one_tlv = FALSE;  
  int8                                   cm_psm_version =0;
  qmi_psmi_client_state_type            *clnt_info = 
                   (qmi_psmi_client_state_type *)cl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct !=NULL);
  req_msg = (psm_indication_register_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(resp_msg));

  // Process the request
  if (req_msg->report_psm_status_valid)
  {
    got_one_tlv = TRUE;
    clnt_info->psm_info.report_psm_status = req_msg->report_psm_status ? TRUE : FALSE;

    //Register with the CM for the PSM callbacks
     cm_psm_version = cm_get_psm_sw_ver();
     if(cm_psm_version == PSM_VERSION_2_V01) 
     {
       if(cm_client_psm_ready_reg(qmi_psm_cm_ready_req_cb,
                                   CM_PSM_MODULE_QMI ) != TRUE)
       {
         QM_MSG_HIGH("Unable to register CM PSM Ready notification cb");
       }
    }
  }
  if (req_msg->report_psm_cfg_change_valid)
  {
    got_one_tlv = TRUE;
    clnt_info->psm_info.report_psm_cfg_change = req_msg->report_psm_cfg_change ? TRUE : FALSE;

  }
  if(req_msg->report_sns_phy_test_ind_valid)
  {
    got_one_tlv = TRUE;
    clnt_info->psm_info.report_sns_phy_test_ind = req_msg->report_sns_phy_test_ind ? TRUE: FALSE;
  }
   if(req_msg->report_sns_phy_test_get_data_ind_valid)
  {
    got_one_tlv = TRUE;
    clnt_info->psm_info.report_sns_phy_test_get_data_ind = req_msg->report_sns_phy_test_get_data_ind ? TRUE: FALSE;
  }
  if(req_msg->report_sensor_status_valid)
  {
    got_one_tlv = TRUE;
    clnt_info->psm_info.report_sensor_policy_met_ind = req_msg->report_sensor_status ? TRUE : FALSE;
  }

  // Check if at least one TLV is present
  if (got_one_tlv)
  {
    resp_msg.resp.error = QMI_ERR_NONE_V01;
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  }
  else
  {
    resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  }

  // Send response
  qmi_psmi_send_response(cmd_buf_p, &resp_msg, sizeof(resp_msg));
  return QMI_CSI_CB_NO_ERR;

} /* qmi_psm_indication_register() */

/*===========================================================================
  FUNCTION qmi_psm_get_cfg_params()

  DESCRIPTION
    Query PSM (Power Save Mode) configuration parameters.

  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request messager

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_get_cfg_params
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_get_cfg_params_resp_msg_v01       resp_msg;
  cm_psm_get_cfg_params_req_s_type      cm_psm_get_cfg_params_req;
  qmi_psm_pending_cmd_buf_addr_s_type  *pend_req_addr = NULL;
  errno_enum_type                       msgr_err;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&resp_msg, 0, sizeof(resp_msg));
  memset(&cm_psm_get_cfg_params_req, 0, sizeof(cm_psm_get_cfg_params_req));

  resp_msg.resp.error = QMI_ERR_NONE_V01;

  /* store the request to into pending requests queue */
  pend_req_addr = QMI_PSM_MEM_ALLOC( sizeof(qmi_psm_pending_cmd_buf_addr_s_type) );
  if(!pend_req_addr)
  {
    resp_msg.resp.error = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
  
  memset(pend_req_addr, 0, sizeof(qmi_psm_pending_cmd_buf_addr_s_type));
  (void) q_link ( pend_req_addr, &pend_req_addr->link );
  pend_req_addr->cmd_buf_addr = (void *)cmd_buf_p;

  // Send command to CM only if no pending request
  if (q_check(&qmi_psmi_global.get_cfg_params_req_buf_p) == NULL)
  {
    cm_psm_get_cfg_params_req.user_config = FALSE;
    msgr_err = qmi_mmode_msgr_send(
                                MM_CM_PSM_GET_CFG_PARAMS_REQ,
                                MSGR_QMI_NAS,
                                (msgr_hdr_struct_type *)&cm_psm_get_cfg_params_req,
                                sizeof(cm_psm_get_cfg_params_req));
    if ( msgr_err != E_SUCCESS )
    {
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
    else
    {
      QM_MSG_HIGH_1("PSM get_cfg_params cmd [%d] sent", MM_CM_PSM_GET_CFG_PARAMS_REQ);
    }
  }

  // Queue and wait for CM response
  q_put( &(qmi_psmi_global.get_cfg_params_req_buf_p), &(pend_req_addr->link) );
  QM_MSG_HIGH("PSM get_cfg_params queued");

send_result:
  if (resp_msg.resp.error != QMI_ERR_NONE_V01)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    qmi_psmi_send_response(cmd_buf_p, &resp_msg, sizeof(resp_msg));
    if (pend_req_addr != NULL)
    {
      QMI_PSM_MEM_FREE(pend_req_addr);
    }
  }

  return QMI_CSI_CB_NO_ERR;

} /* qmii_psm_get_cfg_params() */


/*===========================================================================
  FUNCTION qmi_psmi_validate_cmd_buf_p()

  DESCRIPTION
    This Function validates the async command buffer passed.
    It chcks whether cl_sp in async command buffer is valid by comparing it with
    clients queue in psm state.
    
  PARAMETERS
    async_cmd_buf : pointer to the async command buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_psmi_validate_cmd_buf_p
(
  qmi_mmode_qcsi_transaction_type *pending_req
)
{  
  qmi_psmi_client_state_type * cl_sp =
                 (qmi_psmi_client_state_type *)q_check(&qmi_psm_state.client_q);
  
  if( pending_req != NULL && pending_req->req_handle != NULL )
  {
    QM_MSG_HIGH_1("cl_sp = %0x", pending_req->clnt_info_ptr);
    while(cl_sp != NULL)
    {
      if(cl_sp == pending_req->clnt_info_ptr)
      {
        return TRUE;
      }
      cl_sp = q_next(&qmi_psm_state.client_q, &(cl_sp->link));
    }
  }
  
  QM_MSG_HIGH("Invalid pending_req:client might be disconnected");
  return FALSE;
}

/*===========================================================================
  FUNCTION qmi_psm_get_cfg_params_resp()

  DESCRIPTION
    Generate PSM get cfg params response.

  PARAMETERS
    cmd_buf_ptr : PSM status cmd buf ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_psm_get_cfg_params_resp
(
  cm_psm_get_cfg_params_rsp_s_type psm_cfg_params_rsp
)
{
  psm_get_cfg_params_resp_msg_v01       resp_msg;
  qmi_psm_pending_cmd_buf_addr_s_type  *pend_req_addr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&resp_msg, 0, sizeof(resp_msg));

  // Construct response
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;

  resp_msg.psm_enabled_valid = TRUE;
  resp_msg.psm_enabled = psm_cfg_params_rsp.enabled ? 1 : 0;

  if (resp_msg.psm_enabled)
  {
    resp_msg.duration_threshold_valid = TRUE;
    resp_msg.duration_threshold = psm_cfg_params_rsp.threshold;

    resp_msg.duration_due_to_oos_valid = TRUE;
    resp_msg.duration_due_to_oos = psm_cfg_params_rsp.psm_duration_due_to_oos;

    resp_msg.randomization_window_valid = TRUE;
    resp_msg.randomization_window = psm_cfg_params_rsp.psm_randomization_window;

    resp_msg.active_timer_valid = TRUE;
    resp_msg.active_timer = psm_cfg_params_rsp.active_timer_value;
    /*FIXME: 0xFFFFFF --> means it is not valid/default value. Need to send??*/
    resp_msg.periodic_update_timer_valid = TRUE;
    resp_msg.periodic_update_timer = psm_cfg_params_rsp.periodic_tau_timer_value;

    resp_msg.early_wakeup_time_valid = TRUE;
    resp_msg.early_wakeup_time = psm_cfg_params_rsp.early_wakeup_time;

  }

  // Send to all pending requests
  while ((pend_req_addr = (qmi_psm_pending_cmd_buf_addr_s_type *)
                          q_get(&qmi_psmi_global.get_cfg_params_req_buf_p)) != NULL)
  {
    if (qmi_psmi_validate_cmd_buf_p((qmi_mmode_qcsi_transaction_type *)pend_req_addr->cmd_buf_addr))
    {
      qmi_psmi_send_response((qmi_mmode_qcsi_transaction_type *)pend_req_addr->cmd_buf_addr,
                             &resp_msg,
                             sizeof(resp_msg));
    }
    QMI_PSM_MEM_FREE(pend_req_addr);
  }

} /* qmi_psm_get_cfg_params_resp() */



/*===========================================================================
FUNCTION      qmi_psm_cm_ready_response

DESCRIPTION   This function notifies CM that QMI PSM is ready to enter PSM Mode
              and provide the activate timer    
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void qmi_psm_cm_ready_response
(
  boolean        status,
  uint32         activate_timer
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cm_generic_rpt_u_type    psm_rpt;

  memset(&psm_rpt, 0, sizeof(cm_generic_rpt_u_type));

  QM_MSG_HIGH("PSM Ready response indication to cm");
  
  psm_rpt.psm_ready_rsp.status = SYS_PSM_STATUS_NOT_READY_NOT_IDLE;

  if(status == TRUE)
  {
     psm_rpt.psm_ready_rsp.status = SYS_PSM_STATUS_READY; 
  }

  psm_rpt.psm_ready_rsp.activate_timer =  activate_timer;
  psm_rpt.psm_ready_rsp.module = CM_PSM_MODULE_QMI;

  cm_client_send_psm_rpt(CM_CLIENT_PSM_READY_RSP, &psm_rpt);

  return;
} 

/*===========================================================================
FUNCTION      qmi_psm_cm_ready_ind

DESCRIPTION   This function notifies CM that QMI PSM is ready to enter PSM Mode
              and provide the activate timer in the indication
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void qmi_psm_cm_ready_ind
(
  uint32          activate_timer
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cm_generic_rpt_u_type  psm_rpt;

  memset(&psm_rpt, 0, sizeof(cm_generic_rpt_u_type));

  QM_MSG_HIGH("PSM Ready indication to cm");
  
  psm_rpt.psm_ready_ind.activate_timer =  activate_timer;
  psm_rpt.psm_ready_ind.module = CM_PSM_MODULE_QMI;

  cm_client_send_psm_rpt(CM_CLIENT_PSM_READY_IND,&psm_rpt);

  return;

}

/*===========================================================================
  FUNCTION qmi_psm_enter()

  DESCRIPTION
    Request to enter PSM (Power Save Mode). 

  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_enter
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_enter_req_msg_v01          *req_msg;
  psm_enter_resp_msg_v01          resp_msg;
  cm_psm_enter_req_s_type         cm_psm_enter_req;
  qmi_psmi_client_state_type     *clnt_info = (qmi_psmi_client_state_type *)cl_sp;
  errno_enum_type                 msgr_err;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct !=NULL);
  req_msg = (psm_enter_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(resp_msg));

  memset(&cm_psm_enter_req, 0, sizeof(cm_psm_enter_req));

  // Get request parameters
  if (req_msg->duration_in_psm_valid)
  {
    cm_psm_enter_req.apps_time = req_msg->duration_in_psm;
  } 
  QM_MSG_HIGH_1("qmi_psm_enter  %d ",clnt_info->current_psm_state);

  if(qmi_psmi_global.psm_capability == PSM_VERSION_2_V01)
  {
    if(req_msg->status_valid)
    {     
      if(clnt_info->current_psm_state != QMI_PSM_DISABLE_STATE)
      {
         if(req_msg->status == TRUE) //psm accepted
         {
          //Check what was the old state of the client
           clnt_info->current_psm_state = QMI_PSM_ENABLE_STATE;
           qmi_psm_cm_ready_response(TRUE,req_msg->duration_in_psm);
         }
         else if(req_msg->status == FALSE)
         {
           clnt_info->current_psm_state = QMI_PSM_DISABLE_STATE;        
           qmi_psm_cm_ready_response(FALSE,req_msg->duration_in_psm);
         }
      }
      else if(clnt_info->current_psm_state == QMI_PSM_DISABLE_STATE)
      {
        if(req_msg->status == TRUE) //psm accepted
        {
          clnt_info->current_psm_state = QMI_PSM_ENABLE_STATE;
          qmi_psm_cm_ready_ind(req_msg->duration_in_psm);
        }
        else if(req_msg->status == FALSE)
        {
          qmi_psm_cm_ready_response(FALSE,req_msg->duration_in_psm);
        }
      }
    } 
    else
    {
       clnt_info->current_psm_state = QMI_PSM_ENABLE_STATE;
       qmi_psm_cm_ready_response(TRUE,req_msg->duration_in_psm);
    }
    goto send_result;
  }
  // Send command to CM
  msgr_err = qmi_mmode_msgr_send(
                                MM_CM_PSM_ENTER_REQ,
                                MSGR_QMI_NAS,
                                (msgr_hdr_struct_type *)&cm_psm_enter_req,
                                sizeof(cm_psm_enter_req));

  if ( msgr_err != E_SUCCESS )
  {
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  }
  else
  {
    resp_msg.resp.error = QMI_ERR_NONE_V01;
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    QM_MSG_HIGH_1("PSM enter cmd [%d] sent", MM_CM_PSM_ENTER_REQ);
  }
  
  send_result:
  // Send response
  qmi_psmi_send_response(cmd_buf_p, &resp_msg, sizeof(resp_msg));
  return QMI_CSI_CB_NO_ERR;

} /* qmi_psm_psm_enter() */

/*===========================================================================
  FUNCTION qmi_psm_generate_status_ind()

  DESCRIPTION
    Returns status of PSM MODE

  PARAMETERS
    psm_status

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_psm_generate_status_ind 
(
  psm_status_s_type   psm_status_ind
)
{
  psm_status_ind_msg_v01 ind_msg;
  qmi_psmi_client_state_type *cl_sp;
  qmi_csi_error                rc;
  
  memset (&ind_msg, 0x00, sizeof(psm_status_ind_msg_v01));

  ind_msg.psm_status       = (psm_status_enum_v01)psm_status_ind.status;

  ind_msg.reject_reason_valid = TRUE;
  ind_msg.reject_reason       = (psm_reject_reason_enum_v01)psm_status_ind.reject_reason;

  ind_msg.periodic_update_timer_valid = TRUE;
  ind_msg.periodic_update_timer      = psm_status_ind.periodic_update_timer;

  // Send to all registered clients
  cl_sp = (qmi_psmi_client_state_type *)q_check(&qmi_psm_state.client_q);
  while (cl_sp != NULL)
  {
    if (cl_sp->psm_info.report_psm_status)
    {
      rc = qmi_mmode_qcsi_send_ind(cl_sp->clnt,
                            QMI_PSM_STATUS_IND_V01,
                            &ind_msg,
                            sizeof(ind_msg));
      if (rc != QMI_CSI_NO_ERR)
      {
        QM_MSG_HIGH_1("Failed to report PSM status, error %d", rc);
      }
    }
    cl_sp = q_next(&qmi_psm_state.client_q, &(cl_sp->link));
  }

} 

/*===========================================================================
  FUNCTION qmi_psm_set_cfg_params()

  DESCRIPTION
    Set PSM (Power Save Mode) configuration parameters.
  PARAMETERS
     sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_set_cfg_params
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
   psm_set_cfg_params_resp_msg_v01     resp_msg;
   psm_set_cfg_params_req_msg_v01     *req_msg = NULL;
   cm_mm_psm_set_cfg_params_req_s_type cm_psm_set_cfg_params_req;
   uint32                              input_timer_value = 0;
   errno_enum_type                     msgr_err;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   memset(&resp_msg, 0, sizeof(resp_msg));
   memset(&cm_psm_set_cfg_params_req, 0, sizeof(cm_psm_set_cfg_params_req));
   req_msg = (psm_set_cfg_params_req_msg_v01*)req_c_struct;
 
   resp_msg.resp.error = QMI_ERR_NONE_V01;
   if ( req_msg->psm_enabled_valid == TRUE )
   {
     cm_psm_set_cfg_params_req.change_mask |= CM_PSM_CONFIG_INFO_MASK_PSM_ENABLED;
     cm_psm_set_cfg_params_req.psm_enabled = req_msg->psm_enabled? TRUE : FALSE;
   }

   if ( req_msg->active_timer_valid == TRUE )
   {
     input_timer_value = (uint32)req_msg->active_timer;
      /*Validate of the timer as per Spec*/
     if(((input_timer_value <= 62) && (input_timer_value % 2 == 0)) ||		  
        ((input_timer_value >= 120 && input_timer_value <= 1860) && (input_timer_value % 60 == 0)) ||	   
        ((input_timer_value >= 2160 && input_timer_value <= 11160) && (input_timer_value % 360 == 0)))		   
     {
       cm_psm_set_cfg_params_req.change_mask |= CM_PSM_CONFIG_INFO_MASK_ACTIVE_TIMER;
       cm_psm_set_cfg_params_req.active_timer_value = req_msg->active_timer;
     }
     else
     {
       resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
       goto send_result;
     }
   }

   if ( req_msg->periodic_update_timer_valid == TRUE )
   {
      input_timer_value = (uint32)req_msg->periodic_update_timer;
      /*Validate of the timer as per Spec*/
      if(((input_timer_value <= 62) && (input_timer_value % 2 == 0)) ||
       ((input_timer_value >= 90 && input_timer_value <= 930) && (input_timer_value % 30 == 0))||		  
       ((input_timer_value >= 960 && input_timer_value <= 1860) && (input_timer_value % 60 == 0))||		   
       ((input_timer_value >= 2400 && input_timer_value <= 18600) && (input_timer_value % 600 == 0))||
       ((input_timer_value >= 21600 && input_timer_value <= 111600) && (input_timer_value % 3600 == 0))||
       ((input_timer_value >= 144000 && input_timer_value <= 1116000) && (input_timer_value % 36000 == 0))||
       ((input_timer_value >= 1152000 && input_timer_value <= 35712000) && (input_timer_value % 1152000 == 0)))	   
      {
        cm_psm_set_cfg_params_req.change_mask |= CM_PSM_CONFIG_INFO_MASK_PERIODIC_UPDATE_TIMER;
        cm_psm_set_cfg_params_req.periodic_tau_timer_value = req_msg->periodic_update_timer;
      }
      else
      {
        resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
        goto send_result;
      }
   }
 
   msgr_err = qmi_mmode_msgr_send(
                                  MM_CM_PSM_SET_CFG_PARAMS_REQ,
                                  MSGR_QMI_NAS,
                                  (msgr_hdr_struct_type *)&cm_psm_set_cfg_params_req,
                                  sizeof(cm_psm_set_cfg_params_req));

   if ( msgr_err != E_SUCCESS )
   {
     resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
     goto send_result;
   }
   QM_MSG_HIGH_1("PSM set_cfg_params cmd [%d] sent to CM",
                    MM_CM_PSM_SET_CFG_PARAMS_REQ);
 send_result:
  resp_msg.resp.result = (resp_msg.resp.error == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                     : QMI_RESULT_FAILURE_V01);
  if(resp_msg.resp.error == QMI_ERR_INVALID_ARG_V01)
  {
    QM_MSG_HIGH_2("qmi_psm_set_cfg_params INVALID TIMER: active_timer %d or "
                     "periodic_update_timer: %d",
                     ((req_msg->active_timer_valid == TRUE)?req_msg->active_timer:0),
                     ((req_msg->periodic_update_timer_valid == TRUE)?req_msg->periodic_update_timer:0));
  }
  qmi_psmi_send_response(cmd_buf_p, &resp_msg, sizeof(resp_msg));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_psm_generate_cfg_change_ind()

  DESCRIPTION
    Generate PSM cfg changes indication.

  PARAMETERS
    cmd_buf_ptr : PSM cfg changes cmd buf ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_psm_generate_cfg_change_ind
(
  cm_mm_psm_cfg_change_ind_s_type psm_cfg_params_change_ind
)
{
  psm_cfg_params_change_ind_msg_v01   ind_msg;
  qmi_psmi_client_state_type *cl_sp;
  qmi_csi_error                rc;
  uint16                       change_mask;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&ind_msg, 0, sizeof(ind_msg));

  change_mask = psm_cfg_params_change_ind.change_mask;
  QM_MSG_HIGH_1("qmi_psm_generate_cfg_change_ind: changemask 0x%x",
                     change_mask);

  //ind_msg->active_rat_valid = TRUE;
  //ind_msg->active_rat       = (nas_radio_if_enum_v01)qmi_nas_sys_radio_access_2_radio_if(psm_cfg_params_change_ind.active_rat);

  if(change_mask & CM_PSM_CONFIG_INFO_MASK_PSM_ENABLED)
  {
    ind_msg.psm_enabled_valid = TRUE;
    ind_msg.psm_enabled = psm_cfg_params_change_ind.psm_enabled;
  }
  if(change_mask & CM_PSM_CONFIG_INFO_MASK_ACTIVE_TIMER)
  {
    ind_msg.active_timer_valid = TRUE;
    ind_msg.active_timer = psm_cfg_params_change_ind.active_timer_value;    
  }
  if(change_mask & CM_PSM_CONFIG_INFO_MASK_PERIODIC_UPDATE_TIMER)
  {
    ind_msg.periodic_update_timer_valid = TRUE;
    ind_msg.periodic_update_timer = psm_cfg_params_change_ind.periodic_update_timer_value;
  }
  /*if(psm_cfg_params_change_ind.change_mask & CM_PSM_CONFIG_INFO_MASK_GPRS_READY_TIMER)
  {
   ind_msg->gprs_ready_timer_value_valid = TRUE;
   ind_msg->gprs_ready_timer_value       = psm_cfg_params_change_ind.gprs_ready_timer_value;
  }*/
  
  // Send to all registered clients
  cl_sp = (qmi_psmi_client_state_type *)q_check(&qmi_psm_state.client_q);
  while (cl_sp != NULL)
  {
    if (cl_sp->psm_info.report_psm_cfg_change)
    {
      rc = qmi_psmi_send_ind(cl_sp,
                            QMI_PSM_CFG_PARAMS_CHANGE_IND_V01,
                            &ind_msg,
                            sizeof(ind_msg));
      if (rc != QMI_CSI_NO_ERR)
      {
        QM_MSG_HIGH_1("Failed to report PSM status, error %d", rc);
      }
    }
    cl_sp = q_next(&qmi_psm_state.client_q, &(cl_sp->link));
  }
}

/*===========================================================================
  FUNCTION qmi_psm_get_capability()

  DESCRIPTION
    Set PSM (Power Save Mode) configuration parameters.
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error    qmi_psm_get_capability
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_get_capability_req_msg_v01       *req_msg = (psm_get_capability_req_msg_v01 *)req_c_struct;
  psm_get_capability_resp_msg_v01       resp_msg;
  int8                                  cm_psm_version =0;
  /*-------------------------------------------------------*/
  
  memset(&resp_msg ,0,sizeof(psm_get_capability_resp_msg_v01));
  resp_msg.psm_capability_valid = TRUE;
  resp_msg.psm_capability = PSM_VERSION_UNSPEC_V01;
  cm_psm_version = cm_get_psm_sw_ver();

  QM_MSG_HIGH_1("qmi_psm_get_capability cm_psm_version %d",cm_psm_version);

  if(req_msg->te_psm_capability_valid)
  {
    QM_MSG_HIGH_1("qmi_psm_get_capability te_psm_capability %d",req_msg->te_psm_capability);
    
    if(req_msg->te_psm_capability == PSM_VERSION_UNSPEC_V01)
    {
      goto send_result;
    }
    else if(req_msg->te_psm_capability == PSM_VERSION_1_V01)
    {
      //Call CM api to check which version the CM supports for PSM 
      //return the lower version of the two.
      if(cm_psm_version == PSM_VERSION_2_V01)
      {
        resp_msg.psm_capability = PSM_VERSION_1_V01;
      }
    }
    else if(req_msg->te_psm_capability == PSM_NOT_SUPPORTED_V01)
    {
      resp_msg.psm_capability = PSM_NOT_SUPPORTED_V01;
    }
    else if(req_msg->te_psm_capability == PSM_VERSION_2_V01)
    {
     //Call CM api to check which version the CM supports for PSM
     //return the lower capability of two
      if(cm_psm_version == PSM_VERSION_2_V01)
      {
        resp_msg.psm_capability = PSM_VERSION_2_V01;
      }
    }
  }
  qmi_psmi_global.psm_capability = resp_msg.psm_capability;
  
send_result:
  qmi_psmi_send_response(cmd_buf_p, &resp_msg, sizeof(resp_msg));
  return QMI_CSI_CB_NO_ERR;
}

#ifdef FEATURE_REF_TRACKER
static qmi_error_type_v01  qmi_psmi_convert_cm_cmd_err
(
  cm_sns_cmd_err_e_type              cm_sns_err
)
{
  QM_MSG_HIGH_1("qmi_psmi_convert_cm_cmd_err cm_cmd_err %d",cm_sns_err);
  switch(cm_sns_err)
  {  
    case CM_SNS_CMD_ERR_NOERR:
        return QMI_ERR_NONE_V01;
    case CM_SNS_CMD_ERR_NO_MEMORY:
        return QMI_ERR_NO_MEMORY_V01;
    case CM_SNS_CMD_ERR_DATA_REQ_NOT_ALLOW:
        return QMI_ERR_INVALID_OPERATION_V01;
    case CM_SNS_CMD_ERR_CONFIG_INVALID:
        return QMI_ERR_INVALID_ARG_V01;
    case CM_SNS_CMD_ERR_ARRAY_PTR_NULL:
        return QMI_ERR_INTERNAL_V01;
    case CM_SNS_CMD_ERR_OTHER:
    case CM_SNS_CMD_ERR_MAX:        
        return QMI_ERR_INTERNAL_V01;
  }
  return QMI_ERR_NONE_V01;
}

/*===========================================================================
  FUNCTION qmi_psm_sensor_cm_cmd_callback()

  DESCRIPTION
    Sends the indication about the policy met
    
  PARAMETERS
  cmd_buf_ptr :  Sensor cfg    cmd buf ptr

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static   void qmi_psm_sensor_cm_cmd_callback
(
  cm_sns_cmd_e_type                  cm_sns_cmd,
  cm_sns_cmd_err_e_type              cm_sns_err
)
{
  qmi_psmi_cmd_buf_type             *cmd_ptr = NULL;
  qmi_error_type_v01                 errval = QMI_ERR_NONE_V01;
  /*---------------------------------------------------------------*/
  
  errval =  qmi_psmi_convert_cm_cmd_err(cm_sns_err);
  cmd_ptr = QMI_PSM_MEM_ALLOC(sizeof(qmi_psmi_cmd_buf_type));
  
  if ( cmd_ptr == NULL )
  {
    QM_MSG_ERROR("Out of memory");
    return;
  }

  switch(cm_sns_cmd)
  {
    case CM_SNS_CMD_SET_CONFIG:
      cmd_ptr->cmd_id = QMI_PSM_CMD_SNS_SET_CONFIG;
      break;
    case CM_SNS_CMD_DATA_REQ:
      cmd_ptr->cmd_id = QMI_PSM_CMD_SNS_DATA_RESP;
      break;
    case CM_SNS_CMD_RESET_CONFIG:
      cmd_ptr->cmd_id = QMI_PSM_CMD_SNS_RESET_CONFIG;
      break;
	 case CM_SNS_CMD_OD_SNS_REQ:
      cmd_ptr->cmd_id = QMI_PSM_CMD_SNS_OD_SNS_RESP;
      break;
	case CM_SNS_PHY_TEST:
      cmd_ptr->cmd_id = QMI_PSM_CMD_SNS_PHY_TEST;
	  break;
	case CM_SNS_PHY_TEST_GET_DATA:
	  cmd_ptr->cmd_id = QMI_PSM_CMD_SNS_PHY_TEST_GET_DATA;
	  break;
    case CM_SNS_GET_SERIAL_NUM:
	  cmd_ptr->cmd_id = QMI_PSM_CMD_SNS_GET_SERIAL_NUM;
	  break;		
    #ifdef FEATURE_MMODE_TX_21
    case CM_SNS_SET_LOGGING_LEVEL:
	  cmd_ptr->cmd_id = QMI_PSM_CMD_SNS_SET_LOGGING_LEVEL;
	  break;  
    case CM_SNS_CMD_GET_CONFIG:
       cmd_ptr->cmd_id = QMI_PSM_CMD_SNS_GET_CONFIG_RESP;
      break;
	 #endif 
    default:
      QM_MSG_ERROR("Invalid cm_sns_cmd");
    break;
  }

  QM_MSG_HIGH("sensor_cm_cmd_callback Command Posted");
  
  cmd_ptr->data.sensor_cm_cmd_cb_err = errval;
  qmi_mmode_send_cmd( QMI_MMODE_CMD_PSM_CMD_CB, cmd_ptr);
}

/*===========================================================================
  FUNCTION qmi_psm_set_sensor_configuration()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_set_sensor_configuration
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_set_sensor_configuration_req_msg_v01   *req_msg;
  psm_set_sensor_configuration_resp_msg_v01   resp_msg;
  qmi_error_type_v01                          errval = QMI_ERR_NONE_V01;
  cm_sns_config_data_s_type                   sensor_config[CM_SNS_TYPE_ENV_SENSOR_MAX ];
  uint8                                       policy_len = 0;
  uint8                                       config_list =0; 
  /*--------------------------------------------------*/
  ASSERT(req_c_struct && (req_c_struct_len > 0));

  memset(&resp_msg,0,sizeof(psm_set_sensor_configuration_resp_msg_v01));
  memset(&sensor_config,0,(sizeof(cm_sns_config_data_s_type)*CM_SNS_TYPE_ENV_SENSOR_MAX));
  req_msg = (psm_set_sensor_configuration_req_msg_v01 *)req_c_struct;
  
  if( qmi_psmi_global.sns_set_config_cmd_buf_p != NULL )
  {  
    QM_MSG_HIGH("Set Configuration command is already in progress");
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  if(req_msg->sensor_id != PSM_SENSOR_TYPE_ALL_V01)
  {
     errval = QMI_ERR_INVALID_ARG_V01;
     goto send_result;
  }
  if(req_msg->sensor_config_list_valid)
  {
    policy_len = (uint8)req_msg->sensor_config_list_len;

    if(req_msg->sensor_config_list_len > CM_SNS_TYPE_ENV_SENSOR_MAX)
    {
    QM_MSG_HIGH_1("Invalid command as the total sensor is = %d and Max allowed sensor is CM_SNS_TYPE_ENV_SENSOR_MAX",req_msg->sensor_config_list_len);
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
    }

    QM_MSG_HIGH_2("policy lenth = %d and req_msg_length =%d",policy_len,req_msg->sensor_config_list_len);
  
    for(config_list =0 ;config_list < (uint8)req_msg->sensor_config_list_len; config_list++)
    {
       if((req_msg->sensor_config_list[config_list].sensor_id >= PSM_SENSOR_TYPE_ALS_V01) && (req_msg->sensor_config_list[config_list].sensor_id <= PSM_SENSOR_TYPE_TEMPERATURE_V01) )
       {
       sensor_config[config_list].sns_id = 
                              req_msg->sensor_config_list[config_list].sensor_id;
       sensor_config[config_list].sns_max_batch_cnt =
                              req_msg->sensor_config_list[config_list].sensor_batch_cnt;
       sensor_config[config_list].sns_interval =
                             req_msg->sensor_config_list[config_list].reporting_interval;
       if(req_msg->sensor_config_list[config_list].sensor_policy_len >CM_MAX_SNS_DIMENSION)
       {
          QM_MSG_HIGH("Max policy allwed at lower layers is 2");
          errval = QMI_ERR_INVALID_ARG_V01;
          goto send_result;
       }
       sensor_config[config_list].sns_policy.sns_policy_elem[0].greater_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[0].greater_value;
       sensor_config[config_list].sns_policy.sns_policy_elem[0].lower_value =
                        req_msg->sensor_config_list[config_list].sensor_policy[0].lower_value;
       sensor_config[config_list].sns_policy.sns_policy_elem[0].delta_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[0].delta_value;
       if(req_msg->sensor_config_list[config_list].sensor_policy_len > 1)
       {
         sensor_config[config_list].sns_policy.sns_policy_elem[1].greater_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[1].greater_value;
         sensor_config[config_list].sns_policy.sns_policy_elem[1].lower_value =
                        req_msg->sensor_config_list[config_list].sensor_policy[1].lower_value;
         sensor_config[config_list].sns_policy.sns_policy_elem[1].delta_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[1].delta_value;
       }
      }
    }
  }
  
  if(TRUE !=  cm_sns_cmd_set_config(qmi_psm_sensor_cm_cmd_callback,
                                       qmi_psm_get_cm_client_id(),
                                       sensor_config,
                                        policy_len
                                       ))
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }

  qmi_psmi_global.sns_set_config_cmd_buf_p = cmd_buf_p;
  QM_MSG_HIGH("PSM set_sensor_configuration queued");
 
send_result:
  if (errval != QMI_ERR_NONE_V01)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error  = errval; 
    QM_MSG_HIGH_2("qmi_psm_set_sensor_configuration result %d errval %d", resp_msg.resp.result, errval);
            
    qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                      sizeof(psm_set_sensor_configuration_resp_msg_v01));
  }
          
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_psm_set_sensor_configuration_resp()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
     errval received from CM cmd callback

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_psm_set_sensor_configuration_resp
(
  qmi_error_type_v01                 errval
)
{
  psm_set_sensor_configuration_resp_msg_v01   resp_msg;
  qmi_mmode_qcsi_transaction_type            *cmd_buf_p = qmi_psmi_global.sns_set_config_cmd_buf_p;

  qmi_psmi_global.sns_set_config_cmd_buf_p = NULL;
  if(cmd_buf_p == NULL || qmi_psmi_validate_cmd_buf_p(cmd_buf_p) == FALSE)
  {
    QM_MSG_HIGH("No pending request/client got deallocated");
  }

  memset(&resp_msg,0,sizeof(psm_set_sensor_configuration_resp_msg_v01));
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  QM_MSG_HIGH_2("qmi_psm_set_sensor_configuration result %d errval %d",
          resp_msg.resp.result, errval);
  
  qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                    sizeof(psm_set_sensor_configuration_resp_msg_v01));
}
#ifdef FEATURE_MMODE_TX_21

/*===========================================================================
  FUNCTION qmi_psm_set_sensor_logging_level()

  DESCRIPTION
    Requests modem to enable sensor logging
    
  PARAMETERS
    enable_logging      :       Enable logging
    log_level           :       Logging level

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error qmi_psm_set_sensor_logging_level
(
qmi_mmode_svc_info_type         *sp,
qmi_mmode_qcsi_transaction_type *cmd_buf_p,
void                            *cl_sp,
void*                            req_c_struct,
unsigned int                     req_c_struct_len
)
{
  psm_sensor_logging_req_msg_v01   *req_msg;
  psm_sensor_logging_resp_msg_v01   resp_msg;
  qmi_error_type_v01                errval = QMI_ERR_NONE_V01;

  /*--------------------------------------------------*/
  ASSERT(req_c_struct && (req_c_struct_len > 0));

  memset(&resp_msg,0,sizeof(psm_sensor_logging_resp_msg_v01));
  req_msg = (psm_sensor_logging_req_msg_v01 *)req_c_struct;

  if(!(req_msg->log_enable_level>=0 && req_msg->log_enable_level<=2))
  {
    QM_MSG_HIGH("Logging level is invalid");
    errval = QMI_ERR_INVALID_ARG_V01;
  }
  
  else
  {
    if(TRUE !=  cm_sns_set_sensor_logging_level(qmi_psm_sensor_cm_cmd_callback,
                                       qmi_psm_get_cm_client_id(),
                                       req_msg->log_enable_level
                                       ))
    {
      errval = QMI_ERR_INTERNAL_V01;
    }

    else
    {
      QM_MSG_HIGH("PSM enable_sensor_logging queued");
    }
  }
  qmi_psmi_global.sns_set_logging_level_cmd_buf_p = cmd_buf_p;

  if (errval != QMI_ERR_NONE_V01)
  {
    resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                          : QMI_RESULT_FAILURE_V01);
    resp_msg.resp.error  = errval;
    
    QM_MSG_HIGH_2("qmi_psm_set_sensor_logging_level result %d errval %d", resp_msg.resp.result, errval);
          
    qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                    sizeof(psm_sensor_logging_resp_msg_v01));
  }   
  return QMI_CSI_CB_NO_ERR;
};

/*===========================================================================
  FUNCTION qmi_psm_set_sensor_logging_level_resp()

  DESCRIPTION
    Requests modem to enable sensor logging
    
  PARAMETERS
    enable_logging      :       Enable logging
    log_level           :       Logging level

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_psm_set_sensor_logging_level_resp
(
  qmi_error_type_v01                 errval
)
{
  psm_sensor_logging_resp_msg_v01   resp_msg;
  qmi_mmode_qcsi_transaction_type   *cmd_buf_p = qmi_psmi_global.sns_set_logging_level_cmd_buf_p;

  qmi_psmi_global.sns_set_logging_level_cmd_buf_p = NULL;
  if(cmd_buf_p == NULL || qmi_psmi_validate_cmd_buf_p(cmd_buf_p) == FALSE)
  {
    QM_MSG_HIGH("No pending request/client got deallocated");
  }

  memset(&resp_msg,0,sizeof(psm_sensor_logging_resp_msg_v01));
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  QM_MSG_HIGH_2("qmi_psm_set_sensor_logging_level result %d errval %d",
          resp_msg.resp.result, errval);
  
  qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                    sizeof(psm_sensor_logging_resp_msg_v01));
}

/*===========================================================================
  FUNCTION qmi_psm_set_sensor_configuration_ext()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_set_sensor_configuration_ext
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_set_sensor_configuration_ext_req_msg_v01   *req_msg;
  psm_set_sensor_configuration_ext_resp_msg_v01   resp_msg;
  qmi_error_type_v01                          errval = QMI_ERR_NONE_V01;
  cm_sns_config_data_s_type                   sensor_config[CM_SNS_TYPE_ENV_SENSOR_MAX ];
  cm_sns_config_s_type                        sns_config_ext;
  uint8                                       policy_len = 0;
  uint8                                       config_list =0; 
  uint8                                       config_list_env=0;
  boolean                                     sns_para_set = FALSE;
  /*--------------------------------------------------*/
  ASSERT(req_c_struct && (req_c_struct_len > 0));

  memset(&resp_msg,0,sizeof(psm_set_sensor_configuration_ext_resp_msg_v01));
  memset(&sensor_config,0,(sizeof(cm_sns_config_data_s_type)*CM_SNS_TYPE_ENV_SENSOR_MAX));
  memset(&sns_config_ext,0,(sizeof(cm_sns_config_s_type)));
  req_msg = (psm_set_sensor_configuration_ext_req_msg_v01 *)req_c_struct;
  
  if( qmi_psmi_global.sns_set_config_ext_cmd_buf_p != NULL )
  {  
    QM_MSG_HIGH("Set Configuration command is already in progress");
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
 
  if(req_msg->sensor_config_list_valid)
  {
    policy_len = (uint8)req_msg->sensor_config_list_len;

    if(req_msg->sensor_config_list_len > QMI_PSM_SENSOR_CONFIG_MAX_V01)
    {
    QM_MSG_HIGH_1("Invalid command as the total sensor is = %d and Max allowed sensor is CM_SNS_TYPE_ENV_SENSOR_MAX",req_msg->sensor_config_list_len);
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
    }

    QM_MSG_HIGH_2("policy length = %d and req_msg_length =%d",policy_len,req_msg->sensor_config_list_len);
  
    for(config_list =0 ;config_list < (uint8)req_msg->sensor_config_list_len; config_list++)
    {
       if((req_msg->sensor_config_list[config_list].sensor_id >= PSM_SENSOR_TYPE_PRESSURE_V01) && (req_msg->sensor_config_list[config_list].sensor_id <= PSM_SENSOR_TYPE_TEMPERATURE_V01) && (config_list_env < CM_SNS_TYPE_ENV_SENSOR_MAX) )
       {
       sensor_config[config_list_env].sns_id = 
                              req_msg->sensor_config_list[config_list].sensor_id;
       sensor_config[config_list_env].sns_max_batch_cnt = CM_MAX_BATCH_SUPPORTED;
       sensor_config[config_list_env].sns_interval =
                             req_msg->sensor_config_list[config_list].measurement_period;
       
       sensor_config[config_list_env].sns_high_perf_interval = 
                                 req_msg->sensor_config_list[config_list].high_perf_measurement_period;
       sensor_config[config_list_env].operating_mode =
                                 req_msg->sensor_config_list[config_list].operating_mode;
       sensor_config[config_list_env].sns_performance_mode = 
                                 req_msg->sensor_config_list[config_list].performance_mode;
       sensor_config[config_list_env].sns_hysteresis_duration = 
                                 req_msg->sensor_config_list[config_list].hysteresis_duration;
       sensor_config[config_list_env].sns_hysteresis_count = req_msg->sensor_config_list[config_list].hysteresis_duration;
       
       if(req_msg->sensor_config_list[config_list].sensor_policy_len >CM_MAX_SNS_DIMENSION)
       {
          QM_MSG_HIGH("Max policy allwed at lower layers is 2");
          errval = QMI_ERR_INVALID_ARG_V01;
          goto send_result;
       }
       sensor_config[config_list_env].sns_policy.sns_policy_elem[0].greater_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[0].alert_threshold_high;
       sensor_config[config_list_env].sns_policy.sns_policy_elem[0].lower_value =
                        req_msg->sensor_config_list[config_list].sensor_policy[0].alert_threshold_low;
       sensor_config[config_list_env].sns_policy.sns_policy_elem[0].pre_greater_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[0].pre_alert_threshold_high;
       sensor_config[config_list_env].sns_policy.sns_policy_elem[0].pre_lower_value =
                        req_msg->sensor_config_list[config_list].sensor_policy[0].pre_alert_threshold_low;
       sensor_config[config_list_env].sns_policy.sns_policy_elem[0].delta_value = (float)0XFFFF;

       if(req_msg->sensor_config_list[config_list].sensor_policy_len > 1)
       {
         sensor_config[config_list_env].sns_policy.sns_policy_elem[1].greater_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[1].alert_threshold_high;
         sensor_config[config_list_env].sns_policy.sns_policy_elem[1].lower_value =
                        req_msg->sensor_config_list[config_list].sensor_policy[1].alert_threshold_low;
         sensor_config[config_list_env].sns_policy.sns_policy_elem[1].pre_greater_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[1].pre_alert_threshold_high;
         sensor_config[config_list_env].sns_policy.sns_policy_elem[1].pre_lower_value =
                        req_msg->sensor_config_list[config_list].sensor_policy[1].pre_alert_threshold_low;
         sensor_config[config_list_env].sns_policy.sns_policy_elem[1].delta_value = (float)0XFFFF;
       }
       config_list_env++;
       
       
      }


      if((req_msg->sensor_config_list[config_list].sensor_id == PSM_SENSOR_TYPE_FALL_DETECT_V01))
      {
         sns_config_ext.non_env_sns_mask =  sns_config_ext.non_env_sns_mask | ((1<<CM_SNS_TYPE_FALL_DETECT));
         sns_config_ext.fall_config.is_enable = req_msg->sensor_config_list[config_list].operating_mode;

      }
       
     if((req_msg->sensor_config_list[config_list].sensor_id == PSM_SENSOR_TYPE_FMD_V01))
      {
         sns_config_ext.non_env_sns_mask =  sns_config_ext.non_env_sns_mask | ((1<<CM_SNS_TYPE_FMD));
         sns_config_ext.fmd_config.is_enable = req_msg->sensor_config_list[config_list].operating_mode;
         if(req_msg->fmd_accel_thres_valid)
         {
             sns_para_set = TRUE;
            sns_config_ext.fmd_config.accel_threshold= req_msg->fmd_accel_thres;;
         }
         else 
         {
           sns_config_ext.fmd_config.accel_threshold= (float)0xFFFFFFFF ;
         }
         if(req_msg->fmd_accel_buffer_count_valid)
         {
            sns_para_set = TRUE;
            sns_config_ext.fmd_config.accel_buffer_count= req_msg->fmd_accel_buffer_count;
         }
         else 
         {
           sns_config_ext.fmd_config.accel_buffer_count= 0xFFFFFFFF ;
         }
          if(req_msg->fmd_run_period_valid)
         {  
           sns_para_set = TRUE;
           sns_config_ext.fmd_config.run_duration= req_msg->fmd_run_period;;
         }
         else
         {
           sns_config_ext.fmd_config.run_duration= 0xFFFFFFFF ;
         }
      
      } 

      if((req_msg->sensor_config_list[config_list].sensor_id == PSM_SENSOR_TYPE_ALS_V01))
      {
         sns_config_ext.non_env_sns_mask =  sns_config_ext.non_env_sns_mask | ((1<<PSM_SENSOR_TYPE_ALS_V01));
         sns_config_ext.als_config.is_enable = req_msg->sensor_config_list[config_list].operating_mode;
         if(req_msg->als_threshold_valid)
         {
            sns_config_ext.als_config.threshold = req_msg->als_threshold;
         }
         sns_config_ext.als_config.interval = req_msg->sensor_config_list[config_list].measurement_period;
      }

       if((req_msg->sensor_config_list[config_list].sensor_id == PSM_SENSOR_TYPE_TILT_DETECT_V01))
       {
         sns_config_ext.non_env_sns_mask =  sns_config_ext.non_env_sns_mask | ((1<<CM_SNS_TYPE_TILT));
         sns_config_ext.tilt_config.is_enable = req_msg->sensor_config_list[config_list].operating_mode;
         if(req_msg->transportation_type_valid)
         {  
            sns_para_set = TRUE;
            sns_config_ext.tilt_config.transportation_type= (cm_sns_tilt_tracker_config_transportation_type)req_msg->transportation_type;
         }
         else 
         {
            sns_config_ext.tilt_config.transportation_type = 0;
         } 
         if(req_msg->redo_gravity_init_valid)
         {  sns_para_set = TRUE;
            sns_config_ext.tilt_config.has_redo_gravity_init = TRUE;
            sns_config_ext.tilt_config.redo_gravity_init= req_msg->redo_gravity_init;
         }
        else 
         {
            sns_config_ext.tilt_config.redo_gravity_init = 0;
         }
        if(req_msg->iteration_delay_valid)
         {  
            sns_para_set = TRUE;
            sns_config_ext.tilt_config.has_iteration_delay = TRUE;
            sns_config_ext.tilt_config.iteration_delay= req_msg->iteration_delay;
         }
       else 
        {
            sns_config_ext.tilt_config.iteration_delay = 0xFFFFFFFF;
        }
         
       if(req_msg->init_max_trial_num_valid)
        {
           sns_para_set = TRUE;
           sns_config_ext.tilt_config.has_max_init_trial_num = TRUE;
           sns_config_ext.tilt_config.max_init_trial_num= req_msg->init_max_trial_num;
        }
       else 
       {
         sns_config_ext.tilt_config.max_init_trial_num = 0xFF;
       }
       if(req_msg->update_max_trial_num_valid)
       {
            sns_para_set = TRUE;
            sns_config_ext.tilt_config.has_max_update_trial_num = TRUE;
            sns_config_ext.tilt_config.max_update_trial_num= req_msg->update_max_trial_num;
       }
       else 
       {
           sns_config_ext.tilt_config.max_update_trial_num = 0xFF;
       }
          
       if(req_msg->init_gravity_threshold_valid)
        {
            sns_para_set = TRUE;
            sns_config_ext.tilt_config.has_init_gravity_threshold= TRUE;
            sns_config_ext.tilt_config.init_gravity_threshold= req_msg->init_gravity_threshold;
        }
      else 
       {
          sns_config_ext.tilt_config.init_gravity_threshold = (float)0xFFFFFFFF;
       } 
      if(req_msg->update_gravity_threshold_valid)
       {

	       sns_para_set = TRUE;
          sns_config_ext.tilt_config.has_update_gravity_threshold= TRUE;
          sns_config_ext.tilt_config.update_gravity_threshold= req_msg->update_gravity_threshold;
       }
      else 
       {
         sns_config_ext.tilt_config.update_gravity_threshold = 0xFF;
       }  
      }

      if((req_msg->sensor_config_list[config_list].sensor_id == PSM_SENSOR_TYPE_SMD_V01))
      {
        sns_config_ext.non_env_sns_mask =  sns_config_ext.non_env_sns_mask | ((1<<CM_SNS_TYPE_SMD));

         if(req_msg->smd_perceptron_min_features_valid)
         {  sns_para_set = TRUE;
           memscpy( sns_config_ext.smd_config.smd_perceptron_min_weight, smd_perceptron_min_feature_weight*sizeof(float), req_msg->smd_perceptron_min_features, smd_perceptron_min_feature_weight*sizeof(float) );
         }
        else 
         {
           sns_config_ext.smd_config.smd_perceptron_min_weight[0]= (float)0xFFFFFFFF ;
         }
         if(req_msg->smd_perceptron_weights_valid)
         {
           sns_para_set = TRUE;
           memscpy( sns_config_ext.smd_config.smd_weight, smd_perceptron_weight*sizeof(float), req_msg->smd_perceptron_weights, smd_perceptron_weight*sizeof(float) );
         }
        else 
         {
           sns_config_ext.smd_config.smd_weight[0]= (float)0xFFFFFFFF ;
         }
       
         if(req_msg->smd_perceptron_max_features_valid)
         {
           sns_para_set = TRUE;
           memscpy( sns_config_ext.smd_config.smd_perceptron_max_weight, smd_perceptron_max_feature_weight*sizeof(float), req_msg->smd_perceptron_max_features, smd_perceptron_max_feature_weight*sizeof(float) );
         }
        else 
         {
           sns_config_ext.smd_config.smd_perceptron_max_weight[0]= (float)0xFFFFFFFF ;
         }
         if(req_msg->smd_sum_score_threshold_valid )
         {

		   sns_para_set = TRUE;
           sns_config_ext.smd_config.smd_sum_score_threshold = req_msg->smd_sum_score_threshold;
         }
         else 
         {
           sns_config_ext.smd_config.smd_sum_score_threshold= 0xFF ;
         }
     
      }
      if((req_msg->sensor_config_list[config_list].sensor_id == PSM_SENSOR_TYPE_MOTION_DETECT_V01))
      {
         sns_config_ext.non_env_sns_mask =  sns_config_ext.non_env_sns_mask | ((1<<CM_SNS_TYPE_MOTION_DETECT));
         if(req_msg->md_tuning_threshold_valid)
         {
           sns_para_set = TRUE;
          sns_config_ext.md_config.md_tuning_threshold = req_msg->md_tuning_threshold;
         }
        else 
        {
          sns_config_ext.md_config.md_tuning_threshold = (float)0xFFFFFFFF;
        }
      }
     if((req_msg->sensor_config_list[config_list].sensor_id == PSM_SENSOR_TYPE_MCD_V01))
      {
         sns_config_ext.non_env_sns_mask =  sns_config_ext.non_env_sns_mask | ((1<<CM_SNS_TYPE_MCD));
         sns_config_ext.mcd_config.is_enable = req_msg->sensor_config_list[config_list].operating_mode;
         if(req_msg->mcd_motion_toggle_counter_threshold_valid  )
         {  sns_para_set = TRUE;
            sns_config_ext.mcd_config.mcd_motion_toggle_counter_threshold= req_msg->mcd_motion_toggle_counter_threshold;
         }
         else 
         {
            sns_config_ext.mcd_config.mcd_motion_toggle_counter_threshold = 0xFF; 
         }

         if(req_msg->mcd_motion_toggle_delay_threshold_valid )
         {
            sns_para_set = TRUE;
            sns_config_ext.mcd_config.mcd_motion_toggle_delay_threshold= req_msg->mcd_motion_toggle_delay_threshold;
            
         }
         else 
         {
            sns_config_ext.mcd_config.mcd_motion_toggle_delay_threshold = 0xFFFF; 
         }
         if(req_msg->mcd_psm_timer_config_valid  )
         {
            sns_para_set = TRUE;
            sns_config_ext.mcd_config.mcd_psm_timer_config= req_msg->mcd_psm_timer_config;
         }
         else 
         {
            sns_config_ext.mcd_config.mcd_psm_timer_config = 0xFF; 
         }

      }
    }  
    }

  if((config_list_env >= 0) && (config_list_env < CM_SNS_TYPE_ENV_SENSOR_MAX))
  {
  	memscpy(sns_config_ext.sns_arr,
          config_list_env * sizeof(cm_sns_config_data_s_type),
          sensor_config,
          config_list_env * sizeof(cm_sns_config_data_s_type));
	sns_config_ext.len = config_list_env;
  }

  if((sns_config_ext.non_env_sns_mask & (1<<CM_SNS_TYPE_MCD))!=0
  	|| (sns_config_ext.non_env_sns_mask & (1<<CM_SNS_TYPE_FMD))!=0
  	|| (sns_config_ext.non_env_sns_mask & (1<<CM_SNS_TYPE_TILT))!=0
  	|| (sns_config_ext.non_env_sns_mask & (1<<CM_SNS_TYPE_MOTION_DETECT))!=0
  	|| (sns_config_ext.non_env_sns_mask & (1<<CM_SNS_TYPE_SMD))!=0)

  {

     if(sns_para_set == FALSE)
   {
    if(TRUE !=  cm_sns_cmd_set_config_non_env(qmi_psm_sensor_cm_cmd_callback,
                                       qmi_psm_get_cm_client_id(),
                                       &sns_config_ext,
                                        policy_len
                                       ))
     {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
     }
    }
   else
 	{
   if(TRUE !=  cm_sns_cmd_set_config_non_env(NULL,
                                       qmi_psm_get_cm_client_id(),
                                       &sns_config_ext,
                                        policy_len
                                       ))
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
 }
									   
  qmi_psmi_global.sns_set_config_ext_cmd_buf_p = cmd_buf_p;
  QM_MSG_HIGH("PSM set_sensor_configuration queued");
  }
  else
  {
  if(TRUE !=  cm_sns_cmd_set_config_non_env(qmi_psm_sensor_cm_cmd_callback,
                                       qmi_psm_get_cm_client_id(),
                                       &sns_config_ext,
                                        policy_len
                                       ))
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
 
  qmi_psmi_global.sns_set_config_ext_cmd_buf_p = cmd_buf_p;
  QM_MSG_HIGH("PSM set_sensor_configuration queued");
}
send_result:
  if (errval != QMI_ERR_NONE_V01)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error  = errval; 
    QM_MSG_HIGH_2("qmi_psm_set_sensor_configuration result %d errval %d", resp_msg.resp.result, errval);
            
    qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                      sizeof(psm_set_sensor_configuration_resp_msg_v01));
  }
          
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_psm_set_sensor_configuration_ext_resp()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
     errval received from CM cmd callback

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_psm_set_sensor_configuration_ext_resp
(
  qmi_error_type_v01                 errval
)
{
  psm_set_sensor_configuration_ext_resp_msg_v01   resp_msg;
  qmi_mmode_qcsi_transaction_type            *cmd_buf_p = qmi_psmi_global.sns_set_config_ext_cmd_buf_p;
  uint8 err_len = 0;
  uint8 sns_len;
  config_init_output* output;

  qmi_psmi_global.sns_set_config_ext_cmd_buf_p = NULL;
  if(cmd_buf_p == NULL || qmi_psmi_validate_cmd_buf_p(cmd_buf_p) == FALSE)
  {
    QM_MSG_HIGH("No pending request/client got deallocated");
  }

  memset(&resp_msg,0,sizeof(psm_set_sensor_configuration_ext_resp_msg_v01));

  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  if(errval != QMI_ERR_NONE_V01)
  {
    output =  cm_send_algo_output();
	resp_msg.sensor_error_resp_valid = TRUE;
    
    for(sns_len = 0;sns_len<CM_MAX_SNS_ENABLED;sns_len++)
	{
	  if( output[sns_len].ACK == ACK_ERROR )
	  {
	    resp_msg.sensor_error_resp[err_len].sns_id = output[sns_len].sensor_type;
	    resp_msg.sensor_error_resp[err_len].error_config = output[sns_len].ACK_error_config;
	    resp_msg.sensor_error_resp[err_len].error_type = output[sns_len].ACK_error_type;
		err_len++;
	  }
	  
    }
	resp_msg.sensor_error_resp_len = err_len;
	memset(output,0,sizeof(config_init_output) * CM_MAX_SNS_ENABLED);
  }
  QM_MSG_HIGH_3("qmi_psm_set_sensor_configuration_ext result %d errval %d err_len %d",
          resp_msg.resp.result, errval, err_len);
  
  qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                    sizeof(psm_set_sensor_configuration_ext_resp_msg_v01));
}

/*===========================================================================
  FUNCTION qmi_psm_get_sensor_configuration()

  DESCRIPTION
    Gets the sensor policy information from modem 
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_get_sensor_configuration
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_get_sensor_configuration_req_msg_v01   *req_msg;
  psm_get_sensor_configuration_resp_msg_v01   resp_msg;
  int index;
  boolean flag = 0;
  qmi_error_type_v01                          errval = QMI_ERR_NONE_V01;
  qmi_psm_pending_cmd_buf_addr_s_type  *pend_req_addr = NULL;
  cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();
  cm_sns_id_e_type		  req_sns_id = CM_SNS_TYPE_NONE;

  /*--------------------------------------------------*/
  ASSERT(req_c_struct && (req_c_struct_len > 0));

 

  memset(&resp_msg,0,sizeof(psm_get_sensor_configuration_resp_msg_v01));
 /* store the request to into pending requests queue */
  pend_req_addr = QMI_PSM_MEM_ALLOC( sizeof(qmi_psm_pending_cmd_buf_addr_s_type) );
  if(!pend_req_addr)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  memset(pend_req_addr, 0, sizeof(qmi_psm_pending_cmd_buf_addr_s_type));
  (void) q_link ( pend_req_addr, &pend_req_addr->link );
  pend_req_addr->cmd_buf_addr = (void *)cmd_buf_p;
  req_msg = (psm_get_sensor_configuration_req_msg_v01 *)req_c_struct;
  
   switch(req_msg->sensor_id)
   {
 	case PSM_SENSOR_TYPE_PRESSURE_V01: 
		 req_sns_id = CM_SNS_TYPE_PRESSURE;
         break;
	
    case PSM_SENSOR_TYPE_HUMIDITY_V01:
		 req_sns_id = CM_SNS_TYPE_HUMIDITY;
         break;
	
    case PSM_SENSOR_TYPE_TEMPERATURE_V01:
	     req_sns_id = CM_SNS_TYPE_TEMPERATURE;
         break;

	case PSM_SENSOR_TYPE_FALL_DETECT_V01:
	     req_sns_id = CM_SNS_TYPE_FALL_DETECT;
         break;
	case PSM_SENSOR_TYPE_ALS_V01:
	     req_sns_id = CM_SNS_TYPE_ALS;
         break;	 

       case PSM_SENSOR_TYPE_FMD_V01:
	  req_sns_id = CM_SNS_TYPE_FMD;
         break;

	case PSM_SENSOR_TYPE_TILT_DETECT_V01:
	  req_sns_id = CM_SNS_TYPE_TILT;
         break; 
	default: req_sns_id = CM_SNS_TYPE_NONE;
   }

  QM_MSG_HIGH("qmi_psm_get_sensor_configuration");

 if(sns_ptr->sns_data_arr != NULL)
 {
   for(index = 0;index < CM_SNS_TYPE_ENV_SENSOR_MAX ; index++)
   {
	if(sns_ptr->sns_data_arr[index].sns_config_data.sns_id == req_sns_id)
	{
       resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
	   resp_msg.resp.error	= QMI_ERR_NONE_V01; 

	   resp_msg.sensor_config_list_valid = TRUE;
	   resp_msg.sensor_config_list.sensor_id = req_msg->sensor_id;
	   resp_msg.sensor_config_list.sensor_policy_len = 1;
	   resp_msg.sensor_config_list.sensor_policy->alert_threshold_high = 
         	   	sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem[0].greater_value;
	   resp_msg.sensor_config_list.sensor_policy->alert_threshold_low = 
         	   	sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem[0].lower_value;
	   resp_msg.sensor_config_list.sensor_policy->pre_alert_threshold_high = 
         	   	sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem[0].pre_greater_value;
       resp_msg.sensor_config_list.sensor_policy->pre_alert_threshold_low = 
         	   	sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem[0].pre_lower_value;
	   resp_msg.sensor_config_list.performance_mode = sns_ptr->sns_data_arr[index].sns_config_data.sns_performance_mode;
	   resp_msg.sensor_config_list.operating_mode = sns_ptr->sns_data_arr[index].sns_config_data.operating_mode;
	   resp_msg.sensor_config_list.measurement_period = sns_ptr->sns_data_arr[index].sns_config_data.sns_interval;
	   resp_msg.sensor_config_list.high_perf_measurement_period = sns_ptr->sns_data_arr[index].sns_config_data.sns_high_perf_interval;
	   resp_msg.sensor_config_list.hysteresis_duration = sns_ptr->sns_data_arr[index].sns_config_data.sns_hysteresis_duration;	 

	   flag++;
	   break;
	}	
   }
 	
   if(req_sns_id == CM_SNS_TYPE_ALS )
   {
      resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
	  resp_msg.resp.error	= QMI_ERR_NONE_V01; 
	  resp_msg.sensor_config_list_valid = TRUE;
      resp_msg.sensor_config_list.sensor_id = req_msg->sensor_id;
      resp_msg.sensor_config_list.operating_mode = sns_ptr->sns_intr_data.als_data.is_enable;
      resp_msg.als_threshold_valid = TRUE;
	  resp_msg.als_threshold = sns_ptr->sns_intr_data.als_data.threshold;
	  flag++;
	  QM_MSG_HIGH("qmi_psm_get_sensor_configuration for ALS");
	}


   if(req_sns_id == CM_SNS_TYPE_FALL_DETECT )
   {

      resp_msg.sensor_config_list_valid = TRUE;
	   resp_msg.sensor_config_list.sensor_id = req_msg->sensor_id;
      resp_msg.sensor_config_list.operating_mode = sns_ptr->sns_intr_data.fall_data.is_enable;
      
	  flag++;
	  QM_MSG_HIGH("qmi_psm_get_sensor_configuration for Fall detect");

	  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
	   resp_msg.resp.error	= QMI_ERR_NONE_V01; 
   }	
   }

 if(req_msg->sensor_id != PSM_SENSOR_TYPE_SMD_V01 && req_msg->sensor_id != PSM_SENSOR_TYPE_MCD_V01
 	&& req_msg->sensor_id != PSM_SENSOR_TYPE_MOTION_DETECT_V01 && req_msg->sensor_id != PSM_SENSOR_TYPE_FMD_V01
 	&& req_msg->sensor_id != PSM_SENSOR_TYPE_TILT_DETECT_V01)
    {
   qmi_psmi_send_response(cmd_buf_p, &resp_msg, sizeof(psm_get_sensor_configuration_resp_msg_v01));
   return QMI_CSI_CB_NO_ERR;
    }
     if (q_check(&qmi_psmi_global.sns_get_config_cmd_buf_p) == NULL)
    {
    if(TRUE != cm_sns_cmd_get_config_req(qmi_psm_sensor_cm_cmd_callback,
                                      qmi_psm_get_cm_client_id(),
                                      (cm_sns_id_e_type)(req_msg->sensor_id) ))
     {
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
     }
    }

  // Queue and wait for CM response
  q_put( &(qmi_psmi_global.sns_get_config_cmd_buf_p), &(pend_req_addr->link) );
  QM_MSG_HIGH("PSM get_config sensor_req queued");
	
send_result:

  if (errval != QMI_ERR_NONE_V01)
	{
    	resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error  = errval; 
    QM_MSG_HIGH_2("qmi_psm_get_config result %d errval %d", resp_msg.resp.result, errval);
            
    qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                      sizeof(psm_get_sensor_data_resp_msg_v01));
	 if (pend_req_addr != NULL)
    {
      QMI_PSM_MEM_FREE(pend_req_addr);
    }
	}
  
  return QMI_CSI_CB_NO_ERR;

}

/*===========================================================================
  FUNCTION qmi_psm_get_sensor_configuration()

  DESCRIPTION
    Trigger the FMD status of flight
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error qmi_psm_get_fmd_sns_sample_req
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{

  psm_get_fmd_sns_sample_req_msg_v01   *req_msg;
  psm_get_fmd_sns_sample_resp_msg_v01   resp_msg;
  qmi_error_type_v01                 errval = QMI_ERR_NONE_V01;
  cm_sns_config_fmd_data_s_type          sensor_config;
  qmi_psm_pending_cmd_buf_addr_s_type  *pend_req_addr = NULL;
  boolean logging_mode = FALSE;
  uint32 fmd_mask = 0;
  /*--------------------------------------------------*/

  ASSERT( req_c_struct &&(req_c_struct_len > 0) );

  memset(&resp_msg,0,sizeof(psm_get_fmd_sns_sample_resp_msg_v01));
  memset(&sensor_config,0,sizeof(cm_sns_config_fmd_data_s_type));
  req_msg = (psm_get_fmd_sns_sample_req_msg_v01 *)req_c_struct;
  
  /* store the request to into pending requests queue */
  pend_req_addr = QMI_PSM_MEM_ALLOC( sizeof(qmi_psm_pending_cmd_buf_addr_s_type) );
  if(!pend_req_addr)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
	goto send_result;
    
  }
 
  
    memset(pend_req_addr, 0, sizeof(qmi_psm_pending_cmd_buf_addr_s_type));
    (void) q_link ( pend_req_addr, &pend_req_addr->link );
    pend_req_addr->cmd_buf_addr = (void *)cmd_buf_p;
  
  
  fmd_mask |= (1<<CM_SNS_TYPE_FMD);
  // Send command to CM only if no pending request
  if (qmi_psmi_global.sns_get_fmd_sns_sample_cmd_buf_p == NULL)
  {
    if(TRUE != cm_sns_cmd_ondemand_sns_req(qmi_psm_sensor_cm_cmd_callback,
                                      qmi_psm_get_cm_client_id(),
                                      (fmd_mask)

 ))
    {
      errval = QMI_ERR_INTERNAL_V01;
	  goto send_result;
      
    }
    else 
    	{
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    resp_msg.resp.error  = errval; 
    QM_MSG_HIGH("On demand req has been sent for FMD");
            
    qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                      sizeof(psm_get_fmd_sns_sample_resp_msg_v01));

    if (pend_req_addr != NULL)
    {
      QMI_PSM_MEM_FREE(pend_req_addr);
    }
    	}
  }

  send_result:


  if (errval != QMI_ERR_NONE_V01)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error  = errval; 
    QM_MSG_HIGH_2("qmi_psm_get_sensor_data result %d errval %d", resp_msg.resp.result, errval);
            
    qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                      sizeof(psm_get_fmd_sns_sample_resp_msg_v01));

    if (pend_req_addr != NULL)
    {
      QMI_PSM_MEM_FREE(pend_req_addr);
    }
  }
          
  return QMI_CSI_CB_NO_ERR;


}


#endif
uint32 psm_convert_qmi_to_cm_sensor_mask
( 
  psm_sensor_id_mask_v01 qmi_sensor_mask
)
{
  uint32 ret_mask =0;
  if(qmi_sensor_mask & PSM_MASK_SENSOR_ID_ALS_V01)
    ret_mask |= (1<< CM_SNS_TYPE_ALS);
  if(qmi_sensor_mask & PSM_MASK_SENSOR_ID_PRESSURE_V01)  	
    ret_mask |= (1<< CM_SNS_TYPE_PRESSURE);
  if(qmi_sensor_mask & PSM_MASK_SENSOR_ID_HUMIDITY_V01)
    ret_mask |= (1<< CM_SNS_TYPE_HUMIDITY);
  if(qmi_sensor_mask & PSM_MASK_SENSOR_ID_TEMPERATURE_V01)
    ret_mask |= (1<< CM_SNS_TYPE_TEMPERATURE);
  if(qmi_sensor_mask & PSM_MASK_SENSOR_ID_AMD_V01)
    ret_mask |= (1<< CM_SNS_TYPE_AMD);
  if(qmi_sensor_mask & PSM_MASK_SENSOR_ID_SMD_V01)
    ret_mask |= (1<< CM_SNS_TYPE_SMD);
  if(qmi_sensor_mask & PSM_MASK_SENSOR_ID_FALL_DETECT_V01)
    ret_mask |= (1<< CM_SNS_TYPE_FALL_DETECT);
  if(qmi_sensor_mask & PSM_MASK_SENSOR_ID_FMD_V01)
    ret_mask |= (1<< CM_SNS_TYPE_FMD);
 if(qmi_sensor_mask & PSM_MASK_SENSOR_ID_TILT_DETECT_V01)
   ret_mask |= (1<< CM_SNS_TYPE_TILT);
  if(qmi_sensor_mask & PSM_MASK_SENSOR_ID_MCD_V01)
   ret_mask |= (1<< CM_SNS_TYPE_MCD);

 return ret_mask;
}

psm_sensor_id_mask_v01 psm_convert_cm_to_qmi_sensor_mask
( 
  uint32 cm_sensor_mask
)
{
  psm_sensor_id_mask_v01 ret_mask = 0;
  if(cm_sensor_mask & (1 << CM_SNS_TYPE_ALS))
    ret_mask |= PSM_MASK_SENSOR_ID_ALS_V01;
  if(cm_sensor_mask & (1<<CM_SNS_TYPE_PRESSURE))  	
    ret_mask |= PSM_MASK_SENSOR_ID_PRESSURE_V01;
  if(cm_sensor_mask &(1<< CM_SNS_TYPE_HUMIDITY))
    ret_mask |= PSM_MASK_SENSOR_ID_HUMIDITY_V01;
  if(cm_sensor_mask & (1<<CM_SNS_TYPE_TEMPERATURE))
    ret_mask |= PSM_MASK_SENSOR_ID_TEMPERATURE_V01;
  if(cm_sensor_mask & (1<<CM_SNS_TYPE_AMD))
    ret_mask |= PSM_MASK_SENSOR_ID_AMD_V01;
  if(cm_sensor_mask & (1<<CM_SNS_TYPE_SMD))
    ret_mask |= PSM_MASK_SENSOR_ID_SMD_V01;
  if(cm_sensor_mask & (1<<CM_SNS_TYPE_FALL_DETECT))
    ret_mask |= PSM_MASK_SENSOR_ID_FALL_DETECT_V01;
   if(cm_sensor_mask & (1<<CM_SNS_TYPE_FMD))
    ret_mask |= PSM_MASK_SENSOR_ID_FMD_V01;
  if(cm_sensor_mask & (1<<CM_SNS_TYPE_TILT))
    ret_mask |= PSM_MASK_SENSOR_ID_TILT_DETECT_V01;
   if(cm_sensor_mask & (1<<CM_SNS_TYPE_MCD))
    ret_mask |= PSM_MASK_SENSOR_ID_MCD_V01;
  QM_MSG_HIGH_1("qmi_psm_set_sensor_configuration ret_mask %d", ret_mask);
  
 return ret_mask;
}

/*===========================================================================
  FUNCTION qmi_psm_get_sensor_data()

  DESCRIPTION
    Gets the sensor information from modem based on the sensor ID pass
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_get_sensor_data
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_get_sensor_data_req_msg_v01   *req_msg;
  psm_get_sensor_data_resp_msg_v01   resp_msg;
  qmi_error_type_v01                 errval = QMI_ERR_NONE_V01;
  cm_sns_config_data_s_type          sensor_config;
  qmi_psm_pending_cmd_buf_addr_s_type  *pend_req_addr = NULL;
  boolean logging_mode = FALSE;
  /*--------------------------------------------------*/

  ASSERT( req_c_struct &&(req_c_struct_len > 0) );

  memset(&resp_msg,0,sizeof(psm_get_sensor_data_resp_msg_v01));
  memset(&sensor_config,0,sizeof(cm_sns_config_data_s_type));
  req_msg = (psm_get_sensor_data_req_msg_v01 *)req_c_struct;
  
  /* store the request to into pending requests queue */
  pend_req_addr = QMI_PSM_MEM_ALLOC( sizeof(qmi_psm_pending_cmd_buf_addr_s_type) );
  if(!pend_req_addr)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
  
  memset(pend_req_addr, 0, sizeof(qmi_psm_pending_cmd_buf_addr_s_type));
  (void) q_link ( pend_req_addr, &pend_req_addr->link );
  pend_req_addr->cmd_buf_addr = (void *)cmd_buf_p;
  
  req_msg->sensor_id_mask = req_msg->sensor_id_mask & PSM_MASK_MAX;
  
  if(req_msg->sensor_id_mask == 0)
  {
     errval = QMI_ERR_INVALID_ARG_V01;
     goto send_result;
  }   

  if(req_msg->is_logging_mode_valid)
  {
    logging_mode = req_msg->is_logging_mode;
  }
  
  // Send command to CM only if no pending request
  if (q_check(&qmi_psmi_global.sns_get_data_cmd_buf_p) == NULL)
  {
    if(TRUE != cm_sns_cmd_data_req(qmi_psm_sensor_cm_cmd_callback,
                                      qmi_psm_get_cm_client_id(),
                                      psm_convert_qmi_to_cm_sensor_mask(req_msg->sensor_id_mask)
#ifdef FEATURE_MMODE_TX_21
                                      , logging_mode                                
#endif
 ))
    {
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
  }

  // Queue and wait for CM response
  q_put( &(qmi_psmi_global.sns_get_data_cmd_buf_p), &(pend_req_addr->link) );
  QM_MSG_HIGH("PSM get_sensor_data_req queued");
  
send_result:

  if (errval != QMI_ERR_NONE_V01)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error  = errval; 
    QM_MSG_HIGH_2("qmi_psm_get_sensor_data result %d errval %d", resp_msg.resp.result, errval);
            
    qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                      sizeof(psm_get_sensor_data_resp_msg_v01));

    if (pend_req_addr != NULL)
    {
      QMI_PSM_MEM_FREE(pend_req_addr);
    }
  }
          
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_psm_get_sensor_data_resp()

  DESCRIPTION
    Sends the data      response about the policy met
    
  PARAMETERS
  cmd_buf_ptr :  Sensor cfg    cmd buf ptr

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_psm_get_sensor_data_resp
(
  cm_qmi_sns_data_collection_rsp_s_type sensor_data_collection_rsp
)
{
  psm_get_sensor_data_resp_msg_v01               resp_msg;  
  uint8                                          dimension_cnt =0;
  cm_qmi_sensor_data_rsp_s_type                 *cm_sensor_data_resp;  
  uint8                                          resp_cnt;
  uint8                                          sns_len =0;
  uint8                                          batch_len =0;  
  qmi_error_type_v01                             errval = QMI_ERR_NONE_V01;
  qmi_psm_pending_cmd_buf_addr_s_type           *pend_req_addr = NULL;
/*----------------------------------------------------------------*/
  memset(&resp_msg,0,sizeof(psm_get_sensor_data_resp_msg_v01));
  
  QM_MSG_HIGH("qmi_psm_sensor_get_data_resp");
  
  resp_cnt = sensor_data_collection_rsp.sns_len; 
  resp_msg.sensor_info_valid = TRUE;
  resp_msg.sensor_info_len = resp_cnt;
#ifdef FEATURE_MMODE_TX_21
  resp_msg.algo_info_valid = TRUE;
  resp_msg.algo_info_len = resp_cnt;
 #endif 
  if(resp_msg.sensor_info_len > CM_SNS_TYPE_ENV_SENSOR_MAX)
  {
      errval = QMI_ERR_INVALID_ARG_V01;
	  goto send_result;
  }

  for(sns_len = 0;sns_len< resp_msg.sensor_info_len ;sns_len++)
  {

    cm_sensor_data_resp = &sensor_data_collection_rsp.sns_arr[sns_len];
    resp_msg.sensor_info[sns_len].sensor_id = 
         (psm_sensor_type_id_v01)cm_sensor_data_resp->sensor_id;

    resp_msg.sensor_info[sns_len].sample_list_len       = 
         cm_sensor_data_resp->batch_count;
    
    for(batch_len = 0;batch_len <resp_msg.sensor_info[sns_len].sample_list_len;batch_len++)
    {
       resp_msg.sensor_info[sns_len].sample_list[batch_len].sensor_dimension_data_len = 
                       cm_sensor_data_resp->sample_batch[batch_len].dimension_data_len;
       
       for(dimension_cnt =0; 
           dimension_cnt < resp_msg.sensor_info[sns_len].sample_list[batch_len].sensor_dimension_data_len;
           dimension_cnt++)
       {
         resp_msg.sensor_info[sns_len].sample_list[batch_len].sensor_dimension_data[dimension_cnt] = 
                    cm_sensor_data_resp->sample_batch[batch_len].sns_dimension_data[dimension_cnt];
       }
       resp_msg.sensor_info[sns_len].sample_list[batch_len].timestamp =
                    cm_sensor_data_resp->sample_batch[batch_len].timestamp;
    }
#ifdef FEATURE_MMODE_TX_21

	resp_msg.algo_info[sns_len].event_type = cm_sensor_data_resp->event_type;
	resp_msg.algo_info[sns_len].high_perf_measurement_period = cm_sensor_data_resp->high_perf_measurement_period;
	resp_msg.algo_info[sns_len].sensor_state = cm_sensor_data_resp->sensors_state;
	resp_msg.algo_info[sns_len].slope = cm_sensor_data_resp->slope;
	resp_msg.algo_info[sns_len].ttc = cm_sensor_data_resp->TTC;
#endif
  }

  if(sensor_data_collection_rsp.fmd_status != CM_SNS_FMD_STATUS_NONE )
    {
      resp_msg.sensor_flight_status_valid = TRUE;
	  resp_msg.sensor_flight_status =(psm_sensor_flight_status_enum_v01) sensor_data_collection_rsp.fmd_status;
    }
  if(sensor_data_collection_rsp.tilt_angle > 0 )
	{ 
       resp_msg.sensor_tilt_angle_valid =TRUE;
	   resp_msg.sensor_tilt_angle = sensor_data_collection_rsp.tilt_angle;
	}
  // Send to all pending requests

  send_result:

  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error      = errval;
   QM_MSG_HIGH_2("qmi_psm_get_sensor_data result %d errval %d", resp_msg.resp.result, errval);
   
  while ((pend_req_addr = (qmi_psm_pending_cmd_buf_addr_s_type *)
                          q_get(&qmi_psmi_global.sns_get_data_cmd_buf_p)) != NULL)
  {
    if (qmi_psmi_validate_cmd_buf_p((qmi_mmode_qcsi_transaction_type *)pend_req_addr->cmd_buf_addr))
    {
      qmi_psmi_send_response((qmi_mmode_qcsi_transaction_type *)pend_req_addr->cmd_buf_addr,
                              &resp_msg, 
                              sizeof(psm_get_sensor_data_resp_msg_v01));
    }
    QMI_PSM_MEM_FREE(pend_req_addr);
  }   
}

/*===========================================================================
  FUNCTION qmi_psm_get_sensor_config_resp()

  DESCRIPTION
    Sends the  get config response t
    
  PARAMETERS
  cmd_buf_ptr :  Sensor cfg    cmd buf ptr

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_psm_get_sensor_config_resp
(
  cm_sns_registry_rsp_s_type sensor_config_data
)
{
  psm_get_sensor_configuration_resp_msg_v01              resp_msg;  
  uint8                                          dimension_cnt =0;
  //cm_sns_config_s_type                 *cm_sensor_config_data_resp;  
//  uint8                                          resp_cnt;
  uint8                                          sns_len =0;
  uint8                                          batch_len =0;  
  qmi_error_type_v01                             errval = QMI_ERR_NONE_V01;
  qmi_psm_pending_cmd_buf_addr_s_type           *pend_req_addr = NULL;
//  int i ;
/*----------------------------------------------------------------*/
  memset(&resp_msg,0,sizeof(psm_get_sensor_configuration_resp_msg_v01));
  
  QM_MSG_HIGH("qmi_psm_sensor_get_data_config_resp");
  

  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error      = errval;

 
   if(sensor_config_data.sensor_id == CM_SNS_TYPE_MCD)
    {

     resp_msg.mcd_motion_toggle_counter_threshold_valid = TRUE;
     resp_msg.mcd_motion_toggle_delay_threshold_valid = TRUE;
     resp_msg.mcd_psm_timer_config_valid = TRUE;
     resp_msg.mcd_motion_toggle_counter_threshold = sensor_config_data.registry_mcd_config_data.mcd_motion_toggle_counter_threshold;
     QM_MSG_HIGH_1("mcd_motion_toggle_counter_threshold = %d",sensor_config_data.registry_mcd_config_data.mcd_motion_toggle_counter_threshold);
     resp_msg.mcd_motion_toggle_delay_threshold = sensor_config_data.registry_mcd_config_data.mcd_motion_toggle_delay_threshold;
     QM_MSG_HIGH_1("mcd_motion_toggle_delay_threshold = %d",sensor_config_data.registry_mcd_config_data.mcd_motion_toggle_delay_threshold);
     resp_msg.mcd_psm_timer_config = sensor_config_data.registry_mcd_config_data.mcd_psm_timer_config;
   }
  
    if(sensor_config_data.sensor_id == CM_SNS_TYPE_MOTION_DETECT)
    {

      resp_msg.md_tuning_threshold_valid = TRUE;
      resp_msg.md_tuning_threshold  = sensor_config_data.registry_md_config_data.md_tuning_threshold;
    }

    if(sensor_config_data.sensor_id == CM_SNS_TYPE_SMD)
    {
      resp_msg.smd_perceptron_weights_valid = TRUE;
      resp_msg.smd_perceptron_weights_len =smd_perceptron_weight; 
      memscpy( resp_msg.smd_perceptron_weights, smd_perceptron_weight*sizeof(float), sensor_config_data.registry_smd_config_data.smd_weight , smd_perceptron_weight*sizeof(float) );
      resp_msg.smd_perceptron_min_features_valid = TRUE;
      resp_msg.smd_perceptron_min_features_len =smd_perceptron_min_feature_weight; 
      memscpy( resp_msg.smd_perceptron_min_features, smd_perceptron_min_feature_weight*sizeof(float), sensor_config_data.registry_smd_config_data.smd_perceptron_min_weight , smd_perceptron_min_feature_weight*sizeof(float) );
      resp_msg.smd_perceptron_max_features_valid = TRUE;
      resp_msg.smd_perceptron_max_features_len =smd_perceptron_max_feature_weight; 
      memscpy( resp_msg.smd_perceptron_max_features, smd_perceptron_max_feature_weight*sizeof(float), sensor_config_data.registry_smd_config_data.smd_perceptron_max_weight  , smd_perceptron_max_feature_weight*sizeof(float) );
      resp_msg.smd_sum_score_threshold_valid =TRUE;
      resp_msg.smd_sum_score_threshold = sensor_config_data.registry_smd_config_data.smd_sum_score_threshold;
    }

   if(sensor_config_data.sensor_id == CM_SNS_TYPE_FMD)
    {

     resp_msg.fmd_accel_thres_valid = TRUE;
     resp_msg.fmd_accel_thres= sensor_config_data.registry_fmd_config_data.accel_threshold;
     resp_msg.fmd_accel_buffer_count_valid =TRUE;
     resp_msg.fmd_accel_buffer_count = sensor_config_data.registry_fmd_config_data.accel_buffer_count;
     resp_msg.fmd_run_period_valid =TRUE;
     resp_msg.fmd_run_period =  sensor_config_data.registry_fmd_config_data.run_duration;

    }
   
    if(sensor_config_data.sensor_id == CM_SNS_TYPE_TILT)
    {
       resp_msg.init_gravity_threshold_valid =TRUE;
       resp_msg.init_gravity_threshold = sensor_config_data.registry_tilt_config_data.init_gravity_threshold;
       resp_msg.init_max_trial_num_valid =TRUE;
       resp_msg.init_max_trial_num = (uint8_t)sensor_config_data.registry_tilt_config_data.max_init_trial_num;
       resp_msg.iteration_delay_valid =TRUE;
       resp_msg.iteration_delay = sensor_config_data.registry_tilt_config_data.iteration_delay;
       resp_msg.update_gravity_threshold_valid =TRUE;
       resp_msg.update_gravity_threshold = sensor_config_data.registry_tilt_config_data.update_gravity_threshold;
       resp_msg.redo_gravity_init_valid = TRUE;
       resp_msg.redo_gravity_init = sensor_config_data.registry_tilt_config_data.redo_gravity_init;
       resp_msg.transportation_type_valid = TRUE;
       resp_msg.transportation_type = (sensors_tilt_tracker_transportation_type_v01)sensor_config_data.registry_tilt_config_data.transportation_type;
       resp_msg.update_max_trial_num_valid = TRUE;
       resp_msg.update_max_trial_num = sensor_config_data.registry_tilt_config_data.max_update_trial_num;
    }

  // Send to all pending requests
  while ((pend_req_addr = (qmi_psm_pending_cmd_buf_addr_s_type *)
                          q_get(&qmi_psmi_global.sns_get_config_cmd_buf_p)) != NULL)
  {
    if (qmi_psmi_validate_cmd_buf_p((qmi_mmode_qcsi_transaction_type *)pend_req_addr->cmd_buf_addr))
    {
      qmi_psmi_send_response((qmi_mmode_qcsi_transaction_type *)pend_req_addr->cmd_buf_addr,
                              &resp_msg, 
                              sizeof(psm_get_sensor_configuration_resp_msg_v01));
    }
    QMI_PSM_MEM_FREE(pend_req_addr);
  }   
}

/*===========================================================================
  FUNCTION qmi_psm_get_sensor_data_resp_err()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
     errval received from CM cmd callback

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_psm_get_sensor_data_resp_err
(
  qmi_error_type_v01                 errval
)
{
  psm_get_sensor_data_resp_msg_v01            resp_msg;
  qmi_psm_pending_cmd_buf_addr_s_type        *pend_req_addr = NULL;

  memset(&resp_msg,0,sizeof(psm_get_sensor_data_resp_msg_v01));
  
  resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error  = errval; 
  QM_MSG_HIGH_2("qmi_psm_get_sensor_data_resp_err %d errval %d",
          resp_msg.resp.result, errval);
          
  // Send to all pending requests
  while ((pend_req_addr = (qmi_psm_pending_cmd_buf_addr_s_type *)
                          q_get(&qmi_psmi_global.sns_get_data_cmd_buf_p)) != NULL)
  {
    if (qmi_psmi_validate_cmd_buf_p((qmi_mmode_qcsi_transaction_type *)pend_req_addr->cmd_buf_addr))
    {
      qmi_psmi_send_response((qmi_mmode_qcsi_transaction_type *)pend_req_addr->cmd_buf_addr,
                              &resp_msg, 
                              sizeof(psm_get_sensor_data_resp_msg_v01));
    }
    QMI_PSM_MEM_FREE(pend_req_addr);
  }   
}


/*===========================================================================
  FUNCTION qmi_psm_reset_sensor_config_data()

  DESCRIPTION
    Gets the sensor information from modem based on the sensor ID pass
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_reset_sensor_config_data
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_reset_sensor_config_data_req_msg_v01   *req_msg;
  psm_reset_sensor_config_data_resp_msg_v01   resp_msg;
  qmi_error_type_v01                          errval = QMI_ERR_NONE_V01;
  qmi_psm_pending_cmd_buf_addr_s_type        *pend_req_addr = NULL;
  /*--------------------------------------------------*/
  
  ASSERT( req_c_struct &&(req_c_struct_len > 0) );

  memset(&resp_msg,0,sizeof(psm_reset_sensor_config_data_resp_msg_v01));
  req_msg = (psm_reset_sensor_config_data_req_msg_v01 *)req_c_struct;  

  /* store the request to into pending requests queue */
  pend_req_addr = QMI_PSM_MEM_ALLOC( sizeof(qmi_psm_pending_cmd_buf_addr_s_type) );
  if(!pend_req_addr)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
  
  memset(pend_req_addr, 0, sizeof(qmi_psm_pending_cmd_buf_addr_s_type));
  (void) q_link ( pend_req_addr, &pend_req_addr->link );
  pend_req_addr->cmd_buf_addr = (void *)cmd_buf_p;

  // Send command to CM only if no pending request
  if (q_check(&qmi_psmi_global.sns_reset_config_data_cmd_buf_p) == NULL)
  {
    if(TRUE !=  cm_sns_cmd_reset_config(qmi_psm_sensor_cm_cmd_callback,
                                        qmi_psm_get_cm_client_id()
                                        ))
    {
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
  }

  // Queue and wait for CM response
  q_put( &(qmi_psmi_global.sns_reset_config_data_cmd_buf_p), &(pend_req_addr->link) );
  QM_MSG_HIGH("PSM reset_sensor_config_data queued");
  
 send_result:
  if (errval != QMI_ERR_NONE_V01)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error  = errval; 
    QM_MSG_HIGH_2("qmi_psm_reset_sensor_config_data result %d errval %d",
            resp_msg.resp.result, errval);
            
    qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                      sizeof(psm_reset_sensor_config_data_resp_msg_v01));
  
    if (pend_req_addr != NULL)
    {
      QMI_PSM_MEM_FREE(pend_req_addr);
    }
  }

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_psm_reset_sensor_config_data_resp()

  DESCRIPTION
    Reset the sensor policy information to modem for sensor policy
    
  PARAMETERS
     errval received from CM cmd callback

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_psm_reset_sensor_config_data_resp
(
  qmi_error_type_v01                 errval
)
{
  psm_reset_sensor_config_data_resp_msg_v01   resp_msg;
  qmi_psm_pending_cmd_buf_addr_s_type        *pend_req_addr = NULL;

  memset(&resp_msg,0,sizeof(psm_reset_sensor_config_data_resp_msg_v01));
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  QM_MSG_HIGH_2("qmi_psm_reset_sensor_config_data_resp %d errval %d",
          resp_msg.resp.result, errval);
          
  // Send to all pending requests
  while ((pend_req_addr = (qmi_psm_pending_cmd_buf_addr_s_type *)
                          q_get(&qmi_psmi_global.sns_reset_config_data_cmd_buf_p)) != NULL)
  {
    if (qmi_psmi_validate_cmd_buf_p((qmi_mmode_qcsi_transaction_type *)pend_req_addr->cmd_buf_addr))
    {
      qmi_psmi_send_response((qmi_mmode_qcsi_transaction_type *)pend_req_addr->cmd_buf_addr,
                              &resp_msg, 
                              sizeof(psm_reset_sensor_config_data_resp_msg_v01));
    }
    QMI_PSM_MEM_FREE(pend_req_addr);
  }   
}

/*===========================================================================
  FUNCTION qmi_psm_get_last_policy_met_info()

  DESCRIPTION
    Gets the last sensor information from modem if missed the sensor indication
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_get_last_policy_met_info
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_get_last_policy_met_info_req_msg_v01   *req_msg;
  psm_get_last_policy_met_info_resp_msg_v01   resp_msg;
  qmi_error_type_v01                          errval = QMI_ERR_NONE_V01;
  cm_sns_event_info_s_type                    event_info;
  /*--------------------------------------------------*/

  ASSERT( req_c_struct &&(req_c_struct_len > 0) );
  
  memset(&resp_msg,0,sizeof(psm_get_last_policy_met_info_resp_msg_v01));  
  req_msg = (psm_get_last_policy_met_info_req_msg_v01 *)req_c_struct;

  event_info = cm_sns_get_event_info();
  QM_MSG_HIGH_2("qmi_psm_get_last_policy_met_info result %d errval %d",
          event_info.policy_met_sns_mask, event_info.batch_full_sns_mask);
  
  resp_msg.policy_met_sensor_mask_valid = TRUE;
  resp_msg.policy_met_sensor_mask =  psm_convert_cm_to_qmi_sensor_mask(event_info.policy_met_sns_mask);
  resp_msg.batch_full_sensor_mask_valid =TRUE;
  resp_msg.batch_full_sensor_mask = psm_convert_cm_to_qmi_sensor_mask(event_info.batch_full_sns_mask);
  resp_msg.sensor_fail_cond_met_mask_valid =TRUE;
  resp_msg.sensor_fail_cond_met_mask = psm_convert_cm_to_qmi_sensor_mask(event_info.sns_fail_mask);
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  QM_MSG_HIGH_2("qmi_psm_get_last_policy_met_info result %d errval %d",
          resp_msg.resp.result, errval);
  
  qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                         sizeof(psm_get_last_policy_met_info_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_psm_sensor_policy_met_indication()

  DESCRIPTION
    Sends the indication about the policy met
    
  PARAMETERS
  cmd_buf_ptr :  Sensor cfg    cmd buf ptr

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_sensor_policy_met_indication
(
  cm_qmi_sensor_event_ind_s_type   sensor_event_ind
)
{
  psm_sensor_policy_met_ind_msg_v01           indication;
  qmi_psmi_client_state_type                  *cl_sp;  
  qmi_csi_error                               rc;

  uint32                                      batch_full_sns_mask =0;
  uint32                                      policy_sensor_mask =0;
  uint32                                      sns_fail_mask=0;
  
/*--------------------------------------------------*/
  memset(&indication,0,sizeof(psm_sensor_policy_met_ind_msg_v01));

  batch_full_sns_mask   = sensor_event_ind.sns_event_info.batch_full_sns_mask;
  policy_sensor_mask = sensor_event_ind.sns_event_info.policy_met_sns_mask;
  sns_fail_mask = sensor_event_ind.sns_event_info.sns_fail_mask;
  
  QM_MSG_HIGH_3("qmi_psm_sensor_policy_met_indication "
          "policy_sensor_mask %d batch_full_sns_mask %d sns_fail_mask %d",policy_sensor_mask,batch_full_sns_mask,sns_fail_mask);  
 
  indication.policy_met_sensor_mask =  psm_convert_cm_to_qmi_sensor_mask(policy_sensor_mask);

  if(indication.policy_met_sensor_mask)
      indication.policy_met_sensor_mask_valid = TRUE;

  indication.batch_full_sensor_mask = psm_convert_cm_to_qmi_sensor_mask(batch_full_sns_mask);

  if(indication.batch_full_sensor_mask)
     indication.batch_full_sensor_mask_valid =TRUE;      
  
  indication.sensor_fail_cond_met_mask =  psm_convert_cm_to_qmi_sensor_mask(sns_fail_mask);

  if(indication.sensor_fail_cond_met_mask)
      indication.sensor_fail_cond_met_mask_valid = TRUE;
  
  if((indication.policy_met_sensor_mask_valid)||(indication.batch_full_sensor_mask_valid) || (indication.sensor_fail_cond_met_mask))
  {
    // Send to all registered clients
    cl_sp = (qmi_psmi_client_state_type *)q_check(&qmi_psm_state.client_q);
    while (cl_sp != NULL)
    {
      if (cl_sp->psm_info.report_sensor_policy_met_ind)
      {
        rc = qmi_psmi_send_ind(cl_sp,
                              QMI_PSM_SENSOR_POLICY_MET_IND_V01,
                              &indication,
                              sizeof(indication));
        if (rc != QMI_CSI_NO_ERR)
        {
          QM_MSG_HIGH_1("Failed to report SENSOR_POLICY_MET_IND, error %d", rc);
        }
      }
      cl_sp = q_next(&qmi_psm_state.client_q, &(cl_sp->link));
    }
   }
   return QMI_CSI_CB_NO_ERR;
}
#ifdef FEATURE_MMODE_TX_21

/*===========================================================================
  FUNCTION qmi_psm_phy_sns_test_req()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_phy_sns_test_req
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_phy_sns_test_req_msg_v01   *req_msg;
  psm_phy_sns_test_resp_msg_v01   resp_msg;
  qmi_error_type_v01                          errval =  QMI_ERR_NONE_V01;
 
  ASSERT(req_c_struct && (req_c_struct_len > 0));

  memset(&resp_msg,0,sizeof(psm_phy_sns_test_resp_msg_v01));

  req_msg = (psm_phy_sns_test_req_msg_v01 *)req_c_struct;

  if(req_msg->physical_sns_test_valid != TRUE)
  {
    req_msg->physical_sns_test = PSM_SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY_V01;
  }
  
  if(TRUE !=  cm_sns_phy_sns_test_req(qmi_psm_sensor_cm_cmd_callback,
  	                                  qmi_psm_get_cm_client_id(), 
  	                                  req_msg->sensor_id, 
  	                                  req_msg->physical_sns_test)) 
  {
    errval = QMI_ERR_INTERNAL_V01;
	goto send_result;
  }
									  
  QM_MSG_HIGH("qmi_psm_phy_sns_test_req queued");
  qmi_psmi_global.sns_phy_sns_test = cmd_buf_p;

  send_result:
	if (errval != QMI_ERR_NONE_V01)
	{
	  resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
	  resp_msg.resp.error  = errval; 
	  QM_MSG_HIGH_2("qmi_psm_phy_sns_test_req result %d errval %d", resp_msg.resp.result, errval);
			  
	  qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
						sizeof(psm_phy_sns_test_resp_msg_v01));
	}
	
  return QMI_CSI_CB_NO_ERR;
  
}
/*===========================================================================
  FUNCTION qmi_psm_phy_sns_test_resp()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void qmi_psm_phy_sns_test_resp(
  qmi_error_type_v01                 errval
)
{
  psm_phy_sns_test_resp_msg_v01   resp_msg;
  qmi_mmode_qcsi_transaction_type            *cmd_buf_p = qmi_psmi_global.sns_phy_sns_test;
 
  qmi_psmi_global.sns_phy_sns_test = NULL;

  if(cmd_buf_p == NULL || qmi_psmi_validate_cmd_buf_p(cmd_buf_p) == FALSE)
  {
    QM_MSG_HIGH("No pending request/client got deallocated");
  }
  memset(&resp_msg,0,sizeof(psm_phy_sns_test_resp_msg_v01));

  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01: QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  
  QM_MSG_HIGH_2("qmi_psm_phy_sns_test_resp result %d errval %d",
          resp_msg.resp.result, errval);
  
  qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                    sizeof(psm_phy_sns_test_resp_msg_v01));
}

/*===========================================================================
  FUNCTION qmi_psm_phy_sns_test_ind()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_phy_sns_test_ind
(
  cm_sns_qmi_sns_phy_test_rsp_s_type res
)
{
  psm_phy_sns_test_ind_msg_v01   ind_msg;
  qmi_psmi_client_state_type *cl_sp;
  qmi_csi_error                rc;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&ind_msg, 0, sizeof(ind_msg));

  QM_MSG_HIGH("qmi_psm_phy_sns_test_ind");

  ind_msg.test_passed_valid = TRUE;
  ind_msg.test_passed = res.test_passed;
  ind_msg.physical_sns_test_valid = TRUE;
  ind_msg.physical_sns_test = (phy_sns_test_type_v01)res.physical_sns_test;
 
  
  // Send to all registered clients
  cl_sp = (qmi_psmi_client_state_type *)q_check(&qmi_psm_state.client_q);
  while (cl_sp != NULL)
  {
    if (cl_sp->psm_info.report_sns_phy_test_ind)
    {
      rc = qmi_psmi_send_ind(cl_sp,
                            QMI_PSM_PHY_SNS_TEST_IND_V01,
                            &ind_msg,
                            sizeof(ind_msg));
      if (rc != QMI_CSI_NO_ERR)
      {
        QM_MSG_HIGH_1("Failed to test_ind, error %d", rc);
      }
    }
    cl_sp = q_next(&qmi_psm_state.client_q, &(cl_sp->link));
  }
   return QMI_CSI_CB_NO_ERR;
}




/*===========================================================================
  FUNCTION qmi_psm_phy_sns_test_get_data_req()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_phy_sns_test_get_data_req
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_phy_sns_test_get_data_req_msg_v01   *req_msg;
  psm_phy_sns_test_get_data_resp_msg_v01   resp_msg;
  qmi_error_type_v01                          errval =  QMI_ERR_NONE_V01;
  
  ASSERT(req_c_struct && (req_c_struct_len > 0));


  memset(&resp_msg,0,sizeof(psm_phy_sns_test_get_data_resp_msg_v01));

  req_msg = (psm_phy_sns_test_get_data_req_msg_v01 *)req_c_struct;
   
  if(req_msg->report_rate_valid != TRUE)
  {
	  req_msg->report_rate = 1;
  }
   
  if(req_msg->sample_rate_valid != TRUE)
  {
	  req_msg->sample_rate = 10;
  }

  if(TRUE != cm_sns_phy_sns_test_get_data_req(qmi_psm_sensor_cm_cmd_callback,
  	                                           qmi_psm_get_cm_client_id(),
  	                                           req_msg->sensor_id, 
  	                                           req_msg->is_enabled,
  	                                           req_msg->sample_rate,
  	                                           req_msg->report_rate))
  {
    errval = QMI_ERR_INTERNAL_V01;
	goto send_result;
  }
											   
  QM_MSG_HIGH("qmi_psm_phy_sns_test_get_data_req queued");
  qmi_psmi_global.sns_phy_sns_test_get_data = cmd_buf_p;

  send_result:
	if (errval != QMI_ERR_NONE_V01)
	{
	  resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
	  resp_msg.resp.error  = errval; 
	  QM_MSG_HIGH_2("qmi_psm_phy_sns_test_get_data_req result %d errval %d", resp_msg.resp.result, errval);
			  
	  qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
						sizeof(psm_phy_sns_test_get_data_resp_msg_v01));
	}
	
  return QMI_CSI_CB_NO_ERR;
  
}

/*===========================================================================
  FUNCTION qmi_psm_phy_sns_test_get_data_resp()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_psm_phy_sns_test_get_data_resp(
  qmi_error_type_v01                 errval
)
{
  psm_phy_sns_test_get_data_resp_msg_v01   resp_msg;
  qmi_mmode_qcsi_transaction_type            *cmd_buf_p = qmi_psmi_global.sns_phy_sns_test_get_data;
 
  qmi_psmi_global.sns_phy_sns_test_get_data = NULL;

  if(cmd_buf_p == NULL || qmi_psmi_validate_cmd_buf_p(cmd_buf_p) == FALSE)
  {
    QM_MSG_HIGH("No pending request/client got deallocated");
  }
  memset(&resp_msg,0,sizeof(psm_phy_sns_test_get_data_resp_msg_v01));

  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  
  QM_MSG_HIGH_2("qmi_psm_phy_sns_test_get_data_resp result %d errval %d",
          resp_msg.resp.result, errval);
  
  qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                    sizeof(psm_phy_sns_test_get_data_resp_msg_v01));
}


/*===========================================================================
  FUNCTION qmi_psm_phy_sns_test_get_data_ind()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_phy_sns_test_get_data_ind
(
  cm_sns_qmi_sns_phy_test_get_data_rsp_s_type sample_buffer
)
{
  psm_phy_sns_test_get_data_ind_msg_v01   ind_msg;
  qmi_psmi_client_state_type *cl_sp;
  qmi_csi_error                rc;
  int i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&ind_msg, 0, sizeof(ind_msg));

  QM_MSG_HIGH("qmi_psm_phy_sns_test_get_data_ind");
  
  ind_msg.sensor_id =(psm_sensor_type_id_v01) sample_buffer.sensor_id;
  ind_msg.sensor_data_rsp_valid = TRUE;
  ind_msg.sensor_data_rsp_len = sample_buffer.sample.dimension_data_len;
  for(i=0;i<sample_buffer.sample.dimension_data_len;i++)
  {
    ind_msg.sensor_data_rsp[i] = sample_buffer.sample.sns_dimension_data[i];	
  }
  ind_msg.timestamp_valid = TRUE;
  ind_msg.timestamp = sample_buffer.sample.timestamp;
  
  // Send to all registered clients
  cl_sp = (qmi_psmi_client_state_type *)q_check(&qmi_psm_state.client_q);
  while (cl_sp != NULL)
  {
    if (cl_sp->psm_info.report_sns_phy_test_get_data_ind)
    {
      rc = qmi_psmi_send_ind(cl_sp,
                            QMI_PSM_PHY_SNS_TEST_GET_DATA_IND_V01,
                            &ind_msg,
                            sizeof(ind_msg));
      if (rc != QMI_CSI_NO_ERR)
      {
        QM_MSG_HIGH_1("Failed to test_ind, error %d", rc);
      }
    }
    cl_sp = q_next(&qmi_psm_state.client_q, &(cl_sp->link));
  }
   return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION qmi_psm_get_sns_serial_num_req()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_psm_get_sns_serial_num_req
(
  qmi_mmode_svc_info_type         *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void                            *cl_sp,
  void*                            req_c_struct,
  unsigned int                     req_c_struct_len
)
{
  psm_get_sns_serial_number_req_msg_v01   *req_msg;
  psm_get_sns_serial_number_resp_msg_v01   resp_msg;
  qmi_error_type_v01                          errval =  QMI_ERR_NONE_V01;
 
  ASSERT(req_c_struct && (req_c_struct_len > 0));

  memset(&resp_msg,0,sizeof(psm_get_sns_serial_number_resp_msg_v01));

  req_msg = (psm_get_sns_serial_number_req_msg_v01 *)req_c_struct;
  
  if(TRUE !=  cm_sns_serial_number_req(NULL,
  	                                  qmi_psm_get_cm_client_id(), 
  	                                  req_msg->sensor_id)) 
  {
    errval = QMI_ERR_INTERNAL_V01;
	goto send_result;
  }
									  
  QM_MSG_HIGH("qmi_psm_get_sns_serial_num_req queued");
  qmi_psmi_global.sns_get_serial_number_cmd_buf_p = cmd_buf_p;

  send_result:
	if (errval != QMI_ERR_NONE_V01)
	{
	  resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
	  resp_msg.resp.error  = errval; 
	  QM_MSG_HIGH_2("qmi_psm_phy_sns_test_req result %d errval %d", resp_msg.resp.result, errval);
			  
	  qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
						sizeof(psm_get_sns_serial_number_resp_msg_v01));
	}
	
  return QMI_CSI_CB_NO_ERR;
  
}
/*===========================================================================
  FUNCTION qmi_psm_get_sns_serial_num_resp()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
    sp                        :       PSM service handle
    cmd_buf_p           :       Command info (req_handle, msg_id, etc..)
    cl_sp                    :       Client Info Handle
    req_c_struct         :       Request Message with all TLV
    req_c_struct_len   :       length of request message

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void qmi_psm_get_sns_serial_num_resp(
  cm_sns_qmi_attribute_rsp_s_type sensor_attribute_rsp
)
{
  psm_get_sns_serial_number_resp_msg_v01   resp_msg;
  qmi_mmode_qcsi_transaction_type            *cmd_buf_p = qmi_psmi_global.sns_get_serial_number_cmd_buf_p;
 qmi_error_type_v01                          errval =  QMI_ERR_NONE_V01;
 
  qmi_psmi_global.sns_get_serial_number_cmd_buf_p = NULL;

  if(cmd_buf_p == NULL || qmi_psmi_validate_cmd_buf_p(cmd_buf_p) == FALSE)
  {
    QM_MSG_HIGH("No pending request/client got deallocated");
	errval = QMI_ERR_NO_MEMORY_V01;
  }
  memset(&resp_msg,0,sizeof(psm_get_sns_serial_number_resp_msg_v01));

  resp_msg.serial_number_valid = TRUE;
  resp_msg.serial_number_len = sensor_attribute_rsp.sn_len;
  memscpy(resp_msg.serial_number,sensor_attribute_rsp.sn_len,sensor_attribute_rsp.cm_sns_temperature,sensor_attribute_rsp.sn_len);

  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01: QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  
  QM_MSG_HIGH_2("qmi_psm_get_sns_serial_num_resp result %d errval %d",
          resp_msg.resp.result, errval);
  
  qmi_psmi_send_response(cmd_buf_p, &resp_msg, 
                    sizeof(psm_get_sns_serial_number_resp_msg_v01));
}

#endif
#endif

