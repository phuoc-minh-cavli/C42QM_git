/**
@file uc_env.c 
@brief unified crypto environment source file 
*/

/*===========================================================================

                          Unified Crypto

DESCRIPTION

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
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
10-09-2012   yk      initial version
============================================================================*/
#include "uc_image_env.h"
#include "uc_env.h"
//#include <comdef.h>

/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type uc_malloc(void** ptr, uuint32 ptrLen)
{ 

 if (ptr == NULL || ptrLen == 0 )
	 return UC_E_INVALID_ARG;

 uc_internal_malloc(ptr, ptrLen);


 return UC_E_SUCCESS; // Does not really necessary. Caller should check if ptr is NULL or not
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
sw_crypto_errno_enum_type uc_free(void* ptr)
{
  uc_internal_free(ptr);
  return UC_E_SUCCESS;  
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
sw_crypto_errno_enum_type uc_memcpy(void* dst, void* src, uuint32 len)
{

  memcpy(dst, src, len);

  return UC_E_SUCCESS;
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
sw_crypto_errno_enum_type uc_memset(void* src, uuint32 val, uuint32 len)
{
  memset(src, val, len);

  return UC_E_SUCCESS;
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
sw_crypto_errno_enum_type uc_memcmp(void* dst, void* src, uuint32 len)
{
  if(memcmp(dst, src, len))
    return UC_E_FAILURE;

  return UC_E_SUCCESS;
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
sw_crypto_errno_enum_type uc_rand(uuint8 *out, uuint32 out_len)
{
  uuint32 rv = 0;

 if (out == NULL || out_len == 0 )
	 return UC_E_INVALID_ARG;

  uc_internal_prng_getdata(rv, out, out_len);

  if (rv != out_len)
	 return UC_E_FAILURE;

  return UC_E_SUCCESS;
}


