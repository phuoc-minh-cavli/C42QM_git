#ifndef SHA_SHARED_H
#define SHA_SHARED_H
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

  $Header: //components/rel/core.tx/6.0/securemsm/unifiedcrypto/shared/sha/inc/sha_shared.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/10   yk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uc_comdef.h"
#include "uc_env.h"
#include "sha_core.h"

/**
 * SHA-2 Context definition
 */


typedef enum
{
  SW_AUTH_ALG_SHA1               = 0x1,
  SW_AUTH_ALG_SHA256
} SW_Auth_Alg_Type;

typedef struct 
{
	uuint32  counter[2];
	uuint32  iv[16];
	uuint32  auth_iv[8]; 
	uuint8   leftover[SW_SHA_BLOCK_SIZE];
	uuint32  leftover_size;
	SW_Auth_Alg_Type auth_alg;
} SW_SHA_Ctx;

typedef void CryptoCntxHandle;


#ifndef IOVECDEF
typedef struct {
  void                              *pvBase; 
  uuint32                            dwLen;  
} IovecType;

typedef  struct {
  IovecType                     *iov;                 
  uuint32                            size;                 
} IovecListType;
#define IOVECDEF
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Init(CryptoCntxHandle **handle, SW_Auth_Alg_Type auth_alg); 

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Update(CryptoCntxHandle *handle,IovecListType ioVecIn);

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Final (CryptoCntxHandle *handle,IovecListType *ioVecOut);

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hmac (uuint8 * key_ptr, uuint32 keylen, IovecListType ioVecIn, IovecListType * ioVecOut, SW_Auth_Alg_Type  pAlgo);

#endif /* AES_SHARED */
