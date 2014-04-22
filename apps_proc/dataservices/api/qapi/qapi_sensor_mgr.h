/*====================================================================== 
 Copyright (c) 2019-2021  Qualcomm Technologies, Inc. and/or its subsidiaries. 
 All rights reserved.
 Confidential - Qualcomm Technologies, Inc. - May Contain Trade Secrets
 *
 *        EDIT HISTORY FOR FILE
 *
 *	 This section contains comments describing changes made to the
 *	 module. Notice that changes are listed in reverse chronological
 *	 order.
 *
 ======================================================================*/

/**
  @file qapi_sensor_mgr.h

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none
*/

#ifndef _QAPI_SENSOR_MGR_H
#define _QAPI_SENSOR_MGR_H

#include "qapi_data_txm_base.h"
#include "qapi_status.h"
#include "qapi_driver_access.h"
#include "txm_module.h"

#define  TXM_QAPI_SENSOR_MGR_INIT                 TXM_QAPI_SENSOR_MGR_BASE + 1
#define  TXM_QAPI_SENSOR_MGR_SET_CB               TXM_QAPI_SENSOR_MGR_BASE + 2
#define  TXM_QAPI_SENSOR_MGR_SET_SENSOR_CONFIG    TXM_QAPI_SENSOR_MGR_BASE + 3
#define  TXM_QAPI_SENSOR_MGR_QUERY_SENSOR_IND     TXM_QAPI_SENSOR_MGR_BASE + 4
#define  TXM_QAPI_SENSOR_MGR_GET_SENSOR_DATA      TXM_QAPI_SENSOR_MGR_BASE + 5
#define  TXM_QAPI_SENSOR_MGR_RESET_SENSOR_CFG     TXM_QAPI_SENSOR_MGR_BASE + 6
#define  TXM_QAPI_SENSOR_MGR_RELEASE              TXM_QAPI_SENSOR_MGR_BASE + 7
#define  TXM_QAPI_SENSOR_MGR_SET_CB_V2            TXM_QAPI_SENSOR_MGR_BASE + 8
#define  TXM_QAPI_SENSOR_MGR_QUERY_SENSOR_IND_V2  TXM_QAPI_SENSOR_MGR_BASE + 9

/** @addtogroup chapter_sensor_mgr
 *  @{ */

/** Maximum dimension of a sample of sensor data. */
#define MAX_SENSOR_SAMPLE_DIM_SZ       3

/** Maximum batch size of samples of sensor data. */
#define MAX_SENSOR_SAMPLE_BATCH_SZ     20

/** Maximum number of sensors. */
#define MAX_SENSOR_CNT                 10

/** Maximum number of sensor manager instances. */
#define SENSOR_MGR_MAX_INSTANCES 5

/** Sensor IDs. */
typedef enum
{
	QAPI_SENSOR_MGR_SENSOR_ID_MIN = -2147483647,
	QAPI_SENSOR_MGR_SENSOR_ID_ALS = 1,         /**< Sensor ID for ALS sensor. */
	QAPI_SENSOR_MGR_SENSOR_ID_PRESSURE = 2,    /**< Sensor ID for PRESSURE sensor. */
	QAPI_SENSOR_MGR_SENSOR_ID_HUMIDITY = 3,    /**< Sensor ID for HUMIDITY sensor. */
	QAPI_SENSOR_MGR_SENSOR_ID_TEMPERATURE = 4, /**< Sensor ID for TEMPERATURE sensor. */
	QAPI_SENSOR_MGR_SENSOR_ID_AMD = 5, /**< Sensor ID for AMD sensor. */
	QAPI_SENSOR_MGR_SENSOR_ID_SMD = 6, /**< Sensor ID for SMD sensor. @newpage */
	QAPI_SENSOR_MGR_SENSOR_ID_MAX = 2147483647,
}qapi_Sensor_Mgr_Sensor_Id_t;

/** Sensor IDs masks. */
typedef uint16_t qapi_Sensor_Mgr_Sensor_Id_Mask_t;

#define QAPI_SENSOR_MGR_SENSOR_ID_ALS_MASK          (qapi_Sensor_Mgr_Sensor_Id_Mask_t)0x0001  /**< Sensor ID mask for ALS sensor. */
#define QAPI_SENSOR_MGR_SENSOR_ID_PRESSURE_MASK     (qapi_Sensor_Mgr_Sensor_Id_Mask_t)0x0002  /**< Sensor ID mask for PRESSURE sensor. */
#define QAPI_SENSOR_MGR_SENSOR_ID_HUMIDITY_MASK     (qapi_Sensor_Mgr_Sensor_Id_Mask_t)0x0004  /**< Sensor ID mask for HUMIDITY sensor. */
#define QAPI_SENSOR_MGR_SENSOR_ID_TEMPERATURE_MASK  (qapi_Sensor_Mgr_Sensor_Id_Mask_t)0x0008  /**< Sensor ID mask for TEMPERATURE sensor. */
#define QAPI_SENSOR_MGR_SENSOR_ID_AMD_MASK  (qapi_Sensor_Mgr_Sensor_Id_Mask_t)0x0010  /**< Sensor ID mask for AMD sensor. */
#define QAPI_SENSOR_MGR_SENSOR_ID_SMD_MASK  (qapi_Sensor_Mgr_Sensor_Id_Mask_t)0x0020  /**< Sensor ID mask for SMD sensor. */

/** Sensor Policy condition. */
typedef struct qapi_Sensor_Mgr_Sensor_Policy_Cond_s
{
	float gt;   /**< Sensor value greater than equals. */
	float lt;   /**< Sensor value less than equals. */
	float step; /**< Sensor value changed from previous value. @newpagetable */
}qapi_Sensor_Mgr_Sensor_Policy_Cond_t;

/** Sensor Config Request message. */
typedef struct qapi_Sensor_Mgr_Sensor_Config_Req_Msg_s
{
	qapi_Sensor_Mgr_Sensor_Id_t sensor_id;  /**< Sensor ID. */
	uint32_t sensor_policy_dim;  /**< Sensor dimension. */
	qapi_Sensor_Mgr_Sensor_Policy_Cond_t policy_cond[MAX_SENSOR_SAMPLE_DIM_SZ];  /**< Sensor policy condition for each dimension. */
	uint8_t sensor_batch_cnt;  /**< Sensor batch count. */
	uint32_t sensor_report_intrvl;  /**< Sensor reporting interval. */
}qapi_Sensor_Mgr_Sensor_Config_Req_Msg_t;

/** Sensor sample data. */
typedef struct qapi_Sensor_Mgr_Sensor_Sample_Data_s
{
	uint32_t sample_dim;  /**< Sensor dimension. */
	double sample[MAX_SENSOR_SAMPLE_DIM_SZ];  /**< Sensor sample for each dimension. */
	uint64_t timestamp;  /**< Timestamp when sample collected. */
}qapi_Sensor_Mgr_Sensor_Sample_Data_t;

/** Sensor sample data batch. */
typedef struct qapi_Sensor_Mgr_Sensor_Sample_Data_Batch_s
{
	qapi_Sensor_Mgr_Sensor_Id_t sensor_id; /**< Sensor ID. */ 
	uint32_t sample_batch_len;  /**< Sensor sample batch length. */
	qapi_Sensor_Mgr_Sensor_Sample_Data_t sample_batch[MAX_SENSOR_SAMPLE_BATCH_SZ];  /**< Sensor sample data batch. @newpagetable */
}qapi_Sensor_Mgr_Sensor_Sample_Data_Batch_t;

/** Sensor data response message. */
typedef struct qapi_Sensor_Mgr_Sensor_Data_Resp_Msg_s
{
	uint8_t sensor_info_valid;  /**< Sensor info valid. */
	uint32_t sensor_info_len;  /**< Number of sensors whose data is contained in response. */
	qapi_Sensor_Mgr_Sensor_Sample_Data_Batch_t sensor_data[MAX_SENSOR_CNT];  /**< Sensor data for each of sensors contained in response. */
}qapi_Sensor_Mgr_Sensor_Data_Resp_Msg_t;

/** Sensor data request message. */
typedef struct qapi_Sensor_Mgr_Sensor_Data_Req_Msg_s
{
	qapi_Sensor_Mgr_Sensor_Id_Mask_t sensor_id_mask;  /**< Sensor ID mask. */
}qapi_Sensor_Mgr_Sensor_Data_Req_Msg_t;

/** Sensor data indication message. */
typedef struct qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_s
{
	uint8_t policy_met_sensor_mask_valid;  /**< Policy met mask valid. */
	qapi_Sensor_Mgr_Sensor_Id_Mask_t policy_met_sensor_mask;  /**< Policy met Sensor ID mask. */
	uint8_t batch_full_sensor_mask_valid;  /**< Batch full mask valid. */
	qapi_Sensor_Mgr_Sensor_Id_Mask_t batch_full_sensor_mask;  /**< Batch full Sensor ID mask. @newpagetable */
}qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_t;

/** Sensor data indication message version 2. */
typedef struct qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_v2_s
{
	uint8_t policy_met_sensor_mask_valid;  /**< Policy met mask valid. */
	qapi_Sensor_Mgr_Sensor_Id_Mask_t policy_met_sensor_mask;  /**< Policy met Sensor ID mask. */
	uint8_t batch_full_sensor_mask_valid;  /**< Batch full mask valid. */
	qapi_Sensor_Mgr_Sensor_Id_Mask_t batch_full_sensor_mask;  /**< Batch full Sensor ID mask. */
	uint8_t sensor_fail_cond_met_mask_valid;  /**< Bad state Sensor mask valid. */
	qapi_Sensor_Mgr_Sensor_Id_Mask_t sensor_fail_cond_met_mask;  /**< Bad state Sensor ID mask. */

}qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_v2_t;


/** Sensor manager client handle. */
typedef uint32_t qapi_Sensor_Mgr_Hndl_t;

/** Sensor manager indication callback to be set by client. */
typedef void (*qapi_Sensor_Mgr_Sensor_Ind_Callback_t)(qapi_Sensor_Mgr_Hndl_t sensor_mgr_hndl, const qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_t *ind_msg);

/** Sensor manager indication callback v2 to be set by client. */
typedef void (*qapi_Sensor_Mgr_Sensor_Ind_Callback_v2_t)(qapi_Sensor_Mgr_Hndl_t sensor_mgr_hndl, const qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_v2_t *ind_msg);


static __inline int sensor_mgr_set_byte_pool(qapi_Sensor_Mgr_Hndl_t hndl, void *sensor_mgr_byte_pool_ptr);
static __inline int sensor_mgr_release_indirection(qapi_Sensor_Mgr_Hndl_t hndl);


static __inline void qapi_sensor_mgr_cb_uspace_dispatcher(UINT cb_id,
                                                             void *app_cb,
                                                             UINT cb_param1,
                                                             UINT cb_param2,
                                                             UINT cb_param3,
                                                             UINT cb_param4,
                                                             UINT cb_param5,
                                                             UINT cb_param6,
                                                             UINT cb_param7,
                                                             UINT cb_param8)
{
  void (*pfn_app_cb1) (qapi_Sensor_Mgr_Hndl_t , qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_t *);
  void (*pfn_app_cb2) (qapi_Sensor_Mgr_Hndl_t , qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_v2_t *);
  qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_t *ind_msg = NULL;
  qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_v2_t *ind_msg_v2 = NULL;
  if(cb_id == DATA_CUSTOM_CB_SENSOR_MGR_IND)
  {
    pfn_app_cb1 = (void (*)(qapi_Sensor_Mgr_Hndl_t, qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_t *))app_cb;
	ind_msg = (qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_t *)cb_param2;
   (pfn_app_cb1)((qapi_Sensor_Mgr_Hndl_t)cb_param1, 
                       (qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_t *)cb_param2);
   if(ind_msg)
   	 tx_byte_release(ind_msg);
  }	

  if(cb_id == DATA_CUSTOM_CB_SENSOR_MGR_IND_V2)
  {
    pfn_app_cb2 = (void (*)(qapi_Sensor_Mgr_Hndl_t, qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_v2_t *))app_cb;
	ind_msg_v2 = (qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_v2_t *)cb_param2;
   (pfn_app_cb2)((qapi_Sensor_Mgr_Hndl_t)cb_param1, 
                       (qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_v2_t *)cb_param2);
   if(ind_msg_v2)
   	 tx_byte_release(ind_msg_v2);
  }
}

/** @} */

#ifdef  QAPI_TXM_MODULE

#define qapi_Sensor_Mgr_Init(a)               ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_SENSOR_MGR_INIT,    (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Sensor_Mgr_Set_Callback(a, b) ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_SENSOR_MGR_SET_CB,  (ULONG) a, (ULONG) b, (ULONG) qapi_sensor_mgr_cb_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Sensor_Mgr_Set_Sensor_Config(a, b, c)          ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_SENSOR_MGR_SET_SENSOR_CONFIG,     (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Sensor_Mgr_Query_Sensor_Ind(a, b)          ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_SENSOR_MGR_QUERY_SENSOR_IND,     (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Sensor_Mgr_Get_Sensor_Data(a, b, c)          ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_SENSOR_MGR_GET_SENSOR_DATA,     (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Sensor_Mgr_Reset_Sensor_Config(a)               ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_SENSOR_MGR_RESET_SENSOR_CFG,    (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Sensor_Mgr_Release(a)            sensor_mgr_release_indirection(a)
#define qapi_Sensor_Mgr_Set_Callback_v2(a, b) ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_SENSOR_MGR_SET_CB_V2,  (ULONG) a, (ULONG) b, (ULONG) qapi_sensor_mgr_cb_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Sensor_Mgr_Query_Sensor_Ind_v2(a, b)          ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_SENSOR_MGR_QUERY_SENSOR_IND_V2,     (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))


/**
 * @brief Macro that Passes Byte Pool Pointer for Application using sensor mgr QAPI's.
 * @param[in] 'a' : Handle
 * @param[in] 'b' : Pointer to Byte Pool 
 * @return QAPI_OK is returned on success. On error, <0 is returned.
 * (This Macro is only used in DAM Space)
 */
#define qapi_Sensor_Mgr_Pass_Pool_Ptr(a,b)             sensor_mgr_set_byte_pool(a,b)

static __inline int sensor_mgr_set_byte_pool(qapi_Sensor_Mgr_Hndl_t hndl, void *sensor_mgr_byte_pool_ptr)
{
  qapi_cb_params_uspace_ptr_t *uspace_memory_sensor_mgr = NULL;
  int ret = QAPI_ERROR;

  /* input parameter validation*/
  if((hndl == NULL) || sensor_mgr_byte_pool_ptr == NULL)
  {
	return QAPI_ERR_INVALID_PARAM;
  }
  

  /* Retrieve the user space information stored internally */
  qapi_data_map_handle_to_u_addr(hndl, QAPI_APP_SENSOR_MGR, (void**)&uspace_memory_sensor_mgr);
  /* Handle to user space information already exists */
  if (uspace_memory_sensor_mgr)
  {
    return QAPI_OK;
  }

  tx_byte_allocate(sensor_mgr_byte_pool_ptr, (VOID **) &uspace_memory_sensor_mgr,(sizeof(qapi_cb_params_uspace_ptr_t)), TX_NO_WAIT);
  if (NULL == uspace_memory_sensor_mgr)
  {
    return QAPI_ERR_NO_MEMORY;
  }

  memset (uspace_memory_sensor_mgr,0, sizeof(qapi_cb_params_uspace_ptr_t));

  
  /* Initialize user space information */
  uspace_memory_sensor_mgr->usr_pool_ptr = sensor_mgr_byte_pool_ptr;  
  
  /* Map the user space information to the handle internally and store */ 
  ret = qapi_data_map_u_addr_to_handle((void *)(hndl), QAPI_APP_SENSOR_MGR, uspace_memory_sensor_mgr, 0x00);
  if (ret != QAPI_OK)
  {
    /* Release the allocated resources */
    if (uspace_memory_sensor_mgr)
      tx_byte_release(uspace_memory_sensor_mgr);
  }
  
  return ret;
  
}

/*
 * Releases the user space allocated resources and dispatches the call to qapi_Sensor_Mgr_Release 
 * Parameter 'a' : Handle
 * On success, #QAPI_OK is returned. On error, appropriate QAPI_ERR_ code(qapi_status.h) is returned.
 * (This is only used in DAM Space)
 */
static __inline int sensor_mgr_release_indirection(qapi_Sensor_Mgr_Hndl_t hndl)
{
  int ret = QAPI_ERROR;
  qapi_cb_params_uspace_ptr_t *uspace_memory_sensor_mgr = NULL;
  
  if(hndl == NULL)
  {
    return QAPI_ERR_INVALID_PARAM;
  }
  
  /* Retrieve the user space information stored internally */
  qapi_data_map_handle_to_u_addr(hndl, QAPI_APP_SENSOR_MGR, (void**)&uspace_memory_sensor_mgr);
  
  /* Release the allocated resources */
  if (uspace_memory_sensor_mgr)
    tx_byte_release(uspace_memory_sensor_mgr);
  
  /* Dispatch the call to the module manager */
  ret = ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_SENSOR_MGR_RELEASE, (ULONG) hndl, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0));
  return ret;
}

#else
	
qapi_Status_t qapi_Sensor_Mgr_Handler(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e, UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l);

/** @addtogroup chapter_sensor_mgr
@{ */

/**
*    @versiontable{2.0,2.45,
*    Data\_Services 1.3.0  &  Introduced. @tblendline
*    }
*    @ingroup qapi_Sensor_Mgr_Init
*
*    Creates a new sensor manager context.
*
*    @param[out] sensor_mgr_hndl  Newly created sensor manager context.
*
*    @return #QAPI_OK on success, < 0 on failure.
*/
qapi_Status_t qapi_Sensor_Mgr_Init(qapi_Sensor_Mgr_Hndl_t *sensor_mgr_hndl);

/**
*    @versiontable{2.0,2.45,
*    Data\_Services 1.3.0	&  Introduced. @tblendline
*    }
*    @ingroup qapi_Sensor_Mgr_Set_Callback
*
*    Sets a sensor manager indication callback.
*
*    @param[in] sensor_mgr_hndl  Sensor manager handle.
*    @param[in] cbk         Callback to be registered.
*
*    @return #QAPI_OK on success, < 0 on failure.
*
*    @dependencies
*    qapi_Sensor_Mgr_Init() @newpage
*/
qapi_Status_t qapi_Sensor_Mgr_Set_Callback(qapi_Sensor_Mgr_Hndl_t sensor_mgr_hndl, qapi_Sensor_Mgr_Sensor_Ind_Callback_t cbk);

/**
*    @versiontable{2.0,2.45,
*    Data\_Services 1.14.0	&  Introduced. @tblendline
*    }
*    @ingroup qapi_Sensor_Mgr_Set_Callback_v2
*
*    Sets a sensor manager indication callback.
*
*    @param[in] sensor_mgr_hndl  Sensor manager handle.
*    @param[in] cbk              Callback to be registered.
*
*    @return #QAPI_OK on success, < 0 on failure.
*
*    @dependencies qapi_Sensor_Mgr_Init().
*/
qapi_Status_t qapi_Sensor_Mgr_Set_Callback_v2(qapi_Sensor_Mgr_Hndl_t sensor_mgr_hndl, qapi_Sensor_Mgr_Sensor_Ind_Callback_v2_t cbk);

/**
*    @versiontable{2.0,2.45,
*    Data\_Services 1.3.0	 &	Introduced. @tblendline
*    }
*    @ingroup qapi_Sensor_Mgr_Set_Sensor_Config
*
*    Sets sensor configuration.
*
*    @param[in] sensor_mgr_hndl  Sensor manager handle.
*    @param[in] cfg              Pointer to an array of Sensor Configuration request messages to be set. 
*    @param[in] cfg_len          Number of elements contained in the array of Sensor Configuration request messages.
*    @return #QAPI_OK on success, < 0 on failure.
*
*    @dependencies qapi_Sensor_Mgr_Init().
*/
qapi_Status_t qapi_Sensor_Mgr_Set_Sensor_Config(qapi_Sensor_Mgr_Hndl_t sensor_mgr_hndl, qapi_Sensor_Mgr_Sensor_Config_Req_Msg_t *cfg, uint32_t cfg_len);

/**
*    @versiontable{2.0,2.45,
*    Data\_Services 1.3.0  &  Introduced. @tblendline
*    }
*    @ingroup qapi_Sensor_Mgr_Query_Sensor_Ind
*
*    Queries sensor for any pending or missed indication.
*
*    @param[in] sensor_mgr_hndl  Sensor Mgr handle.
*    @param[out] ind_msg         Sensor indication response message received.
*
*    @return #QAPI_OK on success, < 0 on failure.
*
*    @dependencies qapi_Sensor_Mgr_Init().
*/
qapi_Status_t qapi_Sensor_Mgr_Query_Sensor_Ind(qapi_Sensor_Mgr_Hndl_t sensor_mgr_hndl, qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_t *ind_msg);

/**
*    @versiontable{2.0,2.45,
*    Data\_Services 1.14.0  &  Introduced. @tblendline
*    }
*    @ingroup qapi_Sensor_Mgr_Query_Sensor_Ind_v2
*
*    Queries sensor for any pending or missed indication.
*
*    @param[in] sensor_mgr_hndl  Sensor manager handle.
*    @param[out] ind_msg         Sensor indication response message received.
*
*    @return #QAPI_OK on success, < 0 on failure.
*
*    @dependencies qapi_Sensor_Mgr_Init().
*/
qapi_Status_t qapi_Sensor_Mgr_Query_Sensor_Ind_v2(qapi_Sensor_Mgr_Hndl_t sensor_mgr_hndl, qapi_Sensor_Mgr_Sensor_Data_Ind_Msg_v2_t *ind_msg);

/**
*    @versiontable{2.0,2.45,
*    Data\_Services 1.3.0  &  Introduced. @tblendline
*    }
*    @ingroup qapi_Sensor_Mgr_Get_Sensor_Data
*
*    Gets sensor data.
*
*    @param[in] sensor_mgr_hndl  Sensor manager handle.
*    @param[in] req              Sensor data request message.
*    @param[out] resp            Sensor data response message received.
*
*    @return #QAPI_OK on success, < 0 on failure.
*
*    @dependencies qapi_Sensor_Mgr_Init().
*/
qapi_Status_t qapi_Sensor_Mgr_Get_Sensor_Data(qapi_Sensor_Mgr_Hndl_t sensor_mgr_hndl, qapi_Sensor_Mgr_Sensor_Data_Req_Msg_t *req, qapi_Sensor_Mgr_Sensor_Data_Resp_Msg_t *resp);

/**
*    @versiontable{2.0,2.45,
*    Data\_Services 1.3.0  &  Introduced. @tblendline
*    }
*    @ingroup qapi_Sensor_Mgr_Reset_Sensor_Config
*
*    Resets sensor configuration.
*
*    @param[in] sensor_mgr_hndl  Sensor Manager handle.
*
*    @return #QAPI_OK on success, < 0 on failure.
*
*    @dependencies
*    qapi_Sensor_Mgr_Init(). @newpage
*/
qapi_Status_t qapi_Sensor_Mgr_Reset_Sensor_Config(qapi_Sensor_Mgr_Hndl_t sensor_mgr_hndl);

/**
*    @versiontable{2.0,2.45,
*    Data\_Services 1.3.0  &  Introduced. @tblendline
*    }
*    @ingroup qapi_Sensor_Mgr_Release
*
*    Releases sensor manager context.
*
*    @param[in] sensor_mgr_hndl  Sensor manager handle.
*
*    @return #QAPI_OK on success, < 0 on failure.
*
*    @dependencies qapi_Sensor_Mgr_Init().
*/
qapi_Status_t qapi_Sensor_Mgr_Release(qapi_Sensor_Mgr_Hndl_t sensor_mgr_hndl);

/** @} */

#endif

#endif
