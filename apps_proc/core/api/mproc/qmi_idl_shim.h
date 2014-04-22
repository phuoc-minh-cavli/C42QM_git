#ifndef QMI_IDL_SHIM_H
#define QMI_IDL_SHIM_H 

/**
  @file qmi_idl_shim.h
  @brief  Contains internal definitions for QCCI IDL SHIM layer
  
  *** This is only for internal usage and not for qmi clients ***

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2017  by Qualcomm Technologies, Inc.  All Rights Reserved.
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
 * 4/3/2017     ab      Initial revision
 ======================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

#include "txm_module.h"
#include "qapi_txm_base.h"
#include "qmi_txm_shim.h"


/*===========================================================================
                                DEFINES
===========================================================================*/
//Method Defines between Kernel and User space DAM
#define QMI_SHIM_IDL_GET_SERVICE_ID                 QMI_SHIM_IDL_BASE + 0
#define QMI_SHIM_IDL_GET_IDL_MINOR_VERSION          QMI_SHIM_IDL_BASE + 1
#define QMI_SHIM_IDL_GET_IDL_VERSION                QMI_SHIM_IDL_BASE + 2
#define QMI_SHIM_IDL_GET_MAX_SERVICE_LEN            QMI_SHIM_IDL_BASE + 3
#define QMI_SHIM_IDL_GET_MAX_MESSAGE_LEN            QMI_SHIM_IDL_BASE + 4
#define QMI_SHIM_IDL_GET_MESSAGE_C_STRUCT_LEN       QMI_SHIM_IDL_BASE + 5
#define QMI_SHIM_IDL_GET_MAX_C_STRUCT_LEN           QMI_SHIM_IDL_BASE + 6
#define QMI_SHIM_IDL_MESSAGE_DECODE                 QMI_SHIM_IDL_BASE + 7
#define QMI_SHIM_IDL_MESSAGE_ENCODE                 QMI_SHIM_IDL_BASE + 8
#define QMI_SHIM_IDL_ENCODE_RESP_TLV                QMI_SHIM_IDL_BASE + 9
#define QMI_SHIM_IDL_GET_STD_RESP_TLV_LEN           QMI_SHIM_IDL_BASE + 10
#define QMI_SHIM_IDL_INHERIT_SERVICE_OBJECT         QMI_SHIM_IDL_BASE + 11
#define QMI_SHIM_IDL_GET_INHERITED_SERVICE_OBJECT   QMI_SHIM_IDL_BASE + 12


#ifdef __cplusplus
}
#endif

#endif /* QMI_IDL_SHIM_H */
