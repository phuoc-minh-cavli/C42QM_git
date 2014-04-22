/*==========================================================================
 * FILE:         extpl.h
 *
 * SERVICES:     EXTERNAL PAGER
 *
 * DESCRIPTION:  This file defines external APIs for external pager
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

$Header: //components/rel/perf.mpss/2.1.2.0/extpl/api/extpl.h#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/18/17   che     Initial Version

===========================================================================*/

#ifndef __EXTPL_H__
#define __EXTPL_H__

#include "idyn.h"

typedef enum {
    EXTPL_MODULE_NB1,
    EXTPL_MODULE_M1,
    EXTPL_MODULE_GERAN,
    EXTPL_MODULE_GPS,

#ifdef FEATURE_DYNAMIC_LOADING_FOO
    EXTPL_MODULE_FOO,
#endif

    EXTPL_MODULE_MAX,
} extpl_module_id_t;

typedef enum {
#ifdef FEATURE_DYNAMIC_LOADING_NB1
    EXTPL_HASH_NB1,
#endif

#ifndef FEATURE_CATM1_DISABLE
    EXTPL_HASH_M1,
#endif
    
#ifdef FEATURE_DYNAMIC_LOADING_GERAN
    EXTPL_HASH_GERAN,
#endif
    
#ifdef FEATURE_DYNAMIC_LOADING_GNSS
    EXTPL_HASH_GPS,
#endif
    
    EXTPL_HASH_MAX,
} extpl_hash_id_t;

typedef enum {
    EXTPL_MODULE_CLOSED,
    EXTPL_MODULE_DEACTIVATED,
    EXTPL_MODULE_OPENED,
} extpl_module_state_t;

typedef enum {
    EXTPL_STATUS_OK,
    EXTPL_STATUS_INVALID_PARAM,
    EXTPL_STATUS_INVALID_STATE,
    EXTPL_STATUS_SWAPPOOL_UNAVAILABLE,
    EXTPL_STATUS_MODULE_ALREADY_LOADED,
    EXTPL_STATUS_MODULE_NOT_DEACTIVATED,
    EXTPL_STATUS_AUTH_FAIL,
    EXTPL_STATUS_FAIL,
} extpl_status_t;


/*extpl_swappool.c needs the following calls*/
extern int extpl_load_module(uint32 module, uint32 start_va, uint32 end_va, uint32 partition, uint32 dst_va);
extern int extpl_save_module(uint32 start_va, uint32 end_va, uint32 src_va);

extern extpl_status_t extpl_open_module(extpl_module_id_t module_id);
extern extpl_status_t extpl_close_module(extpl_module_id_t module_id);
extern extpl_status_t extpl_activate_module(extpl_module_id_t module_id);
extern extpl_status_t extpl_deactivate_module(extpl_module_id_t module_id);

#define DMM_API     __attribute__   (( section (".text_dmm_extpl_code") ))
#define DMM_CONST   __attribute__   (( section (".rodata_dmm_extpl_rodata") ))
#define DMM_DATA    __attribute__   (( section (".data_dmm_extpl_data") ))
#define DMM_ZIDATA  __attribute__   (( section (".bss_dmm_extpl_bss") ))

#endif

