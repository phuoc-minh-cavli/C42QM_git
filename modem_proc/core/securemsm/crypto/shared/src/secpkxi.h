#ifndef SECPKXI_H
#define SECPKXI_H



/*===========================================================================

                    S E C U R I T Y    S E R V I C E S

                 C R Y P T O G R A P H I C   L I B R A R Y

         I N T E R N A L   A P P L I C A T I O N   I N T E R F A C E

DESCRIPTION
  This file defines the internal API for applications using
  cryptographic functionality from Security Services.
  The cryptographic API (CAPI) comprises

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cryptographic library is initialized by Security Services
  internally and no application specific intialization is required.

 Copyright (c) 2005-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/core.mpss/3.10/securemsm/crypto/shared/src/secpkxi.h#2 $
  $DateTime: 2018/11/20 22:32:11 $
  $Author: pwbldsvc $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/17   jp      Ported the changes - Added RSA OAEP Padding Support
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC wrap.
04/21/06   jay     updated header files
03/02/05   sb      Created file.

===========================================================================*/

#include "comdef.h"
#include "rex.h"

#include "secerrno.h"


/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION SECAPII_EMSA_PSS_ENCODE()

DESCRIPTION
	Signature encoding algorithm, refer to PKCS#1 V2.1

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY

SIDE EFFECTS
	None.
===========================================================================*/
secerrno_enum_type
secpkxi_emsa_pss_encode(
	uint8* msg_ptr,
	uint32 msg_ptr_len, 
	uint8* encoded_msg_ptr,
	uint32 encoded_msg_len,
    uint32 encoded_msg_bit_len
	);


/*===========================================================================

FUNCTION SECAPII_EMSA_PSS_DECODE()

DESCRIPTION
	Signature decoding algorithm, refer to PKCS#1 V2.1

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY/E_FAILURE

SIDE EFFECTS
	None.
===========================================================================*/
secerrno_enum_type
secpkxi_emsa_pss_decode(
	uint8* msg_ptr,
	uint32 msg_ptr_len, 
	uint8* encoded_msg_ptr,
	uint32 encoded_msg_len,
    uint32 encoded_msg_bit_len
	);

/*===========================================================================

FUNCTION SECPKXI_PKCS_OAEP_PADDING_ENCODE()

DESCRIPTION
	PKCS OAEP encode padding - using SHA256

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY/E_FAILURE

SIDE EFFECTS
	None.
===========================================================================*/
secerrno_enum_type secpkxi_pkcs_oaep_padding_encode(
	uint8_t* msg,
	uint16_t msg_len, 
	uint8_t* out,
	uint16_t out_len,
        uint16_t modulus_bitlen,
        uint16_t hash_algo,
        uint8_t* label_ptr,
        uint16_t label_len
	);

/*===========================================================================

FUNCTION SECPKXI_PKCS_OAEP_PADDING_DECODE()

DESCRIPTION
	PKCS OAEP decode padding - using SHA256

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY/E_FAILURE

SIDE EFFECTS
	None.
===========================================================================*/
secerrno_enum_type secpkxi_pkcs_oaep_padding_decode(
	uint8_t* msg,
	uint16_t msg_len, 
	uint8_t* out,
	uint16_t* out_len,
        uint16_t modulus_bitlen,
        uint16_t hash_algo,
        uint8_t* label_ptr,
        uint16_t label_len
	);
#endif

