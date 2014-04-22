#ifndef _CSD_HANDLE_H_
#define _CSD_HANDLE_H_
/*! \file csd_handle.h
    \brief CSD module handle management

    Copyright (C) 2010 - 2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Proprietary and Confidential.
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/common/inc/csd_handle.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/10   zj      initial release

============================================================================*/
#include "mmdefs.h"
#include "csd.h"


uint32_t csd_handle_malloc(uint32_t type, void* obj);
uint32_t csd_handle_get_type(uint32_t handle);
void* csd_handle_get_obj(uint32_t handle);
void csd_handle_free(uint32_t handle);
bool_t csd_handle_is_ssr(uint32_t handle);
bool_t csd_handle_is_type_ssr(enum csd_open_code code);
int32_t csd_handle_set_type_ssr(enum csd_open_code code, bool_t is_ssr);
int32_t csd_handle_set_ssr(uint32_t handle, bool_t bssr);
bool_t csd_handle_is_obj_ssr(void *obj);
bool_t csd_ioctl_allow_status_in_ssr(uint32_t cmd);


#endif /* _CSD_HANDLE_H_ */

