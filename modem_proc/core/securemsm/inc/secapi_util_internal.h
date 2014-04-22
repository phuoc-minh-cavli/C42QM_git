/**
   @file secapi_util.h
**/
/*===========================================================================
Copyright (c) {2018} Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.10/securemsm/inc/secapi_util_internal.h#1 $
  $DateTime: 2019/03/17 23:07:12 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
13-02-18  sahaj    Initial version(FR#42572)
===========================================================================*/

/**
  Creates a SHA-256 HMAC hash using the primary hardware key. 

  @param[in] msg_ptr          Pointer to the input message to be hashed.
  @param[in] msg_len          Length of the input message in bytes.
  @param[out] msg_digest_ptr  Pointer to the output HMAC buffer (memory 
                              provided by the caller). This buffer must be
                              32bytes long
                              (SECAPI_HSH_SHA_DIGEST_SIZE).
 @param[out] msg_digest_len   20 (SHA-1) 0r 32 bytes (SHA-256)

  @return 
  IxErrNoType.

  @dependencies
  None.
*/
IxErrnoType secapi_util_create_hmac2_with_hw_key 
(
  uint8*   msg_ptr,
  uint32   msg_len,
  uint8*   msg_digest_ptr,
  uint32   msg_digest_len
);