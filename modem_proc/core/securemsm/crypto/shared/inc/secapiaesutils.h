#ifndef SECAPIAESUTILS_H
#define SECAPIAESUTILS_H


/*===========================================================================

                     A E S   U T I L I T Y   F U N C T I O N S
							 
FILE:  secapiaesutils.h

DESCRIPTION:
   

   References:

   RFC 3566; 
   Section B.A.9.2.4 in DVB Bluebook A100 ( IP Datacast over DVB-H:
   Service Purchase and Protection (SPP) )

   Design Notes:
   
EXTERNALIZED FUNCTIONS

   aes_xcbc_mac() 
	AES in CBC  with a set of extensions[XCBC-MAC-1];
	refer to RFC 3566

 Copyright (c) 2005-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*=========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.10/securemsm/crypto/shared/inc/secapiaesutils.h#1 $ 
  $DateTime: 2018/06/26 03:18:17 $ 
  $Author: pwbldsvc $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Removed the FEATURE_SEC wrap 
02/25/05   rv      Created file.

============================================================================*/

#include "comdef.h"
#include "rex.h"
#include "task.h"
#include "assert.h"
#include "msg.h"
#include "err.h"

#include "secerrno.h"
#include "secapi.h" 

/*==========================================================================
 FUNCTION SECAPI_AES_XCBX_MAC_PRF
  
 DESCRIPTION 
   This function implements the DVB Document A100; Section B.A.9.4
   AES-XCBC-MAC-PRF

 PARAMETERS
   auth_seed      -[I/P] AS as specified by DVB DocA100
   auth_seed_len  -[I/P] length of AS ( should be 16 bytes
   constant       -[I/P] CONSTANT as specified by DVB DocA100
   constant_len   -[I/P] length of CONSTANT
   output_ptr     -[O/P] memory for storing the expected out put
                         it should be allocated by the caller
   output_len     -[O/P] the size of memory allocated for output_ptr
                         SHOULD BE A MUTIPLE OF 16 bytes (128 bits)
   
 DEPENDENCIES 
   
 RETURN VALUE 
   Security services error code.
 
 SIDE EFFECTS
   None
===========================================================================*/
secerrno_enum_type secapi_aes_xcbc_mac_prf
(
   uint8*                        auth_seed,
   uint8                         auth_seed_len,
   uint8*                        constant,
   uint64                        constant_len,
   uint8*                        output_ptr,
   uint64*                       output_len
);

/*==========================================================================
 FUNCTION SECENC_AES_XCBX_MAC
  
 DESCRIPTION 
   This function performs AES(in CBC mode) encrypt operations over a 
   message M. The function implements AES-XCBC-MAC algorithm;
   Refer to RFC 3566 for further details

   
 DEPENDENCIES 
   
 RETURN VALUE 
   Security services error code.
 
 SIDE EFFECTS
   None
===========================================================================*/
secerrno_enum_type secenc_aes_xcbc_mac(
   uint8*                        penc_key,
   uint8*                        pmessage,
   uint64                        nmessage_len,
   uint8*                        paes_mac,
   uint64*                       paes_mac_len
);
#endif /* SECAPIAESUTILS_H */


