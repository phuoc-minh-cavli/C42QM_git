/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.tx/6.0/securemsm/unifiedcrypto/shared/sha/src/sha_shared.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/26/12   yk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sha_shared.h"
#include "stdlib.h"

/**
 * @brief This functions gets the Cipher paramaters used by 
 *        SW_Hash_Init
 *
 * @param handle     [in]  Pointer to pointer to hash context
 * @param auth_alg  [in]  Hash algorithm
 
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Init(CryptoCntxHandle **handle, SW_Auth_Alg_Type auth_alg) 
{
  sw_crypto_errno_enum_type ret_val = UC_E_SUCCESS;
  struct __sechsh_ctx_s ctx;
  SW_SHA_Ctx * hashCtxt;
    
  /* Sanity check inputs */
  if (SW_AUTH_ALG_SHA1 != auth_alg && SW_AUTH_ALG_SHA256 != auth_alg) 
  {
      return UC_E_INVALID_ARG;
  }
  uc_mutex_enter(); 

  do{
  /* Allocate memory and check for errors */
  uc_malloc((void**) &(*handle), sizeof(SW_SHA_Ctx));
  if(!*handle)
  {
       ret_val = UC_E_NO_MEMORY;
       break;
  }

  uc_memset(*handle, 0, sizeof(SW_SHA_Ctx)); 

    hashCtxt = (SW_SHA_Ctx*)*handle;
  if (auth_alg == SW_AUTH_ALG_SHA1){
     sechsharm_sha1_init(&ctx);
	 hashCtxt->auth_alg = SW_AUTH_ALG_SHA1 ;}
  else if (auth_alg == SW_AUTH_ALG_SHA256){
     sechsharm_sha256_init(&ctx);
	 hashCtxt->auth_alg = SW_AUTH_ALG_SHA256;}

   uc_memcpy ((unsigned char*)hashCtxt->counter, (unsigned char*)ctx.counter, sizeof(ctx.counter));
   uc_memcpy((unsigned char*)hashCtxt->iv, (unsigned char*)ctx.iv,sizeof(ctx.iv));

  }while(0);

  if (ret_val != UC_E_SUCCESS)
  {
    if(*handle)
    {
      uc_free(*handle);
    }
    uc_mutex_exit();
  }

   return ret_val;
}


/**
 * @brief This function will hash data into the hash context
 *        structure, which must have been initialized by
 *        SW_Hash_Update().
 *
 * @param handle      [in] Pointer to Hash context
 * @param ioVecIn     [in] Input vector

 *
 * @return sw_crypto_errno_enum_type 
 *
 * @see SW_CRYPTO_Hash_Init 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Update(CryptoCntxHandle *handle,IovecListType ioVecIn)
{
   sw_crypto_errno_enum_type ret_val = UC_E_SUCCESS;
   uuint8   leftover[SW_SHA_BLOCK_SIZE];
   uuint32  leftover_size=0;
   uuint8* pData = NULL;
   uuint32 nDataLen = 0;
   struct __sechsh_ctx_s ctx;
   SW_SHA_Ctx* hashCtxt = NULL;

   do{
   /* Sanity check inputs */
     if (!handle)
   {
       ret_val = UC_E_INVALID_ARG;
       break;
   }

     hashCtxt = (SW_SHA_Ctx*)(handle);

   if ((ioVecIn.size!= 1) || (!ioVecIn.iov))
   {
       ret_val = UC_E_INVALID_ARG;
       break;
     }

     if (!ioVecIn.iov[0].pvBase)
     {
       ret_val = UC_E_INVALID_ARG;
       break;
   }
   
   pData = (uuint8*) ioVecIn.iov[0].pvBase;
   nDataLen = ioVecIn.iov[0].dwLen;

   uc_memcpy ((unsigned char*)ctx.counter, (unsigned char*)hashCtxt->counter, sizeof(ctx.counter));
   uc_memcpy((unsigned char*)ctx.iv, (unsigned char*) hashCtxt->iv, sizeof(ctx.iv));
   uc_memcpy((unsigned char *)leftover, (unsigned char *)hashCtxt->leftover, sizeof(leftover));
   leftover_size = hashCtxt->leftover_size; 

   if (hashCtxt->auth_alg == SW_AUTH_ALG_SHA1)
    sechsharm_sha1_update( &ctx, leftover, &leftover_size, pData, nDataLen );
   else if (hashCtxt->auth_alg == SW_AUTH_ALG_SHA256)
    sechsharm_sha256_update( &ctx, leftover, &leftover_size, pData, nDataLen );
     else 
     {
       ret_val = UC_E_INVALID_ARG;
       break;
     }


   uc_memcpy ((unsigned char*)hashCtxt->counter, (unsigned char*)ctx.counter, sizeof(ctx.counter));
   uc_memcpy((unsigned char*)hashCtxt->iv, (unsigned char*)ctx.iv,sizeof(ctx.iv));
   uc_memcpy ((unsigned char*)hashCtxt->leftover, (unsigned char*)leftover, sizeof(leftover));
   hashCtxt->leftover_size = leftover_size;

   }while(0);


   if (ret_val != UC_E_SUCCESS)
   {
     if(handle)
     {
       uc_free(handle);
     }
     uc_mutex_exit();
   }

   return ret_val;
}


/**
 * @brief Compute the final digest hash value.
 *
 * @param handle     [in] Pointer to Hash context
 * @param ioVecOut   [out] Output vector
 
 * @return sw_crypto_errno_enum_type 
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_Hash_Final (CryptoCntxHandle *handle,IovecListType *ioVecOut)
{
   uuint8   digest[SW_SHA256_DIGEST_SIZE];
   uuint8   leftover[SW_SHA_BLOCK_SIZE];
   uuint32  leftover_size=0;
   sw_crypto_errno_enum_type ret_val = UC_E_SUCCESS;
   struct __sechsh_ctx_s ctx;
   uuint32 hashLen = 0;
   SW_SHA_Ctx* hashCtxt = NULL;

  do{
  /* Sanity check inputs */
    if (!handle)
  {
      ret_val = UC_E_INVALID_ARG;
      break;
   }

  if ((ioVecOut->size!= 1) || (!ioVecOut->iov))
  {
      ret_val = UC_E_INVALID_ARG;
      break;
    }

    if (!ioVecOut->iov[0].pvBase)
    {
      ret_val = UC_E_INVALID_ARG;
      break;
  }

    hashCtxt = (SW_SHA_Ctx*) (handle);

  if(hashCtxt->auth_alg == SW_AUTH_ALG_SHA1)
  {
    hashLen = SW_SHA1_DIGEST_SIZE;
  }
  else if (hashCtxt->auth_alg == SW_AUTH_ALG_SHA256)
  {
    hashLen = SW_SHA256_DIGEST_SIZE;
  }
  else
  {
      ret_val = UC_E_INVALID_ARG;
      break;
  }

   uc_memcpy ((unsigned char*)ctx.counter,hashCtxt->counter, sizeof(ctx.counter));
   uc_memcpy((unsigned char*)ctx.iv, hashCtxt->iv, sizeof(ctx.iv));
   uc_memcpy ((unsigned char*)leftover, hashCtxt->leftover, sizeof(leftover));
   leftover_size = hashCtxt->leftover_size;

   if (hashCtxt->auth_alg == SW_AUTH_ALG_SHA1)
     sechsharm_sha1_final( &ctx,  leftover, &leftover_size, digest);
   else if (hashCtxt->auth_alg == SW_AUTH_ALG_SHA256)
     sechsharm_sha256_final( &ctx,  leftover, &leftover_size, digest);
     else 
     {
       ret_val = UC_E_INVALID_ARG;
       break;
     }

  //copy the hash result into ioVecOut

   uc_memcpy(ioVecOut->iov[0].pvBase, digest, hashLen);
   ioVecOut->size = 1;
   ioVecOut->iov[0].dwLen = hashLen;

  }while(0);

  if(handle)
  {
   uc_free(handle);
  }
   uc_mutex_exit();
   return ret_val;
}


sw_crypto_errno_enum_type SW_Hmac(uuint8            *key_ptr, 
                            uuint32               keylen, 
                            IovecListType    ioVecIn,
                            IovecListType    * ioVecOut,
                            SW_Auth_Alg_Type  pAlgo)
{
  CryptoCntxHandle *cntx = NULL;
  sw_crypto_errno_enum_type ret_val = UC_E_SUCCESS; 
  sw_crypto_errno_enum_type status;   
  uuint8            ipad[SW_SHA_BLOCK_SIZE];
  uuint8            opad[SW_SHA_BLOCK_SIZE];
  uuint8            key[SW_SHA_BLOCK_SIZE];
  uuint32           key_len = SW_SHA_BLOCK_SIZE;
  uuint32           hmac[SW_SHA256_DIGEST_SIZE];
  uuint32           i = 0;
  IovecListType  ioVecInTmp;
  IovecListType  ioVecOutTmp;
  IovecType      IovecInTmp;
  IovecType      IovecOutTmp;
  uuint32             hash_size = 0;

  if ( (pAlgo != SW_AUTH_ALG_SHA256) && (pAlgo != SW_AUTH_ALG_SHA1) )
  {
    return UC_E_INVALID_ARG;
  }

  if ((key_ptr == NULL) || (keylen == 0) ||
      (ioVecIn.size != 1)  || ( ioVecIn.iov[0].pvBase == NULL) || ( ioVecIn.iov[0].dwLen == 0) ||
      (ioVecOut == NULL) || ( ioVecOut->size != 1) || 
      ( ioVecOut->iov[0].pvBase == NULL))
  {
    return UC_E_INVALID_ARG;
  }

  if ( ((pAlgo == SW_AUTH_ALG_SHA256) &&
        (ioVecOut->iov[0].dwLen < SW_SHA256_DIGEST_SIZE)) ||
       ((pAlgo == SW_AUTH_ALG_SHA1) &&
        (ioVecOut->iov[0].dwLen < SW_SHA1_DIGEST_SIZE)) )
  {
    return UC_E_INVALID_ARG;
  }

  ioVecInTmp.size = 1;
  ioVecInTmp.iov = &IovecInTmp;

  ioVecOutTmp.size = 1;
  ioVecOutTmp.iov = &IovecOutTmp; 

  /* Update the hash size */
  switch(pAlgo)
  {
    case SW_AUTH_ALG_SHA256:
      hash_size = SW_SHA256_DIGEST_SIZE;
      break;
    case SW_AUTH_ALG_SHA1:
      hash_size = SW_SHA1_DIGEST_SIZE;
      break;
    default:
      /* Validation on pAlgo has already been done */
      break;
  }

  if (keylen > SW_SHA_BLOCK_SIZE)
  {
     status = SW_Hash_Init(&cntx, pAlgo);
     if( UC_E_SUCCESS != status )
     {
       return UC_E_FAILURE;
     }

    ioVecInTmp.iov[0].pvBase = key_ptr;
    ioVecInTmp.iov[0].dwLen  = keylen;

    ioVecOutTmp.iov[0].pvBase = key;
    ioVecOutTmp.iov[0].dwLen  = hash_size;

    status = SW_Hash_Update(cntx, ioVecInTmp);
    if( UC_E_SUCCESS != status )
    {
      return UC_E_FAILURE;
    }

    status = SW_Hash_Final(cntx, &ioVecOutTmp);
    if( UC_E_SUCCESS != status)
    {
      return UC_E_FAILURE;
    }

    uc_memset(key+hash_size, 0, (key_len-hash_size));

  }
  else
  {
    uc_memcpy(key, key_ptr, keylen);
    //KW fix
    if(keylen < SW_SHA_BLOCK_SIZE && (key_len-keylen) <= SW_SHA_BLOCK_SIZE)
    {
      uc_memset(key+keylen, 0, (key_len-keylen));
    }
  }

  uc_memset(ipad, 0, SW_SHA_BLOCK_SIZE);
  uc_memset(opad, 0, SW_SHA_BLOCK_SIZE);
  uc_memcpy(ipad, key, key_len);
  uc_memcpy(opad, key, key_len);

  for (i=0; i < SW_SHA_BLOCK_SIZE; i++) 
  {
    ipad[i] ^= 0x36;
    opad[i] ^= 0x5C;
  }

  /* Inner Hash */


   status = SW_Hash_Init(&cntx, pAlgo);
   if( UC_E_SUCCESS != status )
   {
     return UC_E_FAILURE;
   }

  ioVecInTmp.iov[0].pvBase = ipad;
  ioVecInTmp.iov[0].dwLen  = SW_SHA_BLOCK_SIZE;

  ioVecOutTmp.iov[0].pvBase = hmac;
  ioVecOutTmp.iov[0].dwLen  = hash_size;

  status = SW_Hash_Update (cntx, ioVecInTmp); 
  if (status != UC_E_SUCCESS)
  {
    return UC_E_FAILURE;
  }
 
  ioVecInTmp.iov[0].pvBase = ioVecIn.iov[0].pvBase;
  ioVecInTmp.iov[0].dwLen  = ioVecIn.iov[0].dwLen;
  status = SW_Hash_Update (cntx, ioVecInTmp);
  if (status != UC_E_SUCCESS)
  {
    return UC_E_FAILURE;
  }

  status = SW_Hash_Final (cntx, &ioVecOutTmp);
  if (status != UC_E_SUCCESS)
  {
    return UC_E_FAILURE;
  }

  /* Outer Hash */
  status = SW_Hash_Init (&cntx, pAlgo);
  if (status != UC_E_SUCCESS)
  {
    return UC_E_FAILURE;
  }

  ioVecInTmp.iov[0].pvBase = opad;
  ioVecInTmp.iov[0].dwLen  = SW_SHA_BLOCK_SIZE;
  status = SW_Hash_Update (cntx, ioVecInTmp);
  if (status != UC_E_SUCCESS)
  {
    return status;
  }

  ioVecInTmp.iov[0].pvBase = hmac;
  ioVecInTmp.iov[0].dwLen  = hash_size;
  status = SW_Hash_Update (cntx, ioVecInTmp);
  if (status != UC_E_SUCCESS)
  {
    return UC_E_FAILURE;
  }

  status = SW_Hash_Final (cntx, ioVecOut);
  if (status != UC_E_SUCCESS)
  {
    return UC_E_FAILURE;
  }

  return ret_val;
}



