#ifndef QDSS_OSAL_H
#define QDSS_OSAL_H

/*=============================================================================

FILE:         qdss_osal.h

DESCRIPTION:  QDSS API for OS abstraction

================================================================================
            Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "qurt_mutex.h"

void qdss_osal_mutex_init(qurt_mutex_t * hMutex);
void qdss_osal_mutex_destroy(qurt_mutex_t * hMutex);
void qdss_osal_mutex_lock(qurt_mutex_t * hMutex);
void qdss_osal_mutex_unlock(qurt_mutex_t * hMutex);


#endif	//QDSS_OSAL_H
