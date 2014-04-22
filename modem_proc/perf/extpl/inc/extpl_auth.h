/*==========================================================================
 * FILE:         extpl_auth.h
 *
 * SERVICES:     EXTERNAL PAGER
 *
 * DESCRIPTION:  This file provides the definitions of authentication
 *               and validation service routines and adat types for external
 *               paged modules as well as ondemand pages.
 *
 *
=============================================================================*/

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/perf.mpss/2.1.2.0/extpl/inc/extpl_auth.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/18/17   che     Initial Version

===========================================================================*/

#ifndef __EXTPL_AUTH_H__
#define __EXTPL_AUTH_H__

#define SHA256_HASH_SIZE 32
#define FAST_HASH_SIZE 8

extern int extpl_auth_init();
extern int extpl_validate_swap_hash(uint8* hash, uint32 hash_size);
extern int extpl_validate_module_hash(uint32 module_id, uint32 is_rw, uint8* hash, uint32 hash_size);
extern int extpl_validate_ondemand_full_hash(uint32 idx, uint8* hash, uint32 hash_size);
extern int extpl_validate_ondemand_fast_hash(uint32 idx, uint64 fast_hash);

extern int extpl_create_sha256_hash(uint8* data, uint32 data_size, uint8* digest_out, uint32 out_size);
extern int extpl_create_swap_hash(uint8* data, uint32 data_size);
extern int extpl_init_fast_hash(uint8* page, uint32 data_size, uint32 idx);
extern uint64 extpl_create_fast_hash(uint8* page, uint32 data_size);
extern int extpl_has_fast_hash(uint32 idx);

extern uint8 extpl_swap_hash[SHA256_HASH_SIZE];

#endif
