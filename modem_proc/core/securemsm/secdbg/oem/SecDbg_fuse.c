/*===========================================================================

FILE:  secdbg_fuse.c

DESCRIPTION:
  Update the fuse information for EFS-tar and MCFG authentication

EXTERNALIZED FUNCTIONS


Copyright (c) 2013-2014 by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*==========================================================================

           Include Files for Module

==========================================================================*/
#include "secboot_util.h"


#define SIGNED_IMG_ROOT_HASH \
 {0xbd, 0xaf, 0x51, 0xb5, 0x9b, 0xa2, 0x1d, 0x8a, \
  0x24, 0x37, 0x92, 0xc0, 0xe1, 0x83, 0xe8, 0x8b, \
  0xdd, 0xd3, 0x69, 0xcc, 0xca, 0x58, 0xbc, 0x79, \
  0x2a, 0x3e, 0x4c, 0x22, 0xef, 0xf3, 0x29, 0xe8, \
  0xa8, 0xc7, 0x2d, 0x44, 0x95, 0x59, 0xcd, 0x5f, \
  0x09, 0xeb, 0xfa, 0x5c, 0x7b, 0xf3, 0x98, 0xc0}
	


#define SIGNED_IMG_MSM_HW_ID    0x0000000000000000

static uint8 fs_root_hash[] = SIGNED_IMG_ROOT_HASH;

secboot_util_error_type secboot_util_update_fuse_info(secboot_util_fuse_info_type * fuse_info)
{
	if (fuse_info == NULL)
		return E_SECBOOT_UTIL_INVALID_PARAM;
	
	
        MEMCPY(fuse_info->root_of_trust,&fs_root_hash[0],SHA384_HASH_LENGTH);
	fuse_info->msm_hw_id = SIGNED_IMG_MSM_HW_ID;
	fuse_info->auth_use_serial_num = 0;
        fuse_info->serial_num = 0x0;
        fuse_info->use_root_of_trust_only = FALSE;
	fuse_info->mrc_fuse_info.is_root_sel_enabled = 0;	
	fuse_info->mrc_fuse_info.num_root_certs = 0;
	fuse_info->mrc_fuse_info.activation_list = 0;
	fuse_info->mrc_fuse_info.revocation_list = 0;
	
	return E_SECBOOT_UTIL_SUCCESS;
}