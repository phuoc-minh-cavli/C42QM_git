#ifndef UC_IMAGE_ENV_H
#define UC_IMAGE_ENV_H
/*===========================================================================

                             Unified Crypto 

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) {2012, 2018} by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/ssg.tz/1.1.3/securemsm/uclib/usr/legacy/unifiedcrypto/environment/tz/inc/uc_image_env.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
 5/24/18    ss     update the tzbsp_malloc prototype
 10/10/12   yk     Initial version
 ===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "uc_comdef.h"

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/
/* Forward declare tzbsp functions */
uint32 tzbsp_prng_getdata(uint8 *out, uint32 out_len);

void* tzbsp_malloc(uint64_t size);

void tzbsp_free(void* ptr);

/* end of forward declare */

#define uc_internal_prng_getdata(returnvar, ptr, ptrLen) \
          {returnvar = tzbsp_prng_getdata(ptr, ptrLen);}

#define uc_internal_malloc(ptr, ptrLen) \
       {*ptr = tzbsp_malloc(ptrLen);}

#define uc_internal_free(ptr) \
	   {tzbsp_free (ptr);}

//sw_crypto_errno_enum_type uc_internal_malloc(void** ptr, uint32 ptrLen);

//sw_crypto_errno_enum_type uc_internal_free(void* ptr);

#endif //UC_IMAGE_ENV_H
