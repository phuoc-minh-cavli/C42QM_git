/*===========================================================================

                    S E C U R I T Y    S E R V I C E S

                 C R Y P T O G R A P H I C   L I B R A R Y

         E X T E R N A L   A P P L I C A T I O N   I N T E R F A C E

DESCRIPTION
  This file defines the external API for applications using
  cryptographic functionality from Security Services.
  Please see secapi.h for details.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cryptographic library is initialized by Security Services
  internally and no application specific intialization is required.

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.tx/6.0/securemsm/src/secapi.c#1 $ 
  $DateTime: 2020/01/30 22:49:35 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/13   nk      Intial revision. 

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "msg.h"
#include "err.h"
#include "PrngML.h"
#include "IxErrno.h"
#include "secapi.h"

/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

/* ========================================================
FUNCTION SECAPI_GET_RANDOM()

DESCRIPTION
  This function fetches random data pseudo random number hardware generator.
 
  The required randomness is specified as follows (increasing entropy):
    - SECAPI_PSEUDO_RANDOM:        same as SECAPI_SECURE_RANDOM
	- SECAPI_PKCS_RANDOM:          same as SECAPI_SECURE_RANDOM
	- SECAPI_SECURE_DIRECT_RANDOM: same as SECAPI_SECURE_RANDOM
    - SECAPI_TIME_CRITICAL_RANDOM: same as SECAPI_SECURE_RANDOM
    - SECAPI_SECURE_RANDOM:        Random data enhanced for maximum entropy
                                   (even distributions of 0 and 1 bits),
                                   suitable for commercial and cryptographic 
                                   applications
 
 
DEPENDENCIES
  Pseudo random data :  None
  Random data        :  CDMA search is running long enough to collect data
  Secure random data :  Same as 'Random data' plus access to hash
                        algorithms (runtime on the application DSP).

PARAMETERS
  rnd_data_type - Required randomness of random data
  rnd_data_ptr  - Pointer to array to fill random data into
  rnd_data_len  - Length of data requested (in bytes)

RETURN VALUE
  Status of request:
    E_SUCCESS if successful
    E_AGAIN   if not enough random data has been collected since last call

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secapi_get_random
(
  secapi_rnd_enum_type  rnd_data_type,
  uint8*                rnd_data_ptr,
  uint16                rnd_data_len
)
{

  PrngML_Result_Type prng_errno;

  switch( rnd_data_type )
  {
    case SECAPI_PSEUDO_RANDOM:
    case SECAPI_SECURE_RANDOM:
    case SECAPI_SECURE_DIRECT_RANDOM:
    case SECAPI_TIME_CRITICAL_RANDOM:
    case SECAPI_PKCS_RANDOM:
	  {
		prng_errno =  PrngML_getdata(rnd_data_ptr, rnd_data_len);
		if(prng_errno == PRNGML_ERROR_NONE )
		{ 
		  return E_SUCCESS;
		}
		else if(prng_errno == PRNGML_ERROR_NONE)
		{
		  return E_INVALID_ARG;
		}
		else
		{
		   return E_FAILURE;
		}
        }

    default:
	ERR( "CAPI: Invalid rnd type %d", rnd_data_type, 0, 0 );
        return E_INVALID_ARG;
  }

} /* secapi_get_random */


/* ========================================================
FUNCTION SECAPI_GET_RANDOM_LITE()

DESCRIPTION
  This function fetches random data pseudo random number hardware generator.
  This is same as above except that this is not mutex protected and has no dal layer dependencies..
 
  The required randomness is specified as follows (increasing entropy):
    - SECAPI_PSEUDO_RANDOM:        same as SECAPI_SECURE_RANDOM
	- SECAPI_PKCS_RANDOM:          same as SECAPI_SECURE_RANDOM
	- SECAPI_SECURE_DIRECT_RANDOM: same as SECAPI_SECURE_RANDOM
    - SECAPI_TIME_CRITICAL_RANDOM: same as SECAPI_SECURE_RANDOM
    - SECAPI_SECURE_RANDOM:        Random data enhanced for maximum entropy
                                   (even distributions of 0 and 1 bits),
                                   suitable for commercial and cryptographic 
                                   applications
 
 
DEPENDENCIES
  Pseudo random data :  None
  Random data        :  CDMA search is running long enough to collect data
  Secure random data :  Same as 'Random data' plus access to hash
                        algorithms (runtime on the application DSP).

PARAMETERS
  rnd_data_type - Required randomness of random data
  rnd_data_ptr  - Pointer to array to fill random data into
  rnd_data_len  - Length of data requested (in bytes)

RETURN VALUE
  Status of request:
    E_SUCCESS if successful
    E_AGAIN   if not enough random data has been collected since last call

SIDE EFFECTS
  None.
===========================================================================*/


secerrno_enum_type secapi_get_random_lite
(
  secapi_rnd_enum_type  rnd_data_type,
  uint8*                rnd_data_ptr,
  uint16                rnd_data_len
)
{

  PrngML_Result_Type prng_errno;

  switch( rnd_data_type )
  {
    case SECAPI_PSEUDO_RANDOM:
    case SECAPI_SECURE_RANDOM:
    case SECAPI_SECURE_DIRECT_RANDOM:
    case SECAPI_TIME_CRITICAL_RANDOM:
    case SECAPI_PKCS_RANDOM:
	  {
		prng_errno =  PrngML_getdata_lite(rnd_data_ptr, rnd_data_len);
		if(prng_errno == PRNGML_ERROR_NONE )
		{ 
		  return E_SUCCESS;
		}
		else if(prng_errno == PRNGML_ERROR_NONE)
		{
		  return E_INVALID_ARG;
		}
		else
		{
		   return E_FAILURE;
		}
         }
     
   default:
      ERR( "CAPI: Invalid rnd type %d", rnd_data_type, 0, 0 );
      return E_INVALID_ARG;
  }

} /* secapi_get_random */

/*===============================================================*/


	
