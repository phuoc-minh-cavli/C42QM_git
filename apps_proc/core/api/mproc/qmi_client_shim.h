#ifndef QMI_CLIENT_SHIM_H
#define QMI_CLIENT_SHIM_H 

/**
  @file qmi_client_shim.h
  @brief  Contains internal definitions for QCCI Shim layer
  
  *** This is only for internal usage and not for qmi clients ***

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2016  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *											 EDIT HISTORY FOR FILE
 *
 *	 This section contains comments describing changes made to the
 *	 module. Notice that changes are listed in reverse chronological
 *	 order.
 *
 *	
 *
 *
 * when 				who 		what, where, why
 * ---------- 	--- 		------------------------------------------------
 * 1/17/2017    ab      Initial revision
 ======================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

#include "txm_module.h"
#include "qapi_txm_base.h"
//#include "qmi_client.h"


/*===========================================================================
                                DEFINES
===========================================================================*/

#define TXM_QCCI_NOTIFIER_INIT_CMD          TXM_QAPI_QCCI_BASE + 0
#define TXM_QCCI_CLIENT_INIT_CMD            TXM_QAPI_QCCI_BASE + 1
#define TXM_QCCI_SEND_RAW_MSG_ASYNC_CMD     TXM_QAPI_QCCI_BASE + 2
#define TXM_QCCI_SEND_MSG_ASYNC_CMD         TXM_QAPI_QCCI_BASE + 3
#define TXM_QCCI_DELETE_ASYNC_TXN_CMD       TXM_QAPI_QCCI_BASE + 4
#define TXM_QCCI_SEND_RAW_MSG_SYNC_CMD      TXM_QAPI_QCCI_BASE + 5
#define TXM_QCCI_SEND_MSG_SYNC_CMD          TXM_QAPI_QCCI_BASE + 6
#define TXM_QCCI_RELEASE_ASYNC_CMD          TXM_QAPI_QCCI_BASE + 7
#define TXM_QCCI_RELEASE_CMD                TXM_QAPI_QCCI_BASE + 8
#define TXM_QCCI_MESSAGE_ENCODE_CMD         TXM_QAPI_QCCI_BASE + 9
#define TXM_QCCI_MESSAGE_DECODE_CMD         TXM_QAPI_QCCI_BASE + 10
#define TXM_QCCI_GET_SERVICE_LIST_CMD       TXM_QAPI_QCCI_BASE + 11
#define TXM_QCCI_GET_SERVICE_INSTANCE_CMD   TXM_QAPI_QCCI_BASE + 12
#define TXM_QCCI_GET_INSTANCE_ID_CMD        TXM_QAPI_QCCI_BASE + 13
#define TXM_QCCI_REGISTER_ERROR_CB_CMD      TXM_QAPI_QCCI_BASE + 14
#define TXM_QCCI_REGISTER_NOTIFY_CB_CMD     TXM_QAPI_QCCI_BASE + 15
#define TXM_QCCI_REGISTER_LOG_CB_CMD        TXM_QAPI_QCCI_BASE + 16
#define TXM_QCCI_INIT_INSTANCE_CMD          TXM_QAPI_QCCI_BASE + 17

//New internal helper functions for SHIM
#define TXM_QCCI_USER_MODULE_INIT_CMD            TXM_QAPI_QCCI_BASE + 25
#define TXM_QCCI_USER_MODULE_DEINIT_CMD          TXM_QAPI_QCCI_BASE + 26
#define TXM_QCCI_GET_CB_INFO_CMD                 TXM_QAPI_QCCI_BASE + 27
#define TXM_QCCI_GET_BUF_CMD                     TXM_QAPI_QCCI_BASE + 28

/*===========================================================================
                                STRUCTURES
===========================================================================*/

typedef enum
{
  QMI_CCI_SIGNAL_TYPE_CB,
  QMI_CCI_NOTIFY_CB, 
  QMI_CCI_LOG_CB,
  QMI_CCI_INDICATION_CB,
  QMI_CCI_ERR_CB,
  QMI_CCI_RELEASE_CB,
  QMI_CCI_RAW_MSG_ASYNC_CB,
  QMI_CCI_STRUCT_MSG_ASYNC_CB,
  QMI_CCI_EXIT_CB_HANDLER,    //Used to indicate callback handler to get exited
  QMI_CCI_MAX_CB_TYPES,  //Should be last
}qmi_cci_shim_cb_type;


typedef struct
{
  void   *cb;
  qmi_cci_shim_cb_type cb_type;
  ULONG   a;  //Parameters
  ULONG   b;
  ULONG   c;
  ULONG   d;
  ULONG   e;
  ULONG   f;
  ULONG   g;
  ULONG   h;
  ULONG   i;
  ULONG   j;
} qmi_cci_shim_cb_info;

typedef int qmi_cci_user_module_handle;

#ifdef __cplusplus
}
#endif

#endif /* QMI_CLIENT_SHIM_H  */
