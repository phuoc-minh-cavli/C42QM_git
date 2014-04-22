#ifndef UC_ENV_H
#define UC_ENV_H

/*===========================================================================

                             Unified Crypto 

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE
 
 $Header: //components/rel/core.tx/6.0/securemsm/unifiedcrypto/environment/inc/uc_env.h#1 $
 $DateTime: 2020/01/30 22:49:35 $
 $Author: pwbldsvc $
 
 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 10/29/12   yk      initial version
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "uc_comdef.h"

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/

// Placeholder for future usage
#define uc_mutex_enter()
#define uc_mutex_exit() 

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/



/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type uc_malloc(void** ptr, uuint32 ptrLen);

/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type uc_free(void* ptr);

/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type uc_memcpy(void* dst, void* src, uuint32 len);

/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type uc_memset(void* src, uuint32 val, uuint32 len);



/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type uc_memcmp(void* dst, void* src, uuint32 len);

/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type uc_rand(uuint8 *out, uuint32 out_len);

#endif /* UC_ENV_H */
