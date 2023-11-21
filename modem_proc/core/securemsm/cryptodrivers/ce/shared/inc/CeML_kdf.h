#ifndef _CeML_kdf
#define _CeML_kdf

/*===========================================================================

                    Crypto Engine Module API

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

 $Header: //components/rel/core.mpss/3.10/securemsm/cryptodrivers/ce/shared/inc/CeML_kdf.h#1 $
 $DateTime: 2018/06/26 03:18:17 $
 $Author: pwbldsvc $ 

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
03/13/13     nk     modified doxygen comments of kdf implementation
===========================================================================*/

#include "comdef.h"
#include "stddef.h"
#include "CeML.h"

/**
  Derives a Key using known label, context strings and using Primary HW key or a user provided Key.

  This is an implementation of the key derivation algorithm based on
  AES_CMAC from FIPS Special Publication 800-108.

  @param[in]  input_key        Pointer to the input Key.
                               This should be NULL if Primary HW Key are to be used for Ciphering.
			       
  @param[in]  input_key_len    Length of the 'input_key' in bytes. It should be either 16 or 32. Otherwise, we use 32 as default.

  @param[in]  label            Pointer to a string constant.
                               Contention for label=> Product:usage (e.g., ��remoteFS:encryption��) *No NULLs

  @param[in]  label_len        Length of the string constant 'label'.

  @param[in]  context          Pointer to a string constant.
                               Convention for context => 16 bytes Nonce (e.g., "4902384903414267")

  @param[in]  context_len      Length of the string constant 'context'.

  @param[out]  output_key      Pointer to the generated Key.
                               The Memory for this should be provided by the caller.

  @param[in]   output_key_len  Length of the Key to be generated in bytes.
                               This is size of the buffer that the 'output_key'
                               points to. output_key_len is supported in 0< output_key_len <= (2**32-1)/8
       
  @return
  CEML_ERROR_SUCCESS - Successful completion.
  CEML_ERROR_FAILURE - Any other failure.

  @dependencies
  The memory for the 'output_key' should be allocated by the caller
  according to the 'output_key_len'.
*/
CeMLErrorType CeML_kdf
(
  const void  *input_key,
  size_t       input_key_len,
  const void  *label,
  size_t       label_len,
  const void  *context,
  size_t       context_len,
  void        *output_key,
  size_t       output_key_len
);

#endif
