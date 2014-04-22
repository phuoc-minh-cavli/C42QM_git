/*!
  @file
  qapi_psm.c

  @brief
  PSM driver : To interact with underlying PSM client 
*/

/*===========================================================================
  Copyright (c) 2017  by Qualcomm Technologies, Inc.  All Rights Reserved.
 
  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 * when       who     what, where, why
 * --------   ---     ----------------------------------------------------------
 * 17/03/17   rkr      Initial PSM QAPI
 * 13/11/17   rkr      PSM Register changes for MMU Compliance
 ======================================================================*/

#include "tx_api.h"
#include "qurt_txm_qapi_fwk.h"
#include "qapi_status.h"
#include "qapi_psm.h"
#include "qapi_psm_status.h"
#include "psm_client.h"
#include "psm_common.h"
#include "utils_common.h"
#include "txm_module.h"
#include "tx_thread.h"
#include "qurt_txm_uid.h"
/*======================================================================
                          FUNCTIONS
=======================================================================*/

qapi_PSM_Status_t get_psm_qapi_status(psm_error_type_e status)
{
    qapi_PSM_Status_t retval;

    switch(status)
    {
        case PSM_ERR_NONE:
            retval = QAPI_OK;
            break;

        case PSM_ERR_FAIL:
            retval = QAPI_ERR_PSM_FAIL;
            break;

        case PSM_ERR_GENERIC_FAILURE:
            retval = QAPI_ERR_PSM_GENERIC_FAILURE;
            break;

        case PSM_ERR_APP_NOT_REGISTERED:
            retval = QAPI_ERR_PSM_APP_NOT_REGISTERED;
            break;

        case PSM_ERR_WRONG_ARGUMENTS:
            retval = QAPI_ERR_PSM_WRONG_ARGUMENTS;
            break;

        case PSM_ERR_IPC_FAILURE:
            retval = QAPI_ERR_PSM_IPC_FAILURE;
            break;
       
	   case PSM_ERR_INVALID_ACTIVE_TIME:
            retval = QAPI_ERR_PSM_INVALID_ACTIVE_TIME;
            break;
		
	   case PSM_ERR_INVALID_INTERVAL:
            retval = QAPI_ERR_PSM_INVALID_INTERVAL;
            break;

        default:
            retval = QAPI_OK;
            break;
    }

    return retval;
}

boolean psm_is_mmu_support_enabled() {
#ifdef FEATURE_MMU_ENABLED
    return TRUE;
#else
    return FALSE;
#endif
}

boolean psm_is_user_space_module() {
#ifdef FEATURE_PSM_USE_DAM_CONTAINMENT_CHECK
    TX_THREAD *curr_thread = NULL;

    TX_THREAD_GET_CURRENT(curr_thread);
    return (curr_thread->tx_thread_module_user_mode) ? TRUE : FALSE;
#else
    if (psm_is_mmu_support_enabled()) {
        return TRUE;
    }
    return FALSE;
#endif

}


qapi_Status_t qapi_PSM_Handler(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e, UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l)
{
    qapi_PSM_Status_t   qapi_retval = QAPI_OK;
    psm_error_type_e    retval = PSM_ERR_NONE;
    psm_client_cb_type  psm_client_cb;
    int                 rc = SUCCESS;
#if defined(FEATURE_PSM_USE_DAM_CONTAINMENT_CHECK) || defined(FEATURE_MMU_ENABLED)
    void               *user_cb = NULL;
    void               *manager_cb = NULL;
#endif

    PSM_LOG_FUNC_ENTRY("qapi_PSM_Handler: function entry");

    switch(id)
    {
        case TXM_QAPI_PSM_CLIENT_REGISTER:

            if (psm_is_user_space_module()) {
                user_cb = (void *) b;
                /* configure user callback and get mapped callback
                to be used in kernel space */
                rc = qurt_qapi_callback_configure(
                       TXM_QAPI_PSM_CLIENT_CB_FUNC,    // Callback ID
                       (void*) 0xDEADCAFE,             // Passing garbage. Not used.
                       0,                              // id (unused)
                       (void *) user_cb,               // user callback
                       (void *) d,                     // PSM Userspace dispatcher
                       (void **)&manager_cb,           // Mapped callback to be used in kernel space
                       NULL);
                if(FAIL == rc || NULL == manager_cb || NULL == c ||
                    qurt_check_if_kernel_address((qurt_addr_t *) a) ||
                    qurt_check_if_kernel_address((qurt_addr_t *) c) ||
                    (FALSE == qurt_txm_check_if_valid_module_data_addr((void*)c)))
                {
                    PSM_LOG_ERROR("qapi_PSM_Handler: Invalid memory passed in Qapi call.");
                    retval = PSM_ERR_FAIL;
                    break;
                }
                psm_client_cb = (psm_client_cb_type) manager_cb;
            } else {
                psm_client_cb = (psm_client_cb_type) b;
            }
            retval = psm_client_register_with_msg((int *) a,
                                (psm_client_cb_type) psm_client_cb,
                                (psm_status_msg_type *) c);
          break;

        case TXM_QAPI_PSM_CLIENT_UNREGISTER:
          retval = psm_client_validate_module((int) a);
          if (retval == PSM_ERR_NONE) {
            retval = psm_client_unregister((int) a);
          }
          break;

        case TXM_QAPI_PSM_CLIENT_ENTER_PSM:
          retval = psm_client_validate_module((int) a);
          if (retval == PSM_ERR_NONE) {
            retval = psm_client_enter_power_save((int) a, (psm_info_type *) b);
          }
          break;

        case TXM_QAPI_PSM_CLIENT_ENTER_BACKOFF:
          retval = psm_client_validate_module((int) a);
          if (retval == PSM_ERR_NONE) {
            retval = psm_client_enter_backoff((int) a);
          }
          break;

        case TXM_QAPI_PSM_CLIENT_CANCEL_PSM:
          retval = psm_client_validate_module((int) a);
          if (retval == PSM_ERR_NONE) {
            retval = psm_cancel_power_save((int) a);
          }
          break;

        case TXM_QAPI_PSM_CLIENT_LOAD_MODEM:
          retval = psm_client_validate_module((int) a);
          if (retval == PSM_ERR_NONE) {
            retval = psm_client_load_modem((int) a);
          }
          break;

        case TXM_QAPI_PSM_CLIENT_HC_ACK:
          retval = psm_client_validate_module((int) a);
          if (retval == PSM_ERR_NONE) {
            retval = psm_client_health_check_ack((int) a);
          }
          break;
		  
		  case TXM_QAPI_PSM_CLIENT_OPTML_INTRVL:
           if(qurt_check_if_kernel_address((qurt_addr_t *) c))
            {
                PSM_LOG_INFO("qapi_PSM_Handler: Invalid memory passed for optimal interval.");
                retval = PSM_ERR_FAIL;
                break;
            }
          retval = psm_client_validate_module((int) a);
          if (retval == PSM_ERR_NONE) {
            retval = psm_client_optimal_interval((int) a,(psm_time_interval_group_type *) b,
                                                     (psm_time_interval_type *) c, TRUE);
          }
          break;

        case TXM_QAPI_PSM_CLIENT_ENTER_PSM_WITH_RANGE:
          retval = psm_client_validate_module((int) a);
          if (retval == PSM_ERR_NONE) {
            retval = psm_client_enter_psm_with_range((int) a,(psm_info_interval_type *) b);
          }
          break;
        default:
            break;

    }

    qapi_retval = get_psm_qapi_status(retval);

    PSM_LOG_FUNC_RETURN_WITH_RET(qapi_retval);
    return qapi_retval;
}

/*=========================================================================

    QAPI PSM Wrappers over PSM Client Library
  
===========================================================================*/

qapi_Status_t qapi_PSM_Client_Register
(
    int32_t *client_id,
    psm_client_cb_type cb_func,
    psm_status_msg_type* cb_msg
)
{
    return get_psm_qapi_status(psm_client_register_with_msg(client_id,cb_func,cb_msg));
}
qapi_Status_t qapi_PSM_Client_Unregister(int32_t client_id)
{
    return get_psm_qapi_status(psm_client_unregister(client_id));
}
qapi_Status_t qapi_PSM_Client_Enter_Psm(int32_t client_id, psm_info_type *psm_info)
{
    return get_psm_qapi_status(psm_client_enter_power_save(client_id,psm_info));
}
qapi_Status_t qapi_PSM_Client_Enter_Backoff(int32_t client_id)
{
    return get_psm_qapi_status(psm_client_enter_backoff(client_id));
}
qapi_Status_t qapi_PSM_Client_Cancel_Psm(int32_t client_id)
{
    return get_psm_qapi_status(psm_cancel_power_save(client_id));
}
qapi_Status_t qapi_PSM_Client_Load_Modem(int32_t client_id)
{
    return get_psm_qapi_status(psm_client_load_modem(client_id));
}
qapi_Status_t qapi_PSM_Client_Hc_Ack(int32_t client_id)
{
    return get_psm_qapi_status(psm_client_health_check_ack(client_id));
}
qapi_Status_t qapi_PSM_Client_Get_Optimal_Interval
(
    int32_t client_id,
    psm_time_interval_group_type *interval_group,
    psm_time_interval_type *optimal_interval
)
{
    return get_psm_qapi_status(
        psm_client_optimal_interval(client_id,interval_group,optimal_interval, TRUE));
}
qapi_Status_t qapi_PSM_Client_Enter_Psm_With_Range
(
    int32_t client_id,
    psm_info_interval_type *psm_info
)
{
    return get_psm_qapi_status(psm_client_enter_psm_with_range(client_id,psm_info));
}
