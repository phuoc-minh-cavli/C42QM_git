/*===========================================================================
  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.  All Rights Reserved.  
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-06-11    PG    Updated Copyright format as per guidelines
2017-11-07   KCS    Create.
===========================================================================*/


#ifndef QAPI_PMIC_STATUS_H
#define QAPI_PMIC_STATUS_H

 /**
 * @file qapi_pmic_status.h
 *
 * @Error codes define for PMIC module.
 */
#ifdef __cplusplus
extern "C" {
#endif


#include "qapi_txm_base.h"
#include "qapi_status.h"

/*==============================================================================
                              MACROS
=============================================================================*/

/** @addtogroup qapi_pmapp_vbatt
@{ */								                                      
 
/** Error macros defined for QAPI errors. */
#define ___QAPI_ERROR_PMIC(x)  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_BSP_PMIC,  x)))

/** Error macro for battery absent. */
#define QAPI_ERR_BATT_ABSENT              ___QAPI_ERROR_PMIC(1)

/** @} */ /* end_addtogroup qapi_pmapp_vbatt */

#define TXM_QAPI_PMIC_PON_PRESS_CB              (CUSTOM_CB_QAPI_PMIC_BASE)
#define TXM_QAPI_PMIC_PON_PRESS_EVENT              (TXM_QAPI_PM_EVENTS_BASE +1)

UINT qapi_Pmic_handler(unsigned long request_id, unsigned long a, unsigned long b, UINT c, UINT d, UINT e,
						UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l);

#ifdef  QAPI_TXM_MODULE

int qapi_PMIC_user_cb_dispatcher(UINT cb_id, void *app_cb,
                                     UINT cb_param1, UINT cb_param2,
                                     UINT cb_param3, UINT cb_param4,
                                     UINT cb_param5, UINT cb_param6,
                                     UINT cb_param7, UINT cb_param8);


#define qapi_Pmic_Events(Param) \
(unsigned long) _txm_module_system_call12(TXM_QAPI_PMIC_PON_PRESS_EVENT,\
(unsigned long) Param,(unsigned long) qapi_PMIC_user_cb_dispatcher, (unsigned long) 0, (unsigned long) 0,(unsigned long) 0,\
(unsigned long) 0,(unsigned long) 0,(unsigned long) 0, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0, \
(unsigned long) 0)


#elif defined QAPI_TXM_SOURCE_CODE

qapi_Status_t qapi_Pmic_Events
(
  //qapi_Pmic_Cmd_t cmd, - In case we feel it can be extended
  void * param	
);

#else   

#error "No QAPI flags defined"

#endif

#ifdef __cplusplus
} /* closing brace for extern "C" */

#endif


#endif /** QAPI_PMIC_STATUS_H*/
