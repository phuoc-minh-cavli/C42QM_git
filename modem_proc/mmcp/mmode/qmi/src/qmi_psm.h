#ifndef _QMI_PSM_H
#define _QMI_PSM_H
/*===========================================================================

                         QMI_PSM.H

DESCRIPTION

 QMI_NAS header file for internal definitions

EXTERNALIZED FUNCTIONS

Copyright (c) 2004-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/mmcp.mpss/6.1.5/mmode/qmi/src/qmi_psm.h

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/22/20    rdprasad     Created file
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#include "qm_comdef.h"
#include "queue.h"     /* Interface to queue services */

#include "qmi_mmode_svc.h"
#include "qmi_csi.h"
#include "qmi_mmode_task_cmd.h"
#include "qmi_idl_lib.h"
#include "power_save_mode_service_v01.h"
#include "power_save_mode_service_impl_v01.h"


/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/

#define PSMI_MAX_CLIDS   (32)


#define QMI_PSM_MEM_ALLOC(siz)  modem_mem_alloc(siz, MODEM_MEM_CLIENT_QMI_MMODE)
#define QMI_PSM_MEM_FREE(ptr)  modem_mem_free(ptr, MODEM_MEM_CLIENT_QMI_MMODE)

typedef enum
{
  QMI_CMD_MIN = 0,
    
  QMI_PSM_CMD_STATUS_IND,
  QMI_PSM_CMD_SNS_SET_CONFIG,
  QMI_PSM_CMD_SNS_DATA_RESP,
  QMI_PSM_CMD_SNS_RESET_CONFIG,
  QMI_PSM_CMD_SNS_OD_SNS_RESP,
  QMI_PSM_CMD_SNS_PHY_TEST,
  QMI_PSM_CMD_SNS_PHY_TEST_GET_DATA,
  QMI_PSM_CMD_SNS_GET_SERIAL_NUM,
  #ifdef FEATURE_MMODE_TX_21
  QMI_PSM_CMD_SNS_SET_LOGGING_LEVEL,
  QMI_PSM_CMD_SNS_GET_CONFIG_RESP,
  #endif
  QMI_CMD_MAX
} qmi_psm_cmd_e_type;

/*---------------------------------------------------------------------------
  Enum values for the PSM state of client
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_PSM_DEFAULT_STATE     = 0,
  QMI_PSM_DISABLE_STATE     = 1,
  QMI_PSM_ENABLE_STATE      = 2
} qmi_psm_state_e_type;

/*===========================================================================
TYPEDEF qmi_psm_pending_cmd_buf_addr_s_type

DESCRIPTION
  Stores the address of pending cmd buffer
===========================================================================*/

typedef struct
{
  q_link_type                    link; /*< Data Structure required by the queue 
                                                              library to add this as part of a queue */
  void                          *cmd_buf_addr;
  
} qmi_psm_pending_cmd_buf_addr_s_type;

/*---------------------------------------------------------------------------
  QMI Power Save mode global context
---------------------------------------------------------------------------*/
typedef struct qmi_psmi_global_s
{

  boolean  inited;

  cm_client_id_type                    cm_client_id;
  psm_version_capability_v01           psm_capability;
  q_type                               get_cfg_params_req_buf_p;
  #ifdef FEATURE_REF_TRACKER
  qmi_mmode_qcsi_transaction_type     *sns_set_config_cmd_buf_p; 
  #ifdef FEATURE_MMODE_TX_21
  qmi_mmode_qcsi_transaction_type     *sns_set_config_ext_cmd_buf_p;
  qmi_mmode_qcsi_transaction_type     *sns_get_fmd_sns_sample_cmd_buf_p;
  qmi_mmode_qcsi_transaction_type     *sns_phy_sns_test;
  qmi_mmode_qcsi_transaction_type     *sns_phy_sns_test_get_data;
  qmi_mmode_qcsi_transaction_type     *sns_set_logging_level_cmd_buf_p; 
  qmi_mmode_qcsi_transaction_type     *sns_get_serial_number_cmd_buf_p;
  #endif
  q_type                               sns_get_data_cmd_buf_p;
  q_type                               sns_reset_config_data_cmd_buf_p;
  q_type                               sns_get_config_cmd_buf_p;
  #endif
} qmi_psmi_global_s_type;

/*---------------------------------------------------------------------------
  QMI Power Save mode instance state definition &
 Power Save mode client state definition
---------------------------------------------------------------------------*/
typedef struct qmi_psmi_client_state_s
{

  q_link_type       link; /*< Data Structure required by the queue 
                              library to add this as part of a queue */
                                
  qmi_client_handle            clnt;   /* QCSI Client Handle */

  qmi_psm_state_e_type              current_psm_state;
  
  struct psm_info_s
  {
    boolean  report_psm_status;
    boolean  report_psm_cfg_change;    
    boolean  report_sensor_policy_met_ind;
	boolean  report_sns_phy_test_ind;
    boolean  report_sns_phy_test_get_data_ind;
  } psm_info; 
  
} qmi_psmi_client_state_type;

/*---------------------------------------------------------------------------
  QMI Power Save mode cmd buffer
---------------------------------------------------------------------------*/

typedef struct
{
  cm_psm_status_e_type         status;
  cm_psm_reject_reason_e_type  reject_reason;
  uint32                       periodic_update_timer;
} psm_status_s_type;

typedef struct
{
  qmi_psm_cmd_e_type cmd_id;

  union
  {
    psm_status_s_type psm_status;    
#ifdef FEATURE_REF_TRACKER
    qmi_error_type_v01 sensor_cm_cmd_cb_err;
#endif

  } data;
} qmi_psmi_cmd_buf_type;


//extern qmi_psmi_global_s_type qmi_psmi_global;
//extern qmi_mmode_svc_info_type qmi_psm_state;

/*===========================================================================
  FUNCTION qmi_psm_init()

  DESCRIPTION
    Register the Power save mode Service with QCSI for all applicable QMI links
===========================================================================*/
void qmi_psm_init( void );

/*========================================================================
  FUNCTION qmi_psm_msgr_register

  DESCRIPTION
    register to msgr messages

  PARAMETERS
    msgr client object pointer

  RETURN VALUE
    None
===========================================================================*/
void qmi_psm_msgr_register( msgr_client_t *msgr_client );

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
void qmi_psm_msgr_handler( void *param );

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
);

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
);

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
);

#ifdef FEATURE_REF_TRACKER
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
);

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
);

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
);

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
);

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
);

#ifdef FEATURE_MMODE_TX_21
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
);

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
);

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
);


#endif

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
  FUNCTION qmi_psm_get_sensor_config_resp()

  DESCRIPTION
   GEt config for registry
    
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
void qmi_psm_get_sensor_config_resp(
  cm_sns_registry_rsp_s_type                 sensor_config_data
);


#endif
#endif // !_QMI_PSM_H
