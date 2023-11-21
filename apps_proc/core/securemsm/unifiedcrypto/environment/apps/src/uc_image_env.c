/**
@file CeEL_Env.c 
@brief Crypto Engine Environment Library source file 
*/

/*===========================================================================

                     Crypto Engine Environment Library 

DESCRIPTION

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/core.tx/6.0/securemsm/unifiedcrypto/environment/apps/src/uc_image_env.c#1 $
 $DateTime: 2020/01/30 22:49:35 $
 $Author: pwbldsvc $ 

when         who     what, where, why
--------     ---     ----------------------------------------------------------
10-05-12     yk      Initial version
============================================================================*/

#include "uc_image_env.h"


uuint8 CeMLCntxHandleBuff[5]; 

typedef struct {
	uuint8 IovecTypeBuff[15]; 
	boolean_t used;
} IovecType;
IovecType IovecTypeArray[6];

uuint8 HashCipherAlgoCntxTypeBuff[320]; 

sw_crypto_errno_enum_type uc_internal_malloc(void** ptr, uuint32 ptrLen)
{  
  uuint32 i = 0;
  
  if(ptrLen<5) {
        *ptr = CeMLCntxHandleBuff;

  }
  else if(ptrLen >= 5  && ptrLen <15) {
      for(i=0;i<6;i++){		
          if (IovecTypeArray[i].used  == 1) continue;
          else{
                 *ptr = IovecTypeArray[i].IovecTypeBuff;
  		   IovecTypeArray[i].used = 1;
                  break;
           }
      }
  }
  else if (ptrLen >= 15 && ptrLen <320) {
        *ptr = HashCipherAlgoCntxTypeBuff;
  }
  else {
  	return UC_E_FAILURE;
  }
  	
  if (i >= 5) return UC_E_FAILURE;
  
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
sw_crypto_errno_enum_type uc_internal_free(void* ptr)
{
uuint32 j;


  for (j=0;j<6;j++)
  {
    if(ptr == IovecTypeArray[j].IovecTypeBuff)
    {
        IovecTypeArray[j].used = 0;
        break;
    }
    else
    {
        continue;
    }
  }
  return UC_E_SUCCESS;
}



