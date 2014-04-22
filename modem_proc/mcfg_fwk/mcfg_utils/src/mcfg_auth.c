/*==============================================================================

                M O D E M   C O N F I G   A U T H
 
GENERAL DESCRIPTION
  Auth library for MCFG

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_utils/src/mcfg_auth.c#1 $ 
$DateTime: 2020/02/19 23:07:19 $ 
$Author: pwbldsvc $ 
$Change: 22644646 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/

#include "comdef.h"

#include "msg.h"

#include "mcfg_common.h"
#include "mcfg_utils.h"
#include "mcfg_utils_i.h"
#include "mcfg_auth_i.h"
#include "mcfg_load.h"
#include "mcfg_osal.h"

#include "secapi.h"
#include "secboot_util.h"
#include <IxErrno.h>

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
/*---------------------------------------------------------------------------
  MCFG SW TYPE used for CSMS signing, identically to
  SECBOOT_AMSS_HASH_TABLE_SW_TYPE
---------------------------------------------------------------------------*/
#define MCFG_AMSS_HASH_TABLE_SW_TYPE  2

#define MCFG_ALGO_TYPE    SECAPI_SHA384
#define MCFG_HASH_LEN     SHA384_HASH_LENGTH

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/

/*============================================================================== 
 
                  PRIVATE FUNCTION DEFINITIONS FOR MODULE
 
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_auth_verify_hash

  DESCRIPTION
    Verfies the hash against the data segment

  DEPENDENCIES
    None

  PARAMETERS
    config [in] Pointer to signed image
    
  RETURN VALUE
    TRUE if hash matches, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_auth_verify_hash
(
  uint8 *p_img,
  uint32 sig_offset, 
  uint32 data_offset, 
  uint32 data_seg_size
)
{
  secerrno_enum_type          stat;
  secapi_handle_type          sh = NULL;
  secapi_hsh_param_data_type  hash_param;
  boolean                     error;

  secapi_algo_enum_type algo_type = MCFG_ALGO_TYPE;
  unsigned char digest_out[MCFG_HASH_LEN];
  unsigned char *digest_in;
  uint32 digest_offset;
  sec_img_mbn_header_t *auth_img_hdr;
  boolean retVal = FALSE;

  if (MCFG_INTERGER_OVERFLOWED(p_img, sig_offset))
  {
    MCFG_MSG_ERROR_2("mcfg_auth_verify_hash() invalid start %p offset %lx",
                     p_img, sig_offset);
    return FALSE;
  }

  MCFG_MSG_LOW_3("mcfg_auth_verify_hash() sig_offset 0x%4x, data_offset 0x%4x"\
                 " size %d", sig_offset, data_offset, data_seg_size);

  auth_img_hdr = (sec_img_mbn_header_t *)(p_img + sig_offset);
  
  digest_offset = sizeof(sec_img_mbn_header_t) + 
              auth_img_hdr->qc_metadata_size +
              auth_img_hdr->oem_metadata_size +
              2*MCFG_HASH_LEN; // Skip program and hash segments
              
  if (!MCFG_SIZE_OFFSET_WITHIN_LIMIT(sig_offset, digest_offset, data_offset))
  {
    MCFG_MSG_ERROR_3("Invalid sig_offset %lx digest_offset %lx data_offset %lx",
                     sig_offset, digest_offset, data_offset);
    return FALSE;
  }
  
  memset((void *)digest_out, 0, sizeof(digest_out));
  digest_in = p_img + sig_offset + digest_offset;

  stat = secapi_new(&sh, algo_type);
  if ( stat )
  {
      MSG_ERROR("HASH SHA: failed secapi handle", 0, 0, 0);
      return FALSE;
  }

  error = FALSE;

  do
  {
      hash_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
      stat = secapi_hsh_set_param( sh,
                                   algo_type,
                                   SECAPI_HSH_INPUT_MODE,
                                   &hash_param );
      if ( stat )
      {
          MSG_ERROR("HASH SHA: set param failure", 0, 0, 0);
          break;
      }

      hash_param.common.exec_platform = (data_seg_size < 0x1000) 
        ? SECAPI_EXEC_PLATFORM_ARM
        : SECAPI_EXEC_PLATFORM_ACCELERATOR_PREFERRED;
      stat = secapi_hsh_set_param( sh,
                                   algo_type,
                                   SECAPI_EXEC_PLATFORM,
                                   &hash_param );
      if ( stat )
      {
          MSG_ERROR("HASH SHA: set param failure", 0, 0, 0);
          break;
      }

      /* compute digest, and append result to seg buf */
      stat = secapi_hsh_create_digest( sh,
                                       algo_type,
                                       p_img+data_offset,
                                       data_seg_size,
                                       digest_out );
      if ( stat )
      {
          MSG_ERROR("HASH SHA: failure", 0, 0, 0);
          error = TRUE;
          break;
      }
  } while ( 0 );

  /* clean up */
  stat = secapi_delete(&sh);
  if ( stat || error )
  {
      MSG_ERROR("HASH SHA: secapi delete error", 0, 0, 0);    
      return FALSE;
  }

  if (memcmp(digest_in, digest_out, MCFG_HASH_LEN) != 0)
  {
    MCFG_MSG_ERROR_2("mcfg_auth_verify_hash() hash doesn't match!! 0x%2x 0x%2x", 
                     *digest_in, digest_out[0]);
  }
  else
  {
    MCFG_MSG_LOW("mcfg_auth_verify_hash() hash matches!!");
    retVal = TRUE;
  }

  return retVal;
} /* mcfg_auth_verify_hash */

/*===========================================================================

  FUNCTION mcfg_auth_img_verify

  DESCRIPTION
    Parses the signed image for secure header, image, signature and
    certificates.
    Verfies the signature of passed in image against passed in attestation
    certificate.
    This function should be replaced with secapi_util_auth_img_verify() once
    that API takes image_dest_ptr to compute signature/cert offsets

  DEPENDENCIES
    None

  PARAMETERS
    config [in] Pointer to signed image
    
  RETURN VALUE
    TRUE - If Image signature and certificate chain are valid. \n
    FALSE - Signature validation failed or cert chain is not valid

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_auth_img_verify
(
  uint8* signed_img_ptr,
  uint32 signed_img_len
)
{
  void *mem = NULL; 
  static secboot_util_handle_type  *secboot_handle_ptr = NULL;
  boolean                          retVal = FALSE;
  uint32 auth_enabled;
  const uint8 MEM_ALIGN = 0x7; // for 8byte memory aligning
   
  MCFG_CHECK_NULL_PTR_RET_FALSE(signed_img_ptr);

  /* Create memory with some extra buffer for 8byte aligning */
  mem = mcfg_malloc(sizeof(secboot_util_handle_type)+MEM_ALIGN);
  if(!mem)
  {
    MCFG_MSG_ERROR("Unable to allocate memory for 'mem'!");
    return retVal;
  }

  /* Get the nearby 8byte aligned block's starting address */
  secboot_handle_ptr = (secboot_util_handle_type*)(((int)((char*)mem)+MEM_ALIGN) & ~(MEM_ALIGN));
  MCFG_MSG_LOW_1("secboot_handle_ptr addr = 0x%x", secboot_handle_ptr);

  memset (secboot_handle_ptr, 0,  sizeof (secboot_util_handle_type));
  
  if(E_SUCCESS != secboot_util_hw_is_auth_enabled(
       SECBOOT_UTIL_HW_MSS_CODE_SEGMENT, &auth_enabled))
  {
    MCFG_MSG_ERROR("mcfg_auth_img_verify() failed get HW Auth_EN!!");
  }
  else if(!auth_enabled)
  {
    MCFG_MSG_HIGH("mcfg_auth_img_verify() Auth is disabled");
    retVal = TRUE;
  }
  else if(E_SECBOOT_UTIL_SUCCESS != secboot_util_auth_img_verify( secboot_handle_ptr,
                                                                  signed_img_ptr,
                                                                  signed_img_len,  
                                                                  SECBOOT_UTIL_HW_MSS_CODE_SEGMENT,
                                                                  MCFG_AMSS_HASH_TABLE_SW_TYPE,
                                                                  NULL,
                                                                  NULL))
    {
      MCFG_MSG_ERROR("mcfg_auth_img_verify() failed!!");
    }
    else
    {
      MCFG_MSG_HIGH("mcfg_auth_img_verify() is Successful!!");
      retVal = TRUE;
    }
  mcfg_free(mem);

  return retVal;
} /* mcfg_auth_img_verify() */

/*===========================================================================

  FUNCTION mcfg_auth_init

  DESCRIPTION
    Initialize mcfg auth

  DEPENDENCIES
    None

  PARAMETERS
    None
    
  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_auth_init
(
  void
)
{
  return;
} /* mcfg_auth_init() */


/*==============================================================================
                    PUBLIC FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_auth_check_config

  DESCRIPTION
    This function authenticates the config file, the caller should reject
    the config file when loading or populating the configuration into NV/EFS.

  DEPENDENCIES
    None

  PARAMETERS
      config [in] config structure including config size and buffer
                  pointer to be authenticated
    
  RETURN VALUE
    mcfg_error_e

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_auth_check_config
(
  mcfg_config_s_type *config
)
{
  mcfg_error_e_type error = MCFG_ERR_AUTHENTICATION_FAILED;
  uint32 sig_offset;
  uint32 sig_seg_size = 0;
  uint32 data_offset;
  uint32 data_seg_size = 0;

  MCFG_CHECK_NULL_PTR_RET_ERROR(config, MCFG_ERR_AUTHENTICATION_FAILED);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config->config_addr, 
                                MCFG_ERR_AUTHENTICATION_FAILED);

  if (!mcfg_get_hash_seg_info(config, &sig_offset, &sig_seg_size))
  {
    MCFG_MSG_ERROR(
       "mcfg_auth_check_config Error reading MCFG signature segment");
  }
  else if (!mcfg_get_data_seg_info(config, &data_offset, &data_seg_size))
  {
    MCFG_MSG_ERROR("mcfg_auth_check_config Error reading MCFG data segment");
  }
  else if (!mcfg_auth_verify_hash(config->config_addr, sig_offset, 
                               data_offset, data_seg_size))
  {
    MCFG_MSG_ERROR("mcfg_auth_check_config hash doesn't match");
  }
  else if (!mcfg_auth_img_verify(
              (uint8 *)((uint32)config->config_addr+sig_offset), sig_seg_size))
  {
    MCFG_MSG_ERROR("mcfg_auth_check_config signature auth failed ");
  }
  else
  {  
    error = MCFG_ERR_NONE;
  }
  
  return error;
} /* mcfg_auth_check_config() */

