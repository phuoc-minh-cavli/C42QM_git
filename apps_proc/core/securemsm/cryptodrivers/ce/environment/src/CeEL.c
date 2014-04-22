/**
@file CeEL.c 
@brief Crypto Engine Environment Library source file 
*/

/*===========================================================================

                     Crypto Engine Environment Library 

DESCRIPTION

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2010 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

 $Header: 
 $DateTime: 2020/01/30 22:49:35 $
 $Author: pwbldsvc $ 

when         who     what, where, why
--------     ---     ---------------------------------------------------------- 
2012-03-09   yk      Modification for Crypto5
2010-05-24   bm      Initial version
============================================================================*/

#include <stringl/stringl.h>
#include "CeEL.h"
#include "CeEL_Env.h"

/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
CeELErrorType CeElmalloc(void** ptr, uint32 ptrLen)
{ 
  CeElmalloc_Env(ptr, ptrLen);

  return CEEL_ERROR_SUCCESS;
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
CeELErrorType CeElfree(void* ptr)
{
  CeElfree_Env(ptr);

  return CEEL_ERROR_SUCCESS;
}

