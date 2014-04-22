/**
@file CeEL_Env.c 
@brief Crypto Engine Environment Library source file 
*/

/*===========================================================================

                     Crypto Engine Environment Library 

DESCRIPTION

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2010 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/core.tx/6.0/securemsm/cryptodrivers/ce/environment/src/CeEL_Env.c#1 $
 $DateTime: 2020/01/30 22:49:35 $
 $Author: pwbldsvc $ 

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2010-05-24   bm      Initial version
============================================================================*/
#include "CeEL_Env.h"
#include "DALSys.h"

CeEL_EnvErrorType CeElmalloc_Env(void** ptr, uint32 ptrLen)
{  
  if(0 != DALSYS_Malloc(ptrLen, ptr))
  { 
    return CEEL_ENV_ERROR_FAILURE;
  }
 
  return CEEL_ENV_ERROR_SUCCESS;
}

/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
CeEL_EnvErrorType CeElfree_Env(void* ptr)
{
  DALSYS_Free (ptr);
    
  return CEEL_ENV_ERROR_SUCCESS;
}

