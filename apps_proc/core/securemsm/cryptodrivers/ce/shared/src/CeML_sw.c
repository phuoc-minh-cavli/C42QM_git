/**
@file CeML_sw.c 
@brief Software Crypto Engine Module source file 
       This is used during bringup, to replace the HW with SW
       interface
*/

/**********************************************************************
 * Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE
 
 $Header: //components/rel/core.tx/6.0/securemsm/cryptodrivers/ce/shared/src/CeML_sw.c#1 $
 $DateTime: 2020/01/30 22:49:35 $
 $Author: pwbldsvc $ 

when         who     what, where, why
--------     ---     --------------------------------------------------- 
2015-02-06   yk       Initial Version
=======================================================================*/

#include "comdef.h"
#include "CeML.h"
#include "CeEL.h"
#include "CeEL_Env.h"

#include "sha_shared.h"
#include "uc_env.h"
#include "uc_comdef.h"
#include <stringl/stringl.h>


/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
CeMLErrorType CeMLInit(void)
{
  return CEML_ERROR_SUCCESS;
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
CeMLErrorType CeMLDeInit(void)
{
  return CEML_ERROR_SUCCESS;
}


/**
 * @brief 
 *
 * @param
 *
 * @return 
 *
 * @see 
 *
 */                     
CeMLErrorType CeMLHashInit(CeMLCntxHandle** ceMlHandle, CeMLHashAlgoType pAlgo)
{
  CeMLErrorType ret_val = CEML_ERROR_SUCCESS;
  sw_crypto_errno_enum_type status = UC_E_SUCCESS;
  CryptoCntxHandle *cntx = NULL;
  SW_Auth_Alg_Type auth_al;

  /* Sanity check inputs */
  if (CEML_HASH_ALGO_SHA1 == pAlgo)
  {
	  auth_al = SW_AUTH_ALG_SHA1;
  }
  else if (CEML_HASH_ALGO_SHA256 == pAlgo)
  {
	  auth_al = SW_AUTH_ALG_SHA256;
  }
  else
  {
    return CEML_ERROR_INVALID_PARAM;
  }

  *ceMlHandle = NULL;

  /* Allocate memory and check for errors */
  CeElmalloc((void**) &(*ceMlHandle), sizeof(CeMLCntxHandle));

  if(!*ceMlHandle)
  {
    return CEML_ERROR_INVALID_PARAM;
  }

  (*ceMlHandle)->pClientCtxt = NULL;

  status = SW_Hash_Init(&cntx, auth_al);
 
  ((*ceMlHandle)->pClientCtxt) = cntx;

  if( UC_E_SUCCESS != status )
  {
    return CEML_ERROR_FAILURE;
  }
 
 return ret_val;

}

/**
 * @brief 
 *
 * @param
 *
 * @return 
 *
 * @see 
 *
 */                     
CeMLErrorType CeMLHashDeInit(CeMLCntxHandle** ceMlHandle)
{
  CeMLErrorType ret_val = CEML_ERROR_SUCCESS;

  if ((!ceMlHandle) || (!*ceMlHandle))
  {  
    return CEML_ERROR_INVALID_PARAM;
  }

  //CeElfree((*ceMlHandle)->pClientCtxt);
  //(*ceMlHandle)->pClientCtxt = NULL;
  CeElfree(*ceMlHandle);
  *ceMlHandle = NULL;

  return ret_val;
}

/**
 * @brief 
 *
 * @param 
 *
 * @return 
 *
 * @see  
 *
 */
CeMLErrorType CeMLHashUpdate(CeMLCntxHandle* ceMlHandle, 
                             CEMLIovecListType ioVecIn)
{
   CeMLErrorType ret_val = CEML_ERROR_SUCCESS;
   sw_crypto_errno_enum_type status = UC_E_SUCCESS;
   CryptoCntxHandle *cntx = NULL;
   IovecListType *ioVecIn_sw;

   /* Sanity check inputs */
   if (!ceMlHandle)
   {
     return CEML_ERROR_INVALID_PARAM;
   }

   if ((ioVecIn.size!= 1) || (!ioVecIn.iov))
   {
     return CEML_ERROR_INVALID_PARAM;
   }

   ioVecIn_sw = (IovecListType *)&ioVecIn;


   cntx = (CryptoCntxHandle*) (ceMlHandle->pClientCtxt);
   
   status = SW_Hash_Update(cntx, *ioVecIn_sw);

   if( UC_E_SUCCESS != status )
   {
	 return CEML_ERROR_FAILURE;
   }

   return ret_val;
}

/**
 * @brief 
 *
 * @param 
 * 
  @return 
 *
 * @see 
 *
 */
CeMLErrorType CeMLHashFinal(CeMLCntxHandle* ceMlHandle, 
                            CEMLIovecListType* ioVecOut)
{
  CeMLErrorType         ret_val = CEML_ERROR_SUCCESS;    
  CryptoCntxHandle *cntx = NULL;
  sw_crypto_errno_enum_type status = UC_E_SUCCESS;
  
  /* Sanity check inputs */
  if ((!ceMlHandle) || (!ceMlHandle->pClientCtxt))
  {
    return CEML_ERROR_INVALID_PARAM;
  }

  if ((!ioVecOut) || (ioVecOut->size!= 1) || (!ioVecOut->iov))
  {
    return CEML_ERROR_INVALID_PARAM;
  }

  cntx = (CryptoCntxHandle*) (ceMlHandle->pClientCtxt);

  status = SW_Hash_Final(cntx, (IovecListType *)ioVecOut);

  if( UC_E_SUCCESS != status)
  {
    return CEML_ERROR_FAILURE;
  }

 return ret_val;
}  

/**
 * @brief 
 *
 * @param 
 *
 * @return 
 *
 * @see
 *
 */
CeMLErrorType CeMLHashAtomic(CeMLCntxHandle*    ceMlHandle, 
                             CEMLIovecListType  ioVecIn,
                             CEMLIovecListType* ioVecOut)
{
  CeMLErrorType         ret_val = CEML_ERROR_SUCCESS;    
  CryptoCntxHandle *cntx = NULL;
  IovecListType *ioVecIn_sw;
  
  do
  {         
    /* Sanity check inputs */
    if ((!ceMlHandle) || (!ceMlHandle->pClientCtxt))
    {
      return CEML_ERROR_INVALID_PARAM;
    }

    if ((!ioVecOut) || (ioVecOut->size!= 1) || (!ioVecOut->iov))
    {
      return CEML_ERROR_INVALID_PARAM;
    }

    if ((ioVecIn.size!= 1) || (!ioVecIn.iov))
    {
      return CEML_ERROR_INVALID_PARAM;
    }

    cntx = (CryptoCntxHandle*) (ceMlHandle->pClientCtxt);

    ioVecIn_sw = (IovecListType *)&ioVecIn;

    if (UC_E_SUCCESS != SW_Hash_Update(cntx, *ioVecIn_sw))
    {
      ret_val = CEML_ERROR_FAILURE;
      break;
    }

    if (UC_E_SUCCESS != SW_Hash_Final(cntx, (IovecListType *)ioVecOut))
      ret_val = CEML_ERROR_FAILURE;
   
  } while (0);

  return ret_val;
}


