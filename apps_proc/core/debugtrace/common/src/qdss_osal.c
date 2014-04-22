/*=============================================================================

FILE:         qdss_osal.c

DESCRIPTION:  QDSS API for os abstraction 

================================================================================
            Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "qdss_osal.h"

/*------------------------------------------------
 * Function Declarations and Documentation
-------------------------------------------------*/

/*------------------------------------------------
*Function Name:
*	qdss_osal_mutex_init
*Description:
*	QDSS wrapper function for OS mutex Initialisation
*
*Args: qurt_mutex_t *
-------------------------------------------------*/
void qdss_osal_mutex_init(qurt_mutex_t * hMutex)
{	
	if(hMutex)
	{
		qurt_mutex_init(hMutex);
	}
}

/*------------------------------------------------
*Function Name:
*	qdss_osal_mutex_destroy
*Description:
*	Function to destroy mutex if not in use.
*
*Args: qurt_mutex_t *
-------------------------------------------------*/
void qdss_osal_mutex_destroy(qurt_mutex_t * hMutex)
{
	if(hMutex)
	{	
		qurt_mutex_destroy(hMutex);	
	}		
}

/*------------------------------------------------
*Function Name:
*	qdss_osal_mutex_lock
*Description:
*	Function to acquire mutex Lock
*
*Args: qurt_mutex_t *
-------------------------------------------------*/
void qdss_osal_mutex_lock(qurt_mutex_t * hMutex)
{	
	if(hMutex)
	{
		qurt_mutex_lock(hMutex);
	}
}

/*------------------------------------------------
*Function Name:
*	qdss_osal_mutex_lock
*Description:
*	Function to unlock mutex
*
*Args: qurt_mutex_t *
-------------------------------------------------*/
void qdss_osal_mutex_unlock(qurt_mutex_t * hMutex)
{	
	if(hMutex)
	{
		qurt_mutex_unlock(hMutex);
	}
}











