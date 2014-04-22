#ifndef QMI_TXM_SHIM_H
#define QMI_TXM_SHIM_H 

/**
  @file qmi_txm_shim.h
  @brief  Contains method number ranges for Mproc QMI module
  
  *** This is only for internal usage and not to be used externally ***

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

#include "qapi_txm_base.h"

/*===========================================================================
                                DEFINES
===========================================================================*/

//For QMI Client functions
#define QMI_SHIM_QCCI_BASE         TXM_QAPI_QCCI_BASE
#define QMI_SHIM_QCCI_NUM_IDS      40

//For QMI IDL functions
#define QMI_SHIM_IDL_BASE          QMI_SHIM_QCCI_BASE + QMI_SHIM_QCCI_NUM_IDS
#define QMI_SHIM_IDL_NUM_IDS       20

//This should be sum of last base + last num ids
#define QMI_SHIM_LAST_ID           QMI_SHIM_IDL_BASE + QMI_SHIM_IDL_NUM_IDS

//Check with the num ids allocated to us by kernel team
#if QMI_SHIM_LAST_ID > (TXM_QAPI_QCCI_BASE + TXM_QAPI_QCCI_NUM_IDS)
   #error "QMI TXM Ids Limit crossed"
#endif


#endif /* QMI_TXM_SHIM_H */

